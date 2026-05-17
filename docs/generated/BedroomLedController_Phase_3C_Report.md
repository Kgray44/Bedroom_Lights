# Bedroom LED Controller - Phase 3C Report

Phase 3C completed the browser-side Live Preview and safe Surprise Me generator.

## Browser preview architecture

- Added a Live Preview section to the main controller page.
- Added a `<canvas>` preview strip with `PREVIEW_PIXELS = 48`.
- Kept the preview entirely browser-side. It uses `/api/state`, `/api/modes`, and `/api/palettes` state data plus local JavaScript animation logic.
- Did not add WebSocket, SSE, or streamed LED frame endpoints.
- Added `requestAnimationFrame` rendering with `document.visibilityState` handling.
- Added reduced-motion throttling through `prefers-reduced-motion`.
- Added a Pause Preview toggle.

## Preview rendering method

The preview samples the current state, mode, brightness, white temperature, active color, and active palette. It then renders an approximate LED strip locally in the browser. This is meant as a visual guide, not a physical LED measurement.

Helpers added:

- `hexToRgb`
- `rgbToCss`
- `scaleRgb`
- `blendRgb`
- `samplePaletteColor`
- `kelvinToApproxRgb`
- `activePreviewPaletteColors`
- `previewBaseColor`

Modes with dedicated preview approximations:

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

Unknown modes fall back to the solid preview.

## Surprise Me generator architecture

- Added `SurpriseCandidate` as a bounded generated-lighting model.
- Added `SurpriseControls.h` for mood normalization, candidate generation, safety checks, JSON output, and apply behavior.
- Added `/api/surprise/options` for supported moods and generator rules.
- Added `/api/surprise?mood=...&apply=1` to generate and apply a safe state.
- Added main UI controls for mood selection, Surprise Me, summary output, and saving the current surprise as a scene through `/api/scenes/save`.
- Added last Surprise Me fields to state and diagnostics:
  - `lastSurpriseSummary`
  - `lastSurpriseMood`
  - `lastSurpriseStatus`

## Surprise moods

Supported moods:

- Calm
- Evening
- Sleep-safe
- Focus
- Colorful
- Cozy

Mood selection chooses from existing firmware modes and existing palette data. It does not create duplicate scene storage.

## Safety rules

The generator excludes unsafe or disruptive modes:

- Never selects Strobe.
- Never selects Flash.
- Excludes any mode marked as flashing by metadata.
- Respects Night Guard blocking.
- Applies Night Guard brightness caps.
- Applies the configured power-estimate brightness cap.
- Cancels an active diagnostic LED test before applying.
- Cancels an active timer or fade before applying.
- Uses central state mutation paths and starts the existing transition path.

## Palette integration

Surprise Me uses existing palette storage and tags. Calm moods prefer calm palettes, cozy and evening moods prefer warm or sleep-safe palettes, colorful moods prefer colorful palettes, and focus avoids palette animation unless a mode needs it. Missing palette references fall back safely.

## Power cap behavior

Surprise brightness is bounded by mood first, then capped by:

- `buildPowerEstimate().recommendedBrightnessCap`
- `settings.nightGuardMaxBrightness` when Night Guard is enabled

The response includes whether a power cap or Night Guard cap was applied.

## Resource report

Measured with Arduino CLI compile for `esp8266:esp8266:d1_mini` on the worktree sketch:

- RAM globals/static: 47,228 / 80,192 bytes (58%).
- IRAM: 61,047 / 65,536 bytes (93%).
- Code in flash: 496,420 / 1,048,576 bytes (47%).

Runtime measurements:

- Free heap after /api/palettes: not measured.
- Free heap after /api/surprise/options: not measured.
- Free heap after opening the main browser page: not measured.
- LittleFS free space after palette and scene data: not measured.
- Browser preview CPU cost on the controller: not applicable; preview rendering is browser-side.

The firmware compiled successfully but was not physically tested on LEDs.

## Board recommendation

The D1 mini remains viable for Phase 3C because the sketch compiles and the new preview work is browser-side. However, IRAM is already high at 93%. For later phases with heavier networking, sensors, physical controls, or richer APIs, an ESP32-S3-class board is recommended over continuing to grow the ESP8266/D1 mini build.

## Phase 3D

Recommended next Phase 3D focus:

- Weekly schedules or reusable automation rules.
- Additional transition styles only if memory remains acceptable.
- Browser preview polish after real-device observation.
- More runtime heap measurement on the actual controller.
