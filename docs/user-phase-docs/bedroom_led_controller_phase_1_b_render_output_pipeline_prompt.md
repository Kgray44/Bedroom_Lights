# Bedroom LED Controller — Phase 1B Codex Prompt

## Phase 1B Title

**Render / Output Pipeline + Gamma and Calibration Foundation**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 1B** of the larger upgrade plan. Phase 1A should already have added or started the foundation for:

- A central runtime settings model
- Persistent settings storage
- Debounced settings saves
- Master brightness
- Master brightness UI control
- `/state` and `/set` support for master brightness
- A basic boot behavior scaffold

The goal of Phase 1B is to refactor the LED rendering/output path so every lighting mode flows through one centralized final output pipeline.

This is the structural phase that prepares the project for later:

- Smooth transitions
- Scene recalls
- Palette support
- Browser preview
- Diagnostics
- Power safety estimates
- New calm lighting modes

Do **not** add new lighting modes in this phase.

Do **not** implement scene presets, transitions, diagnostics, timers, schedules, or palette builder yet.

This phase is firmware spinal surgery. Keep the patient alive. Do not let the RGB raccoon grab the scalpel.

---

# Current Project Context

The project already includes:

- ESP8266 / D1 mini target
- WS2812 LED strip control using Adafruit NeoPixel
- Existing animation modes
- Existing calm animation modes
- Existing web UI
- Existing `/state` endpoint
- Existing `/set` endpoint
- Existing OTA support
- Existing browser firmware update route
- Phase 1A settings and master brightness work, or equivalent foundation

Existing routes that should remain functional:

- `/`
- `/state`
- `/set`
- `/ota`
- `/update`

The project should remain local-first and should not require internet/cloud behavior beyond normal local Wi-Fi use and OTA discovery.

---

# Hard Scope Boundary

This prompt covers:

1. Central LED frame buffer
2. Frame helper functions
3. Refactor existing modes to render into the frame buffer
4. Central final output pipeline
5. Master brightness applied in the final output path
6. Gamma correction foundation
7. RGB channel calibration foundation
8. Optional minimal UI/settings support for gamma and RGB gains, if Phase 1A settings make it easy
9. Preservation of all existing modes and routes
10. Compile/test/report

Do **not** implement yet:

- Scene presets
- Favorite scene bar
- Smooth transitions
- Full diagnostics page
- Power estimator
- LED test tools
- Browser live preview
- Palette builder
- New lighting modes
- Timers
- Scheduler
- Physical buttons
- Microphone support
- Light sensor support
- External hardware assumptions

If you see a tempting feature from a later phase, document it as deferred. Do not implement it. The feature goblin must wait in line like everyone else.

---

# Phase 1B Main Goal

All LED modes should render into a central frame buffer first.

Then a single final output function should apply:

1. RGB calibration gains
2. Master brightness scaling
3. Gamma correction
4. Final write to the physical NeoPixel strip

The intended conceptual structure is:

```cpp
void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  updateAnimationFrame();
  serviceSettingsSave();
}

void updateAnimationFrame() {
  clearFrame();
  renderCurrentModeToFrame();
  outputFrameToStrip();
}

void outputFrameToStrip() {
  for each pixel in frameBuffer:
    apply calibration
    apply master brightness
    apply gamma correction
    strip.setPixelColor(...)
  strip.show();
}
```

Exact function names may differ, but the architecture should be centralized and easy to extend.

---

# 1. Add a Central Frame Buffer

## Goal

Create a central in-memory LED frame buffer that represents the raw intended RGB value for each LED before final output processing.

## Recommended Representation

Use a small struct:

```cpp
struct RgbPixel {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

RgbPixel frameBuffer[LED_COUNT];
```

However, note that `LED_COUNT` may be a configurable constant or runtime value in the current sketch.

If `LED_COUNT` is compile-time constant, a static array is acceptable.

If LED count can vary, use the safest existing pattern in the sketch. Avoid large dynamic allocations and avoid heap fragmentation.

For ESP8266 sanity, prefer simple fixed-size/static allocation where practical.

## Important Memory Rule

Do not add unnecessary multiple large buffers yet.

Phase 3 will need previous/current buffers for smooth transitions, but Phase 1B only needs one primary frame buffer unless the current code structure strongly benefits from a small temporary buffer.

