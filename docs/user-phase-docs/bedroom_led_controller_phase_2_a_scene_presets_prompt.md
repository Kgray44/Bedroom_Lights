# Bedroom LED Controller — Phase 2A Codex Prompt

## Phase 2A Title

**Scene Preset Storage and Recall**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 2A** of the larger upgrade plan.

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

The goal of Phase 2A is to add a real **scene preset system**.

A scene is a named saved lighting configuration that can be recalled later with one click or one API call.

This phase should make the controller feel much more like a finished bedroom lighting product. The user should be able to set the lights exactly how they like them, save that configuration as a scene, and recall it later without manually rebuilding the whole vibe like a raccoon assembling IKEA furniture in the dark.

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

Phase 2A may add new routes for scenes, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. Scene data model
2. Scene storage in LittleFS or existing filesystem/persistence layer
3. Built-in default scenes
4. Save current state as scene
5. Recall/load scene
6. Rename scene
7. Delete scene
8. List scenes
9. Scene UI on the main page or a dedicated scene section/page
10. Scene API endpoints
11. Scene compatibility with existing central settings/output pipeline
12. Documentation/report update
13. Compile/test report

Do **not** implement yet:

- Favorite scene bar / quick access row beyond a basic scene list
- Panic Warm button
- Night Guard
- Timers
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

If a later feature seems tempting, document it as deferred. The Scene Goblin is allowed in. The Scheduler Goblin is not.

---

# Phase 2A Main Goal

Add a persistent scene preset system where a scene can save and recall the current lighting state.

At the end of Phase 2A, the user should be able to:

1. Adjust the lights using existing controls.
2. Save the current configuration as a named scene.
3. See a list of saved scenes.
4. Recall a scene.
5. Rename a scene.
6. Delete a scene.
7. Use API endpoints to list/save/load/rename/delete scenes.
8. Reboot the controller and still have scenes available.

---

# 1. Scene Data Model

## Goal

Create a compact scene data model that stores the current lighting state without duplicating unnecessary runtime-only information.

## Required Scene Fields

A scene should include at least:

```cpp
struct ScenePreset {
  String id;
  String name;
  String mode;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint16_t whiteKelvin;
  bool usingWhiteTemperature;
  uint8_t masterBrightness;

  // Existing mode-specific settings from the current controller state.
  uint16_t strobeDelayMs;
  uint16_t flashOffDelayMs;
  uint16_t chaseLoopSeconds;
  uint16_t waveLoopSeconds;
  uint16_t slowPulseSeconds;
  uint8_t slowPulseCount;
  uint16_t rainbowLoopSeconds;

  // Optional, if these are part of user-facing output look.
  bool gammaEnabled;
  uint8_t redGain;
  uint8_t greenGain;
  uint8_t blueGain;
};
```

Use actual setting names from the current sketch.

If some mode-specific fields do not exist in the current sketch, do not invent fake ones.

If the current sketch has additional mode settings, include them if they meaningfully affect the scene.

## Scene ID

Each scene should have a stable ID.

Recommended ID format:

- Lowercase
- URL-safe
- No spaces
- Generated from scene name plus numeric suffix if needed

Examples:

```text
warm-dim
reading
movie
rain-glass
custom-1
```

Scene IDs should remain stable when the user renames the display name, unless renaming IDs is explicitly handled.

Simpler first version:

- Generate ID from name at creation.
- Keep ID stable afterward.

## Scene Name

Scene name is user-facing.

Requirements:

- Non-empty
- Trim whitespace
- Reasonable max length, such as 24–32 characters
- Stored in JSON safely
- Displayed in UI

## Acceptance Criteria

- Scene model captures enough state to restore a lighting configuration.
- Scene model avoids runtime-only noise like uptime/free heap/diagnostic state.
- Scene IDs are stable and safe for API use.

---

# 2. Scene Storage

## Goal

Persist scenes across reboot.

## Preferred Storage

Use LittleFS or the existing filesystem/persistence layer from Phase 1.

Recommended file:

```text
/scenes.json
```

Do not store all scenes inside `/settings.json` unless the existing architecture strongly favors it. Separate files are cleaner:

- `/settings.json` = current controller settings
- `/scenes.json` = saved scene presets

## Storage Format

