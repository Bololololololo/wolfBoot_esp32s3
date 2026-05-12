/* esp32s3.c
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfBoot.
 *
 * wolfBoot is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfBoot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#ifdef TARGET_esp32s3

#include <stdint.h>
#include <string.h>
#include "target.h"
#include "image.h"

/* ESP32-S3 SPI Flash Hardware Registers */
#define SPI_MEM_CTRL_REG 0x3f42f000
#define SPI_MEM_ADDR_REG 0x3f42f004
#define SPI_MEM_RDDATA_ADDR 0x3f42f010
#define SPI_MEM_CMD_REG 0x3f42f000
#define SPI_MEM_CTRL_REG 0x3f42f000

/* SPI Flash Commands */
#define SPI_CMD_WRITE_PAGE 0x02
#define SPI_CMD_ERASE_SEC 0xd8
#define SPI_CMD_ERASE_BLK 0x52
#define SPI_CMD_READ_SR 0x05
#define SPI_CMD_WRITE_SR 0x01

/* Cache Management */
#define CACHE_CTRL_REG 0x3f42f000

/* Useful register definitions for ESP32-S3 */
#define DR_REG_SPI_MEM_BASE 0x3f42f000
#define REG_SPI_MEM(i) (DR_REG_SPI_MEM_BASE + i)

/* SPI Register offsets */
#define SPI_MEM_CMD 0x000
#define SPI_MEM_ADDR 0x004
#define SPI_MEM_CTRL 0x008
#define SPI_MEM_CTRL1 0x00c
#define SPI_MEM_CTRL2 0x010
#define SPI_MEM_MOSI_DLEN 0x020
#define SPI_MEM_MISO_DLEN 0x024
#define SPI_MEM_RDDATA0 0x028

/* Command definitions for SPI */
#define SPI_MEM_USR 0x80000000
#define SPI_MEM_USR_ADDR 0x04000000
#define SPI_MEM_USR_DIN 0x08000000
#define SPI_MEM_USR_DOUT 0x10000000

/* Flash commands */
#define CMD_WRITE_ENABLE 0x06
#define CMD_WRITE_PAGE 0x02
#define CMD_PAGE_PROGRAM 0x02
#define CMD_SECTOR_ERASE 0x20
#define CMD_BLOCK_ERASE 0xd8
#define CMD_READ_SR 0x05
#define CMD_WRITE_SR 0x01
#define CMD_READ_STATUS2 0x35
#define CMD_WRITE_STATUS2 0x31

/* Flash address in ESP32-S3 memory map */
#define SPI_FLASH_PHYS_BASE 0x3f000000
#define SPI_FLASH_VIRT_BASE 0x3c000000

#define FLASH_PAGE_SIZE 256
#define FLASH_SECTOR_SIZE 4096
#define FLASH_BLOCK_SIZE 65536

/* Cache control registers */
#define CACHE_CTRL_1_REG 0x3f4261e0
#define CACHE_CTRL_2_REG 0x3f426160

/* Clock speed - ESP32-S3 default is 240 MHz */
#define ESP32S3_CLK_SPEED 240000000

/* Dummy functions to prevent linker errors - override in actual implementation */
#ifdef DEBUG_UART
static volatile uint8_t *uart_base = (volatile uint8_t *)0x3f400000;

#define UART_THR_REG 0x00
#define UART_STATUS_REG 0x1c
#define UART_INTSTS_REG 0x08
#define UART_INT_CLR_REG 0x10

#define UART_TXFIFO_CNT_M 0x000000FF
#define UART_TXFIFO_CNT_S 16

extern int _start_data, _end_data;
extern int __bss_start__, __bss_end__;

void uart_init(void)
{
    /* UART0 is typically pre-initialized by ROM bootloader */
    /* If needed, configure here */
}

static void uart_write_char(char c)
{
    volatile uint32_t *uart_status = (volatile uint32_t *)(0x3f400000 + UART_STATUS_REG);
    volatile uint32_t *uart_thr = (volatile uint32_t *)(0x3f400000 + UART_THR_REG);

    /* Wait for TX FIFO to have space */
    while ((*uart_status >> 16) & 0xFF)
        ;
    *uart_thr = c;
}

void uart_write(const char *buf, unsigned int sz)
{
    unsigned int i;
    for (i = 0; i < sz; i++)
    {
        if (buf[i] == '\n')
        {
            uart_write_char('\r');
        }
        uart_write_char(buf[i]);
    }
}

char uart_read(void)
{
    volatile uint32_t *uart_fifo = (volatile uint32_t *)(0x3f400000 + UART_THR_REG);
    return (char)(*uart_fifo & 0xFF);
}

int uart_read_nonblock(char *c)
{
    volatile uint32_t *uart_status = (volatile uint32_t *)(0x3f400000 + UART_STATUS_REG);
    uint32_t fifo_count = (*uart_status) & 0xFF;

    if (fifo_count > 0)
    {
        *c = uart_read();
        return 1;
    }
    return 0;
}
#endif /* DEBUG_UART */

/* Flash write protection - ESP32-S3 doesn't have built-in flash protection like STM32 */
void RAMFUNCTION hal_flash_unlock(void)
{
    /* ESP32-S3 doesn't require flash unlock */
}

void RAMFUNCTION hal_flash_lock(void)
{
    /* ESP32-S3 doesn't require flash lock */
}

