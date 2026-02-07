# oneTesla SD Interrupter – ESP32 Port

Port of the oneTesla SD-card interrupter from ATmega328/SdFat/parallel LCD to ESP32 (tested on ESP32 DevKit / NodeMCU-32S) with:
- I2C 16×2 LCD (addr 0x27)
- Arduino `SD` library (ESP32 SPI)
- Buttons on GPIO32/33/27
- Optical output on GPIO25
- MIDI over UART (Serial2 RX16/TX17)
- Long-press exits for Fixed and SD modes

## Features
- Play .omd files from SD card (auto-browse folders)
- Live MIDI mode (5-pin UART or USB-serial bridge)
- Fixed-frequency mode with adjustable freq and volume
- Volume bar + custom LCD characters retained from original firmware
- Long-press SELECT (~1 s) to exit Fixed mode or SD browser

## Hardware
- **MCU**: ESP32 (S3 also works; adjust USB/MIDI if needed)
- **SD card**: CS=GPIO5, SPI defaults SCK=18, MOSI=23, MISO=19
- **Buttons**: UP=32, DOWN=33, SELECT=27 (INPUT_PULLUP)
- **Optical output**: GPIO25
- **LCD**: I2C addr 0x27 (constructor set to 16×2; works on 20×4 too)
- **MIDI (UART)**: RX=16, TX=17 @ 31,250 baud

## Build & Upload (Arduino IDE)
1. Board: **NodeMCU-32S** (or ESP32 Dev Module)
2. Libraries: `LiquidCrystal_I2C` (common fork), built-in `SD`, `SPI`, `Wire`
3. In `main.ino`, adjust LCD size if using 20×4: `LiquidCrystal_I2C lcd(..., 20, 4);`
4. Select correct COM port, Upload.

## Controls
- **Main menu**: UP/DOWN to navigate; SELECT to enter
- **Fixed mode**: short SELECT toggles freq/volume adjust; hold SELECT ~1 s to exit
- **Live mode**: SELECT exits
- **SD browser**: UP/DOWN to navigate; SELECT opens folder or plays file; hold SELECT ~1 s to exit to menu
- **Playback**: SELECT stops; UP/DOWN adjust volume bar

## File format
- Expects `.omd` files (same as original firmware). Copy to SD root or folders; non-`.omd` files are ignored.

## Differences from original
- AVR timers replaced with ESP32 `esp_timer`
- SdFat → Arduino SD
- Parallel LCD → I2C LCD
- Added long-press exits for SD/Fixed modes
- Pots optional: freq=ADC34, PW=ADC35 (scale to 0–255); stub returns 0 if unused

## Pin map (summary)

- Configurable pin map via `constants.h`
 Default Pin map:
| Function | Pin |
| --- | --- |
| SD CS | GPIO5 |
| SCK/MOSI/MISO | 18 / 23 / 19 |
| LCD SDA/SCL | 21 / 22 (ESP32 defaults) |
| Buttons UP/DOWN/SELECT | 32 / 33 / 27 |
| Optical output | 25 |
| MIDI RX/TX | 16 / 17 |

## License
Original code © 2015 oneTesla, LLC (see original README/license). This port modifies only hardware-abstraction layers; comply with the original license when publishing.


