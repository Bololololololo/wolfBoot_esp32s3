#ifndef SPI_DRV_ESP32S3_PORTED_H_INCLUDED
#define SPI_DRV_ESP32S3_PORTED_H_INCLUDED

#include <stdbool.h>
#include <string.h>

#define STR(x) #x
#define XSTR(x) STR(x)
// Places code into IRAM instead of flash
#define IRAM_ATTR __attribute__((section(".iram1." XSTR(__COUNTER__))))
// Forces data into DRAM instead of flash
#define DRAM_ATTR __attribute__((section(".dram1." XSTR(__COUNTER__))))

#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr) (addr)

#define BIT(nr) (1UL << (nr))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// MSPI IOMUX PINs
#define MSPI_FUNC_NUM 0
#define MSPI_IOMUX_PIN_NUM_CS1 26
#define MSPI_IOMUX_PIN_NUM_HD 27
#define MSPI_IOMUX_PIN_NUM_WP 28
#define MSPI_IOMUX_PIN_NUM_CS0 29
#define MSPI_IOMUX_PIN_NUM_CLK 30
#define MSPI_IOMUX_PIN_NUM_MISO 31
#define MSPI_IOMUX_PIN_NUM_MOSI 32
#define MSPI_IOMUX_PIN_NUM_D4 33
#define MSPI_IOMUX_PIN_NUM_D5 34
#define MSPI_IOMUX_PIN_NUM_D6 35
#define MSPI_IOMUX_PIN_NUM_D7 36
#define MSPI_IOMUX_PIN_NUM_DQS 37

// There are 2 sets of GPIO pins which could be routed to FSPICS0, FSPICLK, FSPID, FSPIQ, FSPIHD, FSPIWP.
// However, there is only one set of GPIO pins which could be routed to FSPIIO4, FSPIIO5, FSPIIO6, FSPIIO7.
// As default (when we are not going to use Octal SPI), we make use of SPI2_FUNC_NUM to route one of the 2 sets of GPIO pins to FSPICS0 ~ FSPIWP as follows.
#define SPI2_FUNC_NUM 4
#define SPI2_IOMUX_PIN_NUM_HD 9
#define SPI2_IOMUX_PIN_NUM_CS 10
#define SPI2_IOMUX_PIN_NUM_MOSI 11
#define SPI2_IOMUX_PIN_NUM_CLK 12
#define SPI2_IOMUX_PIN_NUM_MISO 13
#define SPI2_IOMUX_PIN_NUM_WP 14

// When using Octal SPI, we make use of SPI2_FUNC_NUM_OCT to route them as follows.
#define SPI2_FUNC_NUM_OCT 2
#define SPI2_IOMUX_PIN_NUM_HD_OCT 33
#define SPI2_IOMUX_PIN_NUM_CS_OCT 34
#define SPI2_IOMUX_PIN_NUM_MOSI_OCT 35
#define SPI2_IOMUX_PIN_NUM_CLK_OCT 36
#define SPI2_IOMUX_PIN_NUM_MISO_OCT 37
#define SPI2_IOMUX_PIN_NUM_WP_OCT 38
#define SPI2_IOMUX_PIN_NUM_IO4_OCT 10
#define SPI2_IOMUX_PIN_NUM_IO5_OCT 11
#define SPI2_IOMUX_PIN_NUM_IO6_OCT 12
#define SPI2_IOMUX_PIN_NUM_IO7_OCT 13

#define MAX_PAD_GPIO_NUM 48

/* SPI_MEM_FDUMMY_OUT : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: In the DUMMY phase the signal level of SPI bus is output by the SPI0 controller..*/
#define SPI_MEM_FDUMMY_OUT (BIT(3))

/* SPI_MEM_D_POL : R/W ;bitpos:[19] ;default: 1'b1 ; */
/*description: The bit is used to set MOSI line polarity, 1: high 0, low.*/
#define SPI_MEM_D_POL (BIT(19))

/* SPI_MEM_Q_POL : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: The bit is used to set MISO line polarity, 1: high 0, low.*/
#define SPI_MEM_Q_POL (BIT(18))

/* SPI_MEM_CS_HOLD_TIME : R/W ;bitpos:[9:5] ;default: 5'h1 ; */
/*description: SPI Bus CS (SPI_CS) signal is delayed to inactive by SPI Bus clock (SPI_CLK), wh
ich is the SPI_CS hold time in non-ECC mode. These bits are combined with SPI_ME
M_CS_HOLD bit..*/
#define SPI_MEM_CS_HOLD_TIME_V 0x1F
#define SPI_MEM_CS_HOLD_TIME_S 5

/* SPI_MEM_CS_SETUP_TIME : R/W ;bitpos:[4:0] ;default: 5'h1 ; */
/*description: (cycles-1) of PREP phase by SPI_CLK, which is the SPI_CS setup time. These bits
are combined with SPI_MEM_CS_SETUP bit..*/
#define SPI_MEM_CS_SETUP_TIME_V 0x1F
#define SPI_MEM_CS_SETUP_TIME_S 0

/* SPI_MEM_CS_HOLD : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to keep SPI_CS low when MSPI is in DONE state..*/
#define SPI_MEM_CS_HOLD_M (BIT(6))

/* SPI_MEM_CS_SETUP : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to keep SPI_CS low when MSPI is in PREP state..*/
#define SPI_MEM_CS_SETUP_M (BIT(7))

#define DR_REG_SPI0_BASE 0x60003000

#define REG_SPI_MEM_BASE(i) (DR_REG_SPI0_BASE - (i) * 0x1000)
#define SPI_MEM_CTRL_REG(i) (REG_SPI_MEM_BASE(i) + 0x8)
#define SPI_MEM_CTRL2_REG(i) (REG_SPI_MEM_BASE(i) + 0x10)
#define SPI_MEM_USER_REG(i) (REG_SPI_MEM_BASE(i) + 0x18)

#define ESP_ROM_EFUSE_FLASH_DEFAULT_SPI (0)
#define ESP_ROM_EFUSE_FLASH_DEFAULT_HSPI (1)

/* SPI commands (actual on-wire commands not SPI controller bitmasks)
   Suitable for use with the bootloader_execute_flash_command static function.
*/
#define CMD_RDID 0x9F
#define CMD_WRSR 0x01
#define CMD_WRSR2 0x31 /* Not all SPI flash uses this command */
#define CMD_WRSR3 0x11 /* Not all SPI flash uses this command */
#define CMD_WREN 0x06
#define CMD_WRENVSR 0x50 /* Flash write enable for volatile SR bits */
#define CMD_WRDI 0x04
#define CMD_RDSR 0x05
#define CMD_RDSR2 0x35  /* Not all SPI flash uses this command */
#define CMD_RDSR3 0x15  /* Not all SPI flash uses this command */
#define CMD_OTPEN 0x3A  /* Enable OTP mode, not all SPI flash uses this command */
#define CMD_RDSFDP 0x5A /* Read the SFDP of the flash */
#define CMD_RESUME 0x7A /* Resume command to clear flash suspend bit */
#define CMD_RESETEN 0x66
#define CMD_RESET 0x99
#define CMD_FASTRD_QIO_4B 0xEC
#define CMD_FASTRD_QUAD_4B 0x6C
#define CMD_FASTRD_DIO_4B 0xBC
#define CMD_FASTRD_DUAL_4B 0x3C
#define CMD_FASTRD_4B 0x0C
#define CMD_SLOWRD_4B 0x13

#define CMD_WRAP 0x77
#define CMD_BURST_RD 0xC0 /* wrap(0x77) and burst read are functionally same. But commands and formats is different */

#define BYTESHIFT(VAR, IDX) (((VAR) >> ((IDX) * 8)) & 0xFF)
#define ISSI_ID 0x9D
#define MXIC_ID 0xC2
#define GD_Q_ID_HIGH 0xC8
#define GD_Q_ID_MID 0x40
#define GD_Q_ID_LOW 0x16

typedef int esp_err_t;
/* Definitions for error constants. */
#define ESP_OK 0    /*!< esp_err_t value indicating success (no error) */
#define ESP_FAIL -1 /*!< Generic esp_err_t code indicating failure */

#define ESP_ERR_NO_MEM 0x101           /*!< Out of memory */
#define ESP_ERR_INVALID_ARG 0x102      /*!< Invalid argument */
#define ESP_ERR_INVALID_STATE 0x103    /*!< Invalid state */
#define ESP_ERR_INVALID_SIZE 0x104     /*!< Invalid size */
#define ESP_ERR_NOT_FOUND 0x105        /*!< Requested resource not found */
#define ESP_ERR_NOT_SUPPORTED 0x106    /*!< Operation or feature not supported */
#define ESP_ERR_TIMEOUT 0x107          /*!< Operation timed out */
#define ESP_ERR_INVALID_RESPONSE 0x108 /*!< Received response was invalid */
#define ESP_ERR_INVALID_CRC 0x109      /*!< CRC or checksum was invalid */
#define ESP_ERR_INVALID_VERSION 0x10A  /*!< Version was invalid */
#define ESP_ERR_INVALID_MAC 0x10B      /*!< MAC address was invalid */
#define ESP_ERR_NOT_FINISHED 0x10C     /*!< Operation has not fully completed */
#define ESP_ERR_NOT_ALLOWED 0x10D      /*!< Operation is not allowed */

#define ESP_ERR_WIFI_BASE 0x3000      /*!< Starting number of WiFi error codes */
#define ESP_ERR_MESH_BASE 0x4000      /*!< Starting number of MESH error codes */
#define ESP_ERR_FLASH_BASE 0x6000     /*!< Starting number of flash error codes */
#define ESP_ERR_HW_CRYPTO_BASE 0xc000 /*!< Starting number of HW cryptography module error codes */
#define ESP_ERR_MEMPROT_BASE 0xd000   /*!< Starting number of Memory Protection API error codes */

#define PERIPHS_SPI_FLASH_CMD SPI_MEM_CMD_REG(1)
#define PERIPHS_SPI_FLASH_ADDR SPI_MEM_ADDR_REG(1)
#define PERIPHS_SPI_FLASH_CTRL SPI_MEM_CTRL_REG(1)
#define PERIPHS_SPI_FLASH_CTRL1 SPI_MEM_CTRL1_REG(1)
#define PERIPHS_SPI_FLASH_STATUS SPI_MEM_RD_STATUS_REG(1)
#define PERIPHS_SPI_FLASH_USRREG SPI_MEM_USER_REG(1)
#define PERIPHS_SPI_FLASH_USRREG1 SPI_MEM_USER1_REG(1)
#define PERIPHS_SPI_FLASH_USRREG2 SPI_MEM_USER2_REG(1)
#define PERIPHS_SPI_FLASH_C0 SPI_MEM_W0_REG(1)
#define PERIPHS_SPI_FLASH_C1 SPI_MEM_W1_REG(1)
#define PERIPHS_SPI_FLASH_C2 SPI_MEM_W2_REG(1)
#define PERIPHS_SPI_FLASH_C3 SPI_MEM_W3_REG(1)
#define PERIPHS_SPI_FLASH_C4 SPI_MEM_W4_REG(1)
#define PERIPHS_SPI_FLASH_C5 SPI_MEM_W5_REG(1)
#define PERIPHS_SPI_FLASH_C6 SPI_MEM_W6_REG(1)
#define PERIPHS_SPI_FLASH_C7 SPI_MEM_W7_REG(1)
#define PERIPHS_SPI_FLASH_TX_CRC SPI_MEM_TX_CRC_REG(1)

/* SPI_MEM_FWRITE_QIO : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to enable 4-bit-mode(4-bm) in ADDR and DOUT phase in SPI1 write ope
ration..*/
#define SPI_MEM_FWRITE_QIO (BIT(15))
#define SPI_MEM_FWRITE_QIO_M (BIT(15))
#define SPI_MEM_FWRITE_QIO_V 0x1
#define SPI_MEM_FWRITE_QIO_S 15

typedef enum
{
    ESP_ROM_SPIFLASH_QIO_MODE = 0,
    ESP_ROM_SPIFLASH_QOUT_MODE,
    ESP_ROM_SPIFLASH_DIO_MODE,
    ESP_ROM_SPIFLASH_DOUT_MODE,
    ESP_ROM_SPIFLASH_FASTRD_MODE,
    ESP_ROM_SPIFLASH_SLOWRD_MODE,
    ESP_ROM_SPIFLASH_OPI_STR_MODE,
    ESP_ROM_SPIFLASH_OPI_DTR_MODE,
    ESP_ROM_SPIFLASH_OOUT_MODE,
    ESP_ROM_SPIFLASH_OIO_STR_MODE,
    ESP_ROM_SPIFLASH_OIO_DTR_MODE,
    ESP_ROM_SPIFLASH_QPI_MODE,
    ESP_ROM_SPIFLASH_OPI_HEX_DTR_MODE,
} esp_rom_spiflash_read_mode_t;

typedef enum
{
    ESP_ROM_SPIFLASH_RESULT_OK,
    ESP_ROM_SPIFLASH_RESULT_ERR,
    ESP_ROM_SPIFLASH_RESULT_TIMEOUT
} esp_rom_spiflash_result_t;

typedef struct
{
    uint32_t device_id;
    uint32_t chip_size; // chip size in bytes
    uint32_t block_size;
    uint32_t sector_size;
    uint32_t page_size;
    uint32_t status_mask;
} esp_rom_spiflash_chip_t;

typedef struct
{
    esp_rom_spiflash_chip_t chip;
    uint8_t dummy_len_plus[3];
    uint8_t sig_matrix;
} esp_rom_spiflash_legacy_data_t;

