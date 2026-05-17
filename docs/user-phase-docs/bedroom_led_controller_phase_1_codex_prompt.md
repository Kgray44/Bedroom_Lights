# Bedroom LED Controller — Phase 1 Codex Prompt

## Phase 1 Title

**Foundation, Safety, and Output Pipeline**

## Purpose

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This phase must **not** add flashy new lighting modes yet. The purpose is to build the internal foundation that will support the later scene system, smooth transitions, palettes, diagnostics, timers, schedules, and the giant cozy goblin library of calm lighting modes.

Think of this phase as building the electrical/mechanical frame of the project before bolting on the LED disco cathedral.

## Current Project Context

The project already includes:

- A D1 mini / ESP8266 target.
- A WS2812 LED strip.
- Existing LED data output on `D3` unless changed by the user.
- Existing local web UI served by the ESP8266.
- Existing OTA support.
- Existing browser firmware update support.
- Existing color picker.
- Existing white-temperature slider.
- Existing animation modes.
- Existing calm animation modes.
- Existing state and settings endpoints.

The project should remain a **local-first bedroom LED controller**.

Do not introduce cloud dependencies.
Do not require new hardware.
Do not remove OTA.
Do not make the web UI less responsive.
Do not break existing modes.

## Hard Rules

### No New Hardware

Do not implement anything requiring:

- Physical buttons
- Rotary encoders
- Touch sensors
- Microphones
- Light sensors
- Additional microcontrollers
- External displays
- New wiring assumptions

Software hooks may be added only if they are clean, harmless, and clearly documented.

### No Blocking Animation Logic

Do not add blocking `delay()`-based behavior to animation rendering.

The controller must continue to service:

- Web requests
- OTA handling
- Browser update handling
- Animation updates

Use `millis()`-based timing.

### Preserve Existing Behavior

Existing modes must still work unless there is a documented reason to change them.

Existing core routes should remain compatible where possible:

- `/`
- `/state`
- `/set`
- `/ota`
- `/update`

If routes are refactored, preserve compatibility with wrappers or redirects.

### Keep ESP8266 Constraints in Mind

This is not a desktop app. Do not design like it has unlimited RAM, storage, or CPU.

Avoid:

- Huge dynamic allocations
- Excessive JSON size
- Constant filesystem writes
- Heavy JavaScript frameworks
- Streaming LED frames to the browser
- Giant lookup tables unless justified

Prefer:

- Small structs
- Compact JSON
- Reusable buffers
- Simple UI components
- Debounced saves
- Lightweight browser-side JavaScript

---

# Phase 1 Scope Summary

Implement the following systems:

1. Master brightness control
2. Persistent settings
3. Central render/output buffer pipeline
4. Gamma correction
5. Basic color calibration
6. Boot behavior setting
7. Diagnostics and power-safe page
8. Power supply configuration wizard / estimator
9. LED test and wiring check tools
10. Basic local automation API
11. Documentation update

Do not implement Phase 2+ systems yet:

- No full scene presets
- No favorite scene bar
- No timers
- No scheduler
- No palette builder
- No smooth transitions
- No large new mode batch
- No microphone support
- No light sensor support
- No physical controls

---

# Phase 1A — Master Brightness Control

## Goal

Add a true global master brightness control that applies to every mode consistently.

This should be separate from the internal brightness behavior of individual modes.

For example:

- A pulse mode may internally animate between dim and bright.
- A wave mode may internally calculate crests and valleys.
- A solid color may fill the strip with one color.

But after each mode renders its frame, the master brightness cap should scale the final output.

## Required Behavior

Add a `masterBrightness` runtime setting.

Recommended range:

- `0` to `255`

Recommended defaults:

- If the current project already has a `MASTER_BRIGHTNESS` constant, preserve that as the default.
- If the constant is unsafe or set to 255, choose a conservative default such as 180 unless the project clearly expects 255.

UI requirements:

- Add a master brightness slider to the main web UI.
- Show the current numeric value and percentage.
- Changes should apply immediately.
- Changes should be saved persistently using the Phase 1B persistence system.

