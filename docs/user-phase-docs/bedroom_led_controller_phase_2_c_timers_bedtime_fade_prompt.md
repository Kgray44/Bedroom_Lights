# Bedroom LED Controller — Phase 2C Codex Prompt

## Phase 2C Title

**Timer Actions + Bedtime Fade**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 2C** of the larger upgrade plan.

Phase 1 should already have produced a stable foundation with:

- Central runtime settings model
- Persistent settings storage
- Debounced settings saves
- Master brightness control
- Central render/output pipeline
- Gamma correction foundation
- RGB calibration foundation
- Diagnostics page
- Power estimator
- LED test tools
- Local API cleanup
- Preserved OTA/browser updater routes

Phase 2A should already have added or started:

- Scene preset data model
- Scene storage, preferably `/scenes.json`
- Built-in starter scenes
- Save current state as scene
- Recall/load scene
- Rename scene
- Delete scene
- List scenes
- Scene API endpoints
- Scene UI

Phase 2B should already have added or started:

- Favorite scene/action bar
- Favorite storage/defaults
- Panic Warm / Warm Dim Now action
- Off action
- Night Guard settings
- Night Guard effective brightness cap
- Night Guard harsh-mode blocking
- Night Guard API/UI integration

The goal of Phase 2C is to add **temporary timer actions** and a **bedtime fade system**.

This phase should let the user do things like:

- Turn off in 15 minutes
- Fade to off over 30 minutes
- Fade to Warm Dim over 20 minutes
- Run Sleep scene for 1 hour, then turn off
- Start a bedtime fade now
- Cancel the active timer/fade

This should make the controller feel dramatically more useful at night without adding any new hardware.

This is the “I am lying in bed and refuse to negotiate with seven sliders” phase. The LEDs must become civilized.

---

# Current Project Context

The project already includes:

- ESP8266 / D1 mini target
- WS2812 LED strip control using Adafruit NeoPixel
- Local web UI
- Existing animation modes
- Existing calm modes
- Existing `/state` and `/set` compatibility routes
- Phase 1 `/api/...` routes
- Existing OTA support
- Browser firmware update route
- Persistent controller settings
- Central settings model
- Central LED output pipeline
- Diagnostics and power estimate pages
- Phase 2A scene preset system
- Phase 2B favorites, Panic Warm, Off, and Night Guard

Existing routes that must remain functional:

- `/`
- `/state`
- `/set`
- `/ota`
- `/update`
- `/diagnostics`
- `/api/state`
- `/api/brightness`
- `/api/color`
- `/api/temperature`
- `/api/mode`
- `/api/diagnostics`
- `/api/power`
- `/api/test`
- `/api/scenes`
- `/api/scenes/save`
- `/api/scenes/load`
- `/api/scenes/rename`
- `/api/scenes/delete`
- `/api/favorites`
- `/api/favorites/load`
- `/api/favorites/reset`
- `/api/action`
- `/api/nightguard`
- `/api/nightguard/set`

Phase 2C may add timer/fade routes, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. One active temporary timer at a time
2. Timer state model
3. Timer UI controls
4. Timer API endpoints
5. Cancel timer behavior
6. Fade engine for timer-based fades
7. Bedtime fade action
8. Bedtime fade UI controls
9. Optional bedtime fade saved settings
10. Timer/fade state in `/state` and diagnostics
11. Interaction with scenes, Panic Warm, Off, and Night Guard
12. Persistence decisions for timer settings
13. Documentation/report update
14. Compile/test report

Do **not** implement yet:

- Full weekly scheduler
- Day-of-week schedule editor
- NTP-based recurring routines unless a tiny helper already exists and is kept isolated
- Smooth visual mode transitions between arbitrary modes
- Palette builder
- Browser live animation preview
- Random scene generator
- QR/NFC scene links
- New lighting modes
- Physical controls
- Microphone support
- Light sensor support
- New hardware assumptions

If you see a tempting later-phase feature, document it as deferred. The Scheduler Goblin is still not allowed into the kitchen.

