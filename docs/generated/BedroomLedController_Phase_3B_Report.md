# Bedroom LED Controller - Phase 3B Report

## Phase 3B

Palette Builder + Palette-Aware Modes Foundation is implemented.

## Files changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/Config.h`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/ModeMetadata.h`
- `firmware/BedroomLedController/PaletteControls.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/ScenePresets.h`
- `firmware/BedroomLedController/TimerControls.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `firmware/BedroomLedController/Diagnostics.h`
- `firmware/BedroomLedController/LedRendering.h`
- `tests/test_phase3b_contract.py`
- `docs/generated/Phase_Completion.md`

## Palette data model

Phase 3B adds `PaletteColor` and `ColorPalette` models. Each palette stores 2-5 colors, a per-color `weight`, an id, a user-facing name, built-in/custom status, category, and simple tags for calm, sleep-safe, colorful, warm, and cool behavior.

The controller uses a bounded `ColorPalette palettes[MAX_PALETTES]` store with `MAX_PALETTES = 16` and `MAX_PALETTE_COLORS = 5`.

## Palette storage

Palette storage is separate from settings and scenes at `/palettes.json`.

The saved schema uses compact JSON:

- `version`
- active palette state summary
- palette count/max
- `palettes[]`
- each palette includes id, name, built-in flag, category, tag booleans, `colors[]`, and `weights[]`

Load behavior:

- Built-in palettes are seeded from firmware every boot.
- Missing `/palettes.json` falls back to built-ins and saves a default file when storage is available.
- Corrupt palette JSON falls back to built-ins.
- Custom palettes load only if valid, non-built-in, unique, and within the bounded palette count.

Built-in protection policy:

- Built-ins are firmware-defined and cannot be edited or deleted through the API.
- `/api/palettes/reset` restores built-ins and clears custom palettes.

## Built-in palettes

Built-in palettes added:

- Candle
- Ocean
- Garden
- Moonlight
- Peach Sky
- Soft Neon
- Aurora
- Warm Linen
- Storm Blue
- Circuit Glow

`Warm Linen` is the default active palette id, while palette use defaults to disabled.

## Active palette settings

Persistent settings added:

- `settings.paletteEnabled`
- `settings.activePaletteId`

Defaults:

- `paletteEnabled = false`
- `activePaletteId = "warm-linen"`

Settings schema is now version 7.

## Palette API endpoints

Added:

- `/api/palettes`
- `/api/palettes/select`
- `/api/palettes/save`
- `/api/palettes/update`
- `/api/palettes/delete`
- `/api/palettes/reset`

Validation covers unknown ids, invalid colors, fewer than two colors, max palette count, built-in edit/delete protection, and active palette deletion fallback.

## Palette UI behavior

The main page now includes a Palettes section with:

- Enable toggle for compatible modes.
- Active palette dropdown.
- Active swatches.
- Current mode support note.
- Built-in/custom palette list with swatches.
- Custom palette create/edit form using native color inputs.
- Custom delete controls.
- Reset palettes action.

The UI remains lightweight and uses the existing ESP-served page, not a frontend framework.

## Palette-aware mode metadata

Mode metadata now includes:

- `supportsPalette`
- `preferredPaletteRole`

Compatible existing modes are marked for future and current palette support:

- Slow Wave
- Slow Pulse
- Moon Tide
- Velvet Aurora
- Lantern Drift
- Rain Glass

`/api/modes`, `/state`, `/api/state`, and diagnostics expose palette support status.

## Palette-aware modes

Velvet Aurora gained active palette support.

When palette support is enabled and the current palette is valid, Velvet Aurora samples palette colors as soft moving ribbon/accent colors. When palettes are disabled or invalid, the prior active-color behavior remains in place.

## Scene palette reference behavior

Scenes now save:

- `paletteEnabled`
- `paletteId`

Scene load restores the palette reference if it exists. If a scene references a missing palette, palette use is disabled and the scene still loads through the existing active color fallback.

Old scenes without palette fields continue to load with palette use disabled and `warm-linen` as the default palette reference.

## Interactions

Favorites:

- Favorites continue to load scenes through `loadSceneById`, so scene palette references are restored without a separate favorite system.

Timers:

- Timer scene targets carry palette state into target snapshots and use the existing scene load path at completion.
- Timer fade steps do not save palette state or write palette files.

Transitions:

- Palette selection triggers `beginTransitionToCurrentState()` when the current mode supports palettes.
- Palette update of the active palette also refreshes the current frame and can transition.
- Timer-owned fades still suppress double-stacked smooth transitions.

Night Guard:

- Night Guard still caps effective brightness.
- Phase 3B does not block colorful palettes; palette sleep-safety is metadata/UI guidance only.

## State and diagnostics

State JSON adds compact palette fields:

- `paletteEnabled`
- `activePaletteId`
- `activePaletteName`
- `currentModeSupportsPalette`
- `currentModePaletteRole`

Diagnostics add:

- Palette enabled state.
- Active palette id/name.
- Palette storage path.
- Palette count/max count.
- Current mode palette support.
- Palette load/save status.

## Error handling

Handled safely:

- Missing `/palettes.json`
- Corrupt `/palettes.json`
- Invalid color input
- Fewer than two colors
- More than five colors
- Unknown palette id
- Built-in edit/delete attempts
- Palette count limit reached
- Deleting the active custom palette
- Scene references to missing palettes

## Memory and performance notes

- Palette count is bounded at 16.
- Palette colors are loaded into RAM once and sampled from static arrays.
- No file IO occurs during animation frames.
- Palette save operations are explicit user actions.
- `/api/state` stays compact; full palette data is served by `/api/palettes`.

## Compile/test status

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase3b_contract -v`: passed, 10 tests.
- `python -m unittest discover -s tests -v`: passed, 81 tests.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Known limitations

- Only Velvet Aurora currently uses active palette colors in rendering.
- Palette weights are stored and used by helper sampling, but the UI does not expose per-color weight controls yet.
- Built-in reset clears custom palettes in this implementation.
- The palette editor uses simple native controls and does not provide browser-side animation preview.

## Intentionally deferred

- Browser live animation preview.
- Surprise Me / random scene generator.
- New lighting mode batch.
- Weekly scheduler.
- QR/NFC scene links.
- Physical controls.
- Microphone or sensor behavior.
- Light sensor support.

## Recommended next step

Phase 3C - browser-side live preview strip and Surprise Me generator.
