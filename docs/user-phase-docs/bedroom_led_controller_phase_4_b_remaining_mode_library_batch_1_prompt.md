# Bedroom LED Controller — Phase 4B Codex Prompt

## Phase 4B Title

**Remaining Mode Library — Batch 1**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 4B** of the larger upgrade plan.

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

The goal of Phase 4B is to implement the **first batch of the remaining large calm lighting mode library**.

This batch should add high-value, mostly calm, palette-aware modes from the future-mode backlog without implementing every remaining mode at once.

This phase is visual expansion, but still must be disciplined, testable, and D1-mini-safe.

Do **not** add the rest of the remaining modes yet. Do **not** rewrite the scheduler. Do **not** add hardware. Do **not** convert the project to ESP32-only.

This is the “add a beautiful new wing to the cozy lighting castle” phase, not the “throw the castle into a glitter tornado and hope the D1 mini survives” phase.

---

# Critical Board / Resource Context

The project remains **D1 mini / ESP8266 first** unless resource measurements prove it cannot continue safely.

The FireBeetle ESP32-S3 N4 has much better RAM/CPU headroom, but it still has 4 MB flash, so it is not a magic storage dragon. This phase should still target the D1 mini unless the resource report says otherwise.

## Required Resource Discipline

This phase adds more animation code, so Codex must pay attention to:

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

## Required Resource Report Addition

Every Phase 4B report must include:

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
- Free heap while running each new Phase 4B mode, if measurable:
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

Phase 4B may add new mode IDs/settings/metadata/preview renderers, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. New lighting mode registration and metadata
2. Batch 1 remaining lighting modes
3. Per-mode algorithms
4. Per-mode tags/categories/descriptions
5. Per-mode palette compatibility
6. Per-mode browser preview approximations
7. Per-mode scene/save/load compatibility
8. Surprise Me integration
9. Scheduler compatibility
10. Night Guard compatibility
11. Smooth transition compatibility
12. Existing mode metadata cleanup where safe
13. Documentation/report update
14. Compile/test/resource report

Do **not** implement yet:

- Phase 4C remaining mode batch
- Candle Hall
- Slow Prism
- Garden Shade
- Snowfall Hush
- Observatory Glow
- Rainy Window
- Campfire Blanket
- Northern Whisper
- Storm Outside
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

If a later feature seems tempting, document it as deferred. Eight mode goblins may enter. The other goblins must wait in the hallway.

---

# Phase 4B Main Goal

At the end of Phase 4B, the controller should include these new modes:

1. Window Sunrise
2. Ember Quilt
3. Porcelain Sky
4. Hearthline
5. Evening Current
6. Starlit Linen
7. Deep Sea Glass
8. Quiet Northern Lights

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

Add the Phase 4B modes using the existing architecture.

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
window-sunrise
ember-quilt
porcelain-sky
hearthline
evening-current
starlit-linen
deep-sea-glass
quiet-northern-lights
```

Display names:

```text
Window Sunrise
Ember Quilt
Porcelain Sky
Hearthline
Evening Current
Starlit Linen
Deep Sea Glass
Quiet Northern Lights
```

## Acceptance Criteria

- All eight modes are centrally registered.
- All eight modes are UI/API selectable.
- All eight modes appear in metadata/filtering.
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
```

Use integer or lookup-style math where reasonable.

Some sine usage is okay if existing code already uses it, but do not create extremely heavy per-pixel/per-layer floating-point storms.

## Deterministic Texture

For texture modes like Ember Quilt and Starlit Linen:

- Use deterministic pseudo-randomness based on pixel/slot index.
- Do not call true random every frame.
- Keep texture smooth over time.

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

### Window Sunrise

- Direction
- Duration / speed
- Final temperature or warmth
- Brightness cap, optional

### Ember Quilt

- Warmth
- Texture depth
- Drift speed
- Minimum brightness

### Porcelain Sky

- Tint strength
- Cloud speed
- Brightness behavior
- Palette behavior

