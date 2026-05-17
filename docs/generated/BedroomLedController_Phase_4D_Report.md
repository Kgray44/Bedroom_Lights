# BedroomLedController Phase 4D Report

Phase 4D completed the final polish and advanced scene-management pass for the local D1 mini / ESP8266 Bedroom LED Controller.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Implemented feature inventory

- Added advanced scene management routes for duplicate, export, import, and restore built-in scenes.
- Added scene warnings for missing mode, missing palette, Night Guard blocking, and powerBrightness recommendations.
- Added palette export/import plus built-in palette restore that preserves custom palettes where capacity allows.
- Added schedule export/import/reset controls.
- Added guarded backup export, schema-validating backup import, and selective reset endpoints.
- Added a compact `/api-docs` help page served by the ESP8266.
- Added main UI controls for scene import/export, scene duplicate, built-in restore, full backup export/import validation, factory reset, diagnostics, OTA, and API docs.
- Added diagnostics fields for firmware version, board target, LittleFS capacity, sketch size, free sketch space, and OTA/browser-update route presence.
- Raised scene and palette capacities to leave room for custom entries after the built-in library.

## Route map

- `/api/scenes/duplicate?id=...`
- `/api/scenes/export?id=...`
- `/api/scenes/import?json=...`
- `/api/scenes/reset-builtins`
- `/api/palettes/export?id=...`
- `/api/palettes/import?json=...`
- `/api/palettes/reset-builtins`
- `/api/schedule/export`
- `/api/schedule/import?json=...`
- `/api/schedule/reset?confirm=YES`
- `/api/backup/export`
- `/api/backup/import?confirm=YES&json=...`
- `/api/reset?target=scenes-builtins`
- `/api/reset?target=palettes-builtins`
- `/api/reset?target=favorites`
- `/api/reset?target=schedule&confirm=YES`
- `/api/reset?target=settings&confirm=YES`
- `/api/reset?target=factory&confirm=YES`
- `/api-docs`

## Persistent file audit

- `/settings.json`: settings and calibration; reset through `target=settings` or `target=factory`.
- `/scenes.json`: built-in and custom scenes; duplicate/import/export added.
- `/favorites.json`: favorite shortcuts; reset still available.
- `/palettes.json`: built-in and custom palettes; export/import/restore added.
- `/schedule.json`: schedule entries; export/import/reset added.
- Wi-Fi credentials and OTA password remain compile-time configuration and are excluded from backup export.

## Advanced scene management

Scenes can now be duplicated without overwriting the original, exported as schema-versioned JSON, imported as custom scenes, and checked for warnings before use. Built-in scene restore removes stale built-ins and re-adds the firmware defaults while preserving custom scenes when capacity allows.

## Palette/favorite/schedule polish

Palettes now have export/import and built-in restore endpoints. Favorites keep their reset route and are included in full backup export. Schedules can be exported, imported from a schedule array, or cleared with `confirm=YES`.

## Full backup export

`/api/backup/export` returns a schema-versioned JSON bundle containing scenes, palettes, favorites, schedule data, and diagnostics. It excludes Wi-Fi credentials, OTA password, runtime timer state, and runtime transition state.

## Full backup import

`/api/backup/import` is intentionally guarded. It requires `confirm=YES`, validates `backupSchema:1`, and reports `applied:false`; full multi-section restore was not applied because the D1 mini heap/URL-size risks are too high for a safe final-pass bulk restore. Use selective scene, palette, and schedule import routes for applied restore.

## Selective resets

Selective reset support now covers scenes built-ins, palette built-ins, favorites, schedule, settings, and factory reset. Destructive targets require `confirm=YES`.

## API docs/help

`/api-docs` serves a compact local route reference with state, scene, palette, favorite, timer, schedule, backup, and reset endpoints.

## Diagnostics hardening

Diagnostics now report `projectVersion`, `boardTarget`, LittleFS total/used/free bytes, sketch size, free sketch space, `otaRoutePresent`, and `browserUpdateRoutePresent`.

## Final resource report

- Board target used for compile: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 57,800 / 80,192 bytes (72%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 583,964 / 1,048,576 bytes (55%).
- LittleFS usage: not measured.
- Free heap after boot: not measured.
- Free heap after loading main page: not measured.
- Free heap after `/api/state`: not measured.
- Free heap after `/api/scenes`: not measured.
- Free heap after `/api/palettes`: not measured.
- Free heap after `/api/schedule`: not measured.
- Free heap after `/api/backup/export`: not measured.
- OTA upload behavior: not tested.
- Web UI responsiveness: not tested.
- Animation responsiveness: not tested.
- Physical LED behavior: not tested.

Recommendation: the D1 mini still compiles, but IRAM remains at 93% and RAM globals rose to 72%. Stay on the D1 mini for this completed build, avoid heavy new libraries or large server-side features, and prefer ESP32-S3 for any future expansion that adds richer management, sensors, networking, or larger UI payloads.

## Compile/test status

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase4d_contract -v`: passed, 5 tests.
- `python -m unittest discover -s tests -v`: passed, 120 tests.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- Physical LED test: not run.
