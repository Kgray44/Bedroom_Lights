# Bedroom LED Controller — Phase 2B Codex Prompt

## Phase 2B Title

**Favorite Scene Bar + Panic Warm + Night Guard**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 2B** of the larger upgrade plan.

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
- Preserved OTA/browser update routes

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

The goal of Phase 2B is to make the controller much faster and safer for everyday use by adding:

1. A favorite scene/mode quick-access bar
2. A **Panic Warm / Warm Dim Now** comfort action
3. A **Night Guard** safety/comfort mode
4. Basic API endpoints and UI integration for those features
5. Persistence and documentation updates

This phase should make the controller feel less like “open the web page and operate the RGB nuclear submarine” and more like “tap one cozy button and the room behaves.”

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

Phase 2B may add new routes for favorites, Panic Warm, and Night Guard, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. Favorite quick-access bar
2. Favorite scene/mode configuration storage
3. Built-in default favorites
4. Favorite load/apply behavior
5. Panic Warm / Warm Dim Now action
6. Panic Warm API endpoint
7. Panic Warm UI button
8. Night Guard settings model
9. Night Guard UI controls
10. Night Guard enforcement for brightness and harsh modes
11. Night Guard API endpoints
12. State/diagnostics integration
13. Persistence updates
14. Documentation/report update
15. Compile/test report

Do **not** implement yet:

- Timer actions
- Bedtime fade scheduler
- Weekly scheduler
- Smooth transitions
- Palette builder
- Browser live animation preview
- Random scene generator
- QR/NFC scene links
- New lighting modes
- Physical controls
- Microphone support
- Light sensor support
- New hardware assumptions

If you see a tempting later feature, document it as deferred. The Timer Goblin must wait in the hallway with a juice box.

---

# Phase 2B Main Goal

At the end of Phase 2B, the user should be able to:

1. Quickly activate common scenes from a favorite bar.
2. Use one obvious **Warm Dim Now** / **Panic Warm** action to immediately make the room comfortable.
3. Enable **Night Guard** to cap brightness and protect against harsh/flashing modes.
4. Configure Night Guard’s basic behavior.
5. See favorite/Night Guard state in `/state`, `/api/state`, and diagnostics if practical.
6. Use local API endpoints for favorites, Panic Warm, and Night Guard.
7. Reboot the controller and keep favorite/Night Guard settings.

---

# 1. Favorite Quick-Access Bar

## Goal

Add a fast row of common lighting actions near the top of the main web UI.

The favorite bar should reduce the everyday workflow from:

> open page → choose mode → set color → adjust brightness → tweak controls → realize it is midnight and everything is too bright

into:

> tap Sleep

Tiny embedded luxury. Very civilized.

## Required Behavior

The favorite bar should show a small set of favorite scenes or actions.

Recommended default favorites:

1. Off
2. Warm Dim
3. Reading
4. Sleep
5. Movie
6. Focus
7. Rain Glass, if that scene/mode exists
8. Moon Tide, if that scene/mode exists

If a default favorite references a scene that does not exist, either:

- Hide that favorite, or
- Fall back to a safe built-in action, or
- Show it disabled with a helpful reason.

Recommended first version:

- Hide favorites whose target scene does not exist.

## Favorite Types

Favorites may point to different target types.

Support at least:

### Scene Favorite

Loads a saved scene by scene ID.

Example:

```json
{
  "id": "fav-sleep",
  "label": "Sleep",
  "type": "scene",
  "target": "sleep"
}
```

### Action Favorite

Runs a built-in action such as Off or Panic Warm.

Example:

```json
{
  "id": "fav-off",
  "label": "Off",
  "type": "action",
  "target": "off"
}
```

Optional for this phase:

### Mode Favorite

Directly selects a mode, optionally preserving current color/brightness.

Example:

```json
{
  "id": "fav-rain-glass-mode",
  "label": "Rain Glass",
  "type": "mode",
  "target": "Rain Glass"
}
```

