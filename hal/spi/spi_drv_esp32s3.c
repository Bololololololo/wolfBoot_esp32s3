
#include <stdint.h>
#include <loader.h> /* for wolfBoot_panic */

#include "spi_drv.h"
#include "wolfboot/wolfboot.h"

#define ESP_BOOTLOADER_SPIFLASH_BP_MASK_ISSI 0xBC  // BP bits mask for ISSI chips, which has different BP bit layout compared with default flash model. When unlocking, clear these bits to unprotect the whole flash.
#define ESP_BOOTLOADER_SPIFLASH_QE_GD_SR2 0x2      // QE position when you write 8 bits(for SR2) at one time.
#define ESP_BOOTLOADER_SPIFLASH_QE_SR1_2BYTE 0x200 // QE position when you write 16 bits at one time.

// this has to be moved for initialization of the cache context before cache_hal_enable is called, which is required by spi_flash_wrap_enable_77.
static cache_hal_context_t ctx;

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

uint32_t IRAM_ATTR bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len)
{
    assert(mosi_len <= 32);
    assert(miso_len <= 32);
    uint32_t old_ctrl_reg = 0;
    uint32_t old_user_reg = 0;
    uint32_t old_user1_reg = 0;
    uint32_t old_user2_reg = 0;
    spimem_flash_ll_get_common_command_register_info(&SPIMEM_LL_APB, &old_ctrl_reg, &old_user_reg, &old_user1_reg, &old_user2_reg);
    SPIMEM_LL_APB.ctrl.val = 0;
    spimem_flash_ll_set_wp_level(&SPIMEM_LL_APB, true);
    // command phase
    spimem_flash_ll_set_command(&SPIMEM_LL_APB, command, 8);
    // addr phase
    spimem_flash_ll_set_addr_bitlen(&SPIMEM_LL_APB, addr_len);
    spimem_flash_ll_set_usr_address(&SPIMEM_LL_APB, address, addr_len);
    // dummy phase
    uint32_t total_dummy = dummy_len;
    if (miso_len > 0)
    {
        total_dummy += g_rom_spiflash_dummy_len_plus[1];
    }
    spimem_flash_ll_set_dummy(&SPIMEM_LL_APB, total_dummy);
    // output data

    spimem_flash_ll_set_mosi_bitlen(&SPIMEM_LL_APB, mosi_len);
    spimem_flash_ll_set_buffer_data(&SPIMEM_LL_APB, &mosi_data, mosi_len / 8);
    // input data
    spimem_flash_ll_set_miso_bitlen(&SPIMEM_LL_APB, miso_len);

    spimem_flash_ll_user_start(&SPIMEM_LL_APB, false);
    while (!spimem_flash_ll_cmd_is_done(&SPIMEM_LL_APB))
    {
    }
    spimem_flash_ll_set_common_command_register_info(&SPIMEM_LL_APB, old_ctrl_reg, old_user_reg, old_user1_reg, old_user2_reg);

    uint32_t output_data = 0;
    spimem_flash_ll_get_buffer_data(&SPIMEM_LL_APB, &output_data, miso_len / 8);
    uint32_t ret = output_data;
    if (miso_len < 32)
    {
        // set unused bits to 0
        ret &= ~(UINT32_MAX << miso_len);
    }
    return ret;
}

uint32_t IRAM_ATTR bootloader_execute_flash_command(uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len)
{
    const uint8_t addr_len = 0;
    const uint8_t address = 0;
    const uint8_t dummy_len = 0;

    return bootloader_flash_execute_command_common(command, addr_len, address,
                                                   dummy_len, mosi_len, mosi_data, miso_len);
}

static void bootloader_spi_flash_resume(void)
{
    bootloader_execute_flash_command(CMD_RESUME, 0, 0, 0);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
}

static inline __attribute__((always_inline)) bool is_issi_chip(const esp_rom_spiflash_chip_t *chip)
{
    return BYTESHIFT(chip->device_id, 2) == ISSI_ID;
}

static inline __attribute__((always_inline)) bool is_mxic_chip(const esp_rom_spiflash_chip_t *chip)
{
    return BYTESHIFT(chip->device_id, 2) == MXIC_ID;
}

