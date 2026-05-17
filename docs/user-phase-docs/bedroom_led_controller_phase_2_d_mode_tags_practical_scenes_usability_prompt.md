# Bedroom LED Controller — Phase 2D Codex Prompt

## Phase 2D Title

**Mode Tags, Filtering, Practical Scenes, and Everyday Usability Polish**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 2D**, the final everyday-usability pass of Phase 2.

Phase 1 should already have produced a stable foundation with:

- Central runtime settings model
- Persistent settings storage
- Debounced settings saves
- Master brightness control
- Central render/output pipeline
- Gamma correction foundation
- RGB calibration foundation
- Diagnostics page
- Power estimator
- LED test tools
- Local API cleanup
- Preserved OTA/browser updater routes

Phase 2A should already have added or started:

- Scene preset data model
- Scene storage, preferably `/scenes.json`
- Built-in starter scenes
- Save current state as scene
- Recall/load scene
- Rename scene
- Delete scene
- List scenes
- Scene API endpoints
- Scene UI

Phase 2B should already have added or started:

- Favorite scene/action bar
- Favorite storage/defaults
- Panic Warm / Warm Dim Now action
- Off action
- Night Guard settings
- Night Guard effective brightness cap
- Night Guard harsh-mode blocking
- Night Guard API/UI integration

Phase 2C should already have added or started:

- One-shot timer actions
- Timer UI
- Timer API
- Bedtime fade
- Fade-to-Off / fade-to-Warm-Dim / fade-to-scene behavior
- Timer state in `/state` and/or `/api/timer`
- Non-blocking `millis()`-based timer/fade service

The goal of Phase 2D is to make the controller’s everyday use feel organized, understandable, and polished before Phase 3 adds smooth transitions, palettes, preview, and the first new lighting-mode batch.

This phase should improve how the user chooses modes and scenes, add practical everyday scene presets, expose useful tags/categories, and clean up the main UI so the project feels like a coherent local-first lighting product rather than an RGB settings buffet guarded by a caffeinated raccoon.

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
- Existing OTA support
- Browser firmware update route
- Persistent controller settings
- Central settings model
- Central LED output pipeline
- Diagnostics and power estimate pages
- Phase 2A scene preset system
- Phase 2B favorites, Panic Warm, Off, and Night Guard
- Phase 2C timers and bedtime fade

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

Phase 2D may add mode metadata routes and UI filtering, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. Mode metadata model
2. Mode tags/categories
3. Mode list/filtering UI
4. Mode descriptions and “best used for” labels
5. Harsh/flashing/sleep-safe mode metadata
6. Night Guard integration with tags
7. Practical scene additions and cleanup
8. Scene tags / scene categories if practical
9. Favorite bar polish based on tags/scenes
10. Main UI organization and everyday usability cleanup
11. Reduced-motion and low-glare UI preferences if simple
12. State/API additions for mode metadata
13. Documentation/report update
14. Compile/test report

Do **not** implement yet:

- Smooth transitions
- Palette builder
- Browser live animation preview
- Random scene generator
- Full weekly scheduler
- Day-of-week recurring routines
- QR/NFC scene links
- New lighting modes beyond practical scene presets using existing modes
- Physical controls
- Microphone support
- Light sensor support
- New hardware assumptions

If a later feature looks tempting, document it as deferred. The Phase 3 Glitter Dragon can wait outside. It has snacks.

---

# Phase 2D Main Goal

At the end of Phase 2D, the user should be able to:

1. Understand what each mode is for.
2. Filter modes by useful categories such as Calm, Sleep, Focus, Colorful, Utility, and Flashing.
3. See which modes are blocked by Night Guard.
4. Quickly choose practical scenes such as Study / Focus, Workbench, Movie, Sera Call, and Cozy Goblin Mode.
5. Use a cleaner main UI that separates everyday controls from advanced controls.
6. Keep all previous Phase 1 and Phase 2 features working.
7. Start Phase 3 from a well-organized UI and metadata foundation.

This phase is about **organization, usability, and practical polish**, not new animation algorithms.

---

# 1. Mode Metadata Model

## Goal

Add a compact metadata layer for every lighting mode.

The controller already has many modes, and more modes will be added later. A flat dropdown is going to become painful. Mode metadata gives the UI and future systems a way to understand what each mode is.

## Required Metadata Fields

For each mode, define metadata equivalent to:

