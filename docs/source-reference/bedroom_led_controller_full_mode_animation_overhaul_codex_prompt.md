# Bedroom LED Controller — Full Mode Animation Overhaul Codex Prompt

## Mission

You are working on the GitHub repository:

```text
https://github.com/Kgray44/Bedroom_Lights
```

Firmware target:

- D1 mini / ESP8266
- WS2812 / NeoPixel strip
- Local-first web UI
- OTA support
- Browser `/update` support
- Existing central render/output pipeline
- Existing scenes, favorites, palettes, timers, bedtime fade, transitions, schedules, diagnostics, and browser preview

This task is a **complete animation and mode overhaul**.

The purpose is to make every existing lighting mode more visually convincing, more expressive, and more clearly connected to its name. The current issue is that many modes are too subtle, too generic, or behave like a slow wave with a poetic label taped to it. This pass should make each mode feel intentional.

This prompt is **not** about LED density or physical scaling. Another task is handling density-aware rendering. Do not work on LED density here except to avoid breaking anything already present.

This prompt is specifically about:

1. Making the `Animation Strength` control real.
2. Making every mode respond visibly and meaningfully to `Animation Strength`.
3. Improving every existing mode’s animation design.
4. Adding or improving simulation tools so visual behavior can be reviewed before upload.
5. Producing proof that each mode changes across strength levels.
6. Preserving the central output pipeline, OTA, routes, scenes, palettes, schedules, Night Guard, transitions, and diagnostics.

Do **not** add new hardware support.

Do **not** remove existing modes.

Do **not** replace the current project with a new architecture.

Do **not** downgrade visual quality for speed unless measurements prove a specific performance problem.

Do **not** claim physical LED testing unless it actually happened.

The final result should feel like a polished ambient lighting product, not a collection of generic sine waves wearing different hats.

---

# Current Code Inventory Requirement

Before editing, inspect and summarize the current implementation.

Review at least:

```text
firmware/BedroomLedController/ControllerTypes.h
firmware/BedroomLedController/Config.h
firmware/BedroomLedController/SettingsStorage.h
firmware/BedroomLedController/LedRendering.h
firmware/BedroomLedController/WebUi.h
firmware/BedroomLedController/WebRoutes.h
firmware/BedroomLedController/ModeMetadata.h
firmware/BedroomLedController/PaletteControls.h
firmware/BedroomLedController/TransitionControls.h
firmware/BedroomLedController/Diagnostics.h
firmware/BedroomLedController/JsonHelpers.h
firmware/BedroomLedController/ForwardDeclarations.h
```

Confirm:

1. The actual mode enum list.
2. All render functions currently used by `renderCurrentModeToFrame()`.
3. Whether `animationStrength` already exists anywhere.
4. Whether any dead/fake UI slider exists.
5. How `/api/state` is built.
6. How settings are persisted.
7. How mode metadata is served.
8. How the browser preview approximates modes.
9. How Night Guard blocks/caps behavior.
10. How transitions interact with mode rendering.
11. Whether temporal smoothing is active for modes.
12. Current resource status and compile status.

Do not invent missing modes. Work only from the actual code.

---

# Hard Boundaries

## Must preserve

- `/`
- `/state`
- `/set`
- `/ota`
- `/update`
- `/diagnostics`
- `/api/state`
- `/api/modes`
- `/api/scenes`
- `/api/favorites`
- `/api/palettes`
- `/api/timer`
- `/api/schedule`
- `/api/diagnostics`
- existing API compatibility
- scenes
- favorites
- palettes
- timers
- bedtime fade
- schedule behavior
- Night Guard behavior
- transitions
- diagnostics
- central frame buffer
- central output path
- gamma correction
- RGB calibration
- effective brightness cap
- OTA/web responsiveness

## Must not do

- Do not add density-aware rendering in this task.
- Do not add new hardware assumptions.
- Do not add microphone support.
- Do not add light sensor support.
- Do not add physical controls.
- Do not add cloud dependencies.
- Do not remove modes.
- Do not remove the browser preview.
- Do not remove mode metadata.
- Do not bypass the render/output pipeline.
- Do not use blocking `delay()` in animations.
- Do not call `strip.show()` inside normal mode renderers.
- Do not use `strip.setBrightness()` as the live brightness mechanism.
- Do not spam LittleFS writes during animation.
- Do not claim physical testing unless physical LEDs were actually observed.

---

# Core Feature 1 — Real Animation Strength

## Goal

Add or repair a single global animation control:

```cpp
uint8_t animationStrength; // 0..255, default 128
```

This setting must be real, persistent, visible in API state, controlled by the web UI, and consumed by every animated mode.

`Animation Strength` is **not** master brightness.

Master brightness controls final output cap.

Animation Strength controls how alive the animation is.

## Required behavior

At minimum, `Animation Strength` should influence:

- motion speed
- motion range
- contrast depth
- animation amplitude
- shimmer/glint strength
- number of visible animation elements
- flicker depth
- breathing depth
- ribbon opacity
- droplet/star/particle density
- pulse size or intensity
- color spread where appropriate

## Strength interpretation

Use a consistent meaning across the project:

```text
0:
  Very subtle. Mode still alive but nearly calm/static.
  Low contrast, slow motion, minimal shimmer, low density.

64:
  Gentle. Useful for sleep/evening.
  Clearly animated if watched, but not attention-grabbing.

128:
  Normal default. The intended balanced look.
  Visible motion and mode identity without being harsh.

192:
  Expressive. Stronger motion, richer contrast, more elements.
  Still tasteful for ambient modes.

255:
  Maximum expression. Strongest version of the mode.
  Still must remain smooth and non-harsh unless the mode is Strobe/Flash.
```

## Add helpers

Add helpers equivalent to:

```cpp
float animationStrength01();
float animationStrengthCentered();
float strengthCurve(float minValue, float maxValue, float curve);
float strengthLinear(float minValue, float maxValue);
float strengthEase(float minValue, float maxValue);
uint8_t strengthByte(uint8_t minValue, uint8_t maxValue);
float modeSpeedMultiplier(Mode mode);
float modeMotionMultiplier(Mode mode);
float modeContrastMultiplier(Mode mode);
float modeDensityMultiplier(Mode mode);
float modeColorSpreadMultiplier(Mode mode);
uint8_t modeFloorForStrength(uint8_t low, uint8_t high);
uint8_t modeCeilingForStrength(uint8_t low, uint8_t high);
```

Use these helpers instead of scattering raw `settings.animationStrength / 255.0f` everywhere.

## Suggested curve behavior

Use non-linear curves so low strength remains gentle and high strength becomes expressive:

```cpp
float s = animationStrength01();
float gentle = s * s * (3.0f - 2.0f * s);        // smoothstep
float expressive = powf(s, 1.35f);               // more control at low end
float sensitive = sqrtf(s);                       // visible even at low end
```

Use the correct curve per mode:

- Calm modes: slower strength ramp.
- Utility modes: direct strength response.
- Colorful modes: strength affects saturation/spread carefully.
- Sleep-safe modes: cap maximum aggressiveness even at 255 if Night Guard is active.

