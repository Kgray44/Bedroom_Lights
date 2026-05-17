# Bedroom LED Controller — 4-Phase Upgrade Plan

## Purpose

This document breaks the next major upgrade wave for the Bedroom LED Controller into **4 controlled implementation phases** so Codex does not receive 40+ ideas at once and transform the project into a raccoon-powered RGB doom chandelier.

The goal is to keep the project exciting, polished, and hilariously over-engineered, but still structurally sane.

## Core Rule for These 4 Phases

**No new hardware in these phases.**

These phases should only use the existing project foundation:

- D1 mini / ESP8266
- WS2812 LED strip
- Existing local web UI
- Existing OTA support
- Existing animation system
- Existing Wi-Fi/local network behavior

Hardware-related ideas from the uploaded docs are preserved as **deferred future expansions**, but should not be implemented in these four phases.

Deferred hardware ideas include:

- Physical wall control / rotary encoder / capacitive button
- Microphone-based music-reactive quiet mode
- Light-sensor-based room-aware auto brightness

Where possible, these ideas may receive **software hooks or placeholder architecture**, but no hardware assumptions should be added yet.

---

# Big Picture Upgrade Strategy

The project should evolve in this order:

1. **Make the controller safer, more stable, and easier to maintain.**
2. **Make everyday use fast and cozy.**
3. **Make the visual experience polished and beautiful.**
4. **Add the big library of calm lighting modes and automation features.**

This means the project should not start by adding 23 new lighting modes immediately. That would be fun for approximately 17 minutes, and then debugging would become a haunted corn maze with LEDs.

Instead, the first phase should build the internal systems that every future feature depends on.

---

# Phase 1 — Foundation, Safety, and Output Pipeline

## Theme

Build the reliable internal foundation before adding a giant pile of glittery features.

Phase 1 should make the controller safer, more predictable, more configurable, and easier to debug. This is the boring-sounding phase that prevents the project from becoming firmware lasagna.

## Main Goals

- Add a real master brightness system.
- Add persistent settings.
- Add a safer rendering/output pipeline.
- Add diagnostics and power-awareness.
- Add calibration tools.
- Add basic local API endpoints.
- Add LED test tools.

## Features to Implement

### 1. Master Brightness Control

Add a master brightness slider to the web UI.

This should be separate from mode-specific brightness behavior. Each mode can still calculate its own brightness pattern, but the final output should be globally capped by the master brightness value.

Recommended behavior:

- Range: 1–255, or 0–255 if 0 is treated as off.
- Default: conservative value such as 120–180.
- Store value persistently.
- Apply to every mode.
- Show current brightness percentage in the UI.

Implementation direction:

```cpp
renderModeToBuffer(currentMode, ledBuffer);
applyMasterBrightness(ledBuffer, masterBrightness);
applyGammaCorrection(ledBuffer);
strip.show();
```

Do not manually duplicate brightness scaling inside every mode if it can be avoided.

### 2. Persistent Settings

Add persistent storage for the basic current state.

At minimum, persist:

- Active mode
- Active RGB color
- White temperature value
- Master brightness
- Current animation speed/options
- Last selected scene, if scenes are not yet implemented then reserve for later
- Boot behavior setting

Storage options:

- LittleFS is preferred for structured JSON settings.
- EEPROM emulation is acceptable for a tiny first pass, but LittleFS is more expandable.

Settings should not be saved excessively every animation frame. Debounce saves so flash does not get abused like a poor little silicon punching bag.

Recommended save policy:

- Save when the user changes a setting.
- Debounce writes by about 500–1500 ms.
- Avoid repeated writes if the value did not actually change.

### 3. Clean Render Buffer Pipeline

Refactor animation rendering so each mode writes into a temporary LED buffer before output.

Desired conceptual pipeline:

```cpp
readCurrentState();
renderCurrentMode(rawFrameBuffer);
applySceneOrPaletteRules(rawFrameBuffer);
applyMasterBrightness(rawFrameBuffer);
applyGammaCorrection(rawFrameBuffer);
applyPowerSafetyClamp(rawFrameBuffer);
writeBufferToStrip(rawFrameBuffer);
strip.show();
```

This architecture enables later features:

