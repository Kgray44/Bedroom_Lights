# Bedroom LED Controller — Phase 3A Codex Prompt

## Phase 3A Title

**Smooth Mode and Scene Transitions**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 3A** of the larger upgrade plan.

Phase 1 should already have produced a stable foundation with:

- Central runtime settings model
- Persistent settings storage
- Debounced settings saves
- Master brightness control
- Central render/output pipeline
- Frame buffer or equivalent output staging
- Gamma correction foundation
- RGB calibration foundation
- Diagnostics page
- Power estimator
- LED test tools
- Local API cleanup
- Preserved OTA/browser updater routes

Phase 2 should already have produced the everyday control layer with:

- Scene presets
- Favorite scene/action bar
- Panic Warm / Warm Dim Now
- Off action
- Night Guard
- Timer actions
- Bedtime fade
- Mode tags/categories
- Mode filtering UI
- Practical everyday scenes
- Current state summary
- Organized main UI

The goal of Phase 3A is to add a true **smooth transition system** so mode changes, scene recalls, favorites, Warm Dim, Off, and other lighting state changes no longer jump abruptly.

This should make the project feel like a polished ambient lighting device instead of a microcontroller shouting, “NEW MODE NOW!” and slapping the LEDs like a startled raccoon.

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
- OTA support
- Browser firmware update route
- Persistent controller settings
- Central settings model
- Central LED output pipeline
- Diagnostics and power estimate pages
- Scene preset system
- Favorites / Panic Warm / Off / Night Guard
- Timer and bedtime fade system
- Mode metadata and filtering

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
- `/api/timer`
- `/api/timer/start`
- `/api/timer/cancel`
- `/api/bedtime/start`
- `/api/modes`

Phase 3A may add transition routes/settings, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. Transition settings model
2. Transition runtime state model
3. Previous/current/target frame handling
4. Smooth fade transition
5. Optional wipe transition
6. Optional dissolve transition
7. Transition duration settings
8. Transition integration with mode changes
9. Transition integration with scene/favorite/action changes
10. Transition interaction with timers, bedtime fade, Night Guard, Off, and diagnostics tests
11. Transition UI controls
12. Transition API endpoints
13. State/diagnostics integration
14. Documentation/report update
15. Compile/test report

Do **not** implement yet:

- Palette builder
- Browser live animation preview
- Random scene generator
- New lighting modes
- Full weekly scheduler
- QR/NFC scene links
- Physical controls
- Microphone support
- Light sensor support
- New hardware assumptions

If a later feature seems tempting, document it as deferred. The Palette Goblin and Preview Goblin must remain outside the lab until Phase 3B/3C.

---

# Phase 3A Main Goal

At the end of Phase 3A, the user should be able to:

1. Enable or disable smooth transitions.
2. Choose transition duration.
3. Choose a transition style, at least Fade.
4. Change modes without abrupt jumps.
5. Load scenes without abrupt jumps.
6. Use favorites without abrupt jumps.
7. Use Warm Dim Now and Off with graceful behavior where appropriate.
8. Keep timers/bedtime fades working without conflict.
9. Keep OTA/web UI responsive during transitions.
10. See transition status in state/diagnostics if practical.

This phase is about **visual polish and output behavior**, not adding new content.

---

# 1. Transition Settings Model

## Goal

Add persistent user settings controlling global transition behavior.

## Required Settings

Add settings equivalent to:

```cpp
bool transitionsEnabled;
uint16_t transitionDurationMs;
String transitionStyle;
```

Recommended defaults:

```text
transitionsEnabled = true
transitionDurationMs = 1500
transitionStyle = fade
```

## Supported Duration Values

Recommended UI options:

- Instant / 0 ms
- 500 ms
- 1500 ms
- 3000 ms
- 5000 ms

Keep custom input optional.

## Supported Styles

Required:

- `fade`

Optional if feasible:

- `wipe`
- `dissolve`