Recommended JSON shape:

```json
{
  "version": 1,
  "scenes": [
    {
      "id": "warm-dim",
      "name": "Warm Dim",
      "mode": "Solid",
      "red": 255,
      "green": 160,
      "blue": 80,
      "whiteKelvin": 2700,
      "usingWhiteTemperature": true,
      "masterBrightness": 45,
      "strobeDelayMs": 500,
      "flashOffDelayMs": 1000,
      "chaseLoopSeconds": 5,
      "waveLoopSeconds": 15,
      "slowPulseSeconds": 20,
      "slowPulseCount": 5,
      "rainbowLoopSeconds": 10
    }
  ]
}
```

Use compact JSON if memory becomes tight.

## Max Scene Count

Set a reasonable maximum scene count for ESP8266.

Recommended:

```text
MAX_SCENES = 16
```

Acceptable range:

```text
8–24
```

Do not allow unlimited scene creation.

This is a microcontroller, not a cloud database wearing tiny socks.

## Load Behavior

During setup:

1. Initialize defaults.
2. Load `/scenes.json` if it exists.
3. If missing, create/load built-in default scenes.
4. If corrupt, fall back to built-in default scenes and track an error status.
5. Do not crash if the file is missing/corrupt/incomplete.

## Save Behavior

When scenes are created/renamed/deleted:

- Save scenes to `/scenes.json`.
- Debounce if multiple operations can happen quickly.
- Avoid saving every animation frame.

A scene save operation may be immediate because it is user-triggered and uncommon.

## Acceptance Criteria

- Scenes persist after reboot.
- Missing scene file creates/loads defaults.
- Corrupt scene file falls back safely.
- Scene count is bounded.
- Scene storage does not interfere with settings storage.

---

# 3. Built-In Default Scenes

## Goal

Provide useful starter scenes so the UI is not empty after first upload.

## Required Built-In Scenes

Create these defaults if no `/scenes.json` exists:

### Warm Dim

Purpose:

- General cozy low light.

Suggested settings:

- Mode: Solid
- White temperature: 2700K or closest supported warm value
- Master brightness: 40–60
- usingWhiteTemperature: true

### Reading

Purpose:

- Comfortable reading/homework light.

Suggested settings:

- Mode: Solid
- White temperature: 3500K–4500K
- Master brightness: 120–180
- usingWhiteTemperature: true

### Sleep

Purpose:

- Very dim bedtime light.

Suggested settings:

- Mode: Solid or Satin Breathing only if that mode already exists
- White temperature: 2200K–2700K
- Master brightness: 10–35
- usingWhiteTemperature: true

### Movie

Purpose:

- Soft low backlight.

Suggested settings:

- Mode: Solid or Moon Tide if available
- RGB: soft amber or soft blue depending on existing modes
- Master brightness: 30–70

### Focus

Purpose:

- Brighter stable work light.

Suggested settings:

- Mode: Solid
- White temperature: 4500K–5500K
- Master brightness: 140–200
- usingWhiteTemperature: true

### Rain Glass

Purpose:

- Existing calm animated ambience.

Suggested settings:

- Mode: Rain Glass, if available
- Master brightness: 80–140
- Active color: cool soft blue/white

### Moon Tide

Purpose:

- Existing calm animated ambience.

Suggested settings:

- Mode: Moon Tide, if available
- Master brightness: 60–120
- Active color: cool moonlight tone

### Lantern Drift

Purpose:

- Existing warm calm animated ambience.

Suggested settings:

- Mode: Lantern Drift, if available
- Master brightness: 70–140
- Active color: warm amber

## Mode Availability Rule

If a default scene references a mode that does not exist in the actual sketch, do not create a broken scene.

Instead:

- Fall back to Solid mode, or
- Skip that scene and document why.

## Built-In Scene Protection

First version may allow deleting built-in scenes.

Better version:

- Mark built-ins with `builtin: true`.
- Allow reset built-ins later in a future phase.

Do not overbuild if memory/UI complexity becomes too high.

## Acceptance Criteria

- First boot has useful scenes.
- Built-in scenes do not reference missing modes.
- Built-in scenes are saved to `/scenes.json` or recreated when file is absent.

---

# 4. Capture Current State as Scene

## Goal