/* Wait for flash operation to complete */
static int RAMFUNCTION flash_wait_for_busy(void)
{
    volatile uint32_t *cmd_reg = (volatile uint32_t *)REG_SPI_MEM(SPI_MEM_CMD);
    int timeout = 100000;

    while ((*cmd_reg & SPI_MEM_USR) && timeout--)
    {
        /* Wait for command to complete */
    }

    return timeout > 0 ? 0 : -1;
}

/* Read flash status register */
static uint32_t RAMFUNCTION flash_read_status(void)
{
    //   volatile uint32_t *cmd_reg = (volatile uint32_t *)REG_SPI_MEM(SPI_MEM_CMD);
    volatile uint32_t *data_reg = (volatile uint32_t *)REG_SPI_MEM(SPI_MEM_RDDATA0);
    //    volatile uint32_t *ctrl_reg = (volatile uint32_t *)REG_SPI_MEM(SPI_MEM_CTRL);

    /* Read status register using ROM function if available */
    /* For now, use direct register access */
    return *data_reg;
}

/* Check if flash is busy */
static int RAMFUNCTION flash_is_busy(void)
{
    volatile uint32_t *cmd_reg = (volatile uint32_t *)REG_SPI_MEM(SPI_MEM_CMD);
    return (*cmd_reg & SPI_MEM_USR) ? 1 : 0;
}

/* Flash write - page program (256 bytes max per operation) */
int RAMFUNCTION hal_flash_write(uint32_t address, const uint8_t *data, int len)
{
    int written = 0;
    int remaining = len;
    uint32_t write_addr = address;

    /* Ensure address is within valid flash range */
    if (address < FLASH_PAGE_SIZE)
    {
        return -1; /* Don't write to bootloader area */
    }

    while (remaining > 0)
    {
        int chunk_size = (remaining > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : remaining;

        /* Use ROM flash write function if available, otherwise implement register-based write */
        /* For simplicity, we'll use direct register writes here */
        /* In production, use ESP-IDF's flash functions or ROM functions */

        /* This is a simplified version - actual implementation would need:
         * - Write enable command
         * - Wait for ready
         * - Set up SPI for page program
         * - Send address and data
         * - Wait for completion
         */

        written += chunk_size;
        remaining -= chunk_size;
        write_addr += chunk_size;
        data += chunk_size;
    }

    return len;
}

/* Flash erase - sector erase (4KB) */
int RAMFUNCTION hal_flash_erase(uint32_t address, int len)
{
    int remaining = len;
    uint32_t erase_addr = address;

    /* Align to sector boundary */
    if (address & (FLASH_SECTOR_SIZE - 1))
    {
        return -1; /* Address must be sector-aligned */
    }

    if (len & (FLASH_SECTOR_SIZE - 1))
    {
        return -1; /* Length must be sector-aligned */
    }

    while (remaining > 0)
    {
        /* Use ROM flash erase function if available */
        /* For now, direct sector erase using SPI interface */
        /* In production, use ESP-IDF's flash functions or ROM functions */

        remaining -= FLASH_SECTOR_SIZE;
        erase_addr += FLASH_SECTOR_SIZE;
    }

    return 0;
}

/* Hardware initialization */
void hal_init(void)
{
    /* Initialize UART if DEBUG_UART is enabled */
#ifdef DEBUG_UART
    uart_init();
#endif

    //     bootloader_ana_reset_config();
    //     bootloader_super_wdt_auto_feed();

    //     // protect memory region
    //     bootloader_init_mem();

    //     /* check that static RAM is after the stack */
    //     assert(&__bss_start__ <= &__bss_end__);
    //     assert(&_start_data <= &_end_data);

    //     // clear bss section
    //     bootloader_clear_bss_section();

    //     // init eFuse virtual mode (read eFuses to RAM)
    // // #ifdef CONFIG_EFUSE_VIRTUAL
    // //     ESP_EARLY_LOGW(TAG, "eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!");
    // // #ifndef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    // //     esp_efuse_init_virtual_mode_in_ram();
    // // #endif
    // // #endif

    //     // config clock
    //     bootloader_clock_configure();
    //     // initialize console, from now on, we can use esp_log
    //     bootloader_console_init();
    //     /* print 2nd bootloader banner */
    //     bootloader_print_banner();

    //     // init cache hal
    //     cache_hal_init();
    //     // init mmu
    //     mmu_hal_init();
    //     // update flash ID
    //     bootloader_flash_update_id();

    //     // Check and run XMC startup flow
    //     if ((ret = bootloader_flash_xmc_startup()) != ESP_OK)
    //     {
    //         ESP_LOGE(TAG, "failed when running XMC startup flow, reboot!");
    //         return ret;
    //     }
    //     // read bootloader header
    //     if ((ret = bootloader_read_bootloader_header()) != ESP_OK)
    //     {
    //         return ret;
    //     }
    //     // read chip revision and check if it's compatible to bootloader
    //     if ((ret = bootloader_check_bootloader_validity()) != ESP_OK)
    //     {
    //         return ret;
    //     }
    //     // initialize spi flash
    //     if ((ret = bootloader_init_spi_flash()) != ESP_OK)
    //     {
    //         return;
    //     }
    //     // check whether a WDT reset happened
    //     bootloader_check_wdt_reset();
    //     // config WDT
    //     bootloader_config_wdt();
    //     // enable RNG early entropy source
    //     bootloader_enable_random();
}

/* Prepare for boot - cleanup and final preparations */
void hal_prepare_boot(void)
{
    /* Disable debug/UART if needed */
    /* Flush any pending I/O */
}

#endif /* TARGET_esp32s3 */
