#include "spi_drv_esp32s3.h"
#include <stdint.h>
#include "spi_drv.h"
#include <loader.h> /* for wolfBoot_panic */

extern uint32_t esp_rom_efuse_get_flash_gpio_info();
extern uint8_t esp_rom_efuse_get_flash_wp_gpio();
extern void esp_rom_gpio_pad_set_drv(uint32_t iopad_num, uint32_t drv);
extern uint32_t esp_rom_efuse_get_flash_gpio_info();

void IRAM_ATTR
bootloader_configure_spi_pins(int drv)
{
    const uint32_t spiconfig = esp_rom_efuse_get_flash_gpio_info();
    uint8_t wp_pin = esp_rom_efuse_get_flash_wp_gpio();
    uint8_t clk_gpio_num = MSPI_IOMUX_PIN_NUM_CLK;
    uint8_t q_gpio_num = MSPI_IOMUX_PIN_NUM_MISO;
    uint8_t d_gpio_num = MSPI_IOMUX_PIN_NUM_MOSI;
    uint8_t cs0_gpio_num = MSPI_IOMUX_PIN_NUM_CS0;
    uint8_t hd_gpio_num = MSPI_IOMUX_PIN_NUM_HD;
    uint8_t wp_gpio_num = MSPI_IOMUX_PIN_NUM_WP;
    if (spiconfig == 0)
    {
    }
    else
    {
        clk_gpio_num = spiconfig & 0x3f;
        q_gpio_num = (spiconfig >> 6) & 0x3f;
        d_gpio_num = (spiconfig >> 12) & 0x3f;
        cs0_gpio_num = (spiconfig >> 18) & 0x3f;
        hd_gpio_num = (spiconfig >> 24) & 0x3f;
        wp_gpio_num = wp_pin;
    }
    esp_rom_gpio_pad_set_drv(clk_gpio_num, drv);
    esp_rom_gpio_pad_set_drv(q_gpio_num, drv);
    esp_rom_gpio_pad_set_drv(d_gpio_num, drv);
    esp_rom_gpio_pad_set_drv(cs0_gpio_num, drv);
    if (hd_gpio_num <= MAX_PAD_GPIO_NUM)
    {
        esp_rom_gpio_pad_set_drv(hd_gpio_num, drv);
    }
    if (wp_gpio_num <= MAX_PAD_GPIO_NUM)
    {
        esp_rom_gpio_pad_set_drv(wp_gpio_num, drv);
    }
}

void IRAM_ATTR bootloader_flash_set_dummy_out(void)
{
    // Set dummy output for SPI0 and SPI1, which is used in bootloader to read flash.
    REG_SET_BIT(SPI_MEM_CTRL_REG(0), SPI_MEM_FDUMMY_OUT | SPI_MEM_D_POL | SPI_MEM_Q_POL);
    REG_SET_BIT(SPI_MEM_CTRL_REG(1), SPI_MEM_FDUMMY_OUT | SPI_MEM_D_POL | SPI_MEM_Q_POL);
}

void IRAM_ATTR bootloader_flash_cs_timing_config()
{
    // QSPI flash on ESP32-S3 has no CS hold time and CS setup time, so set them to 0 here.
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_HOLD_TIME_V, 0, SPI_MEM_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_SETUP_TIME_V, 0, SPI_MEM_CS_SETUP_TIME_S);
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(0), SPI_MEM_CS_HOLD_M | SPI_MEM_CS_SETUP_M);
}

