# Wiring Plan

This document describes the hardware connections for the ESP32 project with an ST7735 TFT display and battery voltage monitoring.

## Components

- **ESP32** development board (Xtensa architecture)
- **ST7735 TFT Display** (128x160 pixels, SPI interface)
- **Battery** with voltage divider circuit for monitoring

## Pin Assignments

### ST7735 TFT Display (HSPI)

| Display Pin | ESP32 GPIO | Function | Notes |
|-------------|------------|----------|-------|
| CS          | GPIO 15    | Chip Select | HSPI default CS pin |
| RST         | GPIO 4     | Reset | Active low |
| DC          | GPIO 18    | Data/Command | High=Data, Low=Command |
| MOSI        | GPIO 13    | Master Out Slave In | HSPI default MOSI |
| SCLK        | GPIO 14    | Serial Clock | HSPI default CLK |
| BL          | GPIO 27    | Backlight | Can be tied to 3.3V for always-on |
| VCC         | 3.3V       | Power | **Do NOT use 5V!** |
| GND         | GND        | Ground | |

### Battery Voltage Monitoring (ADC)

| Function | ESP32 GPIO | ADC Channel | Notes |
|----------|------------|-------------|-------|
| Voltage Sense | GPIO 32 | ADC1_CH4 | Through voltage divider |
| Reserved | GPIO 33 | ADC1_CH5 | Available for future analog input |

### Reserved for Audio (DAC)

| Function | ESP32 GPIO | Notes |
|----------|------------|-------|
| DAC1 | GPIO 25 | Reserved for audio output |
| DAC2 | GPIO 26 | Reserved for audio output |

### Reserved for I2C

| Function | ESP32 GPIO | Notes |
|----------|------------|-------|
| SDA | GPIO 21 | Default I2C data line |
| SCL | GPIO 22 | Default I2C clock line |

## Wiring Diagram

```
                    ESP32 DevKit
                   +------------+
                   |            |
    TFT CS    ----o| GPIO 15    |
    TFT RST   ----o| GPIO 4     |
    TFT DC    ----o| GPIO 18    |
    TFT MOSI  ----o| GPIO 13    |
    TFT SCLK  ----o| GPIO 14    |
    TFT BL    ----o| GPIO 27    |
                   |            |
   (reserved) ----o| GPIO 25    |o---- DAC1 (audio)
   (reserved) ----o| GPIO 26    |o---- DAC2 (audio)
   (reserved) ----o| GPIO 21    |o---- I2C SDA
   (reserved) ----o| GPIO 22    |o---- I2C SCL
                   |            |
    Battery   ----o| GPIO 32    |o---- Voltage Divider
    (via divider)  |            |
                   |            |
    3.3V      ----o| 3V3        |o---- TFT VCC
    GND       ----o| GND        |o---- TFT GND
                   +------------+
```

## SPI Configuration

This project uses **SPI2_HOST (HSPI)** with default pin assignments:

| HSPI Signal | Default GPIO | Used in Project |
|-------------|--------------|-----------------|
| MOSI        | GPIO 13      | ✓ GPIO 13 |
| MISO        | GPIO 12      | Not used (display is write-only) |
| SCLK        | GPIO 14      | ✓ GPIO 14 |
| CS          | GPIO 15      | ✓ GPIO 15 |

Using HSPI defaults provides:
- Better hardware optimization
- Avoids strapping pins (GPIO 5 is a strapping pin)
- Standard/conventional layout

## Voltage Divider Circuit

The battery voltage is measured through a voltage divider to scale it within the ESP32's ADC range (0-3.3V).

```
    Battery (+) ----[R1]----+----[R2]---- GND
                            |
                            +------------ GPIO 32
```

**Multiplier:** The code uses a multiplier of `3.43` to convert ADC voltage back to battery voltage. Adjust this value based on your actual resistor values:

```
Multiplier = (R1 + R2) / R2
```

**ADC Configuration:**
- ADC Unit: ADC1
- Channel: ADC_CHANNEL_4 (GPIO 32)
- Attenuation: 12dB (0-3.3V range)
- Bit Width: 12-bit (0-4095)

## Important Notes

### Power
- **Always use 3.3V** for the TFT display, not 5V
- The ESP32's ADC can only read 0-3.3V; use a voltage divider for higher voltages

### Strapping Pins to Avoid
The following GPIO pins affect boot behavior and should generally be avoided:
- GPIO 0 (Boot mode)
- GPIO 2 (Boot mode)
- GPIO 5 (SDIO timing) - **Previously used for CS, moved to GPIO 15**
- GPIO 12 (Flash voltage)
- GPIO 15 (SDIO timing) - OK for CS with proper pull configuration

### ADC Limitations
- ADC1 can be used while WiFi is active
- ADC2 cannot be used while WiFi is active
- GPIO 32 is on ADC1, so it's safe to use with WiFi

## Physical Connection Checklist

- [ ] TFT CS connected to GPIO 15
- [ ] TFT RST connected to GPIO 4
- [ ] TFT DC connected to GPIO 18
- [ ] TFT MOSI (SDA) connected to GPIO 13
- [ ] TFT SCLK (SCL) connected to GPIO 14
- [ ] TFT BL connected to GPIO 27 (or 3.3V)
- [ ] GPIO 25 and GPIO 26 left free for audio DAC
- [ ] GPIO 21 and GPIO 22 left free for I2C
- [ ] GPIO 33 left free for ADC
- [ ] TFT VCC connected to 3.3V
- [ ] TFT GND connected to GND
- [ ] Voltage divider output connected to GPIO 32
- [ ] Common ground between ESP32, display, and battery circuit

## Troubleshooting

| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| Display blank | Wrong CS/DC pins | Verify GPIO 15 (CS) and GPIO 33 (DC) |
| Display white | RST not connected | Check GPIO 4 connection |
| No backlight | BL not powered | Connect GPIO 25 or tie to 3.3V |
| Wrong colors | Init type mismatch | Try `INITR_GREENTAB` instead of `INITR_BLACKTAB` |
| ADC reads 0 | No voltage divider | Ensure divider is connected to GPIO 32 |
| ADC reads max | Divider ratio wrong | Adjust resistor values |
| Boot issues | Strapping pin conflict | Ensure GPIO 15 has proper pull-up/down |