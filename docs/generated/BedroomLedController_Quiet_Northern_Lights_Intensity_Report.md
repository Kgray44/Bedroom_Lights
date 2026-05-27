# BedroomLedController Quiet Northern Lights Intensity Report

## Scope

This targeted pass upgrades the existing `Quiet Northern Lights` mode and adds a shared Animation Intensity control. It does not add a new mode, new hardware support, or a new output path.

## Files changed

- `firmware/BedroomLedController/ControllerTypes.h`
- `firmware/BedroomLedController/ForwardDeclarations.h`
- `firmware/BedroomLedController/LedRendering.h`
- `firmware/BedroomLedController/ScenePresets.h`
- `firmware/BedroomLedController/SettingsStorage.h`
- `firmware/BedroomLedController/WebRoutes.h`
- `firmware/BedroomLedController/WebUi.h`
- `docs/generated/BedroomLedController_User_Guide.md`
- `docs/generated/BedroomLedController_Final_Report.md`
- `docs/generated/BedroomLedController_Resource_And_Migration_Report.md`
- `docs/generated/Phase_Completion.md`
- `tests/test_quiet_northern_lights_intensity_contract.py`

## Old behavior

The previous `Quiet Northern Lights` renderer used two sine-wave veil layers over a cool night base. It was calm, but the motion read more like a simple blended gradient than broad aurora curtains.

## Animation Intensity

- Range/default: `0-255`, default `150`.
- Meaning: lower values make animated modes nearly still and subtle; higher values increase motion, contrast, shimmer, ribbon count, dissolve depth, and color spread without changing master brightness.
- UI: the main page now includes an Animation Intensity slider near the mode controls.
- API/state: `/api/state` includes `animationIntensity` and `animationIntensityPercent`; `/api/intensity?value=...` updates the value; legacy `/set?animationIntensity=...` is also accepted.
- Persistence: settings save/load includes `animationIntensity`; old settings files that do not contain it keep the default.
- Scenes: scene capture/load JSON includes `animationIntensity`, so scenes can preserve the current animation behavior.

## Quiet Northern Lights renderer

The renderer now uses multiple broad virtual ribbons over a dark blue/cool night base. Each ribbon has deterministic reversible drift, width, dissolve, soft Gaussian falloff, and internal fold texture.

Intensity mapping:

- `0-50`: two very slow ribbons, mostly anchor/cyan, low contrast, almost no shimmer.
- `51-128`: three soft ribbons with gentle fold movement.
- `129-200`: four richer ribbons with deeper dissolve/reappear behavior.
- `201-255`: five vivid but still calm ribbons.

## Color behavior

When no palette is enabled, the active color remains the anchor. Companion aurora colors are derived from that active color toward aurora green, cyan, violet, and pale mint, with color spread controlled by Animation Intensity.

## Palette behavior

When palette support is enabled for the mode, ribbons sample the active palette through the existing palette path instead of forcing fixed aurora colors.

## Output pipeline preservation

The mode still writes into the central frame buffer with `setFramePixelRgb(...)`. Master brightness, Night Guard effective brightness, RGB calibration, gamma correction, strip writes, and central `strip.show()` remain in the shared output pipeline; the central output pipeline is preserved.

## Browser preview

The browser preview for `quietNorthernLights` now reads `state.animationIntensity`, varies ribbon count and ribbon spread, samples the active palette when enabled, and approximates the firmware's fold/dissolve ribbon model.

## Tests/compile status

- Focused Animation Intensity contract test: passed.
- Full Python contract suite: passed, 142 tests.
- Worktree firmware compile for `esp8266:esp8266:d1_mini`: passed.
- Arduino sketch folder compile for `esp8266:esp8266:d1_mini`: passed.
- Web UI JavaScript syntax check: passed with `node --check` on the extracted script.
- `git diff --check`: passed.

Latest local compile resources after this pass:

- RAM globals/statics: 61,128 / 80,192 bytes (76%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 616,176 / 1,048,576 bytes (58%).

## Known limitations

- Only `Quiet Northern Lights` uses the new shared Animation Intensity setting in firmware during this pass.
- Other calm animated modes can adopt the shared helpers later in small, mode-specific passes.
- Runtime heap after this specific pass has not been measured.

## Physical LED testing status

Physical LED visual behavior for this pass has not been independently observed by Codex. Real-strip tuning should check intensity `0`, `64`, `128`, `192`, and `255`.