## Persistence

Add `animationStrength` to:

- settings defaults
- settings load
- settings save
- settings schema/report
- corrupt/missing settings fallback

Default:

```cpp
settings.animationStrength = 128;
```

Validation:

- Clamp numeric values to `0..255`.
- Reject non-numeric API values.
- Mark settings dirty only if changed.
- Reset temporal smoothing when changed.
- Mark LEDs dirty when changed.

## API

Add or normalize an endpoint:

```text
/api/animation?strength=128
```

Alternative acceptable endpoint:

```text
/api/motion?strength=128
```

If a more consistent route style already exists, use that.

Response example:

```json
{
  "ok": true,
  "message": "Animation strength updated",
  "animationStrength": 128,
  "animationStrengthPercent": 50
}
```

Error example:

```json
{
  "ok": false,
  "error": "Invalid animation strength value"
}
```

Also expose in `/api/state`:

```json
{
  "animationStrength": 128,
  "animationStrengthPercent": 50
}
```

## UI

Add exactly one real slider:

```text
Animation Strength
Subtle ←→ Expressive
```

Recommended placement:

- inside the Mode section
- below selected mode metadata/description
- near mode-specific controls

Slider:

```html
<input id="animationStrength" type="range" min="0" max="255" value="128">
```

Show:

```text
128 / 255
50%
Subtle / Balanced / Expressive label
```

UI behavior:

- Initialize from `/api/state`.
- Debounce updates.
- Do not send updates during hydration.
- Show success/error message through the existing global status system.
- Do not duplicate any existing dead slider.
- Keep old mode-specific speed controls unless intentionally consolidated later.

---

# Core Feature 2 — Simulation Proof Harness

## Goal

Add simulation tooling so Codex can prove each mode changes with animation strength before the user uploads to the ESP8266.

This is required because repeatedly uploading firmware to visually inspect every mode is slow and painful.

## Required tool

Create either:

```text
tools/simulate_modes.py
```

or:

```text
tools/mode_simulator/simulate_modes.py
```

The simulator must generate a reviewable visual output.

Preferred output:

```text
docs/generated/mode_simulations/index.html
docs/generated/mode_simulations/<mode-id>.html
docs/generated/mode_simulations/contact_sheet.html
docs/generated/mode_simulations/mode_metrics.json
docs/generated/BedroomLedController_Mode_Overhaul_Report.md
```

The HTML should show horizontal LED strip previews for each mode at multiple times and strength levels.

## Required simulation matrix

For every mode, simulate at least:

```text
Animation Strength: 0, 64, 128, 192, 255
Time samples: 0s, 2s, 5s, 10s, 20s, 40s
LED count: current configured LED_COUNT
```

Optional but useful:

```text
low LED count sanity: 30 LEDs
large strip sanity: 800 LEDs
```

Do not make density the focus. This task is not about density. The simulator may still test different LED counts to catch scaling bugs, but do not implement density-specific feature work here.

## Required visual proof

Each mode page should include:

- mode name
- mode description
- strength levels shown side by side
- time samples shown vertically or horizontally
- animated GIF or CSS/HTML frame grid if practical
- note whether the mode is calm/sleep-safe/flashing/colorful

## Required metrics

For every mode and every strength level, compute approximate metrics:

```text
averageBrightness
minBrightness
maxBrightness
brightnessRange
spatialContrast
frameToFrameDelta
estimatedMotionScore
colorVariance
activePixelRatio
```

These do not need to be perfect scientific measurements. They are sanity checks.

The simulator should flag:

- animation too subtle at strength 128
- almost no difference between strength 0 and 255
- very low motion score for an animated mode
- excessive black valleys in calm modes
- too much flash intensity in Night Guard-sensitive modes
- suspiciously uniform output for modes that should have texture
- overly chaotic output for sleep-safe modes

## Required proof thresholds

For every animated non-utility mode:

1. Strength 255 must have higher motion/contrast/density than Strength 0.
2. Strength 128 must be visibly different from Strength 0.
3. Strength 0 must still produce a valid, non-broken, non-black output unless the mode intentionally has black phases.
4. Calm/sleep-safe modes must not become harsh at 255.
5. Strobe/Flash may be harsh, but must be clearly marked utility/flashing and blocked by Night Guard.

## Required report section

The report must include a table:

```text
Mode | Strength response proven? | Visual proof generated? | Metrics passed? | Notes
```

Do not write “looks good” without evidence.

The report should link or reference generated HTML files.

## Implementation style

The simulator does not need to compile/run the C++ firmware directly. It may implement Python approximations of each renderer. However:

- The approximation must be close enough to catch motion/strength problems.
- Each simulated mode must use the same conceptual layers as the firmware mode.
- If the simulation is approximate, say so clearly.
- The simulator must not be presented as physical LED proof.

---

# Core Feature 3 — Browser Preview Must Use Animation Strength

The current browser preview must be updated so the user can see the effect of Animation Strength in the web UI.

Update preview state:

```js
state.animationStrength
```

Every browser preview renderer should change based on strength.

At minimum:

- Strobe/Flash preview should alter speed/contrast.
- Chase should alter comet count/trail/speed.
- Wave/Slow Wave should alter depth/speed.
- Slow Pulse should alter pulse depth/count impression.
- Rainbow should alter saturation/speed.
- Rain Glass/Rainy Window should alter droplet density/trails.
- Quiet Northern Lights/Northern Whisper should alter ribbon count/opacity/motion.
- Candle Hall should alter flicker/halo intensity.
- Starlit Linen/Snowfall Hush should alter particle density/brightness.
- Storm Outside should alter storm depth/flash softness.

The preview remains browser-side. Do not stream firmware frames to the browser.

---

# Core Feature 4 — Mode Metadata Updates

Update mode metadata if useful.

Add compact metadata fields if they help the UI/simulator:

```cpp
uint8_t defaultStrength;
uint8_t minRecommendedStrength;
uint8_t maxRecommendedStrength;
bool strengthAffectsSpeed;
bool strengthAffectsDensity;
bool strengthAffectsContrast;
bool strengthAffectsColorSpread;
bool strengthAffectsTexture;
```

If this would bloat RAM, use `const`/flash-backed metadata or keep the fields in the simulator/report only.

Do not break `/api/modes` consumers.

If `/api/modes` schema changes, update:

- Web UI
- API docs
- tests
- mode simulation tool

---

# Mode-by-Mode Overhaul Requirements

The following sections define exactly what each mode should become.

For every mode:

1. Keep the mode name and existing public ID.
2. Preserve scene/palette compatibility unless explicitly inappropriate.
3. Respect master brightness and Night Guard effective brightness.
4. Use central output pipeline.
5. Use `settings.animationStrength` or shared strength helpers.
6. Generate simulation proof.
7. Update browser preview.
8. Document what changed.

---

## 1. Solid

### Current role

A steady color or white temperature.

### Problem to avoid

Solid should not become an animation just because Animation Strength exists. It is the stable baseline mode. It is used for practical scenes, warm dim, focus lighting, white temperature, and recovery/fallback behavior.

