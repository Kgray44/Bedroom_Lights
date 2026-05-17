# Bedroom LED Controller - Phase 1B Report

## Status

Phase 1B is implemented in the split Arduino sketch and mirrored in the worktree firmware copy.

Compiled successfully for the D1 mini / ESP8266 target with `esp8266:esp8266:d1_mini`.

This was not physically tested on LEDs.

## Files Changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `tests/test_phase1b_contract.py`

The same `.ino` and `.h` code files were copied into `C:\Users\kkids\Documents\Arduino\BedroomLedController`.

## Frame Buffer Architecture

Phase 1B adds a static `RgbPixel frameBuffer[LED_COUNT]` buffer. Existing modes now render raw intended RGB values into this buffer instead of writing directly to the NeoPixel strip.

Frame helpers added:

- `clearFrame()`
- `setFramePixel()`
- `setFramePixelColor()`
- `fillFrame()`
- `fillFrameColor()`
- `makeColor()`

`setFramePixel()` bounds-checks LED indexes and ignores invalid pixels safely.

## Final Output Pipeline

Normal animation output now runs through `outputFrameToStrip()`.

Output order:

1. Read raw frame buffer color.
2. Apply RGB calibration gains.
3. Apply master brightness scaling.
4. Apply gamma correction if enabled.
5. Write to `strip.setPixelColor()`.
6. Call `strip.show()`.

Normal mode renderers no longer call `strip.show()` or `strip.setPixelColor()` directly.

## Master Brightness

Master brightness is now applied in the final output pipeline with `scaleByMasterBrightness()`.

`strip.setBrightness(settings.masterBrightness)` is no longer used, which avoids double-applying brightness. The sketch calls `strip.setBrightness(255)` once during setup as a neutral NeoPixel baseline, then all runtime brightness control happens through the pipeline.

## Gamma Correction

Gamma correction is backed by a fixed gamma 2.2 lookup table in PROGMEM.

`settings.gammaEnabled` defaults to `true`, is persisted, exposed in `/state`, settable through `/set?gammaEnabled=...`, and available in the main UI.

## RGB Calibration

Added persisted settings:

- `redGain`
- `greenGain`
- `blueGain`

Each gain ranges from `0` to `255`, with `255` as the neutral default.

Calibration is applied before master brightness and gamma. The main UI has compact gain sliders plus a reset calibration button.

## Persistence Additions

Settings JSON now saves Phase 1B fields:

- `gammaEnabled`
- `redGain`
- `greenGain`
- `blueGain`

Older Phase 1A settings files still load because missing fields fall back to defaults.

## Special Mode Handling

- Rainbow still ignores the active user color, but its generated colors now go through calibration, master brightness, and gamma.
- Strobe and Flash now fill or clear the frame buffer without blocking delays.
- Existing slow/calm modes kept their timing and color math while swapping their final pixel writes for frame-buffer writes.

## Preserved

- Existing routes: `/`, `/state`, `/set`, `/ota`, `/update`
- OTA setup and browser firmware update registration
- Existing mode names and controls
- Phase 1A master brightness, persistence, and boot behavior
- Code-only Arduino sketch folder

## Deferred

- Diagnostics page
- Power estimator
- LED test tools
- Scene presets
- Favorite bar
- Smooth transitions
- Browser preview strip
- Palette builder
- Timers and scheduler
- New lighting modes
- Hardware controls, microphone support, and light sensor behavior

## Known Limitations

- No physical LED verification was performed.
- Gamma 2.2 can make low numeric values appear dimmer than the previous linear output; it can be disabled in the UI or via `/set?gammaEnabled=0`.
- The controller still does not measure real current draw. Any future power safety work must remain estimate-based unless current-sensing hardware is added.
- D3 / GPIO0 remains the configured LED data pin and may affect ESP8266 boot reliability depending on the attached strip and wiring.

## Recommended Next Step

Phase 1C - diagnostics page, power estimator, and LED test tools.
