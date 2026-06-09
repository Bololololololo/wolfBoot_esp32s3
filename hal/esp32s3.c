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
#include "xtensa/xt_instr_macros.h"

/* Hardware initialization */
void hal_init(void)
{
    uint32_t memctl = XCHAL_CACHE_MEMCTL_DEFAULT;
    WSR(MEMCTL, memctl);

    // bootloader_ana_reset_config();
    // bootloader_super_wdt_auto_feed();

    // protect memory region
    // bootloader_init_mem();
    /* check that static RAM is after the stack */

    // assert(&_bss_start <= &_bss_end);
    // assert(&_data_start <= &_data_end);

    //     // clear bss section
    // bootloader_clear_bss_section();
    //     // init eFuse virtual mode (read eFuses to RAM)
    // #ifdef CONFIG_EFUSE_VIRTUAL
    //     ESP_EARLY_LOGW(TAG, "eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!");
    // #ifndef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    //     esp_efuse_init_virtual_mode_in_ram();
    // #endif
    // #endif
    //     // config clock
    //     bootloader_clock_configure();
    //     // initialize console, from now on, we can use esp_log
    //     bootloader_console_init();
    //     /* print 2nd bootloader banner */
    //     bootloader_print_banner();

    // #if !CONFIG_APP_BUILD_TYPE_RAM
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

    //     // initialize spi flash -- it's done by the bootloader via spi_probe

    // #endif // !CONFIG_APP_BUILD_TYPE_RAM
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
