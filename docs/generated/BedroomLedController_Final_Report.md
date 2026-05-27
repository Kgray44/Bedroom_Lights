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

## Candle Hall Flicker Control + OTA Update

- Added a persistent 0-20 Candle Hall flicker amount setting with default `3`.
- Added the Web UI range control, browser preview scaling, `/api/state` output, `/set` handling, saved settings, and scene capture/load support.
- Preserved Candle Hall's existing palette support, warm flame blending, cluster layout, and gentle default look.
- Added per-candle deterministic brightness, width, drift, pulse timing, micro-flicker, and warmth variation so the Candle Hall clusters behave less uniformly at higher strengths.
- OTA firmware upload was performed successfully with `espota.py` after pinning the host IP to the Wi-Fi interface. The Arduino CLI wrapper authenticated but failed to deliver the payload without the pinned host IP.
- Live state after OTA reported `candleHallFlickerAmount: 20` and `settingsSaveStatus: saved`.

## Quiet Northern Lights Intensity Pass

- Added a persistent shared `animationIntensity` setting with range `0-255` and default `150`.
- Exposed Animation Intensity through `/api/state`, `/api/intensity?value=...`, legacy `/set?animationIntensity=...`, saved settings, scene capture/load JSON, and the main Web UI.
- Reworked `Quiet Northern Lights` from a two-veil sine blend into deterministic soft aurora ribbons with reversible drift, dissolve, Gaussian falloff, internal folds, shimmer, active-color anchoring, and palette sampling.
- Updated the browser preview to read `state.animationIntensity` and approximate the new ribbon model.
- Preserved master brightness, Night Guard, gamma correction, RGB calibration, transitions, existing routes, OTA/browser updater routes, and the central frame buffer/output pipeline.
- Latest local compile after this pass: RAM globals/statics 61,128 / 80,192 bytes (76%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 616,176 / 1,048,576 bytes (58%).
- Focused contract test, full Python contract suite, Web UI JavaScript syntax check, worktree compile, sketch mirror compile, and `git diff --check` passed.
- OTA firmware upload was not performed for this specific pass.
- Physical LED visual behavior for this specific pass has not been independently observed by Codex.

## Quiet Northern Lights Smooth Curtain Pass

- Reworked `Quiet Northern Lights` again after live feedback that the prior upgraded version showed obvious stepping/skipping.
- Replaced isolated Gaussian ribbon/blob behavior with broad overlapping deterministic curtain-wave layers.
- Added cheap `softBand(...)` smoothstep falloff and removed `expf()` from the mode renderer.
- Added fade-in/fade-out life envelopes so curtain cycles reset only after fading down.
- Kept active-color anchoring, palette support, Animation Intensity scaling, and existing temporal smoothing eligibility.
- Updated the browser preview to use the same smooth curtain-wave model.
- Latest local compile after this pass: RAM globals/statics 61,128 / 80,192 bytes (76%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 617,872 / 1,048,576 bytes (58%).
- Focused smooth curtain contract, full Python contract suite, Web UI JavaScript syntax check, worktree compile, sketch mirror compile, and `git diff --check` passed.
- OTA upload for this pass passed with direct `espota.py` to `192.168.1.201`; live state was then saved with `mode=quietNorthernLights`, `masterBrightness=255`, and `animationIntensity=255`.
- Live resource snapshot after this pass: `/api/resources` free heap 6,056 bytes, max block 4,344 bytes, fragmentation 17%, min heap since boot 3,200 bytes; `/api/diagnostics` free heap 4,816 bytes, max block 3,960 bytes, fragmentation 18%.
- Physical LED visual behavior for this specific pass has not been independently observed by Codex.

## Quiet Northern Lights True Aurora Wavefield Pass

- Reworked `Quiet Northern Lights` after live feedback that the smooth curtain version still read as one moving blob.
- Removed the local center/distance/falloff model from the mode renderer.
- Replaced it with full-strip aurora wavefield layers using phase, spatial frequency, soft ridge extraction, fold texture, and fine-curtain modulation.
- Added `easeSlowAccelFastDecel(...)` motion shaping so layers gather slowly and settle faster around reversals without snapping.
- Kept fade-in/fade-out life envelopes so layer phase/shape changes occur while faded down.
- Preserved active-color anchoring, palette support, Animation Intensity, temporal smoothing eligibility, and central frame-buffer output.
- Updated the browser preview to use the same wavefield model instead of local blob windows.
- Latest local compile after this pass: RAM globals/statics 61,128 / 80,192 bytes (76%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 620,832 / 1,048,576 bytes (59%).
- Focused true wavefield contract, full Python contract suite, Web UI JavaScript syntax check with bundled Node.js, worktree compile, and sketch mirror compile passed.
- OTA upload for this pass was attempted with direct `espota.py`, throttled `espota.py`, and the browser updater route, but did not complete. The live device still reports the previous sketch size `677936`, so this true wavefield build is not installed on the board yet.
- After the failed OTA attempts, the live controller was restored to `mode=quietNorthernLights`, `masterBrightness=255`, `animationIntensity=255`, and `settingsSaveStatus=saved`.

## Final measured status

- Compile target: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 61,128 / 80,192 bytes (76%)
- IRAM: 61,383 / 65,536 bytes (93%)
- Flash/IROM: 620,832 / 1,048,576 bytes (59%)
- USB upload: passed on `COM5` in an earlier pass; `COM5` was not available during the true wavefield pass
- Latest true wavefield OTA firmware upload: failed mid-transfer; live board remains on the previous `677936` byte sketch
- Live IP: `192.168.1.201`
- LittleFS: 57,344 / 2,072,576 bytes used
- Live sketch size after smooth curtain OTA: 677,936 bytes
- Live free sketch space after smooth curtain OTA: 1,417,216 bytes
- True wavefield OTA binary size: 680,896 bytes
- Lowest measured heap during final Satin Breathing soak: 2,288 bytes since boot
- Candle Hall OTA pass live heap snapshot: diagnostics free heap 5,664 bytes, max block 5,040 bytes, fragmentation 11%, min heap since boot 4,616 bytes
- Smooth curtain OTA pass live heap snapshot: diagnostics free heap 4,816 bytes, max block 3,960 bytes, fragmentation 18%, min heap since boot 3,200 bytes
- OTA page: `/ota` HTTP 200
- Browser updater: `/update` HTTP 401 without auth, HTTP 200 with local OTA credentials
- Physical LED visual behavior: not independently observed by Codex
- Long-duration physical visual behavior: not measured by Codex

## Tests

- Focused critical regression contract tests passed.
- Focused Candle Hall contract test passed.
- Focused true wavefield contract test passed.
- Full Python contract suite passed: `python -m unittest discover -s tests -v`, 144 tests.
- Web UI JavaScript syntax passed with bundled Node.js on the extracted script.

## Known limits

- ESP8266 heap remains tight with 140 LEDs and this feature set.
- `/api/diagnostics` is compact by design on ESP8266; use `/api/resources` for live resource details.
- `/api/resources` may return `endpointHeapMetrics: []` when contiguous heap is too small.
- Full backup import still validates schema only and does not apply a bulk restore.
- Browser `/update` upload was not successful in the true wavefield pass; direct `espota.py` also authenticated but dropped mid-transfer.

## Recommendation

Stay on D1 mini for the current build only. Move future feature expansion to ESP32-S3.