### Required behavior

Solid should remain steady.

Animation Strength should not visibly animate Solid by default.

If you choose to make Solid respond at all, the response must be extremely conservative:

- Strength 0–255 may slightly alter optional texture only if a future explicit setting exists.
- Do not add breathing by default.
- Do not add shimmer by default.
- Do not add palette drift by default.

### End result

Solid is still the dependable mode.

At every Animation Strength level, it should look stable and usable.

### Simulation proof

The simulator should show Solid as unchanged across strength levels, and that is acceptable.

Mark Solid as:

```text
Strength response: intentionally stable
```

---

## 2. Strobe

### Current role

Utility high-contrast on/off flashing.

### Required overhaul

Strobe should remain a utility/flashing mode, but Animation Strength should make it less brutal at low values and more direct at high values.

### Strength mapping

At Strength 0:

- slowest allowed strobe
- low duty cycle or softened output
- less aggressive contrast if practical
- still obviously strobe, but gentler

At Strength 128:

- current-ish strobe behavior
- clear on/off pulses

At Strength 255:

- fastest/strongest allowed strobe within safe code limits
- high contrast

### Implementation notes

Use non-blocking timing only.

Do not use `delay()`.

The strobe timing should consider both the existing strobe delay setting and Animation Strength:

```text
finalStrobePeriod = userStrobePeriod * strengthSpeedMultiplier
```

Low strength should lengthen the period.

High strength should shorten the period, but clamp to a safe minimum.

### Night Guard

Night Guard must block Strobe when flashing blocking is enabled.

Do not weaken that safety rule.

### End result

The user can tell that Animation Strength changes strobe intensity/speed, but Strobe remains clearly marked and treated as a harsh utility mode.

### Simulation proof

Show on/off pattern differences across strength levels.

Metrics should prove frame-to-frame delta increases with strength.

---

## 3. Flash

### Current role

Repeating flash with configurable off delay.

### Required overhaul

Flash should become a softer or sharper alert depending on Animation Strength.

### Strength mapping

At Strength 0:

- soft alert pulse
- slower cycle
- reduced on brightness or shorter on-time

At Strength 128:

- clear flash
- similar to current user expectation

At Strength 255:

- sharp, bright, attention-grabbing flash

### Implementation notes

Animation Strength should influence:

- on-time
- off-time multiplier
- brightness of the flash body if appropriate
- edge softness if implemented through a brief fade envelope

A good non-blocking approach:

```text
cycle position -> envelope -> flash brightness
```

Instead of only binary on/off, low strength can use a short soft envelope.

### Night Guard

Night Guard must block Flash when flashing blocking is enabled.

### End result

Flash should be usable as a signaling tool without always being maximum visual violence.

### Simulation proof

Show flash cycle at each strength.

Metrics should show stronger temporal contrast at higher strength.

---

## 4. Chase

### Current role

Moving active-color chase/wave.

### Current problem

Chase should look like something is chasing or traveling. If it only looks like a generic sine wave, it fails its name.

### Required overhaul

Rebuild Chase as a comet/train mode.

It should contain:

- one or more moving heads
- a bright leading edge
- a fading trail
- optional secondary echoes at high strength
- no blocking delays
- deterministic motion

### Strength mapping

At Strength 0:

- one slow soft comet
- long gentle tail
- low contrast

At Strength 64:

- one visible comet with soft trail

At Strength 128:

- one or two comets
- medium speed
- visible trail

At Strength 192:

- multiple chase elements
- stronger head contrast
- longer or brighter tail

At Strength 255:

- most energetic chase
- multiple visible heads
- crisp but still smooth trails

### Suggested parameters

Strength should control:

```text
cometCount
cometSpeed
trailLength
headSharpness
trailBrightness
backgroundFloor
```

### Visual design

The strip should never be only a single hard pixel unless the user has maximum strength and tiny LED count.

Use soft falloff:

```text
head contribution = exp(-distance^2 / width)
trail contribution = exp(-trailingDistance / trailLength)
```

or a cheaper approximation if needed.

### End result

At normal strength, Chase should look like colored energy moving along the strip with a readable head and tail.

### Simulation proof

The simulator must show the chase head moving over time and changing count/speed/trail with strength.

---

## 5. Wave

### Current role

Smooth moving brightness wave using active color.

### Required overhaul

Wave should become a clean, readable wave mode with broad motion, not just a single sine brightness sweep.

### Required layers

Use at least:

1. primary broad wave
2. secondary slower counter-wave
3. stable brightness floor

Optional:

4. subtle texture at high strength

### Strength mapping

At Strength 0:

- very shallow wave
- slow movement
- mostly steady light

At Strength 128:

- visible broad wave motion
- comfortable contrast

At Strength 255:

- stronger wave depth
- more obvious secondary ripple
- faster movement
- still no harsh black gaps

### Parameters controlled

```text
waveAmplitude
waveSpeed
secondaryRippleAmount
crestSharpness
brightnessFloor
brightnessCeiling
```

### End result

Wave should look like smooth moving energy, not random dimming.

### Simulation proof

Metrics should show increasing spatial contrast and frame-to-frame delta with strength.

---

## 6. Slow Wave

### Current role

Calm slow wave/swell behavior.

### Required overhaul

Slow Wave should feel like broad ocean swells or slow luminous ribbons moving through the strip.

It should not feel like generic pulsing blobs.

### Required layers

Use multiple slow swell layers:

- broad primary swell
- secondary offset swell
- optional soft crest/foam layer
- stable background floor

### Strength mapping

At Strength 0:

- almost still
- extremely broad slow shift
- low contrast

At Strength 64:

- clear but gentle slow swell

At Strength 128:

- current intended calm motion
- readable broad wave fronts

At Strength 192:

- stronger swell layers
- more pronounced crests

At Strength 255:

- expressive ocean-like movement
- still calm, not harsh

### Parameters controlled

```text
swellLayerCount
swellAmplitude
crestEnergy
foamAmount
travelSpeed
backgroundFloor
```

### End result

Slow Wave should be one of the most relaxing movement modes.

### Simulation proof

The simulator must show slow broad movement over time, not a static stripe.

---

## 7. Slow Pulse

### Current role

Soft pulse/bloom mode.

### Required overhaul

Slow Pulse should become a field of breathing light islands.

It should feel like soft pulses appearing, expanding or blooming, holding briefly, then fading.

### Required behavior

Each pulse should have:

- center
- width/radius
- birth time
- duration
- brightness envelope
- optional drift
- seeded randomness

Use fixed arrays. Do not allocate dynamically.

### Strength mapping

At Strength 0:

- one or two very gentle pulses
- low bloom depth
- slow lifetime

At Strength 128:

- several soft breathing zones
- clear pulse movement

At Strength 255:

- more pulse zones
- stronger bloom depth
- more visible drift
- still smooth

### Parameters controlled

```text
pulseCount
pulseRadius
pulseBrightness
pulseLifetime
pulseDrift
textureAmount
backgroundFloor
```

### End result

