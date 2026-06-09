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
test_app_dram_len = 0x8000; /* 32 KB for data */
test_app_iram_len = 0x8000; /* 32 KB for code */

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

    /* Heap space */
    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;
        . += 0x1000; /* 4 KB heap */
        _heap_end = .;
        end = .;
        _end = .;
        __end__ = .;
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