Do not prematurely allocate a giant transition system.

## Acceptance Criteria

- Frame buffer exists.
- All modes can write raw RGB values into it.
- Final output is generated from the buffer.
- Memory use remains appropriate for ESP8266.

---

# 2. Add Frame Helper Functions

## Goal

Avoid having every mode manually poke raw arrays in inconsistent ways.

Add helper functions that make rendering modes easy and safe.

## Required Helpers

Implement helpers equivalent to:

```cpp
void clearFrame();
void setFramePixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void setFramePixelColor(uint16_t index, uint32_t color);
void fillFrame(uint8_t r, uint8_t g, uint8_t b);
void fillFrameColor(uint32_t color);
uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b);
```

Use names that fit the existing codebase.

## Safety Requirements

`setFramePixel` must bounds-check the LED index.

If an invalid pixel index is requested:

- Do not crash.
- Ignore safely.

This matters because animation math can sometimes produce edge-case indexes when widths, centers, or wave offsets change.

## Optional Helpers

Useful additional helpers:

```cpp
uint8_t clamp8(int value);
uint8_t scale8(uint8_t value, uint8_t scale);
RgbPixel blendPixels(RgbPixel a, RgbPixel b, uint8_t amount);
RgbPixel scalePixel(RgbPixel p, uint8_t scale);
```

Do not overbuild if not needed.

## Acceptance Criteria

- Existing modes use frame helpers where practical.
- Bounds are safe.
- Helper names are clear.
- Future transition/palette work will be easier.

---

# 3. Refactor Existing Modes to Render Into the Frame Buffer

## Goal

Existing modes should no longer directly own final physical output.

Each mode should render its raw intended frame into the central buffer.

## Existing Modes to Preserve

Make sure all current modes still work, including the project’s existing list such as:

- Solid Color
- Strobe
- Flash
- Chase
- Wave
- Slow Wave
- Slow Pulse
- Rainbow
- Dawn Bloom
- Moon Tide
- Velvet Aurora
- Lantern Drift
- Rain Glass

If the sketch contains additional modes, preserve those too.

## Refactor Strategy

For each mode:

- Keep its current visual behavior as close as possible.
- Replace direct `strip.setPixelColor(...)` calls with `setFramePixel(...)` or equivalent.
- Avoid calling `strip.show()` inside individual mode functions.
- Avoid calling `strip.setBrightness()` inside individual mode functions.
- Let the centralized output path handle final brightness/gamma/calibration.

## Important Rule

There should ideally be exactly one place in the animation loop where `strip.show()` is called for normal animation output.

Exceptions may exist for OTA/setup/diagnostic edge cases, but normal modes should not each call `strip.show()` independently.

## Strobe / Flash Handling

Strobe and Flash may render either full active color or black depending on timing.

They should still render to the buffer like any other mode:

```cpp
if (isOnPhase) {
  fillFrame(activeColor);
} else {
  clearFrame();
}
```

Do not use blocking delays.

## Rainbow Handling

Rainbow intentionally ignores the active user color.

That behavior should remain.

But the final brightness/gamma/calibration should still apply in the output pipeline.

## Acceptance Criteria

- Existing modes still visually behave similarly.
- Existing mode controls still affect the modes.
- No normal mode directly calls `strip.show()` anymore, unless there is a documented unavoidable exception.
- No normal mode bypasses master brightness.
- Web server and OTA remain responsive.

---

# 4. Central Final Output Pipeline

## Goal

Create one final function that turns the raw frame buffer into actual LED output.

Suggested function names:

```cpp
void outputFrameToStrip();
void showFrame();
void renderAndShowFrame();
```

Use whatever name best fits the sketch.

## Required Processing Order

The final output order should be:

1. Raw mode-rendered RGB value
2. RGB calibration gains
3. Master brightness scaling
4. Gamma correction
5. Write to Adafruit NeoPixel strip
6. `strip.show()`

Recommended conceptual code:

