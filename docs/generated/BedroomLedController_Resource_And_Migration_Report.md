# BedroomLedController Resource And Migration Report

## Final resource report

This report reflects the final regression-stabilized D1 mini build with 140 LEDs configured and live hardware checks performed on `2026-05-17`.

## Compile resources

- Board target: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 60,844 / 80,192 bytes (75%)
- IRAM: 61,383 / 65,536 bytes (93%)
- Flash/IROM: 606,044 / 1,048,576 bytes (57%)
- USB upload size reported by uploader: 665,808 bytes written
- Live `sketchSizeBytes`: 665,808
- Live `freeSketchSpaceBytes`: 1,425,408

IRAM remains the tightest compile resource. Future major expansion should move to ESP32-S3.

## Live hardware context

- USB upload port: `COM5`
- Board detected by uploader: ESP8266EX
- Board MAC reported by uploader: `c8:2b:96:30:46:05`
- Live IP: `192.168.1.201`
- Expected hostname: `bedroom-leds`
- LED count configured: 140
- OTA page reachability: `/ota` HTTP 200
- Browser updater reachability: `/update` HTTP 401 without auth and HTTP 200 with local OTA credentials
- OTA firmware upload: not performed
- Physical LED visual output: not independently observed by Codex

## LittleFS

- Total: 2,072,576 bytes
- Used: 49,152 bytes
- Free: 2,023,424 bytes

## Final live heap snapshots

After setting brightness `77`, color `00AAFF`, and mode `solid`:

| Endpoint | Result | Free heap | Max block | Fragmentation | Min heap since boot |
| --- | --- | ---: | ---: | ---: | ---: |
| `/api/state` | valid JSON | not reported | not reported | not reported | not reported |
| `/api/resources` before heavy routes | valid JSON | 6,536 | 5,408 | 6% | 4,224 |
| `/api/diagnostics` | valid compact JSON | 5,488 | 4,064 | 24% | 4,224 |
| `/api/scenes` | valid JSON | not reported | not reported | not reported | not reported |
| `/api/palettes` | valid JSON | not reported | not reported | not reported | not reported |
| `/api/backup/export` | valid JSON | not reported | not reported | not reported | not reported |
| `/api/resources` after heavy routes | valid JSON | 6,680 | 5,536 | 6% | 4,224 |

During Satin Breathing at brightness `100`:

| Time | Mode | Brightness | Free heap | Max block | Fragmentation | Min heap since boot |
| ---: | --- | ---: | ---: | ---: | ---: | ---: |
| 11s | satinBreathing | 100 | 5,880 | 4,216 | 17% | 2,288 |
| 22s | satinBreathing | 100 | 5,880 | 4,096 | 18% | 2,288 |
| 33s | satinBreathing | 100 | 5,880 | 4,056 | 19% | 2,288 |
| 43s | satinBreathing | 100 | 5,776 | 4,216 | 15% | 2,288 |

Final compact diagnostics after the soak reported free heap 4,552 bytes, max block 3,832 bytes, fragmentation 16%, min heap 2,288 bytes, reset reason `External System`, `settingsDirty=false`, and `settingsSaveStatus=saved`.

## Endpoint hardening changes

- `/api/scenes` is streamed instead of built as one large `String`.
- `/api/palettes` is streamed instead of built as one large `String`.
- `/api/backup/export` streams scenes and palettes and omits full diagnostics with `diagnostics.omittedFromBackup=true`.
- `/api/diagnostics` now returns compact heap-safe diagnostics on ESP8266.
- `/api/resources` returns endpoint metrics when safe and `endpointHeapMetrics: []` when contiguous heap is too small.
- Mutation responses stream their wrapper around state so a successful update does not fail because of a second large allocation.

## UI feedback and motion smoothness pass

The earlier UI feedback and motion smoothness pass remains included in the final resource history. It serialized startup loading, queued heavy browser requests, reduced preview polling, and added one 140 LED temporal smoothing buffer. Physical visual smoothness remains not measured by Codex.

## Recommendation

Stay on the D1 mini only for this completed feature set. Do not add major features, more large API payloads, sensors, physical controls, or richer backup restore on ESP8266. Move the next major generation to ESP32-S3.