Mode favorites are optional if scene favorites cover the need.

## UI Requirements

Place favorite bar near the top of the main page, ideally after the current status/brightness area and before deeper controls.

Each favorite should be a clear button.

Suggested button content:

- Label
- Optional tiny color swatch for scene favorites
- Optional small mode name

The bar should wrap nicely on mobile.

Do not create a huge dashboard monster. This is quick access, not a NASA launch panel for a lamp.

## Acceptance Criteria

- Favorite bar appears on main page.
- Default favorites are useful.
- Scene favorites load scenes.
- Action favorites run actions.
- Missing scenes do not break the UI.
- Existing controls still work.

---

# 2. Favorite Configuration Storage

## Goal

Store favorite bar configuration persistently.

## Storage Recommendation

Because favorites are UI/controller configuration, they can be stored in `/settings.json` or a separate file.

Recommended for simplicity:

- Store favorite configuration in the existing settings JSON if small.

Alternative:

- Store in `/favorites.json` if the architecture is cleaner.

Either approach is acceptable if documented.

## Required Fields

Suggested setting shape:

```json
{
  "favorites": [
    {
      "id": "fav-off",
      "label": "Off",
      "type": "action",
      "target": "off"
    },
    {
      "id": "fav-warm-dim",
      "label": "Warm Dim",
      "type": "scene",
      "target": "warm-dim"
    }
  ]
}
```

## Max Favorites

Set a reasonable maximum.

Recommended:

```text
MAX_FAVORITES = 8
```

Acceptable:

```text
6–10
```

Keep it compact.

## Default Favorites

If favorites are missing from settings, create defaults based on available scenes/actions.

Default candidates:

- Off action
- Panic Warm action or Warm Dim scene
- Reading scene
- Sleep scene
- Movie scene
- Focus scene
- Rain Glass scene if available
- Moon Tide scene if available

## Configuration UI

Full drag-and-drop favorite editing is not required in Phase 2B.

Minimum acceptable:

- Default favorite bar works.
- Favorite config is stored internally.
- API supports listing favorites.

Better if practical:

- Allow user to reset favorites to default.
- Allow user to assign a scene to a favorite slot.

Do not overbuild if it threatens stability.

## Acceptance Criteria

- Favorites persist after reboot.
- Defaults are created if missing.
- Favorite count is bounded.
- Missing target scenes are handled safely.

---

# 3. Favorite API Endpoints

## Goal

Expose favorite bar behavior through local API endpoints.

## Required Endpoints

Implement or normalize:

```text
/api/favorites
/api/favorites/load?id=...
/api/favorites/reset
```

Optional if editing favorites in this phase:

```text
/api/favorites/set?slot=0&type=scene&target=sleep&label=Sleep
```

## `/api/favorites`

Returns favorite list.

Example:

```json
{
  "ok": true,
  "count": 6,
  "maxFavorites": 8,
  "favorites": [
    {
      "id": "fav-sleep",
      "label": "Sleep",
      "type": "scene",
      "target": "sleep",
      "available": true
    }
  ]
}
```

## `/api/favorites/load?id=...`

Loads/runs a favorite.

Behavior:

- Find favorite by ID.
- If type is `scene`, load scene by ID.
- If type is `action`, run matching built-in action.
- If type is `mode`, set mode if mode favorites are supported.
- Return JSON success/error.

## `/api/favorites/reset`

Reset favorites to built-in defaults.

## Response Format

Use existing JSON convention from Phase 1D:

Success:

```json
{
  "ok": true,
  "message": "Favorite loaded"
}
```

Error:

```json
{
  "ok": false,
  "error": "Favorite target scene not found"
}
```

## Acceptance Criteria

- Favorites list endpoint works.
- Loading a favorite works.
- Reset favorites works if implemented.
- Invalid favorite IDs return JSON error.

---

# 4. Panic Warm / Warm Dim Now Action

