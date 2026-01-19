# Pin Change Plan: Switch to HSPI Defaults

## Current State
Using SPI2_HOST (HSPI) with mixed pin assignments:
- TFT_CS: GPIO 5 (VSPI default, strapping pin)
- TFT_RST: GPIO 4
- TFT_DC: GPIO 33
- TFT_MOSI: GPIO 13 (HSPI default - correct!)
- TFT_SCLK: GPIO 18 (VSPI default)
- TFT_BL: GPIO 25
- ADC: GPIO 32

## Target State
Use proper HSPI default pins:
- TFT_CS: GPIO 15 (HSPI default)
- TFT_RST: GPIO 4 (no change)
- TFT_DC: GPIO 33 (no change)
- TFT_MOSI: GPIO 13 (no change)
- TFT_SCLK: GPIO 14 (HSPI default)
- TFT_BL: GPIO 25 (no change)
- ADC: GPIO 32 (no change)

## Benefits
1. Avoids GPIO 5 strapping pin (reduces boot issue risk)
2. Uses proper HSPI clock pin (GPIO 14)
3. More standard/conventional pin layout
4. Easier for others to understand the code

## Implementation Steps

### 1. Update Pin Definitions in Code
Edit `main/st7735.h`:
- Change `TFT_CS` from GPIO 5 → GPIO 15
- Change `TFT_SCLK` from GPIO 18 → GPIO 14

### 2. Power Down ESP32
Completely disconnect power before rewiring.

### 3. Physical Rewiring
Disconnect and reconnect wires:
- **CS wire**: Move from GPIO 5 → GPIO 15
- **SCLK wire**: Move from GPIO 18 → GPIO 14

Keep all other connections the same:
- RST stays on GPIO 4
- DC stays on GPIO 33
- MOSI stays on GPIO 13
- BL stays on GPIO 25
- ADC stays on GPIO 32

### 4. Rebuild Firmware
```bash
idf.py build
```

### 5. Flash Updated Firmware
```bash
idf.py -p /dev/cu.usbserial-* flash monitor
```

### 6. Verify Display Works
Check that:
- Display initializes without errors
- Text displays correctly
- Battery voltage reading appears on screen
- No boot issues or warnings

## Rollback Plan
If issues occur:
1. Revert `main/st7735.h` pin definitions to original values
2. Rebuild and reflash
3. Move wires back to original positions

## Notes
- Only 2 pins need physical rewiring (CS and SCLK)
- Code change is simple (2 line edit)
- Test thoroughly after the change
