# Bedroom LED Controller — Phase 4C Codex Prompt

## Phase 4C Title

**Remaining Mode Library — Batch 2 + Storm Outside**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 4C** of the larger upgrade plan.

Phase 1 should already have produced a stable foundation with:

- Central runtime settings model
- Persistent settings storage
- Debounced settings saves
- Master brightness control
- Central render/output pipeline
- Frame buffer or equivalent output staging
- Gamma correction foundation
- RGB calibration foundation
- Diagnostics page
- Power estimator
- LED test tools
- Local API cleanup
- Preserved OTA/browser updater routes

Phase 2 should already have produced the everyday control layer with:

- Scene presets
- Favorite scene/action bar
- Panic Warm / Warm Dim Now
- Off action
- Night Guard
- Timer actions
- Bedtime fade
- Mode tags/categories
- Mode filtering UI
- Practical everyday scenes
- Current state summary
- Organized main UI

Phase 3 should already have produced the visual-polish layer with:

- Smooth transitions
- Palette system
- Browser-side preview strip
- Surprise Me generator
- First new lighting mode batch
- New mode metadata and preview support
- D1-mini-aware resource reporting discipline

Phase 4A should already have added or started:

- Local time sync foundation
- Weekly scheduler
- Schedule UI/API/storage
- Scene/action links
- QR/NFC-friendly copyable URLs
- Schedule/time diagnostics

Phase 4B should already have added or started the first remaining-mode-library batch:

- Window Sunrise
- Ember Quilt
- Porcelain Sky
- Hearthline
- Evening Current
- Starlit Linen
- Deep Sea Glass
- Quiet Northern Lights

The goal of Phase 4C is to implement the **second remaining-mode-library batch**, completing the major software-only lighting-mode library planned for the Bedroom LED Controller.

This phase adds the remaining calm/fancy modes from the backlog while preserving all existing behavior, keeping the implementation D1-mini-safe, and avoiding hardware assumptions.

This is the “finish the mode library without summoning the firmware kraken” phase.

---

# Critical Board / Resource Context

The project remains **D1 mini / ESP8266 first** unless resource measurements prove it cannot continue safely.

The FireBeetle ESP32-S3 N4 has much better RAM/CPU headroom, but it still has 4 MB flash, so it is not a magic storage dragon. This phase should still target the D1 mini unless the resource report says otherwise.

## Required Resource Discipline

This phase adds a large number of modes. Codex must be extra disciplined.

Pay attention to:

- Sketch size
- OTA slot margin
- Free heap
- Animation timing
- UI responsiveness
- JSON size
- Preview JS size
- Avoiding dynamic allocation in animation loops
- Avoiding per-mode large buffers
- Avoiding repeated expensive `String` operations
- Avoiding blocking `delay()` calls
- Avoiding file I/O during animation rendering
- Avoiding heavy floating-point math per pixel when simple approximations work
- Avoiding huge mode-specific UI bloat

## Required Resource Report Addition

Every Phase 4C report must include:

```text
Resource report:
- Board target used for compile:
- Sketch size:
- Sketch size percentage of available app slot, if known:
- LittleFS usage:
- Free heap after boot:
- Free heap after loading main page:
- Free heap after /api/state:
- Free heap after /api/modes:
- Free heap after /api/palettes:
- Free heap after /api/scenes:
- Free heap after preview UI loads:
- Free heap after /api/schedule:
- Free heap while running each new Phase 4C mode, if measurable:
- OTA still works: yes/no/not tested
- Web UI responsiveness: snappy/acceptable/laggy/not tested
- Animation responsiveness: snappy/acceptable/laggy/not tested
- Recommendation: stay on D1 mini / start worrying / consider ESP32-S3
```

Do not fake these values. If a value cannot be measured, write `not measured`.

---

# Current Project Context

The project already includes:

- ESP8266 / D1 mini target
- WS2812 LED strip control using Adafruit NeoPixel
- Local web UI
- Existing animation modes and calm modes
- Phase 3D first new mode batch, if implemented
- Phase 4B remaining mode batch 1, if implemented
- Existing `/state` and `/set` compatibility routes
- Phase 1 `/api/...` routes
- OTA support
- Browser firmware update route
- Persistent controller settings
- Central settings model
- Central LED output pipeline
- Diagnostics and power estimate pages
- Scene preset system
- Favorites / Panic Warm / Off / Night Guard
- Timer and bedtime fade system
- Mode metadata and filtering
- Smooth transition system
- Palette system
- Browser preview
- Surprise Me generator
- Local scheduler and scene/action links

Existing routes that must remain functional:

- `/`
- `/state`
- `/set`
- `/ota`
- `/update`
- `/diagnostics`
- `/schedule`, if Phase 4A implemented it
- `/api/state`
- `/api/brightness`
- `/api/color`
- `/api/temperature`
- `/api/mode`
- `/api/diagnostics`
- `/api/power`
- `/api/test`
- `/api/scenes`
- `/api/scenes/save`
- `/api/scenes/load`
- `/api/scenes/rename`
- `/api/scenes/delete`
- `/api/favorites`
- `/api/favorites/load`
- `/api/favorites/reset`
- `/api/action`
- `/api/nightguard`
- `/api/nightguard/set`
- `/api/timer`
- `/api/timer/start`
- `/api/timer/cancel`
- `/api/bedtime/start`
- `/api/modes`
- `/api/transitions`
- `/api/transitions/set`
- `/api/palettes`
- `/api/palettes/select`
- `/api/palettes/save`
- `/api/palettes/update`
- `/api/palettes/delete`
- `/api/palettes/reset`
- `/api/schedule`, if implemented
- `/api/time`, if implemented
- `/go` or scene/action link routes, if implemented
- Any Phase 3C preview/surprise endpoints, if implemented