---

# Phase 2C Main Goal

At the end of Phase 2C, the user should be able to:

1. Start a timer from the web UI.
2. Choose what happens when the timer finishes.
3. Start a gradual fade toward Warm Dim, Sleep, or Off.
4. See active timer/fade status.
5. Cancel the active timer/fade.
6. Trigger timer/fade behavior through local API.
7. Reboot without corrupting settings.
8. Keep OTA/web UI responsive while fades run.

This phase is about **temporary one-shot behavior**, not recurring schedules.

---

# 1. Timer State Model

## Goal

Add a small runtime state model for one active timer/fade.

## Required Concept

Only one active timer/fade is required in Phase 2C.

Do not build a full multi-schedule engine yet.

## Suggested Enum / State Fields

Create something equivalent to:

```cpp
enum TimerActionType {
  TIMER_ACTION_NONE,
  TIMER_ACTION_OFF,
  TIMER_ACTION_WARM_DIM,
  TIMER_ACTION_SCENE,
  TIMER_ACTION_PANIC_WARM
};

enum TimerModeType {
  TIMER_MODE_NONE,
  TIMER_MODE_DELAY_THEN_ACTION,
  TIMER_MODE_FADE_TO_ACTION
};

struct ActiveTimerState {
  bool active;
  TimerModeType mode;
  TimerActionType action;
  String targetSceneId;
  unsigned long startedMs;
  unsigned long durationMs;
  unsigned long fadeDurationMs;
  bool fadeActive;
  bool completed;
};
```

Use names that fit the existing codebase.

## Required Fields

The runtime timer state should track:

- Whether a timer is active
- Timer type: delay-only or fade
- Final action
- Target scene ID, if final action is a scene
- Start time in `millis()`
- Duration in milliseconds
- Fade duration in milliseconds, if different
- Current progress percentage
- Completion/cancel status

## Important Rule

Do not store active timer state as normal persistent lighting settings unless intentionally designed.

A temporary timer usually does **not** need to resume after power loss in Phase 2C.

Recommended:

- Active timer/fade is runtime-only.
- Timer presets/settings can persist.
- Active countdown does not need to persist.

Document this clearly.

## Acceptance Criteria

- One active timer can be represented.
- Timer progress can be calculated from `millis()`.
- Timer can be cancelled.
- Timer state is separate from permanent settings.

---

# 2. Timer Action Types

## Goal

Define what a timer can do when it completes.

## Required Actions

Support at least:

### Off

At timer completion, run the existing Off action from Phase 2B.

### Warm Dim Now / Panic Warm

At timer completion, run the existing Warm Dim Now action from Phase 2B.

### Load Scene

At timer completion, load a scene by ID from the Phase 2A scene system.

## Optional Actions

Optional if easy:

### Restore Previous State

Capture current state when timer starts and restore it when timer ends.

This is useful but not required in Phase 2C.

### Keep Current But Dim

Only reduce brightness while leaving mode/color alone.

Useful later, but not required.

## Required Behavior

When a timer completes:

- Execute the selected action through central action/scene functions.
- Mark settings dirty if the final action changes persistent current state.
- Clear active timer state.
- Update `/state` and UI.

## Acceptance Criteria

- Timer can turn lights off.
- Timer can activate Warm Dim.
- Timer can load a scene.
- Timer completion does not bypass existing state/output pipeline.

---

# 3. Timer UI Controls

## Goal

Add simple timer controls to the main web UI.

## Required UI Section

Add a section called something like:

```text
Timers
```

or:

```text
Sleep Timer
```

Recommended controls:

### Duration Buttons

Provide quick buttons:

- 15 min
- 30 min
- 1 hour
- 2 hours

Optional:

- Custom minutes input

### End Action Selector

Options:

- Turn Off
- Warm Dim Now
- Load Scene...

If Load Scene is selected:

- Show scene dropdown from existing scene list.

### Timer Mode Selector

Options:

- Wait, then do action
- Gradually fade to action

### Start Button

Starts the timer/fade.

### Cancel Button

Cancels the active timer/fade.

### Active Timer Display

Show:

- Active/inactive
- Remaining time
- Action at end
- Fade progress, if fading
- Cancel button

## UI Style

Keep it lightweight and clear.

This section should be usable while sleepy.

No tiny hidden buttons. No cockpit nonsense. No “please enter duration in milliseconds like a firmware goblin.”

## Acceptance Criteria

- User can start a 15/30/60/120 minute timer.
- User can choose Off/Warm Dim/Scene action.
- User can choose delay-only or fade.
- Active timer status is visible.
- User can cancel timer.
- Existing main controls still work.

---

# 4. Timer API Endpoints

## Goal

Expose timer behavior through local API endpoints.

## Required Endpoints

Implement or normalize:

```text
/api/timer
/api/timer/start
/api/timer/cancel
```

Optional:

```text
/api/timer/status
```

or make `/api/timer` return status by default.

## `/api/timer`

Returns current timer state.

Example:

```json
{
  "ok": true,
  "active": true,
  "mode": "fadeToAction",
  "action": "off",
  "targetSceneId": "sleep",
  "durationMs": 1800000,
  "elapsedMs": 300000,
  "remainingMs": 1500000,
  "progressPercent": 16,
  "fadeActive": true
}
```

## `/api/timer/start`

Starts a timer.

Recommended query parameters:

```text
/api/timer/start?minutes=30&mode=delay&action=off
/api/timer/start?minutes=30&mode=fade&action=warmDimNow
/api/timer/start?minutes=60&mode=fade&action=scene&sceneId=sleep
```

Supported parameters:

- `minutes`: positive integer, bounded
- `mode`: `delay` or `fade`
- `action`: `off`, `warmDimNow`, `scene`
- `sceneId`: required if action is `scene`

## `/api/timer/cancel`

Cancels active timer/fade.

Recommended behavior:

- Cancelling stops the future action.
- If fade was active, either keep the current partially faded state or restore the pre-fade state.

Recommended Phase 2C behavior:

- Capture state before fade starts.
- On cancel, restore pre-fade state.

If restoration is too large for this phase, keep current state and document it.

## Validation

Validate:

- `minutes` must be numeric and within a sane range.
- Recommended maximum: 240 minutes or 480 minutes.
- `mode` must be known.
- `action` must be known.
- Scene ID must exist if action is scene.

## Response Format

Use Phase 1D JSON conventions.

Success:

```json
{
  "ok": true,
  "message": "Timer started"
}
```

Error:

```json
{
  "ok": false,
  "error": "Unknown timer action"
}
```

## Acceptance Criteria

- Timer status endpoint works.
- Timer start endpoint works.
- Timer cancel endpoint works.
- Invalid inputs return JSON errors.
- Existing APIs still work.

---

# 5. Non-Blocking Timer Service

## Goal

Timers and fades must be non-blocking.

The web UI, OTA, and animation loop must remain responsive.

## Required Behavior

Add a service function called from `loop()` or existing runtime update path:

```cpp
void serviceActiveTimer();
```

or equivalent.

Conceptual loop:

```cpp
void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  serviceActiveTimer();
  updateAnimationFrame();
  serviceSettingsSave();
}
```

Use actual project loop structure.

## Delay-Only Timer

For delay-only mode:

- Wait until `millis() - startedMs >= durationMs`.
- Execute final action.
- Clear timer state.

## Fade Timer

For fade mode:

- Fade gradually from starting state to target state over duration.
- At the end, execute/apply target action exactly.
- Clear timer state.

Do not use `delay()`.

Do not block for long loops.

## `millis()` Rollover

Use unsigned arithmetic patterns that are safe across rollover:

```cpp
if ((unsigned long)(millis() - startedMs) >= durationMs) {
  // complete
}
```

## Acceptance Criteria

- Timer does not block web UI.
- Fade does not block web UI.
- OTA handling remains present.
- Pixel animations continue or are intentionally overridden during fade.