typedef volatile struct spi_mem_dev_s
{
    union
    {
        struct
        {
            uint32_t reserved0 : 17; /*reserved*/
            uint32_t flash_pe : 1;   /*In user mode, it is set to indicate that program/erase operation will be triggered. The bit is combined with SPI_MEM_USR bit. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t usr : 1;        /*User define command enable.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_hpm : 1;  /*Drive Flash into high performance mode.  The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_res : 1;  /*This bit combined with SPI_MEM_RESANDRES bit releases Flash from the power-down state or high performance mode and obtains the devices ID. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_dp : 1;   /*Drive Flash into power down.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_ce : 1;   /*Chip erase enable. Chip erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_be : 1;   /*Block erase enable(32KB) .  Block erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_se : 1;   /*Sector erase enable(4KB). Sector erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_pp : 1;   /*Page program enable(1 byte ~64 bytes data to be programmed). Page program operation  will be triggered when the bit is set. The bit will be cleared once the operation done .1: enable 0: disable. */
            uint32_t flash_wrsr : 1; /*Write status register enable.   Write status operation  will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_rdsr : 1; /*Read status register-1.  Read status operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_rdid : 1; /*Read JEDEC ID . Read ID command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable. */
            uint32_t flash_wrdi : 1; /*Write flash disable. Write disable command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable. */
            uint32_t flash_wren : 1; /*Write flash enable.  Write enable command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable. */
            uint32_t flash_read : 1; /*Read flash enable. Read flash operation will be triggered when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable. */
        };
        uint32_t val;
    } cmd;
    uint32_t addr;
    union
    {
        struct
        {
            uint32_t reserved0 : 3;   /*reserved*/
            uint32_t fdummy_out : 1;  /*In the DUMMY phase the signal level of SPI bus is output by the SPI0 controller.*/
            uint32_t fdout_oct : 1;   /*Set this bit to enable 8-bit-mode(8-bm) in DOUT phase.*/
            uint32_t fdin_oct : 1;    /*Set this bit to enable 8-bit-mode(8-bm) in DIN phase.*/
            uint32_t faddr_oct : 1;   /*Set this bit to enable 8-bit-mode(8-bm) in ADDR phase.*/
            uint32_t fcmd_dual : 1;   /*Set this bit to enable 2-bit-mode(2-bm) in CMD phase.*/
            uint32_t fcmd_quad : 1;   /*Set this bit to enable 4-bit-mode(4-bm) in CMD phase.*/
            uint32_t fcmd_oct : 1;    /*Set this bit to enable 8-bit-mode(8-bm) in CMD phase.*/
            uint32_t fcs_crc_en : 1;  /*For SPI1,  initialize crc32 module before writing encrypted data to flash. Active low.*/
            uint32_t tx_crc_en : 1;   /*For SPI1,  enable crc32 when writing encrypted data to flash. 1: enable 0:disable*/
            uint32_t reserved12 : 1;  /*reserved*/
            uint32_t fastrd_mode : 1; /*This bit should be set when SPI_MEM_FREAD_QIO, SPI_MEM_FREAD_DIO, SPI_MEM_FREAD_QUAD or SPI_MEM_FREAD_DUAL is set.*/
            uint32_t fread_dual : 1;  /*In hardware 0x3B read operation, DIN phase apply 2 signals. 1: enable 0: disable. */
            uint32_t resandres : 1;   /*The Device ID is read out to SPI_MEM_RD_STATUS register,  this bit combine with spi_mem_flash_res bit. 1: enable 0: disable. */
            uint32_t reserved16 : 2;  /*reserved*/
            uint32_t q_pol : 1;       /*The bit is used to set MISO line polarity, 1: high 0, low*/
            uint32_t d_pol : 1;       /*The bit is used to set MOSI line polarity, 1: high 0, low*/
            uint32_t fread_quad : 1;  /*In hardware 0x6B read operation, DIN phase apply 4 signals(4-bit-mode). 1: enable 0: disable. */
            uint32_t wp : 1;          /*Write protect signal output when SPI is idle.  1: output high, 0: output low. */
            uint32_t wrsr_2b : 1;     /*Two bytes data will be written to status register when it is set. 1: enable 0: disable. */
            uint32_t fread_dio : 1;   /*In hardware 0xBB read operation, ADDR phase and DIN phase apply 2 signals(2-bit-mode). 1: enable 0: disable. */
            uint32_t fread_qio : 1;   /*In hardware 0xEB read operation, ADDR phase and DIN phase apply 4 signals(4-bit-mode). 1: enable 0: disable. */
            uint32_t reserved25 : 7;  /*reserved*/
        };
        uint32_t val;
    } ctrl;
    union
    {
        struct
        {
            uint32_t clk_mode : 2;         /*SPI Bus clock (SPI_CLK) mode bits. 0: SPI Bus clock (SPI_CLK) is off when CS inactive 1: SPI_CLK is delayed one cycle after SPI_CS inactive 2: SPI_CLK is delayed two cycles after SPI_CS inactive 3: SPI_CLK is always on.*/
            uint32_t cs_hold_dly_res : 10; /*After RES/DP/HPM/PES/PER command is sent, SPI1 may waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4 or * 256) SPI_CLK cycles.*/
            uint32_t reserved2 : 18;       /*reserved*/
            uint32_t rxfifo_rst : 1;       /*SPI0 RX FIFO reset signal. Set this bit and clear it before SPI0 transfer starts.*/
            uint32_t reserved31 : 1;       /*reserved*/
        };
        uint32_t val;
    } ctrl1;
    union
    {
        struct
        {
            uint32_t cs_setup_time : 5;        /*(cycles-1) of PREP phase by SPI_CLK, which is the SPI_CS setup time. These bits are combined with SPI_MEM_CS_SETUP bit.*/
            uint32_t cs_hold_time : 5;         /*SPI Bus CS (SPI_CS) signal is delayed to inactive by SPI Bus clock (SPI_CLK), which is the SPI_CS hold time in non-ECC mode. These bits are combined with SPI_MEM_CS_HOLD bit.*/
            uint32_t ecc_cs_hold_time : 3;     /*SPI_MEM_CS_HOLD_TIME + SPI_MEM_ECC_CS_HOLD_TIME is the SPI_CS hold cycle in ECC mode when accessed flash.*/
            uint32_t ecc_skip_page_corner : 1; /*1: MSPI skips page corner when accesses flash. 0: Not skip page corner when accesses flash.*/
            uint32_t ecc_16to18_byte_en : 1;   /*Set this bit to enable MSPI ECC 16 bytes data with 2 ECC bytes mode when accesses flash.*/
            uint32_t reserved15 : 10;          /*reserved*/
            uint32_t cs_hold_delay : 6;        /*These bits are used to set the minimum CS high time tSHSL between SPI burst transfer when accesses to flash. tSHSL is (SPI_MEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.*/
            uint32_t sync_reset : 1;           /*The FSM will be reset.*/
        };
        uint32_t val;
    } ctrl2;
    union
    {
        struct
        {
            uint32_t clkcnt_l : 8;       /*It must equal to the value of SPI_MEM_CLKCNT_N. */
            uint32_t clkcnt_h : 8;       /*It must be a floor value of ((SPI_MEM_CLKCNT_N+1)/2-1).*/
            uint32_t clkcnt_n : 8;       /*When SPI0 accesses flash, f_SPI_CLK = f_MSPI_CORE_CLK/(SPI_MEM_CLKCNT_N+1)*/
            uint32_t reserved24 : 7;     /*reserved*/
            uint32_t clk_equ_sysclk : 1; /*When SPI0 accesses flash, set this bit in 1-division mode, f_SPI_CLK = f_MSPI_CORE_CLK.*/
        };
        uint32_t val;
    } clock;
    union
    {
        struct
        {
            uint32_t reserved0 : 6;         /*reserved*/
            uint32_t cs_hold : 1;           /*Set this bit to keep SPI_CS low when MSPI is in DONE state.*/
            uint32_t cs_setup : 1;          /*Set this bit to keep SPI_CS low when MSPI is in PREP state.*/
            uint32_t reserved8 : 1;         /*reserved*/
            uint32_t ck_out_edge : 1;       /*This bit, combined with SPI_MEM_CK_IDLE_EDGE bit, is used to change the clock mode 0~3 of SPI_CLK. */
            uint32_t reserved10 : 2;        /*reserved*/
            uint32_t fwrite_dual : 1;       /*Set this bit to enable 2-bm in DOUT phase in SPI1 write operation.*/
            uint32_t fwrite_quad : 1;       /*Set this bit to enable 4-bm in DOUT phase in SPI1 write operation.*/
            uint32_t fwrite_dio : 1;        /*Set this bit to enable 2-bm in ADDR and DOUT phase in SPI1 write operation.*/
            uint32_t fwrite_qio : 1;        /*Set this bit to enable 4-bit-mode(4-bm) in ADDR and DOUT phase in SPI1 write operation.*/
            uint32_t reserved16 : 8;        /*reserved*/
            uint32_t usr_miso_highpart : 1; /*DIN phase only access to high-part of the buffer SPI_MEM_W8_REG~SPI_MEM_W15_REG. 1: enable 0: disable. */
            uint32_t usr_mosi_highpart : 1; /*DOUT phase only access to high-part of the buffer SPI_MEM_W8_REG~SPI_MEM_W15_REG. 1: enable 0: disable. */
            uint32_t usr_dummy_idle : 1;    /*SPI_CLK is disabled(No clock edges) in DUMMY phase when the bit is enable.*/
            uint32_t usr_mosi : 1;          /*Set this bit to enable the DOUT phase of an write-data operation.*/
            uint32_t usr_miso : 1;          /*Set this bit to enable enable the DIN phase of a read-data operation.*/
            uint32_t usr_dummy : 1;         /*Set this bit to enable enable the DUMMY phase of an operation.*/
            uint32_t usr_addr : 1;          /*Set this bit to enable enable the ADDR phase of an operation.*/
            uint32_t usr_command : 1;       /*Set this bit to enable enable the CMD phase of an operation.*/
        };
        uint32_t val;
    } user;
    union
    {
        struct
        {
            uint32_t usr_dummy_cyclelen : 6; /*The SPI_CLK cycle length minus 1 of DUMMY phase.*/
            uint32_t reserved6 : 20;         /*reserved*/
            uint32_t usr_addr_bitlen : 6;    /*The length in bits of ADDR phase. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } user1;
    union
    {
        struct
        {
            uint32_t usr_command_value : 16; /*The value of user defined(USR) command.*/
            uint32_t reserved16 : 12;        /*reserved*/
            uint32_t usr_command_bitlen : 4; /*The length in bits of CMD phase. The register value shall be (bit_num-1)*/
        };
        uint32_t val;
    } user2;
    union
    {
        struct
        {
            uint32_t usr_mosi_bit_len : 10; /*The length in bits of DOUT phase. The register value shall be (bit_num-1).*/
            uint32_t reserved10 : 22;       /*reserved*/
        };
        uint32_t val;
    } mosi_dlen;
    union
    {
        struct
        {
            uint32_t usr_miso_bit_len : 10; /*The length in bits of DIN phase. The register value shall be (bit_num-1).*/
            uint32_t reserved10 : 22;       /*reserved*/
        };
        uint32_t val;
    } miso_dlen;
    union
    {
        struct
        {
            uint32_t status : 16;    /*The value is stored when set SPI_MEM_FLASH_RDSR bit and SPI_MEM_FLASH_RES bit.*/
            uint32_t wb_mode : 8;    /*Mode bits in the flash fast read mode  it is combined with SPI_MEM_FASTRD_MODE bit.*/
            uint32_t reserved24 : 8; /*reserved*/
        };
        uint32_t val;
    } rd_status;
    uint32_t ext_addr;
    union
    {
        struct
        {
            uint32_t cs0_dis : 1;        /*Set this bit to raise high SPI_CS pin, which means that the SPI device(flash) connected to SPI_CS is in low level when SPI1 transfer starts.*/
            uint32_t cs1_dis : 1;        /*Set this bit to raise high SPI_CS1 pin, which means that the SPI device(Ext_RAM) connected to SPI_CS1 is in low level when SPI1 transfer starts.*/
            uint32_t reserved0 : 5;      /*reserved*/
            uint32_t fsub_pin : 1;       /*Flash is connected to SPI SUBPIN bus.*/
            uint32_t ssub_pin : 1;       /*Ext_RAM is connected to SPI SUBPIN bus.*/
            uint32_t ck_idle_edge : 1;   /*1: SPI_CLK line is high when idle. 0: SPI_CLK line is low when idle */
            uint32_t cs_keep_active : 1; /*SPI_CS line keep low when the bit is set.*/
            uint32_t auto_per : 1;       /*Set this bit to enable auto PER function. Hardware will sent out PER command if PES command is sent.*/
            uint32_t reserved12 : 20;    /*reserved*/
        };
        uint32_t val;
    } misc;
    uint32_t tx_crc;
    union
    {
        struct
        {
            uint32_t req_en : 1;        /*Set this bit to enable Cache's access and SPI0's transfer.*/
            uint32_t usr_cmd_4byte : 1; /*Set this bit to enable SPI0 read flash with 32 bits address. The value of SPI_MEM_USR_ADDR_BITLEN should be 31.*/
            uint32_t flash_usr_cmd : 1; /*1: The command value of SPI0 reads flash is SPI_MEM_USR_COMMAND_VALUE. 0: Hardware read command value, controlled by SPI_MEM_FREAD_QIO, SPI_MEM_FREAD_DIO, SPI_MEM_FREAD_QUAD, SPI_MEM_FREAD_DUAL and SPI_MEM_FASTRD_MODE bits.*/
            uint32_t fdin_dual : 1;     /*When SPI0 accesses to flash, set this bit to enable 2-bm in DIN phase.*/
            uint32_t fdout_dual : 1;    /*When SPI0 accesses to flash, set this bit to enable 2-bm in DOUT phase.*/
            uint32_t faddr_dual : 1;    /*When SPI0 accesses to flash, set this bit to enable 2-bm in ADDR phase.*/
            uint32_t fdin_quad : 1;     /*When SPI0 accesses to flash, set this bit to enable 4-bm in DIN phase.*/
            uint32_t fdout_quad : 1;    /*When SPI0 accesses to flash, set this bit to enable 4-bm in DOUT phase.*/
            uint32_t faddr_quad : 1;    /*When SPI0 accesses to flash, set this bit to enable 4-bm in ADDR phase.*/
            uint32_t reserved9 : 23;    /*reserved*/
        };
        uint32_t val;
    } cache_fctrl;
    union
    {
        struct
        {
            uint32_t usr_scmd_4byte : 1;       /*Set this bit to enable SPI0 read Ext_RAM with 32 bits address. The value of SPI_MEM_SRAM_ADDR_BITLEN should be 31.*/
            uint32_t usr_sram_dio : 1;         /*Set the bit to enable 2-bm in all the phases of SPI0 Ext_RAM transfer.*/
            uint32_t usr_sram_qio : 1;         /*Set the bit to enable QPI mode in all SPI0 Ext_RAM transfer.*/
            uint32_t usr_wr_sram_dummy : 1;    /*When SPI0 accesses to Ext_RAM, set this bit to enable DUMMY phase in write operations.*/
            uint32_t usr_rd_sram_dummy : 1;    /*When SPI0 accesses to Ext_RAM, set this bit to enable DUMMY phase in read operations.*/
            uint32_t usr_rcmd : 1;             /*1: The command value of SPI0 read Ext_RAM is SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE. 0: The value is 0x2.*/
            uint32_t sram_rdummy_cyclelen : 6; /*When SPI0 accesses to Ext_RAM, it is the SPI_CLK cycles minus 1 of DUMMY phase in read data transfer.*/
            uint32_t reserved12 : 2;           /*reserved*/
            uint32_t sram_addr_bitlen : 6;     /*When SPI0 accesses to Ext_RAM, it is the length in bits of ADDR phase. The register value shall be (bit_num-1).*/
            uint32_t usr_wcmd : 1;             /*1: The command value of SPI0 write Ext_RAM is SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE. 0: The value is 0x3.*/
            uint32_t sram_oct : 1;             /*Set the bit to enable OPI mode in all SPI0 Ext_RAM transfer.*/
            uint32_t sram_wdummy_cyclelen : 6; /*When SPI0 accesses to Ext_RAM, it is the SPI_CLK cycles minus 1 of DUMMY phase in write data transfer.*/
            uint32_t reserved28 : 4;           /*reserved*/
        };
        uint32_t val;
    } cache_sctrl;
    union
    {
        struct
        {
            uint32_t sclk_mode : 2;  /*SPI_CLK mode bits  when SPI0 accesses to Ext_RAM. 0: SPI_CLK is off when CS inactive 1: SPI_CLK is delayed one cycle after CS inactive 2: SPI_CLK is delayed two cycles after CS inactive 3: SPI_CLK is always on.*/
            uint32_t swb_mode : 8;   /*Mode bits when SPI0 accesses to Ext_RAM.*/
            uint32_t sdin_dual : 1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 2-bm in DIN phase.*/
            uint32_t sdout_dual : 1; /*When SPI0 accesses to Ext_RAM, set this bit to enable 2-bm in DOUT phase.*/
            uint32_t saddr_dual : 1; /*When SPI0 accesses to Ext_RAM, set this bit to enable 2-bm in ADDR phase.*/
            uint32_t scmd_dual : 1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 2-bm in CMD phase.*/
            uint32_t sdin_quad : 1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 4-bm in DIN phase.*/
            uint32_t sdout_quad : 1; /*When SPI0 accesses to Ext_RAM, set this bit to enable 4-bm in DOUT phase.*/
            uint32_t saddr_quad : 1; /*When SPI0 accesses to Ext_RAM, set this bit to enable 4-bm in ADDR phase.*/
            uint32_t scmd_quad : 1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 4-bm in CMD phase.*/
            uint32_t sdin_oct : 1;   /*When SPI0 accesses to Ext_RAM, set this bit to enable 8-bm in DIN phase.*/
            uint32_t sdout_oct : 1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 8-bm in DOUT phase.*/
            uint32_t saddr_oct : 1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 8-bm in ADDR phase.*/
            uint32_t scmd_oct : 1;   /*When SPI0 accesses to Ext_RAM, set this bit to enable 8-bm in CMD phase.*/
            uint32_t sdummy_out : 1; /*When SPI0 accesses to Ext_RAM, in the DUMMY phase the signal level of SPI bus is output by the SPI0 controller.*/
            uint32_t reserved23 : 9; /*reserved*/
        };
        uint32_t val;
    } sram_cmd;
    union
    {
        struct
        {
            uint32_t usr_rd_cmd_value : 16; /*When SPI0 reads Ext_RAM, it is the command value of CMD phase.*/
            uint32_t reserved16 : 12;       /*reserved*/
            uint32_t usr_rd_cmd_bitlen : 4; /*When SPI0 reads Ext_RAM, it is the length in bits of CMD phase. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } sram_drd_cmd;
    union
    {
        struct
        {
            uint32_t usr_wr_cmd_value : 16; /*When SPI0 writes Ext_RAM, it is the command value of CMD phase.*/
            uint32_t reserved16 : 12;       /*reserved*/
            uint32_t usr_wr_cmd_bitlen : 4; /*When SPI0 writes Ext_RAM, it is the length in bits of CMD phase. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } sram_dwr_cmd;
    union
    {
        struct
        {
            uint32_t cnt_l : 8;      /*It must equal to the value of SPI_MEM_SCLKCNT_N. */
            uint32_t cnt_h : 8;      /*It must be a floor value of ((SPI_MEM_SCLKCNT_N+1)/2-1).*/
            uint32_t cnt_n : 8;      /*When SPI0 accesses to Ext_RAM, f_SPI_CLK = f_MSPI_CORE_CLK/(SPI_MEM_SCLKCNT_N+1)*/
            uint32_t reserved24 : 7; /*reserved*/
            uint32_t equ_sysclk : 1; /*When SPI0 accesses to Ext_RAM, set this bit in 1-division mode, f_SPI_CLK = f_MSPI_CORE_CLK.*/
        };
        uint32_t val;
    } sram_clk;
    union
    {
        struct
        {
            uint32_t st : 3;         /*The status of SPI0 state machine. 0: idle state(IDLE), 1: preparation state(PREP), 2: send command state(CMD), 3: send address state(ADDR), 4: red data state(DIN), 5:write data state(DOUT), 6: wait state(DUMMY), 7: done state(DONE).*/
            uint32_t reserved3 : 29; /*reserved*/
        };
        uint32_t val;
    } fsm;
    uint32_t data_buf[16]; /*data buffer*/
    union
    {
        struct
        {
            uint32_t waiti_en : 1;             /*Set this bit to enable auto-waiting flash idle operation when PP/SE/BE/CE/WRSR/PES command is sent.*/
            uint32_t waiti_dummy : 1;          /*Set this bit to enable DUMMY phase in auto wait flash idle transfer(RDSR).*/
            uint32_t waiti_cmd : 8;            /*The command value of auto wait flash idle transfer(RDSR).*/
            uint32_t waiti_dummy_cyclelen : 6; /*The dummy cycle length when wait flash idle(RDSR).*/
            uint32_t reserved16 : 16;          /*reserved*/
        };
        uint32_t val;
    } flash_waiti_ctrl;
    union
    {
        struct
        {
            uint32_t flash_per : 1;         /*program erase resume bit, program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_pes : 1;         /*program erase suspend bit, program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_per_wait_en : 1; /*Set this bit to add delay time after program erase resume(PER) is sent.*/
            uint32_t flash_pes_wait_en : 1; /*Set this bit to add delay time after program erase suspend(PES) command is sent.*/
            uint32_t pes_per_en : 1;        /*Set this bit to enable PES transfer trigger PES transfer option.*/
            uint32_t pesr_idle_en : 1;      /*1: Separate PER flash wait idle and PES flash wait idle. 0: Not separate.*/
            uint32_t reserved6 : 26;        /*reserved*/
        };
        uint32_t val;
    } flash_sus_cmd;
    union
    {
        struct
        {
            uint32_t flash_pes_en : 1;      /*Set this bit to enable auto-suspend function.*/
            uint32_t flash_per_command : 8; /*Program/Erase resume command value.*/
            uint32_t flash_pes_command : 8; /*Program/Erase suspend command value.*/
            uint32_t reserved17 : 15;       /*reserved*/
        };
        uint32_t val;
    } flash_sus_ctrl;
    union
    {
        struct
        {
            uint32_t flash_sus : 1;         /*The status of flash suspend. This bit is set when PES command is sent, and cleared when PER is sent. Only used in SPI1.*/
            uint32_t reserved1 : 1;         /*reserved*/
            uint32_t flash_hpm_dly_256 : 1; /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after HPM command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after HPM command is sent.*/
            uint32_t flash_res_dly_256 : 1; /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after RES command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after RES command is sent.*/
            uint32_t flash_dp_dly_256 : 1;  /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after DP command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after DP command is sent.*/
            uint32_t flash_per_dly_256 : 1; /*Valid when SPI_MEM_FLASH_PER_WAIT_EN is 1. 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after PER command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PER command is sent.*/
            uint32_t flash_pes_dly_256 : 1; /*Valid when SPI_MEM_FLASH_PES_WAIT_EN is 1. 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after PES command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PES command is sent.*/
            uint32_t reserved7 : 25;        /*reserved*/
        };
        uint32_t val;
    } sus_status;
    union
    {
        struct
        {
            uint32_t timing_clk_ena : 1;       /*Set this bit to power on HCLK. When PLL is powered on, the frequency of HCLK equals to that of PLL. Otherwise, the frequency equals to that of XTAL.*/
            uint32_t timing_cali : 1;          /*Set this bit to add extra SPI_CLK cycles in DUMMY phase for all reading operations.*/
            uint32_t extra_dummy_cyclelen : 3; /*Extra SPI_CLK cycles added in DUMMY phase for timing compensation, when SPI0 accesses to flash. Active when SPI_MEM_TIMING_CALI bit is set.*/
            uint32_t reserved5 : 27;           /*reserved*/
        };
        uint32_t val;
    } timing_cali;
    union
    {
        struct
        {
            uint32_t din0_mode : 3;  /*SPI_D input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din1_mode : 3;  /*SPI_Q input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din2_mode : 3;  /*SPI_WP input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din3_mode : 3;  /*SPI_HD input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din4_mode : 3;  /*SPI_IO4 input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din5_mode : 3;  /*SPI_IO5 input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din6_mode : 3;  /*SPI_IO6 input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din7_mode : 3;  /*SPI_IO7 input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dins_mode : 3;  /*SPI_DQS input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DINS_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DINS_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DINS_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DINS_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DINS_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t reserved27 : 5; /*reserved*/
        };
        uint32_t val;
    } din_mode;
    union
    {
        struct
        {
            uint32_t din0_num : 2;    /*SPI_D input delay number.*/
            uint32_t din1_num : 2;    /*SPI_Q input delay number.*/
            uint32_t din2_num : 2;    /*SPI_WP input delay number.*/
            uint32_t din3_num : 2;    /*SPI_HD input delay number.*/
            uint32_t din4_num : 2;    /*SPI_IO4 input delay number.*/
            uint32_t din5_num : 2;    /*SPI_IO5 input delay number.*/
            uint32_t din6_num : 2;    /*SPI_IO6 input delay number.*/
            uint32_t din7_num : 2;    /*SPI_IO7 input delay number.*/
            uint32_t dins_num : 2;    /*SPI_DQS input delay number.*/
            uint32_t reserved18 : 14; /*reserved*/
        };
        uint32_t val;
    } din_num;
    union
    {
        struct
        {
            uint32_t dout0_mode : 1; /*SPI_D output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout1_mode : 1; /*SPI_Q output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout2_mode : 1; /*SPI_WP output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout3_mode : 1; /*SPI_HD output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout4_mode : 1; /*SPI_IO4 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout5_mode : 1; /*SPI_IO5 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout6_mode : 1; /*SPI_IO6 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout7_mode : 1; /*SPI_IO7 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t douts_mode : 1; /*SPI_DQS output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t reserved9 : 23; /*reserved*/
        };
        uint32_t val;
    } dout_mode;
    uint32_t reserved_b8;
    union
    {
        struct
        {
            uint32_t smem_timing_clk_ena : 1;       /*Set this bit to power on HCLK. When PLL is powered on, the frequency of HCLK equals to that of PLL. Otherwise, the frequency equals to that of XTAL.*/
            uint32_t smem_timing_cali : 1;          /*Set this bit to add extra SPI_CLK cycles in DUMMY phase for all reading operations.*/
            uint32_t smem_extra_dummy_cyclelen : 3; /*Extra SPI_CLK cycles added in DUMMY phase for timing compensation, when SPI0 accesses to Ext_RAM. Active when SPI_SMEM_TIMING_CALI bit is set.*/
            uint32_t reserved5 : 27;                /*reserved*/
        };
        uint32_t val;
    } spi_smem_timing_cali;
    union
    {
        struct
        {
            uint32_t smem_din0_mode : 3; /*SPI_D input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din1_mode : 3; /*SPI_Q input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din2_mode : 3; /*SPI_WP input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din3_mode : 3; /*SPI_HD input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din4_mode : 3; /*SPI_IO4 input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din5_mode : 3; /*SPI_IO5 input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din6_mode : 3; /*SPI_IO6 input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din7_mode : 3; /*SPI_IO7 input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dins_mode : 3; /*SPI_DQS input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DINS_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DINS_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DINS_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DINS_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DINS_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t reserved27 : 5;     /*reserved*/
        };
        uint32_t val;
    } spi_smem_din_mode;
    union
    {
        struct
        {
            uint32_t smem_din0_num : 2; /*SPI_D input delay number.*/
            uint32_t smem_din1_num : 2; /*SPI_Q input delay number.*/
            uint32_t smem_din2_num : 2; /*SPI_WP input delay number.*/
            uint32_t smem_din3_num : 2; /*SPI_HD input delay number.*/
            uint32_t smem_din4_num : 2; /*SPI_IO4 input delay number.*/
            uint32_t smem_din5_num : 2; /*SPI_IO5 input delay number.*/
            uint32_t smem_din6_num : 2; /*SPI_IO6 input delay number.*/
            uint32_t smem_din7_num : 2; /*SPI_IO7 input delay number.*/
            uint32_t smem_dins_num : 2; /*SPI_DQS input delay number.*/
            uint32_t reserved18 : 14;   /*reserved*/
        };
        uint32_t val;
    } spi_smem_din_num;
    union
    {
        struct
        {
            uint32_t smem_dout0_mode : 1; /*SPI_D output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout1_mode : 1; /*SPI_Q output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout2_mode : 1; /*SPI_WP output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout3_mode : 1; /*SPI_HD output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout4_mode : 1; /*SPI_IO4 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout5_mode : 1; /*SPI_IO5 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout6_mode : 1; /*SPI_IO6 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout7_mode : 1; /*SPI_IO7 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_douts_mode : 1; /*SPI_DQS output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t reserved9 : 23;      /*reserved*/
        };
        uint32_t val;
    } spi_smem_dout_mode;
    union
    {
        struct
        {
            uint32_t ecc_err_int_num : 8;     /*Set the error times of MSPI ECC read to generate MSPI SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t fmem_ecc_err_int_en : 1; /*Set this bit to calculate the error times of MSPI ECC read when accesses to flash.*/
            uint32_t reserved9 : 23;          /*reserved*/
        };
        uint32_t val;
    } ecc_ctrl;
    uint32_t ecc_err_addr;
    union
    {
        struct
        {
            uint32_t reserved0 : 6;        /*reserved*/
            uint32_t ecc_data_err_bit : 7; /*It records the first ECC data error bit number when SPI_FMEM_ECC_ERR_INT_EN/SPI_SMEM_ECC_ERR_INT_EN is set and accessed to flash/Ext_RAM. The value ranges from 0~127, corresponding to the bit number in 16 data bytes. It is cleared by SPI_MEM_ECC_ERR_INT_CLR bit.*/
            uint32_t ecc_chk_err_bit : 3;  /*When SPI_MEM_ECC_BYTE_ERR is set, these bits show the error bit number of ECC byte.*/
            uint32_t ecc_byte_err : 1;     /*It records the first ECC byte error when SPI_FMEM_ECC_ERR_INT_EN/SPI_SMEM_ECC_ERR_INT_EN is set and accessed to flash/Ext_RAM. It is cleared by SPI_MEM_ECC_ERR_INT_CLR bit.*/
            uint32_t ecc_err_cnt : 8;      /*This bits show the error times of MSPI ECC read, including ECC byte error and data byte error. It is cleared by when  SPI_MEM_ECC_ERR_INT_CLR bit is set. */
            uint32_t reserved25 : 7;       /*reserved*/
        };
        uint32_t val;
    } ecc_err_bit;
    uint32_t reserved_d8;
    union
    {
        struct
        {
            uint32_t smem_cs_setup : 1;             /*Set this bit to keep SPI_CS low when MSPI is in PREP state.*/
            uint32_t smem_cs_hold : 1;              /*Set this bit to keep SPI_CS low when MSPI is in DONE state.*/
            uint32_t smem_cs_setup_time : 5;        /*(cycles-1) of PREP phase by SPI_CLK, which is the SPI_CS setup time. These bits are combined with SPI_MEM_CS_SETUP bit.*/
            uint32_t smem_cs_hold_time : 5;         /*SPI Bus CS (SPI_CS) signal is delayed to inactive by SPI Bus clock (SPI_CLK), which is the SPI_CS hold time in non-ECC mode. These bits are combined with SPI_MEM_CS_HOLD bit.*/
            uint32_t smem_ecc_cs_hold_time : 3;     /*SPI_SMEM_CS_HOLD_TIME + SPI_SMEM_ECC_CS_HOLD_TIME is the MSPI CS hold cycles in ECC mode when accesses to external RAM.*/
            uint32_t smem_ecc_skip_page_corner : 1; /*1: MSPI skips page corner when accesses to external RAM. 0: Not skip page corner when accesses to external RAM.*/
            uint32_t smem_ecc_16to18_byte_en : 1;   /*Set this bit to enable MSPI ECC 16 bytes data with 2 ECC bytes mode when accesses to external RAM.*/
            uint32_t reserved17 : 7;                /*reserved*/
            uint32_t smem_ecc_err_int_en : 1;       /*Set this bit to calculate the error times of MSPI ECC read when accesses to external RAM.*/
            uint32_t smem_cs_hold_delay : 6;        /*These bits are used to set the minimum CS high time tSHSL between SPI burst transfer when accesses to external RAM. tSHSL is (SPI_SMEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.*/
            uint32_t reserved31 : 1;                /*reserved*/
        };
        uint32_t val;
    } spi_smem_ac;
    union
    {
        struct
        {
            uint32_t fmem_ddr_en : 1;            /*1: in ddr mode,  0 in sdr mode*/
            uint32_t fmem_var_dummy : 1;         /*Set the bit to enable variable dummy cycle in DDR mode.*/
            uint32_t fmem_ddr_rdat_swp : 1;      /*Set the bit to reorder RX data of the word in DDR mode.*/
            uint32_t fmem_ddr_wdat_swp : 1;      /*Set the bit to swap TX data of a word in DDR mode.*/
            uint32_t fmem_ddr_cmd_dis : 1;       /*the bit is used to disable dual edge in CMD phase when ddr mode.*/
            uint32_t fmem_outminbytelen : 7;     /*It is the minimum output data length in the panda device.*/
            uint32_t fmem_tx_ddr_msk_en : 1;     /*Set this bit to mask the first or the last byte in MSPI ECC DDR write mode, when accesses to flash.*/
            uint32_t fmem_rx_ddr_msk_en : 1;     /*Set this bit to mask the first or the last byte in MSPI ECC DDR read mode, when accesses to flash.*/
            uint32_t fmem_usr_ddr_dqs_thd : 7;   /*The delay number of data strobe which from memory based on SPI_CLK.*/
            uint32_t fmem_ddr_dqs_loop : 1;      /*1: Use internal signal  as data strobe, the strobe can not be delayed by input timing module. 0: Use input SPI_DQS signal from PAD as data strobe, the strobe can be delayed by input timing module*/
            uint32_t fmem_ddr_dqs_loop_mode : 1; /*When SPI_FMEM_DDR_DQS_LOOP and SPI_FMEM_DDR_EN are set, 1: Use internal SPI_CLK  as data strobe. 0: Use internal ~SPI_CLK  as data strobe. Otherwise this bit is not active.*/
            uint32_t reserved23 : 1;             /*reserved*/
            uint32_t fmem_clk_diff_en : 1;       /*Set this bit to enable the differential SPI_CLK#.*/
            uint32_t fmem_hyperbus_mode : 1;     /*Set this bit to enable the SPI HyperBus mode.*/
            uint32_t fmem_dqs_ca_in : 1;         /*Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.*/
            uint32_t fmem_hyperbus_dummy_2x : 1; /*Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0 accesses to flash or SPI1 accesses flash or sram.*/
            uint32_t fmem_clk_diff_inv : 1;      /*Set this bit to invert SPI_DIFF when accesses to flash. .*/
            uint32_t fmem_octa_ram_addr : 1;     /*Set this bit to enable octa_ram address out when accesses to flash, which means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.*/
            uint32_t fmem_hyperbus_ca : 1;       /*Set this bit to enable HyperRAM address out when accesses to flash, which means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.*/
            uint32_t reserved31 : 1;             /*reserved*/
        };
        uint32_t val;
    } ddr;
    union
    {
        struct
        {
            uint32_t smem_ddr_en : 1;            /*1: in ddr mode,  0 in sdr mode*/
            uint32_t smem_var_dummy : 1;         /*Set the bit to enable variable dummy cycle in spi ddr mode.*/
            uint32_t smem_ddr_rdat_swp : 1;      /*Set the bit to reorder rx data of the word in spi ddr mode.*/
            uint32_t smem_ddr_wdat_swp : 1;      /*Set the bit to reorder tx data of the word in spi ddr mode.*/
            uint32_t smem_ddr_cmd_dis : 1;       /*the bit is used to disable dual edge in CMD phase when ddr mode.*/
            uint32_t smem_outminbytelen : 7;     /*It is the minimum output data length in the ddr psram.*/
            uint32_t smem_tx_ddr_msk_en : 1;     /*Set this bit to mask the first or the last byte in MSPI ECC DDR write mode, when accesses to external RAM.*/
            uint32_t smem_rx_ddr_msk_en : 1;     /*Set this bit to mask the first or the last byte in MSPI ECC DDR read mode, when accesses to external RAM.*/
            uint32_t smem_usr_ddr_dqs_thd : 7;   /*The delay number of data strobe which from memory based on SPI_CLK.*/
            uint32_t smem_ddr_dqs_loop : 1;      /*1: Use internal signal  as data strobe, the strobe can not be delayed by input timing module. 0: Use input SPI_DQS signal from PAD as data strobe, the strobe can be delayed by input timing module*/
            uint32_t smem_ddr_dqs_loop_mode : 1; /*When SPI_SMEM_DDR_DQS_LOOP and SPI_SMEM_DDR_EN are set, 1: Use internal SPI_CLK  as data strobe. 0: Use internal ~SPI_CLK  as data strobe. Otherwise this bit is not active.*/
            uint32_t reserved23 : 1;             /*reserved*/
            uint32_t smem_clk_diff_en : 1;       /*Set this bit to enable the differential SPI_CLK#.*/
            uint32_t smem_hyperbus_mode : 1;     /*Set this bit to enable the SPI HyperBus mode.*/
            uint32_t smem_dqs_ca_in : 1;         /*Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.*/
            uint32_t smem_hyperbus_dummy_2x : 1; /*Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0 accesses to flash or SPI1 accesses flash or sram.*/
            uint32_t smem_clk_diff_inv : 1;      /*Set this bit to invert SPI_DIFF when accesses to external RAM. .*/
            uint32_t smem_octa_ram_addr : 1;     /*Set this bit to enable octa_ram address out when accesses to external RAM, which means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.*/
            uint32_t smem_hyperbus_ca : 1;       /*Set this bit to enable HyperRAM address out when accesses to external RAM, which means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.*/
            uint32_t reserved31 : 1;             /*reserved*/
        };
        uint32_t val;
    } spi_smem_ddr;
    union
    {
        struct
        {
            uint32_t clk_en : 1;     /*Register clock gate enable signal. 1: Enable. 0: Disable.*/
            uint32_t reserved1 : 31; /*reserved*/
        };
        uint32_t val;
    } clock_gate;
    union
    {
        struct
        {
            uint32_t core_clk_sel : 2; /*When the digital system clock selects PLL clock and the frequency of PLL clock is 480MHz, the value of SPI_MEM_CORE_CLK_SEL:  0: SPI0/1 module clock (MSPI_CORE_CLK) is 80MHz. 1: MSPI_CORE_CLK is 120MHz.  2: MSPI_CORE_CLK is 160MHz. 3: MSPI_CORE_CLK is 240MHz. When the digital system clock selects PLL clock and the frequency of PLL clock is 320MHz, the value of SPI_MEM_CORE_CLK_SEL:  0: MSPI_CORE_CLK is 80MHz. 1: MSPI_CORE_CLK is 80MHz.  2: MSPI_CORE_CLK 160MHz. 3: Not used. */
            uint32_t reserved2 : 30;   /*reserved*/
        };
        uint32_t val;
    } core_clk_sel;
    union
    {
        struct
        {
            uint32_t per_end_en : 1;         /*The enable bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end_en : 1;         /*The enable bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t total_trans_end_en : 1; /*The enable bit for SPI_MEM_TOTAL_TRANS_END_INT interrupt.*/
            uint32_t brown_out_en : 1;       /*The enable bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t ecc_err_en : 1;         /*The enable bit for SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t reserved5 : 27;         /*reserved*/
        };
        uint32_t val;
    } int_ena;
    union
    {
        struct
        {
            uint32_t per_end_int_clr : 1;         /*The clear bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end_int_clr : 1;         /*The clear bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t total_trans_end_int_clr : 1; /*The clear bit for SPI_MEM_TOTAL_TRANS_END_INT interrupt.*/
            uint32_t brown_out_int_clr : 1;       /*The status bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t ecc_err_int_clr : 1;         /*The clear bit for SPI_MEM_ECC_ERR_INT interrupt. SPI_MEM_ECC_ERR_ADDR and SPI_MEM_ECC_ERR_CNT will be cleared by the pulse of this bit.*/
            uint32_t reserved5 : 27;              /*reserved*/
        };
        uint32_t val;
    } int_clr;
    union
    {
        struct
        {
            uint32_t per_end_int_raw : 1;         /*The raw bit for SPI_MEM_PER_END_INT interrupt. 1: Triggered when Auto Resume command (0x7A) is sent and flash is resumed successfully. 0: Others.*/
            uint32_t pes_end_int_raw : 1;         /*The raw bit for SPI_MEM_PES_END_INT interrupt.1: Triggered when Auto Suspend command (0x75) is sent and flash is suspended successfully. 0: Others.*/
            uint32_t total_trans_end_int_raw : 1; /*The raw bit for SPI_MEM_TOTAL_TRANS_END_INT interrupt. 1: Triggered when SPI1 transfer is done and flash is already idle. When WRSR/PP/SE/BE/CE is sent and PES/PER command is sent, this bit is set when WRSR/PP/SE/BE/CE is success.  0: Others.*/
            uint32_t brown_out_int_raw : 1;       /*The raw bit for SPI_MEM_BROWN_OUT_INT interrupt. 1: Triggered condition is that chip is losing power and RTC module sends out brown out close flash request to SPI1. After SPI1 sends out suspend command to flash, this interrupt is triggered and MSPI returns to idle state. 0: Others.*/
            uint32_t ecc_err_int_raw : 1;         /*The raw bit for SPI_MEM_ECC_ERR_INT interrupt. When APB_CTRL_FECC_ERR_INT_EN is set and  APB_CTRL_SECC_ERR_INT_EN is cleared, this bit is triggered when the error times of SPI0/1 ECC read flash are equal or bigger than APB_CTRL_ECC_ERR_INT_NUM. When APB_CTRL_FECC_ERR_INT_EN is cleared and  APB_CTRL_SECC_ERR_INT_EN is set, this bit is triggered when the error times of SPI0/1 ECC read external RAM are equal or bigger than APB_CTRL_ECC_ERR_INT_NUM. When APB_CTRL_FECC_ERR_INT_EN and  APB_CTRL_SECC_ERR_INT_EN are set, this bit is triggered when the total error times of SPI0/1 ECC read external RAM and flash are equal or bigger than APB_CTRL_ECC_ERR_INT_NUM. When APB_CTRL_FECC_ERR_INT_EN and  APB_CTRL_SECC_ERR_INT_EN are cleared, this bit will not be triggered.*/
            uint32_t reserved5 : 27;              /*reserved*/
        };
        uint32_t val;
    } int_raw;
    union
    {
        struct
        {
            uint32_t per_end_int_st : 1;         /*The status bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end_int_st : 1;         /*The status bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t total_trans_end_int_st : 1; /*The status bit for SPI_MEM_TOTAL_TRANS_END_INT interrupt.*/
            uint32_t brown_out_int_st : 1;       /*The status bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t ecc_err_int_st : 1;         /*The status bit for SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t reserved5 : 27;             /*reserved*/
        };
        uint32_t val;
    } int_st;
    uint32_t reserved_100;
    uint32_t reserved_104;
    uint32_t reserved_108;
    uint32_t reserved_10c;
    uint32_t reserved_110;
    uint32_t reserved_114;
    uint32_t reserved_118;
    uint32_t reserved_11c;
    uint32_t reserved_120;
    uint32_t reserved_124;
    uint32_t reserved_128;
    uint32_t reserved_12c;
    uint32_t reserved_130;
    uint32_t reserved_134;
    uint32_t reserved_138;
    uint32_t reserved_13c;
    uint32_t reserved_140;
    uint32_t reserved_144;
    uint32_t reserved_148;
    uint32_t reserved_14c;
    uint32_t reserved_150;
    uint32_t reserved_154;
    uint32_t reserved_158;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t reserved_180;
    uint32_t reserved_184;
    uint32_t reserved_188;
    uint32_t reserved_18c;
    uint32_t reserved_190;
    uint32_t reserved_194;
    uint32_t reserved_198;
    uint32_t reserved_19c;
    uint32_t reserved_1a0;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    uint32_t reserved_1c0;
    uint32_t reserved_1c4;
    uint32_t reserved_1c8;
    uint32_t reserved_1cc;
    uint32_t reserved_1d0;
    uint32_t reserved_1d4;
    uint32_t reserved_1d8;
    uint32_t reserved_1dc;
    uint32_t reserved_1e0;
    uint32_t reserved_1e4;
    uint32_t reserved_1e8;
    uint32_t reserved_1ec;
    uint32_t reserved_1f0;
    uint32_t reserved_1f4;
    uint32_t reserved_1f8;
    uint32_t reserved_1fc;
    uint32_t reserved_200;
    uint32_t reserved_204;
    uint32_t reserved_208;
    uint32_t reserved_20c;
    uint32_t reserved_210;
    uint32_t reserved_214;
    uint32_t reserved_218;
    uint32_t reserved_21c;
    uint32_t reserved_220;
    uint32_t reserved_224;
    uint32_t reserved_228;
    uint32_t reserved_22c;
    uint32_t reserved_230;
    uint32_t reserved_234;
    uint32_t reserved_238;
    uint32_t reserved_23c;
    uint32_t reserved_240;
    uint32_t reserved_244;
    uint32_t reserved_248;
    uint32_t reserved_24c;
    uint32_t reserved_250;
    uint32_t reserved_254;
    uint32_t reserved_258;
    uint32_t reserved_25c;
    uint32_t reserved_260;
    uint32_t reserved_264;
    uint32_t reserved_268;
    uint32_t reserved_26c;
    uint32_t reserved_270;
    uint32_t reserved_274;
    uint32_t reserved_278;
    uint32_t reserved_27c;
    uint32_t reserved_280;
    uint32_t reserved_284;
    uint32_t reserved_288;
    uint32_t reserved_28c;
    uint32_t reserved_290;
    uint32_t reserved_294;
    uint32_t reserved_298;
    uint32_t reserved_29c;
    uint32_t reserved_2a0;
    uint32_t reserved_2a4;
    uint32_t reserved_2a8;
    uint32_t reserved_2ac;
    uint32_t reserved_2b0;
    uint32_t reserved_2b4;
    uint32_t reserved_2b8;
    uint32_t reserved_2bc;
    uint32_t reserved_2c0;
    uint32_t reserved_2c4;
    uint32_t reserved_2c8;
    uint32_t reserved_2cc;
    uint32_t reserved_2d0;
    uint32_t reserved_2d4;
    uint32_t reserved_2d8;
    uint32_t reserved_2dc;
    uint32_t reserved_2e0;
    uint32_t reserved_2e4;
    uint32_t reserved_2e8;
    uint32_t reserved_2ec;
    uint32_t reserved_2f0;
    uint32_t reserved_2f4;
    uint32_t reserved_2f8;
    uint32_t reserved_2fc;
    uint32_t reserved_300;
    uint32_t reserved_304;
    uint32_t reserved_308;
    uint32_t reserved_30c;
    uint32_t reserved_310;
    uint32_t reserved_314;
    uint32_t reserved_318;
    uint32_t reserved_31c;
    uint32_t reserved_320;
    uint32_t reserved_324;
    uint32_t reserved_328;
    uint32_t reserved_32c;
    uint32_t reserved_330;
    uint32_t reserved_334;
    uint32_t reserved_338;
    uint32_t reserved_33c;
    uint32_t reserved_340;
    uint32_t reserved_344;
    uint32_t reserved_348;
    uint32_t reserved_34c;
    uint32_t reserved_350;
    uint32_t reserved_354;
    uint32_t reserved_358;
    uint32_t reserved_35c;
    uint32_t reserved_360;
    uint32_t reserved_364;
    uint32_t reserved_368;
    uint32_t reserved_36c;
    uint32_t reserved_370;
    uint32_t reserved_374;
    uint32_t reserved_378;
    uint32_t reserved_37c;
    uint32_t reserved_380;
    uint32_t reserved_384;
    uint32_t reserved_388;
    uint32_t reserved_38c;
    uint32_t reserved_390;
    uint32_t reserved_394;
    uint32_t reserved_398;
    uint32_t reserved_39c;
    uint32_t reserved_3a0;
    uint32_t reserved_3a4;
    uint32_t reserved_3a8;
    uint32_t reserved_3ac;
    uint32_t reserved_3b0;
    uint32_t reserved_3b4;
    uint32_t reserved_3b8;
    uint32_t reserved_3bc;
    uint32_t reserved_3c0;
    uint32_t reserved_3c4;
    uint32_t reserved_3c8;
    uint32_t reserved_3cc;
    uint32_t reserved_3d0;
    uint32_t reserved_3d4;
    uint32_t reserved_3d8;
    uint32_t reserved_3dc;
    uint32_t reserved_3e0;
    uint32_t reserved_3e4;
    uint32_t reserved_3e8;
    uint32_t reserved_3ec;
    uint32_t reserved_3f0;
    uint32_t reserved_3f4;
    uint32_t reserved_3f8;
    union
    {
        struct
        {
            uint32_t reg_smem_spiclk_fun_drv : 2;   /*The driver of SPI_CLK PAD  is controlled by the bits SPI_SMEM_SPICLK_FUN_DRV[1:0] when the bit SPI_SPICLK_PAD_DRV_CTL_EN is set and MSPI accesses to external RAM.*/
            uint32_t fmem_spiclk_fun_drv : 2;       /*The driver of SPI_CLK PAD  is controlled by the bits SPI_FMEM_SPICLK_FUN_DRV[1:0] when the bit SPI_SPICLK_PAD_DRV_CTL_EN is set and MSPI accesses to flash.*/
            uint32_t reg_spiclk_pad_drv_ctl_en : 1; /*SPI_CLK PAD driver control signal. 1: The driver of SPI_CLK PAD  is controlled by the bits SPI_FMEM_SPICLK_FUN_DRV[1:0] and SPI_SMEM_SPICLK_FUN_DRV[1:0]. 0: The driver of SPI_CLK PAD  is controlled by the bits IO_MUX_FUNC_DRV[1:0] of SPICLK PAD.*/
            uint32_t date : 23;                     /*SPI register version.*/
            uint32_t reserved28 : 4;                /*reserved*/
        };
        uint32_t val;
    } date;
} spi_mem_dev_t;

