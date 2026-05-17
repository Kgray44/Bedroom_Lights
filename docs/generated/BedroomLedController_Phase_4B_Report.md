# BedroomLedController Phase 4B Report

Phase 4B adds the first remaining mode-library batch while keeping the controller inside the existing D1 mini / ESP8266 architecture. The work is software-only: no hardware additions, no ESP32-only rewrite, no cloud/public remote path, no sensors, no microphone, and no server-side preview streaming.

## Files changed

- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/ModeMetadata.h`
- `firmware/BedroomLedController/PaletteControls.h`
- `firmware/BedroomLedController/SurpriseControls.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `docs/generated/BedroomLedController_Phase_4B_Report.md`
- `docs/generated/Phase_Completion.md`
- `tests/test_phase4b_contract.py`

## New modes implemented

- Window Sunrise
- Ember Quilt
- Porcelain Sky
- Hearthline
- Evening Current
- Starlit Linen
- Deep Sea Glass
- Quiet Northern Lights

The matching API keys are `windowSunrise`, `emberQuilt`, `porcelainSky`, `hearthline`, `eveningCurrent`, `starlitLinen`, `deepSeaGlass`, and `quietNorthernLights`.

## Algorithm summary

- Window Sunrise renders a directional warm morning spread with a soft moving edge and peach-sky fallback palette.
- Ember Quilt renders low, warm patchwork zones with gentle ember lift and no flashing.
- Porcelain Sky renders a pale cool-white sky field with soft cloud-like motion.
- Hearthline renders a warm linear hearth base with slow amber bands.
- Evening Current renders broad slow current motion with a calm blue-violet tint.
- Starlit Linen renders a dim warm-linen base with soft deterministic points that fade in and out.
- Deep Sea Glass renders layered cool water/glass motion with quiet cyan and blue gradients.
- Quiet Northern Lights renders slow aurora ribbons without strobe behavior or abrupt brightness jumps.

All eight renderers write through the central frame buffer and existing output pipeline. None of the new mode renderers call `strip.show()`, use `delay()`, allocate with `new`, perform file IO, or stream previews from the ESP8266.

## Metadata/tags

The new modes are registered in the central mode metadata table with display names, descriptions, best-use text, categories, safe animation flags, and palette role strings.

- Morning: Window Sunrise
- Sleep: Ember Quilt and Starlit Linen
- Everyday: Porcelain Sky
- Evening: Hearthline and Evening Current
- Weather: Deep Sea Glass
- Colorful: Quiet Northern Lights

The metadata marks the batch as calm, non-flashing, palette-capable modes. Sleep-friendly and focus-friendly usage is represented through category, text, and existing safe-mode flags rather than a separate mode-specific control surface.

## Palette behavior

Each mode can use the active palette when palette mode is enabled and the current mode supports palettes. If no active palette is available, the renderer samples a named built-in fallback palette such as `peach-sky`, `candle`, `warm-linen`, `moon-tide`, or `deep-sea`.

To keep the render loop safer on ESP8266, the fallback sampler now uses `findPaletteByIdConst(const char*)` instead of constructing an Arduino `String` while rendering pixels.

## Scene compatibility behavior

No new built-in scene slots were added, preserving the existing bounded scene model and avoiding pressure on the 16-scene cap. Existing scene save/load behavior remains generic: user scenes can store any of the new mode IDs, color, brightness, white temperature, and active palette state through the existing scene pipeline.

Existing favorites, panic, warm dim, timers, bedtime fade, and manual scene flows continue to call the same mode parser and settings mutation paths.

## Browser preview behavior

The browser Live Preview has a local JavaScript approximation for each new mode:

- `renderPreviewWindowSunrise`
- `renderPreviewEmberQuilt`
- `renderPreviewPorcelainSky`
- `renderPreviewHearthline`
- `renderPreviewEveningCurrent`
- `renderPreviewStarlitLinen`
- `renderPreviewDeepSeaGlass`
- `renderPreviewQuietNorthernLights`