---

# 6. Fade Engine Basics

## Goal

Implement basic timer-based fade behavior without implementing full Phase 3 smooth mode transitions.

This is not the global smooth-transition system yet.

This phase only needs fades used by timers/bedtime fade.

## Required Fade Types

Support at least:

### Fade to Off

Gradually reduce effective light output to off.

### Fade to Warm Dim

Gradually move toward the Warm Dim Now target:

- Warm white / amber
- Low brightness
- Solid mode target

### Fade to Scene

Gradually move toward the selected scene if feasible.

If full scene interpolation is too complex, acceptable Phase 2C simplification:

- Fade brightness down first.
- Load target scene at low brightness.
- Fade brightness up/down to target brightness.

Document the approach.

## Recommended Simple Fade Strategy

For Phase 2C, prioritize robustness over perfection.

A safe approach:

1. Capture starting state when fade begins.
2. Determine target state.
3. During fade, interpolate master brightness and RGB/white color if practical.
4. Use Solid mode during the fade if interpolating between complex animated modes would be too hard.
5. At the end, apply final target action/scene.

Alternative simpler approach:

- For any fade to Off: only fade master brightness from current effective value to 0.
- For fade to Warm Dim: fade current brightness down/up while shifting color temperature toward warm.
- For fade to Scene: fade out, load scene, then set target brightness.

The exact approach must be documented.

## Important Rule

Do not attempt to build the full Phase 3 transition engine here.

This is a one-shot timer fade, not arbitrary crossfade infrastructure.

## Captured Fade State

When fade starts, capture enough state to calculate fade progress:

- Starting mode
- Starting RGB
- Starting white temperature
- Starting brightness
- Starting usingWhiteTemperature flag
- Target mode
- Target RGB
- Target white temperature
- Target brightness
- Target usingWhiteTemperature flag

## Night Guard Interaction

If Night Guard is active:

- Fade output should still respect Night Guard effective brightness cap.
- Target brightness above cap should be visibly capped but not necessarily rewritten.

## Acceptance Criteria

- Fade to Off works.
- Fade to Warm Dim works.
- Fade to Scene works or is clearly simplified/documented.
- Fade runs over time without blocking.
- Final state is applied correctly.

---

# 7. Bedtime Fade Action

## Goal

Add a user-friendly bedtime fade action built on top of the timer/fade engine.

This should be easier than manually configuring every timer option.

## Required Behavior

Add a built-in action:

```text
Start Bedtime Fade
```

Recommended default behavior:

- Duration: 30 minutes
- Target: Warm Dim or Sleep scene if available
- End behavior: optionally turn off after reaching target, configurable

For Phase 2C, minimum:

- Start a fade from current state to Warm Dim over a selected duration.

Better:

- Let user choose:
  - Fade to Warm Dim
  - Fade to Sleep scene
  - Fade to Off

## UI Controls

Add a simple Bedtime Fade area.

Controls:

- Duration: 10, 20, 30, 45, 60 minutes
- Target: Warm Dim, Sleep scene, Off
- Start Bedtime Fade button
- Cancel button if active

Optional:

- Save these as bedtime fade defaults.

## API Endpoint

Add:

```text
/api/bedtime/start?minutes=30&target=warmDim
/api/bedtime/cancel
```

or reuse timer endpoint with convenient helper action.

Recommended:

- Use `/api/bedtime/start` as a friendly wrapper around `/api/timer/start`.
- Use `/api/timer/cancel` for cancellation, or add `/api/bedtime/cancel` alias.

## Acceptance Criteria

- User can start bedtime fade from UI.
- Bedtime fade uses non-blocking timer/fade engine.
- User can cancel bedtime fade.
- Bedtime fade target is clear.
- Existing controls remain available.

---

# 8. Bedtime Fade Defaults / Persistence

## Goal

Persist user preferences for bedtime fade defaults, but not necessarily the active countdown.

## Required Stored Fields