// IRAM_ATTR uint32_t bootloader_flash_execute_command_common(
//     uint8_t command,
//     uint32_t addr_len, uint32_t address,
//     uint8_t dummy_len,
//     uint8_t mosi_len, uint32_t mosi_data,
//     uint8_t miso_len)
// {
//     assert(mosi_len <= 32);
//     assert(miso_len <= 32);
//     uint32_t old_ctrl_reg = 0;
//     uint32_t old_user_reg = 0;
//     uint32_t old_user1_reg = 0;
//     uint32_t old_user2_reg = 0;
//     spi_flash_ll_get_common_command_register_info(&SPIMEM_LL_APB, &old_ctrl_reg, &old_user_reg, &old_user1_reg, &old_user2_reg);
//     SPIMEM_LL_APB.ctrl.val = 0;
//     spimem_flash_ll_set_wp_level(&SPIMEM_LL_APB, true);
//     // command phase
//     spi_flash_ll_set_command(&SPIMEM_LL_APB, command, 8);
//     // addr phase
//     spi_flash_ll_set_addr_bitlen(&SPIMEM_LL_APB, addr_len);
//     spi_flash_ll_set_usr_address(&SPIMEM_LL_APB, address, addr_len);
//     // dummy phase
//     uint32_t total_dummy = dummy_len;
//     if (miso_len > 0)
//     {
//         total_dummy += g_rom_spiflash_dummy_len_plus[1];
//     }
//     spi_flash_ll_set_dummy(&SPIMEM_LL_APB, total_dummy);
//     // output data

//     spi_flash_ll_set_mosi_bitlen(&SPIMEM_LL_APB, mosi_len);
//     spi_flash_ll_set_buffer_data(&SPIMEM_LL_APB, &mosi_data, mosi_len / 8);
//     // input data
//     spi_flash_ll_set_miso_bitlen(&SPIMEM_LL_APB, miso_len);

//     spi_flash_ll_user_start(&SPIMEM_LL_APB, false);
//     while (!spi_flash_ll_cmd_is_done(&SPIMEM_LL_APB))
//     {
//     }
//     spi_flash_ll_set_common_command_register_info(&SPIMEM_LL_APB, old_ctrl_reg, old_user_reg, old_user1_reg, old_user2_reg);

//     uint32_t output_data = 0;
//     spi_flash_ll_get_buffer_data(&SPIMEM_LL_APB, &output_data, miso_len / 8);
//     uint32_t ret = output_data;
//     if (miso_len < 32)
//     {
//         // set unused bits to 0
//         ret &= ~(UINT32_MAX << miso_len);
//     }
//     return ret;
// }

// uint32_t IRAM_ATTR bootloader_execute_flash_command(uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len)
// {
//     const uint8_t addr_len = 0;
//     const uint8_t address = 0;
//     const uint8_t dummy_len = 0;

//     return bootloader_flash_execute_command_common(command, addr_len, address,
//                                                    dummy_len, mosi_len, mosi_data, miso_len);
// }

// static void bootloader_spi_flash_resume(void)
// {
//     bootloader_execute_flash_command(CMD_RESUME, 0, 0, 0);
//     esp_rom_spiflash_wait_idle(&g_rom_flashchip);
// }

void spi_init(int polarity, int phase)
{
    bootloader_configure_spi_pins(1);
    bootloader_flash_set_dummy_out();
    bootloader_flash_cs_timing_config();

    const uint32_t spiconfig = esp_rom_efuse_get_flash_gpio_info();
    if (spiconfig != ESP_ROM_EFUSE_FLASH_DEFAULT_SPI && spiconfig != ESP_ROM_EFUSE_FLASH_DEFAULT_HSPI)
    {
        spi_release();
        wolfBoot_panic();
    }

    // bootloader_spi_flash_resume();
    // if ((void *)bootloader_flash_unlock != (void *)bootloader_flash_unlock_default)
    // {
    //     ESP_EARLY_LOGD(TAG, "Using overridden bootloader_flash_unlock");
    // }
    // bootloader_flash_unlock();

    // if (!bootloader_flash_is_octal_mode_enabled())
    // {
    //     bootloader_enable_qio_mode();
    // }

    // // header of wolfboot is different from esp-idf bootloader, so we cannot use print_flash_info() here.
    // // print_flash_info(&bootloader_image_hdr);

    // cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
    // update_flash_config(&bootloader_image_hdr);
    // cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);

    // // ensure the flash is write-protected
    // bootloader_enable_wp();
}

void spi_release(void)
{
}

int qspi_transfer(uint8_t fmode, const uint8_t cmd,
                  uint32_t addr, uint32_t addrSz, uint32_t addrMode,
                  uint32_t alt, uint32_t altSz, uint32_t altMode,
                  uint32_t dummySz,
                  uint8_t *data, uint32_t dataSz, uint32_t dataMode)
{
    return 0;
}