Allow the user to save the current lighting state as a new scene.

## Required Behavior

When the user saves a scene:

1. Read current central settings.
2. Copy relevant scene fields.
3. Generate stable scene ID.
4. Store user-provided scene name.
5. Add to scene list.
6. Save `/scenes.json`.
7. Return success/error JSON.
8. Refresh UI scene list.

## Required Validation

Scene name:

- Must not be empty.
- Trim whitespace.
- Clamp max length.
- Escape/sanitize for HTML display.
- Generate URL-safe ID.

Scene count:

- If at max scene count, reject with clear error.

Duplicate names:

- Either allow duplicate display names with unique IDs, or prevent duplicate names.
- Recommended: prevent exact duplicate names for user sanity.

## API Endpoint

Preferred endpoint:

```text
/api/scenes/save?name=Reading
```

Alternative:

```text
/api/scene/save?name=Reading
```

Use whichever naming style best fits the route map, but be consistent.

JSON success example:

```json
{
  "ok": true,
  "message": "Scene saved",
  "scene": {
    "id": "reading",
    "name": "Reading"
  }
}
```

JSON error example:

```json
{
  "ok": false,
  "error": "Scene limit reached"
}
```

## Acceptance Criteria

- User can save current state as a scene.
- Scene appears in list.
- Scene persists after reboot.
- Invalid names are handled safely.
- Scene limit is enforced.

---

# 5. Recall / Load Scene

## Goal

Allow the user to recall a saved scene and apply it to the live controller state.

## Required Behavior

When loading a scene:

1. Find scene by ID.
2. Validate that the mode still exists.
3. Apply scene settings through existing central state mutation functions where practical.
4. Update active mode/color/temperature/brightness/mode-specific settings.
5. Mark current settings dirty so the recalled scene can become the restored state after reboot, unless boot behavior says otherwise.
6. Render updated state on next frame.
7. Return JSON success/error.

## Important Rule

Do not bypass the central settings/output pipeline.

Scene recall should use the same safe state update paths as `/api/color`, `/api/mode`, `/api/brightness`, etc.

This keeps future Phase 2/3 work easier.

## Missing Mode Behavior

If a scene references a mode that no longer exists:

- Do not crash.
- Return a clear error, or
- Fall back to Solid mode and warn.

Recommended:

- Return error for user-saved scenes.
- Built-in scene creation should avoid missing modes.

## API Endpoint

Preferred:

```text
/api/scenes/load?id=warm-dim
```

Optional support:

```text
/api/scene?name=Warm%20Dim
```

Do not implement QR/NFC-friendly `/go/sleep` links yet. That is a later phase.

## Acceptance Criteria

- User can load a scene from UI.
- Scene applies mode/color/brightness correctly.
- Mode-specific settings restore.
- Invalid scene ID returns JSON error.
- Existing lighting resumes through normal output pipeline.

---

# 6. List Scenes

## Goal

Allow UI and API clients to retrieve available scenes.

## API Endpoint

Preferred:

```text
/api/scenes
```

or:

```text
/api/scenes/list
```

Recommended: `/api/scenes` for list, and action endpoints for mutations.

## Required JSON Fields

Return compact scene summaries:

```json
{
  "ok": true,
  "count": 3,
  "maxScenes": 16,
  "scenes": [
    {
      "id": "warm-dim",
      "name": "Warm Dim",
      "mode": "Solid",
      "colorHex": "#FFA050",
      "whiteKelvin": 2700,
      "usingWhiteTemperature": true,
      "masterBrightness": 45,
      "builtin": true
    }
  ]
}
```

Do not return giant unnecessary data if the UI only needs summaries.

Optional:

- Add `/api/scenes/detail?id=...` later if needed.

For Phase 2A, list can include all relevant fields if easier, but keep JSON reasonable.

## Acceptance Criteria

- API lists scenes.
- UI can render scene list from endpoint.
- Count/max count are exposed.

---

# 7. Rename Scene

## Goal

Allow the user to rename a scene.

## Required Behavior

When renaming:

1. Find scene by ID.
2. Validate new name.
3. Update display name.
4. Keep scene ID stable unless project intentionally supports changing IDs.
5. Save scenes file.
6. Return JSON.

## API Endpoint

Preferred:

```text
/api/scenes/rename?id=warm-dim&name=Warm%20Cozy
```

## Validation

- New name must not be empty.
- New name max length enforced.
- Prevent duplicate names if duplicate prevention was chosen.
- Sanitize for HTML display.

## Acceptance Criteria

- User can rename a scene.
- Scene ID remains stable.
- New name persists after reboot.
- Invalid rename returns JSON error.

---

# 8. Delete Scene

## Goal

Allow the user to delete a saved scene.

## Required Behavior

When deleting:

1. Find scene by ID.
2. Remove from scene list.
3. Save scenes file.
4. Return JSON.
5. Refresh UI list.

## Built-In Scene Policy

Choose one of these policies and document it:

### Simple Policy

Built-in scenes can be deleted. They return only if `/scenes.json` is deleted/reset.

### Protected Policy

Built-in scenes cannot be deleted unless user resets scenes.

Recommended for Phase 2A:

- Simple policy is acceptable.
- Protected policy is better if easy.

## API Endpoint

Preferred:

```text
/api/scenes/delete?id=warm-dim
```

## Acceptance Criteria

- User can delete scenes.
- Deleted scene disappears from list.
- Deletion persists after reboot.
- Unknown ID returns JSON error.
- Built-in delete policy is documented.

---

# 9. Scene UI

## Goal

Add scene controls to the web interface without making it feel like a spaceship cockpit after three espressos.

## UI Placement Options

Recommended first version:

- Add a **Scenes** section on the main page below primary color/brightness/mode controls.

Alternative:

- Add a `/scenes` page and link to it from the main page.

Because this is meant for everyday use, scenes should be reasonably accessible.

## Required UI Features

The UI should include:

- Scene list
- Load/apply button for each scene
- Rename button or small inline rename control
- Delete button
- Save current as scene input field
- Save button
- Scene count/max count indicator
- Error/success message area

## Suggested Scene Card Fields

Each scene card/list row should show:

- Scene name
- Mode
- Small color swatch
- Brightness percentage or raw value
- White temperature if relevant
- Load button
- Rename/delete controls

## Mobile Usability

Keep touch targets reasonable.

Do not rely on tiny text links only.

## JavaScript Behavior

Use lightweight JavaScript:

- Fetch `/api/scenes` on page load.
- Render scene list.
- Save scene via API.
- Load scene via API.
- Rename/delete via API.
- Refresh list after mutations.
- Show errors without page reload.

Do not add heavy frontend frameworks.

The ESP8266 has feelings, and most of them are heap-related.

## Acceptance Criteria

- User can manage scenes from browser UI.
- UI updates without full page reload where practical.
- Scene actions show success/error feedback.
- Main lighting controls still work.
- Diagnostics/OTA links remain accessible.

---

# 10. API Route Summary

## Required Scene API Surface

Implement or normalize these endpoints:

```text
GET /api/scenes
GET /api/scenes/save?name=...
GET /api/scenes/load?id=...
GET /api/scenes/rename?id=...&name=...
GET /api/scenes/delete?id=...
```

If the existing route style prefers all actions as `/api/scene?action=...`, that is acceptable, but document it.

Because this is ESP8266 `ESP8266WebServer`, GET endpoints are acceptable for local-only control. If POST handling is already used, POST is also acceptable.

## Required Response Format

All scene API responses should use the Phase 1D JSON convention:

Success:

```json
{
  "ok": true,
  "message": "Scene loaded"
}
```

Error:

```json
{
  "ok": false,
  "error": "Scene not found"
}
```

Content type:

```text
application/json
```

## Acceptance Criteria

- Scene API responses are consistent.
- Invalid requests return JSON errors.
- Scene actions do not crash on malformed input.

---

# 11. State Integration

## Goal

Integrate scenes with current state without creating duplicate state logic.

## Required Behavior

When a scene is loaded:

- Current mode changes.
- Active color changes.
- White temperature changes if relevant.
- Master brightness changes.
- Mode-specific controls change.
- `/state` and `/api/state` reflect the new current values.
- Main UI updates to match scene settings.

## Recommended Implementation

Use central mutation functions from Phase 1D:

```cpp
setModeByName(scene.mode);
setActiveColor(scene.red, scene.green, scene.blue);
setWhiteTemperature(scene.whiteKelvin);
setMasterBrightness(scene.masterBrightness);
```

or carefully update the central settings object directly if mutation functions are not available.

Do not bypass validation unless the scene file is trusted and already validated on load.

## Acceptance Criteria

- Scene recall updates runtime state and visible UI state.
- `/state` reflects loaded scene.
- Existing output pipeline renders scene correctly.

---

# 12. Settings vs Scenes Separation

## Goal

Keep current settings and saved scenes conceptually separate.

## Rule

`/settings.json` should represent the controller’s current/restored state and configuration.

`/scenes.json` should represent saved presets.

Loading a scene may update current settings and eventually save `/settings.json`, but scenes themselves should remain in `/scenes.json`.

## Diagnostic State

Do not save diagnostic test state into scenes.

Do not save:

- Active LED test
- Uptime
- RSSI
- Free heap
- Power estimate results
- Warnings
- OTA status

## Power Configuration

Do not save power supply configuration inside scenes.

Power config is a controller-level setting, not a scene.

## Gamma/RGB Calibration

Decision point:

### Option A — Include Gamma/Gains in Scenes

Useful if scenes should preserve exact appearance.

Downside: recalling a scene changes global calibration, which may surprise the user.

### Option B — Do Not Include Gamma/Gains in Scenes

Recommended for Phase 2A.

Treat gamma and RGB gains as global calibration settings, not scene-specific style.

Unless the existing plan already included gamma/gains in scenes, prefer **Option B**.

Document the choice.

## Acceptance Criteria

- Scenes store lighting look, not diagnostics/power/global hardware config.
- Separation is documented.
- Scene recall does not accidentally overwrite power or diagnostic settings.

---

# 13. Memory and JSON Size Constraints

## Goal

Keep scene storage and UI JSON reasonable for ESP8266.

## Requirements

Avoid:

- Unlimited scenes
- Huge scene descriptions
- Long unbounded names
- Returning giant JSON blobs on every state request
- Duplicating large strings repeatedly
- Heavy frontend libraries

Prefer:

- Max scene count
- Short IDs
- Short names
- Compact JSON
- Simple HTML/JS
- Scene list endpoint separate from `/api/state`

## Acceptance Criteria

- Scene list remains compact.
- Main `/api/state` does not become huge.
- UI remains responsive.

---

# 14. Persistence Error Handling

## Goal

Scenes should fail safely.

## Required Behavior

If scene save fails:

- Keep runtime state unchanged.
- Return JSON error.
- Show error in UI.
- Do not pretend scene was saved.

If scene load file is corrupt:

- Fall back to built-in default scenes.
- Track scene load status for diagnostics if practical.
- Do not crash.

If scene list is full:

- Return clear error.

If scene ID is unknown:

- Return clear error.

## Diagnostics Integration

If easy, add scene status to diagnostics:

- Scene count
- Max scenes
- Scene file load status
- Last scene save status

Do not build a giant new diagnostics expansion if it distracts from Phase 2A.

## Acceptance Criteria

- Scene errors are visible and safe.
- Corrupt/missing scene file does not brick the controller.
- Scene file status is documented and optionally exposed.

---