API/state requirements:

- Include master brightness in `/state` JSON.
- Allow `/set` to update brightness.
- Add `/api/brightness?value=...` or equivalent in Phase 1J.

Implementation preference:

Do not manually duplicate master brightness logic inside every mode.

Prefer a centralized final scaling function:

```cpp
void applyMasterBrightness(ColorBuffer& buffer, uint8_t masterBrightness);
```

or equivalent for the project’s actual data structures.

## Acceptance Criteria

- Every existing mode visibly responds to the master brightness slider.
- Brightness changes do not reset the selected mode.
- Brightness changes do not cause blocking delays.
- Brightness survives restart after persistence is implemented.
- Brightness is reflected in `/state`.

---

# Phase 1B — Persistent Settings

## Goal

Persist the controller’s core state so it survives reboot/power loss.

## Required Persisted Settings

Persist at least:

- Active mode
- Active RGB color
- White temperature value
- Whether the last selected color came from RGB or white temperature, if the current logic tracks that
- Master brightness
- Existing mode-specific speed/options
- Boot behavior setting
- Gamma enabled/disabled
- RGB calibration gains, if implemented in this phase
- Power supply configuration values, if implemented in this phase

## Storage Recommendation

Prefer **LittleFS** with a compact JSON settings file.

Suggested file:

```text
/settings.json
```

If LittleFS is already not initialized in the project, add safe initialization during setup.

Required behavior:

- Load settings during boot.
- If settings are missing or corrupt, fall back to safe defaults.
- If settings are partially missing, use defaults for missing fields.
- Save settings after user changes.
- Debounce writes to protect flash.

## Flash-Wear Protection

Do not save settings every animation frame.

Recommended strategy:

- Mark settings dirty when changed.
- Save after 500–1500 ms of no additional changes.
- Do not save if values are unchanged.
- Optionally expose last save status in diagnostics.

## Error Handling

If settings fail to load:

- Use defaults.
- Show a warning in diagnostics.

If settings fail to save:

- Keep runtime values active.
- Show a warning in diagnostics.

## Acceptance Criteria

- Change color, mode, and brightness.
- Reboot board.
- Controller restores expected settings according to boot behavior.
- Invalid/missing settings file does not crash the controller.
- Settings writes are debounced.
- Diagnostics exposes basic settings load/save status.

---

# Phase 1C — Central Render / Output Buffer Pipeline

## Goal

Refactor rendering so each mode produces a frame into a central buffer before final output processing.

This creates the foundation for later:

- Smooth transitions
- Palettes
- Scene recalls
- Gamma correction
- Brightness scaling
- Power safety clamping
- Browser preview approximations

## Desired Conceptual Pipeline

The final structure should resemble this:

```cpp
handleWebAndOTA();
updateTimersAndState();
renderCurrentMode(rawFrameBuffer);
applyCalibration(rawFrameBuffer);
applyMasterBrightness(rawFrameBuffer, masterBrightness);
applyGammaCorrection(rawFrameBuffer);
applyPowerSafetyClampIfNeeded(rawFrameBuffer);
writeFrameToStrip(rawFrameBuffer);
strip.show();
```

The exact names can differ, but the architecture should be clearly centralized.

## Buffer Requirements

Use memory carefully.

For a 60 LED strip, one RGB buffer is cheap enough.
For later smooth transitions, two buffers may eventually be needed, but Phase 1 does not need to implement transitions.

Acceptable representations:

- Array of small RGB structs
- Array of `uint32_t` colors
- Existing NeoPixel color format if convenient

Make sure the chosen representation does not make calibration/gamma/brightness painfully awkward.

## Existing Mode Compatibility

Each existing mode should be adapted to write to the frame buffer, not directly call `strip.setPixelColor()` all over the place unless there is a wrapper that writes into the buffer.

Recommended helper functions:

```cpp
void setFramePixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void fillFrame(uint8_t r, uint8_t g, uint8_t b);
void clearFrame();
void showFrame();
```

or equivalent.

