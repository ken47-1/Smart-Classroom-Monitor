# Smart Classroom Comfort Monitor

Firmware for an ESP32-based environmental monitoring system that measures temperature, humidity, pressure, and light levels, calculates a comfort score, and displays the results on a TFT screen.

This is a student project developed for a PBL (Project-Based Learning) course at school. The system uses an ESP32 microcontroller with BME280 and BH1750 sensors to provide real-time environmental feedback in a classroom setting.

## Hardware

- **MCU**: ESP32 (any variant with sufficient RAM and flash)
- **Display**: TFT screen (320x240) with SPI interface, driven by TFT_eSPI library
- **Sensors**:
  - BME280: temperature, humidity, pressure (I2C, address 0x76)
  - BH1750: ambient light (I2C, address 0x23)
- **I2C Buses**: Two independent I2C buses to prevent sensor conflicts

## Features

- Real-time measurement of temperature, humidity, pressure, and light
- Comfort score calculation with configurable weights and thresholds
- Individual metric scores for temperature, humidity, and light
- TFT display with sliced rendering for smooth updates
- Fallback values when sensors are unavailable
- Splash screen with boot status messages
- Configurable update interval (1 second default)

## Quick Start

### 1. Install Dependencies

In your Arduino IDE or PlatformIO project, install these libraries:

- TFT_eSPI
- Adafruit_BME280
- BH1750 (by Christopher Laws)

### 2. Configure TFT_eSPI

Edit the `User_Setup.h` file for TFT_eSPI to match your display hardware. The pin definitions in `HardwareConfig.h` are for reference only — the actual pins are configured in `User_Setup.h`.

### 3. Upload

```bash
# PlatformIO example
pio run -t upload
```

### 4. Monitor

```
pio device monitor -b 115200
```

## Pin Configuration

### I2C Buses

| Component | SCL | SDA | Address |
|---|---|---|---|
| BME280 | 22 | 21 | 0x76 |
| BH1750 | 17 | 16 | 0x23 |

### Display (Configured in User_Setup.h)

| Signal | Pin |
|---|---|
| MISO | 19 |
| MOSI | 23 |
| SCLK | 18 |
| CS | 15 |
| DC | 2 |
| RST | 4 |
| BL | 5 |

## Configuration

All settings are in `include/config/`:

| File | Purpose |
|---|---|
| `Config.h` | Weights, thresholds, timing, display settings |
| `HardwareConfig.h` | Pin assignments, I2C addresses |
| `DebugConfig.h` | Debug output toggles |

### Comfort Score Weights

| Metric | Weight |
|---|---|
| Temperature | 40% |
| Humidity | 40% |
| Light | 20% |

### Ideal Ranges

| Metric | Ideal Low | Ideal High | Max Difference |
|---|---|---|---|
| Temperature | 23°C | 27°C | 10°C |
| Humidity | 40% | 60% | 40% |
| Light | 300 lx | 500 lx | 400 lx |

### Comfort Levels

| Score Range | Status | Display Color |
|---|---|---|
| >= 90% | Excellent | Green |
| >= 75% | Comfortable | Green |
| >= 60% | Fair | Yellow |
| >= 40% | Poor | Orange |
| < 40% | Uncomfortable | Red |

### Display Settings

| Setting | Value |
|---|---|
| Screen Resolution | 320x240 |
| Backlight Brightness | 160 (0-255) |
| Update Interval | 1000 ms |

## Project Structure

```
Smart-Classroom-Comfort-Monitor/
├── include/
│   ├── config/
│   │   ├── Config.h              # Software behavior settings
│   │   ├── HardwareConfig.h      # Pin assignments and addresses
│   │   └── DebugConfig.h         # Debug output toggles
│   ├── display/
│   │   ├── display_manager.h     # Display hardware abstraction
│   │   └── ui.h                  # UI rendering logic
│   ├── sensors/
│   │   ├── bme280.h              # BME280 sensor interface
│   │   └── bh1750.h              # BH1750 sensor interface
│   └── comfort_score.h           # Comfort score calculation API
├── src/
│   ├── display/
│   │   ├── display_manager.cpp
│   │   └── ui.cpp
│   ├── sensors/
│   │   ├── bme280.cpp
│   │   └── bh1750.cpp
│   ├── comfort_score.cpp
│   └── main.cpp
└── README.md
```

## Safety Features

- **Sensor Fallbacks**: If a sensor is unavailable, the system uses fallback values and continues operating
- **Independent I2C Buses**: Separate buses for each sensor prevent bus contention
- **Non-blocking Loop**: Uses `millis()` for timing, never calls `delay()`

## Debugging

Enable debug output in `DebugConfig.h`:

```cpp
#define DEBUG_ENABLED  1

#if DEBUG_ENABLED
    #define DEBUG_FRAMEBUFFER_DUMP  1  // Dump framebuffer to serial
    #define DEBUG_DUMP_EVERY_N      1  // Dump every N frames
#endif
```

Set the serial baud rate to 921600. The device sends raw RGB565 pixel data over serial.

### Capture Script

Run `screenshot_capture.py` to receive the data and save it as `full_ui.png`:

```bash
python screenshot_capture.py
```

The script:

1. Opens the serial port (default: COM5)
2. Reads the framebuffer dump protocol
3. Reassembles the screen slices
4. Saves the image to `full_ui.png`

Edit `PORT` in the script to match your device's serial port.

## Troubleshooting

**Display is blank**

- Check TFT backlight connection and `BACKLIGHT_BRIGHTNESS` setting
- Verify TFT_eSPI `User_Setup.h` pin configuration
- Ensure `display.begin()` completes successfully

**No sensor data**

- Check I2C wiring (SCL, SDA, power, ground)
- Verify addresses in `HardwareConfig.h`
- Monitor serial output for sensor initialization status

**Comfort score is always low**

- Check ideal ranges in `Config.h`
- Verify sensor readings are valid
- Adjust weights if needed

**Compilation errors**

- Install all required libraries
- Check TFT_eSPI `User_Setup.h` for your display
- Ensure your ESP32 board is selected in the IDE

## For Developers

This project enforces a strict code layout standard documented in [`docs/Code_Layout_Standard.md`](docs/Code_Layout_Standard.md). Key rules:

- One logical module per file
- Headers declare public API only — no implementation
- Source files contain all implementation and internal state
- Comment hierarchy: T1 (file header) → T7 (inline notes)

When contributing, follow the visual hierarchy scale defined in the standard document.

## License

MIT