# Bedroom LED Controller — Phase 3B Codex Prompt

## Phase 3B Title

**Palette Builder + Palette-Aware Modes Foundation**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 3B** of the larger upgrade plan.

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

The goal of Phase 3B is to add a **palette system** so compatible modes and scenes can use rich groups of colors instead of only one active RGB color.

A palette should contain 2–5 colors, optional weights, tags, and a friendly name. The user should be able to choose built-in palettes, create custom palettes, edit them, delete them, and save palette references inside scenes.

This phase should not add the browser live preview yet. It should not add the random scene generator yet. It should not add the first new lighting mode batch yet. This phase builds the color-language foundation those later features will use.

In tiny embedded-product terms: we are giving the LED controller a box of tasteful crayons, not releasing the entire glitter dragon.

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
- Smooth transition system from Phase 3A

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

Phase 3B may add palette routes/settings, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. Palette data model
2. Palette storage
3. Built-in palettes
4. Active palette selection
5. Palette UI builder/editor
6. Palette API endpoints
7. Palette-aware mode metadata
8. Palette-aware rendering helpers
9. Limited integration with existing compatible modes
10. Scene preset palette references
11. Favorite/scene/timer/transition compatibility with palettes
12. State/diagnostics integration
13. Documentation/report update
14. Compile/test report

Do **not** implement yet:

- Browser live animation preview
- Random / Surprise Me generator
- First new lighting mode batch
- New lighting algorithms except minor palette support for existing modes
- Full weekly scheduler
- QR/NFC scene links
- Physical controls
- Microphone support
- Light sensor support
- New hardware assumptions

If a later feature seems tempting, document it as deferred. The Preview Goblin and Surprise-Me Goblin must wait their turn in the glitter queue.

---

# Phase 3B Main Goal

At the end of Phase 3B, the user should be able to:

1. See a list of built-in palettes.
2. Select an active palette.
3. Create a custom palette with 2–5 colors.
4. Edit custom palette colors, names, and optional weights.
5. Delete custom palettes.
6. Reset built-in palettes if needed.
7. Use palettes in compatible existing modes.
8. Save palette references inside scenes.
9. Recall a scene and restore its palette reference.
10. Use local API endpoints to list, select, create, edit, and delete palettes.
11. Keep smooth transitions, Night Guard, timers, scenes, favorites, diagnostics, and OTA working.

This phase is about **color structure and palette infrastructure**, not adding a pile of new animations.

---

# 1. Palette Data Model

## Goal

Add a compact palette data model suitable for ESP8266 memory/storage constraints.

## Required Palette Fields

A palette should include at least:

```cpp
struct PaletteColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t weight; // optional, default 255 or equal weighting
};

struct ColorPalette {
  String id;
  String name;
  uint8_t colorCount;
  PaletteColor colors[5];
  bool builtin;
  String category;
  bool calm;
  bool sleepSafe;
  bool colorful;
  bool warm;
  bool cool;
};
```

Use names and types that fit the current code.

If repeated `String` fields are risky, use compact fixed-size char arrays or store strings only where already acceptable in the project.

## Required Rules

- Minimum colors per palette: 2
- Maximum colors per palette: 5
- Palette IDs should be URL-safe.
- Palette names should be user-friendly.
- Palette names should have a reasonable max length, such as 24–32 characters.
- Built-in palettes should be protected or at least restorable.
- Custom palettes should be editable/deletable.

## Palette ID Format

Recommended:

- Lowercase
- URL-safe
- Hyphen-separated
- Stable after creation

Examples:

```text
candle
ocean
moonlight
peach-sky
custom-soft-blue
```

## Weights

Weights are optional but useful.

Recommended first behavior:

- Store a weight for each color.
- Default all weights to equal value, such as `255`.
- Palette-aware modes may ignore weights at first, but the data should exist for future use.

If weight support creates too much complexity, keep the field but document that current modes treat palette colors equally.

## Acceptance Criteria

- Palette model supports 2–5 colors.
- Built-in and custom palettes can be represented.
- Palette metadata supports future filtering/generation.
- Memory use remains reasonable.

---

# 2. Palette Storage

## Goal

Persist custom palettes and built-in palette availability across reboot.

## Preferred Storage

Use LittleFS or the existing filesystem layer.

Recommended file:

```text
/palettes.json
```

Keep it separate from:

- `/settings.json` for controller settings
- `/scenes.json` for scene presets

