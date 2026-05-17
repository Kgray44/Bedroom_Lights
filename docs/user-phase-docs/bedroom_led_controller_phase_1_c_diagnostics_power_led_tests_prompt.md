# Bedroom LED Controller — Phase 1C Codex Prompt

## Phase 1C Title

**Diagnostics Page + Power Estimator + LED Test Tools**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 1C** of the larger upgrade plan.

Phase 1A should already have added or started:

- Central runtime settings model
- Persistent settings storage
- Debounced settings saves
- Master brightness control
- Master brightness UI
- `/state` and `/set` support for master brightness
- Basic boot behavior scaffold

Phase 1B should already have added or started:

- Central LED frame buffer
- Centralized final output pipeline
- Master brightness applied centrally
- Gamma correction foundation
- RGB calibration foundation
- Existing modes routed through the output pipeline

The goal of Phase 1C is to add the controller’s **diagnostic and safety surface**:

1. A useful `/diagnostics` page
2. A JSON diagnostics endpoint
3. Power supply configuration and current estimate tools
4. Warnings for common setup risks
5. Safe LED test and wiring-check tools
6. Minimal UI cleanup needed to support those tools
7. Documentation/report update

Do **not** add new aesthetic lighting modes in this phase.

Do **not** implement scene presets, timers, schedules, palettes, smooth transitions, or browser live preview yet.

This phase is where the project gets a tiny lab coat and starts saying things like, “According to my estimates, your LED strip is hungry.”

---

# Current Project Context

The project already includes:

- ESP8266 / D1 mini target
- WS2812 LED strip control using Adafruit NeoPixel
- Existing local web UI
- Existing animation modes
- Existing calm modes
- Existing `/state` endpoint
- Existing `/set` endpoint
- Existing OTA support
- Existing browser firmware update route
- Phase 1A settings/persistence/master brightness foundation
- Phase 1B render/output pipeline and gamma/calibration foundation

Existing routes that should remain functional:

- `/`
- `/state`
- `/set`
- `/ota`
- `/update`

New routes for this phase should be added without breaking the existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. `/diagnostics` browser page
2. `/api/diagnostics` JSON endpoint
3. `/api/power` JSON endpoint, if useful
4. Power supply configuration settings
5. Estimated current / brightness cap calculations
6. Configuration warnings
7. Safe LED test tools
8. LED test state machine
9. Return-to-previous-state behavior after tests
10. Documentation/report update
11. Compile/test report

Do **not** implement yet:

- Scene presets
- Favorite scene bar
- Smooth transitions
- Browser live animation preview
- Palette builder
- New lighting modes
- Timers
- Scheduler
- QR/NFC links
- Full advanced API beyond diagnostics/power/test helpers
- Physical buttons
- Microphone support
- Light sensor support
- New hardware assumptions

If you see a tempting later-phase feature, document it as deferred. The feature goblin may knock. Do not open the door.

---

# Phase 1C Main Goal

Add a practical diagnostics and test interface that helps the user answer:

- Is the controller connected to Wi-Fi?
- What IP/hostname is it using?
- How long has it been running?
- How much heap is free?
- What LED count and pin are configured?
- What mode/color/brightness is active?
- Is gamma enabled?
- What are the RGB calibration gains?
- Did settings load/save correctly?
- How much current could the strip draw?
- Is the selected brightness likely safe for the configured power supply?
- Can the strip show red/green/blue/white correctly?
- Is the LED count/direction correct?

This should make the controller easier to set up, safer to run, and less cursed to debug.

---

# 1. Add `/diagnostics` Browser Page

## Goal

Add a human-readable diagnostics page served by the ESP8266.

## Required Route

Add:

```text
/diagnostics
```

This page should be reachable from the main UI with a clear link or button, such as:

```text
Diagnostics
```

Also consider linking back from diagnostics to:

- Main controller page `/`
- OTA page `/ota`

## Page Style

