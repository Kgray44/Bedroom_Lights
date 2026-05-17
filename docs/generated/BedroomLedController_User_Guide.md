# BedroomLedController User Guide

Open the controller in a browser using the D1 mini IP address or hostname. The main page contains live controls, scenes, palettes, favorites, timers, bedtime fade, transitions, and maintenance tools.

## First-time setup

1. Copy `firmware/BedroomLedController/Secrets.example.h` to `firmware/BedroomLedController/Secrets.h`.
2. Edit `Secrets.h` with local Wi-Fi, OTA user, OTA password, and fallback AP password values.
3. Do not commit `Secrets.h`; it is intentionally ignored by `.gitignore`.
4. Rotate any credentials that were committed before the hardening pass.
5. Adjust `LED_COUNT` and `LED_PIN` in `Config.h` for your strip and wiring.
6. Compile/upload for `esp8266:esp8266:d1_mini`.

## Daily controls

- Use Master Brightness as the global output cap.
- Use Color and White Temperature for the base color.
- Choose a Mode, then adjust the mode-specific timing controls that appear.
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
- Full backup export is chunked, but it is still a heavy route on ESP8266. If the web UI resets or hangs during backup export, restart the controller, check diagnostics, and prefer selective scene/palette/schedule export.
- OTA behavior and physical LED behavior were not measured in this pass.

## Resource metrics

Diagnostics reports free heap, largest free heap block, heap fragmentation percent, minimum free heap since boot, sketch size, free sketch space, and LittleFS total/used/free. Use `/api/resources` for the compact live endpoint heap metric list.

- Low free heap means the firmware has little RAM headroom for large requests.
- High heap fragmentation means a large JSON response may fail even when total free heap looks acceptable.
- Low LittleFS free space means saves/imports may fail.
- Low free sketch space can make OTA uploads fail.

The current D1 mini build is acceptable for the completed feature set, but ESP32-S3 is the recommended future target for major expansion.
