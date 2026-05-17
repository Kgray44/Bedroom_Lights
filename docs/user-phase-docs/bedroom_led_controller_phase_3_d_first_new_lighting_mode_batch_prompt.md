# Bedroom LED Controller — Phase 3D Codex Prompt

## Phase 3D Title

**First New Lighting Mode Batch + Existing Mode Cleanup**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 3D** of the larger upgrade plan.

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

Phase 3A should already have added or started:

- Smooth transition settings
- Transition runtime state
- Fade transition behavior
- Optional wipe/dissolve transitions
- Transition integration with mode, scene, favorite, Warm Dim, and Off actions
- Transition API/UI controls

Phase 3B should already have added or started:

- Palette data model
- Palette storage, preferably `/palettes.json`
- Built-in palettes
- Active palette selection
- Palette UI/API
- Palette-aware mode metadata
- Limited palette-aware mode integration
- Scene palette references

Phase 3C should already have added or started:

- Browser-side live preview strip
- Preview renderers for existing modes
- Preview palette awareness
- Surprise Me generator
- Surprise moods and safety rules
- Resource reporting discipline for D1 mini viability

The goal of Phase 3D is to implement the **first modest batch of new lighting modes** while preserving everything already built.

This is the first phase where new visual behaviors are allowed. However, the implementation must still be careful, incremental, and resource-aware.

Do not implement all future lighting ideas yet.

Do not turn the D1 mini into a tiny flaming RGB lasagna.

---

# Critical Board / Resource Context

The project remains **D1 mini / ESP8266 first** unless resource measurements prove it cannot continue safely.

The FireBeetle ESP32-S3 N4 is technically stronger for RAM/CPU headroom, but it still has 4 MB flash, so it is not a magic storage dragon. The D1 mini can likely continue if code remains compact, JSON stays small, UI assets are lightweight, and dynamic allocation is controlled.

## Required Resource Discipline

This phase adds new modes, so Codex must pay special attention to:

- Sketch size
- Free heap
- OTA margin
- Animation timing responsiveness
- Web UI responsiveness
- JSON/API size
- Avoiding dynamic allocation in animation loops
- Avoiding large per-mode buffers
- Avoiding repeated expensive `String` operations
- Avoiding blocking `delay()` calls
- Avoiding file I/O during animation rendering

## Required Resource Report Addition

Every Phase 3D report must include:

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
- Free heap while running each new mode, if measurable:
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
- Existing animation modes
- Existing calm modes
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

Existing routes that must remain functional:

- `/`
- `/state`
- `/set`
- `/ota`
- `/update`
- `/diagnostics`
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
- Any Phase 3C preview/surprise endpoints, if implemented

Phase 3D may add new mode IDs/settings/metadata, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. New lighting mode architecture review
2. First new lighting mode batch
3. Per-mode metadata, tags, and descriptions
4. Per-mode controls/settings, only where useful
5. Palette compatibility for new modes where appropriate
6. Scene compatibility
7. Favorite/Surprise Me compatibility
8. Browser preview approximations for new modes
9. Night Guard behavior for new modes
10. Timer/transition behavior for new modes
11. Existing mode cleanup and metadata normalization
12. Documentation/report update
13. Compile/test/resource report

Do **not** implement yet:

- Remaining Phase 4 lighting mode library
- Full weekly scheduler
- QR/NFC scene links
- Advanced scene import/export
- Physical controls
- Microphone/music-reactive behavior
- Light sensor/room-aware brightness
- Hardware-specific ESP32 rewrite
- ESP32-only features
- Server-side LED preview streaming
- WebSocket LED frame streaming

If a later feature seems tempting, document it as deferred. The Phase 4 Mode Hydra is not invited to this party yet.

---

# Phase 3D Main Goal

At the end of Phase 3D, the controller should include a modest, polished first batch of new modes:

1. Satin Breathing
2. Paper Lantern Row
3. Library Lamp
4. Solar Noon
5. Golden Hour
6. Dream Aquarium
7. Circuit Glow

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
- Avoid blocking logic.
- Avoid heavy dynamic allocation.
- Keep the D1 mini viable.

This phase is not about adding all possible modes. It is about proving the new architecture can support additional high-quality modes cleanly.

---

# 1. New Mode Architecture Rules

## Goal

Add new modes in a way that fits the existing architecture instead of creating seven tiny side quests of chaos.

## Required Rules

Each new mode must:

- Have a stable internal mode ID.
- Have a user-facing display name.
- Have mode metadata.
- Have a short description.
- Have “best used for” text.
- Render into the central frame buffer.
- Avoid direct `strip.show()` calls inside the mode.
- Avoid direct brightness scaling if the output pipeline handles brightness.
- Avoid blocking `delay()`.
- Use `millis()` or deterministic time calculations.
- Handle low LED counts gracefully.
- Handle missing/disabled palette safely.
- Respect Night Guard mode-blocking and brightness cap.
- Be saveable/restorable in scenes.
- Be selectable through `/api/mode` and legacy `/set` if applicable.
- Appear in `/api/modes` metadata.
- Appear in the mode filtering UI.
- Have a browser preview approximation or explicit fallback.

## Required Mode Registration

If the project has a centralized mode list/table, add the new modes there.

If the project still uses string comparisons, add carefully and consider lightweight helper functions.

Do not duplicate slightly different mode names across UI/API/render logic.

Examples of stable mode IDs:

```text
satin-breathing
paper-lantern-row
library-lamp
solar-noon
golden-hour
dream-aquarium
circuit-glow
```

Display names:

```text
Satin Breathing
Paper Lantern Row
Library Lamp
Solar Noon
Golden Hour
Dream Aquarium
Circuit Glow
```

## Acceptance Criteria

- New modes are centrally registered.
- New modes are selectable from UI/API.
- New modes appear in metadata/filtering.
- No duplicate naming mess is introduced.

---

# 2. Shared Helper Functions For New Modes

## Goal

Avoid each new mode inventing its own math cave.

## Required / Recommended Helpers

Reuse existing helpers where possible.

Add lightweight helpers if missing:

```cpp
uint8_t clamp8(int value);
uint8_t lerp8(uint8_t a, uint8_t b, uint8_t amount);
RgbPixel blendRgb(RgbPixel a, RgbPixel b, uint8_t amount);
RgbPixel scaleRgb(RgbPixel color, uint8_t scale);
RgbPixel warmWhiteApprox(uint16_t kelvin);
RgbPixel sampleActivePaletteOrColor(uint16_t position, uint16_t maxPosition);
float phaseFromMillis(unsigned long now, uint32_t periodMs);
uint8_t sineEase8(float phase);
uint8_t triangleWave8(float phase);
```

Use integer math where practical.

Some existing wave modes may already use floating-point sine. Preserve if acceptable, but do not make every pixel perform heavy floating-point work unnecessarily.

## Deterministic Pseudo-Randomness

Some modes need soft variation.

Use deterministic pseudo-random helpers based on pixel index, seed, or slot index rather than calling expensive/random logic every frame.

Example concept:

```cpp
uint8_t hash8(uint16_t x);
```

Avoid making the animation visibly jump because random values change every frame.

## Acceptance Criteria

- New modes share helpers where practical.
- No repeated heavy allocation/math is introduced unnecessarily.
- Animations are deterministic/smooth rather than jittery.

---

# 3. Mode-Specific Settings Strategy

## Goal

Give modes useful controls without turning the UI into a submarine dashboard.

## Required Behavior

Each new mode should work with safe defaults even if no per-mode controls are exposed yet.

Expose mode-specific controls only where they are useful and compact.

Recommended settings:

### Satin Breathing

- Breath length
- Hold time
- Texture amount
- Night cap, optional or use Night Guard

### Paper Lantern Row

- Lantern count
- Paper warmth / palette choice behavior
- Sway amount
- Brightness style, if useful

### Library Lamp

- Warmth
- Texture amount
- Brightness handled by master brightness

### Solar Noon

- Temperature
- Gradient amount
- Brightness handled by master brightness

### Golden Hour

- Direction
- Warmth
- Movement speed

### Dream Aquarium

- Depth
- Shimmer amount
- Drift speed
- Palette behavior

### Circuit Glow

- Signal density
- Trace speed
- Pulse color / palette behavior
- Fade length

## Recommended First Implementation

To keep Phase 3D manageable:

- Add safe defaults for all 7 modes.
- Expose only a small number of shared/simple controls if the existing UI pattern makes it easy.
- Store any new mode settings in the central settings model if they affect scenes.
- Do not build a massive dynamic control panel.

## Scene Storage

If mode-specific settings are exposed and user-adjustable:

- Include them in scene save/load.

If not exposed yet:

- Defaults are fine; document that deeper controls are deferred.

## Acceptance Criteria

- Every new mode works with default settings.
- Any exposed settings persist and save into scenes.
- UI remains usable.

---

# 4. New Mode 1 — Satin Breathing

## Purpose

A refined whole-strip breathing mode for bedtime, calm evenings, and low-stimulation ambience.

This is not a harsh pulse. It should feel like soft fabric breathing slowly.

## Visual Behavior

- Whole strip brightens and dims together.
- Breathing curve should feel organic:
  - Slow inhale
  - Gentle hold
  - Longer exhale
- Add subtle per-pixel texture so it does not look perfectly flat.
- Optional tiny warmth shift near the top of the breath.
- Should remain calm at low brightness.

## Suggested Algorithm

Use a looping phase:

```text
phase = elapsed / breathPeriod
```

Map phase into:

- inhale region
- hold region
- exhale region

Use easing, not abrupt linear changes.

Suggested brightness envelope:

```text
minimum envelope: 35–60 out of 255 internal scale
maximum envelope: 180–255 internal scale before master brightness
```

But final visible brightness must still pass through master brightness and Night Guard.

Texture:

- Use deterministic per-pixel variation around ±3–8%.
- Keep it slow and subtle.

## Palette Behavior

If palette is enabled and active mode supports palette:

- Use a warm/calm palette softly across the strip.
- Otherwise use active color or white temperature.

## Metadata

Suggested metadata:

- Category: Sleep or Calm
- Tags: calm, sleep-safe, animated, palette-compatible
- Flashing: false
- Blocked by Night Guard: false
- Supports Palette: true

## Preview Approximation

Browser preview should show whole-strip slow breathing with subtle texture.

## Acceptance Criteria

- Satin Breathing renders smoothly.
- No abrupt blinking.
- Night Guard cap works.
- Palette enabled/disabled behavior works.
- Scene save/load works.

---

# 5. New Mode 2 — Paper Lantern Row

## Purpose

A warm decorative mode that makes the strip feel like a row of soft paper lanterns behind translucent material.

## Visual Behavior

- Create several large glowing blobs with very soft edges.
- Lantern centers should be distributed along the strip.
- Each lantern slowly changes brightness independently.
- Centers may drift slightly by one or two pixels over time.
- Warm, creamy, cozy color bias.
- No harsh flicker.

## Suggested Algorithm

Use a fixed number of lantern centers.

For each pixel:

- Calculate distance to each lantern center.
- Add contribution based on soft falloff.
- Blend overlapping lanterns.

Suggested falloff:

```text
contribution = smooth falloff based on distance / radius
```

Use deterministic lantern phases for brightness variation:

```text
lanternBrightness = base + slow sine/triangle variation
```

Keep lantern count reasonable:

```text
3–8 default, depending on LED count
```

## Palette Behavior

If palette enabled:

- Prefer warm palette colors if active palette is warm/calm.
- If palette is cool/colorful, still keep brightness soft.
- Sample palette by lantern index rather than every pixel if easier.

Default palette suggestion:

- Candle or Warm Linen

## Metadata

- Category: Calm / Everyday
- Tags: calm, warm, evening, sleep-safe, animated, palette-compatible
- Flashing: false
- Blocked by Night Guard: false
- Supports Palette: true

## Preview Approximation

Browser preview should show warm soft blobs with slow brightness breathing.

## Acceptance Criteria

- Lantern blobs are soft and wide.
- Movement is subtle.
- Looks cozy, not flickery.
- Works with low LED count.
- Palette disabled uses active warm color.

---

# 6. New Mode 3 — Library Lamp

## Purpose

A practical reading/homework mode with stable warm light and a tiny amount of texture.

This mode should be useful, not flashy.

## Visual Behavior

- Mostly steady warm/neutral light.
- Slight subtle per-pixel texture or extremely slow movement.
- No obvious animation unless staring at it.
- Good for reading, homework, or calm desk work.

## Suggested Algorithm

Base color:

- Use white temperature around 3000K–4200K by default, or active white/color.

Texture:

- Deterministic low-amplitude variation per pixel.
- Very slow drift, maybe 60–180 second cycle.

Avoid:

- Sparkles
- Pulses
- Strong movement
- Color cycling

## Palette Behavior

Palette support optional.

Recommended:

- If palette enabled and palette is warm/everyday/calm, gently blend palette into the base.
- If palette is colorful, reduce saturation or ignore palette.

## Metadata

- Category: Focus / Everyday
- Tags: focus-friendly, calm, everyday, low-motion
- Flashing: false
- Sleep-safe: true at low brightness
- Supports Palette: optional / true with restraint

## Preview Approximation

Preview can show a mostly steady warm strip with faint texture.

## Acceptance Criteria

- Library Lamp is stable and useful.
- It does not distract.
- It works with scenes/favorites.
- Night Guard only caps brightness, not block it.

---

# 7. New Mode 4 — Solar Noon

## Purpose

A practical daytime productivity mode.

This should feel like bright neutral daylight, controlled and stable, without becoming a sterile hospital hallway or interrogation lamp.

## Visual Behavior

- Neutral/cool white base, around 4500K–6000K.
- Stable brightness.
- Optional very slow refined gradient drift.
- Minimal animation.
- Useful for focus, cleaning, electronics work, or daytime ambience.

## Suggested Algorithm

Base:

- White temperature around 5000K by default.

Optional gradient:

- Slightly warmer one side, slightly cooler the other.
- Drift over a long period, such as 120–300 seconds.

Keep contrast low.

## Palette Behavior

Palette support optional.

Recommended:

- Usually ignore colorful palettes.
- If palette is neutral/calm, use very subtle tint.

## Metadata

- Category: Focus / Everyday
- Tags: focus-friendly, bright, daytime, low-motion
- Flashing: false
- Sleep-safe: false by default because it can be bright/cool
- Supports Palette: optional / limited

## Preview Approximation

Preview should show a stable neutral/cool strip with subtle gradient.

## Acceptance Criteria

- Solar Noon feels bright and practical.
- It respects master brightness and power caps.
- It is not animated aggressively.
- Scene save/load works.

---

# 8. New Mode 5 — Golden Hour

## Purpose

A warm sunlight mode that feels like light spilling across the wall from one side.

## Visual Behavior

- Warm amber/gold gradient.
- One side brighter/warmer, fading across the strip.
- Very slow movement or shift.
- Gentle, comforting, good for morning/evening.

## Suggested Algorithm

Use strip position normalized 0–1.

Create gradient:

- Bright warm gold near source side.
- Softer cream/amber farther away.

Motion:

- Source glow slowly shifts or breathes over a long cycle.
- Optional direction setting left/right.

Default cycle:

```text
60–180 seconds
```

## Palette Behavior

If palette enabled:

- Use warm palettes such as Peach Sky, Candle, Warm Linen.
- If active palette is cool, blend gently but preserve golden feel.

## Metadata

- Category: Calm / Everyday
- Tags: calm, warm, morning, evening, animated, palette-compatible
- Flashing: false
- Sleep-safe: true at low brightness
- Supports Palette: true

## Preview Approximation

Preview should show warm one-sided gradient with slow shift.

## Acceptance Criteria

- Golden Hour has a clear directional warmth feel.
- It remains smooth and calm.
- Direction/slow movement does not block.
- Palette support works if implemented.

---

# 9. New Mode 6 — Dream Aquarium

## Purpose

A cool watery calm mode with deep blue/teal motion.

It should feel like light moving through water or glass, not a chaotic ocean rave.

## Visual Behavior

- Deep blue/teal/cyan palette.
- Slow broad caustic-like waves.
- Low shimmer.
- Mostly low-to-medium brightness.
- Soft underwater motion.

## Suggested Algorithm

Combine layers:

1. Base deep cool color.
2. Broad slow sine swell.
3. Faint faster shimmer layer with low amplitude.
4. Optional palette sample across strip.

Keep shimmer subtle.

Default periods:

- Broad drift: 30–120 seconds
- Shimmer: 8–20 seconds, low amplitude

## Palette Behavior

If palette enabled:

- Prefer Ocean, Moonlight, Storm Blue, Aurora.
- If palette is warm, either use it very gently or fall back to Ocean-like colors.

Recommended default if no palette:

- Deep teal/cyan active palette internally.

## Metadata

- Category: Weather / Nature or Calm
- Tags: calm, cool, water, animated, palette-compatible
- Flashing: false
- Sleep-safe: true at low brightness
- Supports Palette: true

## Preview Approximation

Preview should show cool watery waves and subtle shimmer.

## Acceptance Criteria

- Dream Aquarium is calm and watery.
- Shimmer is not harsh.
- Palette behavior is safe.
- Night Guard caps brightness.

---

# 10. New Mode 7 — Circuit Glow

## Purpose

A subtle electronics-inspired mode where soft pulses travel like signals through traces.

This is a KG-special engineering goblin mode, but it should still be calm and bedroom-friendly.

## Visual Behavior

- Low base glow.
- Soft pulses travel along strip.
- Pulses have smooth leading/trailing fades.
- Not fast or harsh.
- Can use green/cyan/blue or palette colors.
- Should feel like living PCB traces, not arcade lightning.

## Suggested Algorithm

Use a small number of moving pulses.

For each pulse:

- Position moves along strip based on time.
- Width controls soft trail.
- Brightness fades with distance.
- Pulse color can come from Circuit Glow palette or active palette.

Suggested defaults:

- Signal count: 2–5
- Speed: slow/moderate
- Base glow: low
- Trail length: medium/long

Use deterministic offsets for pulses.

## Palette Behavior

If palette enabled:

- Use active palette colors for pulses.

Default palette suggestion:

- Circuit Glow

If palette disabled:

- Use cyan/green pulse over deep blue/black base.

## Metadata

- Category: Experimental / Calm
- Tags: animated, experimental, cool, palette-compatible
- Flashing: false
- Sleep-safe: maybe false by default, true at low brightness
- Supports Palette: true

## Preview Approximation

Preview should show soft moving pulses with trails.

## Acceptance Criteria

- Circuit Glow is visibly trace-like.
- It is not harsh or strobing.
- It uses central render pipeline.
- Palette support works.

---

# 11. Mode Metadata and Filtering Updates

## Goal

Every new mode must appear correctly in mode metadata and filtering UI.

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
- Flashing false
- Utility false unless applicable
- Blocked by Night Guard false
- Supports palette true/false
- Preview supported true/false, if such field exists

## Suggested Category Mapping

```text
Satin Breathing: Sleep / Calm
Paper Lantern Row: Calm / Everyday
Library Lamp: Focus / Everyday
Solar Noon: Focus / Everyday
Golden Hour: Calm / Everyday
Dream Aquarium: Weather / Nature / Calm
Circuit Glow: Experimental / Calm
```

## Required UI Behavior

- Mode dropdown/filter list includes new modes.
- Filters show them under appropriate categories.
- Description panel works.
- Night Guard does not mark them as blocked.

## Acceptance Criteria

- `/api/modes` includes new modes.
- UI filter categories work.
- Surprise Me can see metadata for new modes.

---

# 12. Palette Compatibility Updates

## Goal

Make new modes use the Phase 3B palette system where appropriate.

## Palette-Compatible Modes

Recommended palette support:

- Satin Breathing: yes
- Paper Lantern Row: yes
- Library Lamp: limited/optional
- Solar Noon: limited/optional
- Golden Hour: yes
- Dream Aquarium: yes
- Circuit Glow: yes

## Behavior Rules

If palette enabled and mode supports palette:

- Use active palette tastefully.
- If palette is inappropriate for the mode, reduce effect or fallback to a safer default.

Examples:

- Dream Aquarium should prefer cool/water palettes.
- Golden Hour should preserve warm/golden character.
- Library Lamp should avoid neon chaos even if Soft Neon is active.
- Circuit Glow can use colorful/cool palettes.

## Acceptance Criteria

- Palette-enabled new modes look intentional.
- Palette disabled modes use active color/white defaults.
- Invalid/missing palette falls back safely.

---

# 13. Scene Compatibility Updates

## Goal

New modes must work with scene save/load.

## Required Behavior

When user saves a scene while a new mode is active:

- Scene stores the new mode ID/name.
- Scene stores relevant mode-specific settings if exposed.
- Scene stores active palette reference if palette is enabled.

When loading a scene with a new mode:

- Mode is restored.
- Palette state is restored.
- Relevant settings are restored.
- Transition behavior works.

## Built-In Scene Updates

Optional but useful:

Add or update built-in/practical scenes to use new modes if available:

- Sleep → Satin Breathing option, if appropriate
- Reading / Library → Library Lamp
- Workbench / Focus → Solar Noon
- Cozy Goblin / Evening → Paper Lantern Row or Golden Hour
- Dream Aquarium scene, optional
- Circuit Glow scene, optional experimental

Do not overwrite user custom scenes.

If adding built-in scenes, seed only missing ones or provide reset action.

## Acceptance Criteria

- New modes can be saved and loaded through scenes.
- Existing scenes still work.
- User custom scenes are not overwritten.

---

# 14. Favorite Bar / Surprise Me Integration

## Goal

Make new modes available to favorites and Surprise Me where appropriate.

## Favorite Bar

Do not overload the favorite bar automatically.

Optional:

- Add Library Lamp or Golden Hour as favorite candidate if slots are available.
- Do not break existing favorites.

## Surprise Me

Update Surprise Me candidate generation to include new modes:

### Calm

- Satin Breathing
- Paper Lantern Row
- Golden Hour
- Dream Aquarium

### Evening

- Paper Lantern Row
- Golden Hour
- Satin Breathing
- Dream Aquarium

### Sleep-safe

- Satin Breathing
- Paper Lantern Row at low brightness
- Golden Hour at low brightness

### Focus

- Library Lamp
- Solar Noon

### Colorful

- Dream Aquarium
- Circuit Glow
- Existing colorful modes, still no strobe/flash

### Cozy

- Paper Lantern Row
- Golden Hour
- Satin Breathing warm palette

## Safety Rules

Surprise Me must still:

- Never choose Strobe/Flash.
- Respect Night Guard.
- Respect power caps.
- Respect palette validity.

## Acceptance Criteria

- Surprise Me can generate new modes appropriately.
- New modes do not make Surprise Me unsafe.
- Favorites remain stable.

---

# 15. Browser Preview Updates

## Goal

Add approximate browser-side preview renderers for every new mode.

## Required Preview Renderers

Implement browser preview approximations for:

- Satin Breathing
- Paper Lantern Row
- Library Lamp
- Solar Noon
- Golden Hour
- Dream Aquarium
- Circuit Glow

## Preview Principles

- Browser-side only.
- No server-side LED frame streaming.
- No high-frequency ESP8266 polling.
- Use existing `/api/state`, `/api/modes`, `/api/palettes` data.
- Preview approximates, not perfectly reproduces.

## Acceptance Criteria

- Every new mode has a preview approximation or explicit fallback.
- Preview remains responsive.
- Preview does not increase ESP8266 load significantly.

---

# 16. Night Guard Behavior

## Goal

New modes must behave safely under Night Guard.

## Required Behavior

Night Guard should:

- Cap brightness for all new modes.
- Not block any new mode by default unless mode is later found too energetic.
- Allow Sleep-safe filtering to find Satin Breathing and other calm modes.
- Keep full flashing protection unchanged.

## Mode-Specific Considerations

- Solar Noon can be bright/cool; Night Guard cap should make it less intense but not necessarily block it.
- Circuit Glow may be experimental; keep brightness capped and motion soft.
- Dream Aquarium should remain calm.

## Acceptance Criteria

- New modes respect effective brightness.
- Night Guard filter/blocking logic still works.
- No new mode is accidentally tagged flashing.

---

# 17. Timer and Bedtime Fade Behavior

## Goal

New modes should work with timers and bedtime fade.

## Required Behavior

- Timers can load scenes using new modes.
- Bedtime fade can target a scene using Satin Breathing or Paper Lantern Row if user creates one.
- Active timer/fade conflict behavior remains unchanged.

Do not rewrite the timer system.

## Acceptance Criteria

- Timer scene load works with new modes.
- Bedtime fade remains stable.
- No double-transition/fade bugs introduced.

---

# 18. Smooth Transition Behavior

## Goal

New modes should transition cleanly.

## Required Behavior

- Mode changes into new modes use Phase 3A transition behavior.
- Scene loads into new modes transition.
- Surprise Me into new modes transitions.
- Off/Warm Dim still work.

If animated target frames are snapshot-based during transitions, document any limitations.

## Acceptance Criteria

- New modes do not bypass transition system.
- Transitions do not double-apply brightness/gamma.

---

# 19. Existing Mode Cleanup

## Goal

Use Phase 3D to lightly normalize existing modes where needed, without redesigning them.

## Required Checks

Review existing modes for:

- Metadata completeness
- Palette compatibility flags
- Preview support flags
- Scene compatibility
- Night Guard behavior
- Direct output bypasses
- Blocking delays
- Incorrect category/tag assignments

## Important Rule

Do not redesign existing modes visually unless fixing an obvious bug.

This is cleanup, not “everything gets rewritten because Codex found a loose semicolon and became a poet.”

## Acceptance Criteria

- Existing modes still work.
- Existing modes are better integrated with metadata/scenes/preview/palette where practical.
- No regressions.

---

# 20. UI Updates For New Modes

## Goal

Make new modes accessible without UI clutter.

## Required UI Behavior

- New modes appear in mode dropdown/list.
- New modes appear in filters/categories.
- Description/best-used-for text appears.
- Palette support note appears where relevant.
- Preview works when new mode selected.
- Mode-specific controls appear only if implemented.

## UI Constraint

Do not add seven giant control sections to the main page.

If mode-specific controls are added, use the existing conditional controls pattern.

If controls are deferred, document defaults.

## Acceptance Criteria

- UI remains organized.
- New modes are discoverable.
- Main page does not become a cockpit made of accordions.

---

# 21. API Updates

## Goal

Make new modes work through existing APIs.

## Required Behavior

- `/api/modes` includes new modes.
- `/api/mode?name=...` can select new modes.
- Legacy `/set` mode control works if applicable.
- `/api/state` reflects new active mode.
- Scene APIs save/load new modes.
- Surprise API/browser generator can apply new modes.

No new mode-specific API endpoints are required unless controls are exposed.

If controls are exposed:

- Reuse existing `/set` or `/api/mode-settings` style if present.
- Validate inputs.
- Save settings if persistent.

## Acceptance Criteria

- New modes are API-selectable.
- Invalid mode names still return JSON errors.
- Existing APIs remain backward compatible.

---

# 22. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_3D_Report.md
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
9. Night Guard behavior.
10. Timer/transition interaction.
11. Existing mode cleanup performed.
12. UI/API changes.
13. Resource report.
14. Compile/test status.
15. Known limitations.
16. What was intentionally deferred.
17. Recommended next step: Phase 4A advanced scheduler and scene links, or Phase 4 planning review.

## Required Honesty

Do not claim:

- All future lighting modes are implemented.
- Weekly scheduler exists.
- QR/NFC links exist.
- Physical controls exist.
- Microphone behavior exists.
- Light sensor behavior exists.
- Actual current measurement exists.
- Physical LED testing happened unless it actually happened.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

If any of the seven planned modes were deferred, say exactly which and why.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Mode Architecture

Identify:

- Current mode enum/list/table/string handling
- Current render dispatch function
- Current mode metadata table
- Current scene save/load behavior
- Current palette helpers
- Current preview renderer dispatch
- Current Surprise Me generator
- Current settings structure for mode-specific settings

Do not rewrite the whole sketch.

## Step 2 — Add Mode IDs and Metadata First

Register all seven new modes in metadata and mode list.

Compile if possible.

Do not implement visuals yet until registration is clean.

## Step 3 — Add Render Stubs

Add safe render stubs for each mode that render a simple active-color fill or safe fallback.

This ensures mode selection works before complex algorithms are added.

## Step 4 — Implement Simple/Practical Modes First

Recommended order:

1. Library Lamp
2. Solar Noon
3. Satin Breathing
4. Golden Hour
5. Paper Lantern Row
6. Dream Aquarium
7. Circuit Glow

Reason:

- Start with stable practical modes.
- Then add more animated modes.
- Leave the most visually complex modes for later in the subphase.

## Step 5 — Add Palette Behavior

Add palette handling mode by mode.

Do not let missing palettes crash anything.

## Step 6 — Add Scene Compatibility

Ensure scene save/load handles new modes and any new settings.

## Step 7 — Add Preview Renderers

Add browser preview approximations for the new modes.

Keep them lightweight.

## Step 8 — Add Surprise Me Integration

Update candidate logic to include new modes with safe mood rules.

## Step 9 — Add/Update UI Controls

Expose mode-specific controls only if simple.

Otherwise document defaults.

## Step 10 — Existing Mode Cleanup

Review existing mode metadata/palette/preview/night-guard integration.

Fix only safe issues.

## Step 11 — Compile and Resource Report

Compile for D1 mini / ESP8266.

Collect resource measurements if possible.

## Step 12 — Create Phase 3D Report

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
- Existing `/api/...` routes still work.
- Scene/favorite/timer/Night Guard/transition/palette/preview/surprise routes still work.
- `/api/modes` still works.

## New Mode Registration

- Satin Breathing appears in mode list.
- Paper Lantern Row appears in mode list.
- Library Lamp appears in mode list.
- Solar Noon appears in mode list.
- Golden Hour appears in mode list.
- Dream Aquarium appears in mode list.
- Circuit Glow appears in mode list.
- All seven have metadata.
- All seven have descriptions.
- All seven have tags/categories.

## New Mode Rendering

- Satin Breathing renders smoothly.
- Paper Lantern Row renders soft blobs.
- Library Lamp renders stable warm/neutral light.
- Solar Noon renders stable daylight-style light.
- Golden Hour renders warm directional gradient.
- Dream Aquarium renders cool watery movement.
- Circuit Glow renders soft trace-like pulses.
- No new mode uses blocking delays.
- No new mode bypasses output pipeline.

## Palette Behavior

- Palette-enabled modes use palettes safely.
- Palette disabled falls back to active color/white.
- Invalid/missing palette falls back safely.
- Palette support notes are accurate.

## Scene Behavior

- Scenes can save each new mode.
- Scenes can load each new mode.
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
- Focus surprise can choose Library Lamp/Solar Noon.
- Colorful surprise can choose Dream Aquarium/Circuit Glow safely.
- Strobe/Flash remain excluded.

## Night Guard

- Night Guard caps brightness for all new modes.
- New modes are not falsely blocked as flashing.
- Sleep-safe filtering includes appropriate new modes.

## Timers / Bedtime

- Timer can load scene using a new mode.
- Bedtime fade remains stable.
- Timer/fade conflict behavior unchanged.

## Transitions

- Mode changes into new modes transition.
- Scene/favorite/surprise into new modes transition.
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

# Phase 3D Non-Goals Reminder

Do not implement these yet:

- Remaining Phase 4 lighting modes
- Window Sunrise
- Ember Quilt
- Porcelain Sky
- Hearthline
- Evening Current
- Starlit Linen
- Deep Sea Glass
- Quiet Northern Lights
- Candle Hall
- Slow Prism
- Garden Shade
- Snowfall Hush
- Observatory Glow
- Rainy Window
- Campfire Blanket
- Northern Whisper
- Storm Outside
- Full weekly scheduler
- QR/NFC scene links
- Advanced scene import/export
- Physical controls
- Music-reactive behavior
- Room-aware brightness
- ESP32-specific rewrite

If tempted, write it in the report as deferred.

The first mode batch gets seven modes. Not twenty-seven. Firmware goblin math is not allowed.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 3D complete.

Files changed:
- ...

Implemented:
- ...

New modes:
- Satin Breathing: ...
- Paper Lantern Row: ...
- Library Lamp: ...
- Solar Noon: ...
- Golden Hour: ...
- Dream Aquarium: ...
- Circuit Glow: ...

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

Night Guard/timer/transition interaction:
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
- Free heap while running each new mode, if measurable:
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
- Phase 4A — advanced scheduler and scene links, or Phase 4 planning review.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim all future modes, weekly scheduling, QR/NFC links, physical controls, microphone behavior, light sensor behavior, ESP32 migration, or actual current measurement unless those actually exist.

---

# One-Sentence Mission Summary

Implement the first seven new polished lighting modes through the existing D1-mini-safe architecture, with metadata, palette support, scene compatibility, preview approximations, Surprise Me integration, Night Guard safety, transition support, and resource reporting so the project gains visual richness without becoming a firmware dumpster fire in a lava lamp.

