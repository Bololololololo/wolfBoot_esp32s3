/* app_esp32s3.c
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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "target.h"
#include "wolfboot/wolfboot.h"

#ifdef TARGET_esp32s3

/* ESP32-S3 GPIO definitions */
#define GPIO_OUT_REG            0x60004004
#define GPIO_OUT_W1TS_REG       0x60004008  /* GPIO output set */
#define GPIO_OUT_W1TC_REG       0x6000400c  /* GPIO output clear */

/* LED is typically on GPIO 13 or GPIO 2, adjust as needed for your board */
#ifndef USER_LED_PIN
#define USER_LED_PIN 13
#endif

static void gpio_toggle(uint32_t pin)
{
    uint32_t pin_mask = (1 << pin);
    /* Toggle the pin by reading current state and writing opposite */
    uint32_t *gpio_out = (uint32_t *)GPIO_OUT_REG;
    uint32_t current = *gpio_out;
    
    if (current & pin_mask) {
        /* Pin is set, clear it */
        *(uint32_t *)GPIO_OUT_W1TC_REG = pin_mask;
    } else {
        /* Pin is clear, set it */
        *(uint32_t *)GPIO_OUT_W1TS_REG = pin_mask;
    }
}

static void delay_ms(uint32_t ms)
{
    /* Rough delay - adjust based on actual clock frequency */
    for (uint32_t i = 0; i < ms * 1000; i++) {
        asm volatile ("nop");
    }
}

void main(void)
{
    uint32_t version = 0;
    uint8_t *v_array = (uint8_t *)&version;
    int i;

    /* Get current firmware version */
    version = wolfBoot_current_firmware_version();

    /* Main loop - blink LED and get version */
    while (1) {
        gpio_toggle(USER_LED_PIN);
        delay_ms(500);
    }
}

#endif /* TARGET_esp32s3 */