```cpp
void outputFrameToStrip() {
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    uint8_t r = frameBuffer[i].r;
    uint8_t g = frameBuffer[i].g;
    uint8_t b = frameBuffer[i].b;

    r = applyChannelGain(r, settings.redGain);
    g = applyChannelGain(g, settings.greenGain);
    b = applyChannelGain(b, settings.blueGain);

    r = scaleByMasterBrightness(r);
    g = scaleByMasterBrightness(g);
    b = scaleByMasterBrightness(b);

    if (settings.gammaEnabled) {
      r = gamma8(r);
      g = gamma8(g);
      b = gamma8(b);
    }

    strip.setPixelColor(i, strip.Color(r, g, b));
  }

  strip.show();
}
```

Exact code may differ, but the order should be documented if changed.

## Master Brightness Rule

Master brightness should now be applied here, not scattered across modes.

If Phase 1A used `strip.setBrightness()` as a temporary implementation, Phase 1B should either:

- Replace it with pipeline-based scaling, or
- Clearly document why it remains and how it avoids double-scaling.

Avoid double-applying brightness.

If using pipeline scaling, set NeoPixel brightness to full/raw safe baseline, such as 255, and let the pipeline scale channel values.

## Acceptance Criteria

- All modes route through the final output pipeline.
- Master brightness affects all modes consistently.
- Brightness is not applied twice.
- Gamma and calibration apply consistently.
- `strip.show()` is centralized for normal animations.

---

# 5. Gamma Correction Foundation

## Goal

Add gamma correction in the final output stage so dimming looks smoother and more natural.

## Required Behavior

Add a gamma correction function or lookup table.

Recommended:

```cpp
uint8_t gamma8(uint8_t value);
```

or:

```cpp
const uint8_t gammaTable[256] PROGMEM = { ... };
```

For ESP8266, a table in PROGMEM is acceptable if memory usage is reasonable.

## Default Behavior

Recommended default:

- Gamma enabled: true
- Gamma value: approximately 2.2

If implementing a dynamic gamma value is too much for this phase, use a fixed gamma table and a boolean setting.

## Settings Integration

Add to settings if Phase 1A settings model exists:

```cpp
bool gammaEnabled;
```

Optional:

```cpp
uint8_t gammaPreset;
```

Possible presets:

- 18 for gamma 1.8
- 20 for gamma 2.0
- 22 for gamma 2.2
- 24 for gamma 2.4

But do not overcomplicate unless the existing code is ready.

## UI Integration

Minimum:

- Include `gammaEnabled` in `/state` if added.
- Allow `/set?gammaEnabled=1` or equivalent if simple.

Optional:

- Add a small toggle in a settings/calibration area.

If the UI is not ready, it is acceptable to add backend support and document that a full calibration UI will come later.

## Acceptance Criteria

- Gamma correction is applied after master brightness.
- Gamma can be enabled/disabled if the setting is implemented.
- Low brightness appears smoother.
- Existing modes still work.

---

# 6. RGB Channel Calibration Foundation

## Goal

Add basic channel gain controls so the strip can be calibrated if one color channel is too strong or weak.

## Required Settings

Add to settings:

```cpp
uint8_t redGain;
uint8_t greenGain;
uint8_t blueGain;
```

Recommended range:

```text
0–255
```

Default:

```text
255, 255, 255
```

Meaning:

- 255 = full channel strength
- 128 = roughly half channel strength
- 0 = channel disabled

## Required Function

Add helper:

```cpp
uint8_t applyChannelGain(uint8_t channelValue, uint8_t gain) {
  return (uint16_t(channelValue) * gain) / 255;
}
```

## Processing Order

Apply calibration before master brightness and gamma.

Recommended:

1. Raw frame color
2. Channel gain calibration
3. Master brightness
4. Gamma correction
5. Strip output

## UI / Endpoint Integration

Minimum acceptable:

- Store defaults in settings.
- Persist values.
- Include gains in `/state`.

Better:

- Add `/set?redGain=...&greenGain=...&blueGain=...`
- Add simple controls in a small settings/calibration area.

Do not build a huge diagnostics page yet. That is Phase 1C.

A small calibration subsection on the main page is acceptable if it does not clutter the UI.

## Acceptance Criteria

- RGB gains apply to all modes.
- Defaults preserve existing appearance.
- Gains persist if changed.
- White-temperature output is also affected consistently.
- No mode bypasses calibration.

---

# 7. Settings Persistence Updates

