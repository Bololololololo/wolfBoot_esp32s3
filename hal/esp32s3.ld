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
bootloader_heap_seg_len = 0x8000;    /* 32KB Heap space inserted here */
bootloader_iram_loader_seg_len = 0x10000;

/* Start of the lower region is determined by region size and the end of the higher region */
bootloader_dram_seg_end = bootloader_usable_dram_end - bootloader_stack_overhead;
bootloader_dram_seg_start = bootloader_dram_seg_end - bootloader_dram_seg_len; /* 0x3FCE4700 start */

/* Heap boundaries calculated relative to the DRAM segment start */
bootloader_heap_start = bootloader_dram_seg_start - bootloader_heap_seg_len; 
bootloader_heap_end = bootloader_dram_seg_start;

/* Adjust the IRAM loader segment to start right below the new heap region */
bootloader_iram_loader_seg_start = bootloader_heap_start - bootloader_iram_loader_seg_len + iram_dram_offset;

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
PROVIDE ( esp_rom_spiflash_read_status = 0x40000ab0 );
PROVIDE ( esp_rom_spiflash_config_readmode = 0x40000aa4 );
PROVIDE ( esp_rom_spiflash_select_qio_pins = 0x40000a68 );
PROVIDE ( esp_rom_spiflash_config_param = 0x40000a50 );
PROVIDE ( Cache_Disable_ICache = 0x4000186c );
PROVIDE ( Cache_Enable_ICache = 0x40001878 );
PROVIDE ( Cache_Disable_DCache = 0x40001884 );
PROVIDE ( Cache_Enable_DCache = 0x40001890 );

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

    RAM_HEAP (rwx)       : ORIGIN = bootloader_heap_start, LENGTH = bootloader_heap_seg_len
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

