# BedroomLedController Resource And Migration Report

## Final resource report

- Board target used for compile: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 58,948 / 80,192 bytes (73%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 587,436 / 1,048,576 bytes (56%).
- LittleFS total/used/free on a live device: not measured.
- Runtime free heap after boot: not measured.
- Free heap after loading main page `/`: not measured.
- Free heap after `/api/state`: not measured.
- Free heap after `/api/modes`: not measured.
- Free heap after `/api/scenes`: not measured.
- Free heap after `/api/palettes`: not measured.
- Free heap after `/api/schedule`: not measured.
- Free heap after `/api/diagnostics`: not measured.
- Free heap after `/api/backup/export`: not measured.
- OTA upload behavior: not tested.
- Web UI responsiveness: not tested on hardware.
- Animation responsiveness: not tested on hardware.
- Physical LED behavior: not tested.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Resource diagnostics added

The firmware now exposes live resource metrics in `/api/diagnostics`:

- `freeHeap`, `freeHeapNow`
- `maxFreeBlockSize`
- `heapFragmentationPercent`
- `minFreeHeapSinceBoot`
- `sketchSizeBytes`
- `freeSketchSpaceBytes`
- `littleFsTotalBytes`, `littleFsUsedBytes`, `littleFsFreeBytes`
- `endpointHeapMetrics`

Endpoint metrics are populated as routes are exercised on the running device. They include heap before/after, sampled minimum heap, heap delta, and payload byte count.

## Heavy endpoint hardening

- `/api/modes` streams the response in chunks instead of building one full route payload.
- `/api/backup/export` streams backup sections through chunked output.
- `/api/state`, `/api/scenes`, `/api/palettes`, `/api/schedule`, and `/api/diagnostics` are measured for heap before/after.
- Existing JSON field names and route names were preserved.

## Fixed-buffer migration

Selected long-lived values were moved away from heap-backed `String` storage:

- `FavoriteItem.id`
- `FavoriteItem.label`
- `FavoriteItem.type`
- `FavoriteItem.target`
- `TimeSyncState.timeStatus`

Scene, palette, and schedule persistent structs still contain `String` fields. Those are good candidates for a staged future migration if runtime heap or fragmentation readings show pressure.

## Migration recommendation

The completed build can remain on the D1 mini. IRAM is still the main ceiling at 93%, and RAM globals are now 73% after adding endpoint metrics and fixed buffers. Future expansion should move to ESP32-S3 if it adds heavy UI pages, richer backup restore, sensors, cloud features, physical controls, or more persistent data types.

## Stay on D1 mini when

- The project remains local-only.
- New changes are maintenance-only or small.
- Preview remains browser-side.
- No heavy libraries are added.
- Runtime diagnostics show acceptable free heap and fragmentation.

## Move to ESP32-S3 when

- Bulk backup restore needs to apply large multi-section payloads safely.
- Runtime heap measurements show low margin.
- More hardware inputs or sensors are added.
- UI/API payloads keep growing.
- OTA free sketch space becomes tight.
