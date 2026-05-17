# BedroomLedController Final Report

The project implements a local-first D1 mini / ESP8266 bedroom LED controller with a 140 LED WS2812 / NeoPixel strip, ESP-hosted web UI, OTA support, diagnostics, settings persistence, scenes, favorites, Night Guard, timers, bedtime fade, transitions, palettes, browser preview, Surprise Me, schedule automation, scene links, full mode library, and management/export tools.

## D1 mini vs ESP32-S3 final recommendation

Stay on D1 mini for the current build only. Move future feature expansion to ESP32-S3.

## Finalization / Hardening Pass

- Wi-Fi and OTA credentials were moved out of tracked source into ignored `Secrets.h`; `Secrets.example.h` remains tracked.
- Root `README.md` documents setup, safety, D3/GPIO0 warning, build/test commands, and generated docs.
- Runtime resource diagnostics report heap, max free block, fragmentation, sketch size, free sketch space, and LittleFS usage.
- Endpoint resource tracking is exposed when heap allows it.
- Heavy JSON endpoints were made safer: `/api/modes` was already streamed; `/api/scenes` and `/api/palettes` now stream; backup export streams large sections and omits full diagnostics.
- Long-lived fixed-buffer audit fields record the last mutation without `String`.
- Central render/output flow was preserved: frame buffer, calibration, effective brightness/Night Guard cap, gamma, pixel write, and central `strip.show()`.
- Mode renderer quality was preserved; Satin Breathing's envelope was fixed without removing fabric texture, warm peak blending, or palette support.

## Critical Runtime Regression Stabilization

- Fixed malformed `/api/state` JSON that could make refresh look like it reset controls.
- Added Web UI hydration/state guards so page load and `applyState(...)` cannot send mutation endpoints.
- Added valid low-heap fallbacks for mutation responses, resources, diagnostics, scenes, palettes, and backup export.
- Playwright page refresh check on the final firmware observed 14 read-only requests and `mutationCount=0`.
- Persistence check on the final firmware saved and re-read brightness `77`, color `00AAFF`, and mode `solid`.
- Satin Breathing API soak at brightness `100` stayed on `satinBreathing` for 43 seconds with timer inactive and schedule count 0.

## UI Feedback + Motion Smoothness Pass

This pass remains part of the final release history: startup loading is serialized, heavy UI requests are queued, browser preview uses cached state, and temporal smoothing is bounded and bypassed for utility/flashing/diagnostic output.

## Final measured status

- Compile target: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 60,844 / 80,192 bytes (75%)
- IRAM: 61,383 / 65,536 bytes (93%)
- Flash/IROM: 606,044 / 1,048,576 bytes (57%)
- USB upload: passed on `COM5`
- Live IP: `192.168.1.201`
- LittleFS: 49,152 / 2,072,576 bytes used
- Lowest measured heap during final Satin Breathing soak: 2,288 bytes since boot
- OTA page: `/ota` HTTP 200
- Browser updater: `/update` HTTP 401 without auth, HTTP 200 with local OTA credentials
- OTA firmware upload: not performed
- Physical LED visual behavior: not independently observed by Codex
- Long-duration physical visual behavior: not measured by Codex

## Tests

- Focused critical regression contract tests passed.
- Full Python contract suite passed: `python -m unittest discover -s tests -v`, 138 tests.
- Web UI JavaScript syntax passed with `node --check` on the extracted script.

## Known limits

- ESP8266 heap remains tight with 140 LEDs and this feature set.
- `/api/diagnostics` is compact by design on ESP8266; use `/api/resources` for live resource details.
- `/api/resources` may return `endpointHeapMetrics: []` when contiguous heap is too small.
- Full backup import still validates schema only and does not apply a bulk restore.
- OTA capability was reachability/auth tested; an OTA firmware upload was not performed.

## Recommendation

Stay on D1 mini for the current build only. Move future feature expansion to ESP32-S3.