Do not implement complex styles if it risks instability.

A solid, reliable fade is more valuable than three broken fancy transitions wearing tiny clown shoes.

## Persistence

Transition settings should persist in the existing settings file.

If existing settings file lacks transition fields:

- Use defaults.
- Do not treat the file as corrupt.

## Acceptance Criteria

- Transition settings exist.
- Transition settings persist after reboot.
- Missing fields default safely.
- Existing settings still load.

---

# 2. Transition Runtime State Model

## Goal

Track whether a transition is currently active and how far along it is.

## Suggested State

Create a runtime state equivalent to:

```cpp
struct TransitionState {
  bool active;
  String style;
  unsigned long startedMs;
  uint16_t durationMs;
  bool targetStateApplied;
};
```

Depending on the project’s frame pipeline, also track:

```cpp
RgbPixel previousFrame[LED_COUNT];
RgbPixel targetFrame[LED_COUNT];
```

or equivalent buffers.

## Important Memory Rule

The ESP8266 has limited memory.

For 60 LEDs:

- One RGB buffer is about 180 bytes.
- Three buffers are about 540 bytes.

That is acceptable.

For much larger LED counts, memory may matter.

If LED count can be large, consider:

- Using `uint32_t` frame buffers if already used.
- Allocating only required buffers.
- Defining a maximum supported LED count.
- Documenting memory limits.

Do not allocate large buffers repeatedly.

Allocate statically or once.

## Runtime Fields To Expose

Track:

- Transition active/inactive
- Transition style
- Duration
- Elapsed time
- Progress percent

## Acceptance Criteria

- Transition runtime state is clear.
- No repeated heap allocation every frame.
- Progress can be calculated from `millis()`.
- Transition state is runtime-only unless settings are changed.

---

# 3. Frame Buffer Strategy

## Goal

Use the Phase 1B render/output pipeline to blend from an old frame to a new frame.

## Required Concept

For a transition, the controller needs:

1. The last displayed frame.
2. The target frame for the new mode/scene/action state.
3. A blended frame sent to the strip during transition.

## Recommended Buffers

Use buffers equivalent to:

```cpp
RgbPixel currentFrame[LED_COUNT];
RgbPixel previousFrame[LED_COUNT];
RgbPixel targetFrame[LED_COUNT];
RgbPixel outputFrame[LED_COUNT]; // optional if currentFrame can serve this role
```

Exact names may differ.

## Preferred Architecture

Normal rendering:

```cpp
renderCurrentModeToFrame(currentFrame);
applyFinalOutputPipeline(currentFrame);
```

Transition start:

```cpp
copyLastOutputFrameTo(previousFrame);
applyNewStateImmediatelyInSettingsOnly();
renderCurrentModeToFrame(targetFrame);
startTransition(previousFrame, targetFrame);
```

During transition:

```cpp
blend(previousFrame, targetFrame, progress, currentFrame);
applyFinalOutputPipeline(currentFrame);
```

After transition:

```cpp
transition.active = false;
renderCurrentModeNormallyAgain();
```

## Important Rule

Do not apply gamma correction or final brightness twice.

Best practice:

- Store/blend raw pre-output RGB frames.
- Then apply calibration, effective brightness, gamma, and output once.

If blending already-output-corrected frames is simpler, document it and make sure brightness/gamma are not double-applied.

Recommended:

- Blend raw RGB frames before final output pipeline.

## Animated Target Modes

For animated modes, a single target frame can become stale while transitioning.

Acceptable Phase 3A approaches:

### Option A — Static Target Snapshot

At transition start, render one target frame and fade to it.

After transition completes, resume live animation.

This is simple and acceptable.

### Option B — Live Target Render

During transition, render the target mode every frame, then blend previous frame toward the live target frame.

This looks better for animated modes but costs more logic.

Recommended:

- Use Option B if easy with existing pipeline.
- Use Option A if it keeps the implementation safer.

