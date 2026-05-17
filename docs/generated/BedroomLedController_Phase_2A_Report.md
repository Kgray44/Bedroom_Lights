# Bedroom LED Controller Phase 2A Report

## Scope

Phase 2A adds persistent named scene presets. A scene saves the current lighting look so it can be recalled later from the main UI or a local API route.

This phase does not add favorite bars, Panic Warm, Night Guard, timers, schedules, smooth transitions, palettes, browser live preview, QR/NFC links, new lighting modes, physical controls, microphone support, light sensors, or new hardware assumptions.

## Files changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/Config.h`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/Diagnostics.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/ScenePresets.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `tests/test_phase2a_contract.py`
- `docs/generated/BedroomLedController_Phase_2A_Report.md`
- `docs/generated/Phase_Completion.md`

## Scene data model

Phase 2A adds `struct ScenePreset`, stored in a bounded static array:

- `ScenePreset scenes[MAX_SCENES]`
- `MAX_SCENES = 16`
- `MAX_SCENE_NAME_LENGTH = 32`

A scene stores:

- Stable URL-safe `id`
- User-facing `name`
- `mode`
- RGB color
- White temperature and white-temperature source flag
- Master brightness
- Existing mode-specific timing/count settings
- `builtin` marker

Scenes intentionally do not store diagnostic state, power settings, uptime, Wi-Fi state, heap state, warning state, OTA status, or controller-level hardware configuration.

## Scene storage

Scenes are stored separately from current settings at:

```text
/scenes.json
```

`/settings.json` continues to represent the current/restored controller settings. `/scenes.json` stores saved presets.

Missing `/scenes.json` loads built-in default scenes and saves them if LittleFS is available. Corrupt or unreadable scene storage falls back to built-ins and records `sceneLoadStatus`.

## Built-in default scenes

Built-in default scenes:

- Warm Dim
- Reading
- Sleep
- Movie
- Focus
- Rain Glass
- Moon Tide
- Lantern Drift

Default scene modes only use modes available in the current sketch, with unavailable mode names falling back to Solid during built-in creation.

Built-in delete policy: Phase 2A allows built-in scenes to be deleted. They return if `/scenes.json` is removed or reset.

## Scene API endpoints

Added:

- `GET /api/scenes`
- `GET /api/scenes/save?name=...`
- `GET /api/scenes/load?id=...`
- `GET /api/scenes/rename?id=...&name=...`
- `GET /api/scenes/delete?id=...`

Scene API responses follow the Phase 1D JSON convention with `ok: true` on success and `ok: false` plus `error` on failure.

Validation covers empty names, duplicate names, scene limits, unknown IDs, malformed scene files, and unavailable modes.

## Scene UI behavior

The main page now includes a Scenes section with:

- Save-current-scene input
- Scene count/max count
- Scene list
- Color swatches
- Mode/brightness/temperature summary
- Load buttons
- Rename buttons
- Delete buttons
- Inline success/error message area

The UI fetches `/api/scenes` on load and refreshes the list after mutations. Loading a scene also refreshes the current lighting state.

## Save/load/rename/delete behavior

Save current state:

- Trims and length-limits the name.
- Rejects empty names.
- Rejects duplicate names.
- Generates a stable URL-safe ID.
- Captures the current central settings into a scene.
- Saves `/scenes.json`.

Load scene:

- Finds by stable ID.
- Validates the mode still exists.
- Applies scene state through the central Phase 1D mutation helpers.
- Marks current settings dirty so normal settings persistence can keep the recalled state.
- Does not bypass the output pipeline.

Rename scene:

- Keeps the scene ID stable.
- Validates the new name.
- Rejects duplicate names.
- Saves `/scenes.json`.

Delete scene:

- Removes the scene by ID.
- Saves `/scenes.json`.
- Returns an error for unknown IDs.

## Settings-vs-scenes separation

Scenes store lighting look, not controller-level hardware configuration. Power settings stay in `/settings.json`; diagnostic and runtime fields are not stored in scenes.

Gamma/RGB calibration decision: Phase 2A treats gamma and RGB gains as global output calibration, not scene-specific style. Scene recall does not overwrite gamma or RGB gain values.

## Error handling

Scene errors return compact JSON and do not crash the controller:

- Scene name is required
- Duplicate scene name
- Scene limit reached
- Scene not found
- Scene mode is unavailable
- Scene save failed

Scene storage status is surfaced in diagnostics as scene count, max scenes, load status, and save status.

## Memory and limits

The scene count is fixed at 16. Names and IDs are length-limited. Scene list JSON is separate from `/api/state` so the normal state endpoint stays compact.

## Compile/test status

Verified in this pass:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch-folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase2a_contract -v`: passed, 8 tests.
- `python -m unittest discover -s tests -v`: passed, 36 tests.

The firmware was compiled successfully, but not physically tested on LEDs.

## Known limitations

- Scene behavior was compiled and contract-tested, but not physically tested on LEDs.
- Scene recall applies immediately; smooth transitions are intentionally deferred.
- Built-in scenes can be deleted in Phase 2A.
- Scene API is local GET-based control only.
- No favorite bar, scheduling, timers, or QR/NFC shortcuts exist yet.

## Deferred

- Favorite scene bar
- Panic Warm
- Night Guard
- Timers and scheduler
- Smooth transitions
- Palette builder
- Browser live animation preview
- Random scene generator
- QR/NFC scene links
- New lighting modes
- Physical controls
- Music-reactive behavior
- Room-aware brightness

## Recommended next step

Phase 2B: favorite scene bar, Panic Warm, and Night Guard.
