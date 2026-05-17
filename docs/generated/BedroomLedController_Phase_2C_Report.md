# BedroomLedController Phase 2C Report

## Phase 2C

Phase 2C adds one-shot sleep timers and bedtime fades to the Bedroom LED Controller. It builds on the Phase 2B central actions, scene loading, Off action, Warm Dim Now action, and Night Guard protection.

## Files changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/Config.h`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/TimerControls.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `firmware/BedroomLedController/Diagnostics.h`
- `tests/test_phase2c_contract.py`
- `docs/generated/BedroomLedController_Phase_2C_Report.md`
- `docs/generated/Phase_Completion.md`

## Timer state model

Phase 2C adds a runtime-only `ActiveTimerState` model. It supports one active timer or fade at a time and tracks:

- active/inactive state
- timer mode
- final action
- target scene ID
- start time from `millis()`
- duration and fade duration
- fade active state
- progress and remaining time
- completed/cancelled status
- captured start, target, and restore lighting states

Active timers do not persist across reboot. This is intentional for Phase 2C. A reboot cancels the active one-shot countdown and the controller falls back to normal boot behavior.

## Timer actions supported

Supported final actions:

- Off, through the Phase 2B central Off action.
- Warm Dim Now, through the Phase 2B central Warm Dim Now action.
- Panic Warm, through the Phase 2B central action alias.
- Scene load, through the Phase 2A scene loader.

Timer completion uses existing central action and scene paths rather than bypassing the controller state model.

## Timer UI behavior

The main page now includes a Sleep Timer section with:

- 15, 30, 60, and 120 minute quick buttons
- custom minutes input
- delay or fade mode selector
- final action selector
- scene selector for scene timers
- start and cancel controls
- active timer status display

The UI polls `/api/timer` for status so the web page can show remaining time and progress without blocking the ESP8266.

## Timer API endpoints

Added:

- `/api/timer`
- `/api/timer/start`
- `/api/timer/cancel`

Example starts:

- `/api/timer/start?minutes=15&mode=delay&action=off`
- `/api/timer/start?minutes=30&mode=fade&action=warmDimNow`
- `/api/timer/start?minutes=60&mode=fade&action=scene&sceneId=sleep`

Validation rejects invalid minutes, unknown modes, unknown actions, and missing or unavailable scene IDs.

## Fade engine behavior

The fade engine is a one-shot timer fade, not the future Phase 3 global transition system.

Behavior:

- Captures start and restore states once.
- Builds an Off, Warm Dim, Panic Warm, or scene target state once.
- Uses unsigned `millis()` arithmetic.
- Updates fade progress in `serviceActiveTimer()`.
- Uses Solid mode while interpolating brightness and color/temperature.
- Applies the final central action or scene exactly at completion.
- Restores the pre-fade lighting state on cancel.

Fade to scene is intentionally simplified: the controller interpolates toward the scene's color/brightness during the fade, then loads the target scene at completion so animated scene modes remain owned by the existing scene system.

## Bedtime fade behavior

Added `/api/bedtime/start` as a friendly wrapper around the fade timer. The main page has a Bedtime Fade section with:

- duration selector
- target selector for Warm Dim, Sleep scene, or Off
- start button
- cancel button

`/api/bedtime/cancel` aliases timer cancellation.

## Bedtime fade defaults and persistence

Persistent bedtime preference fields were added:

- `bedtimeFadeDefaultMinutes`
- `bedtimeFadeDefaultTarget`
- `bedtimeFadeTurnOffAtEnd`

Defaults:

- `bedtimeFadeDefaultMinutes = 30`
- `bedtimeFadeDefaultTarget = "warmDim"`
- `bedtimeFadeTurnOffAtEnd = false`

The active timer/fade itself is not persisted.

## Interactions

Scenes:

- Timer scene actions validate scene IDs before starting.
- Scene targets use `loadSceneById()` at completion.
- If Night Guard blocks a scene mode, the timer start is rejected.

Panic Warm/Off:

- Warm Dim Now and Off use `runActionByName()`.
- Timer completion reuses the central Phase 2B action path.

Night Guard:

- Fades continue through the normal output pipeline, so effective brightness caps still apply.
- Night Guard blocking is checked for scene targets before a timer starts.

Diagnostic tests:

- Starting a timer stops active diagnostic LED tests before the timer/fade begins.

## Reboot behavior

Active timers do not persist across reboot. Bedtime defaults persist as ordinary settings, but an in-progress countdown or fade is cancelled by power loss or restart.

## Flash-wear prevention behavior

The flash-wear rule for Phase 2C is simple: fade frames must not become flash writes.

Fade steps update runtime lighting state without marking settings dirty. Final actions or explicit bedtime default changes own persistence. This avoids repeated flash writes during long fades.

## Compile/test status

Verified in this pass:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch-folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase2c_contract -v`: passed, 9 tests.
- `python -m unittest discover -s tests -v`: passed, 53 tests.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Known limitations

- Compiled and contract-tested only; not physically tested on LEDs.
- No weekly scheduler.
- No recurring routines.
- No NTP routine engine.
- No global smooth transition engine.
- Fade to scene uses a simple interpolation followed by final scene load.
- `bedtimeFadeTurnOffAtEnd` is persisted for future behavior but not used as a second-stage post-fade action in Phase 2C.

## Intentionally deferred

- Full weekly scheduler
- Day-of-week scheduling
- Recurring routines
- Global smooth transitions between arbitrary modes
- Palette builder
- Browser live animation preview
- Random scene generator
- QR/NFC scene links
- Physical controls
- Microphone support
- Light sensor support

## Recommended next step

Phase 2D mode tags, filtering, practical scenes, and final everyday usability polish.
