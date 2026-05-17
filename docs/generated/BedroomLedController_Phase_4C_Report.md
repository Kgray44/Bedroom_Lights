# BedroomLedController Phase 4C Report

Phase 4C completes the second remaining mode-library batch for the D1 mini / ESP8266 firmware. The implementation stays inside the existing local web UI, central settings model, metadata table, frame buffer renderer, palette system, browser preview, Surprise Me generator, scene system, scheduler, timer, bedtime fade, Night Guard, and smooth transition paths.

## Files changed

- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/ModeMetadata.h`
- `firmware/BedroomLedController/SurpriseControls.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `docs/generated/BedroomLedController_Phase_4C_Report.md`
- `docs/generated/Phase_Completion.md`
- `tests/test_phase4c_contract.py`

## New modes implemented

- Candle Hall
- Slow Prism
- Garden Shade
- Snowfall Hush
- Observatory Glow
- Rainy Window
- Campfire Blanket
- Northern Whisper
- Storm Outside

The matching mode keys are `candleHall`, `slowPrism`, `gardenShade`, `snowfallHush`, `observatoryGlow`, `rainyWindow`, `campfireBlanket`, `northernWhisper`, and `stormOutside`. Hyphenated user inputs such as `candle-hall` also parse correctly because the existing parser normalizes spaces, hyphens, and underscores.

## Algorithm summary

- Candle Hall: soft warm candle clusters with deterministic slow flicker and low shadow fill.
- Slow Prism: a very slow companion-color or active-palette gradient anchored to the selected color.
- Garden Shade: warm daylight blended with muted green-gold bands and slow breeze motion.
- Snowfall Hush: cool or palette-tinted low background with deterministic drifting soft particles.
- Observatory Glow: one broad moonlike glow on a slow cycle with fixed subtle star gradients.
- Rainy Window: storm-blue window ambience with tiny soft droplet beads and trails.
- Campfire Blanket: restrained warm ember movement softened into an even cozy field.
- Northern Whisper: low-brightness aurora ribbons with gentle folds and palette blending.
- Storm Outside: storm-blue ambience with rare softened lightning pulses that are further reduced while Night Guard is enabled.

All nine modes render through `renderCurrentModeToFrame()` and the existing output pipeline. None of the new mode renderers calls `strip.show()`, uses `delay()`, allocates with `new`, calls `random()` per frame, or performs file IO in the render loop.

## Metadata/tags

Each Phase 4C mode has a `ModeMetadata` row with id, display name, description, best-use text, category, safe flags, and palette role.

- Candle Hall: Evening, calm, sleep-safe, warm palette role.
- Slow Prism: Colorful, calm, sleep-safe, focus-friendly, prism palette role.
- Garden Shade: Nature, calm, focus-friendly, natural palette role.
- Snowfall Hush: Sleep, calm, sleep-safe, cool palette role.
- Observatory Glow: Sleep, calm, sleep-safe, celestial palette role.
- Rainy Window: Weather, calm, sleep-safe, weather palette role.
- Campfire Blanket: Evening, calm, sleep-safe, warm palette role.
- Northern Whisper: Colorful, calm, sleep-safe, ribbon palette role.
- Storm Outside: Weather, calm, non-flashing metadata, storm palette role.

No Phase 4C mode is tagged as `Alert`, hard-flashing, or Night Guard blocked.

## Palette behavior

All Phase 4C modes support palettes through the existing palette-compatible mode flag.

- Warm modes use `candle` or warm fallbacks while allowing tasteful active-palette tint.
- Garden Shade uses the `garden` fallback and mutes the active palette into natural green-gold bands.
- Snowfall Hush and Observatory Glow use cool `moonlight` style fallbacks and can become warmer when the active palette is warm.
- Rainy Window and Storm Outside use `storm-blue` fallback behavior.
- Slow Prism and Northern Whisper use palettes more strongly because palette color is central to those modes.

Invalid or missing active palettes fall back through the existing named-palette and active-color helpers.

## Scene compatibility behavior

No scene-storage rewrite was needed. Phase 4C modes use `modeKey(settings.mode)`, `tryModeFromString(...)`, and `setModeByName(...)`, so user scenes can save and restore every new mode. Palette-enabled scenes continue to save `scene.paletteEnabled` and `scene.paletteId`.

No new built-in scene slots were added in this phase, preserving the bounded scene model and avoiding user-scene overwrite risk.

## Browser preview behavior

Added browser-side preview approximations:

- `renderPreviewCandleHall`
- `renderPreviewSlowPrism`
- `renderPreviewGardenShade`
- `renderPreviewSnowfallHush`
- `renderPreviewObservatoryGlow`
- `renderPreviewRainyWindow`
- `renderPreviewCampfireBlanket`
- `renderPreviewNorthernWhisper`
- `renderPreviewStormOutside`

