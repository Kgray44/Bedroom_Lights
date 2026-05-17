# BedroomLedController Modes and Project Notes

This sketch is built for a WS2812 strip on a D1 mini, using pin D3. The controller hosts a local web page with a color wheel, a vertical white-temperature slider, animation controls, and OTA update tools.

## First Setup

Edit these values near the top of `BedroomLedController.ino` before the first USB upload:

- `WIFI_SSID`: your Wi-Fi network name.
- `WIFI_PASS`: your Wi-Fi password.
- `LED_COUNT`: the number of LEDs on the strip.
- `MASTER_BRIGHTNESS`: a global safety cap for brightness.
- `OTA_PASS`: the Arduino OTA password.

If the Wi-Fi credentials are left as placeholders, or if the D1 mini cannot join Wi-Fi, it starts a fallback access point named `BedroomLED-Setup` with password `bedroomleds`. Connect to that access point and open `http://192.168.4.1`.

If it joins your home Wi-Fi, open the IP address shown in Serial Monitor. The configured hostname is `bedroom-leds`, so `http://bedroom-leds.local` may also work on networks with mDNS support.

## OTA Updates

Two OTA paths are enabled:

- Arduino IDE OTA: pick the network port named `bedroom-leds` and use the password from `OTA_PASS`.
- Browser OTA: open `/ota`, then use the Web Updater link at `/update`.

The browser updater uses username `admin` and the password from `OTA_PASS`.

## Electrical Notes

D3 on a D1 mini is GPIO0, which is also a boot-mode pin. It can work for WS2812 data, but if the strip or wiring pulls that pin low while the board boots, the D1 mini may enter flash mode instead of running normally. If booting becomes unreliable, move the data line to D4 or D2 and update `LED_PIN`.

Long WS2812 strips can draw a lot of current, especially at white. A 6 ft strip could be around 55 LEDs if it is 30 LEDs/m, or around 110 LEDs if it is 60 LEDs/m. Set `LED_COUNT` correctly, use a 5 V supply sized for the strip, connect all grounds together, and consider power injection for long or dense strips.

## Web Page Controls

### Color Wheel

The color wheel sets the active color used by solid mode and most animations. When you change the color while an animation is running, the animation keeps running and shifts to the new color. Rainbow is the one exception because it intentionally overrides the active color.

### White Temperature Slider

The vertical slider sets the strip to white from 2700K to 6500K. Its background shows the full warm-to-cool gradient. Moving the slider also updates the active color, so animations can run in warm white, neutral white, or cool white.

### Animation Dropdown

The animation dropdown selects the current mode. Extra settings appear only when the selected mode needs them.

## Included Modes

### Solid Color

Solid Color fills the entire strip with the latest color or white-temperature selection. This is the default mode and the calmest everyday option.

### Strobe

Strobe turns the whole strip fully on, then fully off, repeatedly. The strobe delay dropdown controls both the on time and off time, from 0.1 seconds to 10 seconds.

### Flash

Flash is similar to Strobe, but the on time is fixed at 0.5 seconds. The dropdown controls only the off time, from 0.1 seconds to 10 seconds. This makes the flashes feel like short signals rather than a symmetrical blink.

### Chase

Chase creates a sharper moving wave through the strip using the active color. Pixel brightness ranges from 0 to 255, so the darkest points can go fully black and the crest can reach full brightness. The chase loop dropdown controls how long the wave takes to complete a full pass, from 1 to 15 seconds.

### Wave

Wave is a softer version of Chase. The strip never drops below a brightness value of 40, and the brightest part reaches 255. It uses multiple sine layers so the movement is smooth and less mechanical. The wave loop dropdown controls the speed from 5 to 30 seconds.

### Slow Wave

Slow Wave is the more dramatic wave mode. It layers multiple waves with their own direction, width, life span, and timing. Each wave begins gently, accelerates as it builds brightness, reaches a brighter crest, then enters a crash phase where the crest widens and fades. The result is meant to feel like a side view of beach waves rather than a simple repeating sine wave.

### Slow Pulse