## Acceptance Criteria

- All existing modes still render correctly.
- Final brightness and gamma can be applied centrally.
- The web server remains responsive during animation.
- OTA handling remains present.
- The code structure clearly supports future transitions.

---

# Phase 1D — Gamma Correction

## Goal

Make brightness changes look more natural to human eyes.

WS2812 brightness is not perceptually linear. Gamma correction makes low brightness smoother and prevents dim modes from looking crunchy or abrupt.

## Required Behavior

Add gamma correction as a final output-stage operation.

Recommended first version:

- Gamma enabled by default.
- Use a lookup table.
- Default gamma approximately 2.2.

Possible implementation:

```cpp
uint8_t gamma8(uint8_t value);
```

or a precomputed `gammaTable[256]`.

## UI / Diagnostics

At minimum:

- Show whether gamma correction is enabled in diagnostics.

Better:

- Add toggle for gamma correction in calibration/settings.

Best but optional for Phase 1:

- Allow gamma amount selection, such as 1.8, 2.0, 2.2, 2.4.

Do not overcomplicate if memory becomes annoying.

## Acceptance Criteria

- Gamma is applied after master brightness.
- Gamma can be disabled or at least reported clearly.
- Existing modes still work.
- Low-brightness output looks smoother.

---

# Phase 1E — Basic Color Calibration

## Goal

Allow correction of RGB channel balance so the strip can look less cheap and more room-friendly.

## Required Behavior

Add calibration gains:

- Red gain
- Green gain
- Blue gain

Recommended range:

- 0–255, where 255 means 100% channel output.

Alternative range:

- 0–100 percentage, internally mapped to 0–255.

These gains should be applied centrally before master brightness and gamma, or in a clearly documented order.

Recommended order:

1. Mode renders raw intended color.
2. Calibration gains adjust channel balance.
3. Master brightness scales final brightness.
4. Gamma correction applies perceptual output curve.
5. Frame writes to strip.

## UI Requirements

Add a calibration section, either:

- On the diagnostics page, or
- On a simple settings/calibration page.

Controls:

- Red gain
- Green gain
- Blue gain
- Reset calibration button

Optional controls:

- Minimum visible brightness compensation
- Warm white correction
- Cool white correction

Keep it simple. The first version only needs RGB gains.

## Acceptance Criteria

- RGB gain controls affect output.
- Gains persist across restart.
- Reset calibration restores neutral output.
- Calibration does not break white-temperature mode.

---

# Phase 1F — Boot Behavior Setting

## Goal

Allow the controller to decide what it should do after reboot or power loss.

## Required Options

Implement at least:

1. Restore last state
2. Start off
3. Start warm dim

Optional fourth option:

4. Start diagnostics-safe mode

## Behavior Details

### Restore Last State

Load saved state and resume the last mode/color/brightness.

### Start Off

Turn LEDs off after boot, but keep web UI and OTA alive.

### Start Warm Dim

Start with a warm white low-brightness scene.

Suggested values:

- Temperature: 2200K–2700K
- Brightness: 20–60
- Mode: solid

### Diagnostics-Safe Mode

Optional.

Could start with:

- Low brightness
- Solid warm/neutral color
- Diagnostics warning visible

## UI Requirements

Add boot behavior selector to settings/diagnostics.

Persist the selected behavior.

## Acceptance Criteria

- Boot behavior setting persists.
- Each implemented option works after restart.
- Start off does not disable web/OTA.
- Start warm dim is genuinely dim and not bedroom flashbang mode.

---

# Phase 1G — Diagnostics and Power-Safe Page

## Goal

Add a useful diagnostics page for setup, troubleshooting, and power safety.

This page should help answer:

- What is the controller doing?
- What mode is active?
- What brightness is active?
- Is Wi-Fi okay?
- Is heap memory okay?
- What current might this LED strip draw?
- Are there any obvious configuration risks?

## Required Route

Add a diagnostics page route:

```text
/diagnostics
```

Optional JSON route:

```text
/api/diagnostics
```

## Required Diagnostics Data

