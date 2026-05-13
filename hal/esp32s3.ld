/*
 * ESP32-S3 Linker Script
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * Configured for wolfBoot bootloader
 */

 /* The offset between Dbus and Ibus. Used to convert between 0x403xxxxx and 0x3fcxxxxx addresses. */
iram_dram_offset = 0x6f0000;

/* These lengths can be adjusted, if necessary: */
bootloader_usable_dram_end = 0x3fce9700;
bootloader_stack_overhead = 0x2000; /* For safety margin between bootloader data section and startup stacks */
bootloader_dram_seg_len = 0x5000;
bootloader_iram_loader_seg_len = 0x10000;
bootloader_iram_seg_len = 0x3000;

/* Start of the lower region is determined by region size and the end of the higher region */
bootloader_dram_seg_end = bootloader_usable_dram_end - bootloader_stack_overhead;
bootloader_dram_seg_start = bootloader_dram_seg_end - bootloader_dram_seg_len;
bootloader_iram_loader_seg_start = bootloader_dram_seg_start - bootloader_iram_loader_seg_len + iram_dram_offset;

/*
 * Summary Table
 * Segment          | Hex Size | Decimal Size | Purpose
 * DRAM Data        | 0x5000   | 20 KB        | Variables,BSS, Heap
 * DRAM Stack Guard | 0x2000   | 8 KB         | Safety margin
 * IRAM Loader      | 0x10000  | 64 KB        | Main bootloader code
 * IRAM Extra       | 0x3000   | 12 KB        | Vectors / Fast code
 * Total 0x1A000 104 KB
 */ 

PROVIDE ( esp_rom_efuse_get_flash_gpio_info = 0x40001f74 );
PROVIDE ( esp_rom_efuse_get_flash_wp_gpio = 0x40001fa4 );
PROVIDE ( esp_rom_gpio_pad_set_drv = 0x40001b18 );
PROVIDE ( esp_rom_efuse_get_flash_gpio_info = 0x40001f74 );

MEMORY
{
    /* ESP32-S3 RAM mapping:
     * 0x3fc9_0000 - 0x3fcb_0000: Internal SRAM (128KB per core, typically use first core)
     * 0x3fcb_0000 - 0x3fce_0000: Internal SRAM (for bootloader use)
     */
    /* IRAM: Instruction RAM (for critical code) */
    IRAM (rx)      : ORIGIN = bootloader_iram_loader_seg_start, LENGTH = bootloader_iram_loader_seg_len

    /* DRAM: Data RAM */
    DRAM (rwx)       : ORIGIN = bootloader_dram_seg_start, LENGTH = bootloader_dram_seg_len
}

SECTIONS
{
    /* Code and read-only data */
    .text :
    {
        _start_text = .;
        KEEP(*(.isr_vector))
        KEEP(*(.cpu_exception_handlers))
        *(.text*)
        *(.rodata*)
        *(.init*)
        *(.fini*)
        *(.iram1 .iram1.*) /* catch stray IRAM_ATTR */
        . = ALIGN(16);
        _end_text = .;
    } > IRAM

    /* ARM unwind info (if using Xtensa GCC, this may differ) */
    .exidx :
    {
        . = ALIGN(16);
        *(.ARM.exidx*)
    } > IRAM

    /* Store location for initialized data */
    _stored_data = .;

    /* Initialized data section loaded from flash into RAM */
    .data : AT (_stored_data)
    {
        _start_data = .;
        KEEP(*(.data*))
        . = ALIGN(4);
        _end_data = .;
    } > DRAM

    /* Uninitialized data section */
    .bss (NOLOAD) :
    {
        _start_bss = .;
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _end_bss = .;
        __bss_end__ = .;
        _end = .;
    } > DRAM

    . = ALIGN(4);
}