- Smooth transitions
- Scene presets
- Browser previews
- Gamma correction
- Power safety estimates
- Mode blending
- Random scene generator
- Future palette system

This is the “build the highways before adding 800 tiny LED cars” part.

### 4. Gamma Correction

Add gamma correction so brightness changes look smoother and more natural to human eyes.

Recommended behavior:

- Use a lookup table for speed.
- Default gamma around 2.2.
- Allow gamma to be enabled/disabled in diagnostics or calibration settings.
- Store gamma preference persistently.

Optional first version:

- Hardcode gamma table.
- Add UI calibration later.

Better version:

- Add calibration UI in this phase.

### 5. Color Calibration Page

Add a simple calibration section or page.

Controls:

- Red gain
- Green gain
- Blue gain
- Gamma enable/disable
- Gamma amount, if feasible
- Minimum visible brightness compensation
- Warm white correction
- Cool white correction

Purpose:

WS2812 strips can look weird at low brightness, and RGB white can be ugly if the channels are not balanced. Calibration makes the strip feel more premium.

The goal is not scientific color accuracy. The goal is “this does not look like a gas station hot dog warmer.”

### 6. Boot Behavior Setting

Add a setting for what the controller does after restart or power loss.

Options:

- Restore last state
- Start off
- Start warm dim
- Start selected default scene, once scenes exist
- Start diagnostics-safe mode

Recommended first default:

- Restore last state, unless brightness was high and Night Guard is active later.

This prevents surprise full-bright chaos after a power blink.

### 7. Diagnostics and Power-Safe Page

Add a diagnostics page to the web UI.

Show:

- LED count
- LED pin
- Master brightness
- Estimated full-white current at 255
- Estimated current at current brightness cap
- Active mode
- Active color
- White temperature
- Wi-Fi status
- RSSI signal strength
- IP address
- Hostname
- Uptime
- Free heap
- OTA status/link
- Last settings save time/status

Add warnings:

- LED count is high for configured power supply.
- Master brightness may exceed safe configured current.
- Free heap is low.
- Wi-Fi signal is weak.
- D3/GPIO0 boot warning, if still using D3.

### 8. Power Supply Configuration Wizard

Add a simple software-only power configuration section.

Inputs:

- LED count
- Power supply voltage, default 5 V
- Power supply current rating, such as 3 A, 4 A, 5 A
- Safety margin percentage

Outputs:

- Worst-case full-white current estimate
- Recommended brightness cap
- Estimated current at current master brightness
- Warning if current settings exceed configured safety limit

This should not pretend to be a precision instrument. It is an estimate. Be honest in the UI.

Example UI language:

> Estimated worst-case current assumes about 60 mA per LED at full RGB white. Real current may vary by strip type, wiring, voltage drop, and animation behavior.

### 9. LED Test and Wiring Check Mode

Add a diagnostics test tool that can run simple strip checks.

Tests:

- All red
- All green
- All blue
- All white at low brightness
- Pixel walk from first LED to last LED
- Alternating pixels
- End-to-end direction check
- Safe low-brightness full-strip check

This helps verify:

- LED count
- Color order
- Bad pixels
- Data direction
- Voltage drop
- Wiring issues

This should be accessible from the diagnostics page.

### 10. Basic Local Automation API

Add simple API endpoints for future automation.

Suggested endpoints:

- `/api/state`
- `/api/on`
- `/api/off`
- `/api/brightness?value=120`
- `/api/color?r=255&g=100&b=50`
- `/api/temperature?k=2700`
- `/api/mode?name=RainGlass`
- `/api/test?name=pixelWalk`

Responses should be JSON.

Do not break the existing UI endpoints unless absolutely necessary. If refactoring, keep compatibility redirects or wrappers.

## Phase 1 Acceptance Criteria

Phase 1 is complete when:

- Master brightness affects every mode.
- Current settings survive restart.
- The render/output pipeline is centralized enough to support transitions later.
- Gamma correction works or is cleanly scaffolded.
- Diagnostics page displays useful real state.
- Power estimate updates based on LED count and brightness.
- LED test tools work at safe brightness.
- Basic API endpoints return JSON and can control core functions.
- No new hardware is required.

## Phase 1 Non-Goals

Do not implement yet:

- Full scene preset system
- Smooth mode transitions
- Palette builder
- Large batch of new lighting modes
- Hardware buttons
- Microphone support
- Light sensor support
- Browser animation preview

---

# Phase 2 — Everyday Control, Scenes, Timers, and Cozy Usability

## Theme

Make the controller feel useful every day, not just technically impressive.

This phase turns the controller into a room-lighting appliance. It should become fast to use, friendly at night, and able to remember favorite moods.

## Main Goals

- Add saved scenes.
- Add favorite scene/mode controls.
- Add quick comfort buttons.
- Add timer actions.
- Add Night Guard behavior.
- Add practical room scenes.
- Add mode tags and filtering.

## Features to Implement

### 1. Scene Presets With Save and Recall

Add a scene system stored persistently.

A scene should save:

- Scene name
- Active mode
- Active RGB color
- White temperature
- Master brightness
- Mode-specific speed/settings
- Palette reference, once palettes exist
- Transition preference, once transitions exist
- Tags, once tags exist

Initial built-in scenes:

- Warm Dim
- Reading
- Sleep
- Movie
- Focus
- Rain Glass
- Moon Tide
- Lantern Drift

Scene UI:

- Save current state as scene
- Rename scene
- Delete scene
- Recall scene
- Optional color preview swatch
- Optional mode label

Storage:

- Use LittleFS JSON if available.
- Keep a maximum scene count for ESP8266 sanity, such as 12–24 scenes.

### 2. Favorite Mode / Favorite Scene Bar

Add a compact favorite bar near the top of the main UI.

Recommended quick buttons:

- Off
- Warm Dim
- Solid
- Reading
- Movie
- Sleep
- Rain Glass
- Moon Tide

This should not replace the full dropdown. It should make common actions faster.

Future option:

- Allow user to configure which scenes appear in the favorite bar.

First version:

- Hardcode favorites based on built-in scenes.

### 3. Panic Warm Button

Add one obvious comfort button:

**Warm Dim Now**

Behavior:

- Stop harsh modes.
- Set warm white, around 2200K–2700K.
- Set low brightness.
- Disable flashing.
- Apply instantly or with a very short fade once transitions exist.

This is the “I am tired and my brain is oatmeal, please stop asking me questions” button.

### 4. Night Guard Mode

Add a global Night Guard setting.

Night Guard should be able to:

- Cap brightness after a selected time.
- Warn before enabling Strobe or Flash late at night.
- Prefer warm color temperatures.
- Use slower transitions once transitions exist.
- Hide or visually de-emphasize aggressive modes.
- Add a UI indicator when active.

For Phase 2, this can be manually toggled instead of time-based if NTP scheduling is not ready yet.

Suggested first implementation:

- Manual Night Guard toggle.
- Night max brightness value.
- Optional “block strobe/flash while active” setting.

### 5. Temporary Timer Actions

Add timer actions from the main UI.

Timer examples:

- Turn off in 15 minutes
- Turn off in 30 minutes
- Turn off in 1 hour
- Fade to warm dim in 30 minutes
- Fade to off in 1 hour
- Stay dim until morning, once scheduling exists

The UI should clearly show:

- Current active timer
- What will happen
- When it will happen
- Cancel timer button

Timer state should survive short reloads in the UI, but does not necessarily need to survive power loss in the first version.

### 6. Bedtime Fade Scheduler — Software-Only Version

Implement the software-only parts of the bedtime fade idea.

No new hardware.

Features:

- Bedtime fade start time
- Fade duration
- Target scene or target color/brightness
- Enabled/disabled toggle
- Manual “start bedtime fade now” button

Recommended behavior:

- Fade from current state to warm dim.
- Then optionally fade to off.
- Use saved scenes once available.

This requires timekeeping.

Acceptable Phase 2 approaches:

- Use NTP time sync.
- Or add manual timer-only bedtime fade first.

Preferred:

- Use NTP, show time sync status in diagnostics.

### 7. Scheduled Local Routines — Early Version

Add a basic schedule engine, but keep it modest.

First version:

- Up to 4 scheduled routine entries.
- Each routine has time, enabled flag, scene/mode target, and fade duration.
- Day-of-week selection can be added now or deferred to Phase 4.