### Hearthline

- Heat level
- Band width
- Drift speed
- Color mix

### Evening Current

- Current speed
- Depth
- Direction
- Brightness floor

### Starlit Linen

- Density
- Highlight warmth
- Fade length
- Maximum highlight brightness

### Deep Sea Glass

- Depth
- Shimmer amount
- Drift speed
- Palette

### Quiet Northern Lights

- Ribbon count
- Color spread
- Fold speed
- Brightness cap

## Recommended First Implementation

- Add safe defaults for all modes.
- Add only compact controls if the current settings structure has room.
- Store user-facing settings if exposed.
- Save exposed settings into scenes.
- Otherwise document that deeper controls are deferred.

## Acceptance Criteria

- Each mode works with defaults.
- Any exposed settings validate and persist.
- UI remains usable.

---

# 4. New Mode 1 — Window Sunrise

## Purpose

Simulates morning light spilling into the room from one side of the strip.

This differs from Dawn Bloom: Window Sunrise should feel more directional and architectural, like one side of the wall is closer to a window.

## Visual Behavior

- Starts as a dim amber glow on one side.
- Glow widens across the strip.
- Glow brightens gradually.
- Color slowly shifts from amber/warm to warmer-neutral daylight.
- Far side catches light later.
- Optional tiny shimmer in the bright region, extremely subtle.

## Suggested Algorithm

Use normalized pixel position 0–1.

Use a moving/widening boundary:

```text
sunReach = timePhase
brightness = smooth falloff based on distance from lit side and sunReach
```

Direction can be left-to-right or right-to-left.

Use long default period:

```text
2–10 minutes for full cycle preview/runtime option, or shorter for normal mode if needed
```

For a normal ambient mode, it can loop slowly rather than being a one-shot wake routine.

## Palette Behavior

- Prefer Peach Sky, Warm Linen, Candle.
- If active palette is warm/calm, use it.
- If active palette is cool/colorful, preserve sunrise warmth with only slight palette tint.

## Metadata

- Category: Calm / Morning / Everyday
- Tags: calm, warm, morning, animated, palette-compatible
- Sleep-safe: true at low brightness
- Focus-friendly: maybe
- Flashing: false
- Supports Palette: true

## Preview Approximation

Browser preview should show a directional warm glow widening across the strip.

## Acceptance Criteria

- Window Sunrise has clear directional spread.
- It remains smooth and non-blocking.
- It is not just Dawn Bloom renamed.
- Palette behavior is tasteful.

---

# 5. New Mode 2 — Ember Quilt

## Purpose

Creates the feeling of soft warmth under fabric, like glowing patches under a heavy blanket.

It should not look like fire.

## Visual Behavior

- Wide low-contrast warm patches.
- Patches slowly expand, contract, and drift.
- Colors range between amber, peach, candle white, and warm selected color.
- No harsh flicker.
- Very cozy, very low stress.

## Suggested Algorithm

Divide strip into overlapping quilt zones.

Each zone has:

- Center
- Width
- Warm tint
- Slow phase
- Brightness envelope

For each pixel:

- Sum soft contributions from nearby zones.
- Use smooth falloff.
- Keep contrast restrained.

Use deterministic zone phases.

## Palette Behavior

- Prefer Candle, Warm Linen, Peach Sky.
- If active palette is warm, sample zone colors from it.
- If active palette is cool/colorful, either use it gently or fall back to warm colors.

## Metadata

- Category: Sleep / Calm
- Tags: calm, warm, cozy, sleep-safe, animated, palette-compatible
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview should show warm soft patches slowly breathing.

## Acceptance Criteria

- Ember Quilt feels warm and soft, not fiery.
- Motion is slow.
- No harsh flicker.
- Works with Night Guard.

---

# 6. New Mode 3 — Porcelain Sky

## Purpose

A pale refined daytime gradient mode.

It should feel like soft clouds behind frosted glass: nearly static, polished, and intentionally subtle.

