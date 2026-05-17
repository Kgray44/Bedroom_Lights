# Bedroom LED Controller

Local-first bedroom lighting firmware for a D1 mini / ESP8266 driving a WS2812 / NeoPixel LED strip. The controller serves its own web UI, stores settings in LittleFS, supports OTA/browser update routes, and keeps LED output centralized through a frame buffer pipeline.

## Repository layout

- `firmware/BedroomLedController` - Arduino sketch and `.h` firmware modules.
- `docs/generated` - generated user guide, API reference, phase reports, troubleshooting, final report, and resource reports.
- `docs/user-phase-docs` - original phase and implementation prompts.
- `tests` - Python contract tests that guard the firmware structure and docs.

## Hardware target

- Board: D1 mini / ESP8266.
- LEDs: WS2812 / NeoPixel GRB strip.
- Default data pin: D3 / GPIO0.
- Warning: D3 / GPIO0 is boot-sensitive. If the strip or wiring pulls it low during reset, the board may fail to boot. Move the data pin in wiring and `Config.h` if boot reliability is poor.

## Features

- Local web UI with no cloud dependency.
- OTA page at `/ota` and browser update support at `/update`.
- Settings persistence in LittleFS.
- Diagnostics, power estimate, LED test tools, and resource metrics.
- Scenes, favorites, Night Guard, timers, bedtime fade, transitions, palettes, browser preview, schedule automation, scene links, backup/export/reset tools, and local API docs.

## First-time setup

1. Install the ESP8266 Arduino board package and required libraries: `Adafruit NeoPixel`, `ArduinoOTA`, and ESP8266 LittleFS support.
2. Copy `firmware/BedroomLedController/Secrets.example.h` to `firmware/BedroomLedController/Secrets.h`.
3. Edit `Secrets.h` with local Wi-Fi and OTA credentials. Do not commit `Secrets.h`.
4. Rotate any Wi-Fi or OTA credentials that were ever committed before this hardening pass.
5. Adjust `LED_COUNT` and `LED_PIN` in `firmware/BedroomLedController/Config.h` for your strip and wiring.
6. Compile/upload for the D1 mini:

```powershell
arduino-cli compile --fqbn esp8266:esp8266:d1_mini firmware/BedroomLedController
```

Run the static contract tests:

```powershell
python -m unittest discover -s tests -v
```

## Safety notes

- Do not power large LED strips through the D1 mini.
- Use a separate LED power supply sized for the strip current.
- Connect LED power ground and D1 mini ground together.
- The built-in power estimator is estimate-only. Verify current draw and heat with real hardware.
- Keep brightness conservative until power and wiring are verified.

## Resource status

The firmware currently compiles for `esp8266:esp8266:d1_mini`, but IRAM is tight. Future major expansion should move to an ESP32-S3, especially for more UI pages, sensors, richer restore workflows, or additional persistent data.

Latest hardening compile:

- RAM globals/statics: 58,948 / 80,192 bytes (73%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 587,436 / 1,048,576 bytes (56%).
- Python contract tests: `python -m unittest discover -s tests -v` passed, 124 tests.
- Physical LED behavior: not physically tested on LEDs.
- OTA upload behavior after this pass: not retested.

## Documentation

- [Final report](docs/generated/BedroomLedController_Final_Report.md)
- [User guide](docs/generated/BedroomLedController_User_Guide.md)
- [API reference](docs/generated/BedroomLedController_API_Reference.md)
- [Troubleshooting](docs/generated/BedroomLedController_Troubleshooting.md)
- [Resource and migration report](docs/generated/BedroomLedController_Resource_And_Migration_Report.md)
- [Release hardening report](docs/generated/BedroomLedController_Release_Hardening_Report.md)
