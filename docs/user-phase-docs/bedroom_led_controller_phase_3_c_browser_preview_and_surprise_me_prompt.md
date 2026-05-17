# Bedroom LED Controller — Phase 3C Codex Prompt

## Phase 3C Title

**Browser-Side Live Preview Strip + Surprise Me Generator**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 3C** of the larger upgrade plan.

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

The goal of Phase 3C is to add:

1. A **browser-side live preview strip** that approximates the active lighting state without streaming LED frames from the ESP8266.
2. A **Random but Restrained / Surprise Me generator** that produces tasteful lighting combinations using existing modes, scenes, palettes, tags, Night Guard, brightness limits, and safety rules.

This phase should make the UI feel much more alive and playful while staying disciplined enough for the D1 mini.

In simple terms: the browser gets a tiny pretend LED strip, and the firmware gets a tasteful vibe dice button. The D1 mini does **not** become a Netflix server with RGB opinions.

---

# Critical Board / Resource Context From D1 Mini vs FireBeetle Discussion

The project should remain **D1 mini / ESP8266 first** unless resource measurements prove it cannot continue safely.

Important conclusions that must shape this phase:

- The FireBeetle ESP32-S3 N4 is technically stronger, especially for RAM, CPU, responsiveness, JSON parsing, UI complexity, and future polish.
- The FireBeetle ESP32-S3 N4 does **not** magically solve storage because it still has 4 MB flash, similar to a typical D1 mini.
- The D1 mini can probably continue through Phase 3 if the implementation is compact and disciplined.
- The biggest D1 mini risks are free heap, compiled sketch size, web UI bloat, JSON parsing, and OTA slot margin.
- Browser preview must run in the browser. The ESP8266 must not stream animation frames.
- The ESP8266 should expose compact state/metadata only.
- Avoid heavy frameworks, giant JSON, heap-fragmenting `String` chaos, and repeated dynamic allocation.
- Require a resource report after this phase.

## Required Resource Report Addition

Every Phase 3C report must include:

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
- Free heap after preview UI loads:
- Free heap after Surprise Me API/action:
- OTA still works: yes/no/not tested
- Web UI responsiveness: snappy/acceptable/laggy/not tested
- Recommendation: stay on D1 mini / start worrying / consider ESP32-S3
```

Do not fake these numbers. If a value cannot be measured, say `not measured`.

## D1 Mini Stay/Switch Guidance

Use this guidance in the final report:

```text
Stay on D1 mini:
- Sketch size under about 75% of OTA-safe max
- Free heap after boot above about 25 KB
- Free heap while using UI/API above about 20 KB
- OTA has comfortable margin
- Web UI is responsive

Start worrying:
- Sketch size around 75–85%
- Free heap after boot around 18–25 KB
- Free heap while using UI/API around 12–20 KB
- Web UI gets noticeably laggy