Phase 4C may add new mode IDs/settings/metadata/preview renderers, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. Final remaining lighting mode registration and metadata
2. Batch 2 remaining lighting modes
3. Storm Outside mode
4. Per-mode algorithms
5. Per-mode tags/categories/descriptions
6. Per-mode palette compatibility
7. Per-mode browser preview approximations
8. Per-mode scene/save/load compatibility
9. Surprise Me integration
10. Scheduler compatibility
11. Night Guard compatibility
12. Smooth transition compatibility
13. Existing mode metadata cleanup where safe
14. Documentation/report update
15. Compile/test/resource report

Do **not** implement yet:

- Advanced scene import/export
- Full favorite drag/reorder system
- Physical NFC reader
- Physical buttons
- Rotary encoder
- Microphone/music-reactive behavior
- Light sensor/room-aware brightness
- Cloud scheduling
- Public internet remote access
- ESP32-specific rewrite
- ESP32-only features
- Server-side LED preview streaming
- WebSocket LED frame streaming

If a later feature seems tempting, document it as deferred. This is the final mode-library batch, not a permission slip for every possible feature goblin.

---

# Phase 4C Main Goal

At the end of Phase 4C, the controller should include these new modes:

1. Candle Hall
2. Slow Prism
3. Garden Shade
4. Snowfall Hush
5. Observatory Glow
6. Rainy Window
7. Campfire Blanket
8. Northern Whisper
9. Storm Outside

These modes should:

- Render through the central frame/output pipeline.
- Use master brightness.
- Use gamma correction.
- Use RGB calibration.
- Respect Night Guard.
- Support scenes.
- Support mode metadata/filtering.
- Support smooth transitions.
- Support browser preview approximations.
- Support palettes where appropriate.
- Be compatible with scheduler scene loading.
- Be compatible with Surprise Me where appropriate.
- Avoid blocking logic.
- Avoid heavy dynamic allocation.
- Keep the D1 mini viable.

---

# 1. New Mode Architecture Rules

## Goal

Add the Phase 4C modes using the existing architecture.

## Required Rules

Each new mode must:

- Have a stable internal mode ID.
- Have a user-facing display name.
- Have mode metadata.
- Have a short description.
- Have “best used for” text.
- Render into the central frame buffer.
- Avoid direct `strip.show()` inside the mode.
- Avoid direct brightness scaling if the output pipeline handles brightness.
- Avoid blocking `delay()`.
- Use `millis()` or deterministic time calculations.
- Handle low LED counts gracefully.
- Handle missing/disabled palette safely.
- Respect Night Guard brightness cap.
- Be saveable/restorable in scenes.
- Be selectable through `/api/mode` and legacy `/set` if applicable.
- Appear in `/api/modes` metadata.
- Appear in the mode filtering UI.
- Have a browser preview approximation or explicit fallback.
- Be available to Surprise Me only in safe mood categories.

## Stable Mode IDs

Use stable IDs such as:

```text
candle-hall
slow-prism
garden-shade
snowfall-hush
observatory-glow
rainy-window
campfire-blanket
northern-whisper
storm-outside
```

Display names:

```text
Candle Hall
Slow Prism
Garden Shade
Snowfall Hush
Observatory Glow
Rainy Window
Campfire Blanket
Northern Whisper
Storm Outside
```

## Acceptance Criteria

- All nine modes are centrally registered.
- All nine modes are UI/API selectable.
- All nine modes appear in metadata/filtering.
- No duplicate naming mess is introduced.

---

# 2. Shared Helper Strategy

## Goal

Avoid each mode inventing its own math cave.

## Required / Recommended Helpers

Reuse existing helpers where practical.

Add lightweight helpers if missing:

```cpp
uint8_t clamp8(int value);
uint8_t lerp8(uint8_t a, uint8_t b, uint8_t amount);
RgbPixel blendRgb(RgbPixel a, RgbPixel b, uint8_t amount);
RgbPixel scaleRgb(RgbPixel color, uint8_t scale);
RgbPixel warmWhiteApprox(uint16_t kelvin);
RgbPixel coolWhiteApprox(uint16_t kelvin);
RgbPixel sampleActivePaletteOrColor(uint16_t position, uint16_t maxPosition);
RgbPixel sampleNamedOrActivePalette(const char* fallbackPaletteId, uint16_t position, uint16_t maxPosition);
uint8_t smoothStep8(uint8_t t);
uint8_t triangleWave8(uint32_t now, uint32_t periodMs);
uint8_t sineApprox8(uint32_t now, uint32_t periodMs, uint16_t phaseOffset);
uint8_t hash8(uint16_t value);
uint8_t softPulseEnvelope8(uint32_t now, uint32_t periodMs, uint16_t phaseOffset);
```

Use integer or lookup-style math where reasonable.

Some sine usage is okay if existing code already uses it, but do not create extremely heavy per-pixel/per-layer floating-point storms.

## Deterministic Texture

For modes with small highlights, candles, snow, rain, stars, or sparks:

- Use deterministic pseudo-randomness based on pixel/slot index.
- Do not call true random every frame.
- Keep texture smooth over time.
- Avoid per-frame allocation.

## Acceptance Criteria