typedef volatile struct spi_dev_s
{
    union
    {
        struct
        {
            uint32_t conf_bitlen : 18; /*Define the APB cycles of  SPI_CONF state. Can be configured in CONF state.*/
            uint32_t reserved18 : 5;   /*reserved*/
            uint32_t update : 1;       /*Set this bit to synchronize SPI registers from APB clock domain into SPI module clock domain, which is only used in SPI master mode.*/
            uint32_t usr : 1;          /*User define command enable.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. Can not be changed by CONF_buf.*/
            uint32_t reserved25 : 7;   /*reserved*/
        };
        uint32_t val;
    } cmd;
    uint32_t addr;
    union
    {
        struct
        {
            uint32_t reserved0 : 3;    /*reserved*/
            uint32_t dummy_out : 1;    /*0: In the dummy phase, the FSPI bus signals are not output. 1: In the dummy phase, the FSPI bus signals are output. Can be configured in CONF state.*/
            uint32_t reserved4 : 1;    /*reserved*/
            uint32_t faddr_dual : 1;   /*Apply 2 signals during addr phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t faddr_quad : 1;   /*Apply 4 signals during addr phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t faddr_oct : 1;    /*Apply 8 signals during addr phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t fcmd_dual : 1;    /*Apply 2 signals during command phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t fcmd_quad : 1;    /*Apply 4 signals during command phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t fcmd_oct : 1;     /*Apply 8 signals during command phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t reserved11 : 3;   /*reserved*/
            uint32_t fread_dual : 1;   /*In the read operations, read-data phase apply 2 signals. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t fread_quad : 1;   /*In the read operations read-data phase apply 4 signals. 1: enable 0: disable.  Can be configured in CONF state.*/
            uint32_t fread_oct : 1;    /*In the read operations read-data phase apply 8 signals. 1: enable 0: disable.  Can be configured in CONF state.*/
            uint32_t reserved17 : 1;   /*reserved*/
            uint32_t q_pol : 1;        /*The bit is used to set MISO line polarity, 1: high 0, low. Can be configured in CONF state.*/
            uint32_t d_pol : 1;        /*The bit is used to set MOSI line polarity, 1: high 0, low. Can be configured in CONF state.*/
            uint32_t hold_pol : 1;     /*SPI_HOLD output value when SPI is idle. 1: output high, 0: output low. Can be configured in CONF state.*/
            uint32_t wp_pol : 1;       /*Write protect signal output when SPI is idle.  1: output high, 0: output low.  Can be configured in CONF state.*/
            uint32_t reserved22 : 1;   /*reserved*/
            uint32_t rd_bit_order : 2; /*In read-data (MISO) phase 1: LSB first 0: MSB first. Can be configured in CONF state.*/
            uint32_t wr_bit_order : 2; /*In command address write-data (MOSI) phases 1: LSB firs 0: MSB first. Can be configured in CONF state.*/
            uint32_t reserved27 : 5;   /*reserved*/
        };
        uint32_t val;
    } ctrl;
    union
    {
        struct
        {
            uint32_t clkcnt_l : 6;       /*In the master mode it must be equal to spi_clkcnt_N. In the slave mode it must be 0. Can be configured in CONF state.*/
            uint32_t clkcnt_h : 6;       /*In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In the slave mode it must be 0. Can be configured in CONF state.*/
            uint32_t clkcnt_n : 6;       /*In the master mode it is the divider of spi_clk. So spi_clk frequency is system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1). Can be configured in CONF state.*/
            uint32_t clkdiv_pre : 4;     /*In the master mode it is pre-divider of spi_clk.  Can be configured in CONF state.*/
            uint32_t reserved22 : 9;     /*reserved*/
            uint32_t clk_equ_sysclk : 1; /*In the master mode 1: spi_clk is equal to system 0: spi_clk is divided from system clock. Can be configured in CONF state.*/
        };
        uint32_t val;
    } clock;
    union
    {
        struct
        {
            uint32_t doutdin : 1;           /*Set the bit to enable full duplex communication. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t reserved1 : 2;         /*reserved*/
            uint32_t qpi_mode : 1;          /*Both for master mode and slave mode. 1: spi controller is in QPI mode. 0: others. Can be configured in CONF state.*/
            uint32_t opi_mode : 1;          /*Just for master mode. 1: spi controller is in OPI mode (all in 8-b-m). 0: others. Can be configured in CONF state.*/
            uint32_t tsck_i_edge : 1;       /*In the slave mode, this bit can be used to change the polarity of tsck. 0: tsck = spi_ck_i. 1:tsck = !spi_ck_i.*/
            uint32_t cs_hold : 1;           /*spi cs keep low when spi is in  done  phase. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t cs_setup : 1;          /*spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t rsck_i_edge : 1;       /*In the slave mode, this bit can be used to change the polarity of rsck. 0: rsck = !spi_ck_i. 1:rsck = spi_ck_i.*/
            uint32_t ck_out_edge : 1;       /*the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode. Can be configured in CONF state.*/
            uint32_t reserved10 : 2;        /*reserved*/
            uint32_t fwrite_dual : 1;       /*In the write operations read-data phase apply 2 signals. Can be configured in CONF state.*/
            uint32_t fwrite_quad : 1;       /*In the write operations read-data phase apply 4 signals. Can be configured in CONF state.*/
            uint32_t fwrite_oct : 1;        /*In the write operations read-data phase apply 8 signals. Can be configured in CONF state.*/
            uint32_t usr_conf_nxt : 1;      /*1: Enable the DMA CONF phase of next seg-trans operation, which means seg-trans will continue. 0: The seg-trans will end after the current SPI seg-trans or this is not seg-trans mode. Can be configured in CONF state.*/
            uint32_t reserved16 : 1;        /*reserved*/
            uint32_t sio : 1;               /*Set the bit to enable 3-line half duplex communication mosi and miso signals share the same pin. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t reserved18 : 6;        /*reserved*/
            uint32_t usr_miso_highpart : 1; /*read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t usr_mosi_highpart : 1; /*write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable.  Can be configured in CONF state.*/
            uint32_t usr_dummy_idle : 1;    /*spi clock is disable in dummy phase when the bit is enable. Can be configured in CONF state.*/
            uint32_t usr_mosi : 1;          /*This bit enable the write-data phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_miso : 1;          /*This bit enable the read-data phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_dummy : 1;         /*This bit enable the dummy phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_addr : 1;          /*This bit enable the address phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_command : 1;       /*This bit enable the command phase of an operation. Can be configured in CONF state.*/
        };
        uint32_t val;
    } user;
    union
    {
        struct
        {
            uint32_t usr_dummy_cyclelen : 8;   /*The length in spi_clk cycles of dummy phase. The register value shall be (cycle_num-1). Can be configured in CONF state.*/
            uint32_t reserved8 : 8;            /*reserved*/
            uint32_t mst_wfull_err_end_en : 1; /*1: SPI transfer is ended when SPI RX AFIFO wfull error is valid in GP-SPI master FD/HD-mode. 0: SPI transfer is not ended when SPI RX AFIFO wfull error is valid in GP-SPI master FD/HD-mode.*/
            uint32_t cs_setup_time : 5;        /*(cycles+1) of prepare phase by spi clock this bits are combined with spi_cs_setup bit. Can be configured in CONF state.*/
            uint32_t cs_hold_time : 5;         /*delay cycles of cs pin by spi clock this bits are combined with spi_cs_hold bit. Can be configured in CONF state.*/
            uint32_t usr_addr_bitlen : 5;      /*The length in bits of address phase. The register value shall be (bit_num-1). Can be configured in CONF state.*/
        };
        uint32_t val;
    } user1;
    union
    {
        struct
        {
            uint32_t usr_command_value : 16;    /*The value of  command. Can be configured in CONF state.*/
            uint32_t reserved16 : 11;           /*reserved*/
            uint32_t mst_rempty_err_end_en : 1; /*1: SPI transfer is ended when SPI TX AFIFO read empty error is valid in GP-SPI master FD/HD-mode. 0: SPI transfer is not ended when SPI TX AFIFO read empty error is valid in GP-SPI master FD/HD-mode.*/
            uint32_t usr_command_bitlen : 4;    /*The length in bits of command phase. The register value shall be (bit_num-1). Can be configured in CONF state.*/
        };
        uint32_t val;
    } user2;
    union
    {
        struct
        {
            uint32_t ms_data_bitlen : 18; /*The value of these bits is the configured SPI transmission data bit length in master mode DMA controlled transfer or CPU controlled transfer. The value is also the configured bit length in slave mode DMA RX controlled transfer. The register value shall be (bit_num-1). Can be configured in CONF state.*/
            uint32_t reserved18 : 14;     /*reserved*/
        };
        uint32_t val;
    } ms_dlen;
    union
    {
        struct
        {
            uint32_t cs0_dis : 1;           /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs1_dis : 1;           /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs2_dis : 1;           /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs3_dis : 1;           /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs4_dis : 1;           /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs5_dis : 1;           /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t ck_dis : 1;            /*1: spi clk out disable,  0: spi clk out enable. Can be configured in CONF state.*/
            uint32_t master_cs_pol : 6;     /*In the master mode the bits are the polarity of spi cs line, the value is equivalent to spi_cs ^ spi_master_cs_pol. Can be configured in CONF state.*/
            uint32_t reserved13 : 3;        /*reserved*/
            uint32_t clk_data_dtr_en : 1;   /*1: SPI master DTR mode is applied to SPI clk, data and spi_dqs.  0: SPI master DTR mode is  only applied to spi_dqs. This bit should be used with bit 17/18/19. */
            uint32_t data_dtr_en : 1;       /*1: SPI clk and data of SPI_DOUT and SPI_DIN state are in DTR mode, including master 1/2/4/8-bm.  0:  SPI clk and data of SPI_DOUT and SPI_DIN state are in STR mode. Can be configured in CONF state.*/
            uint32_t addr_dtr_en : 1;       /*1: SPI clk and data of SPI_SEND_ADDR state are in DTR mode, including master 1/2/4/8-bm.  0:  SPI clk and data of SPI_SEND_ADDR state are in STR mode. Can be configured in CONF state.*/
            uint32_t cmd_dtr_en : 1;        /*1: SPI clk and data of SPI_SEND_CMD state are in DTR mode, including master 1/2/4/8-bm. 0:  SPI clk and data of SPI_SEND_CMD state are in STR mode. Can be configured in CONF state.*/
            uint32_t reserved20 : 3;        /*reserved*/
            uint32_t slave_cs_pol : 1;      /*spi slave input cs polarity select. 1: inv  0: not change. Can be configured in CONF state.*/
            uint32_t dqs_idle_edge : 1;     /*The default value of spi_dqs. Can be configured in CONF state.*/
            uint32_t reserved25 : 4;        /*reserved*/
            uint32_t ck_idle_edge : 1;      /*1: spi clk line is high when idle     0: spi clk line is low when idle. Can be configured in CONF state.*/
            uint32_t cs_keep_active : 1;    /*spi cs line keep low when the bit is set. Can be configured in CONF state.*/
            uint32_t quad_din_pin_swap : 1; /*1: SPI quad input swap enable, swap FSPID with FSPIQ, swap FSPIWP with FSPIHD. 0:  spi quad input swap disable. Can be configured in CONF state.*/
        };
        uint32_t val;
    } misc;
    union
    {
        struct
        {
            uint32_t din0_mode : 2;          /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din1_mode : 2;          /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din2_mode : 2;          /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din3_mode : 2;          /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din4_mode : 2;          /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din5_mode : 2;          /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din6_mode : 2;          /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din7_mode : 2;          /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t timing_hclk_active : 1; /*1:enable hclk in SPI input timing module.  0: disable it. Can be configured in CONF state.*/
            uint32_t reserved17 : 15;        /*reserved*/
        };
        uint32_t val;
    } din_mode;
    union
    {
        struct
        {
            uint32_t din0_num : 2;    /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din1_num : 2;    /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din2_num : 2;    /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din3_num : 2;    /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din4_num : 2;    /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din5_num : 2;    /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din6_num : 2;    /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din7_num : 2;    /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t reserved16 : 16; /*reserved*/
        };
        uint32_t val;
    } din_num;
    union
    {
        struct
        {
            uint32_t dout0_mode : 1; /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout1_mode : 1; /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout2_mode : 1; /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout3_mode : 1; /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout4_mode : 1; /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout5_mode : 1; /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout6_mode : 1; /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout7_mode : 1; /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t d_dqs_mode : 1; /*The output signal SPI_DQS is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t reserved9 : 23; /*reserved*/
        };
        uint32_t val;
    } dout_mode;
    union
    {
        struct
        {
            uint32_t outfifo_empty : 1;       /*Records the status of DMA TX FIFO. 1: DMA TX FIFO is not ready for sending data. 0: DMA TX FIFO is ready for sending data.*/
            uint32_t infifo_full : 1;         /*Records the status of DMA RX FIFO. 1: DMA RX FIFO is not ready for receiving data. 0: DMA RX FIFO is ready for receiving data.*/
            uint32_t reserved2 : 16;          /*reserved*/
            uint32_t dma_seg_trans_en : 1;    /*Enable dma segment transfer in spi dma half slave mode. 1: enable. 0: disable.*/
            uint32_t rx_seg_trans_clr_en : 1; /*1: spi_dma_infifo_full_vld is cleared by spi slave cmd 5. 0: spi_dma_infifo_full_vld is cleared by spi_trans_done.*/
            uint32_t tx_seg_trans_clr_en : 1; /*1: spi_dma_outfifo_empty_vld is cleared by spi slave cmd 6. 0: spi_dma_outfifo_empty_vld is cleared by spi_trans_done.*/
            uint32_t rx_eof_en : 1;           /*1: spi_dma_inlink_eof is set when the number of dma pushed data bytes is equal to the value of spi_slv/mst_dma_rd_bytelen[19:0] in spi dma transition.  0: spi_dma_inlink_eof is set by spi_trans_done in non-seg-trans or spi_dma_seg_trans_done in seg-trans.*/
            uint32_t reserved22 : 5;          /*reserved*/
            uint32_t dma_rx_ena : 1;          /*Set this bit to enable SPI DMA controlled receive data mode.*/
            uint32_t dma_tx_ena : 1;          /*Set this bit to enable SPI DMA controlled send data mode.*/
            uint32_t rx_afifo_rst : 1;        /*Set this bit to reset RX AFIFO, which is used to receive data in SPI master and slave mode transfer.*/
            uint32_t buf_afifo_rst : 1;       /*Set this bit to reset BUF TX AFIFO, which is used send data out in SPI slave CPU controlled mode transfer and master mode transfer.*/
            uint32_t dma_afifo_rst : 1;       /*Set this bit to reset DMA TX AFIFO, which is used to send data out in SPI slave DMA controlled mode transfer.*/
        };
        uint32_t val;
    } dma_conf;
    union
    {
        struct
        {
            uint32_t infifo_full_err : 1;         /*The enable bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt.*/
            uint32_t outfifo_empty_err : 1;       /*The enable bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.*/
            uint32_t ex_qpi : 1;                  /*The enable bit for SPI slave Ex_QPI interrupt.*/
            uint32_t en_qpi : 1;                  /*The enable bit for SPI slave En_QPI interrupt.*/
            uint32_t cmd7 : 1;                    /*The enable bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8 : 1;                    /*The enable bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9 : 1;                    /*The enable bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda : 1;                    /*The enable bit for SPI slave CMDA interrupt.*/
            uint32_t rd_dma_done : 1;             /*The enable bit for SPI_SLV_RD_DMA_DONE_INT interrupt.*/
            uint32_t wr_dma_done : 1;             /*The enable bit for SPI_SLV_WR_DMA_DONE_INT interrupt.*/
            uint32_t rd_buf_done : 1;             /*The enable bit for SPI_SLV_RD_BUF_DONE_INT interrupt.*/
            uint32_t wr_buf_done : 1;             /*The enable bit for SPI_SLV_WR_BUF_DONE_INT interrupt.*/
            uint32_t trans_done : 1;              /*The enable bit for SPI_TRANS_DONE_INT interrupt.*/
            uint32_t dma_seg_trans_done : 1;      /*The enable bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt.*/
            uint32_t seg_magic_err : 1;           /*The enable bit for SPI_SEG_MAGIC_ERR_INT interrupt.*/
            uint32_t buf_addr_err : 1;            /*The enable bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.*/
            uint32_t cmd_err : 1;                 /*The enable bit for SPI_SLV_CMD_ERR_INT interrupt.*/
            uint32_t mst_rx_afifo_wfull_err : 1;  /*The enable bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.*/
            uint32_t mst_tx_afifo_rempty_err : 1; /*The enable bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.*/
            uint32_t app2 : 1;                    /*The enable bit for SPI_APP2_INT interrupt.*/
            uint32_t app1 : 1;                    /*The enable bit for SPI_APP1_INT interrupt.*/
            uint32_t reserved21 : 11;             /*reserved*/
        };
        uint32_t val;
    } dma_int_ena;
    union
    {
        struct
        {
            uint32_t infifo_full_err : 1;         /*The clear bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt.*/
            uint32_t outfifo_empty_err : 1;       /*The clear bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.*/
            uint32_t ex_qpi : 1;                  /*The clear bit for SPI slave Ex_QPI interrupt.*/
            uint32_t en_qpi : 1;                  /*The clear bit for SPI slave En_QPI interrupt.*/
            uint32_t cmd7 : 1;                    /*The clear bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8 : 1;                    /*The clear bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9 : 1;                    /*The clear bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda : 1;                    /*The clear bit for SPI slave CMDA interrupt.*/
            uint32_t rd_dma_done : 1;             /*The clear bit for SPI_SLV_RD_DMA_DONE_INT interrupt.*/
            uint32_t wr_dma_done : 1;             /*The clear bit for SPI_SLV_WR_DMA_DONE_INT interrupt.*/
            uint32_t rd_buf_done : 1;             /*The clear bit for SPI_SLV_RD_BUF_DONE_INT interrupt.*/
            uint32_t wr_buf_done : 1;             /*The clear bit for SPI_SLV_WR_BUF_DONE_INT interrupt.*/
            uint32_t trans_done : 1;              /*The clear bit for SPI_TRANS_DONE_INT interrupt.*/
            uint32_t dma_seg_trans_done : 1;      /*The clear bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt.*/
            uint32_t seg_magic_err : 1;           /*The clear bit for SPI_SEG_MAGIC_ERR_INT interrupt.*/
            uint32_t buf_addr_err : 1;            /*The clear bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.*/
            uint32_t cmd_err : 1;                 /*The clear bit for SPI_SLV_CMD_ERR_INT interrupt.*/
            uint32_t mst_rx_afifo_wfull_err : 1;  /*The clear bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.*/
            uint32_t mst_tx_afifo_rempty_err : 1; /*The clear bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.*/
            uint32_t app2 : 1;                    /*The clear bit for SPI_APP2_INT interrupt.*/
            uint32_t app1 : 1;                    /*The clear bit for SPI_APP1_INT interrupt.*/
            uint32_t reserved21 : 11;             /*reserved*/
        };
        uint32_t val;
    } dma_int_clr;
    union
    {
        struct
        {
            uint32_t infifo_full_err : 1;         /*1: The current data rate of DMA Rx is smaller than that of SPI, which will lose the receive data.  0: Others.  */
            uint32_t outfifo_empty_err : 1;       /*1: The current data rate of DMA TX is smaller than that of SPI. SPI will stop in master mode and send out all 0 in slave mode.  0: Others.  */
            uint32_t ex_qpi : 1;                  /*The raw bit for SPI slave Ex_QPI interrupt. 1: SPI slave mode Ex_QPI transmission is ended. 0: Others.*/
            uint32_t en_qpi : 1;                  /*The raw bit for SPI slave En_QPI interrupt. 1: SPI slave mode En_QPI transmission is ended. 0: Others.*/
            uint32_t cmd7 : 1;                    /*The raw bit for SPI slave CMD7 interrupt. 1: SPI slave mode CMD7 transmission is ended. 0: Others.*/
            uint32_t cmd8 : 1;                    /*The raw bit for SPI slave CMD8 interrupt. 1: SPI slave mode CMD8 transmission is ended. 0: Others.*/
            uint32_t cmd9 : 1;                    /*The raw bit for SPI slave CMD9 interrupt. 1: SPI slave mode CMD9 transmission is ended. 0: Others.*/
            uint32_t cmda : 1;                    /*The raw bit for SPI slave CMDA interrupt. 1: SPI slave mode CMDA transmission is ended. 0: Others.*/
            uint32_t rd_dma_done : 1;             /*The raw bit for SPI_SLV_RD_DMA_DONE_INT interrupt. 1: SPI slave mode Rd_DMA transmission is ended. 0: Others.*/
            uint32_t wr_dma_done : 1;             /*The raw bit for SPI_SLV_WR_DMA_DONE_INT interrupt. 1: SPI slave mode Wr_DMA transmission is ended. 0: Others.*/
            uint32_t rd_buf_done : 1;             /*The raw bit for SPI_SLV_RD_BUF_DONE_INT interrupt. 1: SPI slave mode Rd_BUF transmission is ended. 0: Others.*/
            uint32_t wr_buf_done : 1;             /*The raw bit for SPI_SLV_WR_BUF_DONE_INT interrupt. 1: SPI slave mode Wr_BUF transmission is ended. 0: Others.*/
            uint32_t trans_done : 1;              /*The raw bit for SPI_TRANS_DONE_INT interrupt. 1: SPI master mode transmission is ended. 0: others.*/
            uint32_t dma_seg_trans_done : 1;      /*The raw bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt. 1:  spi master DMA full-duplex/half-duplex seg-conf-trans ends or slave half-duplex seg-trans ends. And data has been pushed to corresponding memory.  0:  seg-conf-trans or seg-trans is not ended or not occurred. */
            uint32_t seg_magic_err : 1;           /*The raw bit for SPI_SEG_MAGIC_ERR_INT interrupt. 1: The magic value in CONF buffer is error in the DMA seg-conf-trans. 0: others.*/
            uint32_t buf_addr_err : 1;            /*The raw bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt. 1: The accessing data address of the current SPI slave mode CPU controlled FD, Wr_BUF or Rd_BUF transmission is bigger than 63. 0: Others.*/
            uint32_t cmd_err : 1;                 /*The raw bit for SPI_SLV_CMD_ERR_INT interrupt. 1: The slave command value in the current SPI slave HD mode transmission is not supported. 0: Others.*/
            uint32_t mst_rx_afifo_wfull_err : 1;  /*The raw bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt. 1: There is a RX AFIFO write-full error when SPI inputs data in master mode. 0: Others.*/
            uint32_t mst_tx_afifo_rempty_err : 1; /*The raw bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt. 1: There is a TX BUF AFIFO read-empty error when SPI outputs data in master mode. 0: Others.*/
            uint32_t app2 : 1;                    /*The raw bit for SPI_APP2_INT interrupt. The value is only controlled by software.*/
            uint32_t app1 : 1;                    /*The raw bit for SPI_APP1_INT interrupt. The value is only controlled by software.*/
            uint32_t reserved21 : 11;             /*reserved*/
        };
        uint32_t val;
    } dma_int_raw;
    union
    {
        struct
        {
            uint32_t infifo_full_err : 1;         /*The status bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt.*/
            uint32_t outfifo_empty_err : 1;       /*The status bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.*/
            uint32_t ex_qpi : 1;                  /*The status bit for SPI slave Ex_QPI interrupt.*/
            uint32_t en_qpi : 1;                  /*The status bit for SPI slave En_QPI interrupt.*/
            uint32_t cmd7 : 1;                    /*The status bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8 : 1;                    /*The status bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9 : 1;                    /*The status bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda : 1;                    /*The status bit for SPI slave CMDA interrupt.*/
            uint32_t rd_dma_done : 1;             /*The status bit for SPI_SLV_RD_DMA_DONE_INT interrupt.*/
            uint32_t wr_dma_done : 1;             /*The status bit for SPI_SLV_WR_DMA_DONE_INT interrupt.*/
            uint32_t rd_buf_done : 1;             /*The status bit for SPI_SLV_RD_BUF_DONE_INT interrupt.*/
            uint32_t wr_buf_done : 1;             /*The status bit for SPI_SLV_WR_BUF_DONE_INT interrupt.*/
            uint32_t trans_done : 1;              /*The status bit for SPI_TRANS_DONE_INT interrupt.*/
            uint32_t dma_seg_trans_done : 1;      /*The status bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt.*/
            uint32_t seg_magic_err : 1;           /*The status bit for SPI_SEG_MAGIC_ERR_INT interrupt.*/
            uint32_t buf_addr_err : 1;            /*The status bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.*/
            uint32_t cmd_err : 1;                 /*The status bit for SPI_SLV_CMD_ERR_INT interrupt.*/
            uint32_t mst_rx_afifo_wfull_err : 1;  /*The status bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.*/
            uint32_t mst_tx_afifo_rempty_err : 1; /*The status bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.*/
            uint32_t app2 : 1;                    /*The status bit for SPI_APP2_INT interrupt.*/
            uint32_t app1 : 1;                    /*The status bit for SPI_APP1_INT interrupt.*/
            uint32_t reserved21 : 11;             /*reserved*/
        };
        uint32_t val;
    } dma_int_st;
    union
    {
        struct
        {
            uint32_t infifo_full_err_int_set : 1;         /*The software set bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt.*/
            uint32_t outfifo_empty_err_int_set : 1;       /*The software set bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.*/
            uint32_t ex_qpi_int_set : 1;                  /*The software set bit for SPI slave Ex_QPI interrupt.*/
            uint32_t en_qpi_int_set : 1;                  /*The software set bit for SPI slave En_QPI interrupt.*/
            uint32_t cmd7_int_set : 1;                    /*The software set bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8_int_set : 1;                    /*The software set bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9_int_set : 1;                    /*The software set bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda_int_set : 1;                    /*The software set bit for SPI slave CMDA interrupt.*/
            uint32_t rd_dma_done_int_set : 1;             /*The software set bit for SPI_SLV_RD_DMA_DONE_INT interrupt.*/
            uint32_t wr_dma_done_int_set : 1;             /*The software set bit for SPI_SLV_WR_DMA_DONE_INT interrupt.*/
            uint32_t rd_buf_done_int_set : 1;             /*The software set bit for SPI_SLV_RD_BUF_DONE_INT interrupt.*/
            uint32_t wr_buf_done_int_set : 1;             /*The software set bit for SPI_SLV_WR_BUF_DONE_INT interrupt.*/
            uint32_t trans_done_int_set : 1;              /*The software set bit for SPI_TRANS_DONE_INT interrupt.*/
            uint32_t dma_seg_trans_done_int_set : 1;      /*The software set bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt.*/
            uint32_t seg_magic_err_int_set : 1;           /*The software set bit for SPI_SEG_MAGIC_ERR_INT interrupt.*/
            uint32_t buf_addr_err_int_set : 1;            /*The software set bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.*/
            uint32_t cmd_err_int_set : 1;                 /*The software set bit for SPI_SLV_CMD_ERR_INT interrupt.*/
            uint32_t mst_rx_afifo_wfull_err_int_set : 1;  /*The software set bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.*/
            uint32_t mst_tx_afifo_rempty_err_int_set : 1; /*The software set bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.*/
            uint32_t app2_int_set : 1;                    /*The software set bit for SPI_APP2_INT interrupt.*/
            uint32_t app1_int_set : 1;                    /*The software set bit for SPI_APP1_INT interrupt.*/
            uint32_t reserved21 : 11;                     /*reserved*/
        };
        uint32_t val;
    } dma_int_set;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    uint32_t reserved_5c;
    uint32_t reserved_60;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t data_buf[16]; /*SPI CPU-controlled buffer0*/
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    union
    {
        struct
        {
            uint32_t clk_mode : 2;            /*SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is always on. Can be configured in CONF state.*/
            uint32_t clk_mode_13 : 1;         /*{CPOL, CPHA},1: support spi clk mode 1 and 3, first edge output data B[0]/B[7].  0: support spi clk mode 0 and 2, first edge output data B[1]/B[6].*/
            uint32_t rsck_data_out : 1;       /*It saves half a cycle when tsck is the same as rsck. 1: output data at rsck posedge   0: output data at tsck posedge */
            uint32_t reserved4 : 4;           /*reserved*/
            uint32_t rddma_bitlen_en : 1;     /*1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length in DMA controlled mode(Rd_DMA). 0: others*/
            uint32_t wrdma_bitlen_en : 1;     /*1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data length in DMA controlled mode(Wr_DMA). 0: others*/
            uint32_t rdbuf_bitlen_en : 1;     /*1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length in CPU controlled mode(Rd_BUF). 0: others*/
            uint32_t wrbuf_bitlen_en : 1;     /*1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data length in CPU controlled mode(Wr_BUF). 0: others*/
            uint32_t reserved12 : 10;         /*reserved*/
            uint32_t dma_seg_magic_value : 4; /*The magic value of BM table in master DMA seg-trans.*/
            uint32_t slave_mode : 1;          /*Set SPI work mode. 1: slave mode 0: master mode.*/
            uint32_t soft_reset : 1;          /*Software reset enable, reset the spi clock line cs line and data lines. Can be configured in CONF state.*/
            uint32_t usr_conf : 1;            /*1: Enable the DMA CONF phase of current seg-trans operation, which means seg-trans will start. 0: This is not seg-trans mode.*/
            uint32_t reserved29 : 3;          /*reserved*/
        };
        uint32_t val;
    } slave;
    union
    {
        struct
        {
            uint32_t data_bitlen : 18; /*The transferred data bit length in SPI slave FD and HD mode. */
            uint32_t last_command : 8; /*In the slave mode it is the value of command.*/
            uint32_t last_addr : 6;    /*In the slave mode it is the value of address.*/
        };
        uint32_t val;
    } slave1;
    union
    {
        struct
        {
            uint32_t clk_en : 1;         /*Set this bit to enable clk gate*/
            uint32_t mst_clk_active : 1; /*Set this bit to power on the SPI module clock.*/
            uint32_t mst_clk_sel : 1;    /*This bit is used to select SPI module clock source in master mode. 1: PLL_CLK_80M. 0: XTAL CLK.*/
            uint32_t reserved3 : 29;     /*reserved*/
        };
        uint32_t val;
    } clk_gate;
    uint32_t reserved_ec;
    union
    {
        struct
        {
            uint32_t date : 28;      /*SPI register version.*/
            uint32_t reserved28 : 4; /*reserved*/
        };
        uint32_t val;
    } date;
} spi_dev_t;