Keep the page lightweight and consistent with the existing UI style.

Do not add heavy frameworks.

The ESP8266 is a small microcontroller, not a MacBook wearing a Wi-Fi hat.

Use simple HTML/CSS/JavaScript.

## Required Sections

The page should include these sections:

1. Controller Status
2. LED Configuration
3. Current Lighting State
4. Settings / Persistence Status
5. Output Pipeline Status
6. Power Estimate
7. Configuration Warnings
8. LED Test Tools
9. Links / Maintenance

---

# 2. Controller Status Section

## Required Fields

Show:

- Project name / firmware name
- Firmware version or build label, if available
- Hostname
- IP address
- Wi-Fi connection status
- Wi-Fi SSID, if safe and already exposed elsewhere
- RSSI signal strength
- Uptime
- Free heap
- Chip ID, optional
- Reset reason, optional if easy on ESP8266

## Suggested Display

Example:

```text
Controller Status
Hostname: bedroom-leds
IP Address: 192.168.1.123
Wi-Fi: Connected
Signal: -54 dBm
Uptime: 01:42:18
Free Heap: 32784 bytes
```

## Refresh Behavior

The diagnostics page should refresh live values periodically without reloading the whole page.

Suggested:

- Fetch `/api/diagnostics` every 2–5 seconds.

If JavaScript complexity is a concern, static page load is acceptable for first pass, but live refresh is preferred.

## Acceptance Criteria

- `/diagnostics` loads.
- Controller status shows real values.
- Page does not crash if Wi-Fi info is unavailable.
- Main UI links to diagnostics.

---

# 3. LED Configuration Section

## Required Fields

Show:

- LED count
- LED pin name/number
- LED strip type / color order, if known
- Master brightness
- Master brightness percentage
- Whether the pin is a boot-sensitive ESP8266 pin

## D3 / GPIO0 Warning

If the LED pin is `D3` / GPIO0, show a warning:

```text
D3 on a D1 mini maps to GPIO0, which is a boot-mode pin. It can work for WS2812 data, but if the strip or wiring pulls this pin low during boot, the D1 mini may enter flash/programming mode instead of running normally.
```

Do not automatically change the pin.

Just warn clearly.

## Acceptance Criteria

- LED count is displayed.
- LED pin is displayed.
- D3/GPIO0 warning appears when applicable.
- Brightness info matches current state.

---

# 4. Current Lighting State Section

## Required Fields

Show:

- Active mode
- Active RGB color
- Active color hex value
- White temperature value
- Whether white-temperature mode is currently the active color source, if tracked
- Mode-specific speed/options currently active
- Gamma enabled/disabled
- RGB calibration gains
- Boot behavior setting

## Suggested Format

Example:

```text
Current Lighting State
Mode: Moon Tide
Color: #FFAA55
White Temperature: 2700K
Master Brightness: 180 / 255 (71%)
Gamma Correction: Enabled
RGB Gains: R 255, G 245, B 230
Boot Behavior: Restore last state
```

## Acceptance Criteria

- Current mode matches actual runtime mode.
- Color and temperature values match runtime state.
- Gamma/calibration values reflect Phase 1B settings.

---

# 5. Settings / Persistence Status Section

## Goal

Expose whether settings loaded and saved correctly.

## Required Fields

Show if available:

- Settings file path, such as `/settings.json`
- Last load status
- Last load message/error summary
- Last save status
- Last save message/error summary
- Whether settings are currently dirty/pending save
- Last save time or millis timestamp, if tracked
- Settings version

## Required Behavior

If Phase 1A did not track detailed status, add simple fields now.

Suggested internal fields:

```cpp
bool settingsLoadedOk;
bool settingsSaveOk;
bool settingsDirty;
String settingsStatusMessage;
unsigned long lastSettingsSaveMs;
```

Use compact strings. Do not waste heap with giant messages.

## Acceptance Criteria