## Goal

Add one obvious comfort/safety action that immediately sets the lights to a warm, dim, non-harsh state.

This should be available even if scenes are broken or missing.

This is the “my brain is mashed potatoes, make room cozy now” button.

## Required Behavior

Add a built-in action called one of:

- `panicWarm`
- `warmDimNow`
- `warmNow`

User-facing label:

```text
Warm Dim Now
```

or:

```text
Panic Warm
```

Recommended user-facing label:

```text
Warm Dim Now
```

because it sounds less like the LED strip just filed an emergency report.

## Action Behavior

When triggered:

1. Stop any diagnostic LED test if active.
2. Stop harsh/flashing mode if active.
3. Set mode to Solid.
4. Set color source to white temperature if supported.
5. Set white temperature to warm value.
6. Set brightness to low safe value.
7. Mark settings dirty.
8. Render updated state on next frame.

Recommended values:

```text
Mode: Solid
White temperature: 2200K–2700K, or closest supported warm value
Master brightness: 30–60
RGB fallback: warm amber such as #FF9A45 or equivalent
```

Choose exact values based on existing conversion helpers and current project conventions.

## Important Rule

Panic Warm should not depend on a saved scene.

It can load Warm Dim scene if available, but there must be a direct fallback so it works even if `/scenes.json` is missing/corrupt.

Recommended:

- Implement as direct action.
- Optionally use same values as Warm Dim built-in scene.

## UI Requirements

Add a clear button near the top of the main UI.

Suggested placement:

- Next to favorite bar
- Near master brightness
- In a comfort/safety strip

Button label:

```text
Warm Dim Now
```

Optional subtext:

```text
Sets a safe warm low light instantly.
```

Do not bury this inside diagnostics.

This is an everyday comfort button.

## API Endpoint

Add:

```text
/api/panic-warm
```

or:

```text
/api/action?name=warmDimNow
```

Recommended:

```text
/api/action?name=warmDimNow
```

plus a convenient alias if desired:

```text
/api/panic-warm
```

Use consistent JSON response format.

## Acceptance Criteria

- Warm Dim Now button appears on main UI.
- Triggering it immediately sets a warm dim solid state.
- It works even if scene storage has a problem.
- It stops diagnostic tests if active.
- It persists as current state according to existing settings behavior.
- API trigger works.

---

# 5. Off Action

## Goal

Add or normalize a clean Off action for the favorite bar and API.

## Required Behavior

The Off action should:

- Turn LEDs off.
- Keep web UI active.
- Keep OTA active.
- Not put the microcontroller to sleep.
- Preserve ability to turn lights back on.

Implementation options:

### Option A — Mode-based Off

Add an internal off flag or mode.

### Option B — Brightness-based Off

Set master brightness to `0` while preserving previous mode/color.

### Option C — Solid black

Set mode to Solid and RGB to 0/0/0.

Recommended:

Use whichever approach fits the existing architecture best, but document it.

For future usability, preserving previous mode/color is nice, but not required for Phase 2B.

## API Endpoint

Support through:

```text
/api/action?name=off
```

or existing `/api/off` if Phase 1D added it.

## Acceptance Criteria

- Off favorite works.
- LEDs turn off.
- Web/OTA remain active.
- State/API accurately reflects off behavior.

---

# 6. Night Guard Settings Model

## Goal

Add a global Night Guard mode that makes late-night use safer and calmer.

Night Guard is not a lighting mode. It is a global behavior layer.

## Required Settings

Add settings fields such as:

```cpp
bool nightGuardEnabled;
uint8_t nightGuardMaxBrightness;
bool nightGuardBlockFlashingModes;
bool nightGuardForceWarmOnPanic;
bool nightGuardPreferWarmTemperature;
```

Recommended defaults:

```text
nightGuardEnabled = false
nightGuardMaxBrightness = 80
nightGuardBlockFlashingModes = true
nightGuardForceWarmOnPanic = true
nightGuardPreferWarmTemperature = false
```

