# BedroomLedController Phase 2D Report

## Phase 2D

Phase 2D adds mode tags, practical scenes, and everyday usability polish on top of the Phase 2A scene system, Phase 2B favorite and Night Guard layer, and Phase 2C timer/bedtime behavior.

## Files changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/ModeMetadata.h`
- `firmware/BedroomLedController/ScenePresets.h`
- `firmware/BedroomLedController/ComfortControls.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `firmware/BedroomLedController/Diagnostics.h`
- `tests/test_phase2d_contract.py`
- `docs/generated/BedroomLedController_Phase_2D_Report.md`
- `docs/generated/Phase_Completion.md`

## Mode metadata

Added a compact `ModeMetadata` model covering every actual firmware mode:

- Solid
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

Each row has an ID, display name, description, best-used-for text, category, and tags for animated, calm, sleep-safe, focus-friendly, colorful, flashing, utility, experimental, and Night Guard blocked behavior.

## Mode metadata API

Added `/api/modes`.

The endpoint returns the full mode metadata list, including category, tags, `bestUsedFor`, and the `blockedByNightGuard` flag. The regular state JSON stays compact and only includes the current mode metadata summary.

## Mode filter UI

The main UI now has a dedicated Mode section with filter chips:

- All
- Everyday
- Calm
- Sleep-safe
- Focus
- Colorful
- Flashing
- Utility
- Weather

The selected mode now shows its category, description, best-use guidance, tags, and a Night Guard blocked warning when applicable.

## Night Guard metadata integration

Night Guard blocking now reads mode metadata instead of hardcoding Strobe and Flash inside the blocking helper. The older `isHarshModeName()` helper remains as a compatibility wrapper, but it delegates to `modeIsFlashing()`.

The metadata-owned blocking path is used by:

- direct mode changes
- `/api/mode`
- scene recall
- favorite mode loading
- timer scene validation through existing scene loading checks

## Practical scenes

Added practical built-in scenes using existing modes only:

- Study / Focus
- Workbench Mode
- Sera Call
- Cozy Goblin Mode

Also refined the default Movie scene to a lower-brightness Moon Tide profile. Scene presets now carry lightweight category and usability flags:

- `category`
- `isFavoriteCandidate`
- `isSleepSafe`
- `isFocusFriendly`

Existing `/scenes.json` files are merged with missing built-ins when possible, without overwriting user-created scene entries.

## Favorite defaults

Updated the default favorite order to be more practical:

- Off
- Warm Dim Now
- Cozy Goblin
- Sleep
- Study / Focus
- Movie
- Rain Glass
- Workbench

The favorite cap remains `MAX_FAVORITES = 8`.

## Current Light summary

Added a Current Light summary section to the main UI. It summarizes:

- current mode display name
- last loaded scene
- brightness and effective brightness
- timer status
- Night Guard state

The firmware now tracks `lastLoadedSceneId`, `lastLoadedSceneName`, and whether the current state still matches the last loaded scene. Direct manual changes to brightness, color, white temperature, or mode clear the scene match state.

## State and diagnostics additions

`/state` and `/api/state` now include:

- `modeDisplayName`
- `modeCategory`
- `modeTags`
- `lastLoadedSceneId`
- `lastLoadedSceneName`
- `currentStateMatchesLoadedScene`
- `nightGuardEffectiveBrightness`

Diagnostics now include mode display name, mode category, mode tags, last loaded scene, and scene-match state.

## UI organization

The main UI now exposes clearer everyday sections:

- Quick Actions
- Current Light
- Color and White Temperature
- Mode
- Scenes
- Timers / Bedtime
- Night Guard
- Advanced / Diagnostics

Existing OTA, diagnostics, favorite, scene, timer, bedtime fade, calibration, and Night Guard controls remain available.

## Preserved behavior

- No new lighting algorithms were added.
- No smooth transition engine was added.
- No weekly scheduler was added.
- Existing render modes still use the Phase 1B frame-buffer output pipeline.
- Scenes still avoid storing global power and calibration settings.
- Active timers remain runtime-only.
- OTA and browser updater routes remain wired.

## Compile/test status

Verified in this pass:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch-folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase2d_contract -v`: passed, 9 tests.
- `python -m unittest discover -s tests -v`: passed, 62 tests.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Known limitations

- Compiled and contract-tested only; not physically tested on LEDs.
- Existing scene files are merged with missing built-ins but not rewritten to override user-modified built-in scenes.
- Mode filtering is a browser UI convenience; the canonical mode list remains `/api/modes`.
- Comfort UI preferences remain deferred because the current UI already uses a dark, compact layout.
- No live animation preview.
- No palette builder.
- No QR/NFC scene links.
- No physical controls.
- No microphone, light sensor, or presence sensor support.

## Recommended next step

Phase 3A: smooth transitions.