typedef unsigned (*bootloader_flash_read_status_fn_t)(void);
typedef void (*bootloader_flash_write_status_fn_t)(unsigned);

typedef struct __attribute__((packed))
{
    const char *manufacturer;
    uint8_t mfg_id;    /* 8-bit JEDEC manufacturer ID */
    uint16_t flash_id; /* 16-bit JEDEC flash chip ID */
    uint16_t id_mask;  /* Bits to match on in flash chip ID */
    bootloader_flash_read_status_fn_t read_status_fn;
    bootloader_flash_write_status_fn_t write_status_fn;
    uint8_t status_qio_bit;
} bootloader_qio_info_t;

extern esp_rom_spiflash_legacy_data_t *rom_spiflash_legacy_data;
#define g_rom_flashchip (rom_spiflash_legacy_data->chip)
#define g_rom_spiflash_dummy_len_plus (rom_spiflash_legacy_data->dummy_len_plus)

extern uint32_t esp_rom_efuse_get_flash_gpio_info();
extern uint8_t esp_rom_efuse_get_flash_wp_gpio();
extern void esp_rom_gpio_pad_set_drv(uint32_t iopad_num, uint32_t drv);
extern uint32_t esp_rom_efuse_get_flash_gpio_info();
extern esp_rom_spiflash_result_t esp_rom_spiflash_read_status(esp_rom_spiflash_chip_t *spi, uint32_t *status);
extern esp_rom_spiflash_result_t esp_rom_spiflash_config_readmode(esp_rom_spiflash_read_mode_t mode);
extern esp_rom_spiflash_result_t esp_rom_spiflash_config_param(uint32_t deviceId, uint32_t chip_size, uint32_t block_size,
                                                               uint32_t sector_size, uint32_t page_size, uint32_t status_mask);
