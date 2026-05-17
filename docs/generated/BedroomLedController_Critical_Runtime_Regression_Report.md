# BedroomLedController Critical Runtime Regression Report

## Summary

This pass addressed the reported runtime regressions after the UI feedback and temporal smoothing pass. It did not add lighting modes, new feature families, new hardware support, or renderer downgrades.

## Live evidence gathered before fixes

- Controller reached at `http://bedroom-leds.local`, live IP `192.168.1.201`.
- `/api/state` returned malformed JSON before the fix: invalid /api/state JSON was caused by `activePaletteName` emitting an extra quote before the `hex` field.
- `/api/diagnostics` parsed and showed the live state was actually off at the time of measurement: `mode=solid`, `hex=000000`, `masterBrightness=0`, `effectiveBrightness=0`.
- Timer and schedule checks showed no active timer and no schedules saved.
- Live diagnostics before the fix showed tight heap: free heap around 5,688 bytes, max free block around 1,304 bytes, fragmentation around 25%, minimum free heap since boot down to 2,672 bytes.
- During the requested mutation/persistence sequence, the brightness/color/mode requests timed out and the board later showed a fresh short uptime. That is recorded as instability under the old build, not proof that the patched build is stable.

## Settings reset investigation

Cause: the most concrete cause found was invalid `/api/state` JSON. The Web UI could not parse state on refresh, so controls could remain at HTML defaults even when firmware state had not been loaded into the page. This is a UI-only hydration failure until a post-fix live persistence test proves otherwise.

Fix:

- Removed the extra quote in `buildStateJson()` after `activePaletteName`.
- Added `uiHydrating` and `stateLoaded` guards in the Web UI.
- Added `canMutateFromUi(...)` so startup, failed state loads, and programmatic `applyState(...)` assignments do not send mutation endpoints.
- On state failure, the UI preserves existing controls and shows `State unavailable - controls preserved` instead of silently presenting defaults as real state.

Page refresh mutation check: static code now restricts startup to read-only endpoints and blocks UI-origin mutations while hydrating. A browser Network-tab check on the patched firmware has not yet been performed.

Persistence test result: the requested live persistence sequence could not complete on the old live firmware because mutation requests timed out. Post-fix persistence still needs to be verified after USB upload.

## Random off investigation

Cause: not fully proven. The live state was already off when measured. Timer and schedule were not active. Night Guard was disabled and its cap was 80, not zero. The old build had malformed state JSON, very tight heap, mutation timeouts, and a reboot/short uptime after the attempted test sequence, any of which could make the UI look stuck or make recovery unreliable.

Fixes that reduce the known risks:

- Valid `/api/state` JSON so the UI can hydrate correctly.
- Mutation audit fields now record the last route/action and before/after brightness/mode in diagnostics/resources.
- Temporal smoothing resets on mode, brightness, color, temperature, Off, and Warm Dim Now changes so old black frames do not linger after major state changes.
- UI hydration guards prevent accidental Night Guard cap, brightness, mode, timer, or action writes during startup.

Random off status: not proven fixed until the patched firmware is uploaded and observed for several minutes.

## Satin Breathing envelope

Cause: `renderSatinBreathing()` used `sineEase8(...)` for the exhale segment. That helper is a full sine curve over the interval, so the exhale could become non-monotonic and rise again near the end of the cycle.

Fix:

- Added monotonic `easeInOut01(...)`.
- Rewrote Satin Breathing as smooth inhale, short peak hold, and monotonic exhale.
- Preserved fabric texture via `smoothHash8(...)`, warm peak blending, and palette support.

Physical LED visual status: not yet visually retested on the real strip after this patch.

## Smoothing stabilization

Central smoothing was kept but modified.

- Reset smoothing on major lighting state changes.
- Continue bypassing smoothing for Solid/off-style output, Strobe, Flash, diagnostics, transitions, and metadata-marked utility/flashing modes.
- Off and Warm Dim Now explicitly reset smoothing before applying their target state.

## Verification status

- Arduino compile: passed for `esp8266:esp8266:d1_mini`.
- Compile resources: RAM 60,664 / 80,192 bytes (75%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 609,228 / 1,048,576 bytes (58%).
- Arduino sketch mirror compile: passed for `esp8266:esp8266:d1_mini`.
- Python contract tests: `python -m unittest discover -s tests -v` passed, 136 tests.
- `node --check`: passed for the extracted Web UI script.
- USB upload: attempted on `COM5`, but `esptool.py` reported `PermissionError(13, 'Access is denied.')`; patched firmware was not uploaded by Codex.
- OTA upload: not performed.
- Physical visual test: not performed by Codex.

## Known limitations

- Runtime heap remains tight on ESP8266 and must be watched after the patched upload.
- The old live firmware timed out during the API mutation test; post-fix API mutation/persistence behavior still needs a fresh live run.
- Random-off root cause is narrowed away from timer/schedule/Night Guard based on live reads, but not fully proven until mutation audit data is collected after upload.
- Browser refresh Network-tab proof still needs to be run on the patched firmware after USB upload succeeds.
