# ESP32-S3 HAL Development Guide

This document describes the development of Hardware Abstraction Layer (HAL) support for the Espressif ESP32-S3 microcontroller in wolfBoot.

## Overview

The ESP32-S3 is a dual-core Xtensa processor with 240 MHz clock speed, 8 MB PSRAM, and flexible flash support. The wolfBoot HAL implementation for ESP32-S3 provides:

- Flash write/erase operations via ROM functions or direct SPI interface
- UART debugging support
- Basic clock and peripheral initialization
- Bootloader partition management

## Architecture

The ESP32-S3 HAL support consists of the following components:

### 1. HAL Implementation (`hal/esp32s3.c`)

Provides core HAL functions:

- **`hal_init()`**: Initialize clocks, UART, and peripheral services
- **`hal_flash_write()`**: Write data to flash memory (page-based, 256 bytes/operation)
- **`hal_flash_erase()`**: Erase flash sectors (4 KB minimum)
- **`hal_flash_unlock()`** / **`hal_flash_lock()`**: Flash access control (not implemented on ESP32-S3)
- **`hal_prepare_boot()`**: Cleanup operations before handing control to application
- **UART functions** (with `DEBUG_UART` enabled): Basic serial I/O for debugging

### 2. Linker Script (`hal/esp32s3.ld`)

Defines memory layout:

```
FLASH (rx):  0x00000000 - Bootloader partition
RAM (rwx):   0x3fcb0000 - 128 KB internal SRAM
```

### 3. CMake Configuration

Registered in `CMakeLists.txt`:

- Added to `ARM_TARGETS` list
- Flash offset configured as `0x00000000`
- Preset configuration in `CMakePresets.json`

## Memory Map

```
ESP32-S3 Flash:
0x0000_0000 - 0x0000_1000    Bootloader header
0x0000_1000 - 0x0000_8000    Bootloader
0x0000_8000 - 0x0000_9000    Partition table
0x0001_0000 - 0x0011_0000    OTA_0 (wolfBoot app, 1 MB)
0x0011_0000 - 0x0021_0000    OTA_1 (user app, 1 MB)
0x0021_0000 - ...             Swap partition

ESP32-S3 RAM:
0x3fc9_0000 - 0x3fcb_0000    Core 0/1 SRAM
0x3fcb_0000 - 0x3fce_0000    General SRAM (bootloader use)
0x3f000_000 - 0x3f400_000    Peripheral registers
```

## Building with ESP32-S3 Preset

### Using CMake with Preset

```bash
cd wolfBoot
mkdir build-esp32s3
cd build-esp32s3

# Using the provided preset
cmake --preset esp32s3 ..

# Build
cmake --build .
```

### Custom Configuration

```bash
cmake -DWOLFBOOT_TARGET=esp32s3 \
      -DWOLFBOOT_SECTOR_SIZE=0x1000 \
      -DWOLFBOOT_PARTITION_SIZE=0x8000 \
      -DWOLFBOOT_PARTITION_BOOT_ADDRESS=0x0000 \
      -DWOLFBOOT_PARTITION_UPDATE_ADDRESS=0x100000 \
      -DWOLFBOOT_PARTITION_SWAP_ADDRESS=0x1f0000 \
      -DSIGN=ED25519 \
      -DHASH=SHA256 \
      -DSPMATH=ON \
      -DSKIP_CPU_FEATURE_DETECTION=ON \
      ..
```

## Configuration Options

Key parameters for ESP32-S3 builds:

| Option | Value | Notes |
|--------|-------|-------|
| `WOLFBOOT_TARGET` | `esp32s3` | Target platform |
| `WOLFBOOT_SECTOR_SIZE` | `0x1000` (4 KB) | Flash sector size |
| `WOLFBOOT_PARTITION_SIZE` | `0x8000` | Bootloader size (32 KB) |
| `WOLFBOOT_PARTITION_BOOT_ADDRESS` | `0x0000` | Bootloader start address |
| `WOLFBOOT_PARTITION_UPDATE_ADDRESS` | `0x100000` | Firmware update partition |
| `WOLFBOOT_PARTITION_SWAP_ADDRESS` | `0x1f0000` | Swap partition for updates |
| `SIGN` | `ED25519`, `ECC256`, etc. | Signature algorithm |
| `HASH` | `SHA256` | Hash algorithm |
| `DEBUG_UART` | `yes`/`no` | Enable UART debugging |
| `SPMATH` | `ON` | Use SP Math library |
| `NO_MPU` | `ON` | ESP32-S3 has no MPU |

## Flash Operations

### Writing to Flash

The ESP32-S3 supports flash write via:

1. **Page Program** (256 bytes maximum per operation)
   - Set write enable
   - Load address and data
   - Execute page program command
   - Wait for completion

2. **Sector Erase** (4 KB minimum)
   - Set write enable
   - Issue sector erase command
   - Wait for completion

### Implementation Notes

The current HAL implementation uses direct SPI register access. For production use, consider:

1. Using Espressif's ROM functions (available at specific addresses)
2. Integrating with ESP-IDF flash HAL
3. Adding support for encrypted partitions
4. Implementing wear-leveling for better flash longevity

## UART Debugging

When compiled with `DEBUG_UART=yes`:

- UART0 at default pins (GPIO 43/44 on most ESP32-S3 boards)
- 115200 baud rate (standard)
- Useful for bootloader tracing and diagnostics

## Compiling and Flashing

### Prerequisites

- GCC for Xtensa (`xtensa-esp-elf-gcc`)
- Esptool.py or ESP-IDF tools
- CMake 3.16+

### Build Steps

```bash
# Configure
cmake --preset esp32s3 -B build-esp32s3

# Build
cd build-esp32s3
cmake --build .

# Output files
# - wolfboot_esp32s3.bin: Main bootloader
# - image_v1_signed.bin: Application image
```

### Flashing to Device

```bash
# Using esptool.py
esptool.py -p /dev/ttyUSB0 -b 460800 write_flash 0x0 build-esp32s3/wolfboot_esp32s3.bin

# Or using ESP-IDF
idf.py -p /dev/ttyUSB0 write_flash 0x0 build-esp32s3/wolfboot_esp32s3.bin
```

## Testing

### Verification Steps

1. Build the bootloader
2. Build test application
3. Create signed image
4. Flash bootloader and image
5. Monitor serial output for boot trace

### Debug Output

With `DEBUG_UART` enabled, expect output like:
```
[WolfBoot] Starting bootloader
[WolfBoot] Validating primary partition
[WolfBoot] Image OK, booting application...
```

## Known Limitations

1. **ROM Function Dependencies**: Current implementation uses direct register access; production versions should use ROM functions
2. **No Flash Encryption**: Hardware encryption not yet integrated
3. **No OTA Management**: Basic partition support only
4. **No Wear Leveling**: Direct sector access without wear tracking
5. **Single-core**: No multi-core boot coordination

## Future Enhancements

1. Integrate ESP-IDF flash HAL for better reliability
2. Add support for encrypted partitions
3. Implement firmware rollback protection
4. Add OTA update manager
5. Xtensa-specific optimizations
6. Support for secure boot (Secure Boot Enabled)
7. Add measured boot for TCB attestation

## References

- [ESP32-S3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)
- [wolfBoot Documentation](../README.md)
- [Xtensa ISA Manual](https://www.cadence.com/content/dam/cadence-www/global/en_US/pdf/whitepapers/IdeosXtensaXtensaISAandArchitectureOverview-2008.pdf)

## Support

For issues or questions regarding ESP32-S3 HAL support:

1. Check wolfBoot documentation
2. Review ESP-IDF examples
3. Consult Espressif support resources
4. File issues on wolfBoot repository
