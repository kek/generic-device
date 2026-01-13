# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an ESP32 firmware project using ESP-IDF (Espressif IoT Development Framework) version 5.5.2. The project targets the ESP32 chip (Xtensa architecture).

## Environment Setup

The project uses direnv for environment management. The `.envrc` file sources the ESP-IDF environment:
```bash
. ~/.espressif/v5.5.2/esp-idf/export.sh
```

Before working with this project, ensure ESP-IDF 5.5.2 is installed and the environment is loaded (direnv will handle this automatically if configured).

## Project Structure

The project follows the standard ESP-IDF structure:

- `micro.c` - Main application source code in the root directory
- `main/CMakeLists.txt` - Registers the component and references `../micro.c`
- `CMakeLists.txt` - Root build configuration using ESP-IDF's project.cmake
- `sdkconfig` - Generated configuration file for ESP32 peripherals and features
- `build/` - Generated build artifacts (not committed to git)

Note: The main source file is `micro.c` in the root directory, not in the `main/` folder. The `main/CMakeLists.txt` references it with `SRCS "../micro.c"`.

## Build Commands

All commands should be run from the project root directory.

### Configure the Project
```bash
idf.py menuconfig
```

### Build the Project
```bash
idf.py build
```

### Flash to Device
```bash
idf.py -p PORT flash
```
Replace PORT with your serial port (e.g., `/dev/ttyUSB0` on Linux, `/dev/cu.usbserial-*` on macOS, or `COM3` on Windows).

### Monitor Serial Output
```bash
idf.py -p PORT monitor
```

### Flash and Monitor (Combined)
```bash
idf.py -p PORT flash monitor
```

### Clean Build
```bash
idf.py fullclean
```

### Set Target (if changing chips)
```bash
idf.py set-target esp32
```

## Application Entry Point

The ESP-IDF framework expects a `void app_main(void)` function as the application entry point. This is defined in `micro.c:3` and is called by the framework's startup code after initialization.

## Configuration

The `sdkconfig` file is auto-generated and contains all ESP32 configuration options. To modify configuration:
1. Run `idf.py menuconfig`
2. Make changes in the TUI
3. Save and exit
4. The `sdkconfig` file will be updated

The sdkconfig includes settings for:
- Target chip (ESP32)
- Peripheral support (WiFi, Bluetooth, ADC, UART, etc.)
- FreeRTOS configuration
- Component settings
- Build optimization levels