Document the choice.

## Acceptance Criteria

- Transition blending uses frame buffers safely.
- Final output pipeline still applies once.
- Animated modes behave acceptably after transition.
- Memory use remains reasonable.

---

# 4. Fade Transition

## Goal

Implement the required baseline smooth transition style.

## Behavior

Fade blends each pixel from previous frame to target frame over the selected duration.

Formula concept:

```cpp
blended.r = lerp(previous.r, target.r, progress);
blended.g = lerp(previous.g, target.g, progress);
blended.b = lerp(previous.b, target.b, progress);
```

Use integer math if practical:

```cpp
uint8_t lerp8(uint8_t a, uint8_t b, uint8_t amount) {
  return a + ((int16_t(b) - int16_t(a)) * amount) / 255;
}
```

Where `amount` maps progress from 0–255.

## Easing

Use linear fade for first implementation.

Optional:

- Add ease-in-out curve if simple.

Recommended optional easing:

```cpp
smoothstep = t * t * (3 - 2 * t)
```

But avoid heavy floating-point if not needed.

A good integer approximation is acceptable.

## Acceptance Criteria

- Fade transition works.
- It is non-blocking.
- It completes at the exact target state.
- It does not flicker or reset modes.

---

# 5. Optional Wipe Transition

## Goal

Optionally add a wipe transition if it can be implemented safely.

## Behavior

Wipe reveals the target frame from one end of the strip to the other.

During progress:

- Pixels before the wipe boundary show target.
- Pixels after boundary show previous.
- Optional soft edge blends around boundary.

## UI Style Name

```text
Wipe
```

Internal ID:

```text
wipe
```

## Requirements If Implemented

- Non-blocking
- Direction can be fixed left-to-right for now
- No extra complex UI needed

## Acceptance Criteria If Implemented

- Wipe works and completes at target frame.
- No blocking delays.
- Style is selectable.

## If Deferred

Document that Phase 3A implemented Fade only, with Wipe deferred.

That is acceptable.

---

# 6. Optional Dissolve Transition

## Goal

Optionally add a dissolve transition if safe.

## Behavior

Dissolve gradually replaces pixels from previous to target in a deterministic pseudo-random order.

Important:

- Do not use expensive random generation every frame.
- Use a simple deterministic hash or index pattern.

Example concept:

```cpp
uint8_t thresholdForPixel(uint16_t index);
if (thresholdForPixel(i) < progressAmount) {
  use target pixel;
} else {
  use previous pixel;
}
```

A soft dissolve can blend around threshold, but not required.

## Requirements If Implemented

- Non-blocking
- Deterministic enough not to sparkle chaotically every frame
- Completes cleanly

## If Deferred

Document that Dissolve is deferred.

Fade is enough for Phase 3A.

---

# 7. Starting a Transition

## Goal

Start transitions consistently whenever a major lighting state changes.

## Required Transition Trigger Points

Transitions should trigger when the user changes:

- Mode
- Scene
- Favorite
- Warm Dim Now / Panic Warm
- Off action
- Color, if desired
- White temperature, if desired
- Brightness, if desired

## Recommended Scope For Phase 3A

Required transition triggers:

- Mode changes
- Scene loads
- Favorite loads
- Warm Dim Now
- Off action

Optional transition triggers:

- Color picker changes
- White temperature slider changes
- Master brightness slider changes

Important:

For sliders/color picker, constant transitions on every tiny movement can feel laggy and weird.

Recommended:

- Do not start a full transition for every brightness/color slider input.
- Let direct slider changes remain immediate or lightly smoothed later.

For Phase 3A, focus on transitions between major states.

## Function Design

Add a helper like:

```cpp
void beginTransitionToCurrentState();
```

or:

```cpp
void beginTransition(const ControllerSettings& oldState, const ControllerSettings& newState);
```

Use whichever fits the current architecture.

## Important State-Change Pattern

A safe pattern:

1. Capture currently displayed frame as previous frame.
2. Apply the requested new settings/state.
3. Render target frame for new state.
4. Start transition.
5. During transition, output blended frames.
6. After transition, render normally from the new state.

## Acceptance Criteria

- Mode changes transition smoothly.
- Scene loads transition smoothly.
- Favorite loads transition smoothly.
- Warm Dim Now transitions smoothly.
- Off can fade smoothly.
- Color/brightness sliders are not made annoying.

---

# 8. Transition Interaction With Off

## Goal

Make turning off graceful without breaking the ability to turn back on.

## Required Behavior

When Off is triggered and transitions are enabled:

- Fade from current frame to black/off over the configured duration.
- At completion, apply the project’s normal Off state.

## Important Rule

Web UI and OTA must remain active.

Off means LEDs off, not controller asleep.

## Acceptance Criteria

- Off fades out if transitions enabled.
- Off is instant if transitions disabled or duration is 0.
- Turning lights back on still works.

---

# 9. Transition Interaction With Warm Dim Now / Panic Warm

## Goal

Warm Dim Now should become a graceful cozy rescue rather than an instant state slap.

## Required Behavior

When Warm Dim Now is triggered and transitions are enabled:

- Fade from current frame to warm dim target.
- At completion, active state is Solid warm dim.

## Diagnostic Test Interaction

If a diagnostic LED test is active:

- Stop diagnostic test first.
- Then transition to Warm Dim.

Or if existing Phase 2B behavior already stops tests, preserve it.

## Acceptance Criteria

- Warm Dim Now transitions smoothly.
- It still works even if scenes are broken/missing.
- It remains independent of saved scenes.

---

# 10. Transition Interaction With Scenes and Favorites

## Goal

Scene and favorite activation should feel polished.

## Scene Load Behavior

When loading a scene:

- Validate scene as before.
- Respect Night Guard blocking.
- Capture previous frame.
- Apply scene state.
- Begin transition to scene target.
- Mark current settings dirty according to existing behavior.

## Favorite Load Behavior

When loading a favorite:

- Scene favorites use scene transition behavior.
- Action favorites use action transition behavior.
- Mode favorites, if implemented, use mode transition behavior.

## Acceptance Criteria

- Loading scenes no longer jumps abruptly.
- Loading favorites no longer jumps abruptly.
- Night Guard blocked scenes still fail safely.
- Missing scene targets still fail safely.

---

# 11. Transition Interaction With Timers and Bedtime Fade

## Goal

Avoid conflict between the Phase 2C timer/fade system and the new transition system.

## Required Rule

Do not stack a transition engine on top of an active bedtime fade in a way that causes weird double fades.

## Recommended Behavior

### Delay Timer Completion

If a delay timer completes and triggers Off/Warm Dim/Scene:

- Use the transition system if transitions are enabled.

### Active Fade Timer / Bedtime Fade

If a fade timer is already actively fading:

- Let the timer fade engine control output.
- Do not start a separate smooth transition every fade step.

At the end of a fade timer:

- Apply final state directly or with transition disabled for that final apply, depending on existing behavior.

Recommended:

- Timer fade owns the fade.
- Transition system handles discrete state changes outside active fade.

## API/UI Notes

If a user manually changes scene/mode while a timer fade is active:

Choose one behavior and document it:

### Recommended

- Cancel active fade/timer when the user manually changes major lighting state.

Alternative:

- Let manual change override timer but keep timer pending.

Recommended:

- Manual major change cancels active fade/timer to avoid surprise.

## Acceptance Criteria

- Delay timer final actions can transition.
- Bedtime fade does not double-transition.
- Manual scene/mode changes during active fade are handled consistently.
- Timer API still works.

---

# 12. Transition Interaction With Night Guard

## Goal

Night Guard must still cap brightness and block harsh modes.

## Required Behavior

During transition output:

- Effective brightness should still respect Night Guard cap.
- Blocked modes should still be rejected before transition starts.