- User can see whether settings loaded.
- User can see whether settings are pending save.
- Missing/corrupt settings do not crash diagnostics.

---

# 6. Output Pipeline Status Section

## Goal

Expose the Phase 1B output pipeline behavior so future debugging is easier.

## Required Fields

Show:

- Frame buffer enabled/active
- Master brightness applied centrally: yes/no
- Gamma enabled/disabled
- RGB calibration gains
- Normal animation output path uses centralized `strip.show()`, if tracked/documented

This does not need to be overly technical in the UI.

Suggested UI text:

```text
Output Pipeline
Frame buffer: Active
Brightness stage: Master brightness applied before output
Gamma correction: Enabled
Calibration: R 255 / G 255 / B 255
```

## Acceptance Criteria

- The diagnostics page confirms major output settings.
- It does not overclaim details the code cannot actually verify.

---

# 7. Power Supply Configuration Settings

## Goal

Let the user configure their power supply estimate so the controller can warn about unsafe brightness/current assumptions.

This is software-only. There is no real current sensor.

## Required Settings

Add these settings to the central settings model and persistence if not already present:

```cpp
float supplyVoltage;
float supplyCurrentAmps;
uint8_t supplySafetyMarginPercent;
float controllerOverheadAmps;
```

Recommended defaults:

```text
supplyVoltage = 5.0
supplyCurrentAmps = 5.0
supplySafetyMarginPercent = 20
controllerOverheadAmps = 0.20
```

If float storage feels excessive, store integer values:

```cpp
uint16_t supplyMillivolts = 5000;
uint16_t supplyMilliamps = 5000;
uint8_t supplySafetyMarginPercent = 20;
uint16_t controllerOverheadMilliamps = 200;
```

Integer storage may be cleaner on ESP8266.

## UI Controls

On `/diagnostics`, add a small power configuration form:

- Supply voltage
- Supply current rating
- Safety margin percentage
- Controller overhead estimate, optional advanced field
- Save power settings button

Keep it simple.

Example:

```text
Power Supply
Voltage: [5.0 V]
Current rating: [5.0 A]
Safety margin: [20%]
Controller overhead: [0.20 A]
[Save]
```

## Acceptance Criteria

- User can view configured power supply values.
- User can update values.
- Values persist after restart.
- Invalid inputs are clamped or rejected safely.

---

# 8. Power Estimate Calculations

## Goal

Estimate current draw and recommend a safe brightness cap.

## Required Estimate Assumptions

Use the common WS2812 worst-case estimate:

```text
60 mA per LED at full RGB white and full brightness
```

Make clear in UI/docs that this is an estimate, not measured current.

## Required Calculations

Use formulas equivalent to:

```text
maxLedCurrentA = ledCount * 0.060
maxLedPowerW = supplyVoltage * maxLedCurrentA
currentBrightnessFraction = masterBrightness / 255.0
estimatedLedCurrentAtBrightnessA = maxLedCurrentA * currentBrightnessFraction
estimatedTotalCurrentA = estimatedLedCurrentAtBrightnessA + controllerOverheadA
usableSupplyCurrentA = supplyCurrentA * (1.0 - safetyMarginPercent / 100.0)
safeLedCurrentA = usableSupplyCurrentA - controllerOverheadA
recommendedBrightnessCap = (safeLedCurrentA / maxLedCurrentA) * 255
```

Clamp recommended brightness cap:

```text
0–255
```

Handle edge cases:

- LED count <= 0
- Supply current <= 0
- Safety margin too high
- Max LED current is zero

## Required Display Values

Show:

- Worst-case LED current at full white/full brightness
- Worst-case LED power
- Estimated LED current at current master brightness
- Estimated total current including controller overhead
- Configured usable supply current after safety margin
- Recommended master brightness cap
- Current master brightness
- Whether current brightness exceeds recommended cap

## Apply Recommended Cap Button

Add a button:

```text
Apply Recommended Brightness Cap
```

Behavior:

- Set `masterBrightness` to the recommended cap.
- Mark settings dirty.
- Apply immediately.
- Do not apply automatically without user action.

## Required Honesty Text

Include wording similar to:

```text
These values are estimates based on the common WS2812 worst-case assumption of about 60 mA per LED at full RGB white. The controller does not measure real current draw.
```

## Acceptance Criteria

- Power values update based on LED count, brightness, and supply settings.
- Recommended cap is sane and clamped.
- Apply cap button works.
- UI avoids claiming current is actually measured.

---

# 9. Configuration Warnings

## Goal

Show practical warnings when configuration values look risky.

## Required Warnings

Add warnings for:

### Brightness Above Recommended Cap

If current master brightness is above recommended cap:

```text
Current brightness may exceed the configured power supply safety estimate.
```

### D3 / GPIO0 Boot Pin

If using D3/GPIO0:

```text
D3/GPIO0 can affect ESP8266 boot mode if pulled low during startup.
```

### Weak Wi-Fi

If RSSI is poor, such as below -75 dBm:

```text
Wi-Fi signal is weak. OTA and web control may be unreliable.
```

### Low Heap

If free heap is low, choose a conservative threshold and warn:

```text
Free heap is low. Large pages or complex animations may become unstable.
```

### Settings Load/Save Failure

If settings failed to load or save:

```text
Settings storage reported an error. Changes may not survive reboot.
```

### Full-White Current High

If worst-case current is high relative to supply:

```text
Full-white worst case is above configured supply capacity.
```

## Warning Style

Warnings should be clear but not terrifying.

This is a bedroom LED strip, not a nuclear submarine reactor panel.

Use practical, helpful language.

## Acceptance Criteria

- Warnings appear only when applicable.
- Warnings do not break JSON output.
- Warnings are visible on `/diagnostics`.

---

# 10. Add `/api/diagnostics` JSON Endpoint

## Goal

Provide diagnostics data as JSON for the diagnostics page and future API use.

## Required Route

Add:

```text
/api/diagnostics
```

## Required JSON Fields

Return compact JSON containing at least:

```json
{
  "ok": true,
  "hostname": "bedroom-leds",
  "ip": "192.168.1.123",
  "wifiConnected": true,
  "rssi": -54,
  "uptimeMs": 123456,
  "freeHeap": 32784,
  "ledCount": 60,
  "ledPin": "D3",
  "mode": "Solid",
  "colorHex": "#FFAA55",
  "whiteKelvin": 2700,
  "masterBrightness": 180,
  "masterBrightnessPercent": 71,
  "gammaEnabled": true,
  "redGain": 255,
  "greenGain": 255,
  "blueGain": 255,
  "bootBehavior": "restore",
  "settingsLoadedOk": true,
  "settingsSaveOk": true,
  "settingsDirty": false,
  "warnings": []
}
```

Exact fields may differ based on existing code, but the endpoint should be useful and compact.

## Acceptance Criteria

- Endpoint returns valid JSON.
- Diagnostics page can use it.
- Invalid/unavailable values are handled safely.
- JSON stays reasonably small.

---

# 11. Add `/api/power` JSON Endpoint

## Goal

Provide current power estimate data as JSON.

## Required Route

Add:

```text
/api/power
```

This can be separate or included in `/api/diagnostics`. Separate is preferred if it keeps code organized.

## Required JSON Fields

Return fields equivalent to:

```json
{
  "ok": true,
  "estimateOnly": true,
  "ledCount": 60,
  "supplyVoltage": 5.0,
  "supplyCurrentAmps": 5.0,
  "safetyMarginPercent": 20,
  "controllerOverheadAmps": 0.2,
  "maxLedCurrentAmps": 3.6,
  "maxLedPowerWatts": 18.0,
  "estimatedLedCurrentAmps": 2.54,
  "estimatedTotalCurrentAmps": 2.74,
  "usableSupplyCurrentAmps": 4.0,
  "recommendedBrightnessCap": 255,
  "currentMasterBrightness": 180,
  "aboveRecommendedCap": false
}
```