static inline __attribute__((always_inline)) bool is_gd_q_chip(const esp_rom_spiflash_chip_t *chip)
{
    return BYTESHIFT(chip->device_id, 2) == GD_Q_ID_HIGH && BYTESHIFT(chip->device_id, 1) == GD_Q_ID_MID && BYTESHIFT(chip->device_id, 0) >= GD_Q_ID_LOW;
}

IRAM_ATTR unsigned bootloader_read_status_8b_rdsr(void)
{
    return bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);
}

IRAM_ATTR unsigned bootloader_read_status_8b_rdsr2(void)
{
    return bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8);
}

IRAM_ATTR unsigned bootloader_read_status_16b_rdsr_rdsr2(void)
{
    return bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8) | (bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8) << 8);
}

IRAM_ATTR void bootloader_write_status_8b_wrsr(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_WRSR, new_status, 8, 0);
}

IRAM_ATTR void bootloader_write_status_8b_wrsr2(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_WRSR2, new_status, 8, 0);
}

IRAM_ATTR void bootloader_write_status_16b_wrsr(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_WRSR, new_status, 16, 0);
}

IRAM_ATTR unsigned bootloader_read_status_8b_xmc25qu64a(void)
{
    bootloader_execute_flash_command(CMD_OTPEN, 0, 0, 0); /* Enter OTP mode */
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    uint32_t read_status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);
    bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0); /* Exit OTP mode */
    return read_status;
}

IRAM_ATTR void bootloader_write_status_8b_xmc25qu64a(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_OTPEN, 0, 0, 0); /* Enter OTP mode */
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    bootloader_execute_flash_command(CMD_WRSR, new_status, 8, 0);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0); /* Exit OTP mode */
}

static void IRAM_ATTR bootloader_flash_unlock(void)
{
    // At the beginning status == new_status == status_sr2 == new_status_sr2 == 0.
    // If the register doesn't need to be updated, keep them the same (0), so that no command will be actually sent.
    uint16_t status = 0; // status for SR1 or SR1+SR2 if writing SR with 01H + 2Bytes.
    uint16_t new_status = 0;
    uint8_t status_sr2 = 0; // status_sr2 for SR2.
    uint8_t new_status_sr2 = 0;
    uint8_t sr1_bit_num = 0;

    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    if (is_issi_chip(&g_rom_flashchip) || is_mxic_chip(&g_rom_flashchip))
    {
        // Currently ISSI & MXIC share the same command and register layout, which is different from the default model.
        // If any code here needs to be modified, check both chips.
        status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);

        /* Clear all bits in the mask.
        (This is different from ROM esp_rom_spiflash_unlock, which keeps all bits as-is.)
        */
        sr1_bit_num = 8;
        new_status = status & (~ESP_BOOTLOADER_SPIFLASH_BP_MASK_ISSI);
    }
    else if (is_gd_q_chip(&g_rom_flashchip))
    {
        /* The GD chips behaviour is to clear all bits in SR1 and clear bits in SR2 except QE bit.
           Use 01H to write SR1 and 31H to write SR2.
        */
        status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);
        sr1_bit_num = 8;
        new_status = 0;

        status_sr2 = bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8);
        new_status_sr2 = status_sr2 & ESP_BOOTLOADER_SPIFLASH_QE_GD_SR2;
    }
    else
    {
        /* For common behaviour, like XMC chips, Use 01H+2Bytes to write both SR1 and SR2*/
        status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8) | (bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8) << 8);

        /* Clear all bits except QE, if it is set.
        (This is different from ROM esp_rom_spiflash_unlock, which keeps all bits as-is.)
        */
        sr1_bit_num = 16;
        new_status = status & ESP_BOOTLOADER_SPIFLASH_QE_SR1_2BYTE;
    }

    // When SR is written, set to true to indicate that WRDI need to be sent to ensure the protection is ON before return.
    bool status_written = false;
    // Skip if nothing needs to be changed. Meaningless writing to SR increases the risk during write and wastes time.
    if (status != new_status)
    {
        esp_rom_spiflash_wait_idle(&g_rom_flashchip);
        bootloader_execute_flash_command(CMD_WREN, 0, 0, 0);
        bootloader_execute_flash_command(CMD_WRSR, new_status, sr1_bit_num, 0);
        status_written = true;
    }

    if (status_sr2 != new_status_sr2)
    {
        esp_rom_spiflash_wait_idle(&g_rom_flashchip);
        bootloader_execute_flash_command(CMD_WREN, 0, 0, 0);
        bootloader_execute_flash_command(CMD_WRSR2, new_status_sr2, 8, 0);
        status_written = true;
    }

    if (status_written)
    {
        // Call esp_rom_spiflash_wait_idle to make sure previous WRSR is completed.
        esp_rom_spiflash_wait_idle(&g_rom_flashchip);
        bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0);
    }
}