Slow Pulse should look alive even at 128, but not chaotic.

### Simulation proof

Show pulse centers changing over time.

Metrics should show higher active pixel ratio and brightness variance at higher strength.

---

## 8. Rainbow

### Current role

Colorful showcase mode.

### Required overhaul

Rainbow should remain colorful but become controllable.

### Strength mapping

At Strength 0:

- slow pastel rainbow
- low saturation
- gentle gradient

At Strength 64:

- still soft, more visible color movement

At Strength 128:

- normal rainbow speed and saturation

At Strength 192:

- richer saturation and faster movement

At Strength 255:

- full expressive rainbow
- highest saturation/speed allowed

### Parameters controlled

```text
saturation
speed
colorBandWidth
contrast
brightnessFloor
```

### Night Guard behavior

If Night Guard is active:

- cap speed multiplier
- reduce saturation slightly
- avoid harsh full-intensity color jumps

### End result

Rainbow should range from tasteful pastel drift to full showcase.

### Simulation proof

Show color spread and movement differences across strength.

---

## 9. Dawn Bloom

### Current role

Warm sunrise-like bloom.

### Required overhaul

Dawn Bloom should feel like warm light blooming into the room.

### Visual concept

- warm amber edge/source
- spreading glow
- gradual shift toward softer daylight
- gentle shimmer only at medium/high strength
- no sudden jumps

### Strength mapping

At Strength 0:

- nearly steady warm low dawn light
- minimal motion

At Strength 64:

- gentle widening glow

At Strength 128:

- visible sunrise bloom sweep

At Strength 192:

- stronger warmth shift and bloom depth

At Strength 255:

- expressive sunrise movement
- still smooth and non-flashy

### Parameters controlled

```text
bloomSpread
bloomSpeed
warmthShift
edgeGlow
shimmerAmount
brightnessLift
```

### End result

Dawn Bloom should be recognizably sunrise-inspired, not just a moving brightness gradient.

### Simulation proof

Contact sheet should show the glow expanding over time and stronger bloom at higher strength.

---

## 10. Moon Tide

### Current role

Cool blue tide-like calm movement.

### Required overhaul

Moon Tide should feel like moonlit water moving in broad swells.

### Required layers

- deep moonlit base
- broad primary tide swell
- smaller counter-ripple
- soft crest highlight
- optional palette tint

### Strength mapping

At Strength 0:

- mostly steady moonlight
- barely moving tide

At Strength 64:

- gentle tide motion

At Strength 128:

- visible moonlit waves

At Strength 192:

- deeper tide and more crest detail

At Strength 255:

- expressive oceanic moonlight
- no black valleys

### Parameters controlled

```text
tideAmplitude
rippleAmount
crestHighlight
movementSpeed
coolTintStrength
brightnessFloor
```

### End result

Moon Tide should be soothing, cool, and clearly water-like.

### Simulation proof

Show broad moving swells and increasing crest detail with strength.

---

## 11. Velvet Aurora

### Current role

Rich aurora-like color ambience.

### Required overhaul

Velvet Aurora should become layered, smooth aurora curtains with rich colors.

It should be more saturated and expressive than Northern Whisper, but still not harsh.

### Required layers

- dark/soft base
- broad main aurora ribbon
- secondary ribbon moving independently
- slow fold modulation
- optional palette-driven curtain colors
- soft opacity blending

### Strength mapping

At Strength 0:

- very faint color veil
- slow movement

At Strength 64:

- gentle layered ribbon motion

At Strength 128:

- visible aurora curtain effect

At Strength 192:

- richer folds and color spread

At Strength 255:

- expressive velvet aurora
- multiple overlapping curtains
- still no flashing

### Parameters controlled

```text
ribbonCount
ribbonOpacity
foldDepth
colorSpread
movementSpeed
baseBrightness
```

### End result

Velvet Aurora should feel like a premium colorful ambience mode.

### Simulation proof

Show overlapping broad ribbons moving independently.

---

## 12. Lantern Drift

### Current role

Warm drifting lantern ambience.

### Required overhaul

Lantern Drift should feel like warm paper lantern glows drifting gently in space.

### Required behavior

Each lantern zone should have:

- soft warm center
- broad halo
- independent breathing
- tiny drift/sway
- warm paper tint

### Strength mapping

At Strength 0:

- nearly still warm lantern glow
- very little drift

At Strength 64:

- gentle breathing lanterns

At Strength 128:

- clear lantern drift

At Strength 192:

- more pronounced independent lantern motion

At Strength 255:

- vivid warm lantern field
- still cozy, never chaotic

### Parameters controlled

```text
lanternCount
haloWidth
centerBrightness
breathingDepth
swayAmount
warmthBlend
```

### End result

Lantern Drift should look warm, soft, and intentionally lantern-like.

### Simulation proof

Show multiple lantern zones breathing at different phases.

---

## 13. Rain Glass

### Current role

Cool rain-on-glass ambience.

### Required overhaul

Rain Glass should show droplets, beads, and faint trails.

It should not just look like a few moving bright spots.

### Required droplet lifecycle

Each droplet should conceptually:

1. spawn faintly
2. brighten into a bead
3. slide slowly
4. leave a fading trail
5. dissolve
6. respawn later

Use fixed deterministic droplet slots.

No heap allocation.

### Strength mapping

At Strength 0:

- very few droplets
- short faint trails
- slow movement

At Strength 64:

- gentle rain beads

At Strength 128:

- clear rain-on-glass pattern

At Strength 192:

- more droplets and longer trails

At Strength 255:

- active rainy pane
- still smooth, not sparkly chaos

### Parameters controlled

```text
dropletCount
dropletSpeed
trailLength
beadBrightness
paneShimmer
backgroundCoolness
```

### End result

Rain Glass should be recognizably rainy and glassy.

### Simulation proof

Show droplet positions and trails over time.

Metrics should show active pixel ratio increasing with strength.

---

## 14. Satin Breathing

### Current role

Whole-strip breathing mode with texture.

### Required overhaul

Satin Breathing should feel like a smooth fabric-like inhale/exhale.

### Required envelope

Use a refined breath cycle:

```text
slow inhale
short soft hold
longer exhale
small rest
```

No abrupt corners.

### Strength mapping

At Strength 0:

- almost steady
- very small breath depth
- minimal texture

At Strength 64:

- gentle breathing

At Strength 128:

- clear satin-like breathing

At Strength 192:

- deeper inhale/exhale
- visible fabric texture

At Strength 255:

- strong but smooth breathing
- no flashing

### Parameters controlled

```text
breathDepth
breathPeriod
holdDuration
textureAmount
warmthShift
minimumFloor
```

### End result

Satin Breathing should be a top-tier sleep/evening mode.

### Simulation proof

Show whole-strip brightness envelope across time and strength.

---

## 15. Paper Lantern Row

### Current role

Large warm glowing blobs like lanterns.

### Required overhaul

Paper Lantern Row should feel like distinct paper lanterns arranged across the strip.

### Required lantern model

Each lantern should have:

- soft circular/oval brightness profile
- warm core
- paper-like halo
- independent breathing
- slight sway
- optional palette warmth

### Strength mapping

At Strength 0:

- very stable lantern row
- nearly still

At Strength 64:

- gentle paper breathing

At Strength 128:

- visible independent lantern life

At Strength 192:

- stronger swaying and warm-core contrast

At Strength 255:

- expressive lantern row
- still cozy and readable

### Parameters controlled

```text
lanternCount
lanternBrightness
haloSoftness
breathingDepth
swayAmount
paperWarmth
```

### End result

The user should instantly understand why this mode is called Paper Lantern Row.

### Simulation proof

Show lantern centers and breathing differences across time.

---

## 16. Library Lamp

### Current role

Practical warm reading/homework light.

### Required overhaul

Library Lamp should remain practical and focus-friendly.

Animation Strength must not ruin its purpose.

### Strength mapping

At Strength 0:

- stable warm reading light

At Strength 64:

- almost stable, tiny warmth texture

At Strength 128:

- very faint incandescent variation

At Strength 192:

- slightly more texture, still readable

At Strength 255:

- maximum allowed subtle texture
- still suitable for reading

### Parameters controlled

```text
textureAmount
warmthDrift
microVariation
```

Do not control:

- major brightness pulsing
- large moving waves
- distracting color changes

### End result

Library Lamp should be useful for reading at all strength levels.

### Simulation proof

Metrics should show low motion score even at high strength, intentionally.

Mark as:

```text
Strength response: subtle practical mode
```

---

## 17. Solar Noon

### Current role

Daytime productivity light.

### Required overhaul

Solar Noon should remain practical, bright, and focus-safe.

### Strength mapping

At Strength 0:

- stable neutral daylight

At Strength 64:

- tiny gradient variation

At Strength 128:

- subtle refined daylight drift

At Strength 192:

- slightly more visible daylight gradient

At Strength 255:

- maximum refined gradient movement
- still practical

### Parameters controlled

```text
gradientAmount
gradientDriftSpeed
whiteTemperatureSpread
textureAmount
```

### End result

Solar Noon should feel like clean daylight, not an animated party mode.

### Simulation proof

Low temporal delta is acceptable and should be documented.

---

## 18. Golden Hour

### Current role

Warm sunlight ambience.

### Required overhaul

Golden Hour should feel like warm low-angle sunlight moving across the strip.

### Required layers

- amber sun source
- cream/gold falloff
- broad warm gradient
- soft edge glow
- optional palette warmth

### Strength mapping

At Strength 0:

- mostly static warm golden tone

At Strength 64:

- gentle shifting sun angle

At Strength 128:

- clear warm source drift

At Strength 192:

- stronger amber source and glow

At Strength 255:

- expressive golden hour sweep
- still soft

### Parameters controlled

```text
sourceWidth
sourceBrightness
amberSaturation
movementRange
movementSpeed
edgeGlow
```

### End result

Golden Hour should feel warm, directional, and sunset-like.

### Simulation proof

Show a clear warm source moving over time.

---

## 19. Dream Aquarium

### Current role

Cool watery calm mode.

### Required overhaul

Dream Aquarium should feel underwater.

### Required layers

- deep blue/teal base
- broad water swell
- caustic shimmer
- slow parallax drift
- optional soft shadow bands

### Strength mapping

At Strength 0:

- calm deep water glow
- minimal shimmer

At Strength 64:

- gentle watery movement

At Strength 128:

- visible caustic drift

At Strength 192:

- stronger shimmer and depth

At Strength 255:

- expressive aquarium light
- not noisy

### Parameters controlled

```text
causticContrast
shimmerDensity
waterDepth
swellSpeed
parallaxAmount
shadowDepth
```

### End result

Dream Aquarium should be clearly watery and relaxing.

### Simulation proof

Show caustic-like shimmer increasing with strength.

---

## 20. Circuit Glow

### Current role

Subtle electronics-inspired trace pulses.

### Required overhaul

Circuit Glow should look like soft signals traveling through traces.

### Required behavior

Use multiple deterministic signal lanes:

- base circuit glow
- moving pulse head
- fading trail
- occasional secondary pulse
- no harsh flashing

### Strength mapping

At Strength 0:

- dim base glow with rare slow signal

At Strength 64:

- gentle trace pulses

At Strength 128:

- readable circuit signal motion

At Strength 192:

- more pulse lanes and stronger trails

At Strength 255:

- energetic circuit board glow
- still smooth

### Parameters controlled

```text
signalCount
signalSpeed
trailLength
pulseBrightness
baseGlow
laneVariation
```

### End result

Circuit Glow should feel technical, clean, and alive.

### Simulation proof

Show pulse heads moving and increasing density with strength.

---

## 21. Window Sunrise

### Current role

Directional sunrise from one side.

### Required overhaul

Window Sunrise should feel like light spilling in from one side of the room.

### Required layers

- dark/warm pre-dawn base
- amber leading edge
- widening warm area
- gradual daylight shift
- soft shimmer after the light reaches an area

### Strength mapping

At Strength 0:

- very slow subtle warm spill

At Strength 64:

- gentle sunrise edge

At Strength 128:

- clear directional sunrise

At Strength 192:

- stronger warm edge and brighter spill

At Strength 255:

- expressive sunrise sweep
- still smooth

### Parameters controlled

```text
spreadSpeed
edgeSoftness
warmthShift
shimmerAmount
finalDaylightBlend
```

### End result

Window Sunrise should feel directional and morning-like.

### Simulation proof

Show light spreading from one side across time.

---

## 22. Ember Quilt

### Current role

Soft warm glowing patches.

### Required overhaul

Ember Quilt should feel like low warm patches breathing under a blanket.

### Required layers

- warm base
- overlapping ember islands
- independent breathing
- subtle heat texture
- low contrast

### Strength mapping

At Strength 0:

- steady warm quilt glow

At Strength 64:

- gentle patch breathing

At Strength 128:

- visible warm islands

At Strength 192:

- deeper patch contrast

At Strength 255:

- expressive cozy ember patches
- still soft

### Parameters controlled

```text
patchCount
patchContrast
breathingDepth
driftAmount
textureDepth
shadowFloor
```

### End result

Ember Quilt should be cozy, warm, and low-stress.

### Simulation proof

Show overlapping warm zones changing independently.

---

## 23. Porcelain Sky

### Current role

Pale refined daytime gradient.

### Required overhaul

Porcelain Sky should remain refined and soft.

### Required layers

- pale cool white base
- pearl/warm tint
- slow cloud drift
- optional palette tint

### Strength mapping

At Strength 0:

- nearly static pale sky

At Strength 64:

- faint cloud movement

At Strength 128:

- gentle refined drift

At Strength 192:

- more visible cloud contrast

At Strength 255:

- strongest cloud motion, still elegant

### Parameters controlled

```text
cloudContrast
tintStrength
driftSpeed
paletteInfluence
brightnessVariation
```

### End result

Porcelain Sky should feel clean, pale, and polished.

### Simulation proof

Metrics should show modest but real strength response.

