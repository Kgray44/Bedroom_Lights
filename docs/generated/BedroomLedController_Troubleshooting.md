# BedroomLedController Troubleshooting

## D3/GPIO0 boot pin warning

The LED data pin is configured as D3 / GPIO0. On ESP8266 boards, GPIO0 affects boot mode. If the strip, wiring, or level shifting pulls it low during reset, the board may fail to boot normally. If boot reliability becomes inconsistent, move the LED data line to a safer pin in both wiring and `Config.h`.

## Page does not load

- Confirm the D1 mini is powered.
- Try the fallback AP if Wi-Fi credentials are wrong.
- Check `/diagnostics` after reconnecting.
- If the diagnostics page is sparse, check `/api/resources`; `/api/diagnostics` intentionally returns compact heap-safe JSON on ESP8266.

## UI section says unavailable

- Use the section Retry button first; it refreshes only that section instead of reloading the whole page.
- If Favorites, Scenes, or Palettes are truly empty, the UI now says empty rather than unavailable.
- `Unavailable` means endpoint failure, invalid JSON, timeout, Wi-Fi interruption, or a low-resource controller response.
- Open the browser console and look for `API request failed` entries with URL, status code, raw response preview, and timestamp.
- Check `/api/resources` after repeated failures; low max free block or high fragmentation can make large JSON routes unreliable.

## Page refresh shows defaults or wrong controls

- Page refresh should be read-only. The Web UI now blocks mutation sends while hydrating and until `/api/state` has loaded.
- If state cannot be parsed or loaded, the UI shows `State unavailable - controls preserved` instead of treating HTML defaults as real controller state.
- Check `/api/state` directly. If it fails to parse, fix that before trusting the visible controls.
- Check `/api/diagnostics` or `/api/resources` for `lastMutationRoute`, `lastMutationAction`, and before/after brightness/mode fields.
- Final Playwright testing loaded and reloaded the page with 14 read-only requests and no mutation endpoints.

## Lights turn off unexpectedly

- Immediately read `/api/state`, `/api/timer`, `/api/schedule`, `/api/diagnostics`, and `/api/resources`.
- If `lastMutationRoute` points to `/api/action`, `/api/favorites/load`, `/api/scenes/load`, `/api/timer/start`, or `/api/bedtime/start`, that route is the first suspect.
- If timer is inactive, schedules are empty, Night Guard is disabled, and brightness/color are zero, look for the last mutation audit and reset reason.
- If uptime is very short or reset reason changed, inspect power stability, common ground, and heap pressure before assuming a scheduler or mode bug.

## Scenes or palettes will not save

- Check the reported count against the max count.
- Check diagnostics for LittleFS status.
- If a scene import references a missing palette, the import disables that palette reference.
- LittleFS free space is reported by diagnostics, but long-term LittleFS wear was not measured.

## Schedule does not run

- Confirm time sync status on `/schedule`.
- Confirm the target scene or action still exists.
- Confirm days and local UTC offset.
- Free heap during scheduler execution is not measured.

## OTA issues

- `/ota` links to `/update`.
- Diagnostics reports `otaRoutePresent` and `browserUpdateRoutePresent`.
- Diagnostics reports `freeSketchSpaceBytes`; OTA can fail if free sketch space is too low for the uploaded binary.
- If OTA fails after a large feature build, upload over USB first, then check `/api/diagnostics`.
- Final OTA capability check: `/ota` returned HTTP 200, `/update` returned HTTP 401 without auth and HTTP 200 with local OTA credentials. A later OTA firmware upload passed with direct `espota.py` after pinning the host IP to the Wi-Fi interface.

## Backup issues

- Full backup export is large.
- Full backup export streams scenes and palettes and omits full diagnostics with `diagnostics.omittedFromBackup=true`.
- Full backup import validates schema only and reports `applied:false`.
- Use selective scene, palette, and schedule imports for applied restore.
- If the web UI resets or hangs after backup export, check `minFreeHeapSinceBoot`, `maxFreeBlockSize`, and `heapFragmentationPercent` in `/api/resources`.

## Low heap or fragmentation warnings

- Low `freeHeapNow` means the current runtime has little RAM margin.
- Low `maxFreeBlockSize` means large contiguous allocations may fail.
- High `heapFragmentationPercent` means repeated dynamic allocation may have split the heap into smaller blocks.
- `/api/resources` may return `endpointHeapMetrics: []` when contiguous heap is too small to safely build the detailed metrics array.
- Restart the controller, avoid repeated heavy exports, reduce large imports, and consider ESP32-S3 before adding more large features.

## LittleFS space warning

- Diagnostics reports `littleFsTotalBytes`, `littleFsUsedBytes`, and `littleFsFreeBytes`.
- If free space is low, delete custom scenes/palettes/schedules or use factory reset after exporting anything important.
- LittleFS wear and long-duration save behavior were not measured in this pass.

## Physical LED test checklist

- Confirm external LED power supply rating.
- Confirm D1 mini ground and LED power ground are common.
- Confirm data direction on the LED strip.
- Confirm the data pin matches `LED_PIN`.
- Start with low brightness and run diagnostics LED tests before using full animations.
- Codex performed API-level Satin Breathing soak testing at brightness 100, but did not independently observe physical LED output.