- Shared helpers are reused.
- Animations are smooth/deterministic.
- Memory and CPU usage remain reasonable.

---

# 3. Mode-Specific Settings Strategy

## Goal

Give modes useful controls without exploding the UI.

## Required Behavior

Every new mode should work with safe defaults.

Mode-specific controls may be added only if they fit existing conditional controls cleanly.

If not, defaults are acceptable and deeper tuning can be deferred.

## Suggested Exposed Controls

### Candle Hall

- Candle count
- Flicker softness
- Base warmth
- Shadow level

### Slow Prism

- Spread
- Rotation speed
- Contrast
- Anchor strength

### Garden Shade

- Shade tint
- Breeze speed
- Sun amount
- Contrast

### Snowfall Hush

- Snow density
- Drift speed
- Background temperature
- Highlight brightness

### Observatory Glow

- Main glow speed
- Star density
- Color mood
- Minimum brightness

### Rainy Window

- Droplet density
- Trail length
- Coolness
- Drift speed

### Campfire Blanket

- Warmth
- Movement amount
- Brightness floor
- Cozy depth

### Northern Whisper

- Color spread
- Ribbon softness
- Speed
- Brightness cap

### Storm Outside

- Storm depth
- Flash frequency
- Flash softness
- Base brightness

## Recommended First Implementation

- Add safe defaults for all modes.
- Add only compact controls if current settings structure has room.
- Store user-facing settings if exposed.
- Save exposed settings into scenes.
- Otherwise document that deeper controls are deferred.

## Acceptance Criteria

- Each mode works with defaults.
- Any exposed settings validate and persist.
- UI remains usable.

---

# 4. New Mode 1 — Candle Hall

## Purpose

Mimics a row of distant candles down a hallway in a restrained, cozy way.

It should not be a harsh flame simulator. It should feel like warm candle clusters seen through distance or haze.

## Visual Behavior

- Several candle clusters along the strip.
- Each candle occupies a small group of LEDs.
- Flicker is slow, smoothed, and irregular.
- Base warmth is amber/warm white.
- Shadow side can be influenced by active color or palette.
- Brightness range remains limited.

## Suggested Algorithm

Use a fixed number of candle centers.

For each candle:

- Center position
- Width 2–5 LEDs depending on LED count
- Warm base color
- Smooth flicker envelope
- Deterministic phase offset

For each pixel:

- Sum nearby candle contribution with soft falloff.
- Add low shadow/base level.

Flicker should be generated with slow blended noise or layered sine/triangle functions, not random snap values.

## Palette Behavior

- Prefer Candle, Warm Linen, Peach Sky.
- If active palette is warm, use palette colors for candles.
- If palette is cool/colorful, use it only for subtle shadow tint or fallback to warm.

## Metadata

- Category: Calm / Evening
- Tags: calm, warm, cozy, evening, sleep-safe, animated, palette-compatible
- Flashing: false
- Supports Palette: true

## Preview Approximation

Browser preview should show warm candle clusters with soft slow flicker.

## Acceptance Criteria

- Candle Hall is cozy and not harsh.
- Flicker is smooth.
- Night Guard cap works.
- Palette behavior is tasteful.

---

# 5. New Mode 2 — Slow Prism

## Purpose

Splits the selected color into nearby companion colors and slowly rotates a wide tasteful gradient.

This is not full rainbow chaos unless the selected color/palette invites it.

## Visual Behavior

- Takes active color or active palette as anchor.
- Creates nearby companion colors.
- Displays a wide slow gradient.
- Rotates/drifts slowly across the strip.
- Contrast is tasteful and controllable.
- Avoids harsh saturation by default.

## Suggested Algorithm

If palette enabled:

- Use active palette as prism colors.

If palette disabled:

- Generate companion colors by gently shifting hue around active color.
- Keep saturation controlled.

Use strip position plus slow time offset to sample gradient.

Default periods:

```text
60–300 seconds
```

## Palette Behavior

- Strong palette support.
- Palette colors define the prism companions.
- If palette is vivid, reduce contrast for non-colorful moods.

## Metadata

- Category: Colorful / Calm
- Tags: colorful, calm, animated, palette-compatible
- Sleep-safe: true at low brightness and low contrast
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows slow rotating gradient using active palette or companion colors.

## Acceptance Criteria

- Slow Prism is tasteful.
- It is not just Rainbow.
- Palette disabled still produces companion colors.
- Motion is slow.

---

# 6. New Mode 3 — Garden Shade

## Purpose

Soft sunlight filtered through leaves, but without high-contrast leaf flicker.

It should feel fresh, natural, and daytime-friendly.

## Visual Behavior

- Warm base light.
- Muted green/gold shade bands.
- Slow drifting breeze-like movement.
- Low-to-medium contrast.
- Avoid neon green.

## Suggested Algorithm

Use layered bands:

1. Warm sun base.
2. Green-gold shade layer.
3. Slow breeze offset.
4. Optional active color/palette tint.

Use long periods:

```text
30–180 seconds
```

## Palette Behavior

- Prefer Garden, Warm Linen, Peach Sky.
- If active palette is green/natural, use it.
- If active palette is neon/colorful, mute/desaturate heavily.

## Metadata

- Category: Weather / Nature / Everyday
- Tags: calm, natural, daytime, animated, palette-compatible
- Focus-friendly: maybe
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows warm base with slow muted green-gold bands.

## Acceptance Criteria

- Garden Shade feels natural, not neon.
- Movement is slow.
- Works as daytime ambience.

