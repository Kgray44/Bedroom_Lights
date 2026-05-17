# Bedroom LED Controller — Phase 1D Codex Prompt

## Phase 1D Title

**Local API Cleanup + Documentation Consolidation + Final Phase 1 Hardening**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 1D**, the final cleanup and hardening pass for Phase 1.

By now, Phase 1A, 1B, and 1C should have added or started:

### Phase 1A Foundation

- Central runtime settings model
- Persistent settings storage
- Debounced settings saves
- Master brightness control
- Master brightness UI
- `/state` and `/set` support for master brightness
- Basic boot behavior scaffold

### Phase 1B Output Pipeline

- Central LED frame buffer
- Frame helper functions
- Existing modes routed through the output pipeline
- Master brightness applied centrally
- Gamma correction foundation
- RGB channel calibration foundation

### Phase 1C Diagnostics and Safety

- `/diagnostics` browser page
- `/api/diagnostics` JSON endpoint
- `/api/power` JSON endpoint or equivalent power estimate data
- Power supply configuration settings
- Power estimate and recommended brightness cap
- Configuration warnings
- Safe LED test tools
- Non-blocking pixel walk
- Return-to-previous-state behavior after tests

The goal of Phase 1D is to make Phase 1 feel complete, coherent, testable, documented, and ready for Phase 2.

Do **not** add Phase 2 features yet.

This phase is not where we add scenes, timers, palettes, or new cozy wizard modes. This is where we put labels on the wires, tighten the screws, verify the routes, and stop the firmware raccoon from leaving crumbs in `/api/state`.

---

# Current Project Context

The project already includes:

- ESP8266 / D1 mini target
- WS2812 LED strip control using Adafruit NeoPixel
- Local web UI
- Existing animation modes
- Existing calm animation modes
- OTA support
- Browser firmware update route
- Phase 1A settings/persistence/master brightness foundation
- Phase 1B render/output pipeline/gamma/calibration foundation
- Phase 1C diagnostics/power/test tools foundation

Existing routes that must remain functional:

- `/`
- `/state`
- `/set`
- `/ota`
- `/update`
- `/diagnostics`

Phase 1D may add, normalize, or document API routes, but it must preserve backward compatibility with existing UI behavior.

---

# Hard Scope Boundary

This prompt covers:

1. Local API route cleanup and normalization
2. JSON response consistency
3. Input validation and safe clamping
4. Backward compatibility for `/state` and `/set`
5. Consolidated settings schema review
6. Persistence sanity review
7. Route and UI link consistency
8. OTA and browser update preservation
9. Phase 1 documentation consolidation
10. Compile/test/hardening pass
11. Final Phase 1 report

Do **not** implement yet:

- Scene presets
- Favorite scene bar
- Panic Warm button, unless it already exists from accidental work and only needs documenting
- Night Guard
- Timers
- Scheduler
- Smooth transitions
- Palette builder
- Browser live animation preview
- New aesthetic lighting modes
- QR/NFC scene links
- Physical buttons
- Microphone support
- Light sensor support
- Any new hardware assumptions

If you find partially implemented later-phase work, do not expand it. Stabilize or document it as deferred.

The feature goblin may whisper, “just one tiny scene preset.” Ignore it. That goblin lies.

---

# Phase 1D Main Goal

Make the Phase 1 foundation coherent and ready for Phase 2.

At the end of Phase 1D:

- API routes should be predictable.
- JSON responses should be consistent.
- Inputs should be validated safely.
- Existing UI should still work.
- Settings should load/save reliably.
- Diagnostics should reflect real state.
- Output pipeline should remain centralized.
- OTA should still work.
- Documentation should match the code.
- The sketch should compile for D1 mini / ESP8266.

---

# 1. Inventory Existing Routes and Behaviors

## Goal

Before changing code, inventory the current route map.

## Required Inventory

List all server routes currently registered in the sketch.

At minimum, check:

- `/`
- `/state`
- `/set`
- `/ota`
- `/update`
- `/diagnostics`
- `/api/diagnostics`
- `/api/power`
- `/api/test`
- Any other `/api/...` endpoints added in prior phases

## Required Behavior