extern void esp_rom_spiflash_select_qio_pins(uint8_t wp_gpio_num, uint32_t spiconfig);
extern void Cache_Enable_ICache(uint32_t autoload);
extern uint32_t Cache_Disable_ICache(void);
extern void Cache_Enable_DCache(uint32_t autoload);
extern uint32_t Cache_Disable_DCache(void);

// Functions
// set bit or set bits to register
#define REG_SET_BIT(_r, _b)                                               \
    do                                                                    \
    {                                                                     \
        *(volatile uint32_t *)(_r) = (*(volatile uint32_t *)(_r)) | (_b); \
    } while (0)

// read value from register
#define READ_PERI_REG(addr) ({                         \
    (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))); \
})

// write value to register
#define WRITE_PERI_REG(addr, val)                                            \
    do                                                                       \
    {                                                                        \
        (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val); \
    } while (0)

// set bits of register controlled by mask and shift
#define SET_PERI_REG_BITS(reg, bit_map, value, shift)                                                                 \
    do                                                                                                                \
    {                                                                                                                 \
        WRITE_PERI_REG((reg), (READ_PERI_REG(reg) & (~((bit_map) << (shift)))) | (((value) & (bit_map)) << (shift))); \
    } while (0)

// set bits of register controlled by mask
#define SET_PERI_REG_MASK(reg, mask)                          \
    do                                                        \
    {                                                         \
        WRITE_PERI_REG((reg), (READ_PERI_REG(reg) | (mask))); \
    } while (0)