```cpp
struct ModeMetadata {
  String id;
  String displayName;
  String description;
  String category;
  bool isAnimated;
  bool isCalm;
  bool isSleepSafe;
  bool isFocusFriendly;
  bool isColorful;
  bool isFlashing;
  bool isUtility;
  bool isBlockedByNightGuard;
};
```

Use a representation that fits the current sketch. If many `String` objects are risky, use `const char*` tables in PROGMEM or compact arrays.

## Required Modes To Cover

Add metadata for all existing modes in the sketch, including likely modes such as:

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

If the actual sketch has additional modes, include them.

If a listed mode does not exist, do not invent it as implemented. Only add metadata for actual modes.

## Required Categories

Use categories such as:

```text
Everyday
Calm
Sleep
Focus
Colorful
Weather / Nature
Utility
Experimental
Flashing
```

A mode can have one primary category and multiple booleans/tags.

## Required Tags

At minimum, support tags or booleans for:

- Calm
- Sleep-safe
- Focus-friendly
- Colorful
- Animated
- Flashing
- Utility
- Experimental

## Metadata Guidance

Suggested metadata examples:

### Solid Color

- Category: Everyday
- Calm: true
- Sleep-safe: true if brightness is low
- Focus-friendly: true
- Animated: false
- Flashing: false

### Strobe

- Category: Flashing
- Calm: false
- Sleep-safe: false
- Focus-friendly: false
- Animated: true
- Flashing: true
- Blocked by Night Guard: true

### Flash

- Category: Flashing
- Flashing: true
- Blocked by Night Guard: true

### Rainbow

- Category: Colorful
- Colorful: true
- Animated: true
- Sleep-safe: false by default

### Slow Wave / Slow Pulse

- Category: Calm
- Calm: true
- Animated: true
- Sleep-safe: maybe true depending on brightness

### Moon Tide / Rain Glass / Lantern Drift

- Category: Calm or Weather / Nature
- Calm: true
- Sleep-safe: true if brightness is low
- Animated: true

## Acceptance Criteria

- Every actual mode has metadata.
- Metadata is compact enough for ESP8266.
- Mode metadata can be used by UI, Night Guard, and API.
- No fake modes are added.

---

# 2. Mode Metadata API

## Goal

Expose mode metadata to the browser UI and future automation.

## Required Endpoint

Add:

```text
/api/modes
```

This endpoint should list available modes and metadata.

## Example Response

```json
{
  "ok": true,
  "count": 5,
  "modes": [
    {
      "id": "solid",
      "name": "Solid Color",
      "category": "Everyday",
      "description": "A steady color or white temperature for normal room lighting.",
      "tags": ["calm", "sleep-safe", "focus", "everyday"],
      "animated": false,
      "flashing": false,
      "blockedByNightGuard": false
    },
    {
      "id": "strobe",
      "name": "Strobe",
      "category": "Flashing",
      "description": "Turns the whole strip on and off repeatedly.",
      "tags": ["animated", "flashing", "utility"],
      "animated": true,
      "flashing": true,
      "blockedByNightGuard": true
    }
  ]
}
```

Keep JSON compact. If descriptions make JSON too large, expose shorter descriptions.

## Optional Endpoint

Add:

```text
/api/modes?category=Calm
```

Filtering server-side is optional. Browser-side filtering is acceptable and likely simpler.

## Acceptance Criteria

- `/api/modes` returns valid JSON.
- It includes all actual modes.
- It includes enough metadata for filtering and labels.
- It does not break `/api/mode` mode-setting endpoint.

---

# 3. Mode Filtering UI

## Goal

Improve mode selection so the user does not have to scroll through one giant unorganized dropdown.

## Required UI Behavior

Add mode filtering to the main UI.

Possible UI approaches:

### Option A — Filter Buttons Above Existing Dropdown

Add buttons/chips:

- All
- Everyday
- Calm
- Sleep
- Focus
- Colorful
- Weather
- Utility
- Flashing

Changing the filter narrows which modes appear in the dropdown or mode list.

### Option B — Mode Cards / List

Show mode cards with category/tag labels and select buttons.

This is nicer but may be larger.

### Option C — Hybrid

Keep the dropdown for compact control, but add filters and a short description panel for the selected mode.

Recommended for ESP8266 simplicity:

- Hybrid: keep dropdown, add filter chips, add selected-mode description.

