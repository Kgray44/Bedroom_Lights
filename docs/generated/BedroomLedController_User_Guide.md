# BedroomLedController User Guide

Open the controller in a browser using the D1 mini IP address or hostname. The main page contains live controls, scenes, palettes, favorites, timers, bedtime fade, transitions, and maintenance tools.

## Daily controls

- Use Master Brightness as the global output cap.
- Use Color and White Temperature for the base color.
- Choose a Mode, then adjust the mode-specific timing controls that appear.
- Use Palettes for compatible ambient modes.
- Use Night Guard to cap brightness and block flashing modes.

## Scenes

- Save Current Scene stores the active color, mode, brightness, timing, and palette state.
- Load applies a scene.
- Duplicate creates a custom copy.
- Export writes scene JSON into the scene import box.
- Import Scene JSON imports one exported scene as a custom scene.
- Restore Built-In Scenes re-adds firmware defaults while preserving custom scenes when capacity allows.

## Backup and restore

- Export Full Backup writes a schema-versioned backup JSON bundle.
- Import Full Backup validates a backup with `confirm=YES`; it does not apply a bulk restore on this ESP8266 build.
- Use scene, palette, and schedule import routes for applied selective restore.
- Factory Reset requires typing `YES` and restores firmware defaults.

## Schedule and timers

- Sleep Timer can wait and then turn off, warm dim, or load a scene.
- Bedtime Fade gradually changes toward a bedtime target.
- The Schedule page supports repeated local-time scene/action automation.

## Maintenance

- Diagnostics are at `/diagnostics`.
- OTA upload is at `/ota`, with the browser updater at `/update`.
- API help is at `/api-docs`.
- Runtime heap after backup export, OTA behavior, and physical LED behavior are not measured in this pass.