# 15. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_2A_Report.md
```

The report should include:

1. Files changed.
2. Scene data model summary.
3. Scene storage file path and schema.
4. Built-in default scenes.
5. Scene API endpoints.
6. Scene UI behavior.
7. Scene save/load/rename/delete behavior.
8. Settings-vs-scenes separation decision.
9. Error handling behavior.
10. Memory/scene count limits.
11. Compile/test status.
12. Known limitations.
13. What was intentionally deferred.
14. Recommended next step: Phase 2B favorite bar, Panic Warm, and Night Guard.

## Required Honesty

Do not claim physical LED testing unless actual physical LED testing was performed.

Do not claim scenes have smooth transitions unless Phase 3 transitions already exist.

Do not claim schedules/timers/favorites exist unless actually implemented.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Phase 1 State Model

Identify:

- Central settings object
- Current save/load functions
- Existing mode names
- Mode-specific settings
- Existing API response helpers
- Existing mutation functions
- Existing diagnostics/status helpers

Do not rewrite the whole sketch.

## Step 2 — Add Scene Struct and Scene List

Add scene representation and bounded array/vector/list.

Recommended:

- Static array with max count if easiest/safest.
- Avoid unbounded dynamic allocation.

## Step 3 — Add Scene Serialization

Implement:

- Load scenes from `/scenes.json`
- Save scenes to `/scenes.json`
- Built-in default scene creation
- Missing/corrupt file fallback

## Step 4 — Add Scene Capture Function

Implement:

```cpp
bool saveCurrentStateAsScene(const String& name);
```

or equivalent.

## Step 5 — Add Scene Recall Function

Implement:

```cpp
bool loadSceneById(const String& id);
```

or equivalent.

Make it update the central settings/output state cleanly.

## Step 6 — Add Rename/Delete/List Helpers

Implement:

- `findSceneById`
- `renameScene`
- `deleteScene`
- `sceneNameExists`
- `makeSceneId`

## Step 7 — Add Scene API Routes

Add:

- `/api/scenes`
- `/api/scenes/save`
- `/api/scenes/load`
- `/api/scenes/rename`
- `/api/scenes/delete`

Use consistent JSON responses.

## Step 8 — Add Scene UI

Add a lightweight scene section/page.

Use JavaScript fetch calls to scene API.

## Step 9 — Integrate With State/UI Refresh

After loading a scene, make sure the main UI displays the new state.

If the existing UI state refresh function exists, call/reuse it.

## Step 10 — Add Optional Diagnostics Scene Status

If easy, include scene count/load status in diagnostics.

## Step 11 — Compile and Report

Compile for D1 mini / ESP8266.

Create/update Phase 2A report.

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

## Scene Storage

- Missing `/scenes.json` creates/loads defaults.
- Valid `/scenes.json` loads scenes.
- Corrupt `/scenes.json` falls back safely.
- Scenes persist after reboot.
- Max scene count is enforced.

## Built-In Scenes

- Warm Dim exists.
- Reading exists.
- Sleep exists.
- Movie exists.
- Focus exists.
- Rain Glass exists only if mode exists or safely falls back.
- Moon Tide exists only if mode exists or safely falls back.
- Lantern Drift exists only if mode exists or safely falls back.

## Scene API

- `/api/scenes` lists scenes.
- Save scene works.
- Load scene works.
- Rename scene works.
- Delete scene works.
- Invalid scene ID returns JSON error.
- Empty scene name returns JSON error.
- Duplicate scene name behavior matches documented policy.
- Scene limit reached returns JSON error.

## Scene UI

- Scene section/page loads.
- Scene list renders.
- Load button applies scene.
- Save current scene works.
- Rename works.
- Delete works.
- Errors display to user.
- UI remains mobile-friendly enough.

## State Integration

- Loading scene changes active mode.
- Loading scene changes active color/temperature.
- Loading scene changes master brightness.
- Loading scene restores mode-specific settings.
- `/state` and `/api/state` reflect loaded scene.
- Existing output pipeline renders loaded scene.

## Persistence Separation

- `/settings.json` still represents current settings.
- `/scenes.json` stores presets.
- Diagnostic state is not saved into scenes.
- Power settings are not saved into scenes.

## Existing Lighting Features

After scene work, verify at least:

- Solid Color still works.
- Color picker still works.
- White temperature slider still works.
- Mode dropdown still works.
- Master brightness still works.
- Existing animations still work.
- OTA still works.

---

# Phase 2A Non-Goals Reminder

Do not implement these yet:

- Favorite scene bar
- Panic Warm
- Night Guard
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

The cozy goblin army is forming, but today we are only giving them labeled storage bins.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 2A complete.

Files changed:
- ...

Implemented:
- ...

Scene model:
- ...

Scene storage:
- ...

Built-in scenes:
- ...

Scene API:
- ...

Scene UI:
- ...

Settings/scenes separation:
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
- Phase 2B — favorite scene bar, Panic Warm, and Night Guard.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim smooth transitions, timers, scheduler, palettes, or QR/NFC links unless they actually exist.

---

# One-Sentence Mission Summary

Add persistent named scene presets so the Bedroom LED Controller can save and recall complete lighting vibes without forcing the user to manually reconstruct the perfect cozy configuration like an exhausted RGB archaeologist.

