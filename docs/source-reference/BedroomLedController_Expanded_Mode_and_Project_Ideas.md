# BedroomLedController Expanded Mode and Project Ideas

This document expands the bedroom LED project with a practical power estimate, 15 additional calm lighting mode concepts, and 10 software-only project upgrades that do not require adding any new hardware.

## Power Needed for 60 WS2812 LEDs

WS2812 LEDs are usually estimated at up to 60 mA per LED when red, green, and blue are all at full brightness. That full-white estimate is the important worst case.

For 60 LEDs:

- Maximum current: `60 LEDs x 0.06 A = 3.6 A`
- Maximum LED power: `5 V x 3.6 A = 18 W`
- D1 mini overhead: roughly 80 mA to 200 mA depending on Wi-Fi activity
- Practical safe supply size: 5 V, 4 A minimum
- Better supply size: 5 V, 5 A

The sketch currently has `MASTER_BRIGHTNESS` set to `180`, which is about 70% of full brightness. If every LED were forced to full white at that cap, the LED strip would be closer to:

- Capped current estimate: `3.6 A x 180 / 255 = about 2.54 A`
- With D1 mini overhead: about 2.7 A

So a 5 V, 3 A supply may work with the current brightness cap and normal animations, but a 5 V, 5 A supply is the calmer choice. It gives you headroom for white light, future settings changes, voltage drop, and startup surges.

Do not power a 60 LED strip through the D1 mini's 5 V pin from USB. Feed the LED strip from a proper 5 V supply, connect the D1 mini ground and LED strip ground together, and keep the data wire short if possible. If the far end of the strip looks warmer, dimmer, or pinkish when white is selected, that is usually voltage drop and means the strip wants better power wiring or power injection.

## Additional Calm Lighting Mode Ideas

These are not yet implemented in the sketch. They are written as design concepts for future modes, with enough detail to guide the next coding pass.

### 1. Window Sunrise

Window Sunrise would simulate morning light spilling in from one side of the room. Unlike Dawn Bloom, which is a smooth abstract sunrise, this mode would feel more architectural, as if one edge of the strip is closest to a window and the rest of the wall slowly catches the light.

The animation would begin with a very dim amber glow on one end of the strip. Over several minutes, that glow would widen, brighten, and become less orange. The far end would stay dim longer, then gradually catch up. A tiny amount of soft shimmer could be layered into the bright side, but it should stay almost invisible unless you stare at it.

Suggested controls:

- Direction: left-to-right or right-to-left
- Duration: 2, 5, 10, 20, or 30 minutes
- Final temperature: warm, neutral, or daylight
- Final brightness cap: 80, 120, 180, or 255

This would be excellent as a wake-up scene or as a slow morning ambience while getting ready.

### 2. Ember Quilt

Ember Quilt would create the feeling of warmth under fabric, like soft glowing patches underneath a heavy blanket. It would not look like fire. Instead, it would use wide, low-contrast islands of warm brightness that slowly expand and fade.

The strip would be divided into overlapping zones. Each zone would have a slightly different warm tint, somewhere between amber, peach, and candle white. The zones would breathe independently, but with very slow ramps so the eye reads it as comfort rather than animation.

Suggested controls:

- Warmth: amber, peach, candle, or current color
- Texture depth: subtle, medium, rich
- Drift speed: 15, 30, 60, or 120 seconds
- Minimum brightness: 10 to 80

This mode would be especially good in the evening because it can stay warm and cozy without creating hard motion.

### 3. Porcelain Sky

Porcelain Sky would be a pale, refined mode for daytime. It would blend the chosen color with cool white and create a nearly static, polished gradient across the strip. Movement would be extremely slow, almost like clouds shifting behind frosted glass.

Instead of obvious waves, it would use two or three very long sine layers with slightly different speeds. The overall brightness would stay stable so the room does not pulse. The main visible effect would be a gentle change in color temperature from one side to the other.

Suggested controls:

- Tint strength: low, medium, high
- Cloud speed: 30, 60, 120, or 240 seconds
- Brightness: fixed or softly breathing
- Palette: current color, cool white, pale blue, pale gold

This would be a good "leave on all day" mode because it looks intentional without asking for attention.

### 4. Hearthline

Hearthline would be a calm horizontal warmth mode that suggests a fireplace from far away without becoming flickery. The core idea is a low amber base with a few slow heat bands that gently rise and dissolve along the strip.

Because this is a side strip rather than a matrix, the illusion would come from timing and brightness. Warm bands would appear near one side, stretch wider, then fade while a second slower band begins behind it. The brightness range should stay restrained to avoid a cheap flame effect.

Suggested controls:

- Heat level: low, cozy, deep
- Band width: narrow, medium, wide
- Drift speed: 10 to 90 seconds
- Color mix: amber only, amber plus selected color, candle white

This would be ideal for movie nights, late evenings, or any setting where a warm but non-distracting ambience is wanted.

### 5. Evening Current

Evening Current would build on the wave idea but make it feel smoother and more sophisticated. It would use very long overlapping currents, with no sharp crest and no full black valleys. The strip would feel like a slow river of light moving beside the bed.

The current could be built from three layers: a base brightness floor, a broad moving swell, and a smaller counter-moving ripple. The chosen color would remain dominant. Only brightness would move, with a tiny color temperature shift at the brightest points.

Suggested controls:

- Current speed: 20, 45, 90, or 180 seconds
- Depth: shallow, medium, deep
- Direction: left, right, or alternating
- Brightness floor: 20 to 100

This mode would be relaxing because it has motion without sparkle, flashing, or sudden change.

### 6. Satin Breathing

Satin Breathing would be a whole-strip breathing mode, but more refined than a normal pulse. The strip would brighten and dim as one piece, while tiny per-pixel differences keep it from looking flat.

The breathing curve should be asymmetric: a slow inhale, a gentle hold, and a longer exhale. This makes the pattern feel human and calm. During the hold, the color could subtly warm or cool by a few percent.

Suggested controls:

- Breath length: 6, 10, 20, 40, or 60 seconds
- Hold time: none, short, long
- Texture: clean, soft grain, silk grain
- Night cap: optional lower max brightness

This would be one of the best modes for bedtime because the timing can be slow enough to feel meditative.

### 7. Starlit Linen

Starlit Linen would be a low-brightness texture mode with tiny, soft bright points under a steady base. It should avoid the harsh "twinkle lights" look. Each bright point would fade in slowly, stay dim, then fade out slowly.

The background would use the active color at low brightness. The tiny highlights would be warmer or cooler depending on the selected palette. Each highlight would affect two or three neighboring pixels, so no pixel pops on alone.

Suggested controls:

- Density: sparse, balanced, full
- Highlight warmth: warm, neutral, cool
- Fade length: 5, 15, 30, or 60 seconds
- Maximum sparkle brightness: 80 to 180

This would be a good night mode if the brightness is capped low, or a relaxed evening mode at medium brightness.

### 8. Deep Sea Glass

Deep Sea Glass would be a cool, quiet mode using blue-green or the current color blended with deep cyan. It would create the impression of light passing through thick water or glass.

The movement would be slow and layered. One layer would be a broad dim swell. A second layer would create a faint caustic shimmer, but blurred enough that it never becomes glittery. Brightness should stay mostly low to medium, with rare soft peaks.

Suggested controls:

- Depth: shallow, deep, midnight
- Shimmer amount: none, low, medium
- Drift speed: 30, 60, 120 seconds
- Palette: current color, teal, blue, moonlit cyan

This would pair nicely with the existing Rain Glass and Moon Tide ideas, but it would feel more submerged and smooth.

### 9. Paper Lantern Row

Paper Lantern Row would make the strip feel like a line of lanterns behind translucent paper. Instead of each LED being visible as a point, the code would create larger glowing blobs with very soft edges.

The lanterns would slowly change size and brightness independently. They would not move much; the sophistication comes from the overlapping glow and the warm color mixing. A few lantern centers could drift by one or two pixels over time.

Suggested controls:

- Lantern count: 3 to 12
- Paper warmth: cream, amber, peach, selected color
- Sway amount: none, slight, gentle
- Brightness: low, medium, full

This would be excellent for daytime or evening because it feels designed and decorative rather than technical.

### 10. Quiet Northern Lights

Quiet Northern Lights would be a calmer cousin of Velvet Aurora. It would use soft vertical ribbons of color that appear to fold through the strip. The key difference is that the ribbons would sometimes pause, reverse, and dissolve instead of simply sliding forever.

Because the strip is one-dimensional, the effect would be represented as moving bands with different widths and colors. The active color would be the anchor, while the secondary colors would be derived from it to avoid an overly saturated rainbow look.

Suggested controls:

- Ribbon count: 2, 3, 5, or 8
- Color spread: subtle, balanced, colorful
- Fold speed: 20, 45, 90, or 180 seconds
- Brightness cap: 80 to 255

This mode would look sophisticated in a dark room without becoming as energetic as Rainbow.

### 11. Candle Hall

Candle Hall would mimic a row of distant candles down a hallway, but in a restrained way. Each "candle" would occupy a small cluster of LEDs. The clusters would flicker slowly and irregularly, but with smoothing so they never snap.

The base color would be warm white or amber. The current color could influence the shadow side of each cluster, giving it more personality. Flicker would be limited to a small brightness range so it remains relaxing.

