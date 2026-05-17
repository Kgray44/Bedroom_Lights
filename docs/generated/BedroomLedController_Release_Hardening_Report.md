# BedroomLedController Release Hardening Report

## Release Hardening Summary

This pass focused on final release readiness, resource visibility, and heap-fragmentation risk reduction. No lighting modes were added. No renderer math was downgraded. Mode visual quality preservation was treated as a hard boundary.

## Secrets And Configuration

- Secrets removed from tracked source.
- Added `firmware/BedroomLedController/Secrets.example.h`.
- Added `.gitignore` coverage for `firmware/BedroomLedController/Secrets.h`.
- Updated `Config.h` to include local `Secrets.h` when present and fall back to `Secrets.example.h` for compile-safe examples.
- `Secrets.h` must be copied locally, edited with Wi-Fi/OTA credentials, and kept out of git.
- Any credentials previously committed should be rotated.

## Runtime Heap Metrics

Runtime heap metrics are now exposed for the running device.

`/api/diagnostics` now reports live resource fields:

- `freeHeap`, `freeHeapNow`, `maxFreeBlockSize`, `heapFragmentationPercent`
- `minFreeHeapSinceBoot`
- `sketchSizeBytes`, `freeSketchSpaceBytes`
- `littleFsTotalBytes`, `littleFsUsedBytes`, `littleFsFreeBytes`
- `endpointHeapMetrics`

Endpoint measurement records route, measured time, heap before, heap after, sampled minimum heap, heap delta, and payload bytes for recent heavy routes.

## Endpoint Resource Tracking

Added compact endpoint heap tracking without `String` fields in the metric struct. The pass instruments:

- `/`
- `/api/state`
- `/api/modes`
- `/api/scenes`
- `/api/palettes`
- `/api/schedule`
- `/api/diagnostics`
- `/api/backup/export`

Runtime values are available after exercising the routes on the device. They were not measured on physical hardware in this pass.

## Heavy JSON Handling

- Backup export streaming was added for `/api/backup/export` using chunked `sendContent(...)` output.
- `/api/modes` now streams mode metadata one item at a time instead of building one full response string for the route.
- `/api/scenes`, `/api/palettes`, `/api/schedule`, and `/api/diagnostics` are measured but still build bounded JSON strings in memory.
- Existing JSON field names and route names were preserved.

## Fixed-Size Buffer Migration

Fixed-size buffer migration was applied to selected long-lived structs:

- `FavoriteItem.id`
- `FavoriteItem.label`
- `FavoriteItem.type`
- `FavoriteItem.target`
- `TimeSyncState.timeStatus`

The saved JSON field names remain unchanged. Existing favorites JSON should continue to load; oversized values are truncated safely to the configured buffer sizes.

Scene, palette, and schedule structs still contain `String` fields and should be migrated only in a staged follow-up if runtime diagnostics show pressure or if more persistent data is added.

## Static UI And Metadata

- Existing main UI and diagnostics/schedule pages remain flash-backed where practical through `PROGMEM`/`send_P`.
- Large API docs HTML is still generated dynamically; moving it fully to `PROGMEM` remains a low-risk future cleanup.

## Output Pipeline Preservation

The output flow remains centralized:

1. Render mode, transition, or diagnostic test frame into `frameBuffer`.
2. Apply RGB calibration gains.
3. Apply effective brightness and Night Guard cap.
4. Apply gamma correction when enabled.
5. Write pixels and call `strip.show()` centrally.

No normal lighting renderer was changed to write pixels directly or call `strip.show()`.

## Compile And Test Status

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

Latest compile:

- Board target: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 58,948 / 80,192 bytes (73%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 587,436 / 1,048,576 bytes (56%).

Python contract tests: `python -m unittest discover -s tests -v` passed, 124 tests.

## Known Remaining Risks

- Runtime free heap after boot: not measured.
- Free heap after loading main page: not measured.
- Free heap after `/api/state`: not measured.
- Free heap after `/api/modes`: not measured.
- Free heap after `/api/scenes`: not measured.
- Free heap after `/api/palettes`: not measured.
- Free heap after `/api/schedule`: not measured.
- Free heap after `/api/backup/export`: not measured.
- LittleFS total/used/free on a live flashed device: not measured.
- OTA upload behavior after this pass: not retested.
- Physical LED behavior: not measured.

## Recommendation

Stay on the D1 mini for the current completed build, but treat IRAM as the tight ceiling. Move to ESP32-S3 for future major expansion, especially if adding sensors, physical controls, cloud integrations, richer restore, or more persistent data.
