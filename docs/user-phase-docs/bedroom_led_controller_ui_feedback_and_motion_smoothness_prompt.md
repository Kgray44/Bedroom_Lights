# Bedroom LED Controller — UI Feedback + API Reliability + Motion Smoothness Prompt

You are working on the `Bedroom_Lights` repository.

Repository:

```text
https://github.com/Kgray44/Bedroom_Lights
```

Current target:

- D1 mini / ESP8266
- 140 LEDs configured
- Local web UI served from the ESP8266
- Firmware compiles and uploads over USB
- Runtime heap is tight
- `/api/scenes` is currently the tightest measured route, with route-local heap floor around 2,000 bytes
- `/api/palettes`, `/api/diagnostics`, and startup API loading may also stress heap
- The web UI looks good visually, but user feedback and reliability need work
- The physical animations feel slightly “skippy,” not because frames are obviously dropping, but because brightness/color changes appear to step too much between frames

This is a focused polish and reliability pass.

This prompt has two parts:

1. **Live UI Feedback + ESP8266 API Reliability Fix**
2. **Motion Smoothness / Perceptual Animation Refinement Pass**

Implement them in that order.

Do NOT add new lighting modes.
Do NOT add new feature families.
Do NOT downgrade animation or mode math.
Do NOT rewrite the whole UI.
Do NOT migrate hardware.
Do NOT increase default brightness.
Do NOT remove existing features unless absolutely necessary.
Do NOT simplify renderer math into crude approximations.
Do NOT perform OTA upload unless explicitly instructed.

The goal is to make the existing Bedroom LED Controller feel like a polished local-first embedded lighting appliance:

- Responsive
- Honest
- Clear when actions succeed/fail
- Gentle on ESP8266 heap
- Visually smoother
- Still beautiful
- Still local-first
- Still D1-mini-compatible

The firmware goblin may request more features. Deny it snacks. This pass is about **feedback and smoothness**, not fireworks.

---

# Part 1 — Live UI Feedback + ESP8266 API Reliability Fix

## Observed Real-World Issues

When testing the web UI on the real D1 mini, the user observed:

1. Changing settings gives very little indication anything happened.
2. Favorite section says:

```text
Favorite list unavailable
```

3. Preview says:

```text
Preview State Unavailable
```

4. Surprise Me gets stuck on:

```text
Choosing a safe surprise...
```

5. Palette section says:

```text
Palette List Unavailable
```

6. Night Guard still says:

```text
Night Guard is Off
```

even after toggling it on.

7. Scene section says:

```text
Scene List Unavailable
```

Important distinction:

- If favorites/scenes/palettes are empty, the UI should say they are empty.
- “Unavailable” should only mean endpoint failure, invalid JSON, timeout, or device/resource issue.

---

## Likely Root Cause

The main page currently fires many async API requests on startup, including:

- `/state`
- `/api/modes`
- `/api/palettes`
- `/api/scenes`
- `/api/favorites`
- `/api/timer`
- `/api/state` again for preview
- repeated preview polling

On an ESP8266 with tight heap, this can cause transient route failures, partial/invalid JSON, timeouts, stale UI state, or unavailable sections.

Fix the UI so it does not behave like a browser-powered JSON cannon aimed at a tiny Wi-Fi potato.

---

## Part 1 Hard Requirements

### 1. Add Clear Global User Feedback

Add a lightweight global status/toast/banner area near the top of the UI.

It should show messages such as:

- `Connecting...`
- `Connected`
- `Saving brightness...`
- `Brightness updated`
- `Color updated`
- `Mode changed`
- `Night Guard enabled`
- `Night Guard disabled`
- `Scene loaded`
- `Scene list failed — retrying`
- `Palette list unavailable — retry`
- `Surprise failed — controller did not respond`
- `Controller busy — retrying`

Requirements:

- Must be visible.
- Must not require a page reload.
- Success messages should auto-clear after a few seconds.
- Error messages should remain until the next successful action or retry.
- Do not use heavy libraries.
- Keep style consistent with the current dark UI.

Suggested UI location:

- Under the header
- Or near the Current Light summary
- Or as a compact pill/banner near the existing status pill

