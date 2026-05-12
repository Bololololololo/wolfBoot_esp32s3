/* spi_drv_esp32s3.h
 *
 * GPIO-based SPI backend for external flash on ESP32-S3.
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
 */

#ifndef SPI_DRV_ESP32S3_H_INCLUDED
#define SPI_DRV_ESP32S3_H_INCLUDED

#include <stdint.h>

#define DR_REG_GPIO_BASE      0x60004000U
#define REG_IO_MUX_BASE       0x60009000U
#define PERIPHS_IO_MUX_GPIO0_U (REG_IO_MUX_BASE + 0x04U)
#define IO_MUX_GPIO0_REG      PERIPHS_IO_MUX_GPIO0_U

#define GPIO_OUT_W1TS_REG     (DR_REG_GPIO_BASE + 0x08U)
#define GPIO_OUT_W1TC_REG     (DR_REG_GPIO_BASE + 0x0CU)
#define GPIO_ENABLE_W1TS_REG  (DR_REG_GPIO_BASE + 0x24U)
#define GPIO_ENABLE_W1TC_REG  (DR_REG_GPIO_BASE + 0x28U)
#define GPIO_IN_REG           (DR_REG_GPIO_BASE + 0x3CU)

#define BIT(n)               (1U << (n))

#define FUN_PD               BIT(7)
#define FUN_PU               BIT(8)
#define FUN_IE               BIT(9)
#define MCU_SEL_S            12U
#define MCU_SEL_V            0x7U
#define MCU_SEL_M            (MCU_SEL_V << MCU_SEL_S)

#define PIN_INPUT_ENABLE(PIN_NAME)      (*(volatile uint32_t *)(PIN_NAME) |= FUN_IE)
#define PIN_PULLUP_DIS(PIN_NAME)        (*(volatile uint32_t *)(PIN_NAME) &= ~FUN_PU)
#define PIN_PULLDWN_DIS(PIN_NAME)       (*(volatile uint32_t *)(PIN_NAME) &= ~FUN_PD)
#define PIN_FUNC_SELECT(PIN_NAME, FUNC)  do {                                        \
    volatile uint32_t* _reg = (volatile uint32_t *)(PIN_NAME);                     \
    uint32_t _val = *_reg;                                                         \
    _val &= ~MCU_SEL_M;                                                            \
    _val |= (((uint32_t)(FUNC) << MCU_SEL_S) & MCU_SEL_M);                         \
    *_reg = _val;                                                                  \
} while (0)

#define PIN_FUNC_GPIO        1U

#define SPI_CS_FLASH         29
#ifndef SPI_CS_PIO_BASE
#define SPI_CS_PIO_BASE      0UL
#endif

#define SPI_CLOCK_PIN        30
#define SPI_MISO_PIN         31
#define SPI_MOSI_PIN         32

#endif /* SPI_DRV_ESP32S3_H_INCLUDED */