Show at least:

- Firmware/project name
- Hostname
- IP address
- Wi-Fi SSID or connected/not connected status
- RSSI signal strength
- Uptime
- Free heap
- LED count
- LED pin
- Active mode
- Active RGB color
- White temperature value
- Master brightness
- Gamma enabled/disabled
- RGB calibration values
- Boot behavior
- Last settings load status
- Last settings save status
- OTA link/status

## Power Estimate Section

Show:

- Worst-case current estimate at full white/full brightness
- Estimated current at current master brightness
- Configured power supply current rating
- Recommended brightness cap based on supply rating and safety margin
- Warning if current settings may exceed configured safe current

Use the common WS2812 estimate:

```text
60 mA per LED at full RGB white
```

Formula:

```text
fullWhiteCurrentA = ledCount * 0.060
cappedCurrentA = fullWhiteCurrentA * (masterBrightness / 255.0)
```

If RGB calibration is implemented, the estimate can stay conservative and ignore calibration gains unless you want to document a more nuanced estimate.

## Warning Examples

Show warnings for:

- Master brightness may exceed configured power budget.
- LED count is high.
- Free heap is low.
- Wi-Fi signal is weak.
- Settings failed to load/save.
- LED pin is D3/GPIO0 and may affect boot reliability.

Do not overstate precision. Use “estimated,” not “guaranteed.”

## Acceptance Criteria

- `/diagnostics` loads in browser.
- It shows real live values.
- It clearly identifies active mode, brightness, and power estimate.
- It warns about obvious configuration risks.
- It does not require internet/cloud.

---

# Phase 1H — Power Supply Configuration Wizard / Estimator

## Goal

Let the user configure their power supply rating and safety margin so the controller can recommend a safe brightness cap.

This is software-only.

## Required Settings

Add settings for:

- Power supply voltage, default 5 V
- Power supply current rating, default maybe 3 A, 4 A, or 5 A
- Safety margin percentage, default 20%

Optional:

- D1 mini overhead estimate, default 0.2 A

## Required Calculations

Calculate:

```text
maxLedCurrent = ledCount * 0.060
safeAvailableCurrent = supplyCurrent * (1.0 - safetyMargin) - controllerOverhead
recommendedBrightness = safeAvailableCurrent / maxLedCurrent * 255
```

Clamp recommended brightness:

```text
0–255
```

Show:

- Full white worst case
- Current brightness estimated draw
- Recommended cap
- Current supply configuration

## UI Behavior

The diagnostics page can include a simple form:

- Supply current rating
- Safety margin
- Save button
- Apply recommended brightness cap button

The “apply recommended cap” button should set master brightness to the recommended cap only after user action.

Do not automatically slash brightness every time the estimate changes unless clearly designed and explained.

## Acceptance Criteria

- User can configure supply current and safety margin.
- Recommended brightness cap updates correctly.
- User can apply the recommended cap.
- Values persist across restart.

---

# Phase 1I — LED Test and Wiring Check Tools

## Goal

Add safe diagnostic LED tests for setup and troubleshooting.

## Required Test Modes / Actions

Add tools for:

1. All red
2. All green
3. All blue
4. All white at safe low brightness
5. Pixel walk from first LED to last LED
6. Alternating pixels
7. Clear/off

Optional:

- First LED only
- Last LED only
- Half strip test
- Direction indicator

## Safety Requirements

Diagnostic tests should use safe brightness by default.

Example:

- Temporarily use brightness 40–80 for full-strip white test unless user explicitly raises it.

Do not run full-white at full brightness automatically.

## UI Requirements

Add buttons on diagnostics page.

When a test is active, show:

- Current test name
- How to stop/return to previous mode

Recommended behavior:

- Store previous mode/state before running test.
- Provide “Return to previous state” button.

## Implementation Notes

Avoid blocking tests.

Pixel walk should be driven by `millis()` timing, not long blocking loops.

## Acceptance Criteria

- Each test works.
- Tests do not lock up web UI.
- Tests run at safe brightness.
- User can return to previous mode.

---

