# BedroomLedController Motion Smoothness Report

## Summary

This pass adds a small central temporal smoothing layer for ambient animated modes and smooths a few discrete texture sources. It does not add modes, remove mode layers, simplify renderer math, change default brightness, or move brightness/gamma/calibration out of the central output pipeline.

## Current render flow

- Animated modes are still throttled at roughly 33 ms per frame when the LEDs are not otherwise dirty.
- Normal modes render into `frameBuffer`.
- Optional temporal smoothing now runs before final output processing.
- RGB calibration, effective brightness / Night Guard cap, gamma correction, pixel write, and `strip.show()` still happen centrally in `outputFrameToStrip()`.

## Smoothing approach

- Added `RgbPixel smoothedFrame[LED_COUNT]`, a single 140-pixel smoothing buffer. At 140 LEDs this costs about 420 bytes of static RAM.
- Added `approach8(...)` for bounded per-channel movement toward the newest rendered frame.
- Added `shouldApplyTemporalSmoothing(...)` and `smoothingStepForMode(...)` to keep smoothing mode-aware.
- Added `smoothHash8(...)` for interpolated hash texture where discrete bucket changes could create perceptual stepping.

## Modes affected

Temporal smoothing applies to animated ambient modes that are not utility/flashing modes. Calm and sleep-safe metadata get the strongest smoothing, weather/colorful modes get lighter smoothing, and other animated modes use the default step.

The following renderer texture terms were gently smoothed without removing their visual character:

- Slow Pulse faint texture.
- Satin Breathing fabric texture.
- Ember Quilt warm texture.

## Modes bypassed

Smoothing is bypassed for:

- Solid / off-style immediate output.
- Strobe.
- Flash.
- Diagnostic LED tests.
- Active transition rendering.
- Metadata-marked utility or flashing modes.

Warm Dim Now and Panic Warm remain immediate because they land on solid-style output and do not pass through ambient animation smoothing.

## Visual quality preservation

No layered waves, shimmer, palette blending, Gaussian falloffs, ribbon logic, or weather/nature/evening renderer math was removed. The changes are suspension tuning around frame-to-frame deltas, not a renderer rewrite.

## Physical LED visual status: not visually retested in this pass

The code compiles after verification, but Codex did not visually observe the real LED strip during this pass. Do not treat the smoothing as physically confirmed until the user uploads and watches Solid, Rain Glass, Velvet Aurora, Dream Aquarium, Quiet Northern Lights, Satin Breathing, Paper Lantern Row, Storm Outside, Slow Pulse, and Slow Wave at brightness 100.

## Resource status

- Static RAM impact: one extra 140 LED RGB buffer, approximately 420 bytes. Compile RAM globals/statics rose from the previous 59,896 bytes to 60,312 bytes.
- Compile resources: RAM 60,312 / 80,192 bytes (75%); IRAM 61,383 / 65,536 bytes (93%); Flash/IROM 605,676 / 1,048,576 bytes (57%).
- Python contract tests: passed, 130 tests.
- Runtime heap impact: not remeasured in this pass.
- OTA behavior: not retested in this pass.