If using integer milliamps internally, JSON may expose amps for readability.

## Acceptance Criteria

- Endpoint returns valid JSON.
- Values match diagnostics page.
- Edge cases do not divide by zero.

---

# 12. Add Safe LED Test Tools

## Goal

Add diagnostic LED tests for setup and wiring checks.

These are not new aesthetic modes. They are utility tests.

## Required Tests

Implement at least:

1. All Red
2. All Green
3. All Blue
4. All White at safe low brightness
5. Pixel Walk
6. Alternating Pixels
7. Clear / Off
8. Return to Previous State

Optional useful tests:

- First LED only
- Last LED only
- Half strip A/B
- Color order check sequence

## Safety Requirements

Full-strip tests should use safe brightness by default.

Recommended safe diagnostic brightness:

```text
40–80 out of 255
```

Do not automatically run full-white at full master brightness.

The point is to verify wiring, not summon the Bedroom Sun.

## Test State Model

Add a diagnostic test state machine.

Suggested fields:

```cpp
bool diagnosticTestActive;
String diagnosticTestName;
unsigned long diagnosticTestStartedMs;
unsigned long diagnosticTestLastStepMs;
uint16_t diagnosticTestIndex;
ControllerSettings previousSettingsBeforeTest;
```

If copying the entire settings struct is too heavy, store only the values needed to restore:

- mode
- color
- white temperature
- master brightness
- mode-specific relevant settings if needed

## Required Behavior

When a diagnostic test starts:

1. Save previous lighting state.
2. Set diagnostic test active.
3. Run the selected test through the normal frame/output pipeline where possible.
4. Show current test name on diagnostics page.
5. Allow stop/return to previous state.

When returning:

1. Clear diagnostic test active.
2. Restore previous lighting state.
3. Mark settings dirty only if appropriate.

Important:

- Diagnostic tests should not permanently overwrite the user’s chosen mode unless the user explicitly chooses to keep the test state.

## Non-Blocking Pixel Walk

Pixel walk must not use a long blocking loop.

Use `millis()` timing:

```cpp
if (millis() - diagnosticTestLastStepMs >= interval) {
  diagnosticTestIndex = (diagnosticTestIndex + 1) % ledCount;
  diagnosticTestLastStepMs = millis();
}
```

## Integration With Render Pipeline

Diagnostic tests should render into the frame buffer if Phase 1B pipeline exists.

Suggested conceptual structure:

```cpp
void updateAnimationFrame() {
  clearFrame();

  if (diagnosticTestActive) {
    renderDiagnosticTestToFrame();
  } else {
    renderCurrentModeToFrame();
  }

  outputFrameToStrip();
}
```

## Acceptance Criteria

- Red test works.
- Green test works.
- Blue test works.
- Low-brightness white test works.
- Pixel walk works without blocking the UI.
- Alternating pixels works.
- Clear/off works.
- Return to previous state works.
- Tests do not break OTA or web server handling.

---

# 13. Add LED Test UI Controls

## Goal

Expose test tools on the diagnostics page.

## Required Controls

Add buttons:

- Red Test
- Green Test
- Blue Test
- Safe White Test
- Pixel Walk
- Alternating Pixels
- Clear / Off
- Return to Previous State

Show:

- Current diagnostic test name
- Whether a test is active
- Safe brightness value used for tests

## Endpoint Options

You may implement test triggers using `/set` or dedicated endpoints.

Preferred dedicated endpoint:

```text
/api/test?name=red
/api/test?name=green
/api/test?name=blue
/api/test?name=whiteLow
/api/test?name=pixelWalk
/api/test?name=alternating
/api/test?name=clear
/api/test?name=restore
```