Switch strongly recommended:
- Sketch size above about 85–90%
- Free heap after boot under about 15–18 KB
- Free heap while using UI/API under about 12 KB
- Random resets/watchdog issues
- OTA starts failing
- Web UI becomes painful
```

This phase should be implemented to keep the D1 mini viable.

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

Phase 3C may add preview/surprise routes or metadata, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. Browser-side live preview strip
2. Preview JavaScript animation approximations
3. Preview data/state integration
4. Preview performance/resource guardrails
5. Surprise Me generator state model
6. Surprise Me categories/moods
7. Safe random selection rules
8. Surprise Me UI controls
9. Surprise Me API endpoints
10. Surprise-generated temporary state application
11. Optional save-generated-scene behavior
12. Integration with mode tags, palettes, scenes, Night Guard, transitions, timers, and power estimates
13. Resource report instrumentation/checklist
14. Documentation/report update
15. Compile/test report

Do **not** implement yet:

- New lighting mode algorithms
- First new lighting mode batch
- Full weekly scheduler
- QR/NFC scene links
- Physical controls
- Microphone support
- Light sensor support
- New hardware assumptions
- ESP32-only features
- WebSocket LED frame streaming
- Server-sent LED preview frames
- Heavy frontend framework

If a later feature seems tempting, document it as deferred. The New Mode Goblin is still outside. It may look cold. Do not let it in yet.

---

# Phase 3C Main Goal

At the end of Phase 3C, the user should be able to:

1. See a miniature preview strip in the browser.
2. See approximate animation motion, speed, direction, color, palette, and brightness behavior.
3. Use preview without the ESP8266 streaming frames.
4. Keep the ESP8266 responsive while preview is running.
5. Click **Surprise Me** to generate tasteful lighting combinations.
6. Choose Surprise Me mood categories such as Calm, Evening, Sleep-safe, Focus, Colorful, and Cozy.
7. Trust that Surprise Me will not pick Strobe/Flash or unsafe full-bright chaos.
8. Have Surprise Me respect Night Guard, power limits, mode metadata, palette tags, and brightness caps.
9. Optionally save a generated result as a scene.
10. Keep existing scenes, palettes, transitions, timers, Night Guard, diagnostics, OTA, and mode controls working.

---

# Part A — Browser-Side Live Preview Strip

---

# 1. Preview Principle: Browser-Side Only

## Goal

Add a live preview that runs in the browser, not on the ESP8266.

## Required Behavior

The preview must be drawn by browser JavaScript using state and metadata from existing compact endpoints.

The ESP8266 may provide:

- Current mode
- Active color
- White temperature
- Brightness/effective brightness
- Active palette ID/colors
- Mode speed/settings
- Transition state summary
- Timer state summary
- Night Guard status

The ESP8266 must **not** provide:

- Full LED frame stream
- Per-pixel animation frames every frame
- WebSocket LED buffer stream
- Server-sent animation frames
- High-frequency preview data

## Why

The D1 mini’s danger zones are heap, compiled size, web UI bloat, and JSON parsing. Browser-side preview keeps the microcontroller doing microcontroller things instead of forcing it to become a tiny RGB Twitch server.

## Acceptance Criteria

- Preview is computed in browser JavaScript.
- ESP8266 only serves compact state/metadata.
- No server-side frame streaming exists.
- OTA/web UI remain responsive.

---

# 2. Preview UI Placement

## Goal

Make the preview visible and useful without overwhelming the main page.

## Recommended Placement

Add a section near the current state summary or mode controls:

```text
Live Preview
```

Suggested order:

1. Quick Actions
2. Current State Summary
3. Live Preview
4. Brightness / Color / Palette
5. Mode Controls
6. Scenes / Timers / Night Guard / Advanced

## Required UI Elements

The preview section should include:

- Mini LED strip preview
- Current preview mode label
- Preview status text
- Optional pause/resume preview toggle
- Optional reduced-motion note

Example text:

```text
Preview: approximate browser-side visualization. The real LED strip may differ slightly.
```

## Preview Size

Keep the preview lightweight.

Recommended:

- 30–60 preview pixels
- Canvas width responsive to page width
- Height around 24–60 px

Do not render hundreds of DOM elements if a canvas is cleaner.

## Canvas vs Divs

Preferred:

- Use a lightweight `<canvas>`.

Acceptable:

- Use simple HTML blocks/divs if easier, but keep count low.

Canvas is recommended because it avoids creating many DOM elements.

## Acceptance Criteria

- Preview appears on the main UI.
- It is clearly labeled as approximate.
- It is responsive/mobile-friendly.
- It does not bloat the page dramatically.

---

# 3. Preview Data Sources

## Goal

Use existing compact endpoints wherever possible.

## Preferred Data Sources

Use:

- `/api/state`
- `/api/modes`
- `/api/palettes`
- `/api/timer`, only if timer details are needed
- `/api/transitions`, only if transition details are needed

Do not create large new endpoints unless needed.

## Optional New Endpoint

If combining state for preview reduces browser requests and JSON overhead, add a compact endpoint:

```text
/api/preview-state
```

But it must remain small.

Suggested payload:

```json
{
  "ok": true,
  "mode": "Moon Tide",
  "modeId": "moon-tide",
  "colorHex": "#88AAFF",
  "whiteKelvin": 2700,
  "usingWhiteTemperature": false,
  "effectiveBrightness": 80,
  "masterBrightness": 120,
  "paletteEnabled": true,
  "activePaletteId": "moonlight",
  "paletteColors": ["#DDEBFF", "#8FB8FF", "#405C9A"],
  "modeSpeed": 15,
  "timerActive": false,
  "transitionActive": false,
  "nightGuardEnabled": false
}
```

If `/api/state` already provides enough data and `/api/palettes` can provide palette colors, do not add this endpoint.

## Polling Rate

Do not poll excessively.

Recommended:

- Fetch state every 1–3 seconds.
- Fetch `/api/modes` and `/api/palettes` once on load, then refresh only after changes.
- Refresh palette list after palette edit/save/delete.

Preview animation should run locally using `requestAnimationFrame` or a timed loop.

## Acceptance Criteria

- Preview initializes from current state.
- Preview updates when mode/color/palette changes.
- Polling does not hammer the ESP8266.
- JSON remains compact.

---

# 4. Preview Animation Loop

## Goal

Animate preview smoothly in the browser while keeping CPU/battery reasonable.

## Required Behavior

Use browser-side timing:

```javascript
requestAnimationFrame(drawPreview)
```

or a lightweight interval.

Recommended:

- Use `requestAnimationFrame` when preview is visible.
- Stop or throttle when page is hidden using `document.visibilityState`.
- Respect reduced-motion setting if it exists.

## Frame Rate

Recommended:

- Browser preview can draw around 30 FPS.
- Full 60 FPS is not required.
- If performance is poor, throttle to 15–30 FPS.

## Pause Behavior

Optional but useful:

- Add “Pause Preview” toggle.
- Pause when tab is hidden.

## Acceptance Criteria

- Preview animates without ESP8266 frame streaming.
- Preview does not make UI sluggish.
- Preview pauses/throttles when hidden if implemented.
- Reduced-motion preference is respected if available.

---

# 5. Preview Pixel Model

## Goal

Create a small browser-side pixel array and draw it to canvas.

## Required Behavior

Use a preview pixel count independent of actual LED count.

Recommended:

```javascript
const PREVIEW_PIXELS = 48;
```

or dynamically:

```javascript
previewPixels = Math.min(actualLedCount || 60, 60);
```

Do not render thousands of preview pixels.

## Preview Output

Each preview pixel should have:

- r
- g
- b
- brightness scaling

Draw as:

- Rounded small blocks, or
- Continuous gradient-like strip, or
- Rectangles with slight spacing

Keep it tasteful.

## Acceptance Criteria

- Preview uses a bounded pixel count.
- Drawing is browser-side.
- It visually resembles an LED strip.

---

# 6. Preview Color / Palette Helpers

## Goal

Mirror enough of the firmware color logic in browser JavaScript for preview purposes.

## Required Helpers

Implement lightweight JS helpers:

```javascript
hexToRgb(hex)
rgbToCss(rgb)
scaleRgb(rgb, brightness)
blendRgb(a, b, t)
samplePalette(colors, position)
kelvinToApproxRgb(kelvin)
```

The preview does not need scientific color accuracy.

It should be useful and visually plausible.

## White Temperature Approximation

Use a simple approximation or a small lookup/interpolation for 2700K–6500K.

The firmware remains the source of truth for actual LEDs.

## Palette Handling

If palette is enabled and current mode supports palette:

- Use active palette colors in preview.

If no valid palette:

- Fall back to active color.

## Acceptance Criteria

- Preview can show active RGB color.
- Preview can show white temperature.
- Preview can show palette-based modes.
- Invalid palette/color values fail safely.

---

# 7. Preview Mode Approximations

## Goal

Approximate existing modes in the browser.

The preview does not need to perfectly match firmware. It needs to communicate color, motion, rough speed, direction, and brightness envelope.

## Required Approximation Coverage

Add preview approximations for existing modes if present:

- Solid Color
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

If a mode does not exist in the actual sketch, do not pretend it does.

## Approximation Guidance

### Solid Color

Fill preview with active color/white temperature.

### Strobe

Show full on/off blinking based on configured strobe delay.

If Night Guard blocks Strobe, show blocked indicator or preview as disabled.

### Flash

Show brief on state and longer off state based on configured delay.

### Chase

Show a moving bright crest over dark/low background.

### Wave

Show smoother sine-wave brightness motion with floor brightness.

### Slow Wave

Show broad slow moving wave layers.

### Slow Pulse

Show several soft pulsing islands across strip.

Use deterministic pseudo-random centers so preview does not jump randomly every frame.

### Rainbow

Show moving hue gradient.

### Dawn Bloom

Show warm sweeping glow with gentle breathing.

### Moon Tide

Show cool layered slow wave using active color or Moonlight palette.

### Velvet Aurora

Show soft ribbons using palette colors if palette enabled.

### Lantern Drift

Show wide warm drifting pools.

### Rain Glass

Show soft beads/trails with low base glow.

## Unknown Mode Fallback

If current mode has no preview renderer:

- Show solid active color.
- Display note:

```text
Preview fallback: current mode has no dedicated preview approximation yet.
```

## Acceptance Criteria

- Major existing modes have usable approximations.
- Unknown modes fall back safely.
- Preview does not require firmware changes for every animation detail.

---

# 8. Preview Transition Awareness

## Goal

Make preview cooperate with Phase 3A transitions without duplicating the full firmware transition engine.

## Required Behavior

If `/api/state` or `/api/transitions` says a transition is active:

- Optionally show a small transition status label.
- Preview may approximate transition progress by fading locally between last preview state and new preview state.

## Recommended First Version

Do not overbuild.

Acceptable behavior:

- Preview simply updates to new state.
- Show transition progress text if available.

Better behavior:

- Browser preview crossfades between previous preview state and new preview state over transition duration.

## Important Rule

Do not request frame data from ESP8266 to match transitions perfectly.

## Acceptance Criteria

- Preview does not conflict with transitions.
- If transition status is shown, it is compact and accurate enough.
- No server-side transition frames are streamed.

---

# 9. Preview Interaction With Timers / Bedtime Fade

## Goal

Show timer/fade status without implementing server-side preview frames.

## Required Behavior

If timer or bedtime fade is active:

- Show remaining time or progress in preview/status area.
- Preview may approximate fade by applying current state from `/api/state`.

If detailed timer fade state exists:

- Use it to dim preview brightness locally.

If not:

- Display timer status text only.

## Acceptance Criteria

- Timer status remains visible.
- Preview does not break bedtime fade behavior.
- No additional high-frequency polling required.

---

# 10. Preview Reduced-Motion / Low-Glare Integration

## Goal

Respect UI comfort settings from Phase 2D if implemented.

## Required Behavior

If UI reduced-motion is enabled:

- Pause or slow preview animation.
- Show static representative frame.

If low-glare UI mode exists:

- Keep preview section visually low-glare.
- Avoid bright white backgrounds around preview.

If these settings do not exist:

- Keep preview dark/low-glare by default.
- Document that reduced-motion integration is deferred.

## Acceptance Criteria

- Preview is not visually obnoxious.
- Reduced-motion setting is respected if available.

---

# Part B — Random but Restrained / Surprise Me Generator

---

# 11. Surprise Me Core Philosophy

## Goal

Add a tasteful random scene/state generator.

This should not be pure chaos.

It should generate combinations that feel intentional, cozy, and safe.

## Hard Rules

Surprise Me must:

- Never select Strobe.
- Never select Flash.
- Never select modes tagged flashing or blocked by Night Guard.
- Respect Night Guard.
- Respect configured brightness/power safety caps.
- Avoid full-bright white unless the selected mood explicitly allows bright focus/workbench behavior.
- Prefer calm modes for default surprise behavior.
- Prefer palettes tagged calm/evening/sleep-safe for calm moods.
- Avoid hidden hardware assumptions.
- Use existing modes only.
- Use existing palettes only.
- Use existing scene/palette/mode APIs/state helpers where practical.

This button should produce “oh that looks nice,” not “the Las Vegas printer jam has become sentient.”

## Acceptance Criteria

- Surprise Me generates safe combinations.
- Flashing modes are excluded.
- Night Guard is respected.
- Generated results apply through normal state/output/transition paths.

---

# 12. Surprise Mood Categories

## Goal

Allow user to choose the flavor of Surprise Me.

## Required Moods

Support at least:

1. Calm
2. Evening
3. Sleep-safe
4. Focus
5. Colorful
6. Cozy

Optional:

7. Weather / Nature
8. Experimental, still restrained

## Mood Rules

### Calm

- Prefer calm modes.
- Medium-low brightness.
- Soft palettes.
- Avoid high saturation unless palette is gentle.

### Evening

- Warm or cool calm modes.
- Lower brightness.
- Prefer Candle, Warm Linen, Moonlight, Peach Sky, Storm Blue.

### Sleep-safe

- Only sleep-safe modes/scenes/palettes.
- Very low brightness.
- No flashing.
- Prefer warm/dim settings.

### Focus

- Prefer Solid or focus-friendly modes.
- Neutral/cool white or soft stable palette.
- Higher brightness, but respect caps.
- Avoid busy animations.

### Colorful

- Allow colorful palettes and modes.
- Still avoid flashing.
- Brightness moderate.
- Rainbow may be allowed if not Night Guard and speed/brightness are safe.

### Cozy

- Prefer warm palettes.
- Prefer warm dim scenes/modes.
- Low-medium brightness.
- Good candidates: Lantern Drift, Moon Tide with warm palette if appropriate, Solid warm, Velvet Aurora warm palette.

## Acceptance Criteria

- User can choose a Surprise mood.
- Each mood has distinct safe selection rules.
- Default Surprise uses Calm if no mood is selected.

---

# 13. Surprise Candidate Selection

## Goal

Generate a candidate from existing metadata.

## Required Inputs

Use available metadata from:

- Mode metadata/tags
- Scene metadata/tags, if available
- Palette metadata/tags
- Night Guard state
- Power estimate/recommended brightness cap
- Current master brightness/effective brightness
- Existing scene list
- Existing palette list

## Candidate Types

Surprise Me may generate one of:

### Direct State Candidate

A generated combination of:

- Mode
- Color or white temperature
- Palette enable/disable
- Palette ID
- Master brightness
- Mode-specific settings if safe/simple

### Scene-Based Candidate

Choose an existing scene that matches the mood.

### Hybrid Candidate

Choose a mode + palette + brightness and apply it as current state without saving as scene.

Recommended first implementation:

- Hybrid candidate using mode + palette/color + brightness.
- Optionally allow “choose from existing scenes” for simple safe cases.

## Candidate Filtering

Always filter out:

- Strobe
- Flash
- Modes tagged flashing
- Modes blocked by Night Guard
- Modes unavailable in actual sketch
- Palettes that are invalid/missing
- Scenes using blocked modes if Night Guard is active

## Acceptance Criteria

- Candidate list is generated from real available modes/palettes/scenes.
- No fake modes are selected.
- Blocked/unsafe candidates are excluded.

---

# 14. Surprise Randomness Model

## Goal

Use lightweight randomness suitable for ESP8266 and reproducible enough for debugging.

## Options

### Firmware-Side Random Generation

Use `random()` seeded with analog/noise/time if already available.

Pros:

- API can generate and apply state.
- Easy to use from any client.

Cons:

- More firmware logic.

### Browser-Side Candidate Generation

Browser fetches modes/palettes/state, generates candidate, then applies via existing APIs.

Pros:

- Less firmware burden.
- Better for D1 mini constraints.

Cons:

- Must keep browser logic consistent.

## Recommended for D1 Mini

Use a **hybrid** approach:

- Firmware exposes a compact `/api/surprise` endpoint for simple generation/apply.
- The browser UI may optionally preview candidate metadata before applying.
- Keep generator logic lightweight.

Alternative acceptable:

- Implement generator mostly in browser using existing APIs, with no new firmware route except optional action endpoint.

## Important Rule

Do not store huge candidate tables in RAM.

Use compact arrays, mode metadata, palette metadata, and simple weighted selection.

## Acceptance Criteria

- Surprise generation is lightweight.
- Randomness is good enough for user-facing variety.
- Debug/report can describe how candidates are selected.

---

# 15. Surprise Brightness / Power Safety Rules

## Goal

Keep generated states power-safe and comfortable.

## Required Behavior

Surprise Me should choose brightness based on mood and safety caps.

Suggested mood brightness ranges:

```text
Sleep-safe: 10–45
Cozy: 25–80
Evening: 40–110
Calm: 50–140
Focus: 120–190, capped by power estimate/Night Guard
Colorful: 60–150
```

Always cap by:

- Night Guard max brightness if Night Guard is enabled
- Power estimator recommended brightness cap if available
- Global maximum safe brightness setting, if implemented

If the recommended power cap is unavailable:

- Use conservative defaults.

## Full White Rule

Avoid generating full-bright white.

For Focus mode, white/neutral may be generated, but brightness should still respect power and comfort caps.

## Acceptance Criteria

- Surprise never exceeds Night Guard effective cap.
- Surprise respects power cap if available.
- Sleep-safe surprise stays dim.
- Focus surprise can be brighter but bounded.

---

# 16. Surprise Mode-Specific Settings

## Goal

Optionally generate safe mode-specific settings.

## Required Minimum

Mode-specific settings can be left unchanged or set to safe defaults.

## Useful Safe Defaults

Examples:

### Slow Pulse

- Pulse count: low/medium
- Timing: slow

### Slow Wave / Wave

- Speed: moderate/slow

### Rainbow

- Only allowed in Colorful mood
- Speed: slow/moderate, not chaotic
- Brightness capped

### Rain Glass / Moon Tide / Lantern Drift / Velvet Aurora

- Use calm speeds
- Use matching palette

## Important Rule

Do not over-randomize.

A few tasteful parameter choices are better than every setting becoming a slot machine.

## Acceptance Criteria

- Generated mode settings are safe and not obnoxious.
- Existing mode controls still work after Surprise Me.
- Invalid settings are not generated.

---

# 17. Surprise Apply Behavior

## Goal

Apply generated result through existing safe state paths.

## Required Behavior

When Surprise Me is applied:

1. Generate candidate.
2. Validate candidate.
3. Cancel diagnostic test if active, or reject with useful error.
4. Cancel or resolve active timer/fade according to existing manual-change behavior.
5. Apply mode/color/palette/brightness through central mutation functions.
6. Respect Night Guard and transitions.
7. Start smooth transition if enabled.
8. Mark settings dirty once if current state changes.
9. Update UI state.

## Transition Integration

If transitions are enabled:

- Surprise Me should transition into generated state.

If current timer fade is active:

- Recommended: cancel timer/fade before applying Surprise Me.

Document behavior.

## Acceptance Criteria

- Surprise applies generated state correctly.
- Surprise does not bypass transitions/output pipeline.
- Surprise does not spam settings writes.
- UI reflects generated state.

---

# 18. Surprise UI Controls

## Goal

Add a friendly UI for generating and applying surprises.

## Required UI Elements

Add a section or button near quick actions / current state:

```text
Surprise Me
```

Controls:

- Mood selector
- Surprise Me button
- Optional “Apply” button if candidate preview is generated first
- Optional “Save as Scene” button after applying
- Status/error message

## Recommended Simple Flow

For Phase 3C:

1. User selects mood.
2. User taps Surprise Me.
3. Generated state applies immediately with transition.
4. UI shows generated summary.
5. User may optionally save current state as scene using existing scene save system.

## Better Optional Flow

1. User taps Generate.
2. Candidate summary appears.
3. User taps Apply or Generate Again.
4. User may Save as Scene.

This is nicer but not required.

## Candidate Summary

Show:

- Mood
- Mode
- Palette or color
- Brightness
- Tags/safety notes

Example:

```text
Generated: Calm · Moon Tide · Moonlight palette · 72 / 255 brightness
```

## Acceptance Criteria

- Surprise Me UI is easy to use.
- Mood selector works.
- Generated summary appears.
- Errors are visible.
- Save-as-scene reuse is available if practical.

---

# 19. Surprise API Endpoints

## Goal

Expose Surprise Me behavior through local API endpoints.

## Required Endpoint

Implement one of these approaches.

### Option A — Firmware Generator Endpoint

```text
/api/surprise?mood=calm&apply=1
```

If `apply=0`, return generated candidate without applying.

Example response:

```json
{
  "ok": true,
  "applied": true,
  "mood": "calm",
  "mode": "Moon Tide",
  "paletteEnabled": true,
  "paletteId": "moonlight",
  "brightness": 72,
  "message": "Generated calm lighting state"
}
```

### Option B — Browser Generator Only

No firmware surprise route. Browser uses existing APIs to apply candidate.

If this option is chosen, document clearly.

### Recommended

Use Option A if implementation remains compact.

If firmware size/heap is already worrying, use Option B to keep D1 mini healthier.

## Optional Supporting Endpoint

```text
/api/surprise/options
```

Returns available moods and safety rules.

Not required if UI hardcodes mood list lightly.

## Validation

- Unknown mood returns JSON error.
- If no safe candidate exists, return JSON error.
- If Night Guard blocks all candidates for a mood, return useful message.

## Acceptance Criteria

- Surprise can be triggered from API or documented browser flow.
- Invalid mood returns error.
- No safe candidate returns error.
- Apply behavior uses existing safe state paths.

---

# 20. Save Surprise Result As Scene

## Goal

Let the user save a generated state as a scene without duplicating scene logic.

## Required Behavior

After Surprise Me applies a result:

- User can use existing scene save UI/API to save current state.

Optional convenience:

- Add a “Save Surprise as Scene” button that calls existing `/api/scenes/save?name=...`.

## Suggested Default Name

Use names like:

```text
Calm Surprise
Evening Surprise
Cozy Surprise
```

If duplicate, existing scene name collision rules apply.

## Important Rule

Do not create a separate surprise scene storage system.

Use the existing scene preset system.

## Acceptance Criteria

- Generated state can be saved as scene.
- Scene save uses existing scene logic.
- Palette references are saved if Phase 3B scene palette support exists.

---

# 21. Surprise and Palette Integration

## Goal

Use palettes intelligently.

## Required Behavior

Surprise Me should choose palette based on mood:

### Calm

- Prefer calm palettes.

### Evening / Cozy

- Prefer warm/calm palettes like Candle, Warm Linen, Peach Sky.

### Sleep-safe

- Prefer sleep-safe/warm/calm palettes.

### Focus

- Prefer neutral or low-saturation palettes, or white temperature instead of palette.

### Colorful

- Allow colorful palettes like Aurora, Soft Neon, Circuit Glow, but cap brightness.

## Palette Enable Rule

If generated mode supports palette:

- Enable palette and select matching palette.

If generated mode does not support palette:

- Disable palette or leave palette state unchanged, but use active RGB/white color.

Recommended:

- For generated states, set paletteEnabled based on whether the chosen mode supports palette.

## Acceptance Criteria

- Palette choices match mood.
- Unsupported modes do not confuse user.
- Palette scene references continue working.

---

# 22. Surprise and Existing Scenes

## Goal

Optionally use existing scenes as candidate sources.

## Required Minimum

Surprise can ignore existing scenes and generate direct mode/palette/color settings.

## Optional Scene Candidate Behavior

For mood categories, existing scenes can be candidates if they have metadata/tags or names that match.

Examples:

- Sleep-safe mood can choose Sleep or Cozy Goblin Mode.
- Focus mood can choose Study / Focus or Workbench.
- Evening mood can choose Movie, Moon Tide, Lantern Drift.

## Safety Rules

If a scene uses blocked mode or brightness above cap:

- Reject or cap safely.
- Do not modify the saved scene data unless user explicitly saves edits.

## Acceptance Criteria If Implemented

- Scene candidates are filtered safely.
- Saved scenes are not modified by Surprise Me.

---

# 23. Surprise and Diagnostics / Power Estimate

## Goal

Respect power estimates without pretending the controller measures real current.

## Required Behavior

If `/api/power` or internal power estimate is available:

- Use recommended brightness cap as upper bound.
- If unavailable, use conservative bounds.

Do not claim actual current measurement.

## UI Message

If brightness was capped by power estimate, show something like:

```text
Brightness capped by configured power estimate.
```

## Acceptance Criteria

- Surprise respects power cap.
- UI explains cap when applicable.
- No claim of measured current.

---

# 24. Preview and Surprise Performance Requirements

## Goal

Prevent Phase 3C from becoming the phase that murders the D1 mini.

## Firmware Requirements

Avoid:

- Giant JSON responses
- Heavy JavaScript served inline if it makes sketch size explode
- Rebuilding large strings frequently
- Dynamic allocation in loops
- Parsing palette/scene files during every Surprise request if already loaded in memory
- High-frequency preview polling
- WebSocket/SSE streaming

Prefer:

- Compact JSON
- Existing endpoints
- Static PROGMEM HTML/JS strings if appropriate
- Browser-side computation
- Cached mode/palette metadata in browser
- Small candidate arrays
- Simple weighted random logic

## Browser Requirements

Avoid:

- Heavy frontend frameworks
- Huge DOM strip of hundreds of nodes
- 60 FPS if 30 FPS looks fine
- Polling APIs every animation frame

Prefer:

- Canvas
- `requestAnimationFrame`
- Poll state every 1–3 seconds
- Pause when hidden
- Compact preview pixel count

## Acceptance Criteria

- Free heap remains healthy.
- Web UI remains responsive.
- OTA remains possible.
- Report includes resource measurements or honest “not measured” entries.

---

# 25. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_3C_Report.md
```