Do not blindly rewrite routes.

For each route, identify:

- Method / expected request style, if relevant
- Query parameters accepted
- Response type: HTML, JSON, redirect, plain text
- Whether the main UI depends on it
- Whether it should remain public on local network

## Acceptance Criteria

- Route inventory is included in the Phase 1D report.
- No existing user-facing route is removed without a compatibility wrapper.
- Main UI still works after cleanup.

---

# 2. Normalize Local API Routes

## Goal

Create a coherent local API surface that future phases can build on.

## Required API Routes

Ensure these routes exist or are intentionally documented as equivalent:

```text
/api/state
/api/brightness
/api/color
/api/temperature
/api/mode
/api/diagnostics
/api/power
/api/test
```

If the existing project already uses `/state` and `/set`, preserve them.

Recommended approach:

- Keep `/state` as a backward-compatible alias or wrapper for `/api/state`.
- Keep `/set` as a backward-compatible compatibility endpoint.
- Add cleaner `/api/...` endpoints for future automation.

## Required Route Purposes

### `/api/state`

Returns current controller state.

Should include:

- Mode
- Active RGB
- Active hex color
- White temperature
- Using white temperature flag, if tracked
- Master brightness
- Brightness percentage
- Gamma enabled
- RGB gains
- Boot behavior
- Current diagnostic test state, if active

### `/api/brightness`

Sets or returns master brightness.

Recommended query:

```text
/api/brightness?value=180
```

If no `value` is given, it may return current brightness.

### `/api/color`

Sets active RGB color.

Recommended query:

```text
/api/color?r=255&g=120&b=40
```

Optional support:

```text
/api/color?hex=FF7828
```

### `/api/temperature`

Sets white temperature.

Recommended query:

```text
/api/temperature?k=2700
```

### `/api/mode`

Sets or returns active mode.

Recommended query:

```text
/api/mode?name=RainGlass
```

Support the actual mode names used in the code.

### `/api/diagnostics`

Returns diagnostics JSON.

### `/api/power`

Returns power estimate JSON and optionally updates power configuration if the route already supports that.

### `/api/test`

Triggers diagnostic LED tests.

Recommended query:

```text
/api/test?name=pixelWalk
```

## Acceptance Criteria

- Cleaner `/api/...` routes exist for core actions.
- Legacy `/state` and `/set` still work.
- Future Phase 2 code has a stable API to call.

---

# 3. Consistent JSON Response Format

## Goal

Make API responses predictable.

## Required Success Format

Use a compact success response pattern:

```json
{
  "ok": true,
  "message": "Brightness updated",
  "state": {
    "masterBrightness": 180
  }
}
```

For very small endpoints, it is acceptable to return:

```json
{
  "ok": true,
  "masterBrightness": 180
}
```

The exact shape may vary, but every JSON API response should include:

```json
"ok": true
```

## Required Error Format

Use a compact error response pattern:

```json
{
  "ok": false,
  "error": "Invalid brightness value"
}
```

Every API error should include:

```json
"ok": false
```

and a short useful error message.

## Content Type

For JSON routes, set the response content type:

```text
application/json
```

## Acceptance Criteria

- API success responses are consistent enough for future frontend code.
- API errors are useful and not random HTML/plain text.
- JSON routes use `application/json`.
- Existing UI is not broken by response changes.

---

# 4. Input Validation and Clamping

## Goal

Make all local API inputs safe and boring.

Boring is good here. Exciting inputs are how tiny web servers end up crying in a corner.

## Required Validation

### Brightness

Valid range:

```text
0–255
```

Behavior:

- Clamp or reject invalid values consistently.
- Recommended: reject non-numeric values; clamp numeric out-of-range values.

### RGB Color

Valid range for each channel:

```text
0–255
```

Behavior:

- Reject missing required channels unless `hex` is provided.
- Clamp numeric out-of-range values or reject consistently.
- Reject malformed hex strings.

### White Temperature

Valid range:

```text
2700K–6500K
```

Behavior:

- Clamp or reject outside range consistently.
- Recommended: clamp numeric values into supported range.
- Reject non-numeric values.

### Mode Name

Behavior:

- Must match known mode identifier or display name.
- Reject unknown names with JSON error.
- Avoid case-sensitivity pain if easy, but do not create ambiguous matching.

### Gamma Enabled

Accept:

- `0` / `1`
- optionally `true` / `false`

### RGB Gains

Valid range:

```text
0–255
```

### Power Config

Validate:

- Supply voltage must be positive and sane.
- Supply current must be positive and sane.
- Safety margin should be 0–90% or another safe range.
- Controller overhead should be non-negative and sane.

### Diagnostic Test Name

Behavior:

- Must match known test identifier.
- Unknown test names return JSON error.

## Required Helper Functions

If not already present, add small helpers such as:

```cpp
bool parseIntArg(const String& name, int& out);
int clampInt(int value, int minValue, int maxValue);
bool parseBoolArg(const String& value, bool& out);
bool parseHexColor(const String& hex, uint8_t& r, uint8_t& g, uint8_t& b);
```

Use naming that fits the sketch.

## Acceptance Criteria

- Malformed requests do not crash.
- Invalid requests return JSON errors.
- Numeric values are safe.
- No route accepts nonsense into persistent settings.

---

# 5. Backward Compatibility for `/state` and `/set`

## Goal

Preserve the existing frontend and any previous local shortcuts.

## `/state`

If `/api/state` is added, keep `/state` working.

Options:

- `/state` calls the same handler as `/api/state`.
- `/state` returns the original shape plus new fields.

Do not remove fields the web UI currently expects.

## `/set`

Keep `/set` working for the existing web UI.

If cleaner `/api/...` endpoints are added, `/set` may internally delegate to the same update functions.

Existing accepted parameters should continue to work.

Examples might include:

- mode
- color
- temperature
- delay values
- speed values
- pulse count
- master brightness
- gamma/calibration fields, if already added
- power settings, if Phase 1C used `/set`

## Acceptance Criteria

- The original main page still works.
- Existing JS does not fail because a route disappeared.
- New API endpoints do not require rewriting all old UI code unless intentionally documented and tested.

---

# 6. Consolidate State Mutation Functions

## Goal

Avoid having five routes each mutate state in their own weird little cave.

## Required Refactor

Create or normalize central update functions.

Suggested functions:

```cpp
bool setMasterBrightness(uint8_t value);
bool setActiveColor(uint8_t r, uint8_t g, uint8_t b);
bool setWhiteTemperature(uint16_t kelvin);
bool setModeByName(const String& modeName);
bool setGammaEnabled(bool enabled);
bool setRgbGains(uint8_t rGain, uint8_t gGain, uint8_t bGain);
bool setPowerConfig(...);
bool startDiagnosticTest(const String& testName);
```

Each function should:

- Validate or receive already validated input.
- Update central settings/runtime state.
- Mark settings dirty when persistent values change.
- Avoid duplicate logic.

## Acceptance Criteria

- `/set` and `/api/...` reuse the same mutation functions where practical.
- Future Phase 2 scene recall can call these same functions.
- Settings dirty marking is consistent.

---

# 7. Settings Schema Review and Versioning

## Goal

Review the persistent settings schema after Phase 1A–1C and make it coherent.

## Required Review

Check `/settings.json` fields or equivalent persistence storage.

Expected categories:

### Core Lighting

- Mode
- RGB color
- White temperature
- Using white temperature flag
- Master brightness
- Mode-specific settings

### Behavior

- Boot behavior

### Output Pipeline

- Gamma enabled
- RGB gains

### Power Config

- Supply voltage/current or millivolts/milliamps
- Safety margin
- Controller overhead estimate

## Version Field

Ensure settings include a version field:

```json
"version": 1
```

or increment to a new version if needed.

Do not break loading old Phase 1A/1B/1C settings.

## Missing Field Handling

If a field is missing:

- Use safe default.
- Do not treat the entire settings file as corrupt.
- Save normalized schema on next settings write.

## Corrupt JSON Handling

If settings JSON is corrupt:

- Use defaults.
- Track load error status.
- Show warning in diagnostics.
- Do not crash.

## Acceptance Criteria