/**
 * @brief Macro to force a 32-bit read, modify, then write on a peripheral register
 *
 * Due to a GCC bug, the compiler may still try to optimize read/writes to peripheral register fields by using 8/16 bit
 * access, even if they are marked volatile (i.e., -fstrict-volatile-bitfields has no effect).
 *
 * For ESP chips, the peripheral bus only allows 32-bit read/writes. The following macro works around the compiler issue
 * by forcing a 32-bit read/modify/write.
 *
 * @note This macro should only be called on register fields of xxx_struct.h type headers, as it depends on the presence
 *       of a 'val' field of the register union.
 * @note Current implementation reads into a uint32_t instead of copy base_reg direclty to temp_reg. The reason being
 *       that C++ does not create a copy constructor for volatile structs.
 */
#define HAL_FORCE_MODIFY_U32_REG_FIELD(base_reg, reg_field, field_val) \
    {                                                                  \
        uint32_t temp_val = base_reg.val;                              \
        typeof(base_reg) temp_reg;                                     \
        temp_reg.val = temp_val;                                       \
        temp_reg.reg_field = (field_val);                              \
        (base_reg).val = temp_reg.val;                                 \
    }

extern spi_mem_dev_t SPIMEM1;
#define SPIMEM_LL_APB SPIMEM1