These previews are client-side only and do not add ESP8266 preview streaming.

## Surprise Me integration

Surprise Me mood pools now include Phase 4B modes:

- Calm can select Porcelain Sky, Evening Current, Deep Sea Glass, Ember Quilt, and Window Sunrise.
- Evening can select Hearthline, Evening Current, Ember Quilt, Window Sunrise, and Quiet Northern Lights.
- Sleep-safe can select Ember Quilt, Starlit Linen, Evening Current, and Deep Sea Glass.
- Focus can select Porcelain Sky and Window Sunrise.
- Colorful can select Quiet Northern Lights and Deep Sea Glass.
- Cozy can select Ember Quilt, Hearthline, Window Sunrise, and Evening Current.

The existing safety filters still exclude flashing modes and Night Guard blocked modes. Porcelain Sky can use the white-temperature path for calmer focus-style output.

## Scheduler/timer/bedtime compatibility

Phase 4B does not rewrite the scheduler. Scheduled scenes, `/go` links, timers, bedtime fade, and mode changes keep using the existing route/action/scene execution paths. If a saved scene references a new Phase 4B mode, the scheduler can run it through the same scene loader used for earlier modes.

## Night Guard behavior

None of the new modes are tagged as flashing, and none are blocked by Night Guard. Night Guard continues to cap output brightness through the existing global render/output path.

## Transition behavior

The new modes participate in the existing smooth transition path because they render into the same frame buffer as the earlier mode library. No separate transition engine or special case was added.

## Existing mode cleanup

- Added shared render helpers for cool-white approximation, smoothstep easing, sine wave sampling, triangle wave sampling, and named fallback palette sampling.
- Added `findPaletteByIdConst(const char*)` to avoid per-pixel `String` construction in fallback palette lookup.
- Kept the existing mode parser, metadata JSON, preview, scene, schedule, and Surprise Me seams instead of adding parallel APIs.

## UI/API behavior

- Added the eight mode IDs to the fallback mode picker options.
- Added parser support for API mode selection using the new lowercase tokens.
- Added `modeKey()` output for state JSON, scenes, schedules, and browser UI compatibility.
- No mode-specific UI controls were added in this phase; safe defaults are used.

## Resource report

- Board target used for compile: `esp8266:esp8266:d1_mini`
- Worktree compile: passed.
- RAM globals/statics: 53,388 / 80,192 bytes (66%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Sketch size / flash code: 551,112 / 1,048,576 bytes (52%).
- Sketch size percentage of available app slot, if known: 52%.
- LittleFS usage: not measured.
- Free heap after boot: not measured.
- Free heap after loading main page: not measured.
- Free heap after /api/state: not measured.
- Free heap after /api/modes: not measured.
- Free heap after /api/palettes: not measured.
- Free heap after /api/scenes: not measured.
- Free heap after /api/schedule: not measured.
- Free heap after preview UI loads: not measured.
- Free heap while running each new Phase 4B mode, if measurable: not measured.
- OTA still works: not tested.
- Web UI responsiveness: not tested.
- Animation responsiveness: not tested.

Recommendation: the firmware still fits the D1 mini compile target, but IRAM remains high at 93%. Future phases should keep avoiding extra heavy libraries and should prefer shared helpers over large per-mode code paths.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Compile/test status

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase4b_contract -v`: passed, 7 tests.
- `python -m unittest discover -s tests -v`: passed, 108 tests.
- Arduino sketch mirror hash check: passed for 19 code files.
- Arduino sketch folder documentation check: passed, no `.md` files found.
- `tests/__pycache__` cleanup: completed.

## Limitations and deferred work

- Runtime free heap and LittleFS usage were not measured.
- OTA behavior was not tested.
- Browser responsiveness and physical LED animation quality were not tested on hardware.
- No new built-in scene slots were added.
- No per-mode controls were added.

## Phase 4C

Recommended next step: Phase 4C remaining mode library batch 2.