esp_err_t spi_flash_wrap_probe_c0(uint32_t flash_id)
{
    esp_err_t ret = ESP_OK;

    switch (flash_id)
    {
    /* The flash listed here should enter the wrap with command 0xC0 */
    case 0xC22018:
        break;
    default:
        ret = ESP_ERR_NOT_FOUND;
        break;
    }
    return ret;
}

/**
 * @brief Burst read with command 0xC0 + 8 Bytes
 *
 * |------------|-----------------------------|
 * |    data    |         wrap depth          |
 * |     00h    |             8               |
 * |     01h    |             16              |
 * |     02h    |             32              |
 * |     03h    |             64              |
 * |------------|-----------------------------|
 */
esp_err_t spi_flash_wrap_enable_c0(spi_flash_wrap_size_t wrap_size)
{
    uint8_t wrap_code = (uint8_t)(__builtin_ctz(wrap_size) - 3);
    bootloader_flash_execute_command_common(CMD_BURST_RD, 0, 0, 0, 8, wrap_code, 0);
    return ESP_OK;
}

/**
 * @brief Burst read with command 0x77 + 24 Dummy + 8 Bytes
 *
 * |-------------------|-----------------------------|
 * |    data(W6,W5)    |         wrap depth          |
 * |        00h        |             8               |
 * |        01h        |             16              |
 * |        02h        |             32              |
 * |        03h        |             64              |
 * |-------------------|-----------------------------|
 */
esp_err_t spi_flash_wrap_enable_77(spi_flash_wrap_size_t wrap_size)
{
    uint8_t wrap_code = (uint8_t)(((__builtin_ctz(wrap_size) - 3) * 2) << 4);
    // According to the special format, we need enable QIO_FWRITE for command 77h and clear it after this command is done.
    REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_MEM_FWRITE_QIO);
    bootloader_flash_execute_command_common(CMD_WRAP, 0, 0, 6, 8, wrap_code, 0);
    REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_MEM_FWRITE_QIO);
    return ESP_OK;
}

/**
 * @brief Burst read is cleared by setting 0x1xh,
 *        so we set 0x10 to disable this feature.
 */
esp_err_t spi_flash_wrap_clear_c0(void)
{
    bootloader_flash_execute_command_common(CMD_BURST_RD, 0, 0, 0, 8, 0x10, 0);
    return ESP_OK;
}

/**
 * @brief Burst read is cleared by setting W4 bit 1,
 *        so we set 0x10 to disable this feature.
 */
esp_err_t spi_flash_wrap_clear_77(void)
{
    // According to the special format, we need enable QIO_FWRITE for command 77h and clear it after this command is done.
    REG_SET_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_MEM_FWRITE_QIO);
    bootloader_flash_execute_command_common(CMD_WRAP, 0, 0, 6, 8, 0x10, 0);
    REG_CLR_BIT(PERIPHS_SPI_FLASH_USRREG, SPI_MEM_FWRITE_QIO);
    return ESP_OK;
}

const spi_flash_wrap_info_t spi_flash_wrap_list[] = {
    /* method                probe                 chip wrap set              chip wrap clear  */
    {"C0H+8B", spi_flash_wrap_probe_c0, spi_flash_wrap_enable_c0, spi_flash_wrap_clear_c0},
    {"default", NULL, spi_flash_wrap_enable_77, spi_flash_wrap_clear_77},
};

