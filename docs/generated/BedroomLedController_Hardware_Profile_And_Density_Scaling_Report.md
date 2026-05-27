# Hardware profile and density-scaling migration

This pass migrates the Bedroom LED Controller from raw pixel assumptions toward a hardware-profile model for the ESP32-S3/high-density FCOB strip while preserving the existing ESP8266 path where practical.

## Files changed

- `firmware/BedroomLedController/HardwareProfile.h`
- `firmware/BedroomLedController/Config.h`
- `firmware/BedroomLedController/BedroomLedController.ino`
- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/Diagnostics.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/ScheduleControls.h`
- `firmware/BedroomLedController/ManagementControls.h`
- `firmware/BedroomLedController/WebUi.h`
- `README.md`
- `docs/generated/BedroomLedController_User_Guide.md`
- `docs/superpowers/plans/2026-05-27-hardware-profile-density-scaling.md`
- `tests/test_critical_runtime_regression_contract.py`
- `tests/test_hardware_profile_density_scaling_contract.py`
- `tests/test_phase1c_contract.py`

## Board migration summary

ESP32-S3 support is now conditionalized with the ESP32 Arduino core includes and server type. ESP8266 support remains under the existing ESP8266 conditional branch. The sketch now selects Wi-Fi, web server, filesystem, random seed, reset reason, heap diagnostics, hostname setup, and browser update plumbing through platform-aware wrappers instead of assuming only the D1 mini.

The default board profile is `ESP32-S3-N16R8`. The retained ESP8266 branch reports `D1 mini / ESP8266`.

## Strip profile

- Strip profile: `BTF FCOB 5V 160LED/m 5m`
- Strip type: `BTF-LIGHTING FCOB COB XGB1338(WS2812B) RGB GRB`
- Strip length: 5000 mm
- LED density: 160 LEDs/m by default
- Calculated count when no override is used: 5m x 160 LEDs/m = 800 pixels
- Current LED count override: 800
- Alternate density examples: 5m x 180 LEDs/m = 900 pixels; 5m x 60 LEDs/m = 300 pixels
- Default data pin: GPIO5
- Target frame interval: 33 ms

Density is configured in `HardwareProfile.h` with `LED_DENSITY_PER_METER`. `LED_COUNT_OVERRIDE` can be set to zero to use calculated count, or left at the known physical IC count when the listing or measurement is exact.

## Physical unit helpers

The firmware now exposes:

- `getConfiguredLedCount()`
- `getPixelsPerMeter()`
- `getMmPerPixel()`
- `mmToPixels()`
- `cmToPixels()`
- `metersToPixels()`
- `pixelToMm()`
- `pixelToMeters()`
- `normalizedPosition01()`
- `physicalXmm()`
- `clampPixelIndex()`
- `pixelInRange()`
- `cyclicPixelDistance()`
- `cyclicPhysicalDistanceMm()`
- `pixelsForPhysicalLength()`
- `lastPixelIndex()`

Small nonzero visible features return at least one pixel, conversions clamp safely, and pixel iteration remains bounded by `LED_COUNT`.

## Density math checks

- 100 mm = 6 px at 60 LEDs/m
- 100 mm = 16 px at 160 LEDs/m
- 100 mm = 18 px at 180 LEDs/m
- 5m x 60 LEDs/m = 300 pixels
- 5m x 160 LEDs/m = 800 pixels
- 5m x 180 LEDs/m = 900 pixels

The expected behavior is that increasing density increases smoothness and detail without shrinking physical objects.

## Mode scaling changes

The existing mode list was discovered from the firmware and no new lighting modes were added. The following modes remain present: Solid Color, Strobe, Flash, Chase, Wave, Slow Wave, Slow Pulse, Rainbow, Dawn Bloom, Moon Tide, Velvet Aurora, Lantern Drift, Rain Glass, Satin Breathing, Paper Lantern Row, Library Lamp, Solar Noon, Golden Hour, Dream Aquarium, Circuit Glow, Window Sunrise, Ember Quilt, Porcelain Sky, Hearthline, Evening Current, Starlit Linen, Deep Sea Glass, Quiet Northern Lights, Candle Hall, Slow Prism, Garden Shade, Snowfall Hush, Observatory Glow, Rainy Window, Campfire Blanket, Northern Whisper, and Storm Outside.

Physical-looking dimensions were moved to millimeter-aware constants and helper calls for visible blocks, waves, ribbons, glows, halos, trails, droplet radii, lantern spacing, shimmer scales, candle clusters, star/snow halos, current wavelengths, and chase segments. Strip-spanning gradients use normalized position where the effect is intended to fill the whole strip proportionally.

## Specific mode notes

Candle Hall now uses physical constants for candle core width, glow radius, minimum spacing, and falloff. At 60 LEDs/m the same millimeter-sized candle uses fewer pixels; at 160 or 180 LEDs/m it uses more pixels while staying about the same physical size.

Quiet Northern Lights and Northern Whisper now use physical wavelengths, ribbon widths, and physical strip position so higher density produces smoother aurora ribbons rather than smaller sharp bands.

Rain Glass and Rainy Window now use physical droplet radii and trail lengths. Higher density should make droplets and trails smoother, not microscopic.

Wave, Moon Tide, Evening Current, Deep Sea Glass, Paper Lantern Row, Lantern Drift, Starlit Linen, Snowfall Hush, Observatory Glow, Hearthline, Ember Quilt, Dream Aquarium, Circuit Glow, Chase, Slow Wave, Slow Pulse, Rainbow, Dawn Bloom, Golden Hour, Garden Shade, and Storm Outside were also audited for raw-pixel feature sizes and updated to physical or normalized positioning where appropriate.

Solid, Strobe, Flash, Library Lamp, Solar Noon, Porcelain Sky, Window Sunrise, and Slow Prism remain mostly whole-strip, normalized, or utility effects. Pixel indexes are still used for iteration and output.

## Power estimator

The power estimator now uses the active hardware profile instead of a hardcoded 60 mA-per-pixel assumption.

- Estimate source: `manufacturerRecommendation`
- mA/pixel: 18.75
- Default supply voltage: 5000 mV
- Default supply current: 5000 mA
- Controller overhead: 200 mA
- Safety margin: 20%

The estimator reports its source, mA/pixel assumption, active LED count, strip length, density, and recommended brightness cap behavior. It is still estimate-only and does not measure real current. A generic conservative WS2812 value such as 60 mA/pixel can be configured if desired.

## Diagnostics and API additions

`/api/state`, `/api/diagnostics`, `/api/power`, and the diagnostics page now expose hardware-profile fields including:

- `boardFamily`
- `boardProfile`
- `stripProfile`
- `stripType`
- `stripLengthMm`
- `ledDensityPerMeter`
- `configuredLedCount`
- `mmPerPixel`
- `targetFrameIntervalMs`
- `dataPin`
- `powerEstimateMilliampsPerPixel`
- `powerEstimateSource`

The diagnostics page also shows board profile, strip profile, physical length, LED density, LED count, mm per pixel, frame interval, estimate source, and mA per pixel.

## Existing feature preservation

The existing web UI, state routes, set route, diagnostics routes, power route, mode metadata, scenes, favorites, Night Guard, timer, bedtime fade, OTA setup calls, browser update route, settings persistence, palettes, preview, schedule behavior, backup/export, diagnostics LED tests, Warm Dim Now, Off, and transition code paths were preserved at the structural level.

No scene schema field that stores raw pixel widths was found in the current firmware. Existing scenes continue to store mode, color, brightness, speed, palette, Night Guard, timer, and related controller state rather than pixel-size geometry.

## Compile and test status

- ESP32-S3 compile: blocked. `arduino-cli compile --fqbn esp32:esp32:esp32s3 firmware\BedroomLedController` failed because platform `esp32:esp32` is not installed. A `core install esp32:esp32` attempt timed out after ten minutes and was stopped.
- ESP8266/D1 mini worktree compile: passed with `esp8266:esp8266:d1_mini`. RAM globals/statics: 69,356 / 80,192 bytes (86%). IRAM: 61,383 / 65,536 bytes (93%). Flash/IROM: 623,420 / 1,048,576 bytes (59%).
- ESP8266/D1 mini mirrored sketch compile: passed with the same memory numbers.
- Python contract tests: `python -m unittest discover -s tests -v` passed, 151 tests.
- Whitespace/static diff check: `git diff --check` passed.
- Density math check: 60 LEDs/m -> 300 pixels and 100 mm -> 6 px; 160 LEDs/m -> 800 pixels and 100 mm -> 16 px; 180 LEDs/m -> 900 pixels and 100 mm -> 18 px.
- Physical LED output: not physically tested.
- OTA upload: OTA upload not performed.

## Known limitations

- Changing compile-time strip geometry currently requires rebuilding and uploading firmware.
- GPIO5 is a safe default candidate, not proof of the user's actual wiring. Confirm the ESP32-S3 board pinout before upload.
- The estimator is not a current sensor.
- High-density WS2812-style output time limits realistic animation rates; 800 pixels is roughly a 24 ms data push before rendering work, and 900 pixels is roughly 27 ms.
- ESP8266 compatibility remains best-effort for the legacy smaller strip path; the high-density profile is intended for the ESP32-S3.

## User must configure before upload

- LED data pin
- Actual density if not 160 LEDs/m
- LED count override if the physical IC count is known or differs from calculated density
- Strip length
- Power supply voltage and current
- Power estimate mode or mA/pixel value
- Initial brightness cap
- Wi-Fi and OTA credentials in `Secrets.h`

## Recommended first-upload procedure

1. Select the correct ESP32-S3 board target.
2. Confirm `LED_DATA_PIN`.
3. Confirm `STRIP_LENGTH_MM`.
4. Confirm `LED_DENSITY_PER_METER`.
5. Confirm calculated LED count: 5m x 160 LEDs/m = 800 pixels, or 5m x 180 LEDs/m = 900 pixels.
6. Set a conservative initial brightness cap such as 30-60 / 255.
7. Power the LED strip from an external 5V supply, not through the ESP32-S3.
8. Connect common ground between ESP32-S3 and LED power supply.
9. Use a level shifter if 3.3V data is unreliable.
10. Upload over USB first.
11. Open diagnostics and verify board/strip fields.
12. Run red, green, blue, white-low, alternating, and pixel-walk tests.
13. Test Candle Hall and confirm candle width looks physically correct.
14. Test Quiet Northern Lights and confirm ribbons are broad and smooth.
15. Test Rain Glass and confirm droplets and trails are visible.
16. Increase brightness only after power injection, voltage drop, and heat are verified.
