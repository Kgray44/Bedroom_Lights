# BedroomLedController Project Overview

## Project Idea

This project is a bedroom LED strip controller for a WS2812 LED strip installed along the side of the room. The controller is a D1 mini, and the LED data line is set up on pin D3. The goal is to make the strip feel like a polished room-lighting feature instead of a basic test strip: easy local control, calm lighting modes, OTA updates, and a clean web interface.

The current sketch is named `BedroomLedController`, and it lives in the Arduino-required folder layout:

`Documents/Arduino/BedroomLedController/BedroomLedController.ino`

## What Was Asked For

The original request was for a D1 mini Arduino sketch that controls about 6 ft of WS2812 LEDs on D3 and hosts a local web page. The web page needed a modern, clean design with a color wheel for setting the whole strip color and a vertical white-temperature slider from 2700K to 6500K. The white slider also needed a visible warm-to-cool gradient behind it.

The strip should always use the last selected input color, whether that came from the color wheel or the white-temperature slider. The D1 mini also needed OTA enabled in the easiest practical way.

The requested animation modes were:

- Strobe
- Flash
- Chase
- Wave
- Slow Wave
- Slow Pulse
- Rainbow

The request also asked for five more sophisticated, relaxing, daytime/evening-friendly animation modes, plus a detailed document explaining all modes and a set of future project ideas.

## What Was Implemented

The current sketch implements a self-contained local web controller for the D1 mini. It uses `ESP8266WebServer` to serve the web interface, `Adafruit_NeoPixel` to drive the WS2812 strip, `ArduinoOTA` for Arduino IDE OTA uploads, and `ESP8266HTTPUpdateServer` for browser-based firmware upload.

The sketch currently defaults to:

- LED pin: D3
- LED count: 60
- Master brightness cap: 180 out of 255
- Hostname: `bedroom-leds`
- Fallback access point: `BedroomLED-Setup`

Before uploading to the D1 mini, the Wi-Fi name, Wi-Fi password, LED count, and OTA password should be edited near the top of the sketch.

## Local Web Interface

The web page is served directly from the D1 mini. It is designed as a clean dark interface with simple sections for color, white temperature, and animation controls.

Implemented web routes:

- `/`: main LED control page
- `/state`: JSON state endpoint used by the page
- `/set`: settings endpoint for color, white temperature, mode, and animation options
- `/ota`: friendly OTA information page
- `/update`: browser-based firmware upload page

The main page includes:

- A color picker for the active strip color
- A live color swatch
- A vertical 2700K to 6500K white-temperature slider
- A visible warm-to-cool gradient behind the temperature slider
- A mode dropdown
- Conditional controls that appear only for modes that need extra settings
- A link to the OTA page

## OTA Support

Two OTA update methods are enabled.

The first is Arduino IDE OTA. After the first USB upload, the board should appear on the network as `bedroom-leds` when the Arduino IDE can discover it. The password is controlled by `OTA_PASS` in the sketch.

The second is browser OTA. The `/ota` page explains how to open the web updater. The actual upload page is `/update`, protected by the username and password configured in the sketch.

## Implemented Lighting Modes

### Solid Color

Solid Color fills the whole strip with the latest selected color or white-temperature value.

### Strobe

Strobe flashes the whole strip on and off. The delay dropdown controls both the on time and off time from 0.1 to 10 seconds.

### Flash

Flash turns the whole strip on for a fixed 0.5 seconds, then off for the selected delay. The off-time dropdown ranges from 0.1 to 10 seconds.

### Chase

Chase is a sharper wave-like animation using the active color. Pixel brightness ranges from 0 to 255, and the loop time can be set from 1 to 15 seconds.

### Wave

Wave is a softer version of Chase. Its brightness range is 40 to 255, so the strip never falls fully dark. The loop time can be set from 5 to 30 seconds.

### Slow Wave

Slow Wave is a more complex layered wave animation. Multiple waves build slowly, speed up, brighten, crest, and then crash into a wider fading glow. It is meant to feel more like a side view of beach waves than a basic repeating LED wave.

### Slow Pulse

Slow Pulse creates several random pulsing sections along the strip. Each pulse has a center, width, fade-in, peak, and fade-out. The active pulse count can be set from 1 to 15, and pulse timing can be set from 1 to 60 seconds.

### Rainbow

Rainbow overrides the active color and sends a gliding rainbow down the strip. Its speed can be set from 1 to 30 seconds.

## Added Calm Modes

Five extra calm modes were added directly into the sketch.

### Dawn Bloom

A sunrise-like warm glow that gradually sweeps across the strip with gentle breathing.

### Moon Tide

A cool, soft, layered tide pattern using the active color blended with moonlit white.

### Velvet Aurora

A polished ribbon effect that blends warmer and cooler veils around the active color.

### Lantern Drift

Wide floating pools of warm light that drift slowly through the strip.

### Rain Glass

Soft luminous beads and trails that feel like quiet rain moving across glass.

## Documentation Added

Three documentation files now live beside the sketch.

- `BedroomLedController_Modes_and_Ideas.md`: setup notes, OTA notes, electrical notes, implemented mode explanations, and the first five future ideas.
- `BedroomLedController_Expanded_Mode_and_Project_Ideas.md`: power estimate for 60 LEDs, 15 additional lighting mode ideas, 10 software-only project ideas, and suggested future implementation order.
- `BedroomLedController_Project_Overview.md`: this recap document.

## Power Summary

For 60 WS2812 LEDs, the worst-case full-white estimate is about 3.6 A at 5 V, or 18 W. With the current master brightness cap of 180 out of 255, the practical capped full-white estimate is closer to about 2.5 A for the LEDs, plus the D1 mini overhead.

A 5 V 4 A supply is the practical minimum for 60 LEDs. A 5 V 5 A supply is the better choice because it gives more headroom and should be more stable.

The LED strip should be powered from a proper 5 V supply, not through the D1 mini's USB power path. The strip ground, D1 mini ground, and power supply ground should all be connected together.

## Current Status

The project currently has a complete Arduino sketch, a working local web UI, OTA support, the requested animations, five implemented calm animations, and detailed documentation. The sketch was compiled successfully for the D1 mini board target using the ESP8266 Arduino core.

The next practical step is to edit the Wi-Fi settings and LED count in the sketch, upload it over USB, connect the LEDs and power supply, then open the controller page from the D1 mini's IP address.
