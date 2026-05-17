# Bedroom LED Controller — Phase 1A Codex Prompt

## Phase 1A Title

**Settings Foundation + Master Brightness Control**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 1A** of a larger upgrade plan. The goal is to add the foundational runtime settings system, persistent settings storage, and a real global master brightness control.

Do **not** implement the rest of Phase 1 yet. This prompt is intentionally narrow so the project does not turn into firmware lasagna wearing RGB sunglasses.

## Current Project Context

The project already includes:

- ESP8266 / D1 mini target
- WS2812 LED strip control
- Existing LED data pin, currently likely `D3`
- Existing local web UI
- Existing color picker
- Existing white-temperature slider
- Existing mode dropdown
- Existing animation modes
- Existing `/state` endpoint
- Existing `/set` endpoint
- Existing OTA support
- Existing browser firmware update route

The current sketch is a single Arduino `.ino` file unless the project has already been split.

## Hard Scope Boundary

This prompt only covers:

1. Central runtime settings model
2. Persistent settings storage
3. Debounced settings save behavior
4. Master brightness runtime setting
5. Master brightness UI control
6. `/state` and `/set` updates for master brightness
7. Basic boot behavior setting scaffold, if simple
8. Documentation/report update
9. Compile check

Do **not** implement yet:

- Full render buffer pipeline
- Gamma correction
- RGB calibration
- Diagnostics page
- Power estimator
- LED test tools
- New lighting modes
- Scene presets
- Favorite bar
- Timers
- Scheduler
- Palette builder
- Smooth transitions
- Physical buttons
- Microphones
- Light sensors
- New hardware assumptions

If you see a tempting feature from a later phase, document it as deferred instead of implementing it. The raccoon must remain outside the firmware pantry.

---

# Core Requirements

## 1. Create a Central Runtime Settings Model

Refactor the controller state into one central settings structure/object.

This should become the single source of truth for core user-adjustable settings.

Suggested structure:

```cpp
struct ControllerSettings {
  String mode;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint16_t whiteKelvin;
  bool usingWhiteTemperature;
  uint8_t masterBrightness;

  // Existing mode options should be included if currently present.
  uint16_t strobeDelayMs;
  uint16_t flashOffDelayMs;
  uint16_t chaseLoopSeconds;
  uint16_t waveLoopSeconds;
  uint16_t slowPulseSeconds;
  uint8_t slowPulseCount;
  uint16_t rainbowLoopSeconds;

  // Phase 1A boot behavior scaffold.
  String bootBehavior;
};
```

Use names that fit the existing code, but keep the idea: **one central settings object**.

Do not allow multiple unrelated globals to keep drifting out of sync if they represent the same setting.

### Required Settings To Track

At minimum, centralize:

- Current mode
- Current active RGB color
- Current white temperature value
- Whether the active color came from RGB picker or white-temperature slider, if supported by existing logic
- Master brightness
- Existing mode-specific controls already present in the sketch
- Boot behavior scaffold, if feasible in this pass

### Acceptance Criteria

- Existing web UI and animation logic read from the central settings model where practical.
- Existing behavior is preserved.
- There is no duplicate/conflicting brightness state.
- The code has a clear place to add future settings.

---

# 2. Add Master Brightness Runtime Setting

## Goal

Add a true global master brightness setting that can cap output brightness across all modes.

This is not meant to redesign the entire render pipeline yet. That happens in Phase 1B.

For Phase 1A, implement master brightness in the safest minimal way that works with the current code.

## Required Behavior

Add:

```cpp
uint8_t masterBrightness;
```

or include it inside the central settings model.

Recommended range:

```text
0–255
```

Recommended default:

- If the sketch already has `MASTER_BRIGHTNESS`, use that as the default.
- If the current default is `255`, consider changing the default to `180` for safety, but only if it will not surprise the current code. If changing the default, document it clearly.

## Important Implementation Note

If the project already uses `strip.setBrightness(...)`, be careful.

`Adafruit_NeoPixel::setBrightness()` is not always ideal as a constantly changing per-frame brightness control because it can rescale stored pixel values. If the existing code already uses it safely, you may preserve it for Phase 1A. However, prepare the code so Phase 1B can move brightness scaling into a centralized output pipeline.

