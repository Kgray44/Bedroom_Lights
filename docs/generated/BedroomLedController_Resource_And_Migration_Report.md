# BedroomLedController Resource And Migration Report

## Final resource report

This final resource report now includes compile resources and live local hardware measurements for the 140-LED D1 mini build.

## Local hardware measurement pass

Measurement date: 2026-05-16.

- Board target used for compile: `esp8266:esp8266:d1_mini`
- USB upload port: `COM5`
- Board detected: ESP8266EX
- Board MAC reported by uploader: `c8:2b:96:30:46:05`
- Hostname reached: `http://bedroom-leds.local`
- Live IP reached: `192.168.1.201`
- LED count configured: 140
- Local test default brightness: 100
- OTA firmware upload: not performed
- OTA page reachability: `/ota` returned HTTP 200
- Browser updater reachability: `/update` returned HTTP 200 with credentials; no upload was performed
- Safe LED diagnostic endpoints called: red, green, blue, whiteLow, restore
- Physical visual LED output: not independently observed; not measured by Codex

Full measurement artifact:

- `docs/generated/live_hardware_measurements_2026-05-16.json`

## Compile resources

- RAM globals/statics: 59,896 / 80,192 bytes (74%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 588,188 / 1,048,576 bytes (56%).
- Live `sketchSizeBytes`: 647,552.
- Live `freeSketchSpaceBytes`: 1,445,888.

The firmware compiled and uploaded successfully over USB to the D1 mini.

## Live filesystem resources

- LittleFS total: 2,072,576 bytes.
- LittleFS used: 49,152 bytes.
- LittleFS free: 2,023,424 bytes.

## Live heap snapshots

| Measurement point | Free heap | Max free block | Fragmentation | Min free heap since boot |
| --- | ---: | ---: | ---: | ---: |
| After boot/resource check | 7,768 | 3,960 | 32% | 3,888 |
| After main page `/` | 7,768 | 6,376 | 8% | 3,888 |
| After `/api/state` | 7,880 | 6,720 | 5% | 3,888 |
| After `/api/modes` | 7,736 | 6,568 | 5% | 3,888 |
| After `/api/scenes` | 7,880 | 6,720 | 5% | 2,000 |
| After `/api/palettes` | 7,880 | 3,992 | 33% | 2,000 |
| After `/api/schedule` | 7,880 | 5,936 | 16% | 2,000 |
| After `/api/backup/export` | 7,848 | 6,568 | 7% | 2,000 |
| After control and LED-test endpoint calls | 7,880 | 6,560 | 7% | 2,000 |
| After OTA page checks | 7,664 | 6,256 | 9% | 2,000 |

## Endpoint heap metrics

| Route | Heap before | Heap after | Route minimum | Delta | Payload bytes |
| --- | ---: | ---: | ---: | ---: | ---: |
| `/api/diagnostics` | 7,848 | 3,888 | 3,888 | -3,960 | 3,496 |
| `/` | 7,880 | 6,536 | 6,536 | -1,344 | 97,471 |
| `/api/state` | 7,880 | 5,664 | 5,664 | -2,216 | 2,051 |
| `/api/modes` | 7,880 | 5,264 | 5,264 | -2,616 | 18,305 |
| `/api/scenes` | 7,880 | 2,000 | 2,000 | -5,880 | 5,509 |
| `/api/palettes` | 7,768 | 3,936 | 3,936 | -3,832 | 2,964 |
| `/api/schedule` | 7,880 | 7,216 | 7,216 | -664 | 323 |
| `/api/backup/export` | 7,768 | 6,504 | 6,504 | -1,264 | 12,363 |

`/api/scenes` is the tightest measured route. It dropped the route-local heap floor to 2,000 bytes. The current build recovered afterward, but this is a serious ESP8266 margin warning.

## Resource diagnostics behavior

- `/api/diagnostics` parses as valid JSON after the measurement fix.
- Detailed endpoint metrics are exposed through compact `/api/resources`.
- `/api/diagnostics` now points to `/api/resources` for detailed endpoint metrics to avoid making the already-large diagnostics payload fail under low heap.

## Hardware observations

- The board booted after USB upload.
- Wi-Fi connected and hostname resolution worked through `bedroom-leds.local`.
- Basic control endpoints returned HTTP 200.
- Safe LED diagnostic endpoints returned HTTP 200.
- Codex cannot visually confirm LED colors from this environment; physical LED output remains user-observed/not independently verified.

## Migration recommendation

The build still works on the D1 mini, but the 140-LED configuration makes heap margin tight. Stay on the D1 mini for this completed feature set only if no major features are added and live measurements remain stable. Move to ESP32-S3 for future expansion, richer JSON routes, larger UI pages, more persistent data, sensors, physical controls, or any work that increases heap pressure.
