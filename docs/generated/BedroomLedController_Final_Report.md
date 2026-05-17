# BedroomLedController Final Report

The project now implements a local-first D1 mini / ESP8266 bedroom LED controller with web UI, OTA support, diagnostics, settings persistence, scenes, favorites, Night Guard, timers, bedtime fade, smooth transitions, palettes, browser preview, Surprise Me, schedule automation, scene links, a full mode library, final management tools, and a release hardening pass.

Compiled and uploaded successfully over USB to the D1 mini / ESP8266 target with 140 LEDs configured. Safe LED diagnostic endpoints were called, but physical visual LED output was not independently observed or measured by Codex.

## Final feature set

- Local web UI served directly from the ESP8266.
- WS2812 output through the central frame buffer and output pipeline.
- Settings persistence in LittleFS.
- Scene save/load/rename/delete/duplicate/export/import and built-in restore.
- Palette builder, palette selection, palette export/import, and built-in palette restore.
- Favorites and quick actions.
- Night Guard brightness and flashing-mode protection.
- Timers, bedtime fade, and schedule automation.
- Browser-side preview without server-side LED frame streaming.
- Surprise Me scene generation.
- OTA page and browser updater route.
- Diagnostics, power estimate, and local API docs.
- Full backup export and guarded backup-import schema validation.

## D1 mini vs ESP32-S3 final recommendation

Stay on the D1 mini for this completed build because it still compiles, uploads, boots, and serves the web API with 140 LEDs configured. Do not keep expanding this firmware aggressively on ESP8266: IRAM is 93%, RAM globals/statics are 74%, and live heap measurements showed only a 2,000 byte route-local heap floor during `/api/scenes`. Move to ESP32-S3 for the next major hardware generation, especially if adding sensors, physical controls, richer backup restore, larger UI pages, or more diagnostics.

## Finalization / Hardening Pass

- Moved Wi-Fi and OTA credentials out of tracked source and added `Secrets.example.h`.
- Added root `README.md` with first-time setup, safety notes, test/build commands, and generated doc links.
- Added runtime resource diagnostics for free heap, largest free block, heap fragmentation, minimum free heap since boot, sketch size, free sketch space, and LittleFS capacity.
- Added endpoint resource tracking for the main page and heavy API routes.
- Added chunked JSON output for `/api/modes` and `/api/backup/export`.
- Migrated selected long-lived fields to fixed char buffers: `FavoriteItem` id/label/type/target and `TimeSyncState.timeStatus`.
- Preserved public route names, JSON field names, and storage field names for the changed data.
- Preserved the central render/output pipeline and did not downgrade renderer math or add lighting modes.
- OTA upload behavior was not retested.
- USB upload over `COM5` was tested successfully.
- `/ota` and authenticated `/update` page reachability were tested successfully; no OTA upload was performed.
- Safe LED diagnostic endpoints were called; physical visual LED output was not independently observed.

## Final measured compile resources

- Board target: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 59,896 / 80,192 bytes (74%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 588,188 / 1,048,576 bytes (56%).
- LittleFS usage: 49,152 / 2,072,576 bytes used; 2,023,424 bytes free.
- Runtime free heap after boot/resource check: 7,768 bytes.
- Lowest measured route-local heap floor: 2,000 bytes during `/api/scenes`.
- Python contract tests: `python -m unittest discover -s tests -v` passed, 124 tests.
- Physical visual LED behavior: not independently observed; not measured by Codex.

## Known limits

- Full backup export works, but full backup import validates schema only and does not apply a bulk restore.
- Runtime heap after large API calls was measured on hardware; `/api/scenes` is the tightest route.
- OTA page reachability was tested, but OTA firmware upload was not performed.
- Physical LED test endpoints were called, but visual LED output was not independently observed by Codex.