Do not make it a giant alarm unless the D1 mini is actually on fire. It is probably just heap goblins.

---

### 2. Add Per-Control Pending / Success / Error Feedback

For user actions, show feedback immediately.

Cover at least:

- Master brightness slider
- Color picker
- White temperature slider
- Mode dropdown
- Gamma/calibration controls
- Palette enable/select/save/delete
- Scene save/load/rename/delete
- Favorite load/reset
- Warm Dim Now
- Surprise Me
- Night Guard toggles/sliders
- Timer start/cancel
- Bedtime fade start/cancel

Behavior:

- Disable action buttons while a request is in flight where practical.
- Show temporary pending text or global status.
- On success, show success message.
- On failure, show clear error message and refresh state if possible.

For sliders:

- Do not spam huge toast messages on every tiny movement.
- Use subtle status like `Saving...` then `Saved`.
- Keep existing immediate local readout updates.

---

### 3. Implement a Robust API Fetch Helper

Create a central JavaScript helper for API calls.

Suggested function:

```js
async function apiFetchJson(url, options = {}) {
  // options:
  // - label
  // - timeoutMs
  // - retries
  // - retryDelayMs
  // - showStatus
  // - expectOk
}
```

Required behavior:

- Use `fetch(url, { cache: 'no-store' })`.
- Add timeout using `AbortController`.
- Catch network errors.
- Catch JSON parse errors.
- Check `payload.ok` where expected.
- Return a structured result:
  - `{ ok: true, payload, statusCode }`
  - `{ ok: false, error, statusCode, rawText }`
- Do not throw uncaught errors into click handlers.
- Log technical details to console for debugging.
- Show friendly UI messages to the user.

When JSON parsing fails, console log:

- URL
- HTTP status
- First 200 characters of response text
- Timestamp
- Error message

Do not print secrets.

---

### 4. Serialize Initial Page API Loading

Do not fire all startup API requests at once.

Create a staged startup flow.

Suggested structure:

```js
async function bootUi() {
  showGlobalStatus('Connecting...', 'pending');

  await refreshStateSafe();

  await delay(100);
  await refreshModesSafe();

  await delay(100);
  await refreshFavoritesSafe();

  await delay(100);
  await refreshScenesSafe();

  await delay(100);
  await refreshPalettesSafe();

  await delay(100);
  await refreshTimerSafe();

  refreshPreviewFromCachedState();

  showGlobalStatus('Connected', 'success');
}
```

Equivalent structure is fine.

Rules:

- State loads first.
- Mode metadata loads second.
- Favorites/scenes/palettes load sequentially, not all at once.
- If one list fails, show that section as unavailable but continue loading the rest.
- Add retry buttons for failed sections.
- Do not block the entire UI because scenes failed.

This protects the ESP8266 from startup request pileups.

---

### 5. Add a Lightweight Request Queue / Busy Guard

Add a simple browser-side request queue for heavy endpoints.

Heavy endpoints:

- `/api/scenes`
- `/api/palettes`
- `/api/modes`
- `/api/backup/export`
- `/api/diagnostics`
- `/api/resources`

Rules:

- Only one heavy request should run at a time.
- Small control requests can still happen, but avoid overlapping them with list refresh storms.
- After major state-changing actions, refresh only what is needed.

Examples:

- Brightness change: apply returned state only.
- Color change: apply returned state only.
- Mode change: apply returned state only.
- Night Guard change: apply returned state or fetch `/api/state` once.
- Scene save/delete/rename: refresh scenes and state.
- Palette save/delete/select: refresh palettes and state.
- Favorite reset: refresh favorites only.
- Surprise Me: refresh/apply state, not every list.

Do not refresh scenes, palettes, favorites, timer, and state after every generic action unless required.

---

### 6. Reduce Preview Polling Pressure

The preview currently can request `/api/state` independently.

Change preview behavior:

- Prefer using `lastState` from the main state refresh.
- Continue drawing the browser-side preview from cached state.
- Only call `/api/state` for preview if no valid state exists or after a successful state-changing action.
- Increase preview polling interval to 10–15 seconds, or remove separate preview polling entirely.
- Do not show `Preview State Unavailable` if a valid `lastState` exists.
- If `/api/state` fails, show a retryable message instead of a dead status.