## Goal

If Phase 1A added persistent settings, extend it to save/load Phase 1B settings.

## Add These Fields

Add to settings JSON:

```json
{
  "gammaEnabled": true,
  "redGain": 255,
  "greenGain": 255,
  "blueGain": 255
}
```

Optional:

```json
{
  "gammaPreset": 22
}
```

## Backward Compatibility

If an existing `/settings.json` does not include these fields:

- Use safe defaults.
- Do not treat the file as corrupt.
- Do not erase other saved settings unnecessarily.

## Acceptance Criteria

- Old Phase 1A settings still load.
- New fields get defaults when missing.
- New fields save correctly after changes.
- Settings file version handling remains sane.

---

# 8. Update `/state` and `/set` Carefully

## `/state` Requirements

Expose new pipeline-related settings:

```json
{
  "masterBrightness": 180,
  "gammaEnabled": true,
  "redGain": 255,
  "greenGain": 255,
  "blueGain": 255
}
```

Preserve all existing fields used by the UI.

## `/set` Requirements

Support updates if practical:

```text
/set?gammaEnabled=1
/set?redGain=255
/set?greenGain=240
/set?blueGain=230
```

Validation:

- Gamma enabled should accept sane values such as `0`, `1`, `true`, `false` if easy.
- RGB gains should clamp or reject outside 0–255.
- Malformed inputs should not crash.

When values change:

- Update runtime settings.
- Mark settings dirty for debounced save.
- Apply immediately on next frame.

## Acceptance Criteria

- `/state` remains valid JSON.
- Existing UI does not break.
- New values can be controlled if endpoints are implemented.
- Invalid values are handled safely.

---

# 9. Main Loop Responsiveness

## Goal

The new pipeline must not make the ESP8266 sluggish.

## Requirements

The main loop must continue to service:

- Web server requests
- OTA handling
- Browser update support
- Animation rendering
- Debounced settings save

Avoid:

- Long blocking loops
- `delay()` in animations
- Heavy string building every frame
- Dynamic allocation every frame
- Excessive floating-point math where simple integer math works

Some floating-point math may already exist in wave modes. Preserve if needed, but do not make it dramatically worse.

## Acceptance Criteria

- Main UI remains responsive while animations run.
- OTA handling remains present.
- Animation update cadence remains reasonable.
- No new blocking delay behavior is introduced.

---

# 10. Preserve Existing Visual Behavior

## Goal

Phase 1B should not be a redesign of the lighting modes.

The goal is architectural cleanup while preserving the look.

## Requirements

For each existing mode:

- Keep speed controls working.
- Keep mode-specific behavior working.
- Keep color behavior working.
- Keep white-temperature behavior working.
- Keep Rainbow color override behavior.
- Keep strobe/flash timing behavior.
- Keep calm modes visually recognizable.

Small visual differences are acceptable due to gamma correction and centralized brightness, but do not intentionally redesign the modes.

If gamma changes the appearance significantly, document it.

## Acceptance Criteria

- The controller still feels like the same project after Phase 1B.
- It is just cleaner, safer, and easier to extend.

---

# 11. Documentation / Report Requirement

Create or update a short report document.

Suggested file:

```text
BedroomLedController_Phase_1B_Report.md
```

The report should include:

1. Files changed.
2. Summary of the frame buffer architecture.
3. Summary of final output pipeline order.
4. How master brightness is applied now.
5. Whether `strip.setBrightness()` is still used and why.
6. Gamma correction details.
7. RGB calibration details.
8. Settings persistence additions.
9. Any modes that required special handling.
10. Compile/test status.
11. Known limitations.
12. Recommended next step: Phase 1C diagnostics, power estimator, and LED test tools.

## Required Honesty

Do not claim physical LED testing unless actual physical LED testing was performed.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Current Output Flow

Before editing, identify:

- Where each mode sets pixels
- Where `strip.show()` is called
- Whether `strip.setBrightness()` is used
- How active color is represented
- How white-temperature color is calculated
- How Rainbow generates colors
- How mode timing works

Do not rewrite blindly.

## Step 2 — Add Frame Buffer and Helpers

Add:

- `RgbPixel` or equivalent
- `frameBuffer`
- `clearFrame()`
- `setFramePixel()`
- `fillFrame()`
- color conversion helpers as needed