The report should include:

1. Files changed.
2. Browser preview architecture.
3. Preview rendering method: canvas/divs/etc.
4. Preview data sources and polling rate.
5. Modes with dedicated preview approximations.
6. Unknown-mode preview fallback behavior.
7. Surprise Me generator architecture.
8. Surprise moods and safety rules.
9. Candidate filtering behavior.
10. Palette integration.
11. Scene/favorite/timer/transition/Night Guard interaction.
12. Power cap behavior.
13. D1 mini resource report.
14. Whether FireBeetle/ESP32 migration is recommended yet.
15. Compile/test status.
16. Known limitations.
17. What was intentionally deferred.
18. Recommended next step: Phase 3D first new lighting mode batch.

## Required Honesty

Do not claim:

- Perfect LED preview accuracy.
- Server-side preview streaming.
- New lighting modes.
- Weekly scheduler.
- QR/NFC links.
- Hardware controls.
- Microphone behavior.
- Light sensor behavior.
- Actual current measurement.
- Physical LED testing unless actual LED testing happened.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

If resource values were not measured, say `not measured`.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Current UI/API/Metadata

Identify:

- Current main page structure
- Current `/api/state` fields
- Current `/api/modes` metadata
- Current `/api/palettes` payload
- Current scene/favorite/timer/Night Guard APIs
- Current transition state fields
- Current mode-specific settings available to browser

Do not rewrite the whole UI.

## Step 2 — Decide Preview Data Strategy

Use existing endpoints if enough.

Only add `/api/preview-state` if it reduces total complexity and remains compact.

## Step 3 — Add Preview UI Container

Add a lightweight Live Preview section with canvas and status text.

## Step 4 — Add Browser Preview Helpers

Add JavaScript helpers:

- Color parsing
- Brightness scaling
- Palette sampling
- Kelvin approximation
- Mode renderer dispatch
- Canvas drawing

## Step 5 — Add Preview Renderers

Implement approximations for:

1. Solid Color
2. Chase/Wave
3. Rainbow
4. Slow Pulse
5. Rain Glass / Moon Tide / Lantern Drift / Velvet Aurora if available
6. Fallback renderer

Add other modes as practical.

## Step 6 — Add Preview Polling/Refresh

Poll state slowly.

Fetch modes/palettes on load and after relevant edits.

Pause/throttle when hidden if practical.

## Step 7 — Add Surprise Me Candidate Rules

Implement mood list and filtering rules.

Decide firmware-side, browser-side, or hybrid generation.

## Step 8 — Add Surprise API or Browser Apply Flow

If firmware-side:

- Add `/api/surprise?mood=...&apply=...`

If browser-side:

- Use existing APIs to apply generated state.
- Document no dedicated firmware endpoint.