The preview is approximate; it does not need to hammer the ESP8266 like it owes the browser money.

---

### 7. Make Empty Lists Different From Failed Lists

Favorites:

If `/api/favorites` succeeds but count is 0 or favorites are empty, show:

```text
No favorites configured. Use Reset Favorites to restore defaults.
```

If endpoint fails, show:

```text
Favorite list unavailable.
[Retry]
```

Scenes:

If `/api/scenes` succeeds but count is 0, show:

```text
No scenes saved yet. Save the current light as a scene.
```

If endpoint fails, show:

```text
Scene list unavailable.
[Retry]
```

Palettes:

If `/api/palettes` succeeds but count is 0, show:

```text
No palettes available. Reset built-ins or save a palette.
```

If endpoint fails, show:

```text
Palette list unavailable.
[Retry]
```

Do not confuse “empty” with “failed.”

---

### 8. Add Retry Buttons for Failed Sections

Each unavailable section should include a retry button.

Required retry behavior:

- Favorite retry calls `refreshFavoritesSafe()`
- Scene retry calls `refreshScenesSafe()`
- Palette retry calls `refreshPalettesSafe()`
- Preview retry calls `refreshStateSafe()` or refreshes cached state
- Optional mode retry calls `refreshModesSafe()`

Retry should not reload the whole page.

---

### 9. Fix Surprise Me Stuck State

Update `runSurprise()` so it has full `try/catch/finally` behavior.

Required:

- Set button disabled while running.
- Change button text or status to `Choosing...`
- Set message: `Choosing a safe surprise...`
- If request succeeds:
  - Show selected mood/mode/palette/brightness.
  - Apply returned state or refresh state.
- If request returns `{ok:false}`:
  - Show returned error.
- If request times out/network fails/JSON parse fails:
  - Show:

```text
Surprise failed — controller did not return valid JSON.
```

- Always re-enable the button.
- Always clear or replace the “Choosing...” message.
- Never leave the UI stuck on “Choosing a safe surprise...”

---

### 10. Fix Night Guard Stale Display

Update Night Guard UI handling.

Required:

- When user toggles Night Guard, immediately show:

```text
Updating Night Guard...
```

- Disable Night Guard controls while request is in flight if practical.
- On success:
  - Apply returned state if present.
  - If response does not include state, call `/api/state` once and apply state.
  - Update note to one of:
    - `Night Guard is enabled. Max X / 255.`
    - `Night Guard is limiting output to X / 255.`
    - `Night Guard is off.`
- On failure:
  - Revert checkbox/slider to last known state.
  - Show:

```text
Night Guard update failed.
```

- Debounce `nightGuardMaxBrightness` slider so it does not spam `/api/nightguard/set` on every tiny movement.
- The displayed note must not remain “Night Guard is off” after a successful enable.

Current user-observed issue:

- The user turns Night Guard on, but UI still says “Night Guard is Off.”

This must be fixed.

---

### 11. Improve Generic State-Changing Feedback

Current `send()` / `sendSoon()` applies state but gives little indication anything happened.

Update behavior:

- For non-slider actions, show pending and success/error feedback.
- For sliders, show local readout immediately and a subtle saving/saved indicator.
- On success, apply returned state.
- On failure, show error and refresh state.

Examples:

- Brightness slider:
  - immediate numeric update
  - status: `Saving brightness...`
  - then `Brightness saved`
- Mode dropdown:
  - status: `Changing mode...`
  - then `Mode changed`
- Color picker:
  - swatch updates immediately
  - status: `Saving color...`
  - then `Color saved`

Avoid excessive toast spam during continuous slider movement.

---

### 12. Improve Favorite / Scene / Palette Action Feedback

Favorites:

- On click, show:

```text
Loading favorite...
```

- On success:

```text
Favorite loaded
```

- On failure:
  - Show clear error message.

Scenes:

- Save:
  - `Saving scene...` then `Scene saved`
- Load:
  - `Loading scene...` then `Scene loaded`
