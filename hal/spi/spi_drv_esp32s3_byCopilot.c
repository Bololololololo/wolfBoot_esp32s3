/* spi_drv_esp32s3.c
 *
 * GPIO-based SPI backend for external flash on ESP32-S3.
 *
 * Uses GPIO bit-banging through the ESP32-S3 GPIO and IOMUX registers.
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

#include <stdint.h>
#include "spi_drv.h"
#include "hal/spi/spi_drv_esp32s3.h"

static uint8_t spi_last_rx;
static int spi_polarity;
static int spi_phase;

static inline void write_reg(uint32_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

static inline uint32_t read_reg(uint32_t addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void gpio_set(int pin)
{
    write_reg(GPIO_OUT_W1TS_REG, 1U << pin);
}

static inline void gpio_clear(int pin)
{
    write_reg(GPIO_OUT_W1TC_REG, 1U << pin);
}

static inline void gpio_enable_output(int pin)
{
    write_reg(GPIO_ENABLE_W1TS_REG, 1U << pin);
}

static inline void gpio_disable_output(int pin)
{
    write_reg(GPIO_ENABLE_W1TC_REG, 1U << pin);
}

static inline uint32_t gpio_read(void)
{
    return read_reg(GPIO_IN_REG);
}

static inline void spi_pin_config(int pin, int output, int initial_high)
{
    uint32_t pin_reg = IO_MUX_GPIO0_REG + ((uint32_t)pin * 4U);

    PIN_PULLUP_DIS(pin_reg);
    PIN_PULLDWN_DIS(pin_reg);
    PIN_FUNC_SELECT(pin_reg, PIN_FUNC_GPIO);
    PIN_INPUT_ENABLE(pin_reg);

    if (output)
    {
        if (initial_high)
            gpio_set(pin);
        else
            gpio_clear(pin);
        gpio_enable_output(pin);
    }
    else
    {
        gpio_disable_output(pin);
    }
}

static inline void spi_delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 16; i++)
    {
        __asm__("nop");
    }
}

static inline void spi_clock_idle(void)
{
    if (spi_polarity)
        gpio_set(SPI_CLOCK_PIN);
    else
        gpio_clear(SPI_CLOCK_PIN);
}

static inline void spi_clock_assert(void)
{
    if (spi_polarity)
        gpio_clear(SPI_CLOCK_PIN);
    else
        gpio_set(SPI_CLOCK_PIN);
}

static inline void spi_clock_deassert(void)
{
    if (spi_polarity)
        gpio_set(SPI_CLOCK_PIN);
    else
        gpio_clear(SPI_CLOCK_PIN);
}

#if defined(SPI_FLASH) || defined(WOLFBOOT_TPM)

void spi_init(int polarity, int phase)
{
    static int initialized = 0;

    if (initialized)
        return;

    initialized = 1;
    spi_polarity = polarity;
    spi_phase = phase;

    spi_pin_config(SPI_CS_FLASH, 1, 1);
    spi_pin_config(SPI_CLOCK_PIN, 1, 0);
    spi_pin_config(SPI_MOSI_PIN, 1, 0);
    spi_pin_config(SPI_MISO_PIN, 0, 0);

    spi_clock_idle();
    gpio_set(SPI_CS_FLASH);
}

void spi_release(void)
{
    gpio_disable_output(SPI_CS_FLASH);
    gpio_disable_output(SPI_CLOCK_PIN);
    gpio_disable_output(SPI_MOSI_PIN);
}

void spi_cs_off(uint32_t base, int pin)
{
    (void)base;
    gpio_set(pin);
}

void spi_cs_on(uint32_t base, int pin)
{
    (void)base;
    gpio_clear(pin);
}

uint8_t spi_read(void)
{
    return spi_last_rx;
}

void spi_write(const char byte)
{
    uint8_t rx = 0;
    int bit;

    for (bit = 7; bit >= 0; bit--)
    {
        if (spi_phase == 0)
        {
            if ((byte >> bit) & 0x01U)
                gpio_set(SPI_MOSI_PIN);
            else
                gpio_clear(SPI_MOSI_PIN);

            spi_delay();
            spi_clock_assert();
            spi_delay();

            rx = (uint8_t)((rx << 1) | ((gpio_read() >> SPI_MISO_PIN) & 0x01U));
            spi_clock_deassert();
            spi_delay();
        }
        else
        {
            spi_clock_assert();
            spi_delay();

            if ((byte >> bit) & 0x01U)
                gpio_set(SPI_MOSI_PIN);
            else
                gpio_clear(SPI_MOSI_PIN);

            spi_delay();
            spi_clock_deassert();
            spi_delay();

            rx = (uint8_t)((rx << 1) | ((gpio_read() >> SPI_MISO_PIN) & 0x01U));
        }
    }

    spi_last_rx = rx;
}

#endif /* SPI_FLASH || WOLFBOOT_TPM */