## Storage Format

Recommended JSON shape:

```json
{
  "version": 1,
  "palettes": [
    {
      "id": "candle",
      "name": "Candle",
      "builtin": true,
      "category": "Warm",
      "tags": ["warm", "calm", "sleep-safe"],
      "colors": [
        {"hex": "#FFB15A", "weight": 255},
        {"hex": "#FF7A2F", "weight": 180},
        {"hex": "#FFD6A0", "weight": 120}
      ]
    }
  ]
}
```

Alternative compact storage is acceptable if documented.

## Max Palette Count

Set a reasonable maximum.

Recommended:

```text
MAX_PALETTES = 16
```

or:

```text
10 built-in + 8 custom = 18 total
```

Acceptable range:

```text
12–24 total palettes
```

Do not allow unlimited palette creation.

The ESP8266 is not a museum archive. It is a tiny Wi-Fi beetle with dreams.

## Load Behavior

During setup:

1. Load built-in palette definitions from code.
2. Try to load `/palettes.json`.
3. If file exists and is valid, load custom palettes and any stored palette metadata.
4. If file is missing, seed built-ins and save if appropriate.
5. If file is corrupt, fall back to built-ins and track an error.
6. Do not crash on missing/corrupt/incomplete palette data.

## Save Behavior

When palettes are created/edited/deleted/reset:

- Save palette data to `/palettes.json`.
- Avoid saving every animation frame.
- Avoid rewriting palette file unnecessarily during normal animation.

Palette changes are user-triggered and uncommon, so immediate save is acceptable.

## Acceptance Criteria

- Built-in palettes exist after first boot.
- Custom palettes persist after reboot.
- Missing palette file falls back safely.
- Corrupt palette file falls back safely.
- Palette count is bounded.

---

# 3. Built-In Palettes

## Goal

Provide useful built-in palettes for existing and future calm modes.

## Required Built-In Palette List

Add these built-in palettes if practical:

1. Candle
2. Ocean
3. Garden
4. Moonlight
5. Peach Sky
6. Soft Neon
7. Aurora
8. Warm Linen
9. Storm Blue
10. Circuit Glow

These are part of the planned Phase 3 palette system.

## Suggested Palette Definitions

Use tasteful, not retina-crimes.

### Candle

Warm amber and creamy white.

Suggested colors:

- `#FFB15A`
- `#FF7A2F`
- `#FFD6A0`
- `#FFF0C8`

Tags:

- warm
- calm
- sleep-safe

### Ocean

Soft teal/blue watery colors.

Suggested colors:

- `#0B5D7A`
- `#1EA7A8`
- `#4FD3C4`
- `#B8FFF2`

Tags:

- cool
- calm
- water

### Garden

Muted green/gold natural colors.

Suggested colors:

- `#6E8F4E`
- `#A8B96A`
- `#D7C46A`
- `#F2DFA0`

Tags:

- natural
- calm
- daytime

### Moonlight

Cool white, pale blue, and low-saturation indigo.

Suggested colors:

- `#DDEBFF`
- `#8FB8FF`
- `#405C9A`
- `#172B5A`

Tags:

- cool
- calm
- night

### Peach Sky

Soft peach, pink, and warm cream.

Suggested colors:

- `#FFD0A8`
- `#FF9C8A`
- `#FFE7C7`
- `#F7B7D2`

Tags:

- warm
- calm
- evening

### Soft Neon

Colorful but restrained.

Suggested colors:

- `#4DEEEA`
- `#74EE15`
- `#FFE700`
- `#F000FF`

Tags:

- colorful
- energetic
- experimental

### Aurora

Soft aurora-style blue, green, violet.

Suggested colors:

- `#3EF6B0`
- `#4DA3FF`
- `#9D6CFF`
- `#D6F8FF`

Tags:

- colorful
- calm
- aurora

### Warm Linen

Creamy warm neutral palette.

Suggested colors:

- `#FFE6B8`
- `#FFD9A0`
- `#F4C27A`
- `#FFF3D6`

Tags:

- warm
- calm
- everyday
- sleep-safe

### Storm Blue

Deep moody blue-gray palette.

Suggested colors:

- `#102033`
- `#1F3A5A`
- `#416A8A`
- `#A7C7D9`

Tags:

- cool
- weather
- night

### Circuit Glow

Subtle electronics-inspired green/cyan/blue.

Suggested colors:

- `#0AFF9D`
- `#00B8FF`
- `#0A2A3A`
- `#D8FFF3`

Tags:

- colorful
- experimental
- cool

## Built-In Protection

Choose one policy and document it:

### Recommended Policy

- Built-ins cannot be permanently deleted.
- User can hide/edit only custom copies.
- Provide reset built-ins action.

### Simpler Acceptable Policy

- Built-ins can be deleted from the file but return when palettes are reset.

## Acceptance Criteria

- Required built-in palettes exist.
- Built-ins are valid 2–5 color palettes.
- Built-ins have useful tags/categories.
- Built-in policy is documented.

---

# 4. Active Palette Selection

## Goal

Track which palette is currently active.

## Required Settings

Add to central settings:

```cpp
String activePaletteId;
bool paletteEnabled;
```

Optional:

```cpp
bool paletteAffectsCompatibleModesOnly;
```

Recommended defaults:

```text
paletteEnabled = false
activePaletteId = "warm-linen" or "candle"
```

## Behavior

When palette support is enabled:

- Palette-aware modes may use the active palette.
- Non-palette-aware modes continue using the active RGB/white color.
- Solid Color should generally remain controlled by active color/white temperature unless explicitly designed otherwise.

Do not force every mode to use palettes.

## State Reporting

Expose in `/api/state` and/or `/state`:

```json
{
  "paletteEnabled": true,
  "activePaletteId": "candle",
  "activePaletteName": "Candle"
}
```

## Acceptance Criteria

- Active palette can be selected.
- Palette enable/disable works.
- Non-palette-aware modes are not broken.
- Active palette persists after reboot.

---

# 5. Palette-Aware Mode Metadata

## Goal

Teach the controller which modes can use palettes.

## Required Metadata Additions

Extend mode metadata from Phase 2D with fields such as:

```cpp
bool supportsPalette;
String preferredPaletteRole;
```

Possible roles:

```text
none
accent
gradient
ambient
sparkle
ribbon
```

Keep this simple.

## Existing Modes Likely To Support Palettes

Palette support may be useful for:

- Velvet Aurora
- Moon Tide
- Lantern Drift
- Rain Glass
- Slow Wave
- Slow Pulse
- Rainbow, optional and probably not recommended because Rainbow already owns color

Do not force palette support on every mode.

## Future Mode Metadata Hooks

Future modes should eventually support palettes:

- Slow Prism
- Paper Lantern Row
- Deep Sea Glass
- Quiet Northern Lights
- Northern Whisper
- Dream Aquarium
- Garden Shade
- Starlit Linen
- Circuit Glow

Do not add these as implemented modes if they do not exist yet.

It is acceptable to mention in documentation that the palette system is prepared for future modes.

## Acceptance Criteria

- Mode metadata indicates palette compatibility.
- UI can show whether current mode supports palettes.
- Palette settings do not confuse modes that ignore palettes.

---

# 6. Palette Rendering Helpers

## Goal

Add reusable helper functions for palette-aware modes.

## Required Helpers

Add helpers equivalent to:

```cpp
ColorPalette* getActivePalette();
RgbPixel getPaletteColor(uint8_t index);
RgbPixel samplePalette(uint16_t position, uint16_t maxPosition);
RgbPixel blendPaletteColors(RgbPixel a, RgbPixel b, uint8_t amount);
RgbPixel getWeightedPaletteColor(uint8_t selector);
```

Use names and types that fit the code.

## Sampling Behavior

Recommended first implementation:

- `getPaletteColor(index)` returns the exact palette color by index.
- `samplePalette(position, maxPosition)` interpolates along palette colors to produce a gradient.
- If palette is missing or disabled, fall back to active color.

## Fallback Rules

If palette is enabled but invalid/missing:

- Fall back to active color.
- Return warning in diagnostics/state if practical.
- Do not crash.

If a palette has fewer than 2 valid colors:

- Treat as invalid.
- Fall back to active color.

## Acceptance Criteria

- Palette helper functions exist.
- Helpers handle missing/invalid palettes safely.
- Compatible modes can use helpers without duplicating palette logic.

---

# 7. Limited Existing Mode Integration

## Goal

Use palettes in a small number of existing compatible modes without redesigning every mode.

## Required Approach

Integrate active palette support into **at least one** existing calm/colorful mode if feasible.

Recommended existing candidates:

1. Velvet Aurora
2. Lantern Drift
3. Moon Tide
4. Rain Glass
5. Slow Wave

Do not rewrite all modes.

Do not redesign existing visual behavior dramatically.

This phase is about proving the palette pipeline works.

## Suggested Mode-Specific Behavior

### Velvet Aurora

If palette is enabled:

- Use palette colors as veil/ribbon colors.
- Sample colors slowly across the strip.
- Preserve the soft aurora feel.

### Lantern Drift

If palette is enabled:

- Use warm palette colors for lantern pools.
- If selected palette is cool/bright, still keep brightness gentle.

### Moon Tide

If palette is enabled:

- Use cool palette colors for tide layers.
- If palette is warm, blend gently with moonlight rather than becoming harsh.

### Rain Glass

If palette is enabled:

- Use palette colors for beads/trails.
- Keep the glassy/calm behavior.

## Important Rule

Palette support should be optional.

If `paletteEnabled` is false, modes should behave like before.

## Acceptance Criteria

- At least one existing mode uses active palette when enabled.
- Palette disabled returns to original active-color behavior.
- Existing mode controls still work.
- Smooth transitions still work when switching palettes/modes.

---

# 8. Palette UI Builder

## Goal

Add a lightweight UI for selecting, creating, and editing palettes.

## UI Placement Options

Recommended:

- Add a **Palettes** section on the main page under color/mode controls or in an advanced visual section.

Alternative:

- Add a dedicated `/palettes` page and link to it from the main page.

For everyday usefulness, a compact palette selector should be accessible from the main page.

## Required UI Features

The UI should include:

- Palette enable/disable toggle
- Active palette selector/dropdown
- Built-in palette list
- Custom palette list
- Color swatches for each palette
- Create new palette form
- Edit custom palette form
- Delete custom palette button
- Reset built-in palettes button or reset all palettes button
- Error/success message area

## Palette Creation UI

Minimum fields:

- Palette name
- Color 1
- Color 2
- Optional Color 3
- Optional Color 4
- Optional Color 5

Optional fields:

- Weights per color
- Category
- Tags

For Phase 3B, simple color inputs are enough.

## HTML Input Recommendation

Use browser-native color inputs where possible:

```html
<input type="color">
```

## UI Constraints

Keep UI lightweight.

Do not add heavy frameworks.

The ESP8266 should not have to serve a React app wearing a tuxedo.

## Acceptance Criteria

- User can enable/disable palettes.
- User can select active palette.
- User can see palette swatches.
- User can create a 2–5 color custom palette.
- User can edit custom palettes.
- User can delete custom palettes.
- UI remains usable on mobile.

---

# 9. Palette API Endpoints

## Goal

Expose palette behavior through local JSON API endpoints.

## Required Endpoints

Implement or normalize:

```text
/api/palettes
/api/palettes/select?id=...
/api/palettes/save?name=...&c1=...&c2=...
/api/palettes/update?id=...
/api/palettes/delete?id=...
/api/palettes/reset
```

Alternative route naming is acceptable if documented, but be consistent.

## `/api/palettes`

Returns palette list and active palette status.

Example:

```json
{
  "ok": true,
  "paletteEnabled": true,
  "activePaletteId": "candle",
  "count": 10,
  "maxPalettes": 16,
  "palettes": [
    {
      "id": "candle",
      "name": "Candle",
      "builtin": true,
      "category": "Warm",
      "tags": ["warm", "calm", "sleep-safe"],
      "colors": ["#FFB15A", "#FF7A2F", "#FFD6A0"]
    }
  ]
}
```

## `/api/palettes/select`

Selects active palette.

Example:

```text
/api/palettes/select?id=candle&enabled=1
```

Behavior:

- Validate palette ID exists.
- Set activePaletteId.
- Set paletteEnabled if provided.
- Mark settings dirty.
- Start transition if current mode supports palette and transitions are enabled, if feasible.

## `/api/palettes/save`

Creates a new custom palette.

Example:

```text
/api/palettes/save?name=Soft%20Blue&c1=223344&c2=88CCFF&c3=DDEEFF
```

Color formats to support:

- `RRGGBB`
- optional `#RRGGBB`

Validation:

- Name non-empty.
- At least 2 valid colors.
- No more than 5 colors.
- Palette count below max.
- Duplicate name/ID handled safely.

## `/api/palettes/update`

Updates an existing custom palette.

Required behavior:

- Built-in palettes should not be edited unless the chosen policy allows editing custom copies only.
- Validate colors/name.
- Save palette file.

## `/api/palettes/delete`

Deletes a custom palette.

Behavior:

- Reject deleting protected built-ins.
- If deleted palette is active, switch to a safe default palette and/or disable palette use.
- Save palette file.

## `/api/palettes/reset`

Restores built-in palettes and optionally clears custom palettes depending on chosen behavior.

Recommended:

- Reset built-ins without deleting custom palettes if practical.
- If simpler, document that reset restores defaults and may remove custom palettes.

## Response Format

Use existing JSON convention:

Success:

```json
{
  "ok": true,
  "message": "Palette saved"
}
```

Error:

```json
{
  "ok": false,
  "error": "At least two valid colors are required"
}
```

## Acceptance Criteria

- Palette list endpoint works.
- Select endpoint works.
- Save endpoint works.
- Update endpoint works for custom palettes.
- Delete endpoint works for custom palettes.
- Reset endpoint works or is clearly deferred.
- Invalid inputs return JSON errors.

---

# 10. Palette Scene Integration

## Goal

Allow scenes to remember palette state.

## Required Scene Fields

Extend scene presets with optional palette fields:

```cpp
bool paletteEnabled;
String paletteId;
```

Optional:

```cpp
bool sceneUsesPalette;
```

## Save Current State As Scene

When saving a scene:

- Save paletteEnabled.
- Save activePaletteId if palette is enabled.
- Do not duplicate the full palette colors inside every scene unless intentionally chosen.

Recommended:

- Store only palette ID reference.

Reason:

- Saves memory.
- Allows palette edits to affect scenes that reference it.

## Load Scene

When loading a scene:

- Restore paletteEnabled and activePaletteId if scene contains palette data.
- If referenced palette is missing:
  - Disable palette and warn, or
  - Fall back to a default palette.

Recommended:

- Fall back to active color / palette disabled and return warning.

## Backward Compatibility

Existing scenes without palette fields should still load.

Missing fields default to:

```text
paletteEnabled = false
paletteId = current/default palette
```

## Acceptance Criteria

- New scenes save palette reference.
- Loading a scene restores palette state.
- Old scenes still load.
- Missing palette references fail safely.

---

# 11. Palette Interaction With Favorites, Timers, Bedtime Fade, and Transitions

## Favorites

If a favorite loads a scene with a palette reference:

- The scene load should restore palette state.
- Favorite behavior should not need separate palette logic.

## Timers / Bedtime Fade

If a timer loads a scene with a palette reference:

- The scene load should restore palette state.
- Timer fade should not spam palette saves.

If bedtime fade targets Warm Dim or Off:

- Palette state may remain unchanged unless the target scene/action explicitly changes it.

## Smooth Transitions

If transitions are enabled and the active palette changes:

- If current mode supports palettes, transition smoothly if feasible.
- If not feasible, apply palette change normally and document limitation.

Important:

- Do not double-apply transitions.
- Do not make palette color picker edits trigger a full 1.5-second transition on every tiny change unless intentionally designed.

Recommended:

- Palette selection triggers a transition.
- Editing custom palette colors updates live but may be immediate or lightly smoothed.

## Night Guard

Night Guard brightness cap still applies.

Night Guard should not reject a palette just because it is colorful unless the active mode is flashing/blocked.

Optional:

- Mark palettes as sleep-safe/calm and show UI guidance.

Do not forcibly block colorful palettes in Phase 3B.

## Acceptance Criteria

- Favorites work with palette scenes.
- Timers work with palette scenes.
- Transitions still work with palette changes.
- Night Guard still caps brightness.
- No existing feature breaks.

---

# 12. Palette-Aware State and Diagnostics

## Goal

Expose palette state without bloating core state too much.

## `/api/state` Additions

Add compact fields:

```json
{
  "paletteEnabled": true,
  "activePaletteId": "candle",
  "activePaletteName": "Candle",
  "currentModeSupportsPalette": true
}
```

Do not include the entire palette list in `/api/state`.

Use `/api/palettes` for full palette data.

## Diagnostics Additions

If practical, add to diagnostics:

- Palette enabled/disabled
- Active palette name/id
- Palette file load status
- Palette count/max count
- Current mode supports palette: yes/no
- Last palette save status

Keep it compact.

## Acceptance Criteria