At this step, do not convert every mode yet.

## Step 3 — Add Central Output Function

Add `outputFrameToStrip()` or equivalent.

Initially test it with Solid Color or a simple fill.

## Step 4 — Move Master Brightness Into Output Function

Ensure master brightness is applied in the central output function.

Avoid double scaling.

If `strip.setBrightness()` was used in Phase 1A, decide whether to remove or neutralize it.

Document the decision.

## Step 5 — Add Gamma and Calibration Helpers

Add:

- `applyChannelGain()`
- `gamma8()` or gamma table
- settings fields for gamma/gains
- default values

Apply these in final output.

## Step 6 — Convert Modes One Group at a Time

Recommended order:

1. Solid Color
2. Strobe / Flash
3. Chase / Wave
4. Slow Wave / Slow Pulse
5. Rainbow
6. Existing calm modes

After each group, compile or sanity-check.

## Step 7 — Remove Stray Direct Output Calls

Search for:

- `strip.setPixelColor`
- `strip.show`
- `strip.setBrightness`

Decide whether each remaining call is valid.

Normal modes should use the frame pipeline.

## Step 8 — Update State/Set/Persistence

Add gamma/gain fields to:

- Settings struct
- Defaults
- Load/save JSON
- `/state`
- `/set`, if endpoint control is implemented

## Step 9 — UI Minimal Controls, If Practical

If easy, add a small calibration/settings area:

- Gamma correction toggle
- Red gain slider
- Green gain slider
- Blue gain slider
- Reset calibration button

If this creates too much UI churn, defer the full UI to Phase 1C diagnostics/settings page, but keep backend fields ready.

## Step 10 — Compile and Report

Compile for D1 mini / ESP8266.

Update Phase 1B report.

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

## Existing Controls

- Color picker still changes active color.
- White temperature slider still changes active color/white.
- Mode dropdown still changes modes.
- Mode-specific controls still work.
- Master brightness slider from Phase 1A still works.

## Existing Modes

Verify at least:

- Solid Color
- Strobe
- Flash
- Chase
- Wave
- Slow Wave
- Slow Pulse
- Rainbow
- Dawn Bloom
- Moon Tide
- Velvet Aurora
- Lantern Drift
- Rain Glass

If any mode is missing from the actual sketch, document that rather than pretending it was tested.

## Pipeline Behavior

- All normal modes route through the frame buffer.
- Master brightness affects all modes.
- No mode bypasses master brightness.
- `strip.show()` is centralized for normal animation output.
- Brightness is not double-applied.

## Gamma

- Gamma correction applies after brightness.
- Gamma can be enabled/disabled if implemented.
- Low brightness still renders visibly.
- Gamma defaults do not make the strip unusably dim.

## RGB Calibration

- Red gain affects red output.
- Green gain affects green output.
- Blue gain affects blue output.
- Defaults preserve original look.
- Gains persist if persistence is updated.

## Responsiveness

- Web UI remains responsive while animations run.
- OTA handling remains present.
- No blocking animation delays introduced.

---

# Phase 1B Non-Goals Reminder

Do not implement these yet:

- Scene presets
- Favorite bar
- Smooth transitions
- Browser preview strip
- Diagnostics page
- Power supply wizard
- LED test tools
- Palette builder
- Timers
- Scheduler
- New lighting modes
- Hardware controls
- Music reaction
- Room-aware brightness

If tempted, write it in the report as deferred.

The firmware goblin may request snacks. Deny it.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 1B complete.

Files changed:
- ...

Implemented:
- ...

Preserved:
- ...

Pipeline details:
- Frame buffer representation: ...
- Output order: ...
- Master brightness handling: ...
- Gamma handling: ...
- RGB calibration handling: ...

Deferred:
- ...

Compile/test status:
- ...

Known limitations:
- ...

Recommended next step:
- Phase 1C — diagnostics page, power estimator, and LED test tools.
```

Be honest and specific.

Do not claim hardware testing unless it actually happened.

---

# One-Sentence Mission Summary

Refactor the Bedroom LED Controller so every mode renders into one central frame buffer and every LED output passes through one clean brightness, calibration, and gamma pipeline before touching the strip.