static const spi_flash_wrap_info_t *chip_wrap = NULL;

static esp_err_t spi_flash_wrap_probe(void)
{
    uint32_t flash_chip_id = g_rom_flashchip.device_id;
    const spi_flash_wrap_info_t *chip = spi_flash_wrap_list;
    esp_err_t ret = ESP_OK;
    while (chip->probe)
    {
        ret = chip->probe(flash_chip_id);
        if (ret == ESP_OK)
        {
            break;
        }
        chip++;
    }
    chip_wrap = chip;
    return ret;
}

esp_err_t spi_flash_wrap_enable(spi_flash_wrap_size_t wrap_size)
{
    return chip_wrap->chip_wrap_set(wrap_size);
}

esp_err_t spi_flash_wrap_disable(void)
{
    return chip_wrap->chip_wrap_clr();
}

static esp_err_t enable_qio_mode(bootloader_flash_read_status_fn_t read_status_fn,
                                 bootloader_flash_write_status_fn_t write_status_fn,
                                 uint8_t status_qio_bit)
{
    uint32_t status;

    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    status = read_status_fn();
    wolfBoot_printf("Initial flash chip status: ", status);

    if ((status & (1 << status_qio_bit)) == 0)
    {
        bootloader_execute_flash_command(CMD_WREN, 0, 0, 0);
        write_status_fn(status | (1 << status_qio_bit));

        esp_rom_spiflash_wait_idle(&g_rom_flashchip);

        status = read_status_fn();
        wolfBoot_printf("Updated flash chip status: ", status);
        if ((status & (1 << status_qio_bit)) == 0)
        {
            wolfBoot_printf("Failed to set QIE bit, not enabling QIO mode");
            return ESP_FAIL;
        }
    }
    else
    {
        wolfBoot_printf("QIO mode already enabled in flash");
    }

    wolfBoot_printf("Enabling QIO mode...");
    esp_rom_spiflash_config_readmode(ESP_ROM_SPIFLASH_QIO_MODE);

    esp_rom_spiflash_select_qio_pins(esp_rom_efuse_get_flash_wp_gpio(), esp_rom_efuse_get_flash_gpio_info());
    return ESP_OK;
}

size_t DRAM_ATTR bootloader_flash_qe_list_count = (sizeof(bootloader_flash_qe_support_list) / sizeof(bootloader_qio_info_t));

void bootloader_enable_qio_mode(void)
{
    uint32_t raw_flash_id;
    uint8_t mfg_id;
    uint16_t flash_id;
    size_t i;

    wolfBoot_printf("Probing for QIO mode enable...");
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    raw_flash_id = g_rom_flashchip.device_id;
    wolfBoot_printf("Raw SPI flash chip id: ", raw_flash_id);

    mfg_id = (raw_flash_id >> 16) & 0xFF;
    flash_id = raw_flash_id & 0xFFFF;
    wolfBoot_printf("Manufacturer ID 0x%02x chip ID 0x%04x", mfg_id, flash_id);

    for (i = 0; i < bootloader_flash_qe_list_count - 1; i++)
    {
        const bootloader_qio_info_t *chip = &bootloader_flash_qe_support_list[i];
        if (mfg_id == chip->mfg_id && (flash_id & chip->id_mask) == (chip->flash_id & chip->id_mask))
        {
            wolfBoot_printf("Enabling QIO for flash chip %s", bootloader_flash_qe_support_list[i].manufacturer);
            break;
        }
    }

    if (i == bootloader_flash_qe_list_count - 1)
    {
        wolfBoot_printf("Enabling default flash chip QIO");
    }
    enable_qio_mode(bootloader_flash_qe_support_list[i].read_status_fn,
                    bootloader_flash_qe_support_list[i].write_status_fn,
                    bootloader_flash_qe_support_list[i].status_qio_bit);

    spi_flash_wrap_probe();
    spi_flash_wrap_disable();
}

/**
 * @brief Enable ICache
 *
 * @param inst_autoload_en ICache auto preload enabled
 */