Slow Pulse creates random pulsing sections along the strip. Each pulse has a center, a width, a gradual ramp up and down, and a slightly drifting position. Brightness ranges from 40 to 255. The frequency dropdown controls how many pulses are active at once, from 1 to 15. The speed dropdown controls the maximum life span of each pulse, from 1 to 60 seconds.

The code also tries to keep per-pixel brightness values unique when `LED_COUNT` is 216 or lower. That keeps the strip from looking like repeated blocks of identical brightness.

### Rainbow

Rainbow ignores the active color and sends a gliding rainbow down the strip. The speed dropdown controls the full rainbow loop time from 1 to 30 seconds, where 1 second is very fast and 30 seconds is very slow.

## Added Calm Modes

### Dawn Bloom

Dawn Bloom is a sunrise-style mode built from warm white and the active color. A soft glow slowly sweeps across the strip over a long cycle, while a faint breathing layer keeps the light from feeling static. It is meant for morning, quiet evening light, or any time you want the strip to feel warm without becoming busy.

### Moon Tide

Moon Tide is a gentle layered tide pattern. It blends the active color with a subtle cool moonlight tint, then runs a slow low wave and a lighter secondary ripple across the strip. It is calm enough to leave on for long periods but still has visible movement when you glance at it.

### Velvet Aurora

Velvet Aurora uses two soft color veils around the active color: one cooler, one warmer. The veils move at different speeds and blend into each other, creating a polished ribbon effect. It is more sophisticated than a simple wave because brightness and color temperature both shift slowly.

### Lantern Drift

Lantern Drift creates wide, floating light pools that drift through the strip at different speeds. The active color is warmed slightly so the result feels like distant paper lanterns. The motion is intentionally slow, with soft overlap between layers.

### Rain Glass

Rain Glass creates small luminous beads with trailing fades, blended with a clean glassy tint. It is not a storm effect or a harsh sparkle pattern. It is designed to feel like quiet rain on a window: gentle vertical movement, soft trails, and a low ambient base.

## Five Future Ideas

### 1. Bedtime Fade Scheduler

Add a scheduling page where you can set a bedtime fade and a morning fade. At night, the strip could slowly step down from the current color to a very warm 2200K-style amber, then fade out completely over 20 to 60 minutes. In the morning, it could gradually rise from dark to warm white, then shift toward 5000K daylight.

This would need either NTP time sync or manual time entry after boot. The web UI could include weekday toggles, start time, fade length, target brightness, and a skip button for nights when you want manual control.

### 2. Physical Wall Control

Add a small rotary encoder or capacitive touch button near the bed. A single press could toggle the lights. Rotation could adjust brightness. A long press could cycle favorite modes. A double press could jump to a dim warm-white night setting.

This would make the project feel more finished because you would not need your phone for basic control. The web UI would still be the deep-control surface, while the physical control handles quick everyday actions.

### 3. Saved Scenes and Favorites

Add a scene system stored in ESP8266 flash using LittleFS or EEPROM. A scene could save the current color, white temperature, animation, and all animation settings. The web page could show a compact row of favorite scene buttons such as "Reading", "Evening", "Movie", "Wake", and "Night".

This would be especially useful once you discover the combinations you actually like. Instead of adjusting color, mode, and speed every time, you would tap one scene and get the whole mood back immediately.

### 4. Music-Reactive Quiet Mode

Add an optional microphone module and a calm audio-reactive mode. Instead of the usual aggressive music visualizer look, the strip could respond only to slow energy changes: bass gently expands wave width, mids adjust brightness, and silence slowly returns to the chosen base color.

The important design rule would be restraint. It should feel like the room is breathing with the music, not flashing at you. The web UI could include sensitivity, smoothing, and a maximum brightness limit.

### 5. Room-Aware Auto Brightness

Add a small light sensor so the LEDs automatically adapt to the room. During bright daytime, the strip could run brighter and cooler. In the evening, it could slowly warm and dim. In darkness, it could cap itself to a gentle night-safe brightness.

This would make the lights easier to leave on all day because they would stop feeling too dim in daylight or too intense at night. The web UI could expose three calibration points: daylight brightness, evening brightness, and night brightness.