For Phase 1A, acceptable approaches are:

### Preferred If Easy

Apply master brightness immediately before showing LEDs using a helper/wrapper that all modes pass through.

Example conceptual helper:

```cpp
uint8_t scaleByMaster(uint8_t channel) {
  return (uint16_t(channel) * settings.masterBrightness) / 255;
}
```

Then use it in a central pixel-setting helper if one exists or can be added without rewriting every mode.

### Acceptable Temporary Phase 1A Approach

Use `strip.setBrightness(settings.masterBrightness)` if that is the least risky way to make all existing modes respond to brightness before the Phase 1B render pipeline refactor.

If you choose this approach, clearly document:

- It is temporary.
- Phase 1B should move brightness scaling into the final output pipeline.
- Existing modes remain visually compatible for now.

## Acceptance Criteria

- Master brightness affects Solid Color.
- Master brightness affects existing animations.
- Changing master brightness does not reset the selected mode.
- Brightness can be set to low values safely.
- Brightness persists after reboot once persistence is implemented.
- The implementation does not add blocking delays.

---

# 3. Add Master Brightness UI Control

## Goal

Add a master brightness slider to the main web UI.

## Required UI Behavior

The main page should include a control labeled clearly, such as:

```text
Master Brightness
```

Recommended slider:

```html
<input type="range" min="0" max="255">
```

Display both:

- Raw value, such as `180 / 255`
- Percentage, such as `71%`

When changed:

- Send update to the ESP8266 using the existing `/set` endpoint or compatible mechanism.
- Update the displayed value live.
- Do not require a page reload.
- Do not interrupt the active mode.

## UI Placement

Place master brightness near the top of the controls, because it is a primary safety and comfort setting.

Suggested order:

1. On/off or mode summary, if present
2. Master brightness
3. Color picker
4. White temperature
5. Mode selection
6. Mode-specific controls

Do not make the UI cluttered. This is a bedroom controller, not an airplane cockpit designed by a squirrel.

## Acceptance Criteria

- Slider appears on the main page.
- Slider initializes from `/state`.
- Moving slider updates brightness quickly.
- Current value and percent are visible.
- Page remains usable on mobile.

---

# 4. Update `/state` Endpoint

## Goal

Expose the new settings values through the existing state endpoint.

## Required JSON Fields

Add at least:

```json
{
  "masterBrightness": 180,
  "masterBrightnessPercent": 71,
  "bootBehavior": "restore"
}
```

Use whatever field naming style the existing sketch uses, but be consistent.

If `/state` already returns mode, color, white temperature, and mode-specific settings, preserve those fields.

## Acceptance Criteria

- `/state` returns valid JSON.
- Existing frontend code still works.
- New brightness slider initializes from `/state`.
- JSON is compact enough for ESP8266 use.

---

# 5. Update `/set` Endpoint

## Goal

Allow the existing `/set` endpoint to update `masterBrightness`.

## Required Behavior

Support a request like one of these, depending on the existing convention:

```text
/set?brightness=180
```

or

```text
/set?masterBrightness=180
```

Prefer `masterBrightness` if adding new naming, but preserve any existing `brightness` behavior if it already exists.

Validate input:

- Missing value: ignore or return error depending on existing pattern.
- Non-numeric value: reject or ignore safely.
- Below 0: clamp to 0 or reject safely.
- Above 255: clamp to 255 or reject safely.

Recommended behavior:

- Clamp to 0–255.
- Update runtime setting.
- Apply brightness immediately.
- Mark settings dirty for persistence.
- Return JSON or existing response style.

Do not let malformed input crash the controller.

## Acceptance Criteria

- `/set?masterBrightness=...` or equivalent works.
- Invalid values do not crash.
- Brightness updates immediately.
- Brightness save is queued/debounced.
- Existing `/set` controls still work.

---

# 6. Add Persistent Settings Storage

## Goal

Settings should survive reboot/power loss.

## Preferred Storage

Use **LittleFS**.

Suggested file:

```text
/settings.json
```

If the project does not currently include LittleFS:

- Add the needed include.
- Initialize LittleFS in `setup()`.
- Handle init failure safely.

## Required Load Behavior

During boot:

1. Start with safe default settings.
2. Initialize LittleFS.
3. Try to load `/settings.json`.
4. If the file exists and is valid, apply stored settings.
5. If missing/corrupt/incomplete, use defaults for missing values.
6. Track load status for future diagnostics.

Do not crash if the settings file is corrupt.

## Required Save Behavior

When the user changes a setting:

1. Update runtime setting immediately.
2. Mark settings dirty.
3. Save after a debounce interval.
4. Avoid repeated writes if values have not changed.

Recommended debounce:

```text
500–1500 ms
```

Suggested variables:

```cpp
bool settingsDirty = false;
unsigned long settingsDirtySince = 0;
const unsigned long SETTINGS_SAVE_DEBOUNCE_MS = 1000;
```

Then in `loop()`:

```cpp
if (settingsDirty && millis() - settingsDirtySince >= SETTINGS_SAVE_DEBOUNCE_MS) {
  saveSettings();
}
```

Do not save from inside tight animation code.

## JSON Contents

Save at least:

```json
{
  "version": 1,
  "mode": "Solid",
  "red": 255,
  "green": 160,
  "blue": 80,
  "whiteKelvin": 2700,
  "usingWhiteTemperature": true,
  "masterBrightness": 180,
  "bootBehavior": "restore"
}
```

Also save existing mode-specific settings if they currently exist.

Examples:

```json
{
  "strobeDelayMs": 500,
  "flashOffDelayMs": 1000,
  "chaseLoopSeconds": 5,
  "waveLoopSeconds": 15,
  "slowPulseSeconds": 20,
  "slowPulseCount": 5,
  "rainbowLoopSeconds": 10
}
```

Use the actual names from the current sketch where appropriate.

## Acceptance Criteria

- Change color, mode, brightness.
- Reboot.
- Values restore according to boot behavior.
- Corrupt/missing settings file falls back safely.
- Settings saves are debounced.
- No constant flash writes during animation.

---

# 7. Add Basic Boot Behavior Scaffold

## Goal

Add the first version of boot behavior handling, but keep it simple.

## Required Options

Implement these if feasible in this pass:

```text
restore
startOff
warmDim
```

### restore

Restore saved mode/color/brightness from persistent settings.

### startOff

Boot with LEDs off while keeping web UI and OTA active.

### warmDim

Boot into a safe warm dim solid color.

Suggested warm dim values:

- Mode: Solid
- White temperature: 2200K–2700K, or closest supported value
- Master brightness: 30–60

## UI Requirement

If simple, add a boot behavior selector to the web UI or a small settings section.

If adding the UI would bloat Phase 1A too much, implement the backend setting and expose it in `/state`, then document that a fuller UI can arrive in Phase 1C/diagnostics.

## Acceptance Criteria

- Boot behavior value is saved.
- Restore works.
- Start off works if implemented.
- Warm dim works if implemented.
- Web UI and OTA remain active in all cases.

---

# 8. Avoid Breaking OTA and Existing Routes

## Required Behavior

Do not remove or break:

- ArduinoOTA handling
- Browser update route
- `/ota`
- `/update`
- `/`
- `/state`
- `/set`

Ensure the main loop still calls whatever is needed for:

- Web server handling
- OTA handling
- Animation update
- Debounced settings save

Conceptual loop:

```cpp
void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  updateAnimation();
  serviceSettingsSave();
}
```

Use the actual existing server/OTA structure.

## Acceptance Criteria

- OTA code remains present.
- Web updater remains present.
- Main page loads.
- `/state` loads.
- `/set` works.
- Animations still run.

---

# 9. Documentation / Report Requirement

After implementation, update or create a short report document.

Suggested file:

```text
BedroomLedController_Phase_1A_Report.md
```

The report should include:

1. Files changed.
2. Summary of settings model changes.
3. Summary of persistent storage behavior.
4. Master brightness behavior.
5. Boot behavior behavior.
6. Any compatibility notes.
7. Any known limitations.
8. Whether the sketch compiled for D1 mini / ESP8266.
9. What should happen in Phase 1B.