Add settings fields such as:

```cpp
uint16_t bedtimeFadeDefaultMinutes;
String bedtimeFadeDefaultTarget;
bool bedtimeFadeTurnOffAtEnd;
```

Recommended defaults:

```text
bedtimeFadeDefaultMinutes = 30
bedtimeFadeDefaultTarget = warmDim
bedtimeFadeTurnOffAtEnd = false
```

If using a target scene:

```text
bedtimeFadeDefaultTarget = scene:sleep
```

or separate fields:

```cpp
String bedtimeFadeTargetType;
String bedtimeFadeTargetSceneId;
```

## Persistence Rule

Persist bedtime fade **preferences**, not necessarily active timer state.

If device reboots during active fade:

Recommended Phase 2C behavior:

- Active fade is cancelled.
- Controller boots according to normal boot behavior.
- Document this limitation.

## Acceptance Criteria

- Bedtime fade default duration persists.
- Bedtime fade default target persists if implemented.
- Active timer/fade does not corrupt settings after reboot.

---

# 9. State and Diagnostics Integration

## Goal

Expose active timer/fade status to UI and diagnostics.

## `/api/state` / `/state` Additions

Add fields such as:

```json
{
  "timerActive": true,
  "timerMode": "fade",
  "timerAction": "off",
  "timerTargetSceneId": "sleep",
  "timerDurationMs": 1800000,
  "timerElapsedMs": 300000,
  "timerRemainingMs": 1500000,
  "timerProgressPercent": 16,
  "bedtimeFadeActive": true
}
```

Keep JSON compact.

If `/api/state` becomes too large, expose detailed timer status primarily via `/api/timer` and include only summary fields in `/api/state`.

## Diagnostics Additions

If practical, add to diagnostics:

- Timer active/inactive
- Remaining time
- Timer action
- Bedtime fade default settings
- Last timer completion/cancel status, optional

Do not bloat diagnostics too much.

## Acceptance Criteria

- Main UI can display active timer status.
- `/api/timer` returns detailed timer status.
- Diagnostics can show timer summary if implemented.

---

# 10. Interaction With Existing Features

## Scenes

Timer actions can load a scene.

Rules:

- Validate scene ID before starting timer if possible.
- If scene is deleted before timer completes, final action should fail safely and report status.
- Scene load must use existing scene load function.

## Favorites

Favorites can remain unchanged in Phase 2C.

Optional:

- Add a favorite action for “Bedtime Fade” later, but do not implement unless trivial.

## Panic Warm

Bedtime fade to Warm Dim can use the Panic Warm/Warm Dim target values, but should not necessarily instantly apply Panic Warm at start.

Use Warm Dim as a target state.

## Off

Timer/fade to Off should use the existing Off action at completion.

## Night Guard

Night Guard remains a global cap/protection layer.

Rules:

- Timer/fade output respects Night Guard effective brightness cap.
- Night Guard blocking of Strobe/Flash should still apply if a timer tries to load a blocked scene.
- If blocked, return error when starting timer if possible.

Recommended:

- Reject starting a timer targeting a blocked mode/scene while Night Guard blocking is enabled.

## Diagnostic LED Tests

If a diagnostic LED test is active and user starts a timer/bedtime fade:

Recommended behavior:

- Stop diagnostic test first.
- Then start timer/fade.

Alternatively:

- Reject timer start until diagnostic test is stopped.

Recommended:

- Stop diagnostic test and start timer.

Document behavior.

## Acceptance Criteria

- Timers work with scenes.
- Timers work with Panic Warm/Warm Dim.
- Timers work with Off.
- Night Guard still caps/blocks appropriately.
- Diagnostic tests do not conflict badly.

---

# 11. Runtime and Memory Constraints

## Goal

Keep timer/fade logic lightweight.

## Requirements

Avoid:

- Large dynamic allocations
- Heavy JSON every animation frame
- Blocking `delay()`
- Large scene copies every frame
- Saving settings every fade step

Prefer:

- Capture start/target states once
- Compute progress from `millis()`
- Update runtime state without saving every frame
- Save only when final persistent state changes or when user changes defaults

## Settings Save Rule During Fade

Do not mark settings dirty on every fade step.

Recommended:

- During fade, runtime state can change without saving each step.
- At the end, apply final action and mark settings dirty once.
- If user cancels, restore or keep state according to documented behavior and save only if appropriate.

This protects flash from being emotionally abused by a 30-minute fade.

## Acceptance Criteria

- Fade does not spam settings writes.
- UI remains responsive.
- Memory use remains reasonable.

---

# 12. Optional Preset Timer Buttons

## Goal

Provide simple common bedtime actions.

## Optional UI Buttons

Add buttons such as:

- Fade to Off — 30 min
- Warm Dim — 30 min
- Sleep Scene — 1 hour
- Cancel Timer

These should call the same timer API.

Do not build a full scheduler.

## Acceptance Criteria

- Quick buttons work if implemented.
- They do not duplicate timer logic.

---

# 13. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_2C_Report.md
```

The report should include:

1. Files changed.
2. Timer state model.
3. Timer actions supported.
4. Timer UI behavior.
5. Timer API endpoints.
6. Fade engine behavior and limitations.
7. Bedtime fade behavior.
8. Bedtime fade defaults and persistence.
9. Interaction with scenes, Panic Warm, Off, Night Guard, and diagnostics tests.
10. Whether active timers persist across reboot.
11. Flash-wear prevention behavior.
12. Compile/test status.
13. Known limitations.
14. What was intentionally deferred.
15. Recommended next step: Phase 2D mode tags, filtering, practical scenes, and final everyday usability polish.

## Required Honesty

Do not claim full weekly scheduling exists.

Do not claim recurring routines exist unless actually implemented.

Do not claim global smooth transitions exist unless Phase 3 has implemented them.

Do not claim active timers survive reboot unless they actually do.

Do not claim physical LED testing unless actual physical LED testing was performed.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Phase 2B Action/Scene Functions

Identify:

- Existing Off action function
- Existing Warm Dim Now/Panic Warm function
- Existing scene load function
- Existing Night Guard helpers
- Existing state mutation functions
- Existing `/api/state` response helpers

Do not rewrite these systems.

## Step 2 — Add Timer State Struct

Add runtime-only active timer state.

Add helper functions:

```cpp
void clearActiveTimer();
bool timerIsActive();
unsigned long getTimerElapsedMs();
unsigned long getTimerRemainingMs();
uint8_t getTimerProgressPercent();
```

## Step 3 — Add Target State Capture Helpers

Add helpers to capture current state and determine target states for:

- Off
- Warm Dim
- Scene

## Step 4 — Add Timer Start/Cancel Functions

Add functions:

```cpp
bool startDelayTimer(...);
bool startFadeTimer(...);
bool cancelTimer();
```

## Step 5 — Add Non-Blocking Timer Service

Add `serviceActiveTimer()` to the main loop path.

Use `millis()`.

No blocking delays.

## Step 6 — Add Fade Application Logic

Implement fade progress and state interpolation/simplification.

Keep it robust and documented.

## Step 7 — Add Timer API Routes

Add:

- `/api/timer`
- `/api/timer/start`
- `/api/timer/cancel`

Use consistent JSON responses.

## Step 8 — Add Bedtime Fade API Wrapper

Add:

- `/api/bedtime/start`
- optional `/api/bedtime/cancel`

Have these call the timer/fade engine.

## Step 9 — Add UI Controls

Update main page:

- Timer section
- Bedtime Fade section
- Active timer status
- Cancel button

## Step 10 — Add Persistence for Bedtime Defaults

Persist bedtime fade default duration/target if implemented.

Do not persist active countdown unless intentionally implemented.

## Step 11 — Update State/Diagnostics

Expose timer state to UI/API.

## Step 12 — Compile and Report

Compile for D1 mini / ESP8266.

Create/update Phase 2C report.

---

# Test Checklist

## Compile

- Sketch compiles for D1 mini / ESP8266.

## Existing Routes Still Work

- `/` works.
- `/state` works.
- `/set` works.
- `/ota` works.
- `/update` works.
- `/diagnostics` works.
- Existing `/api/...` routes still work.
- Scene API routes still work.
- Favorite/Night Guard/action routes still work.

## Timer API

- `/api/timer` returns valid JSON.
- `/api/timer/start?minutes=15&mode=delay&action=off` works.
- `/api/timer/start?minutes=30&mode=fade&action=warmDimNow` works.
- `/api/timer/start?minutes=60&mode=fade&action=scene&sceneId=sleep` works if Sleep scene exists.
- `/api/timer/cancel` works.
- Invalid minutes return JSON error.
- Invalid mode returns JSON error.
- Invalid action returns JSON error.
- Missing scene ID returns JSON error for scene action.

## Timer UI

- Timer section appears.
- Quick duration buttons work.
- End action selector works.
- Scene dropdown appears when needed.
- Start button works.
- Active timer status displays remaining time.
- Cancel button works.

## Delay-Only Timer

- Delay timer waits until duration expires.
- Final Off action works.
- Final Warm Dim action works.
- Final Scene load works.
- Timer clears after completion.

## Fade Timer

- Fade to Off works.
- Fade to Warm Dim works.
- Fade to Scene works or documented simplified behavior works.
- Fade does not block UI.
- Fade does not save settings every frame.
- Final state is applied once at completion.

## Bedtime Fade

- Bedtime Fade section appears.
- User can choose duration.
- User can choose target.
- Start Bedtime Fade works.
- Cancel works.
- Defaults persist if implemented.

## Interactions

- Night Guard caps fade output.
- Night Guard blocks blocked scenes/modes.
- Diagnostic test is stopped or rejected when timer starts, per documented behavior.
- Scene deletion before timer completion fails safely if applicable.
- OTA/web UI remain responsive.

## Persistence

- Bedtime default settings persist.
- Active timer does not corrupt settings after reboot.
- Existing settings/scenes/favorites still load.

## Regression

Verify existing basics still work:

- Color picker
- White temperature slider
- Mode dropdown
- Master brightness slider
- Scene save/load
- Favorite bar
- Warm Dim Now
- Off action
- Night Guard
- Diagnostics page
- Power estimator
- LED tests
- OTA routes

---

# Phase 2C Non-Goals Reminder

Do not implement these yet:

- Full weekly scheduler
- Day-of-week scheduling
- Recurring routines
- NTP-based routine engine
- Smooth transitions between arbitrary modes
- Palette builder
- Browser live preview
- Surprise Me generator
- QR/NFC scene links
- New lighting modes
- Physical controls
- Music-reactive behavior
- Room-aware brightness

If tempted, write it in the report as deferred.

The Scheduler Goblin may bang pots and pans. Ignore it. Today is one-shot timers only.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 2C complete.

Files changed:
- ...

Implemented:
- ...

Timer system:
- ...

Timer API:
- ...

Timer UI:
- ...

Fade engine:
- ...

Bedtime fade:
- ...

Persistence:
- ...

Interactions:
- Scenes: ...
- Panic Warm/Off: ...
- Night Guard: ...
- Diagnostics tests: ...

Preserved:
- ...

Deferred:
- ...

Compile/test status:
- ...

Known limitations:
- ...

Recommended next step:
- Phase 2D — mode tags, filtering, practical scenes, and final everyday usability polish.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim weekly scheduling, recurring routines, smooth transitions, palettes, preview, QR/NFC, physical controls, microphone behavior, or light sensor behavior unless they actually exist.

---

# One-Sentence Mission Summary

Add one-shot sleep timers and bedtime fades so the Bedroom LED Controller can gracefully dim, warm, or turn off later without blocking the ESP8266, spamming flash, or forcing a sleepy human to negotiate with RGB settings at midnight.