Important:

- Do not let transition blending bypass Night Guard brightness cap.
- Do not let a transition briefly show a blocked Strobe/Flash target under Night Guard.

## Acceptance Criteria

- Night Guard cap applies during transitions.
- Night Guard blocked modes do not transition in.
- Effective brightness reporting remains accurate.

---

# 13. Transition Interaction With Diagnostic LED Tests

## Goal

Diagnostic tests should remain predictable utility tools.

## Required Behavior

When a diagnostic test is active:

- Do not apply normal scene/mode transitions to the test rendering unless intentionally designed.
- Diagnostic tests should remain direct and clear.

If the user starts a scene/mode/favorite/action while a diagnostic test is active:

Recommended:

- Stop diagnostic test.
- Begin requested transition.

If user starts a diagnostic test while transition is active:

Recommended:

- Cancel transition.
- Start diagnostic test immediately.

Reason:

Diagnostics should be clear, not gracefully crossfaded into a pixel walk like a haunted runway.

## Acceptance Criteria

- Diagnostic tests remain useful.
- Starting tests cancels active transitions.
- Starting normal lighting while tests are active stops test first.

---

# 14. Transition UI Controls

## Goal

Expose transition settings in the web UI without cluttering everyday controls.

## Required UI

Add a small section under settings/advanced or visual polish:

```text
Transitions
```

Controls:

- Enable smooth transitions toggle
- Duration selector
- Style selector

Recommended duration options:

- Instant
- 0.5 s
- 1.5 s
- 3 s
- 5 s

Recommended style options:

- Fade
- Wipe, only if implemented
- Dissolve, only if implemented

## State Display

If transition is currently active, optionally show:

```text
Transitioning: 42%
```

Do not make this too prominent.

## Acceptance Criteria

- User can enable/disable transitions.
- User can choose duration.
- User can choose style among implemented styles.
- Settings persist.
- UI does not become cluttered.

---

# 15. Transition API Endpoints

## Goal

Expose transition settings and state through local API.

## Required Endpoints

Implement or normalize:

```text
/api/transitions
/api/transitions/set
```

## `/api/transitions`

Returns current transition settings and runtime status.

Example:

```json
{
  "ok": true,
  "enabled": true,
  "style": "fade",
  "durationMs": 1500,
  "active": false,
  "progressPercent": 0
}
```

## `/api/transitions/set`

Updates settings.

Example:

```text
/api/transitions/set?enabled=1&durationMs=1500&style=fade
```

Validation:

- `enabled` accepts 0/1 and optionally true/false.
- `durationMs` must be one of allowed values or clamped to safe range.
- `style` must be implemented.
- Unknown style returns JSON error.

## Optional API Action

```text
/api/transitions/cancel
```

This can cancel active transition and immediately apply target state or current blended state.

Not required unless useful for testing.

## Acceptance Criteria

- Transition status endpoint works.
- Transition settings endpoint works.
- Invalid settings return JSON errors.
- Settings persist.

---

# 16. State and Diagnostics Integration

## Goal

Expose transition state without bloating core state too much.

## `/api/state` Additions

Add compact fields:

```json
{
  "transitionsEnabled": true,
  "transitionStyle": "fade",
  "transitionDurationMs": 1500,
  "transitionActive": false,
  "transitionProgressPercent": 0
}
```

If `/api/state` is already large, include only summary fields and rely on `/api/transitions` for details.

## Diagnostics Additions

If practical, add to diagnostics:

- Transitions enabled/disabled
- Current style
- Duration
- Active/inactive
- Progress percent if active
- Buffer memory note if helpful

Do not overdo it.

## Acceptance Criteria

- UI can initialize transition controls.
- Diagnostics can show transition status if implemented.
- State JSON remains manageable.

---

# 17. Settings Persistence Updates

## Goal

Persist transition settings safely.

## Add Settings Fields

