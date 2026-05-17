# Bedroom LED Controller Phase 1 Final Report

## Phase 1 purpose

Phase 1 built the local-first foundation for the Bedroom LED Controller: persistent runtime settings, centralized output rendering, diagnostics, power estimates, safe LED tests, OTA preservation, and a stable local API surface for Phase 2.

The firmware targets a D1 mini / ESP8266 driving a WS2812 / NeoPixel strip on D3 / GPIO0.

## Files changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/Config.h`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/Diagnostics.h`
- `firmware/BedroomLedController/DiagnosticTests.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `tests/test_phase1a_contract.py`
- `tests/test_phase1b_contract.py`
- `tests/test_phase1c_contract.py`
- `tests/test_phase1d_contract.py`
- `docs/generated/BedroomLedController_Phase_1A_Report.md`
- `docs/generated/BedroomLedController_Phase_1B_Report.md`
- `docs/generated/BedroomLedController_Phase_1C_Report.md`
- `docs/generated/BedroomLedController_Phase_1_Final_Report.md`
- `docs/generated/Phase_Completion.md`

The Arduino sketch folder at `C:\Users\kkids\Documents\Arduino\BedroomLedController` is kept as a code-only mirror of `firmware/BedroomLedController`.

## Current route map

| Route | Method | Response | Purpose |
| --- | --- | --- | --- |
| `/` | GET | HTML | Main lighting controls. |
| `/state` | GET | JSON | Backward-compatible state endpoint used by the main UI. |
| `/set` | GET with query params | JSON | Backward-compatible mutation endpoint used by the main UI. |
| `/ota` | GET | HTML | OTA and browser updater info page. |
| `/update` | GET/POST via updater | HTML/upload | Browser firmware update route registered through `ESP8266HTTPUpdateServer`. |
| `/diagnostics` | GET | HTML | Diagnostics, power estimate, warnings, and LED test page. |
| `/api/state` | GET | JSON | Future-facing state endpoint. |
| `/api/brightness?value=180` | GET | JSON | Gets or sets master brightness. Numeric values are clamped to 0-255; non-numeric values are rejected. |
| `/api/color?hex=FF7828` | GET | JSON | Sets active RGB color by hex. |
| `/api/color?r=255&g=120&b=40` | GET | JSON | Sets active RGB color by channels. Numeric channels are clamped to 0-255; malformed input is rejected. |
| `/api/temperature?k=2700` | GET | JSON | Sets white temperature, clamped to 2700K-6500K. |
| `/api/mode?name=rainGlass` | GET | JSON | Sets mode by known identifier or simple display-name form. Unknown modes return JSON errors. |
| `/api/diagnostics` | GET | JSON | Runtime diagnostics, warnings, persistence status, and diagnostic test state. |
| `/api/power` | GET | JSON | Estimate-only current and brightness-cap data. |
| `/api/test?name=pixelWalk` | GET | JSON | Starts a diagnostic LED test or restores previous state. |

All JSON API responses include an `ok` field. Errors use compact JSON such as `{"ok":false,"error":"Invalid brightness value"}`.

## Current settings schema

Settings are saved to LittleFS at `/settings.json` with `"version":3`.

Core lighting fields:

- `mode`
- `red`
- `green`
- `blue`
- `whiteKelvin`
- `usingWhiteTemperature`
- `masterBrightness`

Mode option fields:

- `strobeDelaySec`
- `flashOffDelaySec`
- `chasePeriodSec`
- `wavePeriodSec`
- `slowPulseCount`
- `slowPulseMaxSec`
- `rainbowPeriodSec`

Behavior and output fields:

- `bootBehavior`
- `gammaEnabled`
- `redGain`
- `greenGain`
- `blueGain`

Power estimate fields:

- `supplyMillivolts`
- `supplyMilliamps`
- `supplySafetyMarginPercent`
- `controllerOverheadMilliamps`

Missing fields use safe defaults. Corrupt settings fall back to defaults, record load status, and surface a diagnostics warning. Older Phase 1A/1B/1C settings remain loadable because missing new fields are not treated as a whole-file failure.

## Master brightness behavior

Master brightness is stored in `settings.masterBrightness`, exposed in `/state`, `/api/state`, `/api/brightness`, `/api/diagnostics`, and `/api/power`, and settable through both legacy `/set` and the newer `/api/brightness`.

The output pipeline applies master brightness centrally after RGB calibration and before gamma correction. `strip.setBrightness(255)` is used only as a neutral setup baseline.

## Persistence behavior

Persistent setting changes mark the settings dirty and save through the debounced LittleFS path. The debounce value is `SETTINGS_SAVE_DEBOUNCE_MS = 1000`.

Settings are saved when user-facing persistent configuration changes. They are not saved by animation frames, diagnostics refreshes, uptime/free-heap changes, or pixel-walk steps.

Diagnostic tests save the previous lighting state in RAM and restore it without permanently overwriting the user's preferred settings.

## Output pipeline order

Normal output follows the Phase 1B centralized path:

1. Render the active mode or diagnostic test into `frameBuffer`.
2. Apply RGB calibration gains.
3. Apply master brightness.
4. Apply gamma correction when enabled.
5. Write pixels with `strip.setPixelColor`.
6. Call `strip.show` once in `outputFrameToStrip`.

Normal mode renderers do not call `strip.show` directly.

## Gamma/calibration behavior

Gamma correction uses a gamma 2.2 lookup table in PROGMEM and defaults to enabled. RGB gains default to 255/255/255 and are applied before brightness and gamma. The main UI, `/set`, `/state`, and `/api/state` expose these values.

## Diagnostics page summary

`/diagnostics` shows controller status, LED configuration, active lighting state, settings/persistence status, output pipeline status, the power estimator, warning messages, LED test controls, and links back to the main controls and OTA page.

`/api/diagnostics` is the machine-readable source for the diagnostics page.

## Power estimator

The power estimator uses the common WS2812 worst-case assumption of about 60 mA per LED at full RGB white and full brightness.

Defaults:

- 5000 mV supply voltage
- 5000 mA supply current
- 20 percent safety margin
- 200 mA controller overhead

It calculates worst-case LED current, estimated current at the current master brightness, usable supply current after margin, and a recommended master brightness cap. The cap is only applied when the user requests it.

The controller does not measure actual current.

## LED test tools summary

`/api/test` supports red, green, blue, low white, pixel walk, alternating pixels, clear/off, and restore. Tests render through the same frame buffer and output pipeline as normal modes.

Pixel walk is non-blocking and advances by `millis()` timing. The safe LED test brightness is `DIAGNOSTIC_TEST_BRIGHTNESS = 64`.

## Local API summary

Phase 1D adds stable local API routes for state, brightness, color, temperature, mode, diagnostics, power, and LED test actions. The legacy `/state` and `/set` routes remain for the existing UI and local shortcuts.

Core mutation paths now reuse shared helpers:

- `setMasterBrightness`
- `setActiveColor`
- `setWhiteTemperature`
- `setModeByName`
- `setGammaEnabled`
- `setRgbGains`
- `setPowerConfig`

Malformed core API inputs return JSON errors instead of silently becoming persistent settings.

## OTA/browser update status

OTA support remains in place:

- `setupOta()` calls `ArduinoOTA.begin()`.
- `loop()` continues to call `ArduinoOTA.handle()`.
- `server.handleClient()` remains in the main loop.
- Browser updates remain registered at `/update`.
- The `/ota` page remains available and links back to the main UI and diagnostics.

## Known limitations

- The firmware was compiled and contract-tested, but not physically tested on LEDs.
- The power estimator does not measure actual current and is only a software estimate.
- D3 / GPIO0 remains the LED data pin and may affect ESP8266 boot reliability if the attached strip or wiring pulls the pin low during startup.
- The local API has no authentication beyond local network access and the existing OTA credentials for update paths.
- Heap/RSSI warnings are setup diagnostics, not hardware-grade monitoring.

## Hardware safety notes

- Use an appropriately rated 5V supply for the installed LED count.
- Avoid powering large LED loads through the D1 mini board.
- Tie LED strip ground and controller ground together.
- Keep D3 / GPIO0 boot sensitivity in mind when wiring the LED data line.
- Treat the power estimator as a conservative planning tool, not a measurement.

## Deferred

The following were intentionally not implemented in Phase 1:

- Scene presets
- Favorite scene bar
- Timers and scheduler
- Smooth transitions
- Palette builder
- Browser live animation preview
- QR/NFC scene links
- Physical buttons
- Microphone/music-reactive behavior
- Light-sensor behavior
- New hardware assumptions
- Cloud or home automation integration

## Compile/test status

Verified during Phase 1D:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest discover -s tests -v`: passed, 28 tests.

The firmware compiled successfully for the D1 mini / ESP8266 target, but was not physically tested on LEDs.

## Recommended Phase 2 starting point

Phase 2A: scene preset storage and recall, built on the stable Phase 1 local API and central settings/rendering helpers.