/**
* @brief Write protect signal output when SPI is idle

* @param level 1: 1: output high, 0: output low
*/
static inline void spimem_flash_ll_set_wp_level(spi_mem_dev_t *dev, bool level)
{
    dev->ctrl.wp = level;
}

/**
 * @brief Set common command related registers
 *
 * @param ctrl_reg ctrl_reg
 * @param user_reg user_reg
 * @param user1_reg user1_reg
 * @param user2_reg user2_reg
 */
static inline void spimem_flash_ll_set_common_command_register_info(spi_mem_dev_t *dev, uint32_t ctrl_reg, uint32_t user_reg, uint32_t user1_reg, uint32_t user2_reg)
{
    dev->ctrl.val = ctrl_reg;
    dev->user.val = user_reg;
    dev->user1.val = user1_reg;
    dev->user2.val = user2_reg;
}

/**
 * @brief Set common command related registers
 *
 * @param ctrl_reg ctrl_reg
 * @param user_reg user_reg
 * @param user1_reg user1_reg
 * @param user2_reg user2_reg
 */
static inline void spi_flash_ll_set_common_command_register_info(spi_mem_dev_t *dev, uint32_t ctrl_reg, uint32_t user_reg, uint32_t user1_reg, uint32_t user2_reg)
{
    dev->ctrl.val = ctrl_reg;
    dev->user.val = user_reg;
    dev->user1.val = user1_reg;
    dev->user2.val = user2_reg;
}