Add to settings JSON or equivalent:

```json
{
  "transitionsEnabled": true,
  "transitionDurationMs": 1500,
  "transitionStyle": "fade"
}
```

## Backward Compatibility

If these fields are missing:

- Use defaults.
- Do not mark settings corrupt.
- Save normalized settings on next write.

## Runtime State Not Persistent

Do not persist active transition progress.

If device reboots mid-transition:

- Boot according to normal boot behavior.
- Do not attempt to resume transition.

Document this.

## Acceptance Criteria

- Transition settings persist.
- Active transition state is runtime-only.
- Existing settings still load.

---

# 18. Main Loop Responsiveness

## Goal

Transitions must be non-blocking.

## Required Behavior

Use `millis()`.

Do not use `delay()` to animate transitions.

The loop must keep servicing:

- Web server
- OTA
- Timers/fades
- Animation updates
- Settings save debounce

Conceptual loop:

```cpp
void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  serviceActiveTimer();
  updateAnimationFrameWithTransitions();
  serviceSettingsSave();
}
```

Use actual project structure.

## Acceptance Criteria

- Web UI remains responsive during transitions.
- OTA handling remains present.
- Transitions do not block timers.
- Transitions do not spam settings writes.

---

# 19. Flash-Wear and Save Behavior

## Goal

Avoid saving settings every frame during transitions.

## Required Behavior

When a user triggers a state change:

- Save/dirty the new persistent state once according to existing debounce behavior.

During transition progress:

- Do not mark settings dirty every frame.
- Do not write transition progress to flash.

Changing transition settings themselves should mark settings dirty.

## Acceptance Criteria

- Transition progress does not write to flash.
- Changing duration/style/enabled saves normally.
- Scene/mode changes still save once, not every transition step.

---