# Phase 1J — Basic Local Automation API

## Goal

Add simple JSON endpoints for future automation, phone shortcuts, scene URLs, and local integrations.

This phase only needs core controls.

## Required Endpoints

Add or normalize endpoints such as:

```text
/api/state
/api/on
/api/off
/api/brightness?value=120
/api/color?r=255&g=100&b=50
/api/temperature?k=2700
/api/mode?name=RainGlass
/api/diagnostics
/api/power
```

If the project already has `/state` and `/set`, keep them working.

## Response Format

Return compact JSON.

Successful response example:

```json
{
  "ok": true,
  "mode": "Solid",
  "masterBrightness": 120
}
```

Error response example:

```json
{
  "ok": false,
  "error": "Invalid brightness value"
}
```

## Validation Requirements

Validate inputs:

- Brightness must be 0–255.
- RGB values must be 0–255.
- Temperature should be clamped or rejected outside supported range, such as 2700K–6500K.
- Mode name must match a known mode.

Do not let weird API values crash or corrupt state.

## Acceptance Criteria

- API endpoints return JSON.
- Invalid requests return useful errors.
- Existing UI still works.
- Core state can be controlled through API.

---

# Phase 1K — Documentation Update

## Goal

Update project documentation to explain all Phase 1 changes.

## Required Documentation

Update or create documentation covering:

- Master brightness behavior
- Persistent settings
- Boot behavior options
- Diagnostics page
- Power estimate assumptions
- Power supply configuration
- Gamma correction
- RGB calibration
- LED test tools
- Local API endpoints
- Known limitations
- No-new-hardware scope

## Important Honesty Requirements

Do not claim the power estimate is exact.

Do not claim gamma correction makes colors scientifically accurate.

Do not claim the controller can detect real current draw unless actual current-sensing hardware exists.

Do not claim room-aware brightness or music reaction exists yet.

## Acceptance Criteria

- Docs match actual implemented behavior.
- Setup instructions remain clear.
- New settings are explained.
- Limitations are documented.

---

# Suggested Internal Implementation Order

Within Phase 1, implement in this order:

## Step 1 — Inventory and Stabilize

- Review current sketch structure.
- Identify all routes.
- Identify all mode rendering functions.
- Identify existing state variables.
- Confirm current LED count, LED pin, brightness constant, and OTA behavior.
- Do not rewrite everything blindly.

## Step 2 — Add Settings Model

- Create a central runtime settings struct/object.
- Include mode, color, temperature, brightness, calibration, boot behavior, and power config.
- Make existing UI and modes read/write through this central state where practical.

## Step 3 — Add Persistence

- Add LittleFS init.
- Add load/save settings.
- Add debounced save.
- Add settings status tracking.

## Step 4 — Add Master Brightness

- Add runtime value.
- Add UI slider.
- Add endpoint support.
- Apply centrally if output pipeline already exists, or temporarily in a central show helper before full pipeline refactor.

## Step 5 — Refactor Render Pipeline

- Add frame buffer helpers.
- Convert modes to render into buffer.
- Add central output processing.
- Keep behavior visually similar.

## Step 6 — Add Gamma and Calibration

- Add gamma function/table.
- Add RGB gains.
- Apply in final pipeline.
- Add UI/settings controls.

## Step 7 — Add Boot Behavior

- Implement restore/off/warm dim.
- Add UI setting.
- Test restart behavior.

## Step 8 — Add Diagnostics and Power Estimate

- Add `/diagnostics` page.
- Add diagnostics JSON.
- Add power calculations.
- Add warnings.

## Step 9 — Add LED Test Tools

- Add diagnostic test state.
- Add safe test actions.
- Add return-to-previous-state behavior.

## Step 10 — Add API Endpoints

- Add JSON API routes.
- Validate inputs.
- Preserve old endpoints.

## Step 11 — Documentation and Compile Check

- Update docs.
- Compile for D1 mini / ESP8266.
- Report what changed and any limitations.

---

# Parallel Work Guidance

Some Phase 1 work can be done in parallel, but only if clear boundaries are respected.

