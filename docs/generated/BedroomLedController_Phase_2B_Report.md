# BedroomLedController Phase 2B Report

## Phase 2B

Phase 2B adds the everyday comfort layer on top of the Phase 2A scene preset system: a favorite bar, direct Warm Dim Now and Off actions, and manual Night Guard protection.

## Files changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/Config.h`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/ScenePresets.h`
- `firmware/BedroomLedController/ComfortControls.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `firmware/BedroomLedController/Diagnostics.h`
- `firmware/BedroomLedController/LedRendering.h`
- `tests/test_phase2b_contract.py`
- `docs/generated/BedroomLedController_Phase_2B_Report.md`
- `docs/generated/Phase_Completion.md`

## Favorite bar behavior

The main web UI now includes a compact favorite bar in the Comfort section. The UI lists available favorites from `/api/favorites`, hides favorite entries whose target is unavailable, and loads favorites through `/api/favorites/load?id=...`.

Default favorites are bounded by `MAX_FAVORITES = 8` and are created from:

- Off action
- Warm Dim Now action
- Reading scene
- Sleep scene
- Movie scene
- Focus scene
- Rain Glass scene, when present
- Moon Tide scene, when present

## Favorite storage format and defaults

Favorite configuration is stored separately from scene presets at `/favorites.json`. The file stores compact favorite entries with:

- `id`
- `label`
- `type`
- `target`

This keeps favorite UI configuration separate from `/scenes.json` and from global runtime settings. Missing or corrupt favorites fall back to built-in defaults. Missing scene targets do not break the UI because favorites report `available` and the main UI filters unavailable entries.

## Favorite API endpoints

Added:

- `/api/favorites`
- `/api/favorites/load`
- `/api/favorites/reset`

The list endpoint returns `ok`, `count`, `maxFavorites`, persistence status, and favorite entries. Invalid favorite IDs return JSON errors. Reset restores built-in favorites and saves `/favorites.json` when LittleFS is available.

## Panic Warm / Warm Dim Now behavior

Warm Dim Now is implemented through the central action path and is available from the UI and API even if scene storage is missing or corrupt.

Behavior:

- Stops any active diagnostic LED test.
- Sets mode to Solid.
- Sets white temperature to warm white through the existing temperature setter.
- Sets master brightness to `WARM_DIM_BRIGHTNESS`.
- Marks settings dirty for persistence.
- Requests the next LED render.

The direct API routes are:

- `/api/action?name=warmDimNow`
- `/api/panic-warm`

## Off action behavior

The Off action is also implemented through the central action path.

Behavior:

- Stops any active diagnostic LED test.
- Sets Solid mode.
- Sets RGB color to black.
- Sets master brightness to `0`.
- Leaves the web UI, OTA route, and browser update route active.

It is exposed through `/api/action?name=off` and as the default Off favorite.

## Night Guard settings

Night Guard is a global behavior layer, not a lighting mode. It adds persistent settings:

- `nightGuardEnabled`
- `nightGuardMaxBrightness`
- `nightGuardBlockFlashingModes`
- `nightGuardForceWarmOnPanic`
- `nightGuardPreferWarmTemperature`

Defaults:

- Disabled.
- Max brightness `80`.
- Flashing-mode blocking enabled.
- Force warm on panic enabled.
- Prefer warm disabled.

## Night Guard brightness enforcement behavior

The render output pipeline now uses `effectiveBrightness` instead of raw `masterBrightness`.

When Night Guard is enabled:

```text
effectiveBrightness = min(masterBrightness, nightGuardMaxBrightness)
```

The saved `masterBrightness` is not destroyed by the cap. Disabling Night Guard restores normal output brightness behavior because the cap is applied only in the output pipeline.

## Night Guard harsh-mode blocking behavior

Night Guard blocks Strobe and Flash when `nightGuardBlockFlashingModes` is enabled. The protection applies to:

- `/set?mode=...`
- `/api/mode`
- Scene recall
- Scene favorites
- Mode favorites, if a stored favorite uses type `mode`

Blocked requests return JSON errors such as `Night Guard blocks Strobe while enabled`. Scene recall returns an error if a saved scene uses a blocked mode.

## State/diagnostics additions

`/state` and `/api/state` now include:

- `effectiveBrightness`
- `nightGuardEnabled`
- `nightGuardMaxBrightness`
- `nightGuardBlockFlashingModes`
- `nightGuardForceWarmOnPanic`
- `nightGuardPreferWarmTemperature`
- `nightGuardBrightnessCapped`
- `favoritesCount`
- `maxFavorites`
- `lastActionStatus`

Diagnostics now include Night Guard status, effective brightness, favorite count, favorite storage status, and last action status. `/api/power` reports effective brightness and whether Night Guard is currently capping output.

## Persistence changes

Night Guard settings are persisted in `/settings.json` as part of settings schema version `4`.

Favorites are persisted in `/favorites.json`. If the file is missing, empty, corrupt, or unavailable, built-in defaults are used. Creating defaults does not require scene storage to be healthy for the Off and Warm Dim Now action favorites.

## Interaction with scenes

Scene favorites call the Phase 2A scene loading path. Night Guard caps high-brightness scenes at output time without modifying the saved scene data. Night Guard rejects scenes that use Strobe or Flash while blocking is enabled.

Warm Dim Now does not require a saved scene. It uses direct settings mutation through the central action path.

## Compile/test status

Verified in this pass:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch-folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase2b_contract -v`: passed, 8 tests.
- `python -m unittest discover -s tests -v`: passed, 44 tests.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Known limitations

- The firmware can be compiled and contract-tested, but not physically tested on LEDs in this pass.
- Favorite editing is intentionally limited to default reset and load behavior.
- Warm preference does not clamp arbitrary user-selected colors or white temperatures.
- Night Guard scheduling is not implemented.
- Smooth transitions are not implemented.

## Intentionally deferred

- Timer actions
- Bedtime fade
- Weekly scheduler
- Smooth transitions
- Palette builder
- Browser live animation preview
- Random scene generator
- QR/NFC scene links
- Physical controls
- Microphone support
- Light sensor support

## Recommended next step

Phase 2C timer actions and bedtime fade.
