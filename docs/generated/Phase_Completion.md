# Phase_Completion

This document tracks completed phases and subphases for the Bedroom LED Controller. Add to it whenever a phase or subphase is completed.

## Phase 1A - Settings and Master Brightness

Main additions:

- Organized the project into `Bedroom-Lights` with separate firmware, generated docs, user phase docs, source reference docs, and tests.
- Split the Arduino program into a main `.ino` plus focused `.h` files.
- Added a central `ControllerSettings settings` model.
- Added LittleFS persistence at `/settings.json`.
- Added debounced settings saves with `SETTINGS_SAVE_DEBOUNCE_MS`.
- Added master brightness state, UI, `/state` output, and `/set` support.
- Added boot behavior scaffolding for `restore`, `startOff`, and `warmDim`.
- Mirrored code-only sketch files into `C:\Users\kkids\Documents\Arduino\BedroomLedController`.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_1A_Report.md`

## Phase 1B - Render Output Pipeline

Main additions:

- Added the `RgbPixel frameBuffer[LED_COUNT]` frame buffer.
- Added frame helper functions for clearing, filling, and setting pixels.
- Routed existing modes through the central frame buffer.
- Centralized final output in `outputFrameToStrip()`.
- Moved master brightness into the final output pipeline.
- Added gamma correction with a PROGMEM lookup table.
- Added RGB calibration gains and UI controls.
- Persisted gamma and RGB gain settings.
- Removed normal-mode direct `strip.show()` / direct pixel-output behavior.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_1B_Report.md`

## Phase 1C - Diagnostics, Power, and LED Tests

Main additions:

- Added `/diagnostics` browser page.
- Added `/api/diagnostics` runtime diagnostics JSON.
- Added `/api/power` estimate-only power JSON.
- Added power estimator fields: `supplyMillivolts`, `supplyMilliamps`, `supplySafetyMarginPercent`, and `controllerOverheadMilliamps`.
- Added warnings for power cap, D3/GPIO0 boot risk, weak Wi-Fi, low heap, settings storage issues, and full-white current risk.
- Added safe LED test tools through `/api/test`.
- Added non-blocking pixel walk.
- Added return-to-previous-state behavior for diagnostic tests.
- Kept diagnostics and tests routed through the Phase 1B output pipeline.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_1C_Report.md`

## Phase 1D - Local API Cleanup and Final Hardening

Main additions:

- Completed local API cleanup and final hardening for the Phase 1 foundation.
- Added future-facing local API routes: `/api/state`, `/api/brightness`, `/api/color`, `/api/temperature`, and `/api/mode`.
- Preserved legacy `/state` and `/set` behavior for the existing UI.
- Added consistent JSON success and error helpers with `ok` fields.
- Added validation helpers for integer arguments, booleans, hex colors, and known modes.
- Added shared mutation helpers for master brightness, active color, white temperature, mode, gamma, RGB gains, power config, and boot behavior.
- Hardened invalid local API inputs so malformed requests return JSON errors instead of becoming persistent settings.
- Reviewed settings schema versioning and kept version `3` for the full Phase 1 schema.
- Confirmed OTA and browser updater routes remain wired.
- Added final Phase 1 documentation and this rolling completion document.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_1_Final_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest discover -s tests -v`: passed, 28 tests.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 2A - Scene Presets

Main additions:

- Added persistent scene presets with a bounded `ScenePreset scenes[MAX_SCENES]` store.
- Added separate scene storage at `/scenes.json`.
- Added built-in scene presets: Warm Dim, Reading, Sleep, Movie, Focus, Rain Glass, Moon Tide, and Lantern Drift.
- Added save current state as a scene from the main UI and `/api/scenes/save`.
- Added scene recall from the main UI and `/api/scenes/load`.
- Added scene rename support from the main UI and `/api/scenes/rename`.
- Added scene delete support from the main UI and `/api/scenes/delete`.
- Added `/api/scenes` list output with count and max count.
- Kept power settings, diagnostics, gamma/RGB calibration, and runtime status separate from saved scenes.
- Added scene storage status to diagnostics.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_2A_Report.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase2a_contract -v`: passed, 8 tests.
- `python -m unittest discover -s tests -v`: passed, 36 tests.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 2B - Favorite Bar, Warm Dim Now, and Night Guard

Main additions:

- Added a compact favorite bar for one-tap common actions and scene presets.
- Added persistent favorite configuration at `/favorites.json`.
- Added bounded `FavoriteItem favorites[MAX_FAVORITES]` storage with default favorites.
- Added favorite API endpoints: `/api/favorites`, `/api/favorites/load`, and `/api/favorites/reset`.
- Added a central action path for `warmDimNow`, `panicWarm`, and `off`.
- Added Night Guard API endpoints: `/api/nightguard` and `/api/nightguard/set`.
- Added the Warm Dim Now comfort action as a direct fallback that does not depend on scene storage.
- Added an Off action that keeps the controller web UI, OTA, and update routes active.
- Added persistent Night Guard settings for enablement, max brightness, flashing-mode blocking, and warm preference.
- Added effective brightness output capping through Night Guard without destroying saved `masterBrightness`.
- Added Night Guard blocking for Strobe and Flash through mode changes, API mode changes, scene recall, and favorite loading.
- Added state, diagnostics, and power JSON fields for Night Guard, favorite count, and effective brightness.
- Added main UI controls for the favorite bar, Warm Dim Now, favorite reset, and Night Guard.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_2B_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase2b_contract -v`: passed, 8 tests.
- `python -m unittest discover -s tests -v`: passed, 44 tests.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 2C - Timer Actions and Bedtime Fade

Main additions:

- Added a runtime-only one-shot timer state model for one active timer or fade at a time.
- Added non-blocking timer service through `serviceActiveTimer()` in the main loop.
- Added timer actions for Off, Warm Dim Now, Panic Warm, and scene loading.
- Added a simple fade engine for fade-to-Off, fade-to-Warm-Dim, and fade-to-scene behavior.
- Added timer API endpoints: `/api/timer`, `/api/timer/start`, and `/api/timer/cancel`.
- Added bedtime fade API endpoint `/api/bedtime/start` and cancel alias `/api/bedtime/cancel`.
- Added Sleep Timer and Bedtime Fade controls to the main web UI.
- Added persistent bedtime fade defaults while keeping active timers runtime-only.
- Added timer and bedtime fade state to `/state`, `/api/state`, and diagnostics.
- Added Night Guard and diagnostic-test interaction checks for timer starts.
- Documented that active timers do not persist across reboot.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_2C_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase2c_contract -v`: passed, 9 tests.
- `python -m unittest discover -s tests -v`: passed, 53 tests.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 2D - Mode Tags, Practical Scenes, and Usability Polish

Main additions:

- Added compact mode metadata for every actual firmware mode.
- Added `/api/modes` for mode metadata, categories, tags, and best-use guidance.
- Added metadata-backed Night Guard blocking through `modeBlockedByNightGuard()` and `modeIsFlashing()`.
- Added practical scenes: Study / Focus, Workbench Mode, Sera Call, and Cozy Goblin Mode.
- Refined the Movie default scene to a calmer Moon Tide profile.
- Added scene metadata fields for category, favorite candidacy, sleep-safety, and focus-friendliness.
- Updated favorite defaults around Off, Warm Dim Now, Cozy Goblin, Sleep, Study / Focus, Movie, Rain Glass, and Workbench.
- Added a mode filter UI with All, Everyday, Calm, Sleep-safe, Focus, Colorful, Flashing, Utility, and Weather filters.
- Added current state summary UI for mode, last loaded scene, brightness, timer, and Night Guard.
- Added runtime last-loaded-scene tracking with `lastLoadedSceneId`, `lastLoadedSceneName`, and scene-match state.
- Added current mode metadata and last loaded scene fields to state and diagnostics JSON.
- Kept smooth transitions, palette editing, weekly scheduling, live preview, physical controls, and sensor features deferred.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_2D_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase2d_contract -v`: passed, 9 tests.
- `python -m unittest discover -s tests -v`: passed, 62 tests.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 3A - Smooth Mode and Scene Transitions

Main additions:

- Added persistent transition settings: `transitionsEnabled`, `transitionDurationMs`, and `transitionStyle`.
- Added runtime-only `TransitionState` for active transition status and progress.
- Added static transition buffers for previous frame and target frame blending.
- Implemented the Fade transition style.
- Integrated transitions into the existing raw frame buffer and final output pipeline.
- Added interpolated effective brightness during transitions so Off and Warm Dim can fade gracefully.
- Added transition trigger points for mode changes, scene loads, favorites, Warm Dim Now, Panic Warm, Off, and delay-timer completion actions.
- Kept timer fades and bedtime fades from double-stacking with the new transition engine.
- Added diagnostic-test interaction: starting tests cancels active transitions, while normal lighting changes stop tests and transition away.
- Added `/api/transitions` and `/api/transitions/set`.
- Added transition summary fields to state and diagnostics JSON.
- Added main UI controls for transition enablement, duration, style, and status.
- Kept Wipe, Dissolve, palette builder, live preview, weekly scheduling, physical controls, and sensors deferred.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_3A_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase3a_contract -v`: passed, 9 tests.
- `python -m unittest discover -s tests -v`: passed, 71 tests.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 3B - Palette Builder and Palette-Aware Modes Foundation

Main additions:

- Added persistent palette models with `PaletteColor` and `ColorPalette`.
- Added bounded palette storage with `MAX_PALETTES = 16`, `MAX_PALETTE_COLORS = 5`, and separate `/palettes.json` persistence.
- Added built-in palettes: Candle, Ocean, Garden, Moonlight, Peach Sky, Soft Neon, Aurora, Warm Linen, Storm Blue, and Circuit Glow.
- Added persistent active palette settings: `paletteEnabled` and `activePaletteId`.
- Added `PaletteControls.h` with palette load/save, built-in seeding, custom create/update/delete/reset, color parsing, swatch formatting, and palette sampling helpers.
- Added palette API endpoints: `/api/palettes`, `/api/palettes/select`, `/api/palettes/save`, `/api/palettes/update`, `/api/palettes/delete`, and `/api/palettes/reset`.
- Added palette enablement, selector, swatches, support note, custom palette editor, delete controls, and reset action to the main UI.
- Extended mode metadata with palette support flags and preferred palette roles.
- Added palette support to Velvet Aurora while preserving the original active-color behavior when palettes are disabled.
- Extended scene presets so new scenes save `paletteEnabled` and `paletteId`.
- Added scene-load fallback behavior for missing palette references.
- Added compact palette fields to state JSON and palette load/save/count/support details to diagnostics.
- Preserved favorites, timers, transitions, Night Guard brightness caps, OTA, diagnostics, LED tests, and existing API routes.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_3B_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase3b_contract -v`: passed, 10 tests.
- `python -m unittest discover -s tests -v`: passed, 81 tests.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 3C - Browser Preview and Surprise Me

Main additions:

- Added a browser-side Live Preview canvas to the main controller UI.
- Added `PREVIEW_PIXELS = 48` with local `requestAnimationFrame` rendering.
- Kept preview rendering in browser JavaScript using compact state, modes, and palettes APIs instead of streamed LED frames.
- Added preview helpers for RGB parsing, CSS conversion, brightness scaling, blending, palette sampling, and Kelvin approximation.
- Added dedicated preview approximations for Solid, Strobe, Flash, Chase, Wave, Slow Wave, Slow Pulse, Rainbow, Dawn Bloom, Moon Tide, Velvet Aurora, Lantern Drift, and Rain Glass.
- Added reduced-motion throttling, hidden-tab handling, and a pause toggle for preview rendering.
- Added the `SurpriseCandidate` model and `SurpriseControls.h`.
- Added safe Surprise Me API endpoints: `/api/surprise` and `/api/surprise/options`.
- Added moods for Calm, Evening, Sleep-safe, Focus, Colorful, and Cozy.
- Added Surprise Me safety checks that exclude Strobe, Flash, flashing metadata, and Night Guard blocked modes.
- Added brightness capping against Night Guard and the configured power estimate.
- Added Surprise Me apply behavior that cancels diagnostic tests and active timers/fades before applying through the central mutation and transition paths.
- Added Surprise Me UI controls and a save-current-surprise flow through `/api/scenes/save`.
- Added Surprise Me summary fields to state and diagnostics.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_3C_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- resource report: RAM 47,228 / 80,192 bytes (58%); IRAM 61,047 / 65,536 bytes (93%); flash 496,420 / 1,048,576 bytes (47%).
- Runtime free heap and LittleFS usage after browser interactions: not measured.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 3D - First New Lighting Mode Batch