## Required Honesty

Do not claim hardware testing unless actual physical LED testing was performed.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

---

# 10. Suggested Implementation Order

Follow this order:

## Step 1 — Inventory Current State

Before changing code, identify:

- Current state variables
- Current mode names
- Current animation option variables
- Current `/state` JSON fields
- Current `/set` parameters
- Current brightness behavior
- Current OTA setup

Do not blindly rewrite the whole sketch.

## Step 2 — Add Settings Struct

Create central settings object and migrate existing state fields into it carefully.

Preserve existing names where that reduces breakage.

## Step 3 — Add LittleFS Persistence

Add load/save functions.

Suggested functions:

```cpp
void loadSettings();
void saveSettings();
void markSettingsDirty();
void serviceSettingsSave();
void applyDefaultSettings();
```

## Step 4 — Add Master Brightness Runtime Behavior

Add brightness value to settings.

Make existing output respond to it using the safest current approach.

## Step 5 — Add UI Slider

Update HTML/CSS/JS to show and control master brightness.

## Step 6 — Update `/state` and `/set`

Expose and update new brightness/boot fields.

## Step 7 — Add Boot Behavior Scaffold

Implement restore/startOff/warmDim if feasible.

## Step 8 — Test Existing Features

Confirm:

- Main UI loads
- Color picker works
- White temperature works
- Mode selection works
- Existing animations still run
- OTA code still exists

## Step 9 — Compile

Compile for D1 mini / ESP8266.

Fix compile errors.

## Step 10 — Report

Create/update the Phase 1A report.

---

# Test Checklist

## Compile

- Sketch compiles for D1 mini / ESP8266.

## Web UI

- Main page loads.
- Master brightness slider appears.
- Slider value initializes from state.
- Slider sends updates.
- Displayed percent updates.

## State Endpoint

- `/state` returns valid JSON.
- JSON includes `masterBrightness` or agreed equivalent.
- Existing frontend fields still exist.

## Set Endpoint

- `/set?masterBrightness=0` works or clamps safely.
- `/set?masterBrightness=180` works.
- `/set?masterBrightness=255` works.
- Invalid brightness does not crash.

## Persistence

- Change brightness, reboot, confirm restored.
- Change color, reboot, confirm restored if boot behavior is restore.
- Change mode, reboot, confirm restored if boot behavior is restore.
- Missing settings file does not crash.
- Corrupt settings file does not crash.

## OTA / Web Update

- OTA code remains present.
- Browser update route remains present.
- `/ota` still loads.
- `/update` remains registered.

## Existing Lighting Modes

Verify at least:

- Solid Color still works.
- Strobe still works.
- Flash still works.
- Chase still works.
- Wave still works.
- Slow Wave still works.
- Slow Pulse still works.
- Rainbow still works.
- Existing calm modes still work.

## Brightness Behavior

- Master brightness affects Solid Color.
- Master brightness affects Rainbow.
- Master brightness affects at least one wave/chase mode.
- Low brightness does not produce weird resets.
- Brightness changes while animation is running do not stop the animation.

---

# Phase 1A Non-Goals Reminder

Do not get excited and implement these yet:

- Scene presets
- Smooth transitions
- Palette builder
- Browser animation preview
- Diagnostics page
- Power estimator
- Gamma correction
- RGB calibration
- LED test tools
- New calm modes
- Scheduler
- Timer actions
- Physical controls
- Music reaction
- Light sensor behavior

Write down any tempting ideas in the report as deferred.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 1A complete.

Files changed:
- ...

Implemented:
- ...

Preserved:
- ...

Deferred:
- ...

Compile/test status:
- ...

Known limitations:
- ...

Recommended next step:
- Phase 1B — render/output pipeline + gamma/calibration foundation.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

---

# One-Sentence Mission Summary

Implement the central settings brain, persistent memory, and global master brightness control so the Bedroom LED Controller stops being a forgetful RGB goblin and starts becoming a real local-first lighting product.

