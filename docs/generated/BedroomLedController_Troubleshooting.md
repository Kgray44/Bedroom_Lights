# BedroomLedController Troubleshooting

## D3/GPIO0 boot pin warning

The LED data pin is configured as D3 / GPIO0. On ESP8266 boards, GPIO0 affects boot mode. If the strip, wiring, or level shifting pulls it low during reset, the board may fail to boot normally. If boot reliability becomes inconsistent, move the LED data line to a safer pin in both wiring and `Config.h`.

## Page does not load

- Confirm the D1 mini is powered.
- Try the fallback AP if Wi-Fi credentials are wrong.
- Check `/diagnostics` after reconnecting.
- Runtime free heap after loading the main page is not measured in this pass.

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
- OTA upload was not retested after Phase 4D.

## Backup issues

- Full backup export may be large.
- Full backup export is chunked in this build, but it is still one of the heaviest routes on ESP8266.
- Full backup import validates schema only and reports `applied:false`.
- Use selective scene, palette, and schedule imports for applied restore.
- If the web UI resets or hangs after backup export, check `endpointHeapMetrics`, `minFreeHeapSinceBoot`, `maxFreeBlockSize`, and `heapFragmentationPercent` in `/api/diagnostics`.

## Low heap or fragmentation warnings

- Low `freeHeapNow` means the current runtime has little RAM margin.
- Low `maxFreeBlockSize` means large contiguous allocations may fail.
- High `heapFragmentationPercent` means repeated dynamic allocation may have split the heap into smaller blocks.
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
- Physical LED behavior was not tested in this pass.
