# Bedroom LED Controller Phase 1C Report

## Scope

Phase 1C added the diagnostics and safety surface for the D1 mini / ESP8266 Bedroom LED Controller. It does not add new aesthetic modes, scene presets, timers, schedules, palettes, smooth transitions, or browser live preview.

## Files Changed

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
- `tests/test_phase1c_contract.py`
- `docs/generated/BedroomLedController_Phase_1C_Report.md`

The Arduino sketch folder at `C:\Users\kkids\Documents\Arduino\BedroomLedController` was mirrored from the worktree firmware folder and kept code-only.

## Diagnostics Page

The new `/diagnostics` route serves a lightweight browser page with sections for controller status, LED configuration, current lighting state, settings and persistence status, output pipeline status, power estimate, configuration warnings, LED test tools, and maintenance links.

The main controller UI now links to `/diagnostics` near the OTA link.

## JSON Endpoints

`/api/diagnostics` returns compact runtime JSON for hostname, IP, Wi-Fi status, RSSI, uptime, free heap, LED count, D3/GPIO0 pin status, active mode, color, master brightness, gamma/calibration state, boot behavior, persistence status, diagnostic test state, and warnings.

`/api/power` returns compact power-estimate JSON, including configured supply values, worst-case LED current, estimated total current, usable supply current, recommended brightness cap, current master brightness, and whether the current brightness is above the recommended cap.

## Power Estimator

The estimator uses the common WS2812 worst-case assumption of 60 mA per LED at full RGB white and full brightness.

Defaults:

- Supply voltage: 5000 mV
- Supply current: 5000 mA
- Safety margin: 20 percent
- Controller overhead: 200 mA

The recommended brightness cap is calculated from usable supply current after the safety margin, minus controller overhead. The cap is clamped to 0-255 and is only applied when the user presses the Apply Recommended Brightness Cap button.

This is an estimate only. The controller does not measure actual current draw.

## Warning Logic

Warnings are generated for:

- Current brightness above the recommended power cap
- D3/GPIO0 boot pin risk
- Weak Wi-Fi RSSI
- Low heap
- Settings storage load/save problems
- Full-white current above the configured supply capacity

## LED Test Tools

The `/api/test` endpoint supports:

- Red LED test
- Green LED test
- Blue LED test
- Safe low white LED test
- Pixel Walk
- Alternating Pixels
- Clear / Off
- Return to Previous State

Diagnostic tests render through the Phase 1B frame buffer and output pipeline. Pixel Walk uses non-blocking `millis()` timing. Tests use `DIAGNOSTIC_TEST_BRIGHTNESS` set to 64, and the previous lighting state is restored when the user selects Return to Previous State.

## Persistence Changes

The central settings model now persists:

- `supplyMillivolts`
- `supplyMilliamps`
- `supplySafetyMarginPercent`
- `controllerOverheadMilliamps`

Missing fields default safely, so older settings files remain usable.

## Compile/Test Status

Verified so far in this pass:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed
- `python -m unittest discover -s tests -v`: passed, 21 tests

The firmware was compiled successfully, but not physically tested on LEDs.

## Known Limitations

- The power page estimates current; it does not measure actual current.
- LED tests are software-implemented and compiled, but not physically tested on the LED strip.
- RSSI, heap, and storage warnings are coarse setup diagnostics, not hardware-grade monitoring.

## Recommended Next Step

Phase 1D: local API cleanup, route consolidation, documentation consolidation, and final Phase 1 hardening.
