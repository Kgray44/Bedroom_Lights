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
- OTA upload was not retested after Phase 4D.

## Backup issues

- Full backup export may be large.
- Full backup import validates schema only and reports `applied:false`.
- Use selective scene, palette, and schedule imports for applied restore.
- Heap during backup export/import is not measured.
