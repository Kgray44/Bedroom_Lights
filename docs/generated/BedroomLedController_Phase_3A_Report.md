# BedroomLedController Phase 3A Report

## Phase 3A

Phase 3A adds a non-blocking smooth transition system for major lighting changes. The implementation focuses on a reliable Fade style and keeps the Phase 1B output pipeline as the only place where RGB gain, effective brightness, gamma, and `strip.show()` are applied.

## Files changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/TransitionControls.h`
- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/ScenePresets.h`
- `firmware/BedroomLedController/ComfortControls.h`
- `firmware/BedroomLedController/TimerControls.h`
- `firmware/BedroomLedController/DiagnosticTests.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `firmware/BedroomLedController/Diagnostics.h`
- `tests/test_phase3a_contract.py`
- `docs/generated/BedroomLedController_Phase_3A_Report.md`
- `docs/generated/Phase_Completion.md`

## Transition settings added

Added persistent settings:

- `transitionsEnabled`
- `transitionDurationMs`
- `transitionStyle`

Defaults:

- enabled: `true`
- duration: `1500`
- style: `fade`

Missing fields default safely when older settings files load. Active transition progress is runtime-only and is not written to flash.

## Transition runtime model

Added `TransitionState` with:

- active flag
- style
- start time from `millis()`
- duration
- target-state-applied flag
- start effective brightness
- target effective brightness

The runtime also tracks `lastOutputEffectiveBrightness` so brightness changes such as Off and Warm Dim can fade instead of snapping to the target brightness immediately.

## Buffer strategy

Phase 3A uses static frame buffers:

- `frameBuffer`: existing raw render/output buffer
- `previousFrame`: last displayed raw frame snapshot
- `targetFrame`: live target render snapshot during transition

For the current 60 LED configuration, the three raw RGB buffers use about 540 bytes. Buffers are allocated statically; no per-frame heap allocation is used.

## Supported transition styles

Implemented:

- Fade

Deferred:

- Wipe
- Dissolve

Fade is linear, non-blocking, and updates from `millis()`.

## Output pipeline integration

Transition blending happens before final output. The final output path still applies:

- RGB calibration gains
- effective brightness
- Night Guard brightness cap
- gamma correction
- `strip.show()`

The output pipeline applies these once. Transition progress does not mark settings dirty and does not save to flash.

## Night Guard

Night Guard blocking still occurs before blocked modes can be applied. Night Guard effective brightness also applies during transitions through the normal output pipeline.

## Trigger points

Transitions are started for major lighting changes:

- direct mode changes through `/set?mode=...`
- `/api/mode`
- scene load
- scene favorites
- mode favorites
- Warm Dim Now / Panic Warm
- Off action
- delay-timer completion actions

Slider-style changes for color, white temperature, and master brightness remain immediate to avoid restarting long transitions on every small input movement.

## Scene/favorite/action integration

Scene loads validate as before, respect Night Guard blocking, apply the new scene settings, remember the last loaded scene, and start a transition to the new state.

Favorite loads reuse the existing scene, action, or mode path, so favorites inherit the same transition behavior.

Warm Dim Now and Off remain independent central actions and now start a smooth Fade when transitions are enabled and duration is nonzero.

## Timer/bedtime fade interaction

Delay timer completion can use the transition system for final Off, Warm Dim, or scene actions.

Active timer fades and bedtime fades do not stack a second transition on top of the timer fade. The timer fade owns final apply behavior for its own fade path.

Manual major lighting changes cancel active timers to avoid surprise delayed changes after the user has taken over.

## Diagnostic test interaction

Starting a diagnostic LED test cancels any active transition and runs the diagnostic test directly.

Starting normal lighting while a diagnostic test is active stops the diagnostic test and transitions from the currently displayed frame toward the requested normal target.

## UI controls added

Added a Transitions section to the main UI with:

- enable smooth transitions toggle
- duration selector
- style selector
- compact transition status text

Duration options:

- Instant
- 0.5 s
- 1.5 s
- 3 s
- 5 s

## API endpoints added

Added:

- `/api/transitions`
- `/api/transitions/set`

`/api/transitions` reports settings and runtime status. `/api/transitions/set` validates enabled, duration, and style values and returns JSON errors for invalid duration or unknown style.

## State and diagnostics integration

State JSON now includes:

- `transitionsEnabled`
- `transitionStyle`
- `transitionDurationMs`
- `transitionActive`
- `transitionProgressPercent`

Diagnostics now show transition settings, active state, progress, and transition buffer memory.

## Persistence behavior

Transition settings persist in `/settings.json` with settings schema version `6`.

Runtime transition progress, frame buffers, and active transition state are not persisted. If the controller reboots mid-transition, normal boot behavior applies.

## Compile/test status

Verified in this pass:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch-folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase3a_contract -v`: passed, 9 tests.
- `python -m unittest discover -s tests -v`: passed, 71 tests.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Known limitations

- Compiled and contract-tested only; not physically tested on LEDs.
- Fade is linear; no easing curve was added in Phase 3A.
- Wipe and Dissolve are not implemented.
- Animated target modes use live target rendering during Fade, then resume normal rendering after completion.
- Color picker, temperature slider, and brightness slider changes remain immediate by design.

## Intentionally deferred

- Palette builder
- Browser live animation preview
- Random scene generation
- New lighting modes
- Weekly scheduler
- QR/NFC scene links
- Physical controls
- Microphone support
- Light sensor support

## Recommended next step

Phase 3B: palette builder.
