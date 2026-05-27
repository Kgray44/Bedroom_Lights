# Bedroom LED Controller Hardware Profile And Density Scaling Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make the Bedroom LED Controller describe its real board/strip profile and render density-sensitive visuals in physical strip units.

**Architecture:** Add a small hardware-profile layer in firmware config, expose helper conversions in the rendering layer, and refactor existing mode math from raw pixel widths/speeds to normalized or millimeter-based coordinates. Keep the existing frame-buffer, API, scenes, palettes, schedule, timers, Night Guard, OTA, and browser update surfaces intact.

**Tech Stack:** Arduino C++ sketch split across `.ino` and `.h` files, Adafruit NeoPixel, ESP8266/ESP32 Arduino core conditionals, Python unittest contract tests, generated Markdown reports.

---

### Task 1: Contract Tests For Hardware Profile And Spatial Scaling

**Files:**
- Create: `tests/test_hardware_profile_density_scaling_contract.py`
- Modify: `tests/test_phase1c_contract.py`
- Modify: `tests/test_phase1b_contract.py`

- [ ] **Step 1: Write the failing hardware-profile test**

Create a Python unittest that asserts `HardwareProfile.h` exists, exposes `LedHardwareProfile`, BTF FCOB defaults, calculated LED counts for 60/160/180 LEDs per meter, physical helper APIs, ESP32/ESP8266 conditional includes, hardware profile fields in state/diagnostics/power JSON, and mirrored sketch-folder code.

- [ ] **Step 2: Run the new test and verify RED**

Run: `python -m unittest tests.test_hardware_profile_density_scaling_contract -v`

Expected: FAIL because `HardwareProfile.h`, spatial helper names, and profile JSON fields do not exist yet.

- [ ] **Step 3: Update old contract expectations**

Adjust older D3/GPIO0 and `LED_COUNT`-literal tests so they accept central profile helpers instead of hardcoded ESP8266-only configuration.

### Task 2: Central Hardware Profile And ESP32-S3 Compatibility

**Files:**
- Create: `firmware/BedroomLedController/HardwareProfile.h`
- Modify: `firmware/BedroomLedController/Config.h`
- Modify: `firmware/BedroomLedController/BedroomLedController.ino`
- Modify: `firmware/BedroomLedController/ControllerTypes.h`
- Modify: `firmware/BedroomLedController/ForwardDeclarations.h`

- [ ] **Step 1: Add `LedHardwareProfile`**

Define board/profile strings, strip length, density, optional LED count override, LED data pin, target frame interval, supply defaults, mA-per-pixel estimate, and estimate source.

- [ ] **Step 2: Replace hardcoded LED constants with derived constants**

Keep `LED_COUNT` compile-time for static buffers and the NeoPixel constructor, but derive it from `STRIP_LENGTH_MM`, `LED_DENSITY_PER_METER`, and `LED_COUNT_OVERRIDE`. Default to 5000 mm, 160 LEDs/m, 800 active LEDs, GPIO5, 33 ms frame interval, 5 V, 5 A, 20% safety margin, 200 mA controller overhead, and 18.75 mA/pixel manufacturer estimate.

- [ ] **Step 3: Add board conditional includes**

Use `ESP8266WiFi`/`ESP8266WebServer`/`ESP8266HTTPUpdateServer` on ESP8266 and `WiFi`/`WebServer`/`HTTPUpdateServer` on ESP32. Keep `/update` setup behind the shared update-server object.

- [ ] **Step 4: Add board/platform helper wrappers**

Provide functions for board family/profile, heap fields, filesystem info, reset reason, hostname setup, and random seed so diagnostics compile for both ESP8266 and ESP32.

### Task 3: Physical Unit Helpers And Mode Rendering

**Files:**
- Modify: `firmware/BedroomLedController/LedRendering.h`
- Modify: `firmware/BedroomLedController/DiagnosticTests.h`
- Modify: `firmware/BedroomLedController/TransitionControls.h`

- [ ] **Step 1: Add physical helpers**

Implement `getConfiguredLedCount`, `getPixelsPerMeter`, `getMmPerPixel`, `mmToPixels`, `cmToPixels`, `metersToPixels`, `pixelToMm`, `pixelToMeters`, `normalizedPosition01`, `physicalXmm`, `clampPixelIndex`, `pixelInRange`, and cyclic distance helpers.

- [ ] **Step 2: Refactor render loops to use active LED helpers**

Keep pixel iteration as pixel iteration, but use `getConfiguredLedCount()` and `lastPixelIndex()` for loop bounds and normalized coordinates. Keep `strip.show()` centralized.

- [ ] **Step 3: Convert physical-looking dimensions**

Convert Candle Hall candle/glow/wobble width, Quiet Northern Lights wave length and curtain detail, Rain Glass and Rainy Window bead/trail width, lantern rows, wave/current/glass modes, stars/snow/moon glow, circuit/chase segments, and pulse/slow-wave widths to mm-based helpers. Use normalized position for strip-wide gradients and whole-strip sweeps.

- [ ] **Step 4: Keep animation speeds physical where movement is physical**

Replace pixels-per-frame or pixels-per-millisecond motion with mm-per-second offsets where the effect is a moving visible object. Keep full-strip proportional gradients normalized.

### Task 4: Power, Diagnostics, API, UI Text, And Docs

**Files:**
- Modify: `firmware/BedroomLedController/Diagnostics.h`
- Modify: `firmware/BedroomLedController/WebRoutes.h`
- Modify: `firmware/BedroomLedController/WebUi.h`
- Modify: `README.md`
- Modify: `docs/generated/BedroomLedController_User_Guide.md`
- Create: `docs/generated/BedroomLedController_Hardware_Profile_And_Density_Scaling_Report.md`

- [ ] **Step 1: Update power estimator**

Use active profile mA/pixel and expose estimate source. Do not claim measured current. Include strip length, density, active LED count, supply defaults, safety margin, estimated current at current brightness, and recommended cap.

- [ ] **Step 2: Expose profile fields in JSON**

Add board family/profile, strip profile/type, length, density, configured LED count, mm per pixel, target frame interval, data pin, and power-estimate assumptions to `/api/state`, `/api/diagnostics`, and `/api/power`.

- [ ] **Step 3: Update diagnostics UI and README/user guide**

Show board/strip profile details, GPIO5 default, common ground, external 5 V supply warning, level shifter recommendation, density/override configuration, and first-upload checklist.

- [ ] **Step 4: Write final migration report**

Document files changed, board migration, ESP8266 support status, strip profile, helper summary, mode scaling behavior, Candle Hall/aurora/rain specifics, power estimator behavior, diagnostics/API additions, compile/test status, known limitations, user configuration checklist, and physical testing status.

### Task 5: Verification And Mirror

**Files:**
- Worktree firmware under `firmware/BedroomLedController`
- Arduino sketch mirror under `C:\Users\kkids\Documents\Arduino\BedroomLedController`

- [ ] **Step 1: Mirror code to Arduino sketch folder**

Copy `.ino` and `.h` firmware files only; keep docs out of the Arduino sketch folder.

- [ ] **Step 2: Run tests**

Run: `python -m unittest discover -s tests -v`

Expected: PASS.

- [ ] **Step 3: Attempt compiles honestly**

Try locating `arduino-cli`; if unavailable, report compile blocked. If available, compile ESP32-S3 and ESP8266 targets and report exact outcomes.

- [ ] **Step 4: Static searches**

Search for raw D3 assumptions, raw `LED_COUNT` physical widths, `strip.show()` outside the output pipeline, out-of-range writes, `delay()` added to renderers, and missing mirrored files.