## Visual Behavior

- Pale cool white or selected color blended with white.
- Very slow cloud-like movement.
- Stable overall brightness.
- Subtle color-temperature gradient across strip.
- Low contrast.

## Suggested Algorithm

Use two or three broad sine/gradient layers:

- Base pale color.
- Very long slow gradient drift.
- Slight cool/warm tint shift.

Avoid obvious wave crests.

Default periods:

```text
60–240 seconds
```

## Palette Behavior

- Works with Moonlight, Peach Sky, Warm Linen, Garden if subtle.
- Reduce saturation strongly.
- If palette is vivid, blend with white heavily.

## Metadata

- Category: Everyday / Focus / Calm
- Tags: calm, daytime, low-motion, focus-friendly, palette-compatible
- Sleep-safe: true at low brightness
- Flashing: false
- Supports Palette: true, restrained

## Preview Approximation

Preview shows pale slow gradient drift.

## Acceptance Criteria

- Porcelain Sky is subtle and stable.
- It does not pulse strongly.
- It remains usable as daytime ambience.

---

# 7. New Mode 4 — Hearthline

## Purpose

Suggests far-away fireplace warmth without becoming a cheap flickering flame effect.

## Visual Behavior

- Low amber base.
- Slow warm bands rise/drift/dissolve along strip.
- Restrained brightness changes.
- Cozy movie-night feel.
- No rapid random flicker.

## Suggested Algorithm

Create slow heat bands:

- Band position moves along strip.
- Band width changes slowly.
- Band brightness rises/falls with smooth curve.
- Base remains warm and stable.

Use 2–4 broad bands.

Default periods:

```text
10–90 seconds
```

## Palette Behavior

- Prefer Candle or Warm Linen.
- If palette enabled and warm, use it for band colors.
- If palette not warm, blend minimally or fallback to amber.

## Metadata

- Category: Calm / Evening
- Tags: calm, warm, evening, movie, animated, palette-compatible
- Sleep-safe: true at low brightness
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview should show slow amber bands over warm base.

## Acceptance Criteria

- Hearthline is warm and cinematic.
- It is not a harsh flame/flicker mode.
- Palette behavior is safe.

---

# 8. New Mode 5 — Evening Current

## Purpose

A smoother, more sophisticated river-like current mode.

It should feel like a slow river of light beside the bed.

## Visual Behavior

- Long overlapping currents.
- No sharp crest.
- No full black valleys.
- Chosen color remains dominant.
- Tiny color-temperature shift near brightest points.
- Motion is calm and continuous.

## Suggested Algorithm

Use three layers:

1. Base brightness floor.
2. Broad moving swell.
3. Smaller counter-moving ripple.

Brightness should remain above a floor.

Default periods:

```text
20–180 seconds
```

## Palette Behavior

- Can use active palette gently.
- Sample palette along current position.
- Keep selected color/palette coherent.

## Metadata

- Category: Calm / Evening
- Tags: calm, evening, animated, palette-compatible
- Sleep-safe: true at low brightness
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows broad moving current with subtle counter ripple.

## Acceptance Criteria

- Evening Current feels smooth and river-like.
- No harsh crests or black valleys.
- Works with scenes and transitions.

---

# 9. New Mode 6 — Starlit Linen

## Purpose

Low-brightness soft highlights over a steady base.

It should avoid harsh twinkle lights. It should feel like tiny soft points under fabric.

## Visual Behavior

- Low steady base using active color or palette.
- Tiny soft highlights fade in slowly, stay dim, fade out slowly.
- Highlights affect neighboring pixels, not single harsh dots.
- Sparse/balanced/full density option if controls exist.
- Good night mode at low brightness.

## Suggested Algorithm

Use a fixed list of highlight slots.

Each slot has:

- Center
- Start offset
- Duration
- Warm/cool tint
- Width 2–5 pixels depending LED count

Use smooth envelope:

- Fade in
- Soft peak
- Fade out

Do not randomize every frame.