Suggested controls:

- Candle count: 4 to 16
- Flicker softness: very soft, soft, lively
- Base warmth: 2200K, 2400K, 2700K, selected color
- Shadow level: low, medium, deep

This would be a cozy mode for nighttime that gives movement without reading as a warning light or party effect.

### 12. Slow Prism

Slow Prism would take the current color and split it into nearby companion colors. For example, a blue choice might create blue, cyan, and a little violet. A warm white choice might create peach, gold, and cream.

The strip would show a wide gradient made from these companion colors. Over time, the gradient would rotate slowly through the strip. It should never use a full rainbow unless the selected color already invites that; the point is tasteful color separation, not maximum saturation.

Suggested controls:

- Spread: narrow, medium, wide
- Rotation speed: 30, 60, 180, or 300 seconds
- Contrast: low, medium, high
- Anchor strength: how much the selected color dominates

This would make single-color choices feel richer without requiring the user to manually choose a palette.

### 13. Garden Shade

Garden Shade would be a soft green-tinted mode for daytime relaxation. It would make the light feel like sun filtered through leaves, but without high-contrast leaf flicker.

The algorithm would combine a warm base with muted green-gold bands that drift at different speeds. The current color could tint the warm base, while the shade layer remains subtle. Brightness changes should be very slow.

Suggested controls:

- Shade tint: olive, sage, gold-green, current color blend
- Breeze speed: still, slow, gentle
- Sun amount: low, balanced, bright
- Contrast: soft or defined

This would be ideal for afternoons because it can feel fresh and natural without turning the room neon green.

### 14. Snowfall Hush

Snowfall Hush would be a cool white and pale blue mode where small soft lights drift slowly across the strip and disappear. It should feel quiet and muffled, not like blinking holiday lights.

Each snow point would be a tiny cluster with a long fade. Some would drift left, some right, and some would barely move. The background would remain low and cool. The mode could also work beautifully with warm white by turning into a soft dust-mote effect.

Suggested controls:

- Snow density: sparse, light, full
- Drift speed: 20, 45, 90 seconds
- Background temperature: 2700K to 6500K
- Highlight brightness: 60 to 180

This would be a calm winter-style mode, but subtle enough to use any time.

### 15. Observatory Glow

Observatory Glow would use very slow celestial movement: a dark blue or selected-color base, one broad moonlike glow, and several almost invisible star gradients. It would be more atmospheric than Starlit Linen and more structured than Moon Tide.

The main glow would travel so slowly that a full pass might take several minutes. The secondary points would gently brighten when the main glow passes near them, creating a sense of depth. It should avoid full-black darkness unless the user chooses a very low brightness setting.

Suggested controls:

- Main glow speed: 2, 5, 10, or 20 minutes
- Star density: none, sparse, soft
- Color mood: moon, dusk, selected color
- Minimum brightness: 5 to 80

This would be a beautiful late-night mode for leaving on in the background while the room is otherwise dark.

## Software-Only Project Ideas

These ideas do not require adding sensors, buttons, microphones, or extra hardware. They build on the D1 mini, the existing LED strip, OTA, and local web page.

### 1. Scene Presets With Save and Recall

Add a scene preset system where the web page can save the current color, white temperature, animation mode, speed settings, and brightness cap under a friendly name. Example presets could be Reading, Sleep, Movie, Morning, Stormy Evening, and Focus.

The ESP8266 can store these scenes in LittleFS or EEPROM. The web UI could show each scene as a button with a small color preview and a compact label. Saving a scene should be intentionally simple: set the lights how you like them, tap Save Scene, enter a name, and the controller stores the exact state.

This would make the controller feel much more like a polished room device. You would stop rebuilding favorite moods from scratch and start treating the strip as a set of remembered atmospheres.

### 2. Brightness Master Control

Add a master brightness slider to the web page. This would be separate from color and animation brightness. The selected mode could still animate from dim to bright internally, but the master brightness would cap the final output.

This is especially useful because the right brightness changes dramatically between day and night. A mode that looks beautiful in the afternoon can be too intense after midnight. A master slider also improves power safety because you can keep the whole strip under a known maximum without editing the code.

Implementation idea: store `masterBrightness` as a runtime setting and apply it to every rendered pixel before calling `strip.show()`.

### 3. Favorite Mode Bar

Add a small horizontal favorite bar at the top of the web UI. It could hold 4 to 8 quick buttons for the modes you actually use most. For example: Solid, Warm White, Moon Tide, Lantern Drift, Rain Glass, Slow Pulse.

This would keep the full dropdown for deep control while making the everyday page faster. The favorite buttons could be configurable from a settings page, or they could be hardcoded at first. Each button could also remember its last-used settings, so tapping Moon Tide restores the last Moon Tide speed and color.

