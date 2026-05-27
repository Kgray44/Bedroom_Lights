# BedroomLedController User Guide

Open the controller in a browser using the controller IP address or hostname. The main page contains live controls, scenes, palettes, favorites, timers, bedtime fade, transitions, and maintenance tools.

## First-time setup

1. Copy `firmware/BedroomLedController/Secrets.example.h` to `firmware/BedroomLedController/Secrets.h`.
2. Edit `Secrets.h` with local Wi-Fi, OTA user, OTA password, and fallback AP password values.
3. Do not commit `Secrets.h`; it is intentionally ignored by `.gitignore`.
4. Rotate any credentials that were committed before the hardening pass.
5. Adjust `LED_DATA_PIN`, `STRIP_LENGTH_MM`, `LED_DENSITY_PER_METER`, and `LED_COUNT_OVERRIDE` in `HardwareProfile.h` for your strip and wiring.
6. Compile/upload for the selected ESP32-S3 board target, or use `esp8266:esp8266:d1_mini` for the legacy D1 mini path.

## Hardware profile

- Primary profile: ESP32-S3-N16R8 with a BTF FCOB 5V 160LED/m 5m strip.
- Default data pin: GPIO5. Confirm this against your actual ESP32-S3 development board and wiring.
- Default geometry: 5000 mm, 160 LEDs/m, 800 active pixels by override.
- Change density to 180 LEDs/m for a 900-pixel 5m strip, or 60 LEDs/m for a 300-pixel 5m strip.
- Use the LED count override only when the actual IC count is known and differs from calculated density.
- Do not power the LED strip through the controller. Use an external 5V supply, common ground, and a 5V-safe data level shifter if the strip is unreliable from 3.3V data.
- Start with a conservative brightness cap such as 30-60 / 255.

## Daily controls

- Use Master Brightness as the global output cap.
- Use Animation Intensity to control motion, contrast, shimmer, ribbon depth, and color spread inside supported animated modes. It is separate from Master Brightness.
- Use Color and White Temperature for the base color.
- Choose a Mode, then adjust the mode-specific timing controls that appear.
- Quiet Northern Lights uses Animation Intensity for aurora ribbon count, dissolve depth, fold visibility, shimmer, and companion color spread while preserving active-color anchoring and palette support.
- Candle Hall includes a 0-20 Candle flicker control; `3` keeps the previous gentle feel, while higher values make each candle cluster breathe, drift, and shimmer more strongly.
- Use Palettes for compatible ambient modes.
- Use Night Guard to cap brightness and block flashing modes.

## Scenes

- Save Current Scene stores the active color, mode, brightness, timing, and palette state.
- Load applies a scene.
- Duplicate creates a custom copy.
- Export writes scene JSON into the scene import box.
- Import Scene JSON imports one exported scene as a custom scene.
- Restore Built-In Scenes re-adds firmware defaults while preserving custom scenes when capacity allows.

## Backup and restore

- Export Full Backup writes a schema-versioned backup JSON bundle.
- Import Full Backup validates a backup with `confirm=YES`; it does not apply a bulk restore on this ESP8266 build.
- Use scene, palette, and schedule import routes for applied selective restore.
- Factory Reset requires typing `YES` and restores firmware defaults.

## Schedule and timers

- Sleep Timer can wait and then turn off, warm dim, or load a scene.
- Bedtime Fade gradually changes toward a bedtime target.
- The Schedule page supports repeated local-time scene/action automation.

## Maintenance

- Diagnostics are at `/diagnostics`.
- OTA upload is at `/ota`, with the browser updater at `/update`.
- API help is at `/api-docs`.
- Runtime resource metrics are available from `/api/diagnostics`.
- Board family, board profile, strip profile, physical length, LED density, configured LED count, mm per pixel, frame interval, data pin, and power estimate source are exposed in diagnostics/state APIs.
- Full backup export is chunked, but it is still a heavy route on ESP8266. If the web UI resets or hangs during backup export, restart the controller, check diagnostics, and prefer selective scene/palette/schedule export.
- OTA behavior and physical LED behavior were not measured in this pass.

## Resource metrics

Diagnostics reports free heap, largest free heap block, heap fragmentation percent, minimum free heap since boot, sketch size, free sketch space, and LittleFS total/used/free. Use `/api/resources` for the compact live endpoint heap metric list.

- Low free heap means the firmware has little RAM headroom for large requests.
- High heap fragmentation means a large JSON response may fail even when total free heap looks acceptable.
- Low LittleFS free space means saves/imports may fail.
- Low free sketch space can make OTA uploads fail.

The ESP32-S3 is the recommended target for the high-density strip. The ESP8266/D1 mini code path remains present, but the 800-900 pixel configuration is intended for the ESP32-S3-class memory and flash headroom.

## First physical test

1. Upload over USB first.
2. Open `/diagnostics` and confirm board, pin, strip length, density, LED count, mm per pixel, frame interval, and power estimate source.
3. Run low-brightness red, green, blue, and pixel-walk tests.
4. Confirm pixel count and strip direction.
5. Test Candle Hall and verify candle clusters look physically broad, not tiny.
6. Test Quiet Northern Lights and verify ribbons are broad and smooth.
7. Test Rain Glass and verify droplets and trails are visible at the new density.
8. Raise brightness only after checking power injection, voltage drop, heat, and supply headroom.
