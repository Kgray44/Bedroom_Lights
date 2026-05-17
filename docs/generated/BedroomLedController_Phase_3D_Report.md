# Bedroom LED Controller - Phase 3D Report

Phase 3D added the first modest batch of new lighting modes while preserving the D1 mini / ESP8266-first architecture.

## Files changed

- `firmware/BedroomLedController/Config.h`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/ModeMetadata.h`
- `firmware/BedroomLedController/ScenePresets.h`
- `firmware/BedroomLedController/SurpriseControls.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `tests/test_phase3d_contract.py`
- `docs/generated/BedroomLedController_Phase_3D_Report.md`
- `docs/generated/Phase_Completion.md`

## New modes implemented

Seven new modes were registered in the central mode enum, mode parser, mode key mapper, metadata table, render dispatch, web UI mode list, browser preview dispatch, scene-compatible mode handling, and Surprise Me generator.

## Algorithm summaries

- Satin Breathing: whole-strip slow inhale, hold, and exhale envelope with subtle deterministic per-pixel texture and a warm peak blend.
- Paper Lantern Row: soft warm lantern blobs distributed across the strip, with slow independent brightness drift and wide falloff.
- Library Lamp: stable warm/neutral light with very faint per-pixel texture for reading and desk work.
- Solar Noon: neutral daylight-style light with a very slow low-contrast warm-to-cool gradient drift.
- Golden Hour: warm amber directional gradient that slowly shifts across the strip.
- Dream Aquarium: cool watery motion with layered waves, shimmer, and optional palette drift.
- Circuit Glow: soft trace-like pulses over a dim cool base without strobing or hard flashes.

## Metadata/tags

- Satin Breathing: Sleep, animated, calm, sleep-safe, palette-compatible.
- Paper Lantern Row: Calm, animated, calm, sleep-safe, palette-compatible.
- Library Lamp: Everyday, animated, calm, sleep-safe, focus-friendly, palette-compatible.
- Solar Noon: Focus, animated, focus-friendly, palette-compatible with restrained tint.
- Golden Hour: Calm, animated, calm, sleep-safe, palette-compatible.
- Dream Aquarium: Weather, animated, calm, colorful, palette-compatible.
- Circuit Glow: Experimental, animated, colorful, experimental, palette-compatible.

All seven new modes are non-flashing and are not blocked by Night Guard metadata.

## Palette behavior

Satin Breathing, Paper Lantern Row, Golden Hour, Dream Aquarium, and Circuit Glow use the active palette directly when enabled. Library Lamp and Solar Noon apply palette influence lightly so practical lighting remains useful. Palette disabled or missing falls back to the active color or white-temperature-derived color.

## Scene compatibility

Scenes already store and restore mode IDs generically through `modeKey(settings.mode)`, `tryModeFromString(scene.mode, parsedMode)`, and `setModeByName(scene.mode)`, so all seven new modes can be saved and loaded by user-created scenes. Three optional built-in scenes were added within the existing `MAX_SCENES = 16` cap: Satin Breathing, Library Lamp, and Solar Noon. User custom scenes are not overwritten.

## Browser preview behavior

The browser-side Live Preview now has dedicated local renderers for all seven new modes:

- `renderPreviewSatinBreathing`
- `renderPreviewPaperLanternRow`
- `renderPreviewLibraryLamp`
- `renderPreviewSolarNoon`
- `renderPreviewGoldenHour`
- `renderPreviewDreamAquarium`
- `renderPreviewCircuitGlow`

Preview remains browser-side only. No WebSocket, EventSource, or server-side LED frame streaming was added.

## Surprise Me integration

Surprise Me now includes new modes by mood:

- Calm: Satin Breathing, Paper Lantern Row, Golden Hour, Dream Aquarium.
- Evening: Paper Lantern Row, Golden Hour, Satin Breathing, Dream Aquarium.
- Sleep-safe: Satin Breathing, Paper Lantern Row, Golden Hour.
- Focus: Library Lamp, Solar Noon.
- Colorful: Dream Aquarium, Circuit Glow.
- Cozy: Paper Lantern Row, Golden Hour, Satin Breathing.

The existing Surprise Me safety rules remain in place: no Strobe, no Flash, no flashing metadata, Night Guard blocking respected, and power/Night Guard brightness caps applied.

## Night Guard behavior

Night Guard caps effective brightness for all new modes through the existing output path. None of the new modes are tagged as flashing or blocked by Night Guard by default.

## Timer/transition interaction

Timers and bedtime fade remain unchanged. Timer scene loads can target user-created scenes using the new modes because scene mode restoration is generic. Smooth transitions still route through the existing `beginTransitionToCurrentState()` and frame-buffer transition path. New modes render into `frameBuffer` and do not call `strip.show()`.

## Existing mode cleanup

Existing mode metadata was reviewed while adding the new rows. No visual rewrites were made to existing modes. Existing routes, scene behavior, favorites, timers, Night Guard, transitions, palettes, diagnostics, OTA, and Phase 3C preview/surprise endpoints were preserved.

## UI/API changes

- Added the seven new modes to the mode dropdown fallback list.
- Added metadata rows so `/api/modes` exposes descriptions, categories, tags, Night Guard flags, and palette support.
- Added parser support so `/api/mode?name=...` and legacy `/set?mode=...` can select the new modes.
- Added browser preview dispatch for the new mode IDs.

## Resource report

- Board target used for compile: `esp8266:esp8266:d1_mini`.
- Sketch size: 506,452 bytes code in flash.
- Sketch size percentage of available app slot, if known: 48% of 1,048,576 bytes.
- RAM globals/static: 48,752 / 80,192 bytes (60%).
- IRAM: 61,047 / 65,536 bytes (93%).
- LittleFS usage: not measured.
- Free heap after boot: not measured.
- Free heap after loading main page: not measured.
- Free heap after /api/state: not measured.
- Free heap after /api/modes: not measured.
- Free heap after /api/palettes: not measured.
- Free heap after /api/scenes: not measured.
- Free heap after preview UI loads: not measured.
- Free heap while running each new mode, if measurable: not measured.
- OTA still works: not tested.
- Web UI responsiveness: not tested.
- Animation responsiveness: not tested.
- Recommendation: stay on D1 mini for this phase, but start worrying because IRAM remains high at 93%; consider ESP32-S3 before adding heavier runtime features.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Compile/test status

- Worktree Arduino CLI compile passed for `esp8266:esp8266:d1_mini`.
- Arduino sketch folder mirror compile passed for `esp8266:esp8266:d1_mini`.
- `python -m unittest tests.test_phase3d_contract -v`: passed, 6 tests.
- `python -m unittest discover -s tests -v`: passed, 95 tests.

## Known limitations

- No physical LED observation was performed.
- Runtime heap, LittleFS usage, OTA behavior, web responsiveness, and animation responsiveness were not measured on the real controller.
- Deeper per-mode controls are deferred; all new modes currently use safe defaults.
- Only three optional built-in scenes were added to preserve the existing scene cap.

## Deferred

- Remaining Phase 4 lighting modes.
- Weekly scheduler.
- QR/NFC scene links.
- Advanced scene import/export.
- Physical controls.
- Music-reactive behavior.
- Room-aware brightness.
- ESP32-specific rewrite.

## Recommended next step

Phase 4A - advanced scheduler and scene links, or a Phase 4 planning review before adding more mode library surface area.
