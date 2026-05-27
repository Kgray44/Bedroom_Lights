# BedroomLedController Resource And Migration Report

## Final resource report

This report reflects the final regression-stabilized D1 mini build with 140 LEDs configured, live hardware checks performed on `2026-05-17`, the later Candle Hall and smooth-curtain OTA update passes, and the true aurora wavefield compile/OTA-attempt pass.

## Compile resources

- Board target: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 61,128 / 80,192 bytes (76%)
- IRAM: 61,383 / 65,536 bytes (93%)
- Flash/IROM: 620,832 / 1,048,576 bytes (59%)
- Last OTA binary uploaded after the smooth curtain pass: 677,936 bytes
- True aurora wavefield OTA binary built but not installed: 680,896 bytes
- Live `sketchSizeBytes`: 677,936
- Live `freeSketchSpaceBytes`: 1,417,216

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
- Earlier OTA firmware upload: passed with direct `espota.py` to `192.168.1.201` after pinning host IP `192.168.1.43`
- True aurora wavefield OTA upload: failed mid-transfer with direct `espota.py`, throttled `espota.py`, and the browser updater route; live sketch size stayed at 677,936 bytes
- USB fallback for true wavefield pass: not performed because no serial port was detected and `COM5` was unavailable
- Arduino CLI network upload wrapper: authenticated but failed with `No response from device` until the direct `espota.py` host-IP path was used
- Physical LED visual output: not independently observed by Codex

## LittleFS

- Total: 2,072,576 bytes
- Used: 57,344 bytes
- Free: 2,015,232 bytes

## Candle Hall OTA pass live snapshot

After OTA and setting Candle Hall flicker amount to `20`:

| Endpoint | Result | Free heap | Max block | Fragmentation | Min heap since boot |
| --- | --- | ---: | ---: | ---: | ---: |
| `/api/state` | valid JSON with `candleHallFlickerAmount: 20` | not reported | not reported | not reported | not reported |
| `/api/diagnostics` | valid compact JSON | 5,664 | 5,040 | 11% | 4,616 |
| `/api/resources` | valid JSON | 6,888 | 5,424 | 11% | 4,616 |

The final live state reported `mode=candleHall`, `candleHallFlickerAmount=20`, and `settingsSaveStatus=saved`.

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

## Quiet Northern Lights intensity pass

This pass adds one persistent byte setting, `animationIntensity`, plus one byte in scene presets. `Quiet Northern Lights` now uses the setting to choose ribbon count, contrast, fold depth, shimmer, dissolve behavior, and color spread. The renderer still writes to the central frame buffer and does not add another LED output buffer.

Latest local compile after this pass: RAM globals/statics 61,128 / 80,192 bytes (76%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 616,176 / 1,048,576 bytes (58%). Runtime heap, OTA firmware upload, and physical LED visual behavior for this specific pass have not been measured by Codex.

## Quiet Northern Lights smooth curtain pass

The follow-up smooth curtain pass replaces the prior Gaussian ribbon/blob math with a cheaper deterministic curtain-wave model. It adds `softBand(...)` smoothstep falloff, fade-in/fade-out life envelopes, slow reversible drift, and lower-edge/fold waves. The mode still writes into the existing frame buffer and keeps temporal smoothing eligibility.

Latest local compile after this pass: RAM globals/statics 61,128 / 80,192 bytes (76%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 617,872 / 1,048,576 bytes (58%).

After OTA and setting `Quiet Northern Lights` to max brightness and max Animation Intensity:

| Endpoint | Result | Free heap | Max block | Fragmentation | Min heap since boot |
| --- | --- | ---: | ---: | ---: | ---: |
| `/api/state` | valid JSON with `mode=quietNorthernLights`, `masterBrightness=255`, `animationIntensity=255`, `settingsSaveStatus=saved` | not reported | not reported | not reported | not reported |
| `/api/resources` | valid JSON | 6,056 | 4,344 | 17% | 3,200 |
| `/api/diagnostics` | valid compact JSON | 4,816 | 3,960 | 18% | 3,200 |

Runtime frame timing and physical LED visual behavior for this specific pass have not been independently measured by Codex.

## Quiet Northern Lights true aurora wavefield pass

The true aurora wavefield pass removes the local center/distance/falloff renderer structure from `Quiet Northern Lights`. The replacement uses full-strip phase/ridge/fold layers, deterministic life envelopes, and precomputed layer values before the LED loop. It does not add another LED output buffer and still writes into the existing frame buffer.

Latest local compile after this pass: RAM globals/statics 61,128 / 80,192 bytes (76%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 620,832 / 1,048,576 bytes (59%).

After failed OTA attempts and restoring the live controller to max Quiet Northern Lights state:

| Endpoint | Result | Free heap | Max block | Fragmentation | Min heap since boot |
| --- | --- | ---: | ---: | ---: | ---: |
| `/api/state` | valid JSON with `mode=quietNorthernLights`, `masterBrightness=255`, `animationIntensity=255`, `settingsSaveStatus=saved` | not reported | not reported | not reported | not reported |
| `/api/resources` | valid JSON; live `sketchSizeBytes=677936` | 5,040 | 3,584 | 15% | 2,488 |

Runtime frame timing and physical visual status are not measured for the true wavefield build because it is not installed on the live board yet.

## Recommendation

Stay on the D1 mini only for this completed feature set. Do not add major features, more large API payloads, sensors, physical controls, or richer backup restore on ESP8266. Move the next major generation to ESP32-S3.