- Rename:
  - `Renaming scene...` then `Scene renamed`
- Delete:
  - `Deleting scene...` then `Scene deleted`
- On failure:
  - Show actual error.

Palettes:

- Select:
  - `Selecting palette...` then `Palette selected`
- Save/update/delete/reset:
  - Show pending/success/error.

---

### 13. Avoid Over-Refreshing After Actions

Audit current handlers and reduce unnecessary refreshes.

Example concern:

A generic handler should not always refresh:

- state
- favorites
- scenes
- palettes
- timer
- preview

Instead:

- Favorite load: apply state, maybe refresh favorites only if needed.
- Scene load: apply state and refresh preview; do not refresh palette/favorites unless necessary.
- Scene save/delete/rename: refresh scenes.
- Palette save/delete/select: refresh palettes and state.
- Night Guard set: refresh state only.
- Surprise Me: refresh/apply state and preview only.
- Timer action: refresh timer and state.

This reduces heap pressure and makes UI faster.

---

### 14. Add Browser Console Diagnostics

When an endpoint fails, console log an object like:

```js
console.warn('API request failed', {
  url,
  statusCode,
  error,
  rawTextPreview,
  time: new Date().toISOString()
});
```

Do not show raw secrets.

This helps debug whether failures are:

- HTTP 500
- invalid JSON
- timeout
- abort
- low heap partial response
- network issue

---

## Part 1 Firmware-Side Constraints

Do not make `/api/diagnostics` huge again.

Keep detailed endpoint metrics in compact `/api/resources`.

Do not put detailed endpoint metrics back into `/api/diagnostics`.

If firmware changes are required:

- Keep them minimal.
- Preserve routes and JSON field names.
- Do not increase heap pressure.
- Do not add new features.

---

## Part 1 Testing Checklist

### Browser behavior

Test on the real D1 mini page:

1. Hard refresh the page.
2. Confirm:
   - State loads.
   - Mode list loads.
   - Favorite list either loads or shows empty/default message, not false unavailable.
   - Scene list either loads or shows empty/save prompt, not false unavailable.
   - Palette list either loads or shows empty/reset prompt, not false unavailable.
   - Preview renders from cached state.
   - Global status says connected.

### User feedback

Verify:

- Brightness change shows visible saving/saved feedback.
- Color change shows visible saving/saved feedback.
- Mode change shows visible feedback.
- Night Guard update shows pending and then enabled/off state correctly.
- Surprise Me never gets stuck.
- Scene load/save shows success/error.
- Palette actions show success/error.
- Favorite action shows success/error.
- Timer start/cancel shows success/error.

### ESP8266 stability

Use `/api/resources` after testing.

Record:

- free heap
- max free block
- fragmentation
- endpoint heap metrics
- whether any endpoint caused route-local heap floor below 2,000 bytes

Do not claim improvement unless measured.

---

# Part 2 — Motion Smoothness / Perceptual Animation Refinement Pass

## User Observation

The user reports that physical animations feel slightly “skippy,” not because frames are obviously dropping, but because the animation itself appears to make brightness/color changes in steps that are too large.

Interpretation:

This is likely perceptual stepping / animation value quantization, not basic frame-rate lag.

The goal is to make existing animations feel smoother and more fluid while preserving their visual richness.

Do NOT replace beautiful mode math with crude approximations.
Do NOT remove layered waves, shimmer, texture, palette behavior, or mode personality.
Do NOT globally blur every mode into mush.
Do NOT touch Wi-Fi/API/docs except as needed for compile/tests/report.
Do NOT change default brightness above 100.
Do NOT add new hardware support.

The LED modes are the soul of the project. This pass is suspension tuning, not engine replacement.

---

## Current Known Animation Context

The firmware uses a central render/output pipeline:

1. Render current mode, transition, or diagnostic test into `frameBuffer`.
2. Apply RGB calibration gains.
3. Apply effective brightness / Night Guard cap.
4. Apply gamma correction if enabled.
5. Write pixels.
6. Call `strip.show()` centrally.

Preserve this architecture.

