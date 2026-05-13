#ifndef SPI_DRV_ESP32S3_PORTED_H_INCLUDED
#define SPI_DRV_ESP32S3_PORTED_H_INCLUDED

#define STR(x) #x
#define XSTR(x) STR(x)
#define IRAM_ATTR __attribute__((section(".iram1." XSTR(__COUNTER__))))

#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr) (addr)

#define BIT(nr) (1UL << (nr))

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

#endif /* SPI_DRV_ESP32S3_PORTED_H_INCLUDED */