Optional future fields, but do not fully implement scheduling yet:

```cpp
bool nightGuardScheduled;
uint16_t nightGuardStartMinutes;
uint16_t nightGuardEndMinutes;
```

Do not implement time-based scheduling in Phase 2B unless already trivial and explicitly isolated. Timers/schedules are Phase 2C/2D.

For Phase 2B, Night Guard can be manually toggled.

## Persistence

Night Guard settings should persist after reboot.

Store in `/settings.json` or equivalent.

## Acceptance Criteria

- Night Guard has central settings.
- Settings persist.
- Missing fields use defaults.
- Existing settings still load.

---

# 7. Night Guard Brightness Enforcement

## Goal

When Night Guard is enabled, cap the effective brightness to a configured maximum.

## Required Behavior

If Night Guard is enabled:

```text
actual output brightness = min(masterBrightness, nightGuardMaxBrightness)
```

Important:

- Do not necessarily overwrite `masterBrightness` every frame.
- Prefer applying the cap in the output pipeline as an effective brightness value.

Reason:

If the user had master brightness at 180 and enables Night Guard max 80, output should cap to 80. If Night Guard is disabled later, brightness can return to 180.

This is more flexible than permanently crushing the saved brightness like a sad little photon pancake.

## State Reporting

Expose both:

- `masterBrightness`
- `effectiveBrightness`
- `nightGuardMaxBrightness`
- `nightGuardEnabled`

Example:

```json
{
  "masterBrightness": 180,
  "effectiveBrightness": 80,
  "nightGuardEnabled": true,
  "nightGuardMaxBrightness": 80
}
```

## UI Behavior

When Night Guard is enabled and the current brightness is above the cap, show a subtle note:

```text
Night Guard is limiting output to 80 / 255.
```

## Acceptance Criteria

- Night Guard caps visible output brightness.
- Master brightness value is not destroyed unless the user explicitly changes it.
- State/API report effective brightness.
- Disabling Night Guard restores normal brightness behavior.

---

# 8. Night Guard Harsh Mode Protection

## Goal

Prevent or warn against harsh/flashing modes while Night Guard is enabled.

## Harsh Modes

Treat these as harsh/flashing:

- Strobe
- Flash

Optional, depending on project behavior:

- Very fast Rainbow
- Very fast Chase
- Any mode marked with a `flashing` or `harsh` tag if tags exist

Minimum requirement:

- Protect against Strobe and Flash.

## Required Behavior

When Night Guard is enabled and the user tries to select a blocked harsh mode:

Recommended first behavior:

- Reject the mode change.
- Return JSON error or warning.
- Keep current mode.
- Show UI message.

Alternative acceptable behavior:

- Allow but force safe brightness and show warning.

Recommended:

- Block Strobe and Flash while Night Guard is enabled.

## API Response Example

```json
{
  "ok": false,
  "error": "Night Guard blocks Strobe while enabled"
}
```

## UI Behavior

If Night Guard is enabled:

- Visually mark Strobe/Flash as blocked or disabled in the dropdown if practical.
- Or show an error when selected.

Do not rewrite the whole mode UI if that is too much.

## Acceptance Criteria

- Strobe/Flash cannot be activated while Night Guard blocking is enabled.
- Existing mode selection still works for safe modes.
- API returns useful error/warning.
- UI feedback is visible.

---

# 9. Night Guard Warm Preference

## Goal

Optionally support warm-safe behavior while Night Guard is enabled.

## Required Minimum

Night Guard must support brightness cap and harsh-mode blocking.

Warm preference is optional but useful.

## If Implemented

When `nightGuardPreferWarmTemperature` is enabled:

- Panic Warm uses warm white values.
- If user selects a very cool white temperature while Night Guard is enabled, either:
  - Allow it but show a note, or
  - Clamp to a warmer maximum, such as 3000K–3500K.