Main additions:

- Added seven new lighting modes: Satin Breathing, Paper Lantern Row, Library Lamp, Solar Noon, Golden Hour, Dream Aquarium, and Circuit Glow.
- Registered the new modes in the central `Mode` enum, mode parser, and `modeKey()` output.
- Added full mode metadata for the new modes with categories, descriptions, best-use text, tags, Night Guard flags, and palette support.
- Added render functions for all seven modes through the existing frame buffer and final output pipeline.
- Added shared render helpers for clamping, RGB blending/scaling, palette/color sampling, Kelvin approximation, phase math, waves, and deterministic hash texture.
- Added dedicated browser-side Live Preview approximations for every new mode.
- Added the new mode IDs to the main UI fallback mode list.
- Added Surprise Me mood integration for the new modes while preserving no-strobe/no-flash safety rules.
- Added three optional built-in scenes within the existing 16-scene cap: Satin Breathing, Library Lamp, and Solar Noon.
- Preserved generic scene save/load compatibility for all seven modes.
- Kept Night Guard behavior brightness-cap-only for the new modes; none are tagged as flashing or blocked.
- Left deeper per-mode controls deferred so the main UI remains organized.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_3D_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase3d_contract -v`: passed, 6 tests.
- `python -m unittest discover -s tests -v`: passed, 95 tests.
- resource report: RAM 48,752 / 80,192 bytes (60%); IRAM 61,047 / 65,536 bytes (93%); flash 506,452 / 1,048,576 bytes (48%).
- Runtime free heap, LittleFS usage, OTA behavior, web responsiveness, and animation responsiveness: not measured.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 4A - Advanced Scheduler and Scene Links

Main additions:

- Added a local weekly scheduler with a bounded `MAX_SCHEDULE_ENTRIES = 8` model.
- Added persistent schedule storage in `/schedule.json`, separate from settings, scenes, favorites, and palettes.
- Added fixed local UTC offset support in settings, defaulting to `-240` minutes for the May 2026 New York project context.
- Added NTP-backed time sync state and loop services so schedules only run after the controller has a valid clock.
- Added `/api/time`, `/api/time/sync`, and `/api/time/set`.
- Added `/api/schedule`, `/api/schedule/save`, `/api/schedule/delete`, `/api/schedule/toggle`, `/api/schedule/skip`, and `/api/schedule/run`.
- Added Skip next support that clears after exactly one matching occurrence.
- Added schedule execution through existing scene/action links and logic: scenes use `loadSceneById`, actions use `runActionByName`.
- Added schedule conflict behavior that stops diagnostic tests and overrides active timers/fades.
- Preserved Night Guard behavior for scheduled scenes and output brightness caps.
- Added a dedicated `/schedule` editor page with clock sync status, day selection, scene/action target selection, fade seconds, and run/toggle/delete/skip actions.
- Added main-page schedule summary with Next schedule and a schedule-page link.
- Added diagnostics fields for time sync, current local time, schedule count, next schedule, last schedule status, and schedule storage status.
- Added stable `/go` scene/action links for QR/NFC-friendly local use, including `/go?scene=...`, `/go?action=off`, and `/go?action=warmDimNow`.
- Added scene/action links copy buttons on the main UI and schedule page.
- Documented that QR code generation was deferred and NFC hardware/writing was not added.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_4A_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase4a_contract -v`: passed, 6 tests.
- `python -m unittest discover -s tests -v`: passed, 101 tests.
- resource report: RAM 51,560 / 80,192 bytes (64%); IRAM 61,383 / 65,536 bytes (93%); flash 538,056 / 1,048,576 bytes (51%).
- Runtime free heap, Free heap after /api/schedule, LittleFS space after schedule save, and physical LED behavior: not measured.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Phase 4B - Remaining Mode Library Batch 1

Main additions:

- Added eight new lighting modes: Window Sunrise, Ember Quilt, Porcelain Sky, Hearthline, Evening Current, Starlit Linen, Deep Sea Glass, and Quiet Northern Lights.
- Registered the new modes in the central `Mode` enum, mode parser, and `modeKey()` output.
- Added full metadata rows for the new modes with display names, descriptions, best-use text, categories, safe flags, and palette-role strings.
- Added render functions for all eight modes through the existing frame buffer and output pipeline.
- Added shared helpers for cool-white approximation, smoothstep easing, sine/triangle waves, and named fallback palette sampling.
- Added `findPaletteByIdConst(const char*)` so named fallback palette lookup does not construct `String` values inside per-pixel rendering.
- Added browser-side Live Preview approximations for all eight new modes.
- Added the new mode IDs to the fallback mode picker.
- Added Surprise Me mood-pool support for Calm, Evening, Sleep-safe, Focus, Colorful, and Cozy.
- Preserved scene save/load, favorites, panic, warm dim, timers, bedtime fade, scheduler, `/go` links, and Night Guard behavior through the existing APIs.
- Deferred per-mode controls and additional built-in scenes.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_4B_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase4b_contract -v`: passed, 7 tests.
- `python -m unittest discover -s tests -v`: passed, 108 tests.
- Arduino sketch mirror hash check: passed for 19 code files.
- resource report: RAM 53,388 / 80,192 bytes (66%); IRAM 61,383 / 65,536 bytes (93%); flash 551,112 / 1,048,576 bytes (52%).
- Runtime free heap, Free heap after /api/schedule, LittleFS usage, OTA behavior, web responsiveness, animation responsiveness, and physical LED behavior: not measured.
- The firmware compiled successfully, but was not physically tested on LEDs.

## Release Hardening Pass - Finalization, Resources, And Release Readiness

Main additions:

- Removed tracked Wi-Fi and OTA credentials from firmware configuration.
- Added `Secrets.example.h` and ignored local `Secrets.h`.
- Added root `README.md` with first-time setup, hardware safety notes, docs index, and test/build commands.
- Added runtime resource diagnostics for free heap, largest free block, heap fragmentation, minimum free heap since boot, sketch size, free sketch space, and LittleFS capacity.
- Added endpoint heap tracking for `/`, `/api/state`, `/api/modes`, `/api/scenes`, `/api/palettes`, `/api/schedule`, `/api/diagnostics`, and `/api/backup/export`.
- Streamed `/api/modes` and chunked `/api/backup/export` to reduce large contiguous JSON allocation pressure.
- Migrated selected long-lived fields to fixed char buffers for favorites and time sync status.
- Preserved the central output pipeline and did not add modes or downgrade renderer math.

Generated documentation:

- `README.md`
- `docs/generated/BedroomLedController_Release_Hardening_Report.md`
- `docs/generated/BedroomLedController_Final_Report.md`
- `docs/generated/BedroomLedController_User_Guide.md`
- `docs/generated/BedroomLedController_API_Reference.md`
- `docs/generated/BedroomLedController_Troubleshooting.md`
- `docs/generated/BedroomLedController_Resource_And_Migration_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest discover -s tests -v`: passed, 124 tests.
- resource report: RAM 58,948 / 80,192 bytes (73%); IRAM 61,383 / 65,536 bytes (93%); flash 587,436 / 1,048,576 bytes (56%).
- Runtime free heap after boot, endpoint route heap readings, LittleFS live usage, OTA behavior, web responsiveness, animation responsiveness, and physical LED behavior: not measured.
- Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Local Hardware Measurement Pass - 140 LED D1 Mini

Main additions:

- Updated configured LED count to 140 after the strip length was observed.
- Set the local hardware-test brightness default to 100.
- Added compact `/api/resources` endpoint for live heap/resource measurement.
- Kept detailed endpoint metrics out of the full diagnostics payload to preserve valid JSON under low heap.
- Uploaded the 140-LED build over USB to the connected D1 mini on `COM5`.
- Confirmed boot, Wi-Fi connection, hostname access, diagnostics JSON parsing, resource metrics, control endpoints, safe LED diagnostic endpoints, `/ota`, and authenticated `/update` reachability.

Generated documentation:

- `docs/generated/live_hardware_measurements_2026-05-16.json`
- `docs/generated/BedroomLedController_Resource_And_Migration_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- USB upload to D1 mini over `COM5`: passed.
- resource report: RAM 59,896 / 80,192 bytes (74%); IRAM 61,383 / 65,536 bytes (93%); flash 588,188 / 1,048,576 bytes (56%).
- Live LittleFS: 2,072,576 total / 49,152 used / 2,023,424 free bytes.
- Live free heap after boot/resource check: 7,768 bytes.
- Lowest measured route-local heap floor: 2,000 bytes during `/api/scenes`.
- `/ota`: HTTP 200.
- `/update`: HTTP 200 with credentials; no OTA upload was performed.
- Safe LED diagnostics called red, green, blue, whiteLow, and restore; physical visual LED output was not independently observed by Codex.