## Step 9 — Add Surprise UI

Add mood selector, button, generated summary, and optional save-as-scene helper.

## Step 10 — Integrate Safety Rules

Ensure Surprise respects:

- Night Guard
- Power cap
- Mode flashing metadata
- Palette validity
- Transition behavior
- Timer/fade conflict rules

## Step 11 — Add State/Diagnostics Notes If Needed

Add compact preview/surprise status only if useful.

Do not bloat `/api/state`.

## Step 12 — Measure / Report Resources

Compile and collect resource report fields where possible.

## Step 13 — Create Phase 3C Report

Create/update `BedroomLedController_Phase_3C_Report.md`.

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
- Scene/favorite/timer/Night Guard/transition/palette routes still work.
- `/api/modes` still works.

## Live Preview UI

- Preview section appears.
- Preview is labeled approximate.
- Preview initializes from current state.
- Preview updates after mode change.
- Preview updates after color change.
- Preview updates after white temperature change.
- Preview updates after palette selection.
- Preview uses browser-side rendering.
- Preview does not stream frames from ESP8266.
- Preview remains responsive on mobile.

## Preview Renderers

- Solid preview works.
- Chase/Wave preview works.
- Rainbow preview works.
- Slow Pulse preview works.
- At least several existing calm modes have useful approximations.
- Unknown mode fallback works.
- Palette-enabled preview works for compatible mode.
- Reduced-motion behavior works if implemented.