- Main UI can initialize palette controls from state.
- Diagnostics can show palette status if implemented.
- State JSON remains compact.

---

# 13. Palette Error Handling

## Goal

Palette errors should fail safely and visibly.

## Required Error Cases

Handle:

- Missing `/palettes.json`
- Corrupt `/palettes.json`
- Palette with fewer than 2 colors
- Palette with more than 5 colors
- Invalid hex color
- Duplicate palette name/id
- Palette count limit reached
- Selecting unknown palette ID
- Deleting active palette
- Loading scene referencing missing palette

## Required Behavior

- Do not crash.
- Return JSON error where appropriate.
- Show UI error message.
- Fall back to safe default palette or active color.
- Track palette load/save status if practical.

## Acceptance Criteria

- Malformed palette API calls return JSON errors.
- Corrupt palette file does not brick controller.
- Missing active palette falls back safely.

---

# 14. Memory and Performance Constraints

## Goal

Keep palette logic ESP8266-friendly.

## Avoid

- Unlimited palette count
- Huge palette names/descriptions
- Heavy frontend frameworks
- Large JSON in `/api/state`
- Dynamic allocation every animation frame
- Re-parsing palette JSON every frame
- Saving palette file during animation

## Prefer

- Load palettes once into bounded array/list
- Use compact helper functions for sampling
- Store palette IDs and colors compactly
- Use `/api/palettes` only when palette UI needs it
- Use native HTML color inputs
- Keep palette rendering helpers simple

## Acceptance Criteria

- UI remains responsive.
- OTA handling remains present.
- Animation loop does not perform file IO.
- Palette helpers are not expensive.

---

# 15. UI Integration Details

## Main Page Changes

Add a palette area that fits into the Phase 2D organized UI.

Recommended section placement:

- Under Color and White Temperature, or
- Under Mode controls as “Palette for compatible modes,” or
- Under Advanced Visual Controls

Suggested UI sections:

1. Palette enable toggle
2. Active palette dropdown
3. Active palette swatches
4. Current mode palette support note
5. Create/edit custom palette controls

## Current Mode Support Note

Show something like:

```text
Current mode supports palettes.
```

or:

```text
Current mode uses the selected solid color. Palette is saved but not used by this mode.
```

This prevents confusion.

## Built-In vs Custom Display

Display built-ins and custom palettes clearly.

Example labels:

- Built-in
- Custom
- Warm
- Calm
- Colorful
- Sleep-safe

## Acceptance Criteria

- Palette UI is understandable.
- User can tell whether palette affects current mode.
- UI does not overwhelm the main page.
- Mobile layout remains usable.

---