/**
 * @brief Get common command related registers
 *
 * @param ctrl_reg ctrl_reg
 * @param user_reg user_reg
 * @param user1_reg user1_reg
 * @param user2_reg user2_reg
 */
static inline void spi_flash_ll_get_common_command_register_info(spi_mem_dev_t *dev, uint32_t *ctrl_reg, uint32_t *user_reg, uint32_t *user1_reg, uint32_t *user2_reg)
{
    *ctrl_reg = dev->ctrl.val;
    *user_reg = dev->user.val;
    *user1_reg = dev->user1.val;
    *user2_reg = dev->user2.val;
}

/**
 * Set the command.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param command Command to send
 * @param bitlen Length of the command
 */
static inline void spi_flash_ll_set_command(spi_mem_dev_t *dev, uint8_t command, uint32_t bitlen)
{
    dev->user.usr_command = 1;
    typeof(dev->user2) user2 = {
        .usr_command_value = command,
        .usr_command_bitlen = (bitlen - 1),
    };
    dev->user2.val = user2.val;
}

/**
 * Set the address length to send, in bits. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of the address, in bits
 */
static inline void spi_flash_ll_set_addr_bitlen(spi_mem_dev_t *dev, uint32_t bitlen)
{
    dev->user1.usr_addr_bitlen = (bitlen - 1);
    dev->user.usr_addr = bitlen ? 1 : 0;
}

/**
 * Set the address to send in user mode. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param addr Address to send
 */
static inline void spi_flash_ll_set_usr_address(spi_mem_dev_t *dev, uint32_t addr, uint32_t bitlen)
{
    // The blank region should be all ones
    uint32_t padding_ones = (bitlen == 32 ? 0 : UINT32_MAX >> bitlen);
    dev->addr = (addr << (32 - bitlen)) | padding_ones;
}

/**
 * Set the length of dummy cycles.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param dummy_n Cycles of dummy phases
 */
static inline void spi_flash_ll_set_dummy(spi_mem_dev_t *dev, uint32_t dummy_n)
{
    dev->user.usr_dummy = dummy_n ? 1 : 0;
    if (dummy_n > 0)
    {
        HAL_FORCE_MODIFY_U32_REG_FIELD(dev->user1, usr_dummy_cyclelen, dummy_n - 1)
    }
}

/**
 * Set the output length, in bits (not including command, address and dummy
 * phases)
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of output, in bits.
 */
static inline void spi_flash_ll_set_mosi_bitlen(spi_dev_t *dev, uint32_t bitlen)
{
    dev->user.usr_mosi = bitlen > 0;
    if (bitlen)
    {
        dev->ms_dlen.ms_data_bitlen = bitlen - 1;
    }
}

/**
 * Set the input length, in bits.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of input, in bits.
 */
static inline void spi_flash_ll_set_miso_bitlen(spi_dev_t *dev, uint32_t bitlen)
{
    dev->user.usr_miso = bitlen > 0;
    if (bitlen)
    {
        dev->ms_dlen.ms_data_bitlen = bitlen - 1;
    }
}

/**
 * Set the data to be written in the data buffer.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param buffer Buffer holding the data
 * @param length Length of data in bytes.
 */
static inline void spi_flash_ll_set_buffer_data(spi_mem_dev_t *dev, const void *buffer, uint32_t length)
{
    // Load data registers, word at a time
    int num_words = (length + 3) / 4;
    for (int i = 0; i < num_words; i++)
    {
        uint32_t word = 0;
        uint32_t word_len = MIN(length, sizeof(word));
        memcpy(&word, buffer, word_len);
        dev->data_buf[i] = word;
        length -= word_len;
        buffer = (void *)((intptr_t)buffer + word_len);
    }
}

/**
 * Trigger a user defined transaction. All phases, including command, address, dummy, and the data phases,
 * should be configured before this is called.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param pe_ops Is page program/erase operation or not. (not used in gpspi)
 */
static inline void spi_flash_ll_user_start(spi_dev_t *dev, bool pe_ops)
{
    dev->cmd.update = 1;
    while (dev->cmd.update)
        ;
    dev->cmd.usr = 1;
}

/**
 * Check whether the previous operation is done.
 *
 * @param dev Beginning address of the peripheral registers.
 *
 * @return true if last command is done, otherwise false.
 */
static inline bool spi_flash_ll_cmd_is_done(const spi_mem_dev_t *dev)
{
    return (dev->cmd.usr == 0);
}

/**
 * Get the read data from the buffer after ``gpspi_flash_ll_read`` is done.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param buffer Buffer to hold the output data
 * @param read_len Length to get out of the buffer
 */
static inline void spi_flash_ll_get_buffer_data(spi_mem_dev_t *dev, void *buffer, uint32_t read_len)
{
    if (((intptr_t)buffer % 4 == 0) && (read_len % 4 == 0))
    {
        // If everything is word-aligned, do a faster memcpy
        memcpy(buffer, (void *)dev->data_buf, read_len);
    }
    else
    {
        // Otherwise, slow(er) path copies word by word
        uint32_t copy_len = read_len;
        for (uint32_t i = 0; i < (read_len + 3) / 4; i++)
        {
            int word_len = MIN(sizeof(uint32_t), copy_len);
            uint32_t word = dev->data_buf[i];
            memcpy(buffer, &word, word_len);
            buffer = (void *)((intptr_t)buffer + word_len);
            copy_len -= word_len;
        }
    }
}

#define SPI_ST 0x00000007
#define DR_REG_SPI2_BASE 0x60024000
#define REG_SPI_BASE(i) (((i) >= 2) ? (DR_REG_SPI2_BASE + (i - 2) * 0x1000) : (0)) // GPSPI2 and GPSPI3
#define SPI_EXT2_REG(i) (REG_SPI_BASE(i) + 0xF8)

// read value from register
#define REG_READ(_r) ({           \
    (*(volatile uint32_t *)(_r)); \
})

static void esp_rom_spiflash_wait_idle(esp_rom_spiflash_chip_t *spi)
{
    uint32_t status;
    // wait for spi control ready
    while ((REG_READ(SPI_EXT2_REG(1)) & SPI_ST))
    {
    }
    while ((REG_READ(SPI_EXT2_REG(0)) & SPI_ST))
    {
    }
    // wait for flash status ready
    esp_rom_spiflash_read_status(spi, &status);
}

unsigned bootloader_read_status_8b_rdsr(void);
unsigned bootloader_read_status_8b_rdsr2(void);
unsigned bootloader_read_status_16b_rdsr_rdsr2(void);
void bootloader_write_status_8b_wrsr(unsigned new_status);
void bootloader_write_status_8b_wrsr2(unsigned new_status);
void bootloader_write_status_16b_wrsr(unsigned new_status);
unsigned bootloader_read_status_8b_xmc25qu64a(void);
void bootloader_write_status_8b_xmc25qu64a(unsigned new_status);

/* Array of known flash chips and data to enable Quad I/O mode

   Manufacturer & flash ID can be tested by running "esptool.py
   flash_id"

   If manufacturer ID matches, and flash ID ORed with flash ID mask
   matches, enable_qio_mode() will execute "Read Cmd", test if bit
   number "QIE Bit" is set, and if not set it will call "Write Cmd"
   with this bit set.

   Searching of this table stops when the first match is found.
 */
const DRAM_ATTR bootloader_qio_info_t __attribute__((weak)) bootloader_flash_qe_support_list[] = {
    /*   Manufacturer,   mfg_id, flash_id, id mask, Read Status,                Write Status,               QIE Bit */
    {"MXIC", 0xC2, 0x2000, 0xFF00, bootloader_read_status_8b_rdsr, bootloader_write_status_8b_wrsr, 6},
    {"ISSI", 0x9D, 0x4000, 0xCF00, bootloader_read_status_8b_rdsr, bootloader_write_status_8b_wrsr, 6}, /* IDs 0x40xx, 0x70xx */
    {"WinBond", 0xEF, 0x4000, 0xFF00, bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr, 9},
    {"GD", 0xC8, 0x6000, 0xFF00, bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr, 9},
    {"XM25QU64A", 0x20, 0x3817, 0xFFFF, bootloader_read_status_8b_xmc25qu64a, bootloader_write_status_8b_xmc25qu64a, 6},
    {"TH", 0xcd, 0x6000, 0xFF00, bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr, 9},

    /* Final entry is default entry, if no other IDs have matched.

       This approach works for chips including:
       GigaDevice (mfg ID 0xC8, flash IDs including 4016),
       FM25Q32 (QOUT mode only, mfg ID 0xA1, flash IDs including 4016)
       BY25Q32 (mfg ID 0x68, flash IDs including 4016)
    */
    {NULL, 0xFF, 0xFFFF, 0xFFFF, bootloader_read_status_8b_rdsr2, bootloader_write_status_8b_wrsr2, 1},
};

/**
 * @brief Enum for user to select valid wrap size.
 */
typedef enum
{
    FLASH_WRAP_SIZE_8B = 8,
    FLASH_WRAP_SIZE_16B = 16,
    FLASH_WRAP_SIZE_32B = 32,
    FLASH_WRAP_SIZE_64B = 64,
} spi_flash_wrap_size_t;

typedef enum
{
    CACHE_TYPE_DATA,
    CACHE_TYPE_INSTRUCTION,
    CACHE_TYPE_ALL // This means both ICache and DCache will be used. On some chips, I/D are controlled by a shared Cache. Also use this enum under this condition. See `SOC_SHARED_IDCACHE_SUPPORTED`.
} cache_type_t;

/**
 * Necessary hal contexts, could be maintained by upper layer in the future
 */
typedef struct
{
    bool i_autoload_en;
    bool d_autoload_en;
#if CACHE_LL_ENABLE_DISABLE_STATE_SW
    // There's no register indicating if cache is enabled on these chips, use sw flag to save this state.
    bool i_cache_enabled;
    bool d_cache_enabled;
#endif
} cache_hal_state_t;

typedef struct
{
    cache_hal_state_t l1;
    cache_hal_state_t l2;
} cache_hal_context_t;

#define CACHE_LL_L1_ICACHE_AUTOLOAD (1 << 2)
#define CACHE_LL_L1_DCACHE_AUTOLOAD (1 << 2)
#define CACHE_LL_ID_ALL 2        // All of the caches in a type and level, make this value greater than any id
#define CACHE_LL_LEVEL_INT_MEM 0 // Cache level for accessing internal mem
#define CACHE_LL_LEVEL_EXT_MEM 1 // Cache level for accessing external mem
#define CACHE_LL_LEVEL_ALL 2     // All of the cache levels, make this value greater than any level
#define CACHE_LL_LEVEL_NUMS 1    // Number of cache levels

/**
 * @brief Probe flash wrap method
 *
 * @param flash_id Flash chip ID
 *
 * @return ESP_OK: If succeed
 */
typedef esp_err_t (*spi_flash_wrap_probe_fn_t)(uint32_t flash_id);

/**
 * @brief Set flash wrap
 *
 * @param wrap_size: wrap_size
 *
 * @return ESP_OK: If succeed
 */
typedef esp_err_t (*spi_flash_wrap_set_fn_t)(spi_flash_wrap_size_t wrap_size);

/**
 * @brief Clear flash wrap.
 *
 * @return ESP_OK: If succeed
 */
typedef esp_err_t (*spi_flash_wrap_clr_fn_t)(void);

typedef struct __attribute__((packed))
{
    const char *method;
    spi_flash_wrap_probe_fn_t probe;
    spi_flash_wrap_set_fn_t chip_wrap_set;
    spi_flash_wrap_clr_fn_t chip_wrap_clr;
} spi_flash_wrap_info_t;

// clear bit or clear bits of register
#define REG_CLR_BIT(_r, _b)                                                  \
    do                                                                       \
    {                                                                        \
        *(volatile uint32_t *)(_r) = (*(volatile uint32_t *)(_r)) & (~(_b)); \
    } while (0)

#endif /* SPI_DRV_ESP32S3_PORTED_H_INCLUDED */