## Preview Performance

- State polling is not excessive.
- No high-frequency firmware requests.
- Page hidden throttling works if implemented.
- UI remains responsive.

## Surprise Me UI/API

- Mood selector appears.
- Surprise Me button works.
- Calm mood generates safe calm state.
- Evening mood generates warm/calm state.
- Sleep-safe mood generates dim safe state.
- Focus mood generates stable brighter state within caps.
- Colorful mood avoids flashing and caps brightness.
- Cozy mood generates warm low/medium state.
- Generated summary appears.
- Invalid/unknown mood returns error if API exists.

## Surprise Safety

- Surprise never selects Strobe.
- Surprise never selects Flash.
- Surprise excludes flashing-tagged modes.
- Surprise respects Night Guard.
- Surprise respects power/recommended brightness cap if available.
- Surprise uses only actual existing modes.
- Surprise uses only valid palettes.
- Surprise does not modify saved scenes unless user saves result.

## Surprise Interactions

- Surprise transitions into generated state if transitions enabled.
- Surprise cancels/rejects active timer/fade according to documented behavior.
- Surprise stops/rejects diagnostic tests according to documented behavior.
- Surprise-generated state can be saved as a scene if feature is implemented.
- Scene palette references still work.
- Favorites still work.
- Timers still work.
- Night Guard still works.

