#include "spi_drv_esp32s3.h"
#include <stdint.h>
#include "spi_drv.h"

extern uint32_t esp_rom_efuse_get_flash_gpio_info();
extern uint8_t esp_rom_efuse_get_flash_wp_gpio();
extern void esp_rom_gpio_pad_set_drv(uint32_t iopad_num, uint32_t drv);

void IRAM_ATTR bootloader_configure_spi_pins(int drv)
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

void spi_init(int polarity, int phase)
{
    bootloader_configure_spi_pins(1);
    bootloader_flash_set_dummy_out();
    bootloader_flash_cs_timing_config();
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
