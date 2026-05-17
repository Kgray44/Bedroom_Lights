# BedroomLedController Final Report

The project now implements a local-first D1 mini / ESP8266 bedroom LED controller with web UI, OTA support, diagnostics, settings persistence, scenes, favorites, Night Guard, timers, bedtime fade, smooth transitions, palettes, browser preview, Surprise Me, schedule automation, scene links, a full mode library, final management tools, and a release hardening pass.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

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

Stay on the D1 mini for this completed build because it still compiles and preserves the original hardware target. Do not keep expanding this firmware aggressively on ESP8266: IRAM is 93%, RAM globals/statics are 73%, and future large UI/API additions will narrow the margin. Move to ESP32-S3 for the next major hardware generation, especially if adding sensors, physical controls, richer backup restore, larger UI pages, or more diagnostics.

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
- Physical LED behavior was not tested.

## Final measured compile resources

- Board target: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 58,948 / 80,192 bytes (73%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 587,436 / 1,048,576 bytes (56%).
- LittleFS usage: not measured.
- Runtime free heap: not measured.
- Python contract tests: `python -m unittest discover -s tests -v` passed, 124 tests.
- Physical LED behavior: not measured.

## Known limits

- Full backup export works, but full backup import validates schema only and does not apply a bulk restore.
- Runtime heap after large API calls can now be reported by diagnostics after routes are exercised, but no hardware readings were captured in this pass.
- OTA upload was not retested after Phase 4D.
- Physical LED behavior was not tested in this pass.