---

## 24. Hearthline

### Current role

Distant fireplace warmth.

### Required overhaul

Hearthline should feel like far-away hearth warmth, not an aggressive fire simulation.

### Required layers

- stable warm base
- slow amber bands
- soft ember lift
- no harsh flicker

### Strength mapping

At Strength 0:

- steady warm hearth glow

At Strength 64:

- slight heat band movement

At Strength 128:

- visible hearthline warmth

At Strength 192:

- stronger bands and ember contrast

At Strength 255:

- expressive distant fire warmth
- not chaotic

### Parameters controlled

```text
bandCount
bandSpeed
heatLevel
emberContrast
baseWarmth
flickerSoftness
```

### End result

Hearthline should feel cozy and fireplace-adjacent without becoming harsh.

### Simulation proof

Show slow warm bands moving across time.

---

## 25. Evening Current

### Current role

Smooth river-like light flow.

### Required overhaul

Evening Current should feel like a slow current of light.

### Required layers

- broad primary current
- counter-moving ripple
- stable brightness floor
- gentle warm/cool blend

### Strength mapping

At Strength 0:

- calm evening glow
- nearly still

At Strength 64:

- gentle current movement

At Strength 128:

- clear river-like flow

At Strength 192:

- deeper current and ripple

At Strength 255:

- expressive flowing light
- no black valleys

### Parameters controlled

```text
currentDepth
rippleStrength
flowSpeed
counterFlowAmount
brightnessFloor
colorBlendDepth
```

### End result

Evening Current should be one of the best relaxed movement modes.

### Simulation proof

Show current motion and avoid dark gaps.

---

## 26. Starlit Linen

### Current role

Soft sparse highlights over a steady base.

### Required overhaul

Starlit Linen should feel like tiny soft highlights blooming through fabric.

### Required behavior

Stars/highlights should:

- affect neighboring pixels
- fade in slowly
- peak softly
- fade out slowly
- not appear as harsh single-pixel dots except maybe at maximum strength

### Strength mapping

At Strength 0:

- very few faint highlights

At Strength 64:

- gentle sparse twinkle

At Strength 128:

- visible soft starlit linen effect

At Strength 192:

- more highlights and larger blooms

At Strength 255:

- most visible star field
- still soft

### Parameters controlled

```text
starDensity
starRadius
highlightBrightness
fadeDuration
baseBrightness
warmth
```

### End result

Starlit Linen should be sleep-safe and beautiful, not glittery chaos.

### Simulation proof

Show stars blooming and fading with increased density at higher strength.

---

## 27. Deep Sea Glass

### Current role

Cool quiet light through water/glass.

### Required overhaul

Deep Sea Glass should feel deeper and more layered than Dream Aquarium.

### Required layers

- deep blue-green base
- broad dim swell
- glassy caustic shimmer
- occasional soft glint
- optional palette tint

### Strength mapping

At Strength 0:

- deep steady glass color

At Strength 64:

- faint depth motion

At Strength 128:

- visible glassy water layers

At Strength 192:

- stronger shimmer and glint

At Strength 255:

- expressive glass/water effect
- still smooth

### Parameters controlled

```text
depthContrast
shimmerAmount
glintBrightness
driftSpeed
causticScale
backgroundFloor
```

### End result

Deep Sea Glass should be cool, deep, and calm.

### Simulation proof

Show layered motion and shimmer increasing with strength.

---

## 28. Quiet Northern Lights

### Current role

Aurora-inspired calm colorful mode.

### Current problem

This mode must not look like one blob moving back and forth.

It must not be merely two sine waves.

It should look like overlapping aurora ribbons that glide, fold, dissolve, reverse, and reappear.

### Required major rewrite

Implement ribbon-agent aurora behavior using fixed arrays.

No dynamic allocation.

Each ribbon should conceptually have:

```cpp
struct AuroraRibbon {
  bool active;
  float center;
  float width;
  float velocity;
  float acceleration;
  float opacity;
  float targetOpacity;
  float life;
  float lifeDuration;
  float foldPhase;
  float foldSpeed;
  float colorBlend;
  int8_t direction;
  uint8_t seed;
};
```

Use a fixed maximum ribbon count.

Suggested:

```cpp
const uint8_t MAX_AURORA_RIBBONS = 5;
```

The actual active ribbon count should depend on Animation Strength.

### Required ribbon behavior

Each ribbon should:

1. spawn softly at low opacity
2. glide in one direction
3. slowly accelerate
4. sometimes reverse direction
5. decelerate faster than it accelerates
6. fold/wave internally
7. overlap other ribbons
8. dissolve/fade out
9. respawn with different seed/color/width

### Strength mapping

At Strength 0:

- one faint ribbon
- very slow motion
- minimal color spread

At Strength 64:

- one or two soft ribbons
- gentle drift

At Strength 128:

- two or three overlapping ribbons
- visible folds
- calm aurora behavior

At Strength 192:

- more overlap and fold depth
- stronger but still smooth color

At Strength 255:

- maximum aurora activity
- multiple ribbons
- obvious glide/reverse/dissolve behavior
- still no harsh flashing

### Parameters controlled

```text
activeRibbonCount
ribbonOpacity
ribbonWidth
foldDepth
foldSpeed
colorSpread
velocity
respawnRate
backgroundFloor
```

### Visual requirements

- No obvious single blob.
- No full-strip synchronized wave pretending to be aurora.
- No fast rainbow chaos.
- No harsh black gaps.
- Multiple overlapping ribbons should be visible at normal/high strength.
- Ribbons should have soft edges and internal folds.

### Palette behavior

If palette is enabled and compatible:

- use palette colors for ribbon colors
- still preserve aurora-like cyan/green/violet tendencies if palette is absent

### End result

Quiet Northern Lights should finally look like a calm aurora: gliding ribbons, overlapping curtains, soft folds, disappearing and reappearing.

### Simulation proof

This mode must have the strongest proof requirement.

Simulator must show:

- multiple ribbons at strength 128+
- ribbon positions changing over time
- opacity fade in/out
- overlap
- different strength levels changing ribbon count/opacity/fold depth

The report must explicitly state:

```text
Quiet Northern Lights no longer uses a simple global sine/blob as its primary visual model.
```

---

## 29. Candle Hall

### Current role

Distant candles down a hallway.

### Required overhaul

Candle Hall should feel like multiple warm candle clusters, each with a soft halo and smoothed irregular flame behavior.

### Required candle model

Each candle should have:

- stable position
- warm core
- soft halo
- smoothed flicker value
- small independent sway or flame variation
- shadow/background floor

Use fixed deterministic candles.

### Strength mapping

At Strength 0:

- dim stable candles
- tiny flicker

At Strength 64:

- gentle flame life

At Strength 128:

- clear candle clusters and halos

At Strength 192:

- more flicker and halo brightness

At Strength 255:

- warm vivid candle hall
- still not harsh

### Parameters controlled

```text
candleCount
coreBrightness
haloWidth
flickerAmount
flickerSpeed
shadowFloor
warmth
```

### Visual requirements

- Candles should be clusters, not single dots.
- Flicker must be smoothed, not random harsh jumping.
- Background should stay warm/dark, not pure black unless intentionally very low strength.

### End result

Candle Hall should look like distant candlelight down a hallway.

### Simulation proof

Show candle cores/halos and flicker variation across time.

---

## 30. Slow Prism

### Current role

Tasteful active-color companion gradient.

### Required overhaul

Slow Prism should split the selected color into nearby companion colors without becoming Rainbow.

### Required behavior

- active color remains anchor
- companion hues drift slowly
- wide gradient rotates or breathes
- contrast remains tasteful

### Strength mapping

At Strength 0:

- nearly active color only
- minimal companion tint

At Strength 64:

- subtle color separation

At Strength 128:

- visible prism effect

At Strength 192:

- stronger spread and movement

At Strength 255:

- maximum tasteful prism spread
- not full rainbow unless active palette demands it

### Parameters controlled

```text
colorSpread
companionSaturation
rotationSpeed
contrast
anchorStrength
```

### End result

Slow Prism should be colorful but refined.

### Simulation proof

Show color variance increasing with strength.

---

## 31. Garden Shade

### Current role

Soft sunlight through leaves.

### Required overhaul

Garden Shade should feel like dappled green-gold light moving in a breeze.

### Required layers

- warm sunlight base
- muted green/gold leaf shade patches
- slow breeze drift
- soft contrast

### Strength mapping

At Strength 0:

- warm daylight with tiny green tint

At Strength 64:

- gentle leaf-shadow motion

At Strength 128:

- clear garden shade pattern

At Strength 192:

- stronger dappled contrast

At Strength 255:

- expressive leaf-shade movement
- still calm

### Parameters controlled

```text
shadePatchCount
shadeContrast
breezeSpeed
sunWarmth
leafTint
patchSoftness
```

### End result

Garden Shade should feel organic and daytime-friendly.

### Simulation proof

Show dappled patches drifting with strength changes.

---

## 32. Snowfall Hush

### Current role

Quiet cool-white drifting points.

### Required overhaul

Snowfall Hush should feel like snow or dust motes drifting softly.

### Required particle model

Each particle should have:

- position
- slow drift speed
- soft radius
- fade envelope
- brightness
- optional warm/cool mode from palette/color

Use fixed deterministic particles.

### Strength mapping

At Strength 0:

- few faint particles
- slow drift

At Strength 64:

- gentle snowfall

At Strength 128:

- visible hush snowfall

At Strength 192:

- more particles and brighter highlights

At Strength 255:

- denser snowfall
- still soft, not glittery

### Parameters controlled

```text
particleCount
particleSpeed
particleRadius
highlightBrightness
fadeLength
backgroundContrast
```

### End result

Snowfall Hush should be quiet, slow, and sleep-safe.

### Simulation proof

Show particle density and drift increasing with strength.

---

## 33. Observatory Glow

### Current role

Very slow celestial ambience.

### Required overhaul

Observatory Glow should feel like a dark sky with a slow moonlike glow and soft star gradients.

### Required layers

- dark celestial base
- broad moonlike glow
- sparse soft star gradients
- extremely slow movement

### Strength mapping

At Strength 0:

- mostly dark blue celestial glow
- very few stars

At Strength 64:

- gentle moon glow

At Strength 128:

- visible celestial pass and stars

At Strength 192:

- stronger glow and star detail

At Strength 255:

- richest celestial ambience
- still very slow

### Parameters controlled

```text
moonGlowRadius
moonGlowBrightness
starDensity
starSoftness
passSpeed
backgroundBrightness
```

### End result

Observatory Glow should be one of the slowest, calmest modes.

### Simulation proof

Show moon glow movement over long samples.

---

## 34. Rainy Window

### Current role

Moodier rain/glass mode.

### Required overhaul

Rainy Window should be more emotional and window-like than Rain Glass.

Rain Glass can be glintier; Rainy Window should be moodier, darker, and slower.

### Required droplet lifecycle

Each droplet:

- appears on pane
- bright bead forms
- trail extends downward/across strip motion direction
- bead fades
- trail lingers

### Strength mapping

At Strength 0:

- mostly cool pane glow
- rare faint droplets

At Strength 64:

- gentle rainy window trails

At Strength 128:

- clear rainy window ambience

At Strength 192:

- more trails and bead highlights

At Strength 255:

- most active rain pattern
- still soft and moody

### Parameters controlled

```text
dropletDensity
trailLength
fallSpeed
beadBrightness
paneGlow
coolness
```

### End result

Rainy Window should feel like looking at rain on a window at night.

### Simulation proof

Show bead/trail lifecycle and strength differences.

---

## 35. Campfire Blanket

### Current role

Restrained warm campfire ambience.

### Required overhaul

Campfire Blanket should feel warm and alive, but less flickery than a direct flame mode.

### Required layers

- warm blanket base
- broad ember waves
- subtle orange/red variation
- slow movement
- no harsh flicker

### Strength mapping

At Strength 0:

- steady warm blanket glow

At Strength 64:

- gentle ember warmth

At Strength 128:

- visible campfire blanket motion

At Strength 192:

- richer ember waves

At Strength 255:

- strongest cozy fire movement
- still soft

### Parameters controlled

```text
cozyDepth
emberContrast
movementAmount
waveSpeed
brightnessFloor
warmthSpread
```

### End result

Campfire Blanket should feel cozy and fire-adjacent without aggressive flame flicker.

### Simulation proof

Show warm waves increasing with strength.

---

## 36. Northern Whisper

### Current role

Simpler, sleep-safe aurora mode.

### Required overhaul

Northern Whisper should be the calm sibling of Quiet Northern Lights.

It may use simpler ribbon logic, but it should still feel like ribbons, not a generic sine wave.

### Required behavior

- fewer ribbons than Quiet Northern Lights
- softer opacity
- lower saturation
- slower movement
- sleep-safe brightness profile

### Strength mapping

At Strength 0:

- almost invisible color whisper

At Strength 64:

- one soft ribbon

At Strength 128:

- gentle aurora whisper

At Strength 192:

- two soft ribbons with visible folds

At Strength 255:

- strongest sleep-safe aurora whisper
- still calmer than Quiet Northern Lights

### Parameters controlled

```text
ribbonCount
ribbonOpacity
ribbonSoftness
colorSpread
foldDepth
speed
```

### End result

Northern Whisper should be sleep-safe, subtle, and aurora-like.

### Simulation proof

Show ribbon behavior and prove it remains calmer than Quiet Northern Lights.

---

## 37. Storm Outside

### Current role

Calm storm ambience.

### Required overhaul

Storm Outside should feel like a storm outside the window, not a lightning machine.

### Required layers

- deep blue-gray base
- slow rolling cloud waves
- faint rain shimmer
- occasional distant softened flash
- long fade after flash

### Strength mapping

At Strength 0:

- mostly storm-blue ambience
- no or extremely rare flash

At Strength 64:

- gentle cloud roll
- rare very soft flash

At Strength 128:

- clear storm mood
- distant flashes remain soft

At Strength 192:

- deeper storm motion
- slightly more flash probability/intensity

At Strength 255:

- strongest storm outside effect
- still not harsh lightning

### Parameters controlled

```text
stormDepth
cloudRollSpeed
rainShimmer
flashFrequency
flashSoftness
flashBrightness
postFlashFade
```

### Night Guard behavior

If Night Guard is enabled:

- heavily reduce flash brightness
- reduce flash frequency
- increase flash softness
- preserve storm ambience without harsh bursts

### End result

Storm Outside should feel moody and calm, not like Zeus found a relay board.

### Simulation proof

Show cloud motion and flash events across time/strength.

---

# Shared Implementation Rules for All Modes

## Rule 1 — No barely visible default animations

At `animationStrength = 128`, every animated mode should have a clearly visible identity.

A user should not need to stare for 45 seconds to tell whether anything is happening.

## Rule 2 — No generic sine-wave masquerading

A mode’s algorithm should match its name.

Examples:

- Rain modes need droplets/trails.
- Candle modes need candle cores/halos/flicker.
- Aurora modes need ribbons/folds/overlap.
- Storm mode needs cloud roll and distant soft flashes.
- Aquarium modes need watery caustics.
- Circuit mode needs trace pulses.

## Rule 3 — Keep calm modes calm

Even at strength 255, calm/sleep modes should not become harsh.

They may become more expressive, but not chaotic.

## Rule 4 — Utility modes are allowed to be utility modes

Strobe and Flash may be harsh, but must remain clearly marked and Night Guard-blocked.

## Rule 5 — Strength must be testable

Every animated mode must have simulation evidence that strength changes the output.

## Rule 6 — Preserve palette behavior

If a mode supports palettes now, preserve or improve that behavior.

If a mode does not support palettes, do not force palette behavior unless clearly useful and safe.

## Rule 7 — Preserve transitions

Changing mode algorithms must not break smooth transitions.

Transition rendering should still use frame buffers correctly.

## Rule 8 — Preserve performance discipline

Avoid:

- dynamic allocation per frame
- huge temporary arrays per frame
- repeated large `String` construction in animation path
- blocking delays
- repeated filesystem writes

Prefer:

- fixed arrays
- deterministic pseudo-random helpers
- small structs
- helper functions
- compile-time constants
- frame-local simple math

---

# Tests Required

Add/update tests to verify:

1. `animationStrength` exists in `ControllerSettings`.
2. Default `animationStrength` is 128.
3. Settings save includes `animationStrength`.
4. Settings load reads `animationStrength`.
5. `/api/state` includes `animationStrength` and percentage.
6. The animation API endpoint exists and validates values.
7. The web UI has one real Animation Strength slider.
8. The slider initializes from state.
9. The slider sends debounced updates.
10. Mode render functions use strength helpers or `settings.animationStrength`.
11. Every animated mode has a strength response in code or documented intentional exception.
12. Solid is intentionally stable.
13. Library Lamp and Solar Noon remain practical despite strength.
14. Strobe/Flash remain Night Guard-blocked.
15. Quiet Northern Lights uses ribbon/lifecycle logic, not only simple sine layers.
16. Rain Glass and Rainy Window use droplet/trail lifecycle logic.
17. Candle Hall uses smoothed flicker/halo logic.
18. Starlit Linen and Snowfall Hush use soft neighboring-pixel particles/highlights.
19. The simulator script exists.
20. The simulator generates contact sheets/report files.
21. The simulator produces metrics for every mode.
22. The mode overhaul report exists.
23. No normal mode directly calls `strip.show()`.
24. The central output path still applies RGB gains, effective brightness, and gamma.
25. Existing route names remain present.
26. Compile succeeds for D1 mini if the environment supports it.
27. Python tests pass.

Run:

```text
python -m unittest discover -s tests -v
```

Compile:

```text
arduino-cli compile --fqbn esp8266:esp8266:d1_mini firmware/BedroomLedController
```

If compile cannot run, say so clearly and run available static tests.

---

# Documentation Requirements

Create or update:

```text
docs/generated/BedroomLedController_Mode_Overhaul_Report.md
```

The report must include:

1. Files changed.
2. Current mode inventory.
3. Animation Strength architecture.
4. Persistence/API/UI changes.
5. Browser preview changes.
6. Simulation tool details.
7. Simulation output paths.
8. Strength-level proof summary.
9. Mode-by-mode implementation notes.
10. Known limitations.
11. Resource impact.
12. Compile/test status.
13. Physical LED test status.
14. Recommended real-world test plan.

Update API docs if endpoint added.

Update README/user guide if needed.

Do not claim physical LED testing unless actual LEDs were observed.

Do not claim simulation equals physical proof.

---

# Required Final Codex Response Format

When finished, respond with:

```text
Mode animation overhaul complete.

Files changed:
- ...

Animation Strength:
- Setting:
- Persistence:
- API:
- UI:
- Preview:

Simulation proof:
- Tool added:
- Output files:
- Strength levels tested:
- Modes simulated:
- Metrics generated:
- Limitations:

Mode-by-mode changes:
- Solid:
- Strobe:
- Flash:
- Chase:
- Wave:
- Slow Wave:
- Slow Pulse:
- Rainbow:
- Dawn Bloom:
- Moon Tide:
- Velvet Aurora:
- Lantern Drift:
- Rain Glass:
- Satin Breathing:
- Paper Lantern Row:
- Library Lamp:
- Solar Noon:
- Golden Hour:
- Dream Aquarium:
- Circuit Glow:
- Window Sunrise:
- Ember Quilt:
- Porcelain Sky:
- Hearthline:
- Evening Current:
- Starlit Linen:
- Deep Sea Glass:
- Quiet Northern Lights:
- Candle Hall:
- Slow Prism:
- Garden Shade:
- Snowfall Hush:
- Observatory Glow:
- Rainy Window:
- Campfire Blanket:
- Northern Whisper:
- Storm Outside:

Preserved:
- Central output pipeline:
- OTA:
- Existing routes:
- Scenes/favorites/palettes:
- Timers/bedtime fade:
- Schedules:
- Night Guard:
- Transitions:
- Diagnostics:

Tests:
- Python tests:
- Compile:

Resource status:
- RAM globals/statics:
- IRAM:
- Flash/IROM:
- Runtime heap if measured:
- LittleFS if measured:

Physical LED test status:
- ...

Known limitations:
- ...

Recommended next step:
- Upload over USB first.
- Open diagnostics and confirm heap/resource metrics.
- Test Animation Strength at 0, 64, 128, 192, and 255 on real LEDs.
- Specifically inspect Quiet Northern Lights, Rain Glass, Candle Hall, Starlit Linen, Snowfall Hush, and Storm Outside.
- Tune any modes that still look too subtle, too blob-like, or too generic.
```

Be honest and specific.

Do not write “physically tested” unless physical LED testing happened.

Do not leave Animation Strength as a fake slider.

Do not make every mode a renamed wave.

Do not break the beautiful parts while fixing the weak parts.