Use cases:

- Morning warm light
- Evening current
- Night dim
- Auto off

Keep this simple. The ESP8266 is not a cloud server, even if we keep trying to emotionally convince it otherwise.

### 8. Mode Tags and Filtering

Add mode tags so the UI can help the user choose modes.

Example tags:

- Calm
- Sleep-safe
- Bright
- Animated
- Colorful
- Focus
- Evening
- Experimental
- Flashing
- Utility

Use tags for:

- Filtering the mode dropdown/list.
- Marking Strobe/Flash as flashing.
- Marking safe modes for Night Guard.
- Helping the random scene generator later.

### 9. Practical Built-In Scenes

Add software-defined practical scenes.

#### Study / Focus

- Neutral/cool white, around 4500K–5500K.
- Stable brightness.
- Minimal animation.
- Optional tiny slow gradient.

#### Workbench Mode

- Bright neutral/cool white.
- No animation.
- Optional temporary inspection boost.
- Useful for electronics work, soldering, and finding the resistor that vanished into the carpet dimension.

#### Movie Fade

- Dim warm/amber or soft blue edge glow.
- Low movement.
- Gentle transition from current mode.
- Designed as a calm backlight.

#### Sera Call Scene

- Soft warm-neutral white.
- Camera-friendly brightness.
- Low or no animation.
- Avoid weird color casts.

#### Cozy Goblin Mode

- Warm dim.
- Brightness capped.
- No harsh motion.
- Night-safe.
- One-tap comfort scene.

This is legally required because the project has reached cozy goblin density.

### 10. Scene URL Hooks

Prepare the foundation for direct scene URLs.

Examples:

- `/scene?sleep`
- `/scene?name=Movie`
- `/api/scene?name=Reading`

This supports QR codes and NFC tags later without needing any extra code redesign.

Actual QR/NFC generation can wait until Phase 4, but the URL hooks should be designed now.

## Phase 2 Acceptance Criteria

Phase 2 is complete when:

- Scenes can be saved, recalled, renamed, and deleted.
- Favorite bar provides fast access to common scenes/modes.
- Panic Warm works reliably.
- Night Guard can cap brightness and block/warn against harsh modes.
- Timers can trigger off/fade/warm-dim behavior.
- At least a simple bedtime fade exists.
- Basic local routines work or are scaffolded cleanly.
- Mode tags exist and are used in the UI.
- Practical scenes are present.
- No new hardware is required.

## Phase 2 Non-Goals

Do not implement yet:

- Full advanced palette builder
- Browser live animation preview
- Full QR/NFC workflow
- Large batch of new complex lighting modes
- Microphone support
- Physical controls
- Light sensor support

---

# Phase 3 — Visual Polish, Transitions, Palettes, Preview, and First New Mode Batch

## Theme

Make the controller feel expensive.

This phase is where the project starts to feel less like a web page controlling LEDs and more like a polished ambient lighting system.

## Main Goals

- Add smooth mode transitions.
- Add palette support.
- Add browser-side live preview.
- Add random-but-restrained generator.
- Add the first batch of new calm lighting modes.
- Improve UI structure for mode-heavy control.

## Features to Implement

### 1. Smooth Mode Transitions

Add smooth transitions between modes/scenes.

Transition types:

- Instant
- Fade
- Wipe
- Dissolve

Transition duration options:

- Instant
- 0.5 seconds
- 1.5 seconds
- 3 seconds
- 5 seconds for bedtime/calm scenes

Implementation direction:

- Keep previous rendered frame.
- Render target mode into new frame.
- Blend frames over transition duration.
- Do not block OTA, web server handling, or animations.

This requires non-blocking timing using `millis()`.

No `delay()` goblin nonsense.

### 2. Palette Builder

Add a palette system.

A palette should contain:

- Palette name
- 2–5 colors
- Optional color weights
- Optional warmth/coolness label
- Optional tags

Built-in palettes:

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

Use palettes in compatible modes:

- Velvet Aurora
- Slow Prism
- Paper Lantern Row
- Deep Sea Glass
- Quiet Northern Lights
- Northern Whisper
- Dream Aquarium
- Garden Shade
- Starlit Linen

Scenes should be able to reference palettes once this exists.

