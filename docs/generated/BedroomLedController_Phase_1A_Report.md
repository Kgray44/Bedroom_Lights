# Bedroom LED Controller - Phase 1A Report

## Status

Phase 1A is implemented in the split Arduino sketch and mirrored in the worktree firmware copy.

Compiled successfully for the D1 mini / ESP8266 target with `esp8266:esp8266:d1_mini`.

This was not physically tested on LEDs.

## Files Changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/Config.h`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `firmware/BedroomLedController/LedRendering.h`
- `tests/test_phase1a_contract.py`

The same `.ino` and `.h` code files were copied into `C:\Users\kkids\Documents\Arduino\BedroomLedController`.

## Settings Model

Phase 1A now uses one central `ControllerSettings settings` object for the active mode, RGB color, white temperature state, master brightness, existing mode timing options, and boot behavior.

The old separate globals for current mode, current color, Kelvin, and animation timing options were folded into that settings object.

## Persistent Storage

Settings are saved to LittleFS at `/settings.json`.

The save path is debounced with `SETTINGS_SAVE_DEBOUNCE_MS = 1000`, so UI changes update runtime state immediately but do not write flash on every animation frame or slider tick.

Missing, unreadable, or corrupt settings fall back to safe defaults and record a load status string for future diagnostics.

## Master Brightness

The main web UI now includes a Master Brightness slider with raw `0-255` value and percentage.

`/state` now exposes:

- `masterBrightness`
- `masterBrightnessPercent`
- `bootBehavior`
- `usingWhiteTemperature`
- settings load/save status fields

`/set` accepts both `masterBrightness` and the compatible alias `brightness`.

For Phase 1A, global brightness is applied with `strip.setBrightness(settings.masterBrightness)`. This is intentionally a compatibility step. Phase 1B should move master brightness into the final render/output buffer pipeline.

## Boot Behavior

Implemented boot behavior options:

- `restore`
- `startOff`
- `warmDim`

The selector is available in the main UI and is persisted with the rest of the settings.

## Preserved

- Existing local web UI route `/`
- Existing `/state`
- Existing `/set`
- OTA setup
- Browser update route `/update`
- OTA page `/ota`
- Existing modes and mode controls
- D3 / GPIO0 LED data pin setting

## Deferred

- Full render buffer pipeline
- Gamma correction
- RGB calibration
- Diagnostics page
- Power estimator
- LED test tools
- Local automation API cleanup beyond the Phase 1A `/state` and `/set` work
- Scenes, timers, scheduler, palettes, transitions, and new modes

## Known Limitations

- Master brightness is currently applied through the NeoPixel library brightness path, not a custom final frame pipeline.
- Power draw is not measured. There is no current-sensing hardware.
- D3 / GPIO0 can affect ESP8266 boot reliability depending on the attached LED strip and pull state.
- Physical LED behavior still needs real hardware verification after upload.

## Project Organization

The worktree is organized as:

- `firmware/BedroomLedController` - uploadable firmware copy
- `docs/user-phase-docs` - user-provided phase and implementation docs
- `docs/source-reference` - original project notes moved out of the Arduino sketch folder
- `docs/generated` - Codex-generated reports
- `tests` - local regression checks

The Arduino sketch folder now contains only code files and no documentation files.

## Recommended Next Step

Phase 1B - build the central render/output buffer pipeline, then move master brightness into that pipeline before adding gamma correction and calibration.
