# BedroomLedController Phase 4A Report

Phase 4A adds a D1-mini-safe local weekly scheduler and stable scene/action links. The work stays software-only: no NFC hardware, no sensors, no cloud dependency, and no ESP32 rewrite.

## Files changed

- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/Config.h`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/ScheduleControls.h`
- `firmware/BedroomLedController/ScheduleUi.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `firmware/BedroomLedController/Diagnostics.h`
- `docs/generated/BedroomLedController_Phase_4A_Report.md`
- `docs/generated/Phase_Completion.md`
- `tests/test_phase4a_contract.py`

## Time sync implementation

- Added ESP8266 time support through `<time.h>`.
- Added `TimeSyncState` and the loop services `serviceTimeSync();` and `serviceScheduler();`.
- Added `beginTimeSync(bool manual)` using `configTime(0, 0, "pool.ntp.org", "time.nist.gov")`.
- Added `/api/time`, `/api/time/sync`, and `/api/time/set`.
- Schedules do not run unless time is synced; manual schedule run and `/go` links can still run without synced time.

## Timezone/offset behavior

- Added `settings.localUtcOffsetMinutes`.
- Default is `DEFAULT_LOCAL_UTC_OFFSET_MINUTES = -240`, matching New York daylight time for the May 2026 project context.
- This is a simple fixed UTC offset, not a timezone database and not automatic DST handling.
- The offset is persisted in settings JSON and visible in the schedule page, main state JSON, and diagnostics.

## Schedule data model

- Added bounded `ScheduleEntry schedules[MAX_SCHEDULE_ENTRIES]` with `MAX_SCHEDULE_ENTRIES = 8`.
- Schedule fields include id, name, enabled, daysMask, minuteOfDay, targetType, targetId, fadeSeconds, useTransition, skipNext, lastRunEpoch, and lastRunOccurrenceKey.
- Day mapping is bit 0 Sunday through bit 6 Saturday.
- Schedule IDs are URL/API-safe and stable.

## Schedule storage

- Added a separate `/schedule.json` file.
- Schedule storage is intentionally separate from `/settings.json`, `/scenes.json`, `/favorites.json`, and `/palettes.json`.
- Missing or corrupt schedule storage fails safe: schedules are disabled/empty rather than blocking controller boot.

## Schedule API endpoints

- `/api/schedule` lists entries and schedule/time status.
- `/api/schedule/save` creates or updates an entry.
- `/api/schedule/delete` removes an entry.
- `/api/schedule/toggle` enables or disables an entry.
- `/api/schedule/skip` arms or clears Skip next.
- `/api/schedule/run` runs a schedule immediately, even if time is not synced.

## Schedule UI/editor behavior

- Added a dedicated `/schedule` page.
- The editor supports name, enabled state, day checkboxes, local time, target type, target, fade seconds, and transition use.
- The page shows clock sync status and lets the user save the fixed UTC offset or request a manual sync.
- The main page now shows Next schedule and links to the schedule page.
- Diagnostics now expose time sync, current local time, schedule count, next schedule, last schedule status, and storage status.

## Schedule execution logic

- Scheduler checks once per second and only executes once per matching local minute.
- Schedules are cached in memory; the firmware does not reparse `/schedule.json` inside the loop.
- Schedule targets reuse existing scene/action paths:
  - Scene targets call `loadSceneById`.
  - Action targets call `runActionByName`.
- Scheduled events stop active diagnostic tests and override active timers/fades.
- Scene targets continue to respect Night Guard blocking.
- Night Guard brightness caps still apply through the existing render/output path.

## Skip next

- `skipNext` skips exactly one matching occurrence.
- When the skipped occurrence is reached, `skipNext` is cleared and the schedule remains enabled for future weeks.

## Fade and transition behavior

- `useTransition` controls whether non-fade schedule execution uses the existing transition engine.
- `fadeSeconds` reuses the existing timer fade path with minute granularity, rounding up to the next minute.
- Fade schedules call `startTimer(TIMER_MODE_FADE_TO_ACTION, ...)`.

## Scene/action link routes

- Added `/go` as the stable local wrapper for scenes and actions.
- Supported links include:
  - `/go?scene=sleep`
  - `/go?action=off`
  - `/go?action=warmDimNow`
  - `/go?scene=sleep&format=json`
- HTML is the default response for human use.
- `format=json` returns a JSON response for simple automation.
- The route reuses `loadSceneById` and `runActionByName` instead of duplicating lighting logic.

## QR/NFC-friendly URL behavior

- The main scene UI now has Copy Link buttons for scene URLs.
- Quick action copy buttons were added for Off and Warm Dim Now.
- The schedule page explains that these are QR/NFC-friendly local URLs.
- QR code generation was deferred.
- NFC writing/hardware support was not added.

## Local-network security notes

- `/go` is intended for the same local network as the controller.
- No public internet exposure, cloud broker, account login, or remote relay was added.
- Existing Wi-Fi/AP and OTA behavior remain unchanged.

## Night Guard

- Scheduled scene loads use the existing `loadSceneById` path, so Night Guard blocks unsafe/flashing scenes the same way it does for manual scene loads.
- Scheduled actions use existing action paths and remain subject to the normal brightness/output behavior.

## Resource report

Measured with Arduino CLI compile for `esp8266:esp8266:d1_mini` on the worktree sketch:

- RAM: 51,560 / 80,192 bytes (64%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash: 538,056 / 1,048,576 bytes (51%).
- Free heap after /api/schedule: not measured.
- Free heap after opening schedule editor: not measured.
- LittleFS space after schedule save: not measured.
- Scheduler responsiveness on hardware: not measured.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Deferred to later phases

- Full timezone database and automatic DST handling.
- QR code rendering inside the firmware UI.
- NFC tag writing or NFC hardware.
- Cloud, remote access, sensors, or ESP32-only features.
- Phase 4B can build on this scheduler/link foundation without rewriting it.