### 3. Browser-Side Live Preview Strip

Add a small visual preview strip to the web UI.

Important rule:

- The ESP8266 should not stream animation frames to the browser.
- The browser should approximate the animation locally based on current state.

The preview should show:

- Selected color or palette
- Rough animation motion
- Direction
- Brightness envelope
- Speed

It does not need to be pixel-perfect.

It just needs to say, “yes, this mode is slow and soft,” or “warning, you selected chaos noodle mode.”

### 4. Random but Restrained Scene Generator

Add a **Surprise Me** button with strict safety rules.

Rules:

- Do not select Strobe.
- Do not select Flash.
- Do not exceed current master brightness cap.
- Prefer calm modes.
- Prefer palettes tagged calm/evening/sleep-safe.
- Avoid full-bright white unless user explicitly allows it.
- Respect Night Guard.

Optional controls:

- Surprise: Calm
- Surprise: Evening
- Surprise: Focus
- Surprise: Colorful
- Surprise: Sleep-safe

This should generate tasteful combinations, not “Las Vegas printer jam.”

### 5. UI Layout Improvements for Many Modes

The mode list is about to get huge. A normal dropdown may become annoying.

Improve mode selection with:

- Category tabs or filters
- Mode tags
- Search field
- Favorites first
- Utility modes separated from ambient modes
- Flashing modes visually marked

Suggested categories:

- Everyday
- Calm
- Sleep
- Focus
- Colorful
- Weather / Nature
- Utility
- Experimental

### 6. First New Lighting Mode Batch

Implement a modest first batch of new modes. Do not add all 23 new modes in one Codex prompt.

This batch should prioritize modes that are useful, relatively simple, and good for validating the new render pipeline.

#### 1. Satin Breathing

Whole-strip breathing mode with refined timing.

Behavior:

- Slow inhale.
- Gentle hold.
- Longer exhale.
- Slight per-pixel texture so it does not look flat.
- Optional warmth shift during hold.

Controls:

- Breath length
- Hold time
- Texture amount
- Night cap

#### 2. Paper Lantern Row

Large warm glowing blobs with soft edges.

Behavior:

- Several lantern centers across the strip.
- Independent slow brightness changes.
- Slight drift.
- Warm paper-like palette.

Controls:

- Lantern count
- Paper warmth
- Sway amount
- Brightness

#### 3. Library Lamp

Practical reading/homework mode.

Behavior:

- Warm stable light.
- Slight soft texture.
- Minimal movement.
- Designed for reading and comfort.

Controls:

- Warmth
- Brightness
- Texture amount

#### 4. Solar Noon

Daytime productivity mode.

Behavior:

- Neutral/cool white.
- Bright but controlled.
- Almost no animation.
- Optional slow refined gradient drift.

Controls:

- Temperature
- Brightness
- Gradient amount

#### 5. Golden Hour

Warm sunlight from one side of the strip.

Behavior:

- Warm gradient, strongest on one side.
- Slowly shifts across strip.
- Gentle amber/gold tones.

Controls:

- Direction
- Warmth
- Movement speed
- Brightness

#### 6. Dream Aquarium

Cool watery calm mode.

Behavior:

- Deep blue/teal palette.
- Slow caustic-like waves.
- Low shimmer.
- Soft underwater motion.

Controls:

- Depth
- Shimmer amount
- Drift speed
- Palette

#### 7. Circuit Glow

Subtle electronics-inspired mode.

Behavior:

- Soft pulses travel like signals along traces.
- Base glow remains calm.
- Pulses fade smoothly.
- Not harsh or fast.

Controls:

- Signal density
- Trace speed
- Base glow
- Pulse color
- Fade length

### 7. Existing Mode Cleanup

While adding new modes, normalize existing mode behavior.

Ensure every mode:

- Uses master brightness.
- Uses gamma correction.
- Can be tagged.
- Can be stored in scenes.
- Can transition cleanly.
- Does not block the web server.
- Handles low LED counts gracefully.

## Phase 3 Acceptance Criteria

Phase 3 is complete when:

- Smooth transitions work between modes and scenes.
- Palette system exists with built-in palettes.
- Browser preview strip approximates current mode.
- Surprise Me generates safe, tasteful scenes.
- UI remains usable with many modes.
- First new mode batch is implemented and tagged.
- Existing modes work through the same output pipeline.
- No new hardware is required.

## Phase 3 Non-Goals

Do not implement yet:

- Every remaining lighting mode
- Full advanced schedule editor
- NFC/QR creation UI
- Hardware integrations
- Microphone support
- Light sensor support

---

# Phase 4 — Full Mode Library, Automation, and Final Software Polish

## Theme

Finish the software-only dream system.

This phase adds the remaining lighting concepts, upgrades scheduling, adds shareable scene links/QR support, and makes the controller feel complete.

## Main Goals

- Implement the remaining calm lighting modes.
- Add advanced local scheduling.
- Add QR/NFC-friendly scene links.
- Add stronger mode organization.
- Add final polish and resilience.
- Preserve hardware ideas for later without implementing them yet.

## Features to Implement

### 1. Full Weekly Schedule Editor

Expand the schedule system from Phase 2.

Features:

- Multiple routine entries, such as 8–16.
- Day-of-week selection.
- Scene target.
- Fade duration.
- Enabled/disabled toggle.
- Skip next occurrence.
- Manual run now.
- Time sync status.

Useful routines:

- Morning warm fade
- Afternoon focus
- Evening cozy
- Bedtime fade
- Auto-off
- Weekend different schedule

### 2. Scene QR Code / NFC-Friendly URLs

Add stable scene URLs.

Examples:

- `/go/sleep`
- `/go/movie`
- `/go/focus`
- `/api/scene?name=Sleep`

Optional UI:

- Show copyable scene link.
- Show simple QR code if feasible in browser JavaScript.
- Explain that NFC tags can open scene URLs.

No NFC hardware is added to the controller. This is just URL support so a phone can trigger scenes.

### 3. Advanced Scene Management

Improve scenes:

- Duplicate scene
- Export scenes as JSON
- Import scenes from JSON
- Reset built-in scenes
- Sort/reorder favorites
- Add scene tags
- Show scene compatibility warnings

This helps protect the user from losing favorite settings.

### 4. Advanced Local API

Expand the API for future Home Assistant, shortcuts, scripts, or other local automation.

Suggested endpoints:

- `/api/scenes`
- `/api/scene/save`
- `/api/scene/load`
- `/api/palette`
- `/api/palettes`
- `/api/schedule`
- `/api/nightguard`
- `/api/diagnostics`
- `/api/power`

All responses should be JSON.

Keep authentication simple but consider an optional local API token if controls become exposed beyond the local network.

### 5. Final New Lighting Mode Batch From Uploaded Docs

Implement the remaining uploaded-doc calm mode ideas.

These are design concepts and should be implemented carefully, preferably in small Codex sub-prompts, not all at once.

#### 1. Window Sunrise

Morning light spilling in from one side of the room.

Key traits:

- Starts dim and amber on one end.
- Slowly widens and brightens.
- Gradually becomes less orange.
- Great for wake-up scenes.

Controls:

- Direction
- Duration
- Final temperature
- Final brightness cap

#### 2. Ember Quilt

Soft glowing warm patches under a blanket.

Key traits:

- Warm islands of brightness.
- Slow independent breathing zones.
- Cozy and low-contrast.

Controls:

- Warmth
- Texture depth
- Drift speed
- Minimum brightness

#### 3. Porcelain Sky

Pale refined daytime gradient.

Key traits:

- Cool white and selected color blend.
- Very slow cloud-like movement.
- Stable brightness.

Controls:

- Tint strength
- Cloud speed
- Brightness behavior
- Palette

#### 4. Hearthline

Far-away fireplace warmth without harsh flicker.

Key traits:

- Low amber base.
- Slow warm bands.
- Restrained movement.

Controls:

- Heat level
- Band width
- Drift speed
- Color mix

#### 5. Evening Current

Smooth river-like light flow.

Key traits:

- Broad moving swell.
- Counter-moving ripple.
- No full black valleys.
- Good for relaxing.

Controls:

- Current speed
- Depth
- Direction
- Brightness floor

#### 6. Starlit Linen

Soft low-brightness highlights over a steady base.

Key traits:

- Not harsh twinkle.
- Highlights affect neighboring pixels.
- Slow fade in/out.

Controls:

- Density
- Highlight warmth
- Fade length
- Maximum highlight brightness

#### 7. Deep Sea Glass

Cool quiet light through water/glass.

Key traits:

- Blue-green or cyan blend.
- Broad dim swell.
- Faint caustic shimmer.

Controls:

- Depth
- Shimmer amount
- Drift speed
- Palette

#### 8. Quiet Northern Lights

Calmer aurora ribbons.

Key traits:

- Soft bands with pause/reverse/dissolve.
- Active color remains anchor.
- Not full rainbow chaos.

Controls:

- Ribbon count
- Color spread
- Fold speed
- Brightness cap

#### 9. Candle Hall

Distant candles down a hallway.

Key traits:

- Small warm clusters.
- Smoothed irregular flicker.
- Low brightness range.

Controls:

- Candle count
- Flicker softness
- Base warmth
- Shadow level

#### 10. Slow Prism

Selected color split into nearby companion colors.

Key traits:

- Tasteful color separation.
- Wide gradient rotates slowly.
- Avoids full rainbow unless desired.

Controls:

- Spread
- Rotation speed
- Contrast
- Anchor strength

#### 11. Garden Shade

Soft sunlight through leaves.

Key traits:

- Warm base.
- Muted green-gold bands.
- Slow movement.
- Daytime-friendly.

Controls:

- Shade tint
- Breeze speed
- Sun amount
- Contrast

#### 12. Snowfall Hush

Quiet cool-white drifting points.

Key traits:

- Soft tiny clusters.
- Long fades.
- Low cool background.
- Can also become warm dust-mote mode.

Controls:

- Snow density
- Drift speed
- Background temperature
- Highlight brightness

#### 13. Observatory Glow

Very slow celestial ambience.

Key traits:

- Dark blue or selected-color base.
- One broad moonlike glow.
- Soft star gradients.
- Very slow pass.

Controls:

- Main glow speed
- Star density
- Color mood
- Minimum brightness

#### 14. Rainy Window

Moodier cousin of Rain Glass.

Key traits:

- Cool soft background.
- Occasional tiny trails.
- Long fading droplets.
- Slow and emotional, but not depressing.

Controls:

- Droplet density
- Trail length
- Coolness
- Drift speed

#### 15. Campfire Blanket

Restrained warm amber comfort.

Key traits:

- Warm low-motion glow.
- Gentle living variation.
- Less flickery than fire.
- More alive than solid warm white.

Controls:

- Warmth
- Movement amount
- Brightness floor
- Cozy depth

#### 16. Northern Whisper

Simpler soft aurora mode.

Key traits:

- Barely saturated cyan/blue/green/violet.
- Soft ribbons.
- Very calm.
- Less complex than Quiet Northern Lights.

Controls:

- Color spread
- Ribbon softness
- Speed
- Brightness cap

### 6. Existing Implemented Calm Modes Review

Review and polish existing modes:

- Dawn Bloom
- Moon Tide
- Velvet Aurora
- Lantern Drift
- Rain Glass
- Slow Wave
- Slow Pulse

Ensure each has:

- Clear controls
- Tags
- Scene compatibility
- Palette compatibility where useful
- Smooth transitions
- Browser preview approximation
- Night Guard behavior

### 7. Storm Outside Mode

Add a calm storm ambience mode.

Key traits:

- Deep blue-gray base.
- Very slow dark waves.
- Occasional soft distant flash.
- Long fade after flash.
- Low brightness option.

Important:

This should not be aggressive lightning. It should feel like a rainy night, not Zeus performing an electrical safety violation in the bedroom.

Controls:

- Storm depth
- Flash frequency
- Flash softness
- Base brightness

### 8. Final UI Polish

Clean up the interface after the mode library gets big.

Polish tasks:

- Improve mode selection layout.
- Add mode descriptions.
- Add small “best used for” labels.
- Add consistent buttons.
- Add clear current state summary.
- Improve phone layout.
- Add reduced-motion UI option.
- Add lower-glare nighttime UI theme.
- Hide advanced controls behind expandable sections.

### 9. Software Hooks for Deferred Hardware Ideas