__attribute__((always_inline)) static inline void cache_ll_l1_enable_icache(bool inst_autoload_en)
{
    Cache_Enable_ICache(inst_autoload_en ? CACHE_LL_L1_ICACHE_AUTOLOAD : 0);
}

/**
 * @brief Disable ICache
 */
__attribute__((always_inline)) static inline void cache_ll_l1_disable_icache(void)
{
    Cache_Disable_ICache();
}

/**
 * @brief Enable DCache
 *
 * @param data_autoload_en DCache auto preload enabled
 */
__attribute__((always_inline)) static inline void cache_ll_l1_enable_dcache(bool data_autoload_en)
{
    Cache_Enable_DCache(data_autoload_en ? CACHE_LL_L1_DCACHE_AUTOLOAD : 0);
}

/**
 * @brief Disable DCache
 */
__attribute__((always_inline)) static inline void cache_ll_l1_disable_dcache(void)
{
    Cache_Disable_DCache();
}

/**
 * @brief Disable Cache
 *
 * @param cache_level  level of the cache
 * @param type         see `cache_type_t`
 * @param cache_id     id of the cache in this type and level
 */
__attribute__((always_inline)) static inline void cache_ll_disable_cache(uint32_t cache_level, cache_type_t type, uint32_t cache_id)
{
    switch (type)
    {
    case CACHE_TYPE_INSTRUCTION:
        cache_ll_l1_disable_icache();
        break;
    case CACHE_TYPE_DATA:
        cache_ll_l1_disable_dcache();
        break;
    default: // CACHE_TYPE_ALL
        cache_ll_l1_disable_icache();
        cache_ll_l1_disable_dcache();
        break;
    }
}

/**
 * @brief Enable Cache
 *
 * @param cache_level       level of the cache
 * @param type              see `cache_type_t`
 * @param cache_id          id of the cache in this type and level
 * @param data_autoload_en  data autoload enabled or not
 * @param inst_autoload_en  inst autoload enabled or not
 */
__attribute__((always_inline)) static inline void cache_ll_enable_cache(uint32_t cache_level, cache_type_t type, uint32_t cache_id, bool inst_autoload_en, bool data_autoload_en)
{
    switch (type)
    {
    case CACHE_TYPE_INSTRUCTION:
        cache_ll_l1_enable_icache(inst_autoload_en);
        break;
    case CACHE_TYPE_DATA:
        cache_ll_l1_enable_dcache(data_autoload_en);
        break;
    default: // CACHE_TYPE_ALL
        cache_ll_l1_enable_icache(inst_autoload_en);
        cache_ll_l1_enable_dcache(data_autoload_en);
        break;
    }
}

void cache_hal_disable(uint32_t cache_level, cache_type_t type)
{
    cache_ll_disable_cache(cache_level, type, CACHE_LL_ID_ALL);
}

void cache_hal_enable(uint32_t cache_level, cache_type_t type)
{
    if (cache_level == 1)
    {
        cache_ll_enable_cache(1, type, CACHE_LL_ID_ALL, ctx.l1.i_autoload_en, ctx.l1.d_autoload_en);
    }
    else if (cache_level == 2)
    {
        cache_ll_enable_cache(2, type, CACHE_LL_ID_ALL, ctx.l2.i_autoload_en, ctx.l2.d_autoload_en);
    }
}

void bootloader_enable_wp(void)
{
    bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0); /* Exit OTP mode */
}

static void update_flash_config()
{
    // Set flash chip size SPI FLASH = 16 Mb
    esp_rom_spiflash_config_param(g_rom_flashchip.device_id, 16 * 0x100000, 0x10000, 0x1000, 0x100, 0xffff);
    // TODO: set mode
    // TODO: set frequency
}

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

    bootloader_spi_flash_resume();

    bootloader_flash_unlock();

    bootloader_enable_qio_mode();

    // // header of wolfboot is different from esp-idf bootloader, so we cannot use print_flash_info() here.
    // // print_flash_info(&bootloader_image_hdr);

    cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
    update_flash_config();
    cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);

    // // ensure the flash is write-protected
    bootloader_enable_wp();
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