## Required Filter Behavior

The UI should allow filtering by at least:

- All
- Calm
- Sleep-safe
- Focus
- Colorful
- Flashing
- Utility

If category names differ, document them.

## Night Guard Behavior

If Night Guard is enabled:

- Mark blocked modes clearly.
- Disable Strobe/Flash options if practical.
- Or show a clear note/error when selected.

Do not silently fail.

## Acceptance Criteria

- User can filter modes.
- Selected mode description appears.
- Blocked flashing modes are visible as blocked or produce clear feedback.
- Existing mode selection still works.

---

# 4. Mode Descriptions and “Best Used For” Labels

## Goal

Help the user understand modes without guessing.

## Required Data

Each mode should have a short description.

Examples:

### Solid Color

```text
Steady room lighting using the selected color or white temperature.
```

Best used for:

```text
Everyday, reading, simple warm/cool light.
```

### Slow Pulse

```text
Soft pulsing sections that breathe slowly across the strip.
```

Best used for:

```text
Evening ambience and calm background movement.
```

### Rain Glass

```text
Soft luminous beads and trails like quiet rain on glass.
```

Best used for:

```text
Relaxing, rainy-night ambience, late evening.
```

### Strobe

```text
A strong on/off flashing mode.
```

Best used for:

```text
Utility/signaling only. Not recommended at night.
```

## UI Requirement

When a mode is selected, show:

- Description
- Category
- Tags
- Best used for
- Night Guard blocked warning, if applicable

Keep text concise.

## Acceptance Criteria

- Mode descriptions display in UI.
- Labels are useful but not enormous.
- Flashing modes are clearly identified.

---

# 5. Night Guard Integration With Tags

## Goal

Use mode metadata to improve Night Guard behavior.

## Required Behavior

Update Night Guard harsh-mode blocking to use metadata instead of hardcoded scattered checks where practical.

A mode should be blocked if:

```text
nightGuardEnabled && nightGuardBlockFlashingModes && modeMetadata.isFlashing
```

Optional:

Also block if `isBlockedByNightGuard` is true.

## Required Integration Points

Use Night Guard metadata checks in:

- `/api/mode` mode setting
- Legacy `/set` mode handling
- Scene load
- Favorite load
- Timer final scene/action validation if relevant
- Bedtime fade target validation if relevant

## Acceptance Criteria

- Strobe and Flash remain blocked under Night Guard.
- Metadata is the source of truth for flashing/blocking where practical.
- Safe modes still work.
- Error messages remain clear.

---

# 6. Practical Scene Additions

## Goal

Add or refine practical everyday scenes using existing modes.

Do not add new lighting mode algorithms in this phase. These are presets/scenes using existing capabilities.

## Required Practical Scenes

Add these if not already present:

### Study / Focus

Purpose:

- Homework, desk work, concentration.

Recommended settings:

- Mode: Solid
- White temperature: 4500K–5500K
- Master brightness: 140–200, respecting power/Night Guard caps
- usingWhiteTemperature: true

### Workbench Mode

Purpose:

- Electronics work, soldering, inspection.

Recommended settings:

- Mode: Solid
- White temperature: 5000K–6000K
- Master brightness: 180–220 if safe
- usingWhiteTemperature: true

Important:

- This is a scene, not a new hardware/tool mode.
- No claims about true color rendering accuracy.

### Movie

Purpose:

- Soft backlight for watching movies.

Recommended settings:

- Mode: Solid, Moon Tide, or Lantern Drift depending on available modes
- Color: warm amber, dim blue, or soft neutral
- Master brightness: 30–70

If Movie already exists, refine/check it.

### Sera Call

Purpose:

- Soft flattering video-call lighting.

Recommended settings:

- Mode: Solid
- White temperature: 3000K–4000K
- Master brightness: 90–150
- usingWhiteTemperature: true

Important:

- Avoid weird saturated colors.
- No claims about professional camera lighting.

### Cozy Goblin Mode

Purpose:

- Warm, dim, low-effort comfort lighting.

Recommended settings:

- Mode: Solid or Lantern Drift if available
- White temperature: 2200K–2700K
- Master brightness: 25–60
- usingWhiteTemperature: true

Yes, this name is silly. That is allowed. The firmware deserves joy.

## Built-In Scene Handling

If scenes already exist:

- Do not duplicate exact scene names.
- Add missing scenes.
- Update built-in defaults only if safe.

If built-in scenes are user-editable/deletable, be careful not to overwrite user customizations on every boot.

Recommended:

- Only seed missing built-in practical scenes when scene storage is missing or when user explicitly resets/adds built-ins.

## Acceptance Criteria

- Practical scenes exist or are seeded safely.
- Existing custom scenes are not overwritten accidentally.
- Scenes use existing modes only.
- Scene list remains bounded.

---

# 7. Scene Tags / Scene Categories

## Goal

Make scene organization easier, especially as more scenes are added.

## Optional But Recommended

Add basic scene tags/categories if the Phase 2A scene model can support it without major churn.

Scene metadata fields:

```cpp
String category;
bool isFavoriteCandidate;
bool isSleepSafe;
bool isFocusFriendly;
bool isBuiltin;
```

or a compact tag representation.

Suggested scene categories:

- Everyday
- Sleep
- Focus
- Movie
- Calm
- Utility
- Custom

## UI Use

If implemented, scene UI can show:

- Category label
- Tags
- Filter scenes by category

Do not overbuild drag/drop scene management yet.

## Acceptance Criteria

- Scene tags/categories exist if practical.
- Existing scene save/load still works.
- Missing tag fields default safely.
- UI remains simple.

---

# 8. Favorite Bar Polish

## Goal

Improve the favorite bar from Phase 2B using practical scenes and metadata.

## Required Behavior

Ensure the favorite bar prioritizes everyday actions.

Recommended default order:

1. Off
2. Warm Dim Now
3. Cozy Goblin Mode
4. Sleep
5. Reading or Study / Focus
6. Movie
7. Rain Glass or Moon Tide
8. Workbench or Sera Call, depending on available scenes

Do not exceed the max favorite count.

## Missing Scene Behavior

If a scene does not exist:

- Hide that favorite.
- Or replace with a safe fallback.

Do not show broken buttons.

## Optional UI Improvement

Add tiny tags or icons/text:

- Sleep
- Focus
- Calm
- Utility

Keep it light.

## Acceptance Criteria

- Favorite bar uses the best available practical scenes/actions.
- Broken favorite targets are handled safely.
- Favorite bar remains compact.

---

# 9. Main UI Organization Cleanup

## Goal

Make the main page easier to use now that it has many features.

## Current Problem

The main page may now contain:

- Brightness controls
- Color picker
- White temperature slider
- Mode dropdown
- Mode-specific controls
- Scene controls
- Favorites
- Panic Warm
- Night Guard
- Timers
- Bedtime fade
- Diagnostics/OTA links

Without organization, this becomes a small RGB airplane cockpit. Fun, but terrifying while sleepy.

## Required Organization

Group controls into clear sections:

1. Quick Actions
2. Current Light
3. Color and White Temperature
4. Mode
5. Scenes
6. Timers / Bedtime
7. Night Guard
8. Advanced / Diagnostics

## Collapsible Advanced Controls

If practical, put less-used controls behind collapsible sections:

- Calibration
- Detailed mode settings
- Power/diagnostics links
- Advanced scene management

Do not hide essential controls:

- Favorite bar
- Warm Dim Now
- Off
- Brightness
- Current mode

## Acceptance Criteria

- Main UI feels organized.
- Everyday actions are near the top.
- Advanced controls do not dominate.
- Existing JS still works.
- Mobile layout remains usable.

---

# 10. Current State Summary Card

## Goal

Show a small, clear summary of what the lights are currently doing.

## Required Summary Fields

Show:

- Current mode
- Current scene name, if known/last loaded
- Master brightness
- Effective brightness if Night Guard is capping
- Active timer/fade status if active
- Night Guard status

Example:

```text
Current: Moon Tide · 71% brightness · Night Guard off
Timer: Fade to Sleep in 24 min
```

If Night Guard is capping:

```text
Current: Movie · Master 140 / Effective 80 because Night Guard is active
```

## Acceptance Criteria

- User can glance at page and understand state.
- Timer/Night Guard effects are visible.
- Summary updates after scene/favorite/timer changes.

---

# 11. Reduced-Motion and Low-Glare UI Preferences

## Goal

Add simple UI comfort preferences if feasible.

This is for the web UI only, not LED animations.

## Optional Settings

Add UI preferences:

```cpp
bool uiReducedMotion;
bool uiLowGlareMode;
```