# 20. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_3A_Report.md
```

The report should include:

1. Files changed.
2. Transition settings added.
3. Transition runtime model.
4. Buffer strategy and memory notes.
5. Supported transition styles.
6. Output pipeline integration.
7. How master brightness/gamma/calibration/Night Guard apply during transitions.
8. Trigger points for transitions.
9. Scene/favorite/action integration.
10. Timer/bedtime fade interaction.
11. Diagnostic test interaction.
12. UI controls added.
13. API endpoints added.
14. Persistence behavior.
15. Compile/test status.
16. Known limitations.
17. What was intentionally deferred.
18. Recommended next step: Phase 3B palette builder.

## Required Honesty

Do not claim palette builder exists.

Do not claim browser live preview exists.

Do not claim new lighting modes exist.

Do not claim transitions are physically tested unless actual LED testing was performed.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

If only Fade was implemented and Wipe/Dissolve were deferred, say so clearly.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Current Output Pipeline

Identify:

- Existing frame buffer representation
- Final output function
- Where master brightness applies
- Where gamma applies
- Where RGB gains apply
- How modes render frames
- How scene/mode/favorite/action changes are triggered
- How timer/bedtime fades modify output

Do not rewrite the whole sketch.

## Step 2 — Add Transition Settings

Add settings fields:

- enabled
- duration
- style

Add defaults, load/save, `/api/state` fields.

## Step 3 — Add Transition Buffers and Runtime State

Add previous/target/blended frame storage as needed.

Avoid repeated dynamic allocation.

## Step 4 — Add Frame Copy and Blend Helpers

Add helpers:

```cpp
copyFrame(source, dest);
blendFrames(previous, target, output, amount);
lerp8(a, b, amount);
```

## Step 5 — Add Fade Transition Implementation

Implement Fade first.

Make it reliable before adding optional styles.

## Step 6 — Integrate Transition Start Points

Hook transitions into:

- Mode change
- Scene load
- Favorite load
- Warm Dim Now
- Off

Avoid full transitions on every slider movement unless deliberately chosen.

## Step 7 — Integrate With Output Loop

Modify animation update path to:

- Render transition frame if active
- Otherwise render normal current mode

Ensure final output pipeline still applies once.

## Step 8 — Handle Timers and Diagnostics Tests

Prevent conflicts with active timer fades and diagnostic tests.

## Step 9 — Add UI Controls

Add transition controls to advanced/visual settings section.

## Step 10 — Add API Endpoints

Add `/api/transitions` and `/api/transitions/set`.

## Step 11 — Add Diagnostics Summary

Add transition status if practical.

## Step 12 — Optional Wipe/Dissolve

Only after Fade is stable.

If skipped, document as deferred.

## Step 13 — Compile and Report

Compile for D1 mini / ESP8266.

Create/update Phase 3A report.

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
- Scene/favorite/timer/Night Guard routes still work.
- `/api/modes` still works.

## Transition Settings

- Transitions enabled setting works.
- Duration setting works.
- Style setting works for implemented styles.
- Settings persist after reboot.
- Missing fields default safely.

## Transition API

- `/api/transitions` returns valid JSON.
- `/api/transitions/set?enabled=1&durationMs=1500&style=fade` works.
- Invalid style returns JSON error.
- Invalid duration returns JSON error or clamps safely.

## Fade Transition

- Mode change fades smoothly.
- Scene load fades smoothly.
- Favorite load fades smoothly.
- Warm Dim Now fades smoothly.
- Off fades to black/off.
- Transition completes at target state.
- Transitions disabled makes changes instant.
- Duration 0 behaves as instant.

## Output Pipeline

- Brightness applies once.
- Gamma applies once.
- RGB calibration applies once.
- Night Guard effective brightness applies during transition.
- No mode bypasses output pipeline.

## Timers / Bedtime Fade

- Delay timer final action can transition if appropriate.
- Active fade timer does not get double-faded.
- Bedtime fade still works.
- Manual mode/scene change during active fade follows documented behavior.

## Diagnostic Tests

- Starting diagnostic test cancels active transition.
- Starting normal lighting while diagnostic test active stops test and transitions normally.
- Pixel walk/test tools still work.

## UI

- Transition controls appear.
- Controls initialize from state.
- Changing settings updates behavior.
- Main UI remains organized.

## Responsiveness

- Web UI stays responsive during transitions.
- OTA handling remains present.
- No blocking delays introduced.
- Settings are not saved every transition frame.

## Regression

Verify existing basics still work:

- Color picker
- White temperature slider
- Master brightness slider
- Mode filtering
- Scene save/load/rename/delete
- Favorite bar
- Warm Dim Now
- Off action
- Night Guard
- Timers
- Bedtime fade
- Diagnostics page
- Power estimator
- LED tests
- OTA routes

---

# Phase 3A Non-Goals Reminder

Do not implement these yet:

- Palette builder
- Browser live animation preview
- Surprise Me generator
- New lighting mode algorithms
- Weekly scheduler
- QR/NFC scene links
- Physical controls
- Music-reactive behavior
- Room-aware brightness

If tempted, write it in the report as deferred.

The premium visual wizardry begins here, but the glitter cannon has a checklist.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 3A complete.

Files changed:
- ...

Implemented:
- ...

Transition settings:
- ...

Transition runtime/buffers:
- ...

Supported styles:
- ...

Output pipeline integration:
- ...

Trigger points:
- ...

Timers/bedtime interaction:
- ...

Night Guard interaction:
- ...

Diagnostics/test interaction:
- ...

UI/API:
- ...

Persistence:
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
- Phase 3B — palette builder.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim palettes, browser preview, random scene generation, weekly scheduling, QR/NFC, physical controls, microphone behavior, light sensor behavior, or new lighting modes unless they actually exist.

---

# One-Sentence Mission Summary

Add a non-blocking smooth transition system so mode changes, scene recalls, favorites, Warm Dim Now, and Off glide between lighting states through the central output pipeline instead of jump-cutting like a startled RGB goblin.