Recommended for Phase 2B:

- Do not forcibly clamp normal color/temperature choices yet.
- Only make Panic Warm and built-in Night Guard recommendations warm.

## Acceptance Criteria

- Warm preference does not surprise users by rewriting colors unexpectedly.
- Panic Warm remains warm.
- Behavior is documented.

---

# 10. Night Guard UI Controls

## Goal

Expose Night Guard controls in the main UI or a small settings section.

## Required Controls

Add:

- Enable/disable Night Guard toggle
- Night Guard max brightness slider
- Block flashing modes toggle

Optional:

- Prefer warm at night toggle
- Small explanation text

## Suggested UI Text

```text
Night Guard
Caps brightness and blocks harsh flashing modes for late-night use.
```

Controls:

```text
[ ] Enable Night Guard
Max brightness: [80 / 255]
[x] Block Strobe and Flash
```

## UI Placement

Suggested:

- Main page settings/comfort section
- Near master brightness/favorites
- Not buried only in diagnostics

This is an everyday safety feature.

## Acceptance Criteria

- User can enable/disable Night Guard.
- User can set max brightness.
- User can toggle flashing-mode blocking.
- Settings persist.
- UI reflects current Night Guard state from `/state` or `/api/state`.

---

# 11. Night Guard API Endpoints

## Goal

Expose Night Guard through the local API.

## Required Endpoints

Implement or normalize:

```text
/api/nightguard
/api/nightguard/set?enabled=1&maxBrightness=80&blockFlashing=1
```

Alternative:

Use `/api/action` and `/api/state`, but dedicated endpoint is cleaner.

## `/api/nightguard`

Returns current settings:

```json
{
  "ok": true,
  "enabled": true,
  "maxBrightness": 80,
  "blockFlashing": true,
  "effectiveBrightness": 80
}
```

## `/api/nightguard/set`

Updates one or more values.

Accepted query parameters:

- `enabled=0|1`
- `maxBrightness=0..255`
- `blockFlashing=0|1`
- optionally `preferWarm=0|1`

Validation:

- Reject non-numeric brightness.
- Clamp max brightness to 0–255.
- Parse booleans safely.

## Acceptance Criteria

- Night Guard API returns current settings.
- Night Guard API updates settings.
- Invalid values return JSON errors.
- Changes mark settings dirty.

---

# 12. State and Diagnostics Integration

## Goal

Make favorite/Panic Warm/Night Guard state visible to existing state and diagnostics tools.

## `/api/state` / `/state` Additions

Add fields such as:

```json
{
  "nightGuardEnabled": true,
  "nightGuardMaxBrightness": 80,
  "nightGuardBlockFlashingModes": true,
  "effectiveBrightness": 80,
  "favoritesCount": 6
}
```

If using different names, keep them consistent.

## Diagnostics Additions

If practical, add to `/diagnostics` and/or `/api/diagnostics`:

- Night Guard enabled/disabled
- Night Guard max brightness
- Whether brightness is currently capped
- Favorite count
- Last favorite/action used, optional

Do not bloat diagnostics excessively.

## Acceptance Criteria

- Main UI can initialize Night Guard controls from state.
- Diagnostics can show Night Guard status.
- Effective brightness is visible somewhere.

---

# 13. Central Action System

## Goal

Add a small central action system so Favorites, Panic Warm, Off, and future physical controls can call the same logic.

This is a software hook for future work, not hardware support.

## Suggested Actions

Support central action names:

```text
off
warmDimNow
loadFavorite
```

Optional future-ready actions:

```text
nextFavorite
previousFavorite
brightnessUp
brightnessDown
```

Do not implement hardware controls.

## Recommended Function

```cpp
bool runActionByName(const String& actionName);
```

or equivalent.

For now, implement at least:

- `off`
- `warmDimNow`

Favorites can call this for action-type favorites.

## Acceptance Criteria

