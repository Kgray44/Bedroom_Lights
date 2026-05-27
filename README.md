# Bedroom LED Controller

Local-first bedroom lighting firmware for an ESP32-S3 or D1 mini / ESP8266 driving a WS2812-compatible addressable strip. The controller serves its own web UI, stores settings in LittleFS, supports OTA/browser update routes where the board core supports them, and keeps LED output centralized through a frame buffer pipeline.

## Repository layout

- `firmware/BedroomLedController` - Arduino sketch and `.h` firmware modules.
- `docs/generated` - generated user guide, API reference, phase reports, troubleshooting, final report, and resource reports.
- `docs/user-phase-docs` - original phase and implementation prompts.
- `tests` - Python contract tests that guard the firmware structure and docs.

## Hardware target

- Primary board profile: ESP32-S3-N16R8 development board class.
- Backward-compatible board path: D1 mini / ESP8266 remains wrapped with `ESP8266` conditionals.
- Default strip profile: BTF FCOB 5V 160LED/m 5m, 800 addressable ICs.
- Configured strip length: 5000 mm.
- Configured LED density: 160 LEDs/m by default; change `LED_DENSITY_PER_METER` to 60, 180, or another real strip density as needed.
- LED count: calculated from length and density unless `LED_COUNT_OVERRIDE` is nonzero. The current override is 800 because the listing reports 800 ICs.
- Default data pin: GPIO5. Confirm your ESP32-S3 board pinout and wiring before upload.
- Avoid boot/strapping pins unless you intentionally handle their reset behavior.

## Features

- Local web UI with no cloud dependency.
- OTA page at `/ota` and browser update support at `/update`.
- Settings persistence in LittleFS.
- Diagnostics, power estimate, LED test tools, and resource metrics.
- Scenes, favorites, Night Guard, timers, bedtime fade, transitions, palettes, browser preview, schedule automation, scene links, backup/export/reset tools, and local API docs.

## First-time setup

1. Install the ESP32 or ESP8266 Arduino board package and required libraries: `Adafruit NeoPixel`, `ArduinoOTA`, and LittleFS support for the selected board core.
2. Copy `firmware/BedroomLedController/Secrets.example.h` to `firmware/BedroomLedController/Secrets.h`.
3. Edit `Secrets.h` with local Wi-Fi and OTA credentials. Do not commit `Secrets.h`.
4. Rotate any Wi-Fi or OTA credentials that were ever committed before this hardening pass.
5. Adjust `LED_DATA_PIN`, `STRIP_LENGTH_MM`, `LED_DENSITY_PER_METER`, and `LED_COUNT_OVERRIDE` in `firmware/BedroomLedController/HardwareProfile.h` for your strip and wiring.
6. Compile/upload for the ESP32-S3 board selected in Arduino IDE, or compile the legacy D1 mini path:

```powershell
arduino-cli compile --fqbn esp8266:esp8266:d1_mini firmware/BedroomLedController
```

Run the static contract tests:

```powershell
python -m unittest discover -s tests -v
```

## Safety notes

- Do not power the LED strip through the ESP32-S3 or D1 mini.
- Use a separate LED power supply sized for the strip current.
- Connect LED power ground and controller ground together.
- Use a 5V-safe data level shifter if the strip is unreliable from 3.3V data.
- Start with a conservative brightness cap such as 30-60 / 255.
- The built-in power estimator is estimate-only and follows the active hardware profile. Verify current draw, voltage drop, injection, and heat with real hardware.
- The BTF FCOB profile defaults to the listing-style 18.75 mA/pixel estimate. Use a conservative 60 mA/pixel profile value if you want generic WS2812 worst-case planning.

## First upload checklist

- Confirm the ESP32-S3 board target.
- Confirm `LED_DATA_PIN`.
- Confirm strip geometry: 5m x 160 LEDs/m = 800 pixels, 5m x 180 LEDs/m = 900 pixels, 5m x 60 LEDs/m = 300 pixels.
- Confirm power supply voltage and current in settings or defaults.
- Upload over USB first.
- Open `/diagnostics` and run low-brightness red, green, blue, and pixel-walk tests.
- Test Candle Hall, Quiet Northern Lights, and Rain Glass before raising brightness.

## Resource status

The firmware now targets the ESP32-S3 as the high-density strip profile while retaining ESP8266 compatibility where practical. Prior D1 mini live measurements remain historical and should not be treated as proof of the new ESP32-S3/high-density build:

- RAM globals/statics: 59,896 / 80,192 bytes (74%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 588,188 / 1,048,576 bytes (56%).
- Python contract tests: `python -m unittest discover -s tests -v` passed, 124 tests.
- Live heap after boot/resource check: 7,768 bytes free, 3,960 byte max free block, 32% fragmentation.
- Lowest measured route heap floor: 2,000 bytes during `/api/scenes`.
- USB upload over `COM5`: passed.
- `/ota` and authenticated `/update` page reachability: passed; no OTA upload performed.
- Physical visual LED behavior: visual output was not independently observed or measured for this ESP32-S3/high-density migration pass; not physically tested by Codex vision.

## Documentation

- [Final report](docs/generated/BedroomLedController_Final_Report.md)
- [User guide](docs/generated/BedroomLedController_User_Guide.md)
- [API reference](docs/generated/BedroomLedController_API_Reference.md)
- [Troubleshooting](docs/generated/BedroomLedController_Troubleshooting.md)
- [Resource and migration report](docs/generated/BedroomLedController_Resource_And_Migration_Report.md)
- [Release hardening report](docs/generated/BedroomLedController_Release_Hardening_Report.md)
- [Hardware profile and density-scaling report](docs/generated/BedroomLedController_Hardware_Profile_And_Density_Scaling_Report.md)