or browser-local settings if easier.

## Behavior

### Reduced Motion

- Reduce UI animations/transitions if any exist.
- Do not affect LED animation modes.

### Low-Glare Mode

- Keep UI dark and lower contrast at night.
- Avoid huge bright white panels.

If the existing UI is already dark and simple, this can be documented as already satisfied or deferred.

## Acceptance Criteria

- UI preference does not complicate firmware heavily.
- If implemented, preference persists or uses browser local storage.
- If deferred, document why.

---

# 12. API State Additions

## Goal

Expose enough metadata for UI organization and future phases.

## Required Additions

Update `/api/state` or related endpoints to include:

- Current mode ID/name
- Current mode category/tags if lightweight
- Last loaded scene ID/name if tracked
- Night Guard effective brightness
- Timer summary if active

Do not bloat `/api/state` with the full mode list. Use `/api/modes` for full metadata.

## Acceptance Criteria

- Main UI can refresh current state summary.
- Full mode list comes from `/api/modes`.
- State JSON remains compact.

---

# 13. Last Loaded Scene Tracking

## Goal

Track the last loaded scene, if useful for UI summary.

## Required Behavior

If a scene is loaded:

- Store `lastLoadedSceneId` and `lastLoadedSceneName` in runtime state.

If user manually changes color/mode/brightness after loading a scene:

- Either clear last loaded scene or mark state as modified.

Recommended simple behavior:

- Track `lastLoadedSceneId`.
- Clear it whenever user manually changes mode/color/temperature/brightness directly.

Do not overbuild dirty scene comparison.

## Persistence

Optional:

- Persist last loaded scene ID if current settings were saved from it.

Not required.

## Acceptance Criteria

- Current state summary can show scene after loading.
- Manual edits do not misleadingly claim the scene is still exact, if clearing is implemented.

---

# 14. Final Phase 2 Documentation Consolidation

## Goal

End Phase 2 with documentation that explains the everyday control system.

## Required Report

Create or update:

```text
BedroomLedController_Phase_2D_Report.md
```

and optionally:

```text
BedroomLedController_Phase_2_Final_Report.md
```

## Required Phase 2D Report Sections

Include:

1. Files changed.
2. Mode metadata model.
3. Mode categories/tags.
4. `/api/modes` endpoint.
5. Mode filtering UI behavior.
6. Night Guard metadata integration.
7. Practical scenes added/refined.
8. Scene tags/categories, if implemented.
9. Favorite bar polish.
10. Main UI organization changes.
11. Current state summary card.
12. UI comfort preferences, if implemented/deferred.
13. Compile/test status.
14. Known limitations.
15. What was intentionally deferred.
16. Recommended next step: Phase 3A smooth transitions.

## Required Phase 2 Final Report Sections, If Created

Include:

1. Phase 2 purpose.
2. Scene presets summary.
3. Favorites/Panic Warm/Night Guard summary.
4. Timers/bedtime fade summary.
5. Mode tags/filtering/practical scene summary.
6. Current everyday workflow.
7. Remaining limitations.
8. Deferred Phase 3+ features.
9. Compile/test status.
10. Recommended Phase 3 starting point.

## Required Honesty

Do not claim:

- Smooth transitions exist.
- Palette builder exists.
- Browser live preview exists.
- Random scene generator exists.
- Weekly scheduler exists.
- QR/NFC links exist.
- New hardware support exists.

unless those features actually exist.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Current Modes and UI

Identify:

- Actual mode names/IDs
- Mode dropdown rendering
- Night Guard blocking checks
- Scene list/default scenes
- Favorite bar implementation
- Timer/bedtime UI
- Current state API response

Do not invent modes that are not in the sketch.

## Step 2 — Add Mode Metadata Table

Create compact metadata for every actual mode.

Add helper functions:

```cpp
const ModeMetadata* findModeMetadataByName(...);
bool modeIsFlashing(...);
bool modeIsSleepSafe(...);
bool modeBlockedByNightGuard(...);
```

Use names that fit the code.

## Step 3 — Add `/api/modes`

Return mode metadata as compact JSON.

## Step 4 — Refactor Night Guard Blocking To Use Metadata

Use `isFlashing` / `blockedByNightGuard` metadata in mode selection, scene load, favorite load, and timer validation where practical.

## Step 5 — Update Mode UI