---

# 7. New Mode 4 — Snowfall Hush

## Purpose

A quiet cool-white / pale-blue drifting mode with soft points that feel muffled and calm.

It should not look like blinking holiday lights.

## Visual Behavior

- Low cool background.
- Soft tiny clusters drift slowly left/right.
- Long fade in/out.
- Some points barely move.
- Can also work as warm dust-mote mode if palette/temperature is warm.

## Suggested Algorithm

Use a fixed list of snow/dust particles.

Each particle has:

- Position
- Drift direction
- Drift speed
- Brightness envelope
- Width 2–4 LEDs
- Phase offset

Use deterministic phases and slow movement.

Do not randomize every frame.

## Palette Behavior

- Prefer Moonlight, Storm Blue, Warm Linen for warm dust variant.
- If active palette is warm, shift to dust-mote feel.
- If active palette is cool, keep snowfall feel.

## Metadata

- Category: Sleep / Weather / Nature / Calm
- Tags: calm, sleep-safe, cool, winter, night, animated, palette-compatible
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows low background with soft drifting points.

## Acceptance Criteria

- Snowfall Hush is quiet and soft.
- No harsh single-pixel sparkle.
- Works at low brightness.

---

# 8. New Mode 5 — Observatory Glow

## Purpose

Very slow celestial ambience with one broad moonlike glow and soft star gradients.

It should be atmospheric and structured, not random twinkle clutter.

## Visual Behavior

- Dark blue or selected-color base.
- One broad moonlike glow travels very slowly.
- Several subtle star gradients brighten near the main glow.
- Very slow movement.
- Avoid full black unless brightness is set low.

## Suggested Algorithm

Layers:

1. Base low background.
2. Broad main glow with soft falloff.
3. Fixed star positions with soft local glow.
4. Stars gently brighten when main glow is nearby.

Default main glow period:

```text
2–20 minutes, or shorter preview scaling if necessary
```

For normal mode, use a long cycle.

## Palette Behavior

- Prefer Moonlight, Storm Blue, Aurora.
- If active palette is warm, shift toward dusk/peach sky version.

## Metadata

- Category: Sleep / Calm / Night
- Tags: calm, night, celestial, sleep-safe, animated, palette-compatible
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows broad moon glow moving slowly with faint stars.

## Acceptance Criteria

- Observatory Glow is slow and celestial.
- Stars are subtle.
- Movement is very calm.

---

# 9. New Mode 6 — Rainy Window

## Purpose

A moodier cousin of Rain Glass: cool soft background, occasional tiny trails, long fading droplets.

It should feel like a rainy window at night, not a weather radar explosion.

## Visual Behavior

- Cool soft background.
- Droplets appear rarely or moderately.
- Droplets produce long fading trails.
- Motion is slow and emotional, not depressing.
- Low/medium brightness.

## Suggested Algorithm

Use fixed droplet lanes/slots.

Each droplet has:

- Position
- Trail length
- Fade envelope
- Drift direction/speed
- Spawn phase offset

Use deterministic cycle instead of true random every frame.

## Palette Behavior

- Prefer Moonlight, Storm Blue, Ocean.
- If active palette is warm, turn into warm rainy-window/dusty-glass ambience but still restrained.

## Metadata

- Category: Weather / Nature / Calm
- Tags: calm, rain, night, animated, palette-compatible
- Sleep-safe: true at low brightness
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows soft cool base with occasional fading trails.

## Acceptance Criteria

- Rainy Window differs from Rain Glass by being moodier/slower.
- Trails are soft.
- No harsh sparkle.

---

# 10. New Mode 7 — Campfire Blanket

## Purpose

Restrained warm amber comfort.

This is not a flame mode. It should feel like warm low-motion glow under a blanket — alive, but not flickery.

## Visual Behavior

- Warm amber/cream base.
- Gentle low-motion variation.
- Cozy depth and warmth.
- Less flickery than fire.
- More alive than solid warm white.

## Suggested Algorithm

Use broad warmth zones and slow brightness variation.

Layers:

1. Warm base floor.
2. Slow amber swell.
3. Tiny very-low-amplitude texture.

Keep periods slow:

```text
20–120 seconds
```

## Palette Behavior

- Prefer Candle, Warm Linen, Peach Sky.
- If active palette is warm, use it.
- If active palette is cool, reduce influence or fallback to warm.

## Metadata

- Category: Sleep / Calm / Evening
- Tags: calm, warm, cozy, sleep-safe, animated, palette-compatible
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows warm soft slow living glow.

## Acceptance Criteria

- Campfire Blanket is warm and calm.
- It is not a fire/flicker effect.
- It works well at bedtime brightness.

---

# 11. New Mode 8 — Northern Whisper

## Purpose

A simpler, softer aurora mode than Quiet Northern Lights.

It should use barely saturated cyan/blue/green/violet ribbons with very calm motion.

## Visual Behavior

- Soft ribbons.
- Barely saturated aurora colors.
- Very calm, low contrast.
- Less complex than Quiet Northern Lights.
- Good night ambience.

## Suggested Algorithm

Use 2–4 broad ribbons.

Each ribbon:

- Wide soft falloff.
- Slow drift.
- Low saturation.
- Palette-derived color.

Keep brightness modest.

## Palette Behavior

- Prefer Aurora, Moonlight, Ocean.
- If active palette is vivid, desaturate/reduce contrast.

## Metadata