- Panic Warm uses central action path.
- Off favorite uses central action path.
- Future Phase 4 physical controls could call these actions later.
- No hardware code is added.

---

# 14. Persistence Updates

## Goal

Persist favorite and Night Guard settings.

## Required Stored Fields

Store:

- Favorite list/configuration, unless using separate favorites file
- Night Guard enabled
- Night Guard max brightness
- Night Guard block flashing setting
- Night Guard warm preference, if implemented

## Backward Compatibility

If existing settings file lacks these fields:

- Use safe defaults.
- Do not treat file as corrupt.
- Save normalized settings on next settings write.

## Acceptance Criteria

- Night Guard settings persist after reboot.
- Favorites persist after reboot.
- Missing fields default safely.
- Existing Phase 1/2A settings still load.

---

# 15. Interaction With Scene Presets

## Goal

Make favorites and Panic Warm work well with Phase 2A scenes.

## Required Behavior

Scene favorites should load scenes through the same scene load function added in Phase 2A.

Panic Warm should not require a scene, but can optionally update the current state in a way that could later be saved as a scene.

Night Guard should affect scene recall safely:

- If a scene has brightness above Night Guard max, output should be capped by Night Guard.
- The scene's saved brightness should not be modified just because Night Guard is active.
- If a scene uses Strobe/Flash and Night Guard blocks flashing modes, loading the scene should fail or be redirected to a safe fallback.

Recommended behavior for blocked scene load:

- Return JSON error:

```json
{
  "ok": false,
  "error": "Night Guard blocks this scene because it uses Flash"
}
```

Alternative:

- Load scene but switch mode to Solid warm dim and warn.

Recommended:

- Reject blocked harsh scene loads to avoid surprising behavior.

## Acceptance Criteria

- Scene favorites load properly.
- Night Guard caps high-brightness scenes without altering saved scene data.
- Night Guard blocks scenes using Strobe/Flash if blocking is enabled.

---

# 16. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_2B_Report.md
```

The report should include:

1. Files changed.
2. Favorite bar behavior.
3. Favorite storage format and defaults.
4. Favorite API endpoints.
5. Panic Warm / Warm Dim Now behavior.
6. Off action behavior.
7. Night Guard settings and defaults.
8. Night Guard brightness enforcement behavior.
9. Night Guard harsh-mode blocking behavior.
10. State/diagnostics additions.
11. Persistence changes.
12. Interaction with scenes.
13. Compile/test status.
14. Known limitations.
15. What was intentionally deferred.
16. Recommended next step: Phase 2C timer actions and bedtime fade.

## Required Honesty

Do not claim scheduling exists unless implemented.

Do not claim smooth transitions exist unless Phase 3 has implemented them.

Do not claim physical buttons or sensors exist.

Do not claim scene QR/NFC links exist.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Phase 2A Scene System

Identify:

- Scene list/load functions
- Scene IDs for default scenes
- Scene API endpoints
- Current UI scene rendering
- Current state mutation functions
- Existing Off behavior, if any

Do not rewrite the scene system.

## Step 2 — Add Central Action Helpers

Add or normalize:

- `runActionByName()`
- `runWarmDimNow()`
- `runOffAction()`

Keep them independent from scene storage.

## Step 3 — Add Night Guard Settings

Add Night Guard fields to central settings.

Add defaults, load/save handling, and `/state` fields.

## Step 4 — Add Effective Brightness Helper

Add helper such as:

```cpp
uint8_t getEffectiveBrightness();
```

This should return:

```cpp
settings.nightGuardEnabled
  ? min(settings.masterBrightness, settings.nightGuardMaxBrightness)
  : settings.masterBrightness;
