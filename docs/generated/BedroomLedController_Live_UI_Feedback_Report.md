# BedroomLedController Live UI Feedback Report

## Summary

This pass makes the ESP8266-hosted web UI more honest and less aggressive with API traffic. It does not add lighting modes, new hardware support, cloud features, or new UI feature families.

## What changed

- Added a visible global status banner under the header for connecting, saving, success, retry, and error states.
- Added a central `apiFetchJson(...)` helper with `fetch(..., { cache: 'no-store' })`, `AbortController` timeouts, retry support, friendly errors, and structured console diagnostics.
- Serialized startup loading so state loads first, followed by modes, favorites, scenes, palettes, and timer status with small delays between heavy calls.
- Added a browser-side heavy request queue for `/api/modes`, `/api/scenes`, `/api/palettes`, `/api/backup/export`, `/api/diagnostics`, and `/api/resources`.
- Reduced preview pressure by rendering from cached state and replacing the old 2.5 second preview state poll with a quieter 15 second state refresh.
- Added retryable failed-section states for favorites, scenes, palettes, and preview without requiring a full page reload.

## Empty vs unavailable

- Favorites now show `No favorites configured. Use Reset Favorites to restore defaults.` when the endpoint succeeds but no usable favorites are available.
- Scenes now show `No scenes saved yet. Save the current light as a scene.` when the endpoint succeeds with no scenes.
- Palettes now show `No palettes available. Reset built-ins or save a palette.` when the endpoint succeeds with no palettes.
- `Unavailable` messaging is reserved for endpoint failure, timeout, invalid JSON, or resource issues.

## Action feedback

- Brightness, color, white temperature, mode, gamma, RGB calibration, transitions, palette actions, scene actions, favorite actions, timer actions, bedtime fade, backup, and reset actions now update the global status area.
- Slider-style controls use debounced saving feedback rather than emitting a large notification for every tiny movement.
- Scene, favorite, palette, backup, timer, and Night Guard sections still keep their local inline messages.

## Specific fixes

- Surprise Me now uses `try` / `catch` / `finally`, disables its button while running, and always replaces the choosing message.
- Night Guard updates now show pending state, disable controls while saving, apply returned state or refresh state once, and revert visible controls on failure.
- Failed list sections include retry buttons that call only the relevant refresh path.

## ESP8266 protection

The page no longer fires the full startup API set concurrently. Heavy list and diagnostics-style requests are queued one at a time on the browser side, while lightweight control actions still apply returned state without refreshing every list.

## Critical regression stabilization update

Live testing of the previous build found that `/api/state` returned invalid JSON because `activePaletteName` emitted an extra quote before the `hex` field. That could make page refresh look like it reset controls to defaults because the UI could not hydrate real controller state.

Additional stabilization added after that finding:

- `uiHydrating` now stays true during startup and while `applyState(...)` programmatically assigns input values.
- `stateLoaded` is set only after a successful state payload is applied.
- Mutation paths call `canMutateFromUi(...)` so startup, failed state loading, and programmatic state application do not send `/set`, action, timer, scene, palette, or Night Guard writes.
- If state is unavailable, the UI reports `State unavailable - controls preserved` instead of overwriting visible controls with defaults.

Page refresh mutation check: statically guarded in code; browser Network-tab verification on the patched firmware is still pending.

## Verification status

- Firmware compile after regression stabilization: passed for `esp8266:esp8266:d1_mini`.
- Compile resources after regression stabilization: RAM 60,664 / 80,192 bytes (75%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 609,228 / 1,048,576 bytes (58%).
- Python contract tests: passed, 136 tests.
- Browser/live patched behavior: not retested because USB upload to `COM5` was blocked by `PermissionError(13, 'Access is denied.')`.
- Runtime heap impact: not remeasured in this pass.