The firmware throttles animated mode updates to about 33 ms when not dirty. That is roughly 30 FPS. Do not assume the problem is only frame rate. The observed issue sounds like per-frame value deltas are too large or some procedural texture/noise changes are too abrupt.

Runtime heap is tight. Be careful with new static buffers. One extra 140 LED RGB buffer costs about 420 bytes; that may be acceptable if justified, but do not add multiple large buffers casually.

---

## Part 2 Main Goal

Make current animations feel smoother by reducing abrupt brightness/color jumps.

Focus on:

- Temporal smoothing
- Perceptual brightness smoothing
- Smoothed procedural texture/noise
- Gentler per-frame deltas
- Preserving visual quality and mode personality

---

## Part 2 Hard Scope Boundaries

### Do implement if useful

- Small helper functions for temporal interpolation
- Smoothed noise/hash helpers
- Optional per-mode smoothing factors
- Adaptive output smoothing for calm modes
- Delta limiting for calm/sleep/evening modes
- Tunable constants for smoothing strength
- Documentation/report describing what changed

### Do not implement

- New lighting modes
- New palettes
- New scenes
- UI redesign
- New API families
- ESP32 migration
- Hardware controls
- Microphone/light sensor support
- Large heap-heavy structures
- Massive renderer rewrite
- Crude math simplification
- Reduced animation complexity

---

## Required Rule: Preserve Utility Sharpness

Do not smooth modes/actions that are supposed to be immediate or diagnostic.

Bypass smoothing for:

- Strobe
- Flash
- Diagnostic LED tests
- Pixel Walk
- Alternating test
- Clear/off test
- Off action
- Warm Dim Now / Panic Warm immediate action
- Any route/action that explicitly needs immediate feedback

Smoothing should primarily affect calm/ambient modes.

---

## Step 1 — Inspect Animation Update Flow

Before editing, inspect:

- `updateLeds()`
- `renderCurrentModeToFrame()`
- `outputFrameToStrip()`
- transition rendering
- diagnostic test rendering
- mode metadata tags
- Night Guard effective brightness
- existing frame buffers:
  - `frameBuffer`
  - `previousFrame`
  - `targetFrame`
  - any transition buffers

Document:

- Current animation frame interval
- Where smoothing could be safely inserted
- Which modes should bypass smoothing
- Whether an extra smoothing buffer is justified

Do not blindly rewrite.

---

## Step 2 — Identify Likely Steppy Sources

Review modes for abrupt value changes.

Look especially for patterns like:

```cpp
hash8(... + now / 713)
hash8(... + now / 997)
hash8(... + now / 1600)
random-like texture changes
integer bucket time changes
hard threshold sparkle/glint behavior
sudden flash/glint conditions
large brightness floor-to-peak changes
```

These can cause visual stepping even when frame rate is stable.

Do not remove the texture. Smooth it.

---

## Step 3 — Add Smoothed Noise Helper

If modes use hash/time texture that jumps between discrete buckets, add a helper like:

```cpp
uint8_t smoothHash8(uint16_t seed, uint32_t now, uint16_t periodMs) {
  uint32_t bucket = now / periodMs;
  uint16_t progress = (now % periodMs) * 255UL / periodMs;
  uint8_t a = hash8(seed + bucket);
  uint8_t b = hash8(seed + bucket + 1);
  return lerp8(a, b, progress);
}
```

Use actual project helper names/types.

Requirements:

- No dynamic allocation
- Cheap integer math if possible
- Preserves texture personality
- Avoids abrupt noise jumps

Apply this only to texture/noise terms that visibly step.

Do not replace all hash uses blindly.

---

## Step 4 — Consider Output Temporal Smoothing

Evaluate adding a single optional smoothing buffer:

```cpp
RgbPixel smoothedFrame[LED_COUNT];
bool smoothedFrameInitialized;
```

Memory cost at 140 LEDs:

- 420 bytes

If added, apply only when appropriate.

Conceptual flow:

```cpp
renderCurrentModeToFrame(now);

if (shouldSmoothCurrentMode()) {
  smoothFrameToward(frameBuffer, smoothedFrame, smoothingAmount);
  output smoothedFrame through pipeline;
} else {
  output frameBuffer through pipeline;
}
```