Do not implement hardware yet, but prepare gentle hooks.

#### Physical Wall Control — Deferred

Do not add encoder/button code yet.

Possible software prep:

- Define core actions like toggle, brightness up/down, next favorite, activate Warm Dim.
- Make these actions callable from API so a future physical control can trigger them.

#### Music-Reactive Quiet Mode — Deferred

Do not add microphone code yet.

Possible software prep:

- Create placeholder mode metadata only.
- Do not show as available unless hardware support exists.
- Keep audio-reactive architecture separate from normal modes.

#### Room-Aware Auto Brightness — Deferred

Do not add light sensor code yet.

Possible software prep:

- Add manual Day / Evening / Night brightness profiles.
- Later, a sensor can choose among those profiles.

## Phase 4 Acceptance Criteria

Phase 4 is complete when:

- Weekly schedule editor is usable.
- Scene URLs work reliably.
- API supports scenes, palettes, schedules, diagnostics, and core controls.
- Remaining software-only lighting modes are implemented or intentionally deferred with clear reason.
- Existing modes are polished and integrated with tags, scenes, transitions, palettes, and preview.
- UI remains usable despite the large feature set.
- Hardware ideas are preserved as future work but not implemented.
- No new hardware is required.

---

# Final Four-Phase Summary

## Phase 1 — Foundation, Safety, and Output Pipeline

Build the core systems:

- Master brightness
- Persistent settings
- Render buffer pipeline
- Gamma correction
- Calibration
- Boot behavior
- Diagnostics
- Power estimate
- LED tests
- Basic API

## Phase 2 — Everyday Control, Scenes, Timers, and Cozy Usability

Make it useful every day:

- Scene presets
- Favorite bar
- Panic Warm
- Night Guard
- Timers
- Bedtime fade
- Simple routines
- Mode tags
- Practical scenes

## Phase 3 — Visual Polish, Transitions, Palettes, Preview, and First New Mode Batch

Make it feel premium:

- Smooth transitions
- Palette builder
- Browser preview strip
- Surprise Me generator
- Mode filtering
- First new mode batch
- Existing mode cleanup

## Phase 4 — Full Mode Library, Automation, and Final Software Polish

Finish the software-only dream system:

- Full weekly scheduler
- Scene URLs / QR-friendly links
- Advanced scenes
- Advanced API
- Remaining lighting modes
- Final UI polish
- Hooks for future hardware ideas

---

# Recommended Codex Prompt Strategy

Do not give Codex this entire plan as one implementation request.

Use this document as the governing roadmap, then feed Codex one subphase at a time.

Recommended prompt chunks:

1. Phase 1A — Master brightness and persistent settings
2. Phase 1B — Render buffer pipeline and gamma correction
3. Phase 1C — Diagnostics, power estimate, and LED test tools
4. Phase 1D — Basic local API cleanup
5. Phase 2A — Scene storage and recall
6. Phase 2B — Favorite bar, Panic Warm, Night Guard
7. Phase 2C — Timers and bedtime fade
8. Phase 2D — Tags and practical scenes
9. Phase 3A — Smooth transitions
10. Phase 3B — Palette builder
11. Phase 3C — Browser preview and Surprise Me
12. Phase 3D — First new lighting mode batch
13. Phase 4A — Advanced scheduler and scene links
14. Phase 4B — Remaining mode library batch 1
15. Phase 4C — Remaining mode library batch 2
16. Phase 4D — Final polish, cleanup, and documentation

Each Codex prompt should require:

- No blocking delays in animation logic.
- No new hardware assumptions.
- Existing modes must not regress.
- OTA and web server responsiveness must remain intact.
- All new settings should be documented.
- Any limitations should be reported honestly.
- The sketch should still compile for the D1 mini / ESP8266 target.

---

# Project Vibe Statement

The Bedroom LED Controller should feel like a small local-first ambient lighting product:

- Calm by default
- Beautiful when desired
- Safe with power
- Useful at night
- Fast for everyday actions
- Fun without becoming obnoxious
- Over-engineered in the best possible way

In technical terms: cozy embedded systems goblin excellence.

In emotional terms: the room gets vibes, the firmware gets structure, and the raccoon stays away from the meth-powered design keyboard.

