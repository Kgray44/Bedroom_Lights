# BedroomLedController Quiet Northern Lights True Aurora Wavefield Report

## Scope

This pass rewrites only the existing `Quiet Northern Lights` renderer and its browser preview. It does not add a new mode, rename the public mode, change hardware support, or bypass the central output pipeline.

## Files changed

- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/WebUi.h`
- `tests/test_quiet_northern_lights_intensity_contract.py`
- `docs/generated/BedroomLedController_Quiet_Northern_Lights_True_Aurora_Wavefield_Report.md`
- `docs/generated/BedroomLedController_Final_Report.md`
- `docs/generated/BedroomLedController_Resource_And_Migration_Report.md`
- `docs/generated/Phase_Completion.md`

## Problem found

The previous smooth curtain version removed the harsher Gaussian ribbon math, but it still had a blob-like visual structure: each layer had a local position, a width, and a local falloff. On the LED strip that can still read as one broad luminous mass moving back and forth instead of an aurora sheet.

## New wavefield layer model

The new renderer treats each aurora layer as a full-strip wavefield rather than a local object. Each layer has:

- spatial frequency
- phase shift
- warp frequency
- fold frequency
- opacity
- color bias
- life/fade cycle

For each LED, the renderer evaluates multiple phase-continuous layers, turns each layer into a soft ridge, modulates it with fold and fine-curtain texture, and blends the overlapping contributions into a deep night-sky base.

## How the renderer avoids blobs

The main `Quiet Northern Lights` effect no longer uses a local center, pixel distance from that center, or center-width falloff as its primary model. The visible structure comes from `fieldPhase`, `waveValue`, `softRidge(...)`, `folds`, `fineCurtain`, and `waveContribution` across the full strip.

## Glide, reverse, and motion shaping

Each layer uses a deterministic oscillator with `easeSlowAccelFastDecel(...)`. The eased phase shift gives slow acceleration and relatively faster deceleration near reversal, without sudden snaps or constant-speed marching.

## Fade-out and reappear behavior

Each layer has a smooth `lifeEnvelope` made from `fadeIn` and `fadeOut`. This fade-out path lets layer generation values change phase and shape only as the life cycle is faded down, so reappear behavior is soft instead of popping at full brightness.

## Active color anchoring

When palette mode is disabled, the active color remains the anchor. Aurora green, cyan, teal, violet, and pale companions are blended from that active color according to Animation Intensity, so the mode remains user-color aware instead of becoming a fixed rainbow effect.

## Palette support

When palette mode is enabled and the current mode supports palettes, each layer samples the active palette with a slow full-field coordinate. Palette sampling remains local to the existing palette system and does not stream frames from the browser.

## Animation Intensity

Animation Intensity remains a persistent `0-255` setting separate from master brightness.

- Low intensity uses fewer, broader, calmer layers.
- Medium intensity increases overlap, fold visibility, and color spread.
- High intensity increases layer drama and overlap while staying non-flashing.

Master brightness, Night Guard, gamma correction, and RGB calibration still happen in the central output pipeline after this renderer writes the frame buffer.

## Skipping reduction

- Removed the local blob/window model from this mode.
- Removed `expf()` and Gaussian-style falloff from this renderer.
- Uses deterministic layer seeds and no per-frame `random()`.
- Uses slow motion rates and phase-continuous wave equations.
- Precomputes layer parameters outside the LED pixel loop.
- Keeps the active layer count bounded at 2-5 on ESP8266.
- Keeps `MODE_QUIET_NORTHERN_LIGHTS` eligible for temporal smoothing.

## Tests/compile status

- Focused true wavefield contract test: passed.
- Full Python contract suite: passed, 144 tests.
- Worktree compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch mirror compile for `esp8266:esp8266:d1_mini`: passed.
- Web UI JavaScript syntax check: passed with the bundled Node.js runtime.
- OTA upload: attempted but did not complete.
- Live device sketch after OTA attempts: still reports `sketchSizeBytes=677936`, so the true wavefield build was not installed over OTA.
- USB fallback: not performed because no local serial port was detected; `COM5` was unavailable.
- Live state restored after failed OTA attempts: `mode=quietNorthernLights`, `masterBrightness=255`, `animationIntensity=255`, `settingsSaveStatus=saved`.

Latest local compile resources after this pass:

- RAM globals/statics: 61,128 / 80,192 bytes (76%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 620,832 / 1,048,576 bytes (59%).
- OTA binary size: 680,896 bytes.

Live resource snapshot after failed OTA attempts and restoring max state:

- `/api/resources`: free heap 5,040 bytes, max block 3,584 bytes, fragmentation 15%, min heap since boot 2,488 bytes.
- Live `freeSketchSpaceBytes`: 1,417,216.

## Known limitations

- Runtime frame timing is not directly instrumented.
- The effect is deterministic rather than physically simulated aurora.
- Final speed, contrast, and fold-depth tuning still require viewing the real strip.
- The true wavefield build has not been installed on the live D1 mini yet because OTA transfers repeatedly dropped mid-upload.

## Physical LED testing status

Physical LED visual behavior has not yet been independently observed by Codex for this pass.