- Category: Calm / Colorful / Night
- Tags: calm, colorful, aurora, sleep-safe, animated, palette-compatible
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows soft low-saturation ribbons.

## Acceptance Criteria

- Northern Whisper is quieter than Quiet Northern Lights.
- It is not a rainbow mode.
- Palette support works.

---

# 12. New Mode 9 — Storm Outside

## Purpose

A calm storm ambience mode.

This should feel like a rainy night outside, not Zeus committing an electrical safety violation in the bedroom.

## Visual Behavior

- Deep blue-gray base.
- Very slow dark waves.
- Occasional soft distant flash.
- Long fade after flash.
- Low brightness option.
- No rapid strobing.

## Suggested Algorithm

Layers:

1. Deep storm base.
2. Slow dark wave motion.
3. Rare distant flash envelope.
4. Flash affects broad section or whole strip softly.

Flash frequency must be low.

Flash softness must be high.

Use deterministic pseudo-random flash timing or long interval logic.

Important:

- This mode includes flashes, but they must be soft, rare, and non-strobe.
- It should not be tagged as `flashing` unless the implementation makes it visually sharp.
- If Night Guard is active, either suppress flashes or reduce flash brightness heavily.

Recommended Night Guard behavior:

- Allow Storm Outside, but suppress or heavily soften flashes when Night Guard is enabled.

## Palette Behavior

- Prefer Storm Blue, Moonlight, Ocean.
- If active palette is warm, use only slight tint or fallback to storm palette.

## Metadata

- Category: Weather / Nature / Calm
- Tags: calm, storm, rain, night, animated, palette-compatible
- Sleep-safe: maybe true only if flashes are suppressed under Night Guard / low brightness
- Flashing: false if flashes are soft/rare; true only if implementation is sharper than intended
- Supports Palette: true

## Preview Approximation

Preview shows dark blue-gray slow waves with rare soft flash.

## Acceptance Criteria

- Storm Outside is calm, not alarming.
- Flashes are rare and soft.
- Night Guard suppresses/softens flashes.
- It is not equivalent to Strobe/Flash.

---

# 13. Mode Metadata and Filtering Updates

## Goal

Every new Phase 4C mode must appear correctly in mode metadata and filtering UI.

## Required Metadata Fields

For each new mode include:

- ID
- Display name
- Description
- Best used for
- Category
- Tags
- Animated true/false
- Calm true/false
- Sleep-safe true/false
- Focus-friendly true/false
- Colorful true/false
- Flashing false unless truly necessary
- Utility false unless applicable
- Blocked by Night Guard false unless truly necessary
- Supports palette true/limited
- Preview supported true/false, if such field exists

## Suggested Category Mapping

```text
Candle Hall: Calm / Evening
Slow Prism: Calm / Colorful
Garden Shade: Weather / Nature / Everyday
Snowfall Hush: Sleep / Weather / Nature / Calm
Observatory Glow: Sleep / Calm / Night
Rainy Window: Weather / Nature / Calm
Campfire Blanket: Sleep / Calm / Evening
Northern Whisper: Calm / Colorful / Night
Storm Outside: Weather / Nature / Calm
```

## Acceptance Criteria

- `/api/modes` includes all Phase 4C modes.
- UI filters show the modes in sensible categories.
- Surprise Me can use metadata safely.

---

# 14. Palette Compatibility Updates

## Goal

Make Phase 4C modes use the palette system appropriately.

## Palette-Compatible Modes

Recommended palette support:

- Candle Hall: yes, warm-biased
- Slow Prism: yes, primary feature
- Garden Shade: yes, natural-biased
- Snowfall Hush: yes, cool/warm dust fallback
- Observatory Glow: yes, night/celestial-biased
- Rainy Window: yes, cool/weather-biased
- Campfire Blanket: yes, warm-biased
- Northern Whisper: yes, aurora-biased
- Storm Outside: yes, storm/cool-biased

## Behavior Rules

If palette enabled and mode supports palette:

- Use active palette tastefully.
- If palette is inappropriate, reduce influence or use a safe fallback.

Examples:

- Candle Hall should preserve warm candle character.
- Slow Prism can use palettes strongly.
- Garden Shade should avoid neon green.
- Snowfall Hush can become warm dust if palette is warm.
- Storm Outside should preserve stormy blue-gray character.

## Acceptance Criteria

- Palette-enabled modes look intentional.
- Palette disabled modes use active color/white defaults.
- Invalid/missing palette falls back safely.

---

# 15. Scene Compatibility Updates

## Goal

Phase 4C modes must work with scenes.

## Required Behavior

When user saves a scene while a Phase 4C mode is active:

- Scene stores the mode ID/name.
- Scene stores relevant mode-specific settings if exposed.
- Scene stores palette reference if palette is enabled.

When loading a scene with a Phase 4C mode:

- Mode restores.
- Palette state restores.
- Relevant settings restore.
- Transition behavior works.

## Optional Built-In Scenes

Add missing built-in/practical scenes only if safe and not overwriting user customizations:

- Candle Hall
- Slow Prism
- Garden Shade
- Snowfall Hush
- Observatory Glow
- Rainy Window
- Campfire Blanket
- Northern Whisper
- Storm Outside

Recommended:

- Add scene seed/reset helper only if existing scene system supports it.
- Do not overwrite user-edited scenes.

## Acceptance Criteria

- New modes can be saved and loaded through scenes.
- Existing scenes still work.
- User custom scenes are not overwritten.

---

# 16. Surprise Me Integration

## Goal