```

Then update output pipeline to use effective brightness instead of raw master brightness.

## Step 5 — Add Harsh Mode Blocking

Add helper:

```cpp
bool isModeBlockedByNightGuard(const String& modeName);
```

Use it in:

- Mode API endpoint
- `/set` mode handling
- Scene load
- Favorite load if target would activate blocked mode

## Step 6 — Add Favorite Model and Defaults

Add favorite data structure and default favorite creation based on existing scenes/actions.

## Step 7 — Add Favorite API

Add:

- `/api/favorites`
- `/api/favorites/load`
- `/api/favorites/reset`

Optional editing endpoint if simple.

## Step 8 — Add Panic Warm and Off API

Add:

- `/api/action?name=warmDimNow`
- `/api/action?name=off`

Optionally add alias:

- `/api/panic-warm`

## Step 9 — Add UI Controls

Update main UI:

- Favorite bar
- Warm Dim Now button
- Night Guard section/toggle/settings

## Step 10 — Update Diagnostics, If Practical

Add compact Night Guard/favorite status.

## Step 11 — Compile and Report

Compile for D1 mini / ESP8266.

Create/update Phase 2B report.

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
- Scene API routes from Phase 2A still work.

## Favorite Bar

- Favorite bar appears on main page.
- Off favorite works.
- Warm Dim favorite/action works.
- Scene favorites load scenes.
- Missing scene targets are handled safely.
- Favorite list persists after reboot.
- `/api/favorites` returns valid JSON.
- `/api/favorites/load?id=...` works.
- Invalid favorite ID returns JSON error.

## Panic Warm / Warm Dim Now

- Button appears on main page.
- API action works.
- Sets mode to Solid.
- Sets warm color/temperature.
- Sets low brightness.
- Stops diagnostic test if active.
- Does not require scene storage.
- State reflects new values.

## Off Action

- Off action turns LEDs off.
- Web UI remains active.
- OTA remains active.
- State reflects off behavior.
- Lights can be turned back on.

## Night Guard

- Night Guard toggle works.
- Max brightness slider works.
- Block flashing toggle works.
- Settings persist after reboot.
- Effective brightness is capped when Night Guard is enabled.
- Master brightness is not permanently overwritten by the cap.
- Disabling Night Guard restores normal brightness behavior.
- Strobe/Flash are blocked if blocking is enabled.
- Safe modes still work.
- Scenes using blocked modes are handled safely.

## State / Diagnostics

- `/state` or `/api/state` includes Night Guard status.
- Effective brightness is exposed.
- Diagnostics shows Night Guard status if implemented.
- Favorite count is exposed if implemented.

## Persistence

- Existing settings still load.
- Night Guard fields default safely if missing.
- Favorite fields default safely if missing.
- No repeated flash writes from UI refreshes.

## Regression

Verify existing basics still work:

- Color picker
- White temperature slider
- Mode dropdown
- Master brightness slider
- Scene save/load
- Diagnostics page
- Power estimator
- LED tests
- OTA routes

---

# Phase 2B Non-Goals Reminder

Do not implement these yet:

- Timer actions
- Bedtime fade
- Weekly scheduler
- Smooth transitions
- Palette builder
- Browser live preview
- Surprise Me generator
- QR/NFC scene links
- New lighting modes
- Physical controls
- Music-reactive behavior
- Room-aware brightness

If tempted, write it in the report as deferred.

The firmware raccoon is allowed a blanket and cocoa, but not a feature bazooka.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 2B complete.

Files changed:
- ...

Implemented:
- ...

Favorite bar:
- ...

Panic Warm / Warm Dim Now:
- ...

Off action:
- ...

Night Guard:
- ...

Scene interaction:
- ...

State/API/diagnostics:
- ...

Persistence:
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
- Phase 2C — timer actions and bedtime fade.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim timers, schedules, transitions, palettes, QR/NFC, physical controls, microphone behavior, or light sensor behavior unless they actually exist.

---

# One-Sentence Mission Summary

Add the everyday comfort layer: a favorite bar for one-tap scenes, a direct Warm Dim Now action for immediate cozy rescue, and Night Guard to cap brightness and block harsh flashing modes before the bedroom becomes a tiny RGB interrogation chamber.