Add filter chips/buttons and selected-mode description.

Keep existing dropdown working.

## Step 6 — Add/Refine Practical Scenes

Seed missing practical scenes safely.

Do not overwrite user customizations.

## Step 7 — Add Scene Tags/Categories If Practical

Add fields and UI labels only if this does not destabilize scenes.

## Step 8 — Polish Favorite Bar

Update default favorite candidates using practical scenes and actions.

Handle missing targets safely.

## Step 9 — Reorganize Main UI Sections

Group controls into clear sections.

Add current state summary card.

## Step 10 — Add Optional UI Comfort Preferences

Add reduced-motion/low-glare preferences only if simple.

Otherwise document as deferred/already satisfied.

## Step 11 — Update State/Diagnostics

Expose compact summary fields as needed.

## Step 12 — Compile and Report

Compile for D1 mini / ESP8266.

Create/update Phase 2D report and optional Phase 2 final report.

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
- Scene API routes still work.
- Favorite/Night Guard/action routes still work.
- Timer/bedtime routes still work.

## Mode Metadata

- Every actual mode has metadata.
- Strobe is marked flashing/blocked.
- Flash is marked flashing/blocked.
- Calm modes are marked calm.
- Sleep-safe modes are marked appropriately.
- `/api/modes` returns valid JSON.

## Mode Filtering UI

- Filter chips/buttons appear.
- All filter works.
- Calm filter works.
- Sleep-safe filter works.
- Focus filter works.
- Flashing filter works.
- Selected-mode description updates.
- Mode selection still works.

## Night Guard Integration

- Night Guard still blocks Strobe/Flash.
- Blocking uses metadata where practical.
- Scene/favorite/timer attempts to activate blocked flashing modes fail safely.
- Safe modes work while Night Guard is active.

## Practical Scenes

- Study / Focus scene exists or is refined.
- Workbench Mode scene exists.
- Movie scene exists/refined.
- Sera Call scene exists.
- Cozy Goblin Mode exists.
- Existing custom scenes are not overwritten.
- Scene count limit is respected.

## Favorite Bar

- Favorite bar still appears.
- Favorites load correctly.
- Missing targets are hidden/handled safely.
- Warm Dim Now and Off still work.
- Favorite defaults are sensible.

## Main UI Organization

- Quick actions are near the top.
- Current state summary appears.
- Color/temperature/mode controls remain usable.
- Scene controls remain usable.
- Timer/bedtime controls remain usable.
- Night Guard controls remain usable.
- Diagnostics/OTA links remain accessible.
- Mobile layout remains usable.

## Current State Summary

- Shows current mode.
- Shows last loaded scene if tracked.
- Shows brightness/effective brightness.
- Shows timer status if active.
- Shows Night Guard status.
- Updates after scene/favorite/timer changes.

## Regression

Verify existing basics still work:

- Color picker
- White temperature slider
- Master brightness slider
- Mode dropdown
- Scene save/load/rename/delete
- Favorite loading
- Warm Dim Now
- Off action
- Night Guard
- Timers
- Bedtime fade
- Diagnostics page
- Power estimator
- LED tests
- OTA routes

---

# Phase 2D Non-Goals Reminder

Do not implement these yet:

- Smooth transitions
- Palette builder
- Browser live preview
- Surprise Me generator
- Weekly scheduler
- Day-of-week recurring routines
- QR/NFC scene links
- New lighting mode algorithms
- Physical controls
- Music-reactive behavior
- Room-aware brightness

If tempted, write it in the report as deferred.

The Phase 3 glitter cannons are loaded, but they are not authorized to fire yet.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 2D complete.

Files changed:
- ...

Implemented:
- ...

Mode metadata:
- ...

Mode filtering UI:
- ...

Night Guard metadata integration:
- ...

Practical scenes:
- ...

Favorite/UI polish:
- ...

State/API/diagnostics:
- ...

Documentation:
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
- Phase 3A — smooth transitions.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim smooth transitions, palettes, browser preview, random scene generation, weekly scheduling, QR/NFC links, physical controls, microphone behavior, or light sensor behavior unless they actually exist.

---

# One-Sentence Mission Summary

Add mode tags, filtering, practical everyday scenes, and UI organization so the Bedroom LED Controller becomes easy to understand and pleasant to use before Phase 3 starts adding the premium visual polish and fancy lighting wizardry.