The preview remains browser-side only. No WebSocket, EventSource, server-side LED frame streaming, or high-frequency ESP8266 polling was added.

## Surprise Me integration

Phase 4C modes are added to the existing Surprise Me selector:

- Calm: Garden Shade, Snowfall Hush, Observatory Glow, Rainy Window, Campfire Blanket, Northern Whisper.
- Evening: Candle Hall, Campfire Blanket, Rainy Window, Observatory Glow, Storm Outside.
- Sleep-safe: Candle Hall, Snowfall Hush, Observatory Glow, Campfire Blanket, Northern Whisper, Rainy Window.
- Focus: Garden Shade and Slow Prism.
- Colorful: Slow Prism, Northern Whisper, and Observatory Glow.
- Cozy: Candle Hall, Campfire Blanket, Rainy Window, and Snowfall Hush.
- Weather: Garden Shade, Snowfall Hush, Rainy Window, Storm Outside, Deep Sea Glass, Rain Glass, and Dream Aquarium.
- Nature: Garden Shade, Snowfall Hush, Rainy Window, Campfire Blanket, Window Sunrise, and Golden Hour.

The UI and `/api/surprise/options` now expose Weather and Nature moods. Existing Surprise Me safety filters still reject Strobe, Flash, flashing metadata, Night Guard blocked modes, invalid palettes, and unsafe brightness caps.

## Scheduler/timer/bedtime compatibility

Phase 4C does not rewrite the scheduler, timer, or bedtime fade code. Scheduled scenes, `/go` scene links, favorites, timers, and bedtime fade can reach Phase 4C modes through the existing scene/action and mode parser paths.

## Night Guard behavior

Night Guard brightness capping continues to apply globally through `getEffectiveBrightness()` and the central output path. Storm Outside has rare softened lightning pulses, is not tagged as hard flashing, and further reduces its lightning contribution when `settings.nightGuardEnabled` is true.

## Transition behavior

Mode changes into Phase 4C modes use the existing smooth transition system because the new modes render into the same frame buffer as the rest of the library. No brightness, gamma, or calibration bypass was added.

## Existing mode cleanup

- Added `softPulseEnvelope8(...)` as a shared helper for slow deterministic particle/flicker envelopes.
- Reused the existing named fallback palette helper from Phase 4B.
- Added Weather and Nature Surprise Me moods without changing existing mood behavior.
- Left existing modes otherwise unchanged except for shared selector integration.

## UI/API changes

- Added all nine new modes to the fallback mode dropdown.
- Added API parsing for all nine normalized tokens.
- Added `modeKey()` cases for state JSON, scenes, schedules, and UI selection.
- Added Weather and Nature options to the Surprise Me mood dropdown and options JSON.
- No mode-specific control panels were added; safe defaults are used.

## Resource report

- Board target used for compile: `esp8266:esp8266:d1_mini`
- Sketch size: 567,000 / 1,048,576 bytes flash code.
- Sketch size percentage of available app slot, if known: 54%.
- RAM globals/statics: 55,628 / 80,192 bytes (69%).
- IRAM: 61,383 / 65,536 bytes (93%).
- LittleFS usage: not measured.
- Free heap after boot: not measured.
- Free heap after loading main page: not measured.
- Free heap after /api/state: not measured.
- Free heap after /api/modes: not measured.
- Free heap after /api/palettes: not measured.
- Free heap after /api/scenes: not measured.
- Free heap after preview UI loads: not measured.
- Free heap after /api/schedule: not measured.
- Free heap while running each new Phase 4C mode, if measurable: not measured.
- OTA still works: not tested.
- Web UI responsiveness: not tested.
- Animation responsiveness: not tested.
- Recommendation: stay on D1 mini for this phase, but keep worrying about IRAM headroom before adding heavy libraries or large new UI/runtime features.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Compile/test status

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase4c_contract -v`: passed, 7 tests.
- `python -m unittest discover -s tests -v`: passed, 115 tests.
- Arduino sketch mirror hash check: passed for 19 code files.
- Arduino sketch folder documentation check: passed, no `.md` files found.
- `tests/__pycache__` cleanup: completed.

## Known limitations

- Runtime free heap, LittleFS usage, OTA behavior, browser responsiveness, animation responsiveness, and physical LED output were not measured.
- Browser previews are approximations, not pixel-perfect simulations.
- Storm Outside is intentionally soft and rare rather than a high-intensity lightning effect.

## Deferred

- Advanced scene import/export.
- Full favorite drag/reorder system.
- Physical NFC reader or physical controls.
- Microphone/music-reactive behavior.
- Light sensor or room-aware brightness.
- Cloud scheduling or public internet remote access.
- ESP32-specific rewrite or migration.
- Per-mode control panels for the nine Phase 4C modes.

## Recommended next step

Phase 4D final polish, advanced scene management, import/export, and documentation consolidation.