- Schema is documented in Phase 1D report.
- Missing fields are handled safely.
- Corrupt settings are handled safely.
- Settings version is present or intentionally documented as deferred.

---

# 8. Persistence Sanity and Flash-Wear Review

## Goal

Make sure settings persistence is reliable and not abusing flash.

## Required Checks

Review:

- Settings dirty flag behavior
- Debounce time
- Whether animations accidentally mark settings dirty repeatedly
- Whether diagnostics refresh accidentally saves settings
- Whether API reads accidentally save settings
- Whether LED tests accidentally overwrite the user's preferred mode permanently

## Required Behavior

Settings should be saved when the user changes persistent configuration.

Settings should **not** be saved when:

- An animation frame updates
- Diagnostics data refreshes
- Pixel walk test advances one pixel
- Uptime changes
- Free heap changes
- A temporary diagnostic test renders

## Acceptance Criteria

- Saves are debounced.
- Repeated slider changes do not spam flash constantly.
- Non-persistent runtime changes do not trigger saves.
- Diagnostic tests do not permanently overwrite preferred settings unless explicitly designed.

---

# 9. Diagnostics Consistency Review

## Goal

Ensure diagnostics reflect the real runtime state after API cleanup.

## Required Checks

Verify `/diagnostics`, `/api/diagnostics`, and `/api/power` still show accurate values after route cleanup.

Check:

- Mode
- Color
- Temperature
- Master brightness
- Gamma status
- RGB gains
- Boot behavior
- Settings load/save status
- Power settings
- Recommended brightness cap
- Active diagnostic test name
- Warnings

## Acceptance Criteria

- Diagnostics page and JSON agree.
- Power estimate values are consistent across UI and JSON.
- Warnings still work.
- No stale variable names remain from earlier phases.

---

# 10. LED Test Tool Hardening

## Goal

Make diagnostic LED tests safe and predictable.

## Required Checks

Verify:

- Test names are normalized.
- Unknown test names return JSON error.
- Pixel walk is non-blocking.
- Safe white test uses safe brightness.
- Return-to-previous-state works.
- Test state is visible in diagnostics.
- Tests do not persistently overwrite user settings unless explicitly intended.
- Tests still pass through the central output pipeline.

## Optional Improvement

Add a timeout for diagnostic tests if useful.

Example:

- Static tests can stay active until user restores.
- Pixel walk can continue until stopped.
- Optional auto-timeout after 5 minutes for safety.

Do not overbuild.

## Acceptance Criteria

- Test tools are stable.
- API errors are useful.
- Existing lighting resumes correctly after restore.

---

# 11. OTA and Browser Update Preservation

## Goal

Make sure hardening did not break the ability to update firmware.

## Required Checks

Verify code still includes and services:

- ArduinoOTA setup
- ArduinoOTA handle call in loop
- Browser update server setup
- `/ota` info page
- `/update` upload route

## Required Behavior

The main loop must keep calling OTA/web server handling while animations/tests run.

Conceptual loop:

```cpp
void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  updateAnimationFrame();
  serviceSettingsSave();
}
```

Use actual existing server names.

## Acceptance Criteria

- OTA code remains present.
- Browser updater route remains registered.
- Diagnostics/test tools do not block OTA handling.

---

# 12. UI Navigation and Link Consistency

## Goal

Make the web UI feel coherent after adding diagnostics/API tools.

## Required Links

Ensure these navigation paths exist:

- Main page → Diagnostics
- Main page → OTA page
- Diagnostics → Main page
- Diagnostics → OTA page
- OTA page → Main page, if practical

## UI Requirements

Keep UI lightweight.

Do not add a huge navigation framework.

Simple buttons or links are enough.

## Acceptance Criteria

- User can move between main controls, diagnostics, and OTA pages.
- Links are clear.
- Mobile usability is not worsened.

---

# 13. Documentation Consolidation

## Goal

Update project documentation so it matches the actual Phase 1 state.

## Required Docs

Update or create a final Phase 1 summary document.

Suggested file:

```text
BedroomLedController_Phase_1_Final_Report.md
```

Also update existing docs if they now contain outdated information.

Potential docs to update:

- Project overview
- Modes and ideas document
- Expanded ideas document
- Phase 1A report
- Phase 1B report
- Phase 1C report
- New final Phase 1 report

Do not duplicate huge amounts unnecessarily. The final report can summarize and link/mention subphase reports if they exist.

## Required Final Report Sections

Include:

1. Phase 1 purpose
2. Files changed
3. Current route map
4. Current settings schema
5. Master brightness behavior
6. Persistence behavior
7. Output pipeline order
8. Gamma/calibration behavior
9. Diagnostics page summary
10. Power estimator assumptions
11. LED test tools summary
12. Local API summary
13. OTA/browser update status
14. Known limitations
15. Hardware safety notes
16. What was intentionally deferred
17. Compile/test status
18. Recommended Phase 2 starting point

## Required Honesty

Do not claim physical LED testing unless actual physical LED testing was performed.

Do not claim actual current measurement.

Do not claim cloud/home automation support if only local API endpoints exist.

Do not claim new hardware support.

Use clear language such as:

```text
The power estimator uses the common WS2812 worst-case assumption of about 60 mA per LED at full RGB white. It does not measure real current.
```

and:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

if applicable.

## Acceptance Criteria

- Docs match the code.
- Outdated defaults are corrected.
- Known limitations are honest.
- Phase 2 can start from the docs without archaeological digging.

---

# 14. Code Cleanup and Naming Review

## Goal

Clean obvious naming, duplication, and confusing leftovers from Phase 1A–1C.

## Required Cleanup

Review for:

- Duplicate brightness variables
- Duplicate color state variables
- Old direct `strip.show()` calls in normal modes
- Old `strip.setBrightness()` use that double-applies brightness
- Unused helper functions
- Unused route handlers
- Inconsistent mode names
- Inconsistent JSON field names
- Excessive `String` churn in frequently called animation code
- Settings fields not included in save/load
- Save/load fields not included in defaults

## Important Rule

Do not perform a massive unrelated style rewrite.

This is cleanup, not “rewrite the entire app because Codex had coffee.”

## Acceptance Criteria

- Obvious duplicates are removed or documented.
- Naming is more consistent.
- No functional regressions.

---

# 15. Compile and Regression Test

## Goal

End Phase 1 with a compileable, coherent sketch.

## Required Compile

Compile for the D1 mini / ESP8266 board target.

If the environment cannot compile, state that honestly in the report and list what was checked instead.

## Regression Checklist

Verify or inspect:

### Core UI

- `/` loads.
- Color picker works.
- White temperature slider works.
- Mode dropdown works.
- Master brightness slider works.

### Existing Modes

At minimum verify by compile/logic review or physical test if available:

- Solid Color
- Strobe
- Flash
- Chase
- Wave
- Slow Wave
- Slow Pulse
- Rainbow
- Existing calm modes

### Persistence

- Settings load defaults if no settings file exists.
- Settings save after changes.
- Settings dirty debounce works.
- Missing fields use defaults.

### Output Pipeline

- Modes render through central buffer.
- Brightness applies centrally.
- Gamma applies centrally if enabled.
- RGB gains apply centrally.
- `strip.show()` is centralized for normal output.

### Diagnostics

- `/diagnostics` loads.
- `/api/diagnostics` returns valid JSON.
- `/api/power` returns valid JSON.
- Warnings appear when applicable.

### LED Tests

- Test endpoint validates names.
- Pixel walk is non-blocking.
- Restore previous state exists.

### API

- `/api/state`
- `/api/brightness`
- `/api/color`
- `/api/temperature`
- `/api/mode`
- `/api/diagnostics`
- `/api/power`
- `/api/test`

### OTA

- ArduinoOTA setup remains.
- `ArduinoOTA.handle()` remains in loop.
- Browser update route remains.
- `/ota` remains.
- `/update` remains.

## Acceptance Criteria

- Compile passes, or limitations are clearly reported.
- No known Phase 1 route is broken.
- Phase 1 final report is created.

---

# 16. Phase 1D Non-Goals Reminder

Do not implement these yet:

- Scene presets
- Favorite scene bar
- Panic Warm
- Night Guard
- Timers
- Bedtime scheduler
- Weekly scheduler
- Smooth transitions
- Palette builder
- Browser animation preview
- New calm modes
- QR/NFC scene links
- Physical controls
- Microphone/music-reactive behavior
- Light-sensor room-aware brightness

Those are Phase 2+.

If you accidentally find them partially implemented, stabilize/document only. Do not expand them.

The cozy goblin army is coming later. Today we inventory the armory.

---

# Suggested Implementation Order

Follow this order to reduce risk:

## Step 1 — Route Inventory

List existing registered routes and what they do.

## Step 2 — Add or Normalize `/api/state`

Make `/api/state` the clean future-facing state endpoint.

Keep `/state` working.

## Step 3 — Add Core Control API Endpoints

Add/normalize:

- `/api/brightness`
- `/api/color`
- `/api/temperature`
- `/api/mode`

Use central state mutation functions.

## Step 4 — Normalize JSON Response Helpers

Add small helpers for success/error JSON responses.

## Step 5 — Add Input Parsing/Validation Helpers

Add reusable parse/clamp helpers.

## Step 6 — Wire Legacy `/set` Through Shared Mutation Functions

Preserve existing behavior while avoiding duplicate logic.

## Step 7 — Review Settings Schema and Persistence

Ensure all Phase 1 fields default/load/save correctly.

## Step 8 — Verify Diagnostics and Power Endpoints

Make sure `/api/diagnostics` and `/api/power` still agree with runtime state.

## Step 9 — Harden LED Test Endpoint

Validate names, restore behavior, and non-blocking operation.

## Step 10 — Review OTA and UI Links

Ensure navigation and update routes remain intact.

## Step 11 — Cleanup Obvious Duplication

Remove dead/confusing code only when safe.

## Step 12 — Compile and Create Final Phase 1 Report

Compile for D1 mini / ESP8266 and document results.

---

# Test Checklist

## Compile

- Sketch compiles for D1 mini / ESP8266.

## Route Availability

- `/` works.
- `/state` works.
- `/set` works.
- `/ota` works.
- `/update` works.
- `/diagnostics` works.
- `/api/state` works.
- `/api/brightness` works.
- `/api/color` works.
- `/api/temperature` works.
- `/api/mode` works.
- `/api/diagnostics` works.
- `/api/power` works.
- `/api/test` works.

## JSON Behavior

- API success responses include `ok: true`.
- API error responses include `ok: false`.
- JSON routes use `application/json`.
- Invalid inputs return errors without crashing.

## Backward Compatibility

- Existing frontend still uses `/state` and/or `/set` successfully.
- Existing mode controls still work.
- Existing OTA links still work.

## Settings

- Settings schema has version or documented schema versioning.
- Missing fields default safely.
- Corrupt settings default safely.
- Saves are debounced.
- Diagnostics refresh does not save settings.
- Animations do not save settings.

## Output

- Brightness still applies to all modes.
- Gamma/calibration still apply through the central pipeline.
- No double brightness scaling.
- Normal modes do not bypass output pipeline.

## Diagnostics

- Diagnostics reflects current state.
- Power estimates are consistent.
- Warnings still appear.
- LED test status appears.

## LED Tests

- Unknown test name returns JSON error.
- Pixel walk is non-blocking.
- Restore previous state works.
- Tests do not permanently overwrite settings unintentionally.

## Documentation

- Final Phase 1 report exists.
- Route map documented.
- API documented.
- Settings schema documented.
- Known limitations documented.
- Deferred Phase 2+ work documented.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 1D complete.

Files changed:
- ...

Implemented:
- ...

API cleanup:
- ...

Backward compatibility:
- ...

Settings schema:
- ...

Diagnostics/power/test hardening:
- ...

Documentation:
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
- Phase 2A — scene preset storage and recall.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim actual current measurement unless current-sensing hardware exists.

Do not claim Phase 2 features are implemented unless they actually are.

---

# One-Sentence Mission Summary

Clean up and harden the Phase 1 foundation so the Bedroom LED Controller has a stable local API, truthful diagnostics, sane persistence, preserved OTA, accurate documentation, and a compileable codebase ready for Phase 2 scenes and cozy chaos.