Return JSON.

Example success:

```json
{
  "ok": true,
  "testActive": true,
  "testName": "pixelWalk"
}
```

Example error:

```json
{
  "ok": false,
  "error": "Unknown test name"
}
```

## Acceptance Criteria

- Buttons trigger tests.
- Buttons do not require full page reload.
- Current active test status updates.
- Invalid test names are handled safely.

---

# 14. Main UI Link and Minimal Cleanup

## Goal

Make diagnostics discoverable without cluttering the main controller page.

## Required Change

Add a small link or button on the main page:

```text
Diagnostics
```

Place it near:

- OTA link
- Footer
- Settings area
- Header utility area

Do not turn the main page into a cockpit.

Main controller remains for everyday lighting. Diagnostics is for setup/debugging.

## Acceptance Criteria

- User can navigate from main page to diagnostics.
- User can navigate back from diagnostics to main page.

---

# 15. Persistence Updates

## Goal

Persist new power configuration settings.

## Add Settings Fields

Add to `/settings.json` or equivalent:

```json
{
  "supplyVoltage": 5.0,
  "supplyCurrentAmps": 5.0,
  "supplySafetyMarginPercent": 20,
  "controllerOverheadAmps": 0.2
}
```

or integer equivalents:

```json
{
  "supplyMillivolts": 5000,
  "supplyMilliamps": 5000,
  "supplySafetyMarginPercent": 20,
  "controllerOverheadMilliamps": 200
}
```

Use whichever is more appropriate for the existing code.

## Backward Compatibility

If these fields are missing from existing settings:

- Use defaults.
- Do not treat settings as corrupt.
- Save them on next settings write.

## Acceptance Criteria

- Power settings persist after reboot.
- Missing fields use defaults.
- Existing Phase 1A/1B settings still load.

---

# 16. Documentation / Report Requirement

Create or update a short report document.

Suggested file:

```text
BedroomLedController_Phase_1C_Report.md
```

The report should include:

1. Files changed.
2. Summary of `/diagnostics` page.
3. Summary of `/api/diagnostics`.
4. Summary of `/api/power`.
5. Power estimate assumptions.
6. Power supply settings and defaults.
7. Warning logic.
8. LED test tools and safety behavior.
9. Persistence changes.
10. Any UI changes.
11. Compile/test status.
12. Known limitations.
13. Recommended next step: Phase 1D local API cleanup, documentation consolidation, and final Phase 1 hardening.

## Required Honesty

Do not claim real current measurement.

Use language like:

```text
The power page estimates worst-case current using the common WS2812 60 mA per LED assumption. The controller does not measure actual current draw.
```

Do not claim physical LED testing unless actual physical LED testing was performed.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Current State

Before editing, identify:

- Existing settings struct/object
- Existing persistence functions
- Existing output pipeline
- Existing state endpoint
- Existing set endpoint
- Existing UI layout
- Existing OTA/web updater links

Do not rewrite blindly.

## Step 2 — Add Diagnostics Data Helpers

Create helper functions for:

- Uptime formatting
- IP address string
- Color hex string
- Brightness percentage
- Warning generation
- Settings status summary

Keep helpers small.

## Step 3 — Add Power Estimate Model

Add power configuration fields to settings.

Add calculation helpers:

- `getMaxLedCurrent()`
- `getEstimatedCurrentAtBrightness()`
- `getUsableSupplyCurrent()`
- `getRecommendedBrightnessCap()`

Use integer math where practical, but readability matters too.

## Step 4 — Add `/api/diagnostics`

Return compact JSON with controller, LED, settings, output pipeline, and warning information.

## Step 5 — Add `/api/power`

Return compact JSON with power estimate values.

## Step 6 — Add `/diagnostics` HTML Page

Build simple lightweight page.

Include:

- Status cards/sections
- Power form
- Warning area
- LED test buttons
- Links back to main/OTA