## Can Run in Parallel

### Lane A — Settings and Persistence

Work items:

- Central settings model
- LittleFS settings load/save
- Debounced writes
- Boot behavior storage
- Settings status tracking

This lane can run mostly in parallel with diagnostics UI design, but it should coordinate variable names and JSON field names.

### Lane B — UI Controls and Diagnostics Page

Work items:

- Master brightness slider UI
- Calibration controls UI
- Diagnostics page layout
- Power estimate display
- LED test buttons

This can run in parallel with core logic as long as the API/state field names are agreed first.

### Lane C — Local API Endpoints

Work items:

- `/api/state`
- `/api/brightness`
- `/api/color`
- `/api/temperature`
- `/api/mode`
- `/api/diagnostics`
- `/api/power`

This can run in parallel after the central settings model exists.

### Lane D — Documentation

Documentation can start early, but final docs must be corrected after implementation.

Useful early docs:

- Intended behavior
- Power estimate assumptions
- API draft
- Diagnostics field descriptions

### Lane E — LED Test Tools

Can be developed after a basic output helper exists.

Do not do this before the output path is understood.

## Should Not Run in Parallel Too Early

### Render Pipeline Refactor

This should be handled carefully and probably by one implementation lane at a time.

Reason:

It touches every animation mode and final output behavior. Parallel edits here are likely to cause conflicts or subtle regressions.

### Gamma / Calibration Application Order

This depends on the render pipeline.

Do not implement in three different places.

There should be one final output order:

1. Mode render
2. Calibration
3. Master brightness
4. Gamma
5. Power/safety clamp if present
6. Strip output

### Persistence Field Names

Do not let multiple parallel tasks invent different names for the same setting.

Create one central settings schema first.

---

# Testing Checklist

After Phase 1, test at least:

## Basic Control

- Main page loads.
- Color picker works.
- White temperature slider works.
- Mode dropdown works.
- Existing modes still animate.
- OTA page still exists.
- Browser update route still exists.

## Brightness

- Master brightness affects Solid Color.
- Master brightness affects Rainbow.
- Master brightness affects Chase/Wave.
- Master brightness affects calm modes.
- Brightness 0 or off behavior is safe and expected.
- Brightness persists after restart.

## Persistence

- Change color, reboot, confirm behavior.
- Change mode, reboot, confirm behavior.
- Change brightness, reboot, confirm behavior.
- Corrupt/missing settings file falls back safely.

## Output Pipeline

- No mode causes blocking UI behavior.
- No mode bypasses master brightness.
- Gamma can be enabled/disabled if toggle exists.
- Calibration gains affect output.

## Boot Behavior

- Restore last state works.
- Start off works.
- Start warm dim works.

## Diagnostics

- `/diagnostics` loads.
- IP/RSSI/uptime/free heap shown.
- Active mode shown.
- Power estimates shown.
- Warnings shown when applicable.

## LED Tests

- Red test works.
- Green test works.
- Blue test works.
- White low-brightness test works.
- Pixel walk works without blocking UI.
- Return to previous state works.

## API

- `/api/state` returns JSON.
- `/api/brightness?value=...` works.
- Invalid brightness returns JSON error.
- `/api/color` works.
- `/api/temperature` works.
- `/api/mode` works.
- Existing `/state` and `/set` still work.

---

# Final Codex Response Requirements

After implementation, report:

1. Files changed.
2. What was implemented.
3. What was intentionally not implemented.
4. Any limitations.
5. Any user settings that need editing before upload.
6. Whether the sketch compiled for D1 mini / ESP8266.
7. Any warnings about power supply, LED count, or D3/GPIO0.
8. Suggested next subphase.

Be honest. Do not hallucinate hardware tests. If the code compiles but was not tested on physical LEDs, say so clearly.

---

# One-Sentence Phase 1 Mission

Build the safe, persistent, diagnosable, centrally-rendered LED controller foundation so every later cozy lighting monster can plug into it without turning the sketch into spaghetti wearing a rave bracelet.

