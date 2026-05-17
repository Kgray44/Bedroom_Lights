# BedroomLedController Critical Runtime Regression Report

## Summary

This pass stabilized the runtime regressions reported after the UI feedback and temporal smoothing work. It did not add lighting modes, new feature families, new hardware support, or renderer downgrades.

## Root Causes Found

- The previous build had invalid /api/state JSON because `activePaletteName` wrote an extra quote before the `hex` field.
- Browser refresh could appear to reset controls because the UI could not hydrate real state after that JSON parse failure. This was the UI-only hydration failure path.
- Startup UI code needed explicit hydration guards so programmatic input assignments could not send mutation endpoints.
- Satin Breathing used a non-monotonic exhale envelope, which could create sudden brightness jumps near cycle wrap.
- Several large ESP8266 JSON endpoints could silently collapse under fragmented heap: `/api/resources` endpoint metrics, `/api/scenes`, `/api/palettes`, `/api/diagnostics`, and `/api/backup/export`.

## Fixes

- Fixed `buildStateJson()` so `/api/state` parses cleanly.
- Added `uiHydrating`, `stateLoaded`, and `canMutateFromUi(...)` guards in the Web UI.
- Added fixed-buffer last-mutation audit fields for route, action, before/after brightness, and before/after mode.
- Added `easeInOut01(...)` and rewrote Satin Breathing as monotonic inhale, soft hold, and monotonic exhale while preserving texture, warm blending, and palette support.
- Reset temporal smoothing on major state changes, Off, and Warm Dim Now.
- Streamed `/api/scenes` and `/api/palettes`.
- Streamed backup export scene/palette sections and omitted full diagnostics from backup export with an explicit `diagnostics.omittedFromBackup=true` marker.
- Made mutation responses stream their wrapper around state instead of allocating a larger nested JSON string.
- Made `/api/diagnostics` a compact heap-safe response on ESP8266.
- Made `/api/resources` omit endpoint metrics as `[]` when contiguous heap is too small.

## Live Verification

Final USB upload to `COM5` succeeded. The board booted at `192.168.1.201`.

- Persistence test: set brightness `77`, color `00AAFF`, mode `solid`; after debounce `/api/state` reported `masterBrightness=77`, `hex=00AAFF`, `mode=solid`, `settingsSaveStatus=saved`, `settingsLoadStatus=loaded`.
- Browser refresh mutation check: Playwright loaded and reloaded `/`; 14 board requests were observed, all read-only, with `mutationCount=0`. State remained brightness `77`, color `00AAFF`, mode `solid`.
- Heavy endpoints: `/api/state`, `/api/resources`, `/api/diagnostics`, `/api/scenes`, `/api/palettes`, and `/api/backup/export` all returned valid JSON on the final flashed build.
- Backup export: returned 16 scenes, 10 palettes, and `diagnostics.omittedFromBackup=true`.
- OTA capability: `/ota` returned HTTP 200; `/update` returned HTTP 401 without auth and HTTP 200 with local OTA credentials. Arduino CLI detected the network port at `192.168.1.201`. No OTA firmware upload was performed.
- Satin Breathing API soak: ran `satinBreathing` at brightness `100` for 43 seconds. API state stayed on `satinBreathing`, timer stayed inactive, schedule count stayed 0, and resources/diagnostics stayed valid.

## Satin Breathing envelope

The Satin Breathing envelope now uses a monotonic inhale/hold/exhale curve. The fabric texture, warm peak blending, and palette support were preserved.

## Random Off Status

Timer, schedule, and Night Guard did not appear to be the cause in live API checks. The old build showed invalid JSON and heap pressure; the final build keeps state stable through refresh and a short Satin Breathing API soak. Codex cannot visually confirm whether the LEDs physically turned off during the soak.

Random off status: improved and API-soak stable, but not physically proven by Codex.

Physical LED visual status: not independently observed by Codex.

## Verification Status

- Arduino compile: passed for `esp8266:esp8266:d1_mini`.
- Compile resources: RAM 60,844 / 80,192 bytes (75%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 606,044 / 1,048,576 bytes (57%).
- USB upload: passed on `COM5`.
- Python contract tests: `python -m unittest discover -s tests -v` passed, 138 tests.
- `node --check` on extracted Web UI script: passed.
- Physical LED visual test: not independently observed by Codex.
- OTA upload: not performed.

## Known Limits

- Runtime heap is still tight on D1 mini, especially with 140 LEDs and large web/API responses.
- `/api/diagnostics` is intentionally compact on ESP8266; use `/api/resources` for endpoint/resource metrics.
- Endpoint metrics may be returned as `[]` when contiguous heap is too small.
- OTA browser updater reachability was tested, but OTA firmware upload was not performed.