## Step 7 — Add Power Settings Update Handling

Support updating supply voltage/current/margin/overhead.

Use `/set` or a dedicated endpoint.

Mark settings dirty after changes.

## Step 8 — Add Diagnostic Test State Machine

Add diagnostic test state.

Render diagnostic tests through the Phase 1B frame pipeline.

Keep pixel walk non-blocking.

## Step 9 — Add Test Trigger Endpoint

Add `/api/test?name=...` or equivalent.

Return JSON.

## Step 10 — Add Main Page Link

Add a small diagnostics link/button to the main UI.

## Step 11 — Compile and Report

Compile for D1 mini / ESP8266.

Create/update Phase 1C report.

---

# Test Checklist

## Compile

- Sketch compiles for D1 mini / ESP8266.

## Existing Routes

- `/` loads.
- `/state` returns valid JSON.
- `/set` still works.
- `/ota` still loads.
- `/update` remains registered.

## New Diagnostics Routes

- `/diagnostics` loads.
- `/api/diagnostics` returns valid JSON.
- `/api/power` returns valid JSON if implemented.

## Diagnostics Page

- Shows hostname.
- Shows IP address.
- Shows Wi-Fi status/RSSI.
- Shows uptime.
- Shows free heap.
- Shows LED count/pin.
- Shows active mode.
- Shows active color/temperature.
- Shows master brightness.
- Shows gamma/calibration values.
- Shows settings load/save status.
- Shows warnings when applicable.

## Power Estimator

- Defaults are sane.
- Supply current can be changed.
- Safety margin can be changed.
- Values persist after reboot.
- Recommended brightness cap calculates correctly.
- Apply recommended cap button works.
- UI clearly says estimate only.

## Warning Logic

- D3/GPIO0 warning appears if applicable.
- Brightness warning appears if above recommended cap.
- Weak Wi-Fi warning appears if RSSI threshold is crossed.
- Settings error warning appears if simulated/tracked.

## LED Test Tools

- Red test works.
- Green test works.
- Blue test works.
- Safe white test works at low brightness.
- Pixel walk runs non-blocking.
- Alternating pixels works.
- Clear/off works.
- Return to previous state works.
- UI remains responsive during tests.
- OTA handling remains present during tests.

## Existing Lighting Modes

After tests, verify existing modes still work:

- Solid Color
- Strobe
- Flash
- Chase
- Wave
- Slow Wave
- Slow Pulse
- Rainbow
- Existing calm modes

## Persistence

- Power config persists.
- Existing settings still persist.
- Missing new fields default safely.

---

# Phase 1C Non-Goals Reminder

Do not implement these yet:

- Scene presets
- Favorite scene bar
- Smooth transitions
- Browser live preview strip
- Palette builder
- Timer actions
- Scheduler
- QR/NFC links
- New lighting modes
- Hardware buttons
- Microphone/music reaction
- Light sensor/room-aware brightness

If tempted, write it in the report as deferred.

The firmware raccoon is not allowed to order extra features without adult supervision.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 1C complete.

Files changed:
- ...

Implemented:
- ...

Diagnostics details:
- /diagnostics: ...
- /api/diagnostics: ...
- /api/power: ...

Power estimator:
- Assumptions: ...
- Defaults: ...
- Recommended cap behavior: ...

LED test tools:
- Tests added: ...
- Safety brightness behavior: ...
- Restore behavior: ...

Preserved:
- ...

Deferred:
- ...

Compile/test status:
- ...

Known limitations:
- ...

Recommended next step:
- Phase 1D — local API cleanup, route consolidation, documentation consolidation, and final Phase 1 hardening.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim actual current measurement unless hardware current sensing exists.

---

# One-Sentence Mission Summary

Add a practical diagnostics page, honest power estimates, configuration warnings, and safe LED wiring tests so the Bedroom LED Controller becomes easier to trust, debug, and keep from becoming an accidental miniature sun.