## Phase 4D - Final Polish, Advanced Scene Management, And Docs

Main additions:

- Added advanced scene management for duplicate, export, import, built-in restore, and warning reporting.
- Added scene warnings for missing mode, missing palette, Night Guard blocking, and powerBrightness recommendations.
- Added palette export/import and built-in palette restore.
- Added schedule export/import/reset.
- Added guarded full backup export and backup schema validation for import.
- Added selective resets for scene built-ins, palette built-ins, favorites, schedule, settings, and factory reset.
- Added `/api-docs` as the local API reference/help page.
- Added UI controls for scene JSON import/export, duplicate, built-in restore, backup/maintenance, full backup, and factory reset.
- Added diagnostics hardening for project version, board target, LittleFS capacity, sketch size, free sketch space, and OTA/update route presence.
- Added final user, API, troubleshooting, resource, migration, and final project documentation.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_4D_Report.md`
- `docs/generated/BedroomLedController_Final_Report.md`
- `docs/generated/BedroomLedController_User_Guide.md`
- `docs/generated/BedroomLedController_API_Reference.md`
- `docs/generated/BedroomLedController_Troubleshooting.md`
- `docs/generated/BedroomLedController_Resource_And_Migration_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase4d_contract -v`: passed, 5 tests.
- `python -m unittest discover -s tests -v`: passed, 120 tests.
- resource report: RAM 57,800 / 80,192 bytes (72%); IRAM 61,383 / 65,536 bytes (93%); flash 583,964 / 1,048,576 bytes (55%).
- Runtime free heap, LittleFS usage, OTA behavior, web responsiveness, animation responsiveness, and physical LED behavior: not measured.
- Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## UI Feedback + Motion Smoothness

Main additions:

- Added a visible global status banner for connection, save, success, retry, and error messages.
- Added a central browser API helper with no-store fetches, timeout handling, retries, invalid JSON diagnostics, and structured console warnings.
- Serialized initial UI API loading to reduce startup pressure on the ESP8266.
- Added a browser-side queue for heavy endpoints such as modes, scenes, palettes, diagnostics/resources, and backup export.
- Changed preview behavior to use cached state and reduced background state refresh pressure.
- Split empty-list messages from true unavailable/error states for favorites, scenes, and palettes.
- Added retry buttons for failed favorites, scenes, palettes, and preview sections.
- Hardened Surprise Me so it cannot remain stuck on the choosing message.
- Hardened Night Guard UI updates so enabled/off/capped notes reflect successful state changes.
- Added one central temporal smoothing buffer and bounded per-channel smoothing for ambient animated modes.
- Added interpolated hash texture for selected renderer texture terms that could step between time buckets.
- Preserved diagnostic/utility immediacy and bypassed smoothing for Solid, Strobe, Flash, diagnostics, active transitions, and utility/flashing modes.

Generated documentation:

- `docs/generated/BedroomLedController_Live_UI_Feedback_Report.md`
- `docs/generated/BedroomLedController_Motion_Smoothness_Report.md`
- `docs/generated/BedroomLedController_Final_Report.md`
- `docs/generated/BedroomLedController_Resource_And_Migration_Report.md`
- `docs/generated/BedroomLedController_Troubleshooting.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest discover -s tests -v`: passed, 130 tests.
- resource report: RAM 60,312 / 80,192 bytes (75%); IRAM 61,383 / 65,536 bytes (93%); flash 605,676 / 1,048,576 bytes (57%).
- Runtime heap after UI changes: not measured.
- Physical LED visual smoothness: not visually retested by Codex.
- OTA firmware upload: not performed.

## Critical Runtime Regression Stabilization

Main fixes:

- Investigated live controller behavior before changing persistence code.
- Found malformed `/api/state` JSON in the previous build: `activePaletteName` emitted an extra quote before `hex`.
- Fixed `buildStateJson()` so the Web UI can parse and hydrate real controller state.
- Added `uiHydrating` and `stateLoaded` guards so browser refresh and `applyState(...)` cannot send mutation requests.
- Added state-unavailable UI handling that preserves current controls instead of presenting defaults as real state.
- Added fixed-buffer last-mutation audit fields for route, action, before/after mode, and before/after brightness.
- Exposed mutation audit fields through diagnostics/resources/state for future random-off tracing.
- Fixed Satin Breathing's non-monotonic exhale by adding `easeInOut01(...)`.
- Kept fabric texture, warm peak blending, and palette support.
- Reset temporal smoothing on mode, brightness, color, white-temperature, Off, and Warm Dim Now changes.
- Kept smoothing bypasses for Solid/off-style output, Strobe, Flash, diagnostics, transitions, and utility/flashing modes.

Generated documentation:

- `docs/generated/BedroomLedController_Critical_Runtime_Regression_Report.md`
- `docs/generated/BedroomLedController_Live_UI_Feedback_Report.md`
- `docs/generated/BedroomLedController_Motion_Smoothness_Report.md`
- `docs/generated/BedroomLedController_Final_Report.md`
- `docs/generated/BedroomLedController_Resource_And_Migration_Report.md`
- `docs/generated/BedroomLedController_Troubleshooting.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Live pre-fix reads reached `http://bedroom-leds.local` at `192.168.1.201`.
- Live pre-fix timer check: inactive.
- Live pre-fix schedule check: no schedules saved.
- Live pre-fix Night Guard: disabled, cap 80.
- Live pre-fix mutation/persistence sequence: did not complete because requests timed out and the board showed instability/short uptime afterward.
- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest discover -s tests -v`: passed, 136 tests.
- `node --check` on extracted Web UI script: passed.
- resource report: RAM 60,664 / 80,192 bytes (75%); IRAM 61,383 / 65,536 bytes (93%); flash 609,228 / 1,048,576 bytes (58%).
- USB upload on `COM5`: attempted but blocked by access denied, so patched live API/persistence checks were not completed.
- Physical LED visual behavior: not claimed unless separately observed after upload.
- OTA firmware upload: not performed.

## Phase 4C - Remaining Mode Library Batch 2

Main additions:

- Added nine new lighting modes: Candle Hall, Slow Prism, Garden Shade, Snowfall Hush, Observatory Glow, Rainy Window, Campfire Blanket, Northern Whisper, and Storm Outside.
- Registered the new modes in the central `Mode` enum, parser, and `modeKey()` output.
- Added metadata rows for all nine modes with safe flags, category/filter data, descriptions, best-use text, and palette roles.
- Added render functions for all nine modes through the existing frame buffer and output pipeline.
- Added the shared `softPulseEnvelope8(...)` helper for smooth deterministic flicker and particle envelopes.
- Added browser-side Live Preview approximations for every Phase 4C mode.
- Added the new mode IDs to the fallback mode picker.
- Added Phase 4C modes to Surprise Me mood pools.
- Added Weather and Nature Surprise Me moods.
- Preserved scene save/load, favorites, timers, bedtime fade, scheduler, `/go` links, Night Guard caps, and smooth transitions through existing APIs.
- Deferred per-mode control panels and new built-in scene slots.

Generated documentation:

- `docs/generated/BedroomLedController_Phase_4C_Report.md`
- `docs/generated/Phase_Completion.md`

Verification status:

- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- `python -m unittest tests.test_phase4c_contract -v`: passed, 7 tests.
- `python -m unittest discover -s tests -v`: passed, 115 tests.
- Arduino sketch mirror hash check: passed for 19 code files.
- resource report: RAM 55,628 / 80,192 bytes (69%); IRAM 61,383 / 65,536 bytes (93%); flash 567,000 / 1,048,576 bytes (54%).
- Runtime free heap, Free heap after /api/schedule, LittleFS usage, OTA behavior, web responsiveness, animation responsiveness, and physical LED behavior: not measured.
- The firmware compiled successfully, but was not physically tested on LEDs.
