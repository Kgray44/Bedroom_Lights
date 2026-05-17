# BedroomLedController Resource And Migration Report

## Final resource report

- Board target used for compile: `esp8266:esp8266:d1_mini`
- RAM globals/statics: 57,800 / 80,192 bytes (72%).
- IRAM: 61,383 / 65,536 bytes (93%).
- Flash/IROM: 583,964 / 1,048,576 bytes (55%).
- LittleFS usage: not measured.
- Free heap after boot: not measured.
- Free heap after loading main page: not measured.
- Free heap after `/api/state`: not measured.
- Free heap after `/api/scenes`: not measured.
- Free heap after `/api/palettes`: not measured.
- Free heap after `/api/schedule`: not measured.
- Free heap after `/api/backup/export`: not measured.
- OTA upload behavior: not tested.
- Web UI responsiveness: not tested.
- Animation responsiveness: not tested.
- Physical LED behavior: not tested.

Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.

## Migration recommendation

The completed build can remain on the D1 mini. IRAM is the main ceiling at 93%, followed by RAM globals at 72%. Future expansion should move to ESP32-S3 if it adds heavy UI pages, richer backup restore, sensors, cloud features, physical controls, or more persistent data types.

## Stay on D1 mini when

- The project remains local-only.
- New features are small and reuse existing helpers.
- Preview remains browser-side.
- No heavy libraries are added.

## Move to ESP32-S3 when

- Bulk backup restore needs to apply large multi-section payloads safely.
- Runtime heap measurements show low margin.
- More hardware inputs or sensors are added.
- UI/API payloads keep growing.