Make Surprise Me include Phase 4C modes safely.

## Mood Mapping

### Calm

Can choose:

- Garden Shade
- Snowfall Hush
- Observatory Glow
- Rainy Window
- Campfire Blanket
- Northern Whisper

### Evening

Can choose:

- Candle Hall
- Campfire Blanket
- Rainy Window
- Observatory Glow
- Storm Outside with softened flashes

### Sleep-safe

Can choose:

- Candle Hall at low brightness
- Snowfall Hush
- Observatory Glow
- Campfire Blanket
- Northern Whisper low brightness
- Rainy Window low brightness

### Focus

Can choose:

- Garden Shade if subtle/daytime
- Slow Prism only if low contrast and slow

### Colorful

Can choose:

- Slow Prism
- Northern Whisper
- Observatory Glow with Aurora/Moonlight palette

### Cozy

Can choose:

- Candle Hall
- Campfire Blanket
- Rainy Window warm variant
- Snowfall Hush warm dust variant

### Weather / Nature

Can choose:

- Garden Shade
- Snowfall Hush
- Rainy Window
- Storm Outside

## Safety Rules

Surprise Me must still:

- Never choose Strobe/Flash.
- Respect Night Guard.
- Respect power caps.
- Respect palette validity.
- Keep sleep-safe moods dim.
- Avoid Storm Outside flashes in sleep-safe mode unless suppressed.

## Acceptance Criteria

- Surprise Me can choose Phase 4C modes appropriately.
- Surprise Me remains safe and restrained.

---

# 17. Browser Preview Updates

## Goal

Add browser-side preview approximations for Phase 4C modes.

## Required Preview Renderers

Implement approximations for:

- Candle Hall
- Slow Prism
- Garden Shade
- Snowfall Hush
- Observatory Glow
- Rainy Window
- Campfire Blanket
- Northern Whisper
- Storm Outside

## Preview Principles

- Browser-side only.
- No server-side LED frame streaming.
- No high-frequency ESP8266 polling.
- Use existing `/api/state`, `/api/modes`, `/api/palettes` data.
- Preview approximates; it does not need pixel-perfect match.

## Acceptance Criteria

- Every Phase 4C mode has preview approximation or explicit fallback.
- Preview remains responsive.
- ESP8266 load does not increase significantly.

---

# 18. Night Guard Behavior

## Goal

Phase 4C modes must behave safely under Night Guard.

## Required Behavior

Night Guard should:

- Cap brightness for all Phase 4C modes.
- Keep Strobe/Flash blocking unchanged.
- Allow sleep-safe filtering to find appropriate new modes.
- Suppress or heavily soften Storm Outside flashes.
- Keep Candle Hall flicker smooth and low.
- Keep Slow Prism slow/low-contrast if sleep-safe or Night Guard is active.

## Metadata Note

Do not mark modes as flashing unless the implementation truly behaves like a flashing mode.

Storm Outside may contain rare soft flashes. If implemented correctly, it should not be treated like Strobe/Flash. But if the actual implementation uses sharp flashes, it must be tagged accordingly and Night Guard must block or suppress it.

## Acceptance Criteria

- New modes respect effective brightness.
- Night Guard filter/blocking logic still works.
- No new mode accidentally becomes harsh under Night Guard.

---

# 19. Scheduler, Timer, and Bedtime Behavior

## Goal

Phase 4C modes should work with schedules and timers.

## Required Behavior

- Scheduled scenes using Phase 4C modes load correctly.
- Timer/bedtime fade can target scenes using Phase 4C modes.
- Schedule-triggered Phase 4C scenes respect transitions, palettes, and Night Guard.
- Active timer/schedule conflict behavior remains unchanged.

Do not rewrite the scheduler.

## Acceptance Criteria

- Schedule can run a scene using a Phase 4C mode.
- Timer can load a scene using a Phase 4C mode.
- Bedtime fade remains stable.

---

# 20. Smooth Transition Behavior

## Goal

Phase 4C modes should transition cleanly.

## Required Behavior

- Mode changes into Phase 4C modes use Phase 3A transition behavior.
- Scene/favorite/schedule/surprise into Phase 4C modes transition.
- Off/Warm Dim still work.

If animated target frames are snapshot-based during transitions, document limitations.

## Acceptance Criteria

- New modes do not bypass transition system.
- Transitions do not double-apply brightness/gamma.

---

# 21. Existing Mode Cleanup

## Goal

Lightly normalize existing modes where needed.

## Required Checks

Review existing modes for:

- Metadata completeness
- Palette compatibility flags
- Preview support flags
- Scene compatibility
- Night Guard behavior
- Direct output bypasses
- Blocking delays
- Incorrect tags/categories

## Important Rule

Do not redesign existing modes unless fixing an obvious bug.

This phase is primarily final mode library batch 2.

## Acceptance Criteria

- Existing modes still work.
- Metadata/preview/palette cleanup is safe.
- No regressions.

---

# 22. UI Updates For Phase 4C Modes

## Goal

Make new modes accessible without UI clutter.

## Required UI Behavior

- New modes appear in mode dropdown/list.
- New modes appear in filters/categories.
- Description/best-used-for text appears.
- Palette support note appears where relevant.
- Preview works when selected.
- Mode-specific controls appear only if implemented.

## UI Constraint

Do not add nine giant control panels to the main page.

Use existing conditional controls pattern.

If controls are deferred, document defaults.

## Acceptance Criteria

- UI remains organized.
- New modes are discoverable.
- Main page remains usable on mobile.