## Palette Behavior

- Base can use active palette or active color.
- Highlights can use warm/cool palette colors.
- Prefer Warm Linen, Moonlight, Candle.

## Metadata

- Category: Sleep / Calm
- Tags: calm, sleep-safe, night, animated, subtle, palette-compatible
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows low base with tiny soft fading highlights.

## Acceptance Criteria

- Highlights are soft, not harsh sparkles.
- Works at low brightness.
- Night Guard cap works.

---

# 10. New Mode 7 — Deep Sea Glass

## Purpose

Cool, quiet light through water/glass.

It should feel submerged and smooth.

## Visual Behavior

- Blue-green / cyan / deep blue palette.
- Broad dim swell.
- Faint caustic shimmer, blurred and subtle.
- Mostly low-to-medium brightness.
- Rare soft peaks only.

## Suggested Algorithm

Use layers:

1. Deep base color.
2. Broad slow swell.
3. Faint caustic shimmer layer.
4. Optional palette gradient.

Shimmer amplitude should be small.

Default periods:

```text
Broad swell: 30–120 seconds
Shimmer: 10–30 seconds, subtle
```

## Palette Behavior

- Prefer Ocean, Moonlight, Storm Blue, Aurora.
- If active palette is warm, reduce influence or fallback to Ocean-like colors.

## Metadata

- Category: Weather / Nature / Calm
- Tags: calm, cool, water, night, animated, palette-compatible
- Sleep-safe: true at low brightness
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows deep cyan/blue slow waves and faint shimmer.

## Acceptance Criteria

- Deep Sea Glass is smooth and cool.
- Shimmer is subtle.
- It does not become glittery or chaotic.

---

# 11. New Mode 8 — Quiet Northern Lights

## Purpose

A calmer cousin of Velvet Aurora.

It should use soft ribbons that sometimes pause, reverse, or dissolve, instead of simply sliding forever.

## Visual Behavior

- Soft vertical/ribbon-like bands represented along 1D strip.
- Active color remains an anchor.
- Secondary colors are derived from palette or nearby hues.
- Ribbons move slowly.
- Some ribbons pause/reverse/dissolve.
- Avoid full rainbow chaos.

## Suggested Algorithm

Use 2–5 ribbon layers.

Each ribbon has:

- Center position
- Width
- Direction
- Speed
- Color
- Life/fade envelope
- Pause/reverse behavior from deterministic phase

For each pixel:

- Sum soft ribbon contributions.
- Blend with low base.

## Palette Behavior

- Prefer Aurora, Moonlight, Ocean, Soft Neon if restrained.
- Use palette colors as ribbon colors.
- If palette is very warm, blend with active color and keep saturation controlled.

## Metadata

- Category: Calm / Colorful
- Tags: calm, colorful, aurora, animated, palette-compatible
- Sleep-safe: true at low brightness
- Flashing: false
- Supports Palette: true

## Preview Approximation

Preview shows soft colored ribbons drifting/folding slowly.

## Acceptance Criteria

- Quiet Northern Lights is colorful but calm.
- It does not become Rainbow 2.0.
- Palette behavior works.
- Night Guard only caps brightness.

---

# 12. Mode Metadata and Filtering Updates

## Goal

Every new Phase 4B mode must appear correctly in mode metadata and filtering UI.

## Required Metadata Fields

For each new mode include:

- ID
- Display name
- Description
- Best used for
- Category
- Tags
- Animated true
- Calm true
- Sleep-safe true/false
- Focus-friendly true/false
- Colorful true/false
- Flashing false
- Utility false unless applicable
- Blocked by Night Guard false
- Supports palette true/limited
- Preview supported true/false, if such field exists

## Suggested Category Mapping

```text
Window Sunrise: Calm / Morning / Everyday
Ember Quilt: Sleep / Calm
Porcelain Sky: Everyday / Focus / Calm
Hearthline: Calm / Evening
Evening Current: Calm / Evening
Starlit Linen: Sleep / Calm
Deep Sea Glass: Weather / Nature / Calm
Quiet Northern Lights: Calm / Colorful
```