## Resource Report

- Sketch size reported or marked not measured.
- LittleFS usage reported or marked not measured.
- Free heap after boot reported or marked not measured.
- Free heap after main page reported or marked not measured.
- Free heap after key APIs reported or marked not measured.
- OTA status reported.
- D1-mini-vs-ESP32 recommendation included.

## Regression

Verify existing basics still work:

- Color picker
- White temperature slider
- Master brightness slider
- Mode filtering
- Scene save/load/rename/delete
- Favorite bar
- Warm Dim Now
- Off action
- Night Guard
- Timers
- Bedtime fade
- Smooth transitions
- Palette builder
- Diagnostics page
- Power estimator
- LED tests
- OTA routes

---

# Phase 3C Non-Goals Reminder

Do not implement these yet:

- First new lighting mode batch
- New lighting algorithms
- Weekly scheduler
- QR/NFC scene links
- Physical controls
- Music-reactive behavior
- Room-aware brightness
- ESP32-specific rewrite
- Server-side preview streaming
- WebSocket LED frame streaming

If tempted, write it in the report as deferred.

The preview gets to dance in the browser. The D1 mini gets to breathe. This is the law.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 3C complete.

Files changed:
- ...

Implemented:
- ...

Browser preview:
- Rendering method: ...
- Data sources: ...
- Polling rate: ...
- Dedicated mode approximations: ...
- Fallback behavior: ...

Surprise Me:
- Generator location: firmware/browser/hybrid
- Supported moods: ...
- Safety rules: ...
- Palette integration: ...
- Scene/save behavior: ...

Interactions:
- Transitions: ...
- Palettes: ...
- Timers/bedtime: ...
- Night Guard: ...
- Diagnostics tests: ...

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
- Free heap after preview UI loads:
- Free heap after Surprise Me API/action:
- OTA still works: yes/no/not tested
- Web UI responsiveness: snappy/acceptable/laggy/not tested
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
- Phase 3D — first new lighting mode batch.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim perfect preview accuracy, new lighting modes, weekly scheduling, QR/NFC links, physical controls, microphone behavior, light sensor behavior, or actual current measurement unless those actually exist.

---

# One-Sentence Mission Summary

Add a browser-side approximate live preview and a safe Surprise Me generator while keeping the ESP8266/D1 mini on a strict resource diet, preserving OTA and responsiveness, and reporting whether the project can stay on the D1 mini or should start considering an ESP32-S3 migration.

