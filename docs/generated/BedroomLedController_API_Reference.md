# BedroomLedController API Reference

All routes are local GET routes served by the ESP8266. Large JSON imports are constrained by browser and ESP8266 URL limits.

## State and output

- `GET /api/state`
- `GET /state`
- `GET /api/brightness?value=0..255`
- `GET /api/animation?strength=0..255`
- `GET /api/intensity?value=0..255` (compatibility alias for Animation Strength)
- `GET /api/color?hex=%23RRGGBB`
- `GET /api/temperature?kelvin=2700..6500`
- `GET /api/mode?mode=solid`
- `GET /api/power`
- `GET /api/diagnostics`
- `GET /api/resources`

## Scenes

- `GET /api/scenes`
- `GET /api/scenes/save?name=Reading`
- `GET /api/scenes/load?id=reading`
- `GET /api/scenes/rename?id=reading&name=New`
- `GET /api/scenes/delete?id=reading`
- `GET /api/scenes/duplicate?id=reading`
- `GET /api/scenes/export?id=reading`
- `GET /api/scenes/import?json=...`
- `GET /api/scenes/reset-builtins`

## Palettes

- `GET /api/palettes`
- `GET /api/palettes/select?id=warm-linen&enabled=1`
- `GET /api/palettes/save?...`
- `GET /api/palettes/update?id=...`
- `GET /api/palettes/delete?id=...`
- `GET /api/palettes/export?id=...`
- `GET /api/palettes/import?json=...`
- `GET /api/palettes/reset-builtins`
- `GET /api/palettes/reset`

## Favorites, timers, and schedule

- `GET /api/favorites`
- `GET /api/favorites/load?id=...`
- `GET /api/favorites/reset`
- `GET /api/timer`
- `GET /api/timer/start?...`
- `GET /api/timer/cancel`
- `GET /api/bedtime/start?...`
- `GET /api/bedtime/cancel`
- `GET /api/schedule`
- `GET /api/schedule/save?...`
- `GET /api/schedule/delete?id=...`
- `GET /api/schedule/toggle?id=...`
- `GET /api/schedule/skip?id=...`
- `GET /api/schedule/run?id=...`
- `GET /api/schedule/export`
- `GET /api/schedule/import?json=...`
- `GET /api/schedule/reset?confirm=YES`

## Backup and reset

- `GET /api/backup/export`
- `GET /api/backup/import?confirm=YES&json=...`
- `GET /api/reset?target=scenes-builtins`
- `GET /api/reset?target=palettes-builtins`
- `GET /api/reset?target=favorites`
- `GET /api/reset?target=schedule&confirm=YES`
- `GET /api/reset?target=settings&confirm=YES`
- `GET /api/reset?target=factory&confirm=YES`
- `GET /api-docs`

## Diagnostics resource fields

`GET /api/diagnostics` includes runtime resource fields when running on the device:

- `animationStrength`, `animationStrengthPercent`
- `freeHeap`, `freeHeapNow`
- `maxFreeBlockSize`
- `heapFragmentationPercent`
- `minFreeHeapSinceBoot`
- `sketchSizeBytes`
- `freeSketchSpaceBytes`
- `littleFsTotalBytes`, `littleFsUsedBytes`, `littleFsFreeBytes`
- `endpointHeapMetrics`

Detailed endpoint heap metrics are available from `GET /api/resources`. The compact resource route is preferred for measurement because the full diagnostics payload is already large on the ESP8266.

Live endpoint heap readings were measured during the local hardware pass and recorded in `BedroomLedController_Resource_And_Migration_Report.md`.

Physical visual LED output was not measured by Codex, and no OTA firmware upload was performed.