This is a pure usability upgrade, but it would make the project feel much more complete.

### 4. Smooth Mode Transitions

Right now, when you select a different mode, the strip can immediately jump into the new pattern. Smooth transitions would crossfade from the current rendered frame into the new mode over a short time.

For example, switching from Solid Color to Rain Glass could slowly introduce the rain beads over 1.5 seconds. Switching from Rainbow to Warm White could fade the rainbow down while fading the white up. This makes the controller feel expensive and intentional.

Suggested controls:

- Transition duration: instant, 0.5 seconds, 1.5 seconds, 3 seconds
- Transition style: fade, wipe, dissolve
- Per-mode override: calm modes could default to longer transitions

This would require the code to keep a copy of the previous LED frame and blend it with the new frame during the transition.

### 5. Color Palette Builder

Add a palette page where you can choose two to five colors and let compatible animations use that palette instead of a single active color. Calm modes like Velvet Aurora, Slow Prism, Paper Lantern Row, and Deep Sea Glass would benefit from this.

The page could include a few built-in palettes such as Candle, Ocean, Garden, Moonlight, Peach Sky, and Soft Neon. It could also let you create a custom palette with color pickers. Each palette color should have an optional weight so one color can dominate while others appear as accents.

This would dramatically increase the range of looks without needing new hardware or a different LED library.

### 6. Scheduled Local Routines

Add a local schedule system that runs scenes at certain times. Since the D1 mini has Wi-Fi, it can sync time using NTP. Once it knows the current time, it can run routines like Morning Warm White at 7:00 AM, Evening Current at sunset-like hours, and Night Dim at 11:30 PM.

The UI could include a simple weekly schedule:

- Time
- Days of week
- Scene or mode
- Fade duration
- Enabled or disabled

This would make the strip feel integrated into the room instead of manually controlled every time.

### 7. Temporary Timer Actions

Add timer buttons for temporary behavior: 15 minutes, 30 minutes, 1 hour, 2 hours, and until morning. A timer could either turn the strip off at the end or fade to a selected scene.

Example uses:

- Start Slow Pulse for 30 minutes while winding down
- Fade from warm white to off over 1 hour
- Keep a dim night glow until morning
- Run a focus scene for 45 minutes

The key is that timers should be visible and cancellable from the main page. The UI should clearly say what will happen and when.

### 8. Web UI Theme and Layout Settings

Add a small settings page for the controller interface itself. You could choose between a compact phone layout, a larger tablet layout, and a dark or light UI theme. Since this is a room controller you may open at night, the default should stay dark and low-glare.

Useful software-only UI settings:

- Compact controls mode
- Larger touch targets mode
- Hide advanced animation controls until expanded
- Reduce motion in the web UI
- Show or hide the OTA link from the main page

This does not change the LEDs, but it makes the controller easier to live with.

### 9. Live Preview Strip in the Browser

Add a miniature preview strip to the web page that shows a simplified version of the active animation. It would not need to be perfectly identical to the LEDs, but it could show the selected color, rough movement direction, speed, and brightness envelope.

This would be useful when you are away from the room or when the strip is behind you. It also helps when adjusting speeds because the UI gives immediate visual feedback. The preview could be drawn with small HTML blocks or a lightweight canvas.

To keep the D1 mini comfortable, the animation preview should run in the browser, not be streamed from the device. The ESP8266 would only send current state, and JavaScript would draw the approximation locally.

### 10. Diagnostics and Power-Safe Page

Add a diagnostics page that explains the current state of the controller and helps prevent power mistakes. It could show LED count, master brightness, estimated maximum current, active mode, Wi-Fi signal strength, uptime, free heap memory, and OTA status.

The power estimate could update live:

- Full theoretical current at 255 brightness
- Estimated capped current at current master brightness
- Estimated current for solid white
- A warning if LED count and brightness suggest a supply above the configured safe limit

This would turn the project into something easier to maintain. If the strip flickers or resets, the diagnostics page would give you a first place to look before changing wiring or code.

## Suggested Next Implementation Order

If you decide to add some of these later, the best software-only order would be:

1. Master brightness control, because it improves power safety and comfort.
2. Scene presets, because they make every mode more useful.
3. Smooth mode transitions, because they make the whole controller feel better.
4. Diagnostics and power-safe page, because it helps with setup and troubleshooting.
5. One or two new calm modes, starting with Satin Breathing and Paper Lantern Row.

The best new lighting mode to implement first would probably be Satin Breathing. It is simple enough to fit well on the ESP8266, it would be useful every night, and it would establish the final brightness-scaling pattern needed for many of the more complex modes.
