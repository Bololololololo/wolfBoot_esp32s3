/* ESP32-S3 Test App Linker Script
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * Configured for wolfBoot test application on ESP32-S3
 */

/* The offset between Dbus and Ibus. Used to convert between 0x403xxxxx and 0x3fcxxxxx addresses. */
iram_dram_offset = 0x6f0000;

/* Partition layout for test app */
test_app_dram_end = 0x3fcd0000;
test_app_stack_overhead = 0x1000;
test_app_dram_len = 0x4000; /* 16 KB for test app data */
test_app_iram_len = 0x4000; /* 16 KB for test app code */

test_app_dram_end_adjusted = test_app_dram_end - test_app_stack_overhead;
test_app_dram_start = test_app_dram_end_adjusted - test_app_dram_len;
test_app_iram_start = test_app_dram_start - test_app_iram_len + iram_dram_offset;

MEMORY
{
    /* IRAM: Instruction RAM (for application code) */
    IRAM (rx)       : ORIGIN = test_app_iram_start, LENGTH = test_app_iram_len

    /* DRAM: Data RAM (for variables and heap) */
    DRAM (rwx)      : ORIGIN = test_app_dram_start, LENGTH = test_app_dram_len
}

SECTIONS
{
    .text :
    {
        _start_text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.init)
        *(.fini)
        . = ALIGN(16);
        KEEP(*(.rodata*))
        . = ALIGN(16);
        _end_text = .;
    } > IRAM

    /* ARM unwind sections (Xtensa may use these) */
    .exidx :
    {
        . = ALIGN(16);
        *(.ARM.exidx*)
        . = ALIGN(16);
    } > IRAM

    /* Store location for initialized data */
    _stored_data = .;

    /* Initialized data section */
    .data : AT (_stored_data)
    {
        . = ALIGN(4);
        _data_start = .;
        *(.data*)
        . = ALIGN(4);
        _data_end = .;
    } > DRAM

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > DRAM

    /* Heap space */
    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;
        . += 0x1000; /* 4 KB heap */
        _heap_end = .;
    } > DRAM

    /* Stack space */
    .stack :
    {
        . = ALIGN(4);
        _stack_start = .;
        . += 0x1000; /* 4 KB stack */
        _stack_end = .;
    } > DRAM
}