---

# 23. API Updates

## Goal

Make Phase 4C modes work through existing APIs.

## Required Behavior

- `/api/modes` includes Phase 4C modes.
- `/api/mode?name=...` can select Phase 4C modes.
- Legacy `/set` mode control works if applicable.
- `/api/state` reflects new active mode.
- Scene APIs save/load new modes.
- Surprise API/browser generator can apply new modes.
- Schedule APIs can run scenes using new modes.

No new mode-specific API endpoints are required unless controls are exposed.

If controls are exposed:

- Reuse existing `/set` or mode settings route if present.
- Validate inputs.
- Save settings if persistent.

## Acceptance Criteria

- Phase 4C modes are API-selectable.
- Invalid mode names still return JSON errors.
- Existing APIs remain backward compatible.

---

# 24. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_4C_Report.md
```

The report should include:

1. Files changed.
2. New modes implemented.
3. Algorithm summary for each new mode.
4. Metadata/tags for each new mode.
5. Palette behavior for each new mode.
6. Scene compatibility behavior.
7. Browser preview behavior.
8. Surprise Me integration.
9. Scheduler/timer/bedtime compatibility.
10. Night Guard behavior.
11. Transition behavior.
12. Existing mode cleanup performed.
13. UI/API changes.
14. Resource report.
15. Compile/test status.
16. Known limitations.
17. What was intentionally deferred.
18. Recommended next step: Phase 4D final polish, advanced scene management, import/export, and documentation consolidation.

## Required Honesty

Do not claim:

- Advanced scene import/export exists unless implemented.
- Full favorite drag/reorder system exists unless implemented.
- Physical controls exist.
- NFC hardware exists.
- Microphone behavior exists.
- Light sensor behavior exists.
- Actual current measurement exists.
- ESP32 migration happened unless it actually happened.
- Physical LED testing happened unless it actually happened.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

If any of the nine planned modes were deferred, say exactly which and why.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Current Mode Architecture

Identify:

- Current mode enum/list/table/string handling
- Current render dispatch function
- Current mode metadata table
- Current scene save/load behavior
- Current palette helpers
- Current preview renderer dispatch
- Current Surprise Me generator
- Current scheduler scene execution behavior
- Current settings structure for mode-specific settings

Do not rewrite the whole sketch.

## Step 2 — Register Mode IDs and Metadata

Register all nine Phase 4C modes in mode list and metadata.

Compile or sanity-check if possible.

## Step 3 — Add Render Stubs

Add safe render stubs for each mode that render a simple active-color fill or safe fallback.

This ensures mode selection works before complex algorithms are added.

## Step 4 — Implement Modes In Risk Order

Recommended order:

1. Campfire Blanket
2. Candle Hall
3. Garden Shade
4. Slow Prism
5. Snowfall Hush
6. Rainy Window
7. Observatory Glow
8. Northern Whisper
9. Storm Outside

Reason:

- Start with warm/soft modes.
- Add gradient/color modes.
- Add particle/trail/star modes.
- Leave Storm Outside last because it has special Night Guard flash behavior.

## Step 5 — Add Palette Behavior

Add palette handling mode by mode.

Use safe fallbacks.

## Step 6 — Add Scene Compatibility

Ensure scene save/load handles new modes and any new settings.

## Step 7 — Add Preview Renderers

Add browser preview approximations.

Keep them lightweight and browser-side.

## Step 8 — Add Surprise Me Integration

Update candidate logic with safe mood mappings.

## Step 9 — Add Scheduler Compatibility Checks

Verify scheduled scenes with Phase 4C modes work.

Do not rewrite scheduler.

## Step 10 — Add/Update UI Controls

Expose mode-specific controls only if simple.

Otherwise document defaults.

## Step 11 — Existing Mode Cleanup

Review existing mode metadata/palette/preview/Night Guard integration.

Fix only safe issues.

## Step 12 — Compile and Resource Report

Compile for D1 mini / ESP8266.

Collect resource measurements if possible.

## Step 13 — Create Phase 4C Report

Document everything honestly.

---

# Test Checklist

## Compile

- Sketch compiles for D1 mini / ESP8266.

## Existing Routes Still Work

- `/` works.
- `/state` works.
- `/set` works.
- `/ota` works.
- `/update` works.
- `/diagnostics` works.
- `/schedule` works if implemented.
- Existing `/api/...` routes still work.
- Scene/favorite/timer/Night Guard/transition/palette/preview/surprise/schedule routes still work.
- `/api/modes` still works.

## New Mode Registration

- Candle Hall appears in mode list.
- Slow Prism appears in mode list.
- Garden Shade appears in mode list.
- Snowfall Hush appears in mode list.
- Observatory Glow appears in mode list.
- Rainy Window appears in mode list.
- Campfire Blanket appears in mode list.
- Northern Whisper appears in mode list.
- Storm Outside appears in mode list.
- All nine have metadata.
- All nine have descriptions.
- All nine have tags/categories.

## New Mode Rendering

- Candle Hall renders smooth candle clusters.
- Slow Prism renders slow tasteful color gradient.
- Garden Shade renders natural green-gold bands.
- Snowfall Hush renders soft drifting points.
- Observatory Glow renders celestial broad glow/stars.
- Rainy Window renders soft trails/droplets.
- Campfire Blanket renders restrained warm comfort glow.
- Northern Whisper renders soft aurora ribbons.
- Storm Outside renders calm storm ambience with rare soft flashes.
- No new mode uses blocking delays.
- No new mode bypasses output pipeline.

## Palette Behavior

- Palette-enabled modes use palettes safely.
- Palette disabled falls back to active color/white.
- Invalid/missing palette falls back safely.
- Palette support notes are accurate.

## Scene Behavior

- Scenes can save each new Phase 4C mode.
- Scenes can load each new Phase 4C mode.
- Existing scenes still load.
- Palette references work with new mode scenes.

## Preview Behavior

- Preview approximates each new mode.
- Preview remains browser-side.
- Unknown fallback still works.
- Preview performance remains acceptable.

## Surprise Me Behavior

- Surprise Me can choose new modes for appropriate moods.
- Sleep-safe surprise stays safe.
- Focus surprise can choose Garden Shade or low-contrast Slow Prism safely if implemented.
- Colorful surprise can choose Slow Prism/Northern Whisper safely.
- Weather/Nature surprise can choose Garden Shade/Snowfall/Rainy Window/Storm Outside safely.
- Strobe/Flash remain excluded.

## Scheduler / Timer Behavior

- Schedule can run scene using a Phase 4C mode.
- Timer can load scene using a Phase 4C mode.
- Bedtime fade remains stable.
- Schedule/transition/fade conflict behavior remains unchanged.

## Night Guard

- Night Guard caps brightness for all new modes.
- Storm Outside flashes are suppressed/softened under Night Guard.
- Slow Prism is subdued under Night Guard if needed.
- New modes are not falsely blocked as flashing unless implementation requires it.
- Sleep-safe filtering includes appropriate Phase 4C modes.

## Transitions

- Mode changes into Phase 4C modes transition.
- Scene/favorite/schedule/surprise into Phase 4C modes transition.
- Off/Warm Dim still work.

## UI

- Mode filtering includes new modes.
- Descriptions show.
- Best-used-for labels show.
- UI remains organized.
- Mobile layout remains usable.

## Regression

Verify existing basics still work:

- Color picker
- White temperature slider
- Master brightness slider
- Mode filtering
- Existing modes
- Phase 3D modes, if present
- Phase 4B modes, if present
- Scene save/load/rename/delete
- Favorite bar
- Warm Dim Now
- Off action
- Night Guard
- Timers
- Bedtime fade
- Smooth transitions
- Palette builder
- Browser preview
- Surprise Me
- Scheduler
- Scene/action links
- Diagnostics page
- Power estimator
- LED tests
- OTA routes

## Resource Report

- Sketch size reported or marked not measured.
- LittleFS usage reported or marked not measured.
- Free heap after boot reported or marked not measured.
- Free heap after key UI/API actions reported or marked not measured.
- Free heap while running new modes reported if measurable.
- OTA status reported.
- D1-mini-vs-ESP32 recommendation included.

---

# Phase 4C Non-Goals Reminder

Do not implement these yet:

- Advanced scene import/export
- Scene duplicate/export/import/reset polish unless reserved for Phase 4D
- Full favorite drag/reorder system
- Physical NFC reader
- Physical controls
- Microphone/music-reactive behavior
- Light sensor/room-aware brightness
- Cloud scheduling
- Public internet remote access
- ESP32-specific rewrite

If tempted, write it in the report as deferred.

This is the final lighting mode wave. It is not the paperwork apocalypse. Phase 4D gets the broom, clipboard, and final polish goblin.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 4C complete.

Files changed:
- ...

Implemented:
- ...

New modes:
- Candle Hall: ...
- Slow Prism: ...
- Garden Shade: ...
- Snowfall Hush: ...
- Observatory Glow: ...
- Rainy Window: ...
- Campfire Blanket: ...
- Northern Whisper: ...
- Storm Outside: ...

Mode metadata/filtering:
- ...

Palette behavior:
- ...

Scene compatibility:
- ...

Preview behavior:
- ...

Surprise Me integration:
- ...

Scheduler/timer compatibility:
- ...

Night Guard/transition interaction:
- ...

Existing mode cleanup:
- ...

Resource report:
- Board target used for compile:
- Sketch size:
- Sketch size percentage of available app slot, if known:
- LittleFS usage:
- Free heap after boot:
- Free heap after loading main page:
- Free heap after /api/state:
- Free heap after /api/modes:
- Free heap after /api/palettes:
- Free heap after /api/scenes:
- Free heap after preview UI loads:
- Free heap after /api/schedule:
- Free heap while running each new Phase 4C mode, if measurable:
- OTA still works: yes/no/not tested
- Web UI responsiveness: snappy/acceptable/laggy/not tested
- Animation responsiveness: snappy/acceptable/laggy/not tested
- Recommendation: stay on D1 mini / start worrying / consider ESP32-S3

Preserved:
- ...

Deferred:
- ...

Compile/test status:
- ...

Known limitations:
- ...

Recommended next step:
- Phase 4D — final polish, advanced scene management, import/export, and documentation consolidation.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim advanced scene import/export, physical controls, NFC hardware, microphone behavior, light sensor behavior, ESP32 migration, cloud remote access, or actual current measurement unless those actually exist.

---

# One-Sentence Mission Summary

Implement the final remaining lighting-mode batch — Candle Hall, Slow Prism, Garden Shade, Snowfall Hush, Observatory Glow, Rainy Window, Campfire Blanket, Northern Whisper, and Storm Outside — through the existing D1-mini-safe architecture with metadata, palettes, scenes, preview, Surprise Me, scheduler compatibility, Night Guard safety, transitions, and resource reporting.

