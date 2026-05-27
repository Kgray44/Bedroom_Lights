# BedroomLedController Quiet Northern Lights Smooth Curtain Report

## Scope

This pass rewrites only the existing `Quiet Northern Lights` renderer and its browser preview. It does not add a new mode, rename the public mode, or change hardware support.

## Files changed

- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/WebUi.h`
- `docs/generated/BedroomLedController_Quiet_Northern_Lights_Smooth_Curtain_Report.md`
- `docs/generated/BedroomLedController_Final_Report.md`
- `docs/generated/BedroomLedController_Resource_And_Migration_Report.md`
- `docs/generated/Phase_Completion.md`
- `tests/test_quiet_northern_lights_intensity_contract.py`

## Problem found

The previous upgrade skipped too much because the renderer behaved like several independent bright blobs instead of a continuous aurora sheet. It used per-pixel Gaussian `expf()` falloff across multiple ribbons, stronger shimmer, and visible time/modulo motion in palette sampling. That made motion more computationally expensive and easier to perceive as stepping on the ESP8266.

## New aurora curtain-wave model

The new model uses broad overlapping curtain-wave layers. Each curtain spans a wide section of the strip and has:

- Stable deterministic seed values.
- Slow reversible left/right glide.
- Secondary organic drift.
- A smooth cycle with fade in and fade out.
- Cheap smoothstep-style soft-band falloff.
- Internal fold and lower-edge wave texture.
- A calm night background and soft lower glow.

New waves appear without popping because the cycle reset happens only after the `lifeEnvelope` is faded down. The renderer does not use `random()` in the animation path.

## Color behavior

When palette is disabled, the active color remains the anchor. Aurora green, cyan, violet, and pale mint companions are blended toward that anchor based on Animation Intensity, so high intensity is richer without becoming Rainbow mode.

## Palette behavior

When palette is enabled and the mode supports palettes, each curtain samples the active palette slowly from position, curtain center, and a small continuous sine offset. There is no visible modulo jump in the palette travel.

## Animation Intensity

Animation Intensity still uses the existing `0-255` setting and remains separate from master brightness.

- `0-50`: two very soft curtain waves, low contrast, nearly still.
- `51-128`: three gentle curtain waves with subtle folds.
- `129-200`: four overlapping waves with richer color and fade depth.
- `201-255`: five vivid but smooth curtain waves.

## Skipping reduction

- Replaced `expf()` Gaussian falloff with `softBand(...)`, a cheap smoothstep falloff.
- Precomputed shared color/intensity values once per frame.
- Kept wave count bounded at 2-5.
- Removed hash shimmer from the renderer in favor of very slow sine shimmer.
- Avoided per-frame `random()`, `String`, heap allocation, `delay()`, and mode-local `strip.show()`.
- Kept `MODE_QUIET_NORTHERN_LIGHTS` eligible for the existing temporal smoothing path.

## Output pipeline preservation

The mode writes through `setFramePixelRgb(...)` into the central frame buffer. Master brightness, Night Guard, gamma correction, RGB calibration, and central `strip.show()` remain unchanged.

## Tests/compile status

- Focused smooth curtain contract test: passed.
- Full Python contract suite: passed, 143 tests.
- Worktree compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch mirror compile: passed.
- Web UI JavaScript syntax check: passed with `node --check` on the extracted script.
- `git diff --check`: passed.
- OTA upload after this pass: passed with direct `espota.py` to `192.168.1.201`.
- Live state after OTA: `mode=quietNorthernLights`, `masterBrightness=255`, `animationIntensity=255`, `settingsSaveStatus=saved`.

Latest local compile resources after this pass:

- RAM globals/statics: 61,128 / 80,192 bytes (76%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 617,872 / 1,048,576 bytes (58%).

Latest live resource snapshot after OTA and setting max brightness/max animation:

- `/api/resources`: free heap 6,056 bytes, max block 4,344 bytes, fragmentation 17%, min heap since boot 3,200 bytes.
- `/api/diagnostics`: free heap 4,816 bytes, max block 3,960 bytes, fragmentation 18%, min heap since boot 3,200 bytes.
- Live `sketchSizeBytes`: 677,936.
- Live `freeSketchSpaceBytes`: 1,417,216.
- LittleFS: 57,344 / 2,072,576 bytes used.

## Known limitations

- This pass uses static deterministic curtain cycles rather than persistent curtain objects.
- Runtime frame timing was not instrumented in this pass.
- Final visual tuning still needs the physical strip.

## Physical LED testing status

Physical LED visual behavior has not yet been independently observed by Codex for this pass.