Alternative:

- Smooth in-place using an existing safe buffer if one is unused outside transitions.
- Do not break transitions.

Important:

- Do not smooth diagnostic tests.
- Do not smooth strobe/flash.
- Do not double-smooth transition frames unless tested and intentional.
- If `transitionState.active`, either bypass smoothing or use very light smoothing only if it does not make transitions sluggish.

If adding a buffer worsens compile/static RAM too much, skip this and use mode-local smoothing/noise smoothing instead.

---

## Step 5 — Add Mode Smoothing Metadata / Helper

Use existing mode metadata if possible.

Create helper:

```cpp
bool shouldApplyTemporalSmoothing(Mode mode);
uint8_t smoothingAmountForMode(Mode mode);
```

Guidance:

- Strong smoothing for sleep/calm modes
- Medium smoothing for weather/nature/evening modes
- Light smoothing for colorful modes
- None for utility/flashing/diagnostic modes

Example behavior:

- Satin Breathing: medium/strong
- Paper Lantern Row: medium
- Rain Glass: light/medium
- Velvet Aurora: light/medium
- Dream Aquarium: medium
- Circuit Glow: light
- Storm Outside: light/medium, but do not erase lightning/glint intent
- Strobe/Flash: none
- Solid: optional none or very light only when changing state manually

Do not make every mode feel slow or smeared.

---

## Step 6 — Add Per-Channel Delta Limiting If Better Than Lerp

Another acceptable strategy:

```cpp
uint8_t approach8(uint8_t current, uint8_t target, uint8_t maxStep);
```

This limits per-frame jumps.

Example:

- Calm modes: max 2–5 counts/frame
- Weather/colorful modes: max 5–10 counts/frame
- Utility modes: no limit

This may look more natural than fixed lerp at low brightness.

If implemented, document max steps.

Do not use floating point if integer math is simpler.

---

## Step 7 — Preserve Gamma/Brightness/Night Guard Order

Smoothing should happen before final output processing unless there is a specific reason otherwise.

Preferred order:

1. Mode renders raw frame.
2. Optional temporal smoothing between raw frames.
3. RGB calibration.
4. Effective brightness / Night Guard cap.
5. Gamma correction.
6. Strip output.

Do not smooth after gamma unless clearly justified.

Do not change master brightness semantics.

Do not bypass Night Guard.

---

## Step 8 — Tune Existing Modes Without Downgrading

If specific mode formulas have hard thresholds or abrupt changes, gently refine them.

Examples:

- Replace hard sparkle threshold with softened falloff.
- Replace integer-time hash texture with interpolated hash.
- Reduce brightness delta between adjacent states only if visual character remains.
- Add small crossfade between random texture samples.
- Avoid sudden jumps in palette position.

Do not:

- Remove shimmer
- Remove texture
- Remove palette blending
- Make everything static
- Remove mode-specific identity

---

## Step 9 — Add Optional Compile-Time Constants

Add small constants in `Config.h` or a local header if useful:

```cpp
const bool ENABLE_MOTION_SMOOTHING = true;
const uint8_t DEFAULT_CALM_SMOOTHING = 48;
const uint8_t DEFAULT_WEATHER_SMOOTHING = 36;
const uint8_t DEFAULT_COLORFUL_SMOOTHING = 28;
```

Use names that fit the project.

If runtime UI controls are tempting, defer them. Do not add UI controls in this pass unless trivial and low risk.

---

# Combined Testing Checklist

## Compile/tests

Run:

```powershell
arduino-cli compile --fqbn esp8266:esp8266:d1_mini firmware/BedroomLedController
python -m unittest discover -s tests -v
```

If local D1 mini is connected and upload is appropriate, upload over USB only:

```powershell
arduino-cli upload -p COM5 --fqbn esp8266:esp8266:d1_mini firmware/BedroomLedController
```

Do not perform OTA upload unless explicitly asked.

---

## Browser behavior

Test on the real D1 mini page:

1. Hard refresh the page.
2. Confirm:
   - State loads.
   - Mode list loads.
   - Favorite list either loads or shows empty/default message, not false unavailable.
   - Scene list either loads or shows empty/save prompt, not false unavailable.
   - Palette list either loads or shows empty/reset prompt, not false unavailable.
   - Preview renders from cached state.
   - Global status says connected.