## Acceptance Criteria

- `/api/modes` includes all Phase 4B modes.
- UI filters show the modes in sensible categories.
- Surprise Me can use metadata safely.

---

# 13. Palette Compatibility Updates

## Goal

Make Phase 4B modes use the palette system appropriately.

## Palette-Compatible Modes

Recommended palette support:

- Window Sunrise: yes, warm-biased
- Ember Quilt: yes, warm-biased
- Porcelain Sky: yes, desaturated/restrained
- Hearthline: yes, warm-biased
- Evening Current: yes
- Starlit Linen: yes
- Deep Sea Glass: yes, cool-biased
- Quiet Northern Lights: yes

## Behavior Rules

If palette enabled and mode supports palette:

- Use active palette tastefully.
- If palette is inappropriate, reduce influence or use a safe fallback.

Examples:

- Window Sunrise should preserve warm sunrise character.
- Ember Quilt should stay warm/cozy.
- Porcelain Sky should heavily desaturate vivid palettes.
- Deep Sea Glass should prefer cool palettes.
- Quiet Northern Lights can use Aurora-style palettes.

## Acceptance Criteria

- Palette-enabled modes look intentional.
- Palette disabled modes use active color/white defaults.
- Invalid/missing palette falls back safely.

---

# 14. Scene Compatibility Updates

## Goal

Phase 4B modes must work with scenes.

## Required Behavior

When user saves a scene while a Phase 4B mode is active:

- Scene stores the mode ID/name.
- Scene stores relevant mode-specific settings if exposed.
- Scene stores palette reference if palette is enabled.

When loading a scene with a Phase 4B mode:

- Mode restores.
- Palette state restores.
- Relevant settings restore.
- Transition behavior works.

## Optional Built-In Scenes

Add missing built-in/practical scenes only if safe and not overwriting user customizations:

- Window Sunrise
- Ember Quilt
- Porcelain Sky
- Hearthline
- Evening Current
- Starlit Linen
- Deep Sea Glass
- Quiet Northern Lights

Recommended:

- Add scene seed/reset helper only if existing scene system supports it.
- Do not overwrite user-edited scenes.

## Acceptance Criteria

- New modes can be saved and loaded through scenes.
- Existing scenes still work.
- User custom scenes are not overwritten.

---

# 15. Surprise Me Integration

## Goal

Make Surprise Me include Phase 4B modes safely.

## Mood Mapping

### Calm

Can choose:

- Porcelain Sky
- Evening Current
- Deep Sea Glass
- Ember Quilt
- Window Sunrise

### Evening

Can choose:

- Hearthline
- Evening Current
- Ember Quilt
- Window Sunrise
- Quiet Northern Lights at low/moderate brightness

### Sleep-safe

Can choose:

- Ember Quilt
- Starlit Linen
- Evening Current at low brightness
- Deep Sea Glass at low brightness

### Focus

Can choose:

- Porcelain Sky
- Window Sunrise if bright/neutral enough

### Colorful

Can choose:

- Quiet Northern Lights
- Deep Sea Glass
- Existing colorful non-flashing modes

### Cozy

Can choose:

- Ember Quilt
- Hearthline
- Window Sunrise
- Evening Current warm palette

## Safety Rules

Surprise Me must still:

- Never choose Strobe/Flash.
- Respect Night Guard.
- Respect power caps.
- Respect palette validity.
- Keep sleep-safe moods dim.

## Acceptance Criteria

- Surprise Me can choose Phase 4B modes appropriately.
- Surprise Me remains safe and restrained.

---

# 16. Browser Preview Updates

## Goal

Add browser-side preview approximations for Phase 4B modes.

## Required Preview Renderers

Implement approximations for:

- Window Sunrise
- Ember Quilt
- Porcelain Sky
- Hearthline
- Evening Current
- Starlit Linen
- Deep Sea Glass
- Quiet Northern Lights

## Preview Principles

- Browser-side only.
- No server-side LED frame streaming.
- No high-frequency ESP8266 polling.
- Use existing `/api/state`, `/api/modes`, `/api/palettes` data.
- Preview approximates; it does not need pixel-perfect match.

## Acceptance Criteria

- Every Phase 4B mode has preview approximation or explicit fallback.
- Preview remains responsive.
- ESP8266 load does not increase significantly.

---

# 17. Night Guard Behavior

## Goal

Phase 4B modes must behave safely under Night Guard.

## Required Behavior

Night Guard should:

- Cap brightness for all Phase 4B modes.
- Not block any Phase 4B mode by default.
- Keep Strobe/Flash blocking unchanged.
- Allow sleep-safe filtering to find appropriate new modes.

## Mode-Specific Considerations

- Quiet Northern Lights is colorful but not flashing; do not tag as flashing.
- Starlit Linen should remain gentle and sleep-safe.
- Hearthline should not flicker harshly.
- Deep Sea Glass should not shimmer aggressively.

## Acceptance Criteria

- New modes respect effective brightness.
- Night Guard filter/blocking logic still works.
- No new mode is accidentally tagged flashing.

---

# 18. Scheduler, Timer, and Bedtime Behavior

## Goal

Phase 4B modes should work with schedules and timers.

## Required Behavior

- Scheduled scenes using Phase 4B modes load correctly.
- Timer/bedtime fade can target scenes using Phase 4B modes.
- Schedule-triggered Phase 4B scenes respect transitions, palettes, and Night Guard.
- Active timer/schedule conflict behavior remains unchanged.

Do not rewrite the scheduler.

## Acceptance Criteria

- Schedule can run a scene using a Phase 4B mode.
- Timer can load a scene using a Phase 4B mode.
- Bedtime fade remains stable.

---

# 19. Smooth Transition Behavior

## Goal

Phase 4B modes should transition cleanly.

## Required Behavior

- Mode changes into Phase 4B modes use Phase 3A transition behavior.
- Scene/favorite/schedule/surprise into Phase 4B modes transition.
- Off/Warm Dim still work.

If animated target frames are snapshot-based during transitions, document limitations.

## Acceptance Criteria

- New modes do not bypass transition system.
- Transitions do not double-apply brightness/gamma.

---

# 20. Existing Mode Cleanup

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

This phase is primarily new mode batch 1.

## Acceptance Criteria

- Existing modes still work.
- Metadata/preview/palette cleanup is safe.
- No regressions.

---

# 21. UI Updates For Phase 4B Modes

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

Do not add eight giant control panels to the main page.

Use existing conditional controls pattern.

If controls are deferred, document defaults.

## Acceptance Criteria

- UI remains organized.
- New modes are discoverable.
- Main page remains usable on mobile.

---

# 22. API Updates

## Goal

Make Phase 4B modes work through existing APIs.

## Required Behavior

- `/api/modes` includes Phase 4B modes.
- `/api/mode?name=...` can select Phase 4B modes.
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

- Phase 4B modes are API-selectable.
- Invalid mode names still return JSON errors.
- Existing APIs remain backward compatible.

---

# 23. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_4B_Report.md
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
18. Recommended next step: Phase 4C remaining mode library batch 2.

## Required Honesty

Do not claim:

- Phase 4C modes are implemented.
- Every remaining lighting mode is done.
- Advanced scene import/export exists.
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

If any of the eight planned modes were deferred, say exactly which and why.

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

Register all eight Phase 4B modes in mode list and metadata.

Compile or sanity-check if possible.

## Step 3 — Add Render Stubs

Add safe render stubs for each mode that render a simple active-color fill or safe fallback.

This ensures mode selection works before complex algorithms are added.

## Step 4 — Implement Modes In Risk Order

Recommended order:

1. Porcelain Sky
2. Evening Current
3. Window Sunrise
4. Ember Quilt
5. Hearthline
6. Starlit Linen
7. Deep Sea Glass
8. Quiet Northern Lights

