# Bedroom LED Controller — Phase 4A Codex Prompt

## Phase 4A Title

**Advanced Local Scheduler + Scene Links / QR-Friendly URLs**

## Mission

You are working on the `BedroomLedController` Arduino project for a D1 mini / ESP8266 controlling a WS2812 LED strip through a local web interface.

This subphase is **Phase 4A** of the larger upgrade plan.

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

The goal of Phase 4A is to add a **local weekly scheduler** and **stable scene/action links**.

The scheduler should let the controller run saved scenes or actions at specific times and days without relying on cloud services. Scene links should allow phones, shortcuts, QR codes, NFC tags, or local scripts to trigger scenes/actions by opening a stable URL.

This phase must remain software-only. Do **not** add physical buttons, NFC readers, sensors, microphones, or new hardware. QR/NFC support here means URL-friendly links that an external phone/tag can open — not NFC hardware inside the controller.

This is the “the bedroom lights now have a tiny calendar and polite URL doorbells” phase. The firmware goblin is allowed a planner. It is not allowed a cloud subscription.

---

# Critical Board / Resource Context

The project remains **D1 mini / ESP8266 first** unless resource measurements prove it cannot continue safely.

Important constraints:

- Keep JSON compact.
- Avoid heavy JavaScript frameworks.
- Avoid unbounded schedule entries.
- Avoid heap-fragmenting dynamic allocation.
- Avoid excessive `String` churn in frequent code paths.
- Do not stream LED frames.
- Do not poll time servers aggressively.
- Do not write schedule/settings files repeatedly.
- Preserve OTA margin and web responsiveness.

The FireBeetle ESP32-S3 N4 may provide better runtime headroom, but this phase should still target the D1 mini unless measurements show otherwise.

## Required Resource Report Addition

Every Phase 4A report must include:

```text
Resource report:
- Board target used for compile:
- Sketch size:
- Sketch size percentage of available app slot, if known:
- LittleFS usage:
- Free heap after boot:
- Free heap after loading main page:
- Free heap after /api/state:
- Free heap after /api/scenes:
- Free heap after /api/schedule:
- Free heap after opening schedule editor:
- Free heap after triggering a scene link:
- Free heap after schedule check cycle, if measurable:
- OTA still works: yes/no/not tested
- Web UI responsiveness: snappy/acceptable/laggy/not tested
- Scheduler responsiveness: snappy/acceptable/laggy/not tested
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
- New Phase 3D modes, if implemented
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

Phase 4A may add scheduler and scene-link routes, but must not break existing routes.

---

# Hard Scope Boundary

This prompt covers:

1. Time sync foundation using NTP or existing time service
2. Time status in diagnostics
3. Local weekly schedule data model
4. Persistent schedule storage
5. Schedule UI/editor
6. Schedule API endpoints
7. Schedule execution engine
8. Manual run now behavior
9. Skip next occurrence behavior
10. Enable/disable schedule entries
11. Scene/action targets for schedules
12. Fade/transition behavior for scheduled events
13. Stable scene/action URLs
14. QR-friendly scene link UI
15. Optional browser-generated QR codes
16. Scene link API compatibility
17. Scheduler/scene link interaction with Night Guard, timers, transitions, palettes, and diagnostics
18. Resource report
19. Documentation/report update
20. Compile/test report

Do **not** implement yet:

- Remaining large Phase 4 lighting mode library
- Advanced scene import/export
- Full favorite sorting/reordering unless trivial
- Hardware NFC reader
- Physical wall controls
- Microphone/music-reactive behavior
- Light sensor/room-aware brightness
- Cloud scheduling
- External account integrations
- ESP32-only features
- WebSocket frame streaming

If a later feature seems tempting, document it as deferred. The Feature Hydra gets one head in this phase: scheduler + scene links. No more heads. Firmware mythological animal control is expensive.

---

# Phase 4A Main Goal

At the end of Phase 4A, the user should be able to:

1. Sync time locally via NTP.
2. See time sync status.
3. Create a weekly schedule entry.
4. Choose which days the entry runs.
5. Choose a time of day.
6. Choose a scene or action target.
7. Choose fade duration / transition behavior.
8. Enable/disable schedule entries.
9. Skip the next occurrence of a schedule entry.
10. Manually run a schedule entry now.
11. Reboot and keep schedule entries.
12. Trigger scenes/actions from stable URLs.
13. Copy scene/action links from the UI.
14. Optionally show QR codes generated in the browser.
15. Keep all previous features working.
16. Keep D1 mini resource usage under control.

---

# Part A — Time Sync Foundation

---

# 1. Timekeeping Goals

## Goal

Add reliable-enough local time awareness for schedules.

## Required Behavior

The controller should obtain current time using NTP when connected to Wi-Fi.

It should track:

- Whether time is synced
- Last sync attempt
- Last successful sync
- Current local time
- Time zone offset / timezone rule
- Next scheduled event, if available

## Important Rule

Schedules should not fire until time is valid.

If time is not synced:

- Show warning in UI/diagnostics.
- Do not run schedule entries based on invalid time.
- Allow manual scene links/actions to still work.

## Acceptance Criteria

- Controller can report time sync status.
- Schedules do not run with unknown/invalid time.
- Manual controls still work without time sync.

---

# 2. Time Zone Handling

## Goal

Support local time without making the ESP8266 run a full timezone database.

## Required Behavior

Use a simple configurable timezone model.

Recommended first implementation:

- Store UTC offset in minutes.
- Optional daylight saving toggle or manual offset adjustment.

Default for the user/project:

```text
America/New_York style offset, but do not hardcode permanently without UI setting.
```

Because full timezone/DST rules can be complex, a simple manual offset is acceptable for Phase 4A.

## Suggested Settings

```cpp
int16_t timezoneOffsetMinutes;
bool daylightSavingEnabled;
```

or:

```cpp
int16_t localUtcOffsetMinutes;
```

Recommended default:

```text
-300 minutes for EST, or -240 for EDT depending on current user setting if known.
```

Better:

- Let the user set offset in UI.
- Store it persistently.

## UI Requirement

Add a simple timezone/time section:

- Current controller time
- UTC offset setting
- Save timezone setting
- Sync time now button

## Acceptance Criteria

- User can see configured UTC offset.
- User can update offset.
- Offset persists.
- Current local time displays according to offset.

---

# 3. NTP Sync Strategy

## Goal

Sync time without hammering network or blocking the controller.

## Required Behavior

Use non-blocking or minimally blocking NTP approach supported by the ESP8266 environment.

Acceptable approaches:

- Use built-in time functions with `configTime()` if already available.
- Use an NTP client library if already present or lightweight.

Do not add a huge dependency unless necessary.

## Sync Policy

Recommended:

- Try sync on Wi-Fi connect.
- Retry periodically if not synced, such as every 5–15 minutes.
- Once synced, refresh every 6–24 hours.
- Provide manual “Sync now” button.

Do not query NTP every loop.

Do not block animations/web server for long periods.

## Diagnostics

Show:

- Time synced: yes/no
- Current local time
- Last sync success
- Last sync failure, if tracked
- Next retry time, optional

## Acceptance Criteria

- NTP sync is not aggressive.
- Failed sync does not crash controller.
- Time status is visible.
- OTA/web UI remain responsive.

---

# Part B — Local Weekly Scheduler

---

# 4. Schedule Data Model

## Goal

Add a bounded weekly schedule model.

## Required Concept

A schedule entry runs on selected days at a selected local time and triggers a scene or action.

## Suggested Data Model

```cpp
struct ScheduleEntry {
  String id;
  String name;
  bool enabled;
  uint8_t daysMask; // bitmask for Sun-Sat or Mon-Sun, document mapping
  uint16_t minuteOfDay; // 0..1439 local time
  String targetType; // scene, action, favorite, mode optional
  String targetId;
  uint16_t fadeSeconds;
  bool useTransition;
  bool skipNext;
  unsigned long lastRunEpoch;
};
```

Use field names/types that fit the codebase.

## Required Fields

Each entry should include:

- Stable ID
- User-facing name
- Enabled flag
- Days of week
- Time of day
- Target type
- Target ID/name
- Fade duration or transition duration
- Skip next occurrence flag
- Last run tracking to avoid repeated firing in the same minute

## Max Schedule Entries

Set a safe bounded maximum.

Recommended:

```text
MAX_SCHEDULE_ENTRIES = 8
```

Acceptable:

```text
4–12
```

Do not allow unlimited schedules.

The D1 mini is not Google Calendar. It is a lamp goblin with Wi-Fi.

## Day Mask Mapping

Document mapping clearly.

Example:

```text
bit 0 = Sunday
bit 1 = Monday
bit 2 = Tuesday
bit 3 = Wednesday
bit 4 = Thursday
bit 5 = Friday
bit 6 = Saturday
```

or use Monday-first, but document it.

## Acceptance Criteria

- Schedule entries can represent weekly routines.
- Schedule count is bounded.
- Entry IDs are stable.
- Day mask is documented.

---

# 5. Schedule Storage

## Goal

Persist schedule entries across reboot.

## Preferred Storage

Use LittleFS or existing filesystem layer.

Recommended file:

```text
/schedule.json
```

Keep separate from:

- `/settings.json`
- `/scenes.json`
- `/palettes.json`

## Storage Format

Recommended JSON shape:

```json
{
  "version": 1,
  "entries": [
    {
      "id": "morning-warm",
      "name": "Morning Warm",
      "enabled": true,
      "daysMask": 62,
      "minuteOfDay": 420,
      "targetType": "scene",
      "targetId": "warm-dim",
      "fadeSeconds": 900,
      "useTransition": true,
      "skipNext": false,
      "lastRunEpoch": 0
    }
  ]
}
```

Use compact JSON if needed.

## Load Behavior

During setup:

1. Initialize empty schedule list.
2. Load `/schedule.json` if it exists.
3. If missing, keep empty schedule list or seed examples disabled.
4. If corrupt, keep empty list and track error.
5. Do not crash on missing/corrupt/incomplete file.

## Save Behavior

Save schedule when:

- Entry created
- Entry edited
- Entry deleted
- Entry enabled/disabled
- Skip next changed
- Last run timestamp updated, but avoid excessive writes if possible

## Last Run Save Strategy

Avoid writing flash every minute if unnecessary.

Recommended:

- Track last run in RAM.
- Persist lastRunEpoch only when schedule file is otherwise saved, or save sparingly.
- Use RAM-only last-fired minute if enough to prevent repeated firing during runtime.

Because power loss may reset last-run memory, guard by comparing date/minute if time is synced.

## Acceptance Criteria

- Schedule persists after reboot.
- Missing/corrupt schedule file is safe.
- Schedule save does not happen every loop.
- Entry count limit is enforced.

---

# 6. Schedule Target Types

## Goal

Allow schedules to trigger existing safe lighting targets.

## Required Target Types

Support at least:

### Scene Target

```text
targetType = scene
targetId = scene ID
```

Loads a scene using existing scene load function.

### Action Target

```text
targetType = action
targetId = warmDimNow / off / etc.
```

Runs an existing action using central action system.

## Optional Target Types

### Favorite Target

Loads a favorite by ID.

### Mode Target

Sets a mode directly.

Mode targets are optional. Scenes are preferred because they capture full lighting state.

## Required Built-In Actions

Schedules should support these action targets if they exist:

- `off`
- `warmDimNow`
- `panicWarm`, if this is the internal name
- Optional: `surpriseCalm`, only if Surprise Me API is stable and safe

Do not add new action semantics unless needed.

## Target Validation

When creating/editing schedule:

- Validate scene exists for scene target.
- Validate action name exists for action target.
- Validate favorite exists if favorite target supported.
- Return JSON error for invalid target.

When schedule runs:

- Validate target again.
- If target missing, skip and log/report error.

## Acceptance Criteria

- Schedule can run a scene.
- Schedule can run Off.
- Schedule can run Warm Dim Now.
- Invalid targets are rejected or fail safely.

---

# 7. Schedule Execution Engine

## Goal

Run enabled schedule entries at the correct local time.

## Required Behavior

Add a scheduler service function called regularly from the main loop:

```cpp
void serviceScheduler();
```

Conceptual loop:

```cpp
void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  serviceTimeSync();
  serviceScheduler();
  serviceActiveTimer();
  updateAnimationFrame();
  serviceSettingsSave();
}
```

Use actual project structure.

## Execution Logic

For each enabled schedule entry:

1. Ensure time is synced.
2. Determine local day of week and minute of day.
3. Check if today is selected in daysMask.
4. Check if current minute equals entry minuteOfDay.
5. Ensure entry did not already run during this occurrence.
6. If skipNext is true:
   - Clear skipNext.
   - Mark schedule dirty.
   - Do not run target.
7. If not skipped:
   - Execute target through existing safe path.
   - Track last run occurrence.

## Avoid Repeated Firing

A schedule entry must not run repeatedly every loop during the matching minute.

Use one of:

- Last fired date/minute in RAM
- Last run epoch/minute
- Last run occurrence key

Example:

```cpp
uint32_t occurrenceKey = yyyymmdd * 1440 + minuteOfDay;
```

Use lightweight representation.

## Acceptance Criteria

- Schedules fire once per occurrence.
- Disabled schedules do not fire.
- Skip next works.
- Invalid time prevents schedule firing.
- No blocking delays.

---

# 8. Schedule Fade / Transition Behavior

## Goal

Allow scheduled changes to feel gentle.

## Required Behavior

Each schedule entry should have either:

- Fade duration, or
- Use existing transition duration, or
- Both, with clear rules.

Recommended simple behavior:

- `useTransition = true/false`
- `fadeSeconds` overrides transition duration if > 0 and supported.

Execution examples:

### Scene Target With Fade

- If fadeSeconds > 0, use existing timer/fade or transition system to fade into scene.
- If not feasible, use smooth transition with configured transition duration.

### Off Target With Fade

- If fadeSeconds > 0, use existing timer/fade to fade to off.
- Otherwise use Off action with transition if enabled.

### Warm Dim Target With Fade

- Use bedtime fade / warm dim fade path if available.

## Important Rule

Do not create a second independent fade engine if Phase 2C/3A already has fade/transition systems.

Reuse existing timer/fade/transition functions.

## Conflict With Active Timer/Fade

If a schedule fires while an active one-shot timer/fade is running:

Choose one behavior and document it.

Recommended:

- Scheduled event overrides and cancels active one-shot timer/fade.
- Manual timers are user-initiated, but scheduled routines should be predictable.

Alternative:

- If active manual timer exists, skip scheduled event and log warning.

Recommended for Phase 4A:

- Add setting later if needed; for now, scheduled event overrides active timer/fade.

## Acceptance Criteria

- Scheduled scene changes transition/fade gently.
- Off schedule can fade to off.
- Warm Dim schedule can fade if available.
- No double-fade chaos.

---

# 9. Schedule UI / Weekly Editor

## Goal

Add a manageable weekly schedule editor to the web UI.

## UI Placement

Recommended:

- Add a **Schedules** section/page.
- Main page can show a compact schedule summary and link to full editor.

Because schedule editing is not an every-minute task, a dedicated `/schedule` page is acceptable and probably cleaner.

## Required Route

Add one of:

```text
/schedule
```

or include editor in main page under collapsible Advanced/Schedules.

Recommended:

```text
/schedule
```

Add navigation links:

- Main page → Schedule
- Schedule → Main page
- Schedule → Diagnostics

## Required UI Features

The scheduler UI should allow:

- View all schedule entries
- Add new entry
- Edit entry
- Delete entry
- Enable/disable entry
- Skip next occurrence
- Manual run now
- Select days of week
- Set time of day
- Select target type
- Select scene/action/favorite target
- Set fade duration
- Show next run time if time is synced
- Show time sync status

## Recommended Controls

For each entry:

- Name input
- Enabled checkbox
- Day buttons: Sun Mon Tue Wed Thu Fri Sat
- Time input: `<input type="time">`
- Target type dropdown
- Target dropdown
- Fade duration dropdown/input
- Save button
- Run now button
- Skip next button
- Delete button

## Keep UI Lightweight

Use vanilla JS.

No heavy frameworks.

Avoid rendering huge tables.

Max entries are bounded, so simple card/list layout is fine.

## Acceptance Criteria

- User can create/edit/delete schedules from browser.
- UI shows time sync status.
- UI shows next run if possible.
- UI remains mobile-friendly enough.
- Existing main UI remains usable.

---

# 10. Schedule API Endpoints

## Goal

Expose scheduler through local JSON API.

## Required Endpoints

Implement or normalize:

```text
/api/schedule
/api/schedule/save
/api/schedule/delete
/api/schedule/enable
/api/schedule/skip-next
/api/schedule/run-now
/api/time
/api/time/sync
```

Alternative naming is acceptable if documented.

## `/api/schedule`

Returns schedule list and time status.

Example:

```json
{
  "ok": true,
  "timeSynced": true,
  "localTime": "2026-05-16 21:42",
  "count": 2,
  "maxEntries": 8,
  "entries": [
    {
      "id": "morning-warm",
      "name": "Morning Warm",
      "enabled": true,
      "daysMask": 62,
      "minuteOfDay": 420,
      "targetType": "scene",
      "targetId": "warm-dim",
      "targetName": "Warm Dim",
      "fadeSeconds": 900,
      "useTransition": true,
      "skipNext": false,
      "nextRun": "Monday 07:00"
    }
  ]
}
```

## `/api/schedule/save`

Creates or updates an entry.

Suggested query parameters:

```text
id=optional-existing-id
name=Morning%20Warm
enabled=1
daysMask=62
minuteOfDay=420
targetType=scene
targetId=warm-dim
fadeSeconds=900
useTransition=1
```

Validation:

- Name non-empty.
- daysMask not zero.
- minuteOfDay 0–1439.
- targetType valid.
- targetId valid.
- fadeSeconds bounded.
- Max entry count enforced.

## `/api/schedule/delete`

```text
/api/schedule/delete?id=morning-warm
```

## `/api/schedule/enable`

```text
/api/schedule/enable?id=morning-warm&enabled=0
```

## `/api/schedule/skip-next`

```text
/api/schedule/skip-next?id=morning-warm&skip=1
```

## `/api/schedule/run-now`

Runs entry target immediately, without waiting for schedule time.

```text
/api/schedule/run-now?id=morning-warm
```

## `/api/time`

Returns time status.

```json
{
  "ok": true,
  "timeSynced": true,
  "epoch": 1778982120,
  "localTime": "2026-05-16 21:42",
  "utcOffsetMinutes": -240,
  "lastSyncMs": 123456,
  "lastSyncOk": true
}
```

## `/api/time/sync`

Triggers manual sync attempt.

## Response Format

Use existing JSON convention:

Success:

```json
{
  "ok": true,
  "message": "Schedule saved"
}
```

Error:

```json
{
  "ok": false,
  "error": "Invalid target scene"
}
```

## Acceptance Criteria

- Schedule API returns valid JSON.
- Create/update/delete works.
- Enable/disable works.
- Skip next works.
- Run now works.
- Time API works.
- Invalid inputs return JSON errors.

---

# 11. Schedule Diagnostics Integration

## Goal

Expose scheduler status in diagnostics.

## Required Diagnostics Fields

Add compact diagnostics fields:

- Time synced yes/no
- Current local time
- UTC offset
- Last time sync status
- Schedule count/max
- Next scheduled event
- Last schedule run
- Last schedule error, if tracked

Do not bloat diagnostics heavily.

## Warnings

Show warnings for:

- Time not synced; schedules paused.
- Schedule target missing.
- Schedule file failed to load/save.
- Too many entries attempted.

## Acceptance Criteria

- Diagnostics can explain why schedules are not running.
- Warnings are clear and truthful.

---

# Part C — Scene Links / QR-Friendly URLs

---

# 12. Stable Scene Link Goals

## Goal

Provide stable URLs that trigger scenes/actions from a browser, phone shortcut, QR code, NFC tag, or local script.

No new hardware is added.

The controller only exposes URLs.

External NFC tags or QR codes can open those URLs using a phone.

## Required Link Types

Support stable links for:

- Load scene by ID
- Run action by name
- Load favorite by ID, optional
- Start bedtime fade, optional

## Required Routes

Add clean human-friendly routes such as:

```text
/go/scene?id=sleep
/go/action?name=off
/go/action?name=warmDimNow
```

Optional aliases:

```text
/go/sleep
/go/movie
/go/focus
/go/off
/go/warm
```

Recommended first implementation:

- Use parameterized routes to avoid registering a route per scene.

Examples:

```text
/go?scene=sleep
/go?action=off
/go?action=warmDimNow
```

Because `ESP8266WebServer` route matching can be simple, parameterized query routes may be easier and more reliable.

## Behavior

When a link is opened:

1. Validate target.
2. Apply scene/action through existing safe functions.
3. Respect Night Guard.
4. Respect transitions.
5. Stop diagnostic test if needed.
6. Return a small friendly HTML confirmation page.

Example response:

```text
Scene Loaded: Sleep
[Back to Controller]
```

For API clients, also allow JSON if `format=json` is requested.

## Acceptance Criteria

- Scene link loads scene.
- Action link runs Off/Warm Dim Now.
- Invalid target shows helpful error.
- Links use existing scene/action logic.
- Links do not require cloud.

---

# 13. Scene Link API Compatibility

## Goal

Make scene links useful to both humans and automation.

## Existing API Routes

Do not break existing scene APIs:

```text
/api/scenes/load?id=...
/api/action?name=...
```

The new `/go` route can be a friendly wrapper around these existing behaviors.

## JSON Option

Support optional JSON response:

```text
/go?scene=sleep&format=json
```

Response:

```json
{
  "ok": true,
  "message": "Scene loaded",
  "sceneId": "sleep"
}
```

HTML default is fine for phone/QR use.

## Acceptance Criteria

- `/go` wrapper does not duplicate logic badly.
- JSON format works if implemented.
- Existing API remains stable.

---

# 14. Scene Link UI

## Goal

Let the user copy stable links from the web UI.

## Required UI Features

In scene management UI, show for each scene:

- Copy Link button
- Link text, optionally hidden/collapsible
- Optional QR button if QR is implemented

For actions, provide copyable links for:

- Off
- Warm Dim Now
- Optional Bedtime Fade
- Optional favorite links

## Link Generation

Generate links using current device origin in browser JS:

```javascript
const base = window.location.origin;
const link = `${base}/go?scene=${encodeURIComponent(scene.id)}`;
```

This avoids hardcoding IP/hostname.

## Acceptance Criteria

- User can copy a scene link.
- Link uses current device origin.
- Link works when opened.
- UI remains lightweight.

---

# 15. QR-Friendly Support

## Goal

Support QR code use without making firmware huge.

## Required Minimum

A link must be copyable so the user can make a QR code externally.

## Optional Browser-Generated QR

If adding QR display is lightweight, generate QR codes in the browser.

Important:

- Do not add a huge QR library inline if it bloats sketch size.
- Prefer small browser-side JS only if acceptable.
- If too heavy, defer QR generation and provide copyable links only.

Recommended Phase 4A behavior:

- Implement copyable links.
- Document QR generation as external or deferred unless a very small QR solution already exists.

## NFC-Friendly Explanation

Add UI help text:

```text
You can write this URL to an NFC tag or turn it into a QR code using your phone. The controller does not need NFC hardware; the tag just opens the URL.
```

## Acceptance Criteria

- QR/NFC-friendly URLs exist.
- No NFC hardware is assumed.
- QR code generation is either lightweight or clearly deferred.

---

# 16. Link Security / Safety Boundary

## Goal

Keep local URL control useful but not reckless.

## Required Behavior

Links are local-network control URLs.

Do not expose cloud access.

Do not add remote internet tunneling.

If the project already has local auth/API token support, respect it.

If not, document that these URLs work on the local network and should not be exposed publicly.

## Optional Token Support

If Phase 1/2 added an API token, support optional token query parameter.

Do not add a huge auth system in Phase 4A unless already planned.

## Acceptance Criteria

- Scene links are local-only.
- Documentation warns not to expose controller to public internet.
- No cloud dependency added.

---

# Part D — Interactions and Safety

---

# 17. Scheduler Interaction With Night Guard

## Goal

Schedules should respect Night Guard rules.

## Required Behavior

When a schedule triggers:

- Night Guard brightness cap still applies.
- If schedule target scene/mode is blocked by Night Guard, do not run it.
- Log/report skipped/blocked event.
- Show warning/status in diagnostics if practical.

Alternative:

- If blocked, run Warm Dim Now fallback.

Recommended:

- Block and report first. Do not surprise the user by changing to a different scene unless explicitly configured later.

## Acceptance Criteria

- Night Guard cap applies to scheduled outputs.
- Blocked schedule targets fail safely.
- Diagnostics can explain blocked event if implemented.

---

# 18. Scheduler Interaction With Manual Controls

## Goal

Manual actions should remain possible and predictable.

## Required Behavior

If user manually changes lights after a schedule runs:

- Manual change simply changes current state.
- Schedule remains enabled for future occurrences.

If schedule triggers during active manual timer/fade:

- Use documented behavior from schedule fade section.

Recommended:

- Schedule overrides active timer/fade.

If user disables schedule:

- It should not run until re-enabled.

## Acceptance Criteria

- Manual controls still work.
- Schedules do not lock the user out.
- Active timer conflict behavior is documented.

---

# 19. Scheduler Interaction With Transitions and Palettes

## Goal

Scheduled events should use existing polish systems.

## Required Behavior

If schedule loads a scene:

- Scene palette reference restores.
- Smooth transition/fade applies if configured.
- Output pipeline handles brightness/gamma/calibration.

If schedule triggers action Off/Warm Dim:

- Existing transition/fade behavior applies.

## Acceptance Criteria

- Scheduled scene with palette works.
- Scheduled scene transitions cleanly.
- No double-transition/fade chaos.

---

# 20. Scheduler Interaction With Surprise Me

## Goal

Avoid surprising surprises unless explicitly requested.

## Required Minimum

Do not schedule Surprise Me unless action support already exists and is explicitly selected.

Optional:

- Allow targetType `action`, targetId `surprise:calm` or similar only if safe and already implemented.

Recommended for Phase 4A:

- Do not include Surprise Me as default schedule target.
- Document as future optional target.

## Acceptance Criteria

- Scheduler does not randomly choose Surprise Me unless user explicitly configured such action.
- Surprise Me system remains intact.

---

# 21. Scheduler Interaction With Diagnostics Tests

## Goal

Diagnostics tests should not permanently block schedules.

## Required Behavior

If a diagnostic LED test is active when a schedule fires:

Choose one behavior and document it.

Recommended:

- Stop diagnostic test and run scheduled event.

Alternative:

- Skip schedule and report diagnostic test was active.

Recommended for Phase 4A:

- Stop test and run schedule, because schedules are intended routines.

## Acceptance Criteria

- Behavior is implemented and documented.
- Diagnostic tests still work normally.

---

# Part E — UI / API / Persistence Details

---

# 22. Schedule Persistence Error Handling

## Goal

Schedules must fail safely.

## Required Error Cases

Handle:

- Missing `/schedule.json`
- Corrupt `/schedule.json`
- Too many entries
- Invalid day mask
- Invalid time
- Missing target scene/action
- Schedule save failure
- Time sync unavailable

## Required Behavior

- Do not crash.
- Return JSON error where appropriate.
- Show UI error message.
- Track load/save status if practical.
- Diagnostics should show schedule file error if available.

## Acceptance Criteria

- Schedule errors are visible and safe.
- Corrupt schedule file does not brick controller.

---

# 23. Schedule Entry ID and Name Rules

## Goal

Keep schedule entries stable and URL/API-safe.

## Required Rules

Schedule ID:

- Lowercase
- URL-safe
- Generated from name + suffix if needed
- Stable after creation unless explicitly regenerated

Name:

- User-facing
- Non-empty
- Trimmed
- Max length, such as 24–32 characters
- HTML-escaped in UI

## Acceptance Criteria

- IDs are stable.
- Duplicate names handled safely.
- Invalid names rejected.

---

# 24. Main UI Summary Additions

## Goal

Expose schedule status without clutter.

## Required Main Page Summary

Add compact summary to main page or current state card:

- Next scheduled event, if time synced and schedule exists
- Time sync warning if not synced
- Link to Schedule page

Example:

```text
Next schedule: Evening Cozy today at 9:30 PM
```

or:

```text
Schedules paused: time not synced
```

## Acceptance Criteria

- Main page shows useful schedule summary.
- Full editor is not forced onto main page if using `/schedule`.

---

# 25. Settings / Schedule Separation

## Goal

Keep persistent files conceptually clean.

## Required Separation

- `/settings.json` = controller configuration/current state
- `/scenes.json` = scenes
- `/palettes.json` = palettes
- `/schedule.json` = schedule entries

Do not dump all schedules into settings unless the existing architecture strongly requires it.

## Acceptance Criteria

- Storage separation is documented.
- Existing settings/scenes/palettes still load.

---

# 26. Compile-Time / Runtime Guardrails

## Goal

Keep the scheduler efficient.

## Avoid

- Checking schedules with heavy date calculations every millisecond.
- Writing schedule file every minute.
- Re-parsing schedule JSON every loop.
- Large HTML/JS bloat.
- Huge QR libraries.
- Unbounded schedule entries.

## Prefer

- Check schedule once per second or once per loop with cheap minute comparison.
- Cache schedule entries in bounded array.
- Calculate next run only for UI/API requests or occasionally.
- Use browser-side JS for UI calculations where reasonable.
- Keep JSON compact.

## Acceptance Criteria

- Scheduler is non-blocking.
- Web UI remains responsive.
- OTA remains possible.

---

# 27. Documentation / Report Requirement

Create or update a report document.

Suggested file:

```text
BedroomLedController_Phase_4A_Report.md
```

The report should include:

1. Files changed.
2. Time sync implementation.
3. Timezone/offset behavior.
4. Schedule data model.
5. Schedule storage file and schema.
6. Schedule API endpoints.
7. Schedule UI/editor behavior.
8. Schedule execution logic.
9. Skip next / run now / enable-disable behavior.
10. Schedule fade/transition behavior.
11. Scene/action link routes.
12. QR/NFC-friendly URL behavior.
13. Whether QR code generation was implemented or deferred.
14. Local-network security notes.
15. Interactions with Night Guard, timers, transitions, palettes, diagnostics tests, and Surprise Me.
16. Resource report.
17. Compile/test status.
18. Known limitations.
19. What was intentionally deferred.
20. Recommended next step: Phase 4B remaining mode library batch 1.

## Required Honesty

Do not claim:

- Full timezone database support unless implemented.
- Perfect DST automation unless implemented.
- Cloud scheduling.
- Public internet access.
- NFC hardware support.
- QR generation if only copyable links exist.
- Physical controls.
- Microphone behavior.
- Light sensor behavior.
- Actual current measurement.
- Physical LED testing unless actual testing happened.

If only compiled, say:

```text
Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.
```

If compile was not run, say that clearly.

If resource values were not measured, say `not measured`.

---

# Suggested Implementation Order

Follow this order to reduce chaos:

## Step 1 — Inventory Existing Systems

Identify:

- Current settings struct/object
- Current scene load/action functions
- Current timer/fade system
- Current transition system
- Current Night Guard checks
- Current diagnostics page/API
- Current UI route structure
- Current filesystem helpers

Do not rewrite the whole sketch.

## Step 2 — Add Time Settings and Status

Add:

- UTC offset setting
- Time synced flag/status
- Time sync helpers
- `/api/time`
- Diagnostics time status

## Step 3 — Add NTP Sync

Add non-blocking or minimally blocking NTP sync.

Add manual sync endpoint.

## Step 4 — Add Schedule Data Model

Add bounded schedule entry list and defaults.

## Step 5 — Add Schedule Storage

Implement `/schedule.json` load/save with safe fallback.

## Step 6 — Add Schedule API

Add list/save/delete/enable/skip-next/run-now endpoints.

## Step 7 — Add Schedule Execution Engine

Add service function, once-per-minute/occurrence detection, and target execution.

## Step 8 — Add Schedule UI

Add `/schedule` page or schedule section.

Use vanilla JS.

## Step 9 — Add Scene/Action Link Routes

Add `/go` route or equivalent.

Reuse existing scene/action logic.

## Step 10 — Add Copyable Link UI

Add copy links for scenes/actions.

Optionally add browser-side QR if very lightweight; otherwise document deferred.

## Step 11 — Integrate Diagnostics/Main Summary

Add schedule/time summary to diagnostics and main page.

## Step 12 — Test Interactions

Test Night Guard, timers, transitions, palettes, diagnostic tests, scene links.

## Step 13 — Compile and Resource Report

Compile for D1 mini / ESP8266 and gather measurements where possible.

## Step 14 — Create Phase 4A Report

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

## Time Sync

- `/api/time` returns valid JSON.
- Time sync status displays.
- Manual sync endpoint works or reports failure safely.
- Timezone/UTC offset setting works.
- Time not synced warning appears.
- Schedules do not fire without valid time.

## Schedule Storage

- Missing `/schedule.json` is safe.
- Valid schedule file loads.
- Corrupt schedule file falls back safely.
- Schedule entries persist after reboot.
- Entry count limit is enforced.

## Schedule API

- `/api/schedule` lists entries.
- Save/create entry works.
- Edit entry works.
- Delete entry works.
- Enable/disable works.
- Skip next works.
- Run now works.
- Invalid day mask returns error.
- Invalid time returns error.
- Invalid target returns error.

## Schedule UI

- Schedule page/section loads.
- Add entry works.
- Edit entry works.
- Delete entry works.
- Day selection works.
- Time input works.
- Target selection works.
- Fade duration works.
- Run now works.
- Skip next works.
- Enable/disable works.
- Time sync status visible.
- Next run shown if implemented.

## Schedule Execution

- Enabled schedule fires at matching local day/time.
- Disabled schedule does not fire.
- Entry fires once per occurrence.
- Skip next skips exactly one occurrence.
- Scene target loads.
- Off action target works.
- Warm Dim Now target works.
- Fade/transition behavior works as documented.

## Scene / Action Links

- `/go?scene=sleep` or chosen equivalent works.
- `/go?action=off` works.
- `/go?action=warmDimNow` works.
- Invalid scene/action shows helpful error.
- JSON response option works if implemented.
- Copy link UI works.
- QR support works if implemented, or deferred clearly.

## Interaction Tests

- Night Guard caps scheduled outputs.
- Night Guard blocks blocked schedule targets safely.
- Schedule with palette scene restores palette.
- Schedule transition/fade does not double-fade.
- Schedule firing during diagnostic test follows documented behavior.
- Schedule firing during active timer/fade follows documented behavior.
- Manual scene/action links respect transitions and Night Guard.

## Main UI / Diagnostics

- Main page shows schedule summary or link.
- Diagnostics shows time/schedule status.
- Warnings appear for unsynced time or schedule errors.

## Resource Report

- Sketch size reported or marked not measured.
- LittleFS usage reported or marked not measured.
- Free heap after boot reported or marked not measured.
- Free heap after key pages/APIs reported or marked not measured.
- OTA status reported.
- D1-mini-vs-ESP32 recommendation included.

## Regression

Verify existing basics still work:

- Color picker
- White temperature slider
- Master brightness slider
- Mode filtering
- Existing modes
- New Phase 3D modes, if present
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

---

# Phase 4A Non-Goals Reminder

Do not implement these yet:

- Remaining Phase 4 lighting modes
- Full advanced scene import/export
- Full favorite drag/reorder system
- Physical NFC reader
- Physical buttons
- Rotary encoder
- Microphone/music-reactive behavior
- Light sensor/room-aware brightness
- Cloud scheduling
- Public internet remote access
- ESP32-specific rewrite

If tempted, write it in the report as deferred.

The scheduler may hold a clipboard. It may not summon cloud demons.

---

# Final Response Format For Codex

When finished, respond with:

```text
Phase 4A complete.