---

## User feedback

Verify:

- Brightness change shows visible saving/saved feedback.
- Color change shows visible saving/saved feedback.
- Mode change shows visible feedback.
- Night Guard update shows pending and then enabled/off state correctly.
- Surprise Me never gets stuck.
- Scene load/save shows success/error.
- Palette actions show success/error.
- Favorite action shows success/error.
- Timer start/cancel shows success/error.

---

## Animation visual testing

After upload, visually test these modes at brightness 100:

- Solid
- Rain Glass
- Velvet Aurora
- Dream Aquarium
- Quiet Northern Lights
- Satin Breathing
- Paper Lantern Row
- Storm Outside
- Slow Pulse
- Slow Wave

Confirm:

- Existing modes feel smoother, not blurrier.
- No mode loses its visual identity.
- No new lighting modes are added.
- No renderer math is downgraded.
- Utility/diagnostic modes remain immediate.
- Night Guard still works.
- Transitions still work.
- Central output pipeline remains intact.

Also verify bypass modes:

- Strobe still sharp
- Flash still sharp
- Diagnostic red/green/blue/whiteLow still immediate
- Pixel walk still immediate
- Off still immediate
- Warm Dim Now still immediate

---

## ESP8266 stability

Use `/api/resources` after testing.

Record:

- free heap
- max free block
- fragmentation
- endpoint heap metrics
- whether any endpoint caused route-local heap floor below 2,000 bytes

Do not claim improvement unless measured.

---

# Documentation / Reports

Update:

```text
docs/generated/BedroomLedController_Resource_And_Migration_Report.md
docs/generated/BedroomLedController_Final_Report.md
docs/generated/BedroomLedController_Troubleshooting.md
docs/generated/Phase_Completion.md
```

Create, if useful:

```text
docs/generated/BedroomLedController_Live_UI_Feedback_Report.md
docs/generated/BedroomLedController_Motion_Smoothness_Report.md
```

Report:

- What UI feedback was added
- What request queue/loading changes were made
- Which sections now distinguish empty vs unavailable
- Whether Surprise Me stuck state was fixed
- Whether Night Guard stale state was fixed
- What caused the perceived animation stepping
- What smoothing approach was implemented
- Which modes use smoothing
- Which modes bypass smoothing
- Memory/static RAM impact
- Compile resource change
- Live heap/resource impact if measured
- Physical LED visual observations if actually observed
- Remaining known limitations

Be honest:

- Do not claim physical LED visual verification unless observed.
- Do not claim heap improvement unless measured.
- Do not claim OTA upload unless actually performed.
- Do not claim animation improvement unless visually observed.

---

# Final Response Format

When finished, respond:

```text
UI feedback and motion smoothness pass complete.

Files changed:
- ...

User feedback:
- ...

API request handling:
- ...

Startup loading:
- ...

Preview:
- ...

Favorites/scenes/palettes:
- ...

Surprise Me:
- ...

Night Guard:
- ...

Motion smoothness:
- ...

Smoothing approach:
- ...

Modes affected:
- ...

Modes bypassed:
- ...

Output pipeline:
- ...

ESP8266 protection:
- ...

Resource impact:
- RAM globals/statics:
- IRAM:
- Flash/IROM:
- Runtime heap if measured:

Visual test status:
- ...

Compile/test status:
- ...

Live hardware status:
- ...

Measured resources:
- ...

Known limitations:
- ...

Recommended next step:
- ...
```

Be specific and honest.

Do not claim:

- physical LED visual testing unless physically observed
- OTA upload unless actually performed
- heap improvements unless measured
- animation visual improvement unless observed
- full reliability if an endpoint still times out or fails

One-sentence mission:

Make the existing Bedroom LED Controller UI feel responsive and trustworthy on the real D1 mini, then make the existing animations feel smoother and more fluid by reducing abrupt brightness/color stepping — all without adding features, downgrading the beautiful lighting modes, or stressing the ESP8266 harder than necessary.