Reason:

- Start with broad-gradient/simple calm modes.
- Then add texture/highlight modes.
- Leave the more complex ribbon mode for last.

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

Verify scheduled scenes with Phase 4B modes work.

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

## Step 13 — Create Phase 4B Report

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

- Window Sunrise appears in mode list.
- Ember Quilt appears in mode list.
- Porcelain Sky appears in mode list.
- Hearthline appears in mode list.
- Evening Current appears in mode list.
- Starlit Linen appears in mode list.
- Deep Sea Glass appears in mode list.
- Quiet Northern Lights appears in mode list.
- All eight have metadata.
- All eight have descriptions.
- All eight have tags/categories.

## New Mode Rendering

- Window Sunrise renders directional warm spread.
- Ember Quilt renders soft warm patches.
- Porcelain Sky renders pale refined gradient.
- Hearthline renders restrained warm bands.
- Evening Current renders smooth current motion.
- Starlit Linen renders soft low-brightness highlights.
- Deep Sea Glass renders cool water/glass motion.
- Quiet Northern Lights renders calm ribbons.
- No new mode uses blocking delays.
- No new mode bypasses output pipeline.

## Palette Behavior

- Palette-enabled modes use palettes safely.
- Palette disabled falls back to active color/white.
- Invalid/missing palette falls back safely.
- Palette support notes are accurate.

## Scene Behavior

- Scenes can save each new Phase 4B mode.
- Scenes can load each new Phase 4B mode.
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
- Focus surprise can choose Porcelain Sky or Window Sunrise safely.
- Colorful surprise can choose Quiet Northern Lights safely.
- Strobe/Flash remain excluded.

## Scheduler / Timer Behavior

- Schedule can run scene using a Phase 4B mode.
- Timer can load scene using a Phase 4B mode.
- Bedtime fade remains stable.
- Schedule/transition/fade conflict behavior remains unchanged.

## Night Guard

- Night Guard caps brightness for all new modes.
- New modes are not falsely blocked as flashing.
- Sleep-safe filtering includes appropriate Phase 4B modes.

## Transitions

- Mode changes into Phase 4B modes transition.
- Scene/favorite/schedule/surprise into Phase 4B modes transition.
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

# Phase 4B Non-Goals Reminder

Do not implement these yet:

- Candle Hall
- Slow Prism
- Garden Shade
- Snowfall Hush
- Observatory Glow
- Rainy Window
- Campfire Blanket
- Northern Whisper
- Storm Outside
- Advanced scene import/export
- Full favorite drag/reorder system
- Physical NFC reader
- Physical controls
- Microphone/music-reactive behavior
- Light sensor/room-aware brightness
- ESP32-specific rewrite

If tempted, write it in the report as deferred.

Eight modes. Not seventeen. The RGB raccoon may count, but it may not negotiate.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 4B complete.

Files changed:
- ...

Implemented:
- ...

New modes:
- Window Sunrise: ...
- Ember Quilt: ...
- Porcelain Sky: ...
- Hearthline: ...
- Evening Current: ...
- Starlit Linen: ...
- Deep Sea Glass: ...
- Quiet Northern Lights: ...

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
- Free heap while running each new Phase 4B mode, if measurable:
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
- Phase 4C — remaining mode library batch 2.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim Phase 4C modes, advanced scene import/export, physical controls, NFC hardware, microphone behavior, light sensor behavior, ESP32 migration, or actual current measurement unless those actually exist.

---

# One-Sentence Mission Summary

Implement the first remaining-mode-library batch — Window Sunrise, Ember Quilt, Porcelain Sky, Hearthline, Evening Current, Starlit Linen, Deep Sea Glass, and Quiet Northern Lights — through the existing D1-mini-safe architecture with metadata, palettes, scenes, preview, Surprise Me, scheduler compatibility, Night Guard safety, transitions, and resource reporting.