Files changed:
- ...

Implemented:
- ...

Time sync:
- ...

Scheduler:
- Data model: ...
- Storage: ...
- Execution behavior: ...
- UI: ...
- API: ...

Scene/action links:
- Routes: ...
- Copyable links: ...
- QR support: implemented/deferred
- NFC-friendly behavior: ...

Interactions:
- Night Guard: ...
- Timers/bedtime: ...
- Transitions: ...
- Palettes: ...
- Diagnostics tests: ...

Resource report:
- Board target used for compile:
- Sketch size:
- Sketch size percentage of available app slot, if known:
- LittleFS usage:
- Free heap after boot:
- Free heap after loading main page:
- Free heap after /api/state:
- Free heap after /api/scenes:
- Free heap after /api/schedule:
- Free heap after opening schedule editor:
- Free heap after triggering a scene link:
- Free heap after schedule check cycle, if measurable:
- OTA still works: yes/no/not tested
- Web UI responsiveness: snappy/acceptable/laggy/not tested
- Scheduler responsiveness: snappy/acceptable/laggy/not tested
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
- Phase 4B — remaining mode library batch 1.
```

Be honest and specific.

Do not claim physical LED testing unless it actually happened.

Do not claim full timezone database support, perfect DST automation, cloud scheduling, public internet access, NFC hardware, physical controls, microphone behavior, light sensor behavior, ESP32 migration, or actual current measurement unless those actually exist.

---

# One-Sentence Mission Summary

Add a D1-mini-safe local weekly scheduler and stable scene/action URLs so the Bedroom LED Controller can run routines and respond to QR/NFC-friendly links without cloud services, new hardware, blocking code, or firmware goblin overreach.