# 16. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_3B_Report.md
```

The report should include:

1. Files changed.
2. Palette data model.
3. Palette storage file and schema.
4. Built-in palettes.
5. Active palette settings.
6. Palette API endpoints.
7. Palette UI behavior.
8. Palette-aware mode metadata.
9. Which existing modes gained palette support.
10. Scene palette reference behavior.
11. Interaction with favorites, timers, transitions, and Night Guard.
12. Error handling behavior.
13. Memory/performance notes.
14. Compile/test status.
15. Known limitations.
16. What was intentionally deferred.
17. Recommended next step: Phase 3C browser preview and Surprise Me generator.

## Required Honesty

Do not claim browser preview exists.

Do not claim Surprise Me / random generator exists.

Do not claim new lighting modes exist unless they actually do.

Do not claim all modes support palettes unless they actually do.

Do not claim physical LED testing unless actual LED testing was performed.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

If only one or two existing modes support palettes, say so clearly.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Existing State, Scene, Mode, and Transition Systems

Identify:

- Current settings struct/object
- Current scene data model
- Current save/load functions
- Current mode metadata table
- Current transition start points
- Current route/API helper style
- Current UI section organization

Do not rewrite the whole sketch.

## Step 2 — Add Palette Structs and Constants

Add:

- Palette color representation
- Palette representation
- Max palette count
- Max colors per palette
- Active palette settings defaults

## Step 3 — Add Built-In Palette Definitions

Add the 10 built-in palettes.

Keep them compact.

## Step 4 — Add Palette Storage

Implement:

- Load palettes
- Save palettes
- Missing/corrupt fallback
- Built-in reset behavior

## Step 5 — Add Palette Helper Functions

Implement:

- Find palette by ID
- Get active palette
- Parse/format hex color
- Sample palette gradient
- Get palette color safely

## Step 6 — Add Palette API Routes

Add:

- `/api/palettes`
- `/api/palettes/select`
- `/api/palettes/save`
- `/api/palettes/update`
- `/api/palettes/delete`
- `/api/palettes/reset`

Use consistent JSON responses.

## Step 7 — Add Palette UI

Add:

- Enable toggle
- Active palette dropdown
- Swatches
- Create custom palette form
- Edit/delete custom palette controls
- Current mode support note

## Step 8 — Extend Mode Metadata

Add `supportsPalette` or equivalent.

Update `/api/modes` if needed.

## Step 9 — Integrate One or More Existing Modes

Add optional palette behavior to at least one existing compatible mode.

Recommended first target: Velvet Aurora.

Second target if easy: Lantern Drift, Moon Tide, or Rain Glass.

## Step 10 — Extend Scene Save/Load

Add paletteEnabled and activePaletteId to scenes.

Maintain backward compatibility with old scenes.

## Step 11 — Update State/Diagnostics

Expose compact palette status.

## Step 12 — Test Interactions

Check scenes, favorites, timers, transitions, Night Guard, diagnostics, and OTA.

## Step 13 — Compile and Report

Compile for D1 mini / ESP8266.

Create/update Phase 3B report.

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
- Scene/favorite/timer/Night Guard/transition routes still work.
- `/api/modes` still works.

## Palette Storage

- Built-in palettes exist on first boot.
- `/palettes.json` loads if valid.
- Missing palette file falls back to built-ins.
- Corrupt palette file falls back safely.
- Custom palettes persist after reboot.
- Palette count limit is enforced.

## Palette API

- `/api/palettes` returns valid JSON.
- `/api/palettes/select?id=candle&enabled=1` works.
- Saving a custom palette works.
- Updating a custom palette works.
- Deleting a custom palette works.
- Reset palettes works or is documented as deferred.
- Invalid hex colors return JSON error.
- Fewer than 2 colors returns JSON error.
- Unknown palette ID returns JSON error.

## Palette UI

- Palette enable toggle works.
- Active palette dropdown works.
- Swatches render.
- Custom palette creation works.
- Custom palette editing works.
- Custom palette deletion works.
- UI shows whether current mode supports palette.
- Mobile layout remains usable.

## Palette-Aware Modes

- At least one existing mode uses active palette when enabled.
- Palette disabled restores original active-color behavior.
- Non-palette-aware modes still work.
- Palette mode metadata is accurate.

## Scene Integration

- New scenes save paletteEnabled and paletteId.
- Loading a palette scene restores palette state.
- Old scenes without palette fields still load.
- Missing palette reference fails safely.

## Interactions

- Favorites loading palette scenes works.
- Timer loading palette scenes works.
- Transitions still work with palette scene changes.
- Night Guard still caps brightness.
- Diagnostics still works.
- LED tests still work.
- OTA remains present.

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
- Diagnostics page
- Power estimator
- LED tests
- OTA routes

---

# Phase 3B Non-Goals Reminder

Do not implement these yet:

- Browser live animation preview
- Surprise Me / random scene generator
- New lighting mode batch
- Weekly scheduler
- QR/NFC scene links
- Physical controls
- Music-reactive behavior
- Room-aware brightness

If tempted, write it in the report as deferred.

The Palette Goblin may stay. The Preview Goblin may not bring luggage.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 3B complete.

Files changed:
- ...

Implemented:
- ...

Palette model/storage:
- ...

Built-in palettes:
- ...

Palette API:
- ...

Palette UI:
- ...

Palette-aware modes:
- ...

Scene integration:
- ...

Interactions:
- Favorites: ...
- Timers: ...
- Transitions: ...
- Night Guard: ...

State/diagnostics:
- ...

Preserved:
- ...

Deferred:
- ...

Compile/test status:
- ...

Known limitations:
- ...

Recommended next step:
- Phase 3C — browser-side live preview strip and Surprise Me generator.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim browser preview, Surprise Me, random scene generation, new lighting modes, weekly scheduling, QR/NFC links, physical controls, microphone behavior, or light sensor behavior unless they actually exist.

---

# One-Sentence Mission Summary

Add a persistent 2–5 color palette system with built-in and custom palettes, palette APIs, palette UI, scene palette references, and limited palette-aware mode support so the Bedroom LED Controller can finally make sophisticated color combinations without turning into an overcaffeinated rainbow blender.

