#ifndef BEDROOM_LED_CONTROLLER_LED_RENDERING_H
#define BEDROOM_LED_CONTROLLER_LED_RENDERING_H

const uint8_t GAMMA_2_2[256] PROGMEM = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,
    3,   3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   6,   6,   6,
    6,   7,   7,   7,   8,   8,   8,   9,   9,   9,  10,  10,  11,  11,  11,  12,
   12,  13,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,
   20,  20,  21,  22,  22,  23,  23,  24,  25,  25,  26,  26,  27,  28,  28,  29,
   30,  30,  31,  32,  33,  33,  34,  35,  35,  36,  37,  38,  39,  39,  40,  41,
   42,  43,  43,  44,  45,  46,  47,  48,  49,  49,  50,  51,  52,  53,  54,  55,
   56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,
   73,  74,  75,  76,  77,  78,  79,  81,  82,  83,  84,  85,  87,  88,  89,  90,
   91,  93,  94,  95,  97,  98,  99, 100, 102, 103, 105, 106, 107, 109, 110, 111,
  113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 127, 129, 130, 132, 133, 135,
  137, 138, 140, 141, 143, 145, 146, 148, 149, 151, 153, 154, 156, 158, 159, 161,
  163, 165, 166, 168, 170, 172, 173, 175, 177, 179, 181, 182, 184, 186, 188, 190,
  192, 194, 196, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221,
  223, 225, 227, 229, 231, 234, 236, 238, 240, 242, 244, 246, 248, 251, 253, 255
};

void applyMasterBrightness() {
  ledsDirty = true;
}

uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b) {
  return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | b;
}

void clearFrame() {
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    frameBuffer[i] = {0, 0, 0};
  }
}

void setFramePixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index >= LED_COUNT) {
    return;
  }
  frameBuffer[index] = {r, g, b};
}

void setFramePixelColor(uint16_t index, uint32_t color) {
  setFramePixel(
    index,
    static_cast<uint8_t>((color >> 16) & 0xFF),
    static_cast<uint8_t>((color >> 8) & 0xFF),
    static_cast<uint8_t>(color & 0xFF)
  );
}

void fillFrame(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    frameBuffer[i] = {r, g, b};
  }
}

void fillFrameColor(uint32_t color) {
  fillFrame(
    static_cast<uint8_t>((color >> 16) & 0xFF),
    static_cast<uint8_t>((color >> 8) & 0xFF),
    static_cast<uint8_t>(color & 0xFF)
  );
}

uint8_t applyChannelGain(uint8_t channelValue, uint8_t gain) {
  return static_cast<uint8_t>((static_cast<uint16_t>(channelValue) * gain) / 255U);
}

uint8_t scaleByMasterBrightness(uint8_t channelValue) {
  return static_cast<uint8_t>((static_cast<uint16_t>(channelValue) * getEffectiveBrightness()) / 255U);
}

uint8_t gamma8(uint8_t value) {
  return pgm_read_byte(&GAMMA_2_2[value]);
}

uint8_t lerp8(uint8_t a, uint8_t b, uint8_t amount);

uint8_t clamp8(int value) {
  if (value <= 0) return 0;
  if (value >= 255) return 255;
  return static_cast<uint8_t>(value);
}

RgbPixel blendRgb(RgbPixel a, RgbPixel b, uint8_t amount) {
  return {
    lerp8(a.r, b.r, amount),
    lerp8(a.g, b.g, amount),
    lerp8(a.b, b.b, amount)
  };
}

RgbPixel scaleRgb(RgbPixel color, uint8_t scale) {
  return {
    static_cast<uint8_t>((static_cast<uint16_t>(color.r) * scale) / 255U),
    static_cast<uint8_t>((static_cast<uint16_t>(color.g) * scale) / 255U),
    static_cast<uint8_t>((static_cast<uint16_t>(color.b) * scale) / 255U)
  };
}

RgbPixel warmWhiteApprox(uint16_t kelvin) {
  Rgb color = kelvinToRgb(kelvin);
  return {color.r, color.g, color.b};
}

RgbPixel coolWhiteApprox(uint16_t kelvin) {
  Rgb color = kelvinToRgb(kelvin);
  Rgb cool = {210, 232, 255};
  Rgb blended = blendColor(color, cool, 0.18f);
  return {blended.r, blended.g, blended.b};
}

RgbPixel sampleActivePaletteOrColor(uint16_t position, uint16_t maxPosition) {
  if (settings.paletteEnabled && currentModeSupportsPalette() && getActivePalette() != nullptr) {
    return samplePalette(position, maxPosition);
  }
  return {settings.color.r, settings.color.g, settings.color.b};
}

RgbPixel sampleNamedOrActivePalette(const char* fallbackPaletteId, uint16_t position, uint16_t maxPosition) {
  if (settings.paletteEnabled && currentModeSupportsPalette() && getActivePalette() != nullptr) {
    return samplePalette(position, maxPosition);
  }
  int paletteIndex = findPaletteByIdConst(fallbackPaletteId);
  if (paletteIndex >= 0) {
    ColorPalette& palette = palettes[paletteIndex];
    if (palette.colorCount == 0) {
      return {settings.color.r, settings.color.g, settings.color.b};
    }
    uint16_t safeMax = maxPosition == 0 ? 1 : maxPosition;
    uint32_t scaled = (static_cast<uint32_t>(position) * 255UL) / safeMax;
    uint8_t selector = static_cast<uint8_t>(scaled & 0xFF);
    uint8_t weightedTotal = 0;
    for (uint8_t i = 0; i < palette.colorCount; i++) {
      weightedTotal += max(static_cast<uint8_t>(1), palette.colors[i].weight);
    }
    if (weightedTotal == 0) {
      weightedTotal = palette.colorCount;
    }
    uint8_t slot = static_cast<uint8_t>((static_cast<uint16_t>(selector) * weightedTotal) / 255U);
    uint8_t running = 0;
    for (uint8_t i = 0; i < palette.colorCount; i++) {
      running += max(static_cast<uint8_t>(1), palette.colors[i].weight);
      if (slot < running) {
        return {palette.colors[i].r, palette.colors[i].g, palette.colors[i].b};
      }
    }
    return {palette.colors[palette.colorCount - 1].r, palette.colors[palette.colorCount - 1].g, palette.colors[palette.colorCount - 1].b};
  }
  return {settings.color.r, settings.color.g, settings.color.b};
}

float phaseFromMillis(uint32_t now, uint32_t periodMs) {
  if (periodMs == 0) {
    return 0.0f;
  }
  return static_cast<float>(now % periodMs) / static_cast<float>(periodMs);
}

float easeInOut01(float t) {
  float x = clamp01(t);
  return x * x * (3.0f - 2.0f * x);
}

uint8_t sineEase8(float phase) {
  float wave = (sinf((phase * TWO_PI) - (PI * 0.5f)) + 1.0f) * 0.5f;
  return clampByte(wave * 255.0f);
}

uint8_t triangleWave8(float phase) {
  float wrapped = phase - floorf(phase);
  float value = wrapped < 0.5f ? wrapped * 2.0f : (1.0f - wrapped) * 2.0f;
  return clampByte(value * 255.0f);
}

uint8_t smoothStep8(uint8_t t) {
  float x = static_cast<float>(t) / 255.0f;
  return clampByte((x * x * (3.0f - 2.0f * x)) * 255.0f);
}

uint8_t triangleWave8(uint32_t now, uint32_t periodMs) {
  return triangleWave8(phaseFromMillis(now, periodMs));
}

uint8_t sineApprox8(uint32_t now, uint32_t periodMs, uint16_t phaseOffset) {
  float phase = phaseFromMillis(now, periodMs) + static_cast<float>(phaseOffset) / 65535.0f;
  return clampByte((sinf(phase * TWO_PI) + 1.0f) * 127.5f);
}

uint8_t softPulseEnvelope8(uint32_t now, uint32_t periodMs, uint16_t phaseOffset) {
  return smoothStep8(sineApprox8(now, periodMs, phaseOffset));
}

uint8_t hash8(uint16_t x) {
  x ^= x >> 8;
  x *= 37U;
  x ^= x >> 4;
  x *= 17U;
  return static_cast<uint8_t>(x & 0xFF);
}

uint8_t smoothHash8(uint16_t seed, uint32_t now, uint16_t periodMs) {
  if (periodMs == 0) {
    return hash8(seed);
  }
  uint32_t bucket = now / periodMs;
  uint8_t progress = static_cast<uint8_t>(((now % periodMs) * 255UL) / periodMs);
  uint8_t eased = smoothStep8(progress);
  uint8_t a = hash8(seed + static_cast<uint16_t>(bucket & 0xFFFF));
  uint8_t b = hash8(seed + static_cast<uint16_t>((bucket + 1UL) & 0xFFFF));
  return lerp8(a, b, eased);
}

uint8_t approach8(uint8_t current, uint8_t target, uint8_t maxStep) {
  if (maxStep == 0 || current == target) {
    return target;
  }
  if (current < target) {
    uint16_t next = static_cast<uint16_t>(current) + maxStep;
    return next > target ? target : static_cast<uint8_t>(next);
  }
  int16_t next = static_cast<int16_t>(current) - maxStep;
  return next < target ? target : static_cast<uint8_t>(next);
}

bool shouldApplyTemporalSmoothing(Mode mode) {
  if (!ENABLE_MOTION_SMOOTHING || diagnosticTest.active || transitionState.active) {
    return false;
  }
  if (mode == MODE_SOLID || mode == MODE_STROBE || mode == MODE_FLASH) {
    return false;
  }
  const ModeMetadata* metadata = findModeMetadata(mode);
  if (metadata != nullptr && (metadata->flashing || metadata->utility)) {
    return false;
  }
  return true;
}

uint8_t smoothingStepForMode(Mode mode) {
  const ModeMetadata* metadata = findModeMetadata(mode);
  if (metadata == nullptr) {
    return DEFAULT_MOTION_SMOOTHING;
  }
  if (metadata->sleepSafe || metadata->calm) {
    return DEFAULT_CALM_SMOOTHING;
  }
  if (metadata->category != nullptr && strcmp(metadata->category, "Weather") == 0) {
    return DEFAULT_WEATHER_SMOOTHING;
  }
  if (metadata->colorful) {
    return DEFAULT_COLORFUL_SMOOTHING;
  }
  return DEFAULT_MOTION_SMOOTHING;
}

void resetTemporalSmoothing() {
  smoothedFrameInitialized = false;
}

void applyTemporalSmoothingToFrame() {
  uint8_t step = smoothingStepForMode(settings.mode);
  if (!smoothedFrameInitialized) {
    copyFrame(frameBuffer, smoothedFrame);
    smoothedFrameInitialized = true;
    return;
  }
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    smoothedFrame[i].r = approach8(smoothedFrame[i].r, frameBuffer[i].r, step);
    smoothedFrame[i].g = approach8(smoothedFrame[i].g, frameBuffer[i].g, step);
    smoothedFrame[i].b = approach8(smoothedFrame[i].b, frameBuffer[i].b, step);
    frameBuffer[i] = smoothedFrame[i];
  }
}

void setFramePixelRgb(uint16_t index, RgbPixel color) {
  setFramePixel(index, color.r, color.g, color.b);
}

void outputFrameToStrip() {
  uint8_t effectiveBrightness = getEffectiveBrightness();
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    uint8_t r = frameBuffer[i].r;
    uint8_t g = frameBuffer[i].g;
    uint8_t b = frameBuffer[i].b;

    r = applyChannelGain(r, settings.redGain);
    g = applyChannelGain(g, settings.greenGain);
    b = applyChannelGain(b, settings.blueGain);

    r = scaleByMasterBrightness(r);
    g = scaleByMasterBrightness(g);
    b = scaleByMasterBrightness(b);

    if (settings.gammaEnabled) {
      r = gamma8(r);
      g = gamma8(g);
      b = gamma8(b);
    }

    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
  lastOutputEffectiveBrightness = effectiveBrightness;
}

void resetAnimationState() {
  uint32_t now = millis();
  strobeOn = true;
  strobeLastToggleMs = now;
  flashOn = true;
  flashLastToggleMs = now;

  for (uint8_t i = 0; i < MAX_PULSES; i++) {
    initPulse(i, now - random(0, 1000));
  }
  for (uint8_t i = 0; i < 4; i++) {
    initSlowWave(i, now - random(0, 4000));
  }
}

void updateLeds() {
  uint32_t now = millis();
  bool animatedMode = diagnosticTest.active || transitionState.active || settings.mode != MODE_SOLID;

  if (!animatedMode && !ledsDirty) {
    return;
  }

  if (animatedMode && now - lastFrameMs < 33 && !ledsDirty) {
    return;
  }

  if (animatedMode) {
    lastFrameMs = now;
  }

  if (diagnosticTest.active) {
    renderDiagnosticTestToFrame(now);
  } else if (transitionState.active) {
    renderTransitionFrame(now);
  } else {
    renderCurrentModeToFrame(now);
  }
  if (shouldApplyTemporalSmoothing(settings.mode)) {
    applyTemporalSmoothingToFrame();
  } else {
    smoothedFrameInitialized = false;
  }
  outputFrameToStrip();
  ledsDirty = false;
}

void renderCurrentModeToFrame(uint32_t now) {
  clearFrame();
  switch (settings.mode) {
    case MODE_STROBE: renderStrobe(now); break;
    case MODE_FLASH: renderFlash(now); break;
    case MODE_CHASE: renderChase(now); break;
    case MODE_WAVE: renderWave(now); break;
    case MODE_SLOW_WAVE: renderSlowWave(now); break;
    case MODE_SLOW_PULSE: renderSlowPulse(now); break;
    case MODE_RAINBOW: renderRainbow(now); break;
    case MODE_DAWN_BLOOM: renderDawnBloom(now); break;
    case MODE_MOON_TIDE: renderMoonTide(now); break;
    case MODE_VELVET_AURORA: renderVelvetAurora(now); break;
    case MODE_LANTERN_DRIFT: renderLanternDrift(now); break;
    case MODE_RAIN_GLASS: renderRainGlass(now); break;
    case MODE_SATIN_BREATHING: renderSatinBreathing(now); break;
    case MODE_PAPER_LANTERN_ROW: renderPaperLanternRow(now); break;
    case MODE_LIBRARY_LAMP: renderLibraryLamp(now); break;
    case MODE_SOLAR_NOON: renderSolarNoon(now); break;
    case MODE_GOLDEN_HOUR: renderGoldenHour(now); break;
    case MODE_DREAM_AQUARIUM: renderDreamAquarium(now); break;
    case MODE_CIRCUIT_GLOW: renderCircuitGlow(now); break;
    case MODE_WINDOW_SUNRISE: renderWindowSunrise(now); break;
    case MODE_EMBER_QUILT: renderEmberQuilt(now); break;
    case MODE_PORCELAIN_SKY: renderPorcelainSky(now); break;
    case MODE_HEARTHLINE: renderHearthline(now); break;
    case MODE_EVENING_CURRENT: renderEveningCurrent(now); break;
    case MODE_STARLIT_LINEN: renderStarlitLinen(now); break;
    case MODE_DEEP_SEA_GLASS: renderDeepSeaGlass(now); break;
    case MODE_QUIET_NORTHERN_LIGHTS: renderQuietNorthernLights(now); break;
    case MODE_CANDLE_HALL: renderCandleHall(now); break;
    case MODE_SLOW_PRISM: renderSlowPrism(now); break;
    case MODE_GARDEN_SHADE: renderGardenShade(now); break;
    case MODE_SNOWFALL_HUSH: renderSnowfallHush(now); break;
    case MODE_OBSERVATORY_GLOW: renderObservatoryGlow(now); break;
    case MODE_RAINY_WINDOW: renderRainyWindow(now); break;
    case MODE_CAMPFIRE_BLANKET: renderCampfireBlanket(now); break;
    case MODE_NORTHERN_WHISPER: renderNorthernWhisper(now); break;
    case MODE_STORM_OUTSIDE: renderStormOutside(now); break;
    case MODE_SOLID:
    default: renderSolid(); break;
  }
}

void renderSolid() {
  fillScaled(settings.color, 255);
}

void renderStrobe(uint32_t now) {
  uint32_t delayMs = static_cast<uint32_t>(settings.strobeDelaySec * 1000.0f);
  if (now - strobeLastToggleMs >= delayMs) {
    strobeOn = !strobeOn;
    strobeLastToggleMs = now;
  }
  if (strobeOn) {
    fillScaled(settings.color, 255);
  } else {
    clearFrame();
  }
}

void renderFlash(uint32_t now) {
  uint32_t activeMs = flashOn ? 500UL : static_cast<uint32_t>(settings.flashOffDelaySec * 1000.0f);
  if (now - flashLastToggleMs >= activeMs) {
    flashOn = !flashOn;
    flashLastToggleMs = now;
  }
  if (flashOn) {
    fillScaled(settings.color, 255);
  } else {
    clearFrame();
  }
}

void renderChase(uint32_t now) {
  float period = (settings.chasePeriodSec < 1 ? 1 : settings.chasePeriodSec) * 1000.0f;
  float phase = fmodf(now, period) / period * TWO_PI;
  const float cycles = 2.25f;
  float denominator = LED_COUNT > 1 ? LED_COUNT - 1 : 1;

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / denominator;
    float wave = (sinf((x * cycles * TWO_PI) - phase) + 1.0f) * 0.5f;
    uint8_t brightness = clampByte(powf(wave, 2.4f) * 255.0f);
    setPixelWithBrightness(i, settings.color, brightness);
  }
}

void renderWave(uint32_t now) {
  float period = (settings.wavePeriodSec < 5 ? 5 : settings.wavePeriodSec) * 1000.0f;
  float phase = fmodf(now, period) / period * TWO_PI;
  float denominator = LED_COUNT > 1 ? LED_COUNT - 1 : 1;

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / denominator;
    float a = (sinf((x * TWO_PI * 1.4f) - phase) + 1.0f) * 0.5f;
    float b = (sinf((x * TWO_PI * 0.65f) + phase * 0.55f) + 1.0f) * 0.5f;
    uint8_t brightness = clampByte(40.0f + ((a * 0.7f + b * 0.3f) * 215.0f));
    setPixelWithBrightness(i, settings.color, brightness);
  }
}

void renderSlowWave(uint32_t now) {
  float brightnessMap[LED_COUNT];
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    brightnessMap[i] = 18.0f;
  }

  for (uint8_t layer = 0; layer < 4; layer++) {
    CrashWave& wave = slowWaves[layer];
    float t = static_cast<float>(now - wave.bornMs) / static_cast<float>(wave.durationMs);

    if (t >= 1.0f) {
      initSlowWave(layer, now);
      t = 0.0f;
    }

    float build = smoothstep(0.0f, 0.72f, t);
    float crash = smoothstep(0.72f, 0.86f, t);
    float fade = 1.0f - smoothstep(0.84f, 1.0f, t);
    float travel = powf(clamp01(t / 0.80f), 2.4f);
    float pos = wave.direction > 0.0f
      ? (-wave.width + travel * (LED_COUNT + wave.width * 2.0f))
      : (LED_COUNT + wave.width - travel * (LED_COUNT + wave.width * 2.0f));
    float energy = (0.25f + build * 0.75f + crash * 0.55f) * fade;
    float foamWidth = wave.width * (1.0f + crash * 1.8f);

    for (uint16_t i = 0; i < LED_COUNT; i++) {
      float d = fabsf(static_cast<float>(i) - pos);
      float crest = expf(-(d * d) / (2.0f * wave.width * wave.width));
      float foam = expf(-(d * d) / (2.0f * foamWidth * foamWidth)) * crash * 0.55f;
      brightnessMap[i] += (crest + foam) * energy * 210.0f;
    }
  }

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    setPixelWithBrightness(i, settings.color, clampByte(brightnessMap[i]));
  }
}

void renderSlowPulse(uint32_t now) {
  uint8_t brightnessMap[LED_COUNT];
  bool used[256] = {false};

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float value = 40.0f;

    for (uint8_t p = 0; p < settings.slowPulseCount; p++) {
      Pulse& pulse = pulses[p];
      float t = static_cast<float>(now - pulse.bornMs) / static_cast<float>(pulse.durationMs);
      if (t >= 1.0f) {
        initPulse(p, now);
        t = 0.0f;
      }

      float centerDrift = sinf((now * 0.00011f) + pulse.seed) * 2.5f;
      float distance = fabsf(static_cast<float>(i) - (pulse.center + centerDrift));
      float spatial = clamp01(1.0f - (distance / pulse.width));
      spatial = spatial * spatial * (3.0f - 2.0f * spatial);
      float temporal = sinf(t * PI);
      value = max(value, 40.0f + spatial * temporal * 215.0f);
    }

    value += (static_cast<float>(smoothHash8(i * 37U, now, 713UL)) / 255.0f) * 3.28f;
    uint8_t candidate = clampByte(value);
    if (candidate < 40) {
      candidate = 40;
    }

    if (LED_COUNT <= 216 && used[candidate]) {
      for (uint16_t offset = 1; offset < 216; offset++) {
        int up = candidate + offset;
        int down = candidate - offset;
        if (up <= 255 && !used[up]) {
          candidate = up;
          break;
        }
        if (down >= 40 && !used[down]) {
          candidate = down;
          break;
        }
      }
    }

    used[candidate] = true;
    brightnessMap[i] = candidate;
  }

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    setPixelWithBrightness(i, settings.color, brightnessMap[i]);
  }
}

void renderRainbow(uint32_t now) {
  uint32_t periodMs = static_cast<uint32_t>(settings.rainbowPeriodSec) * 1000UL;
  uint16_t offset = static_cast<uint16_t>((now % periodMs) * 256UL / periodMs);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    uint8_t wheelPos = static_cast<uint8_t>((i * 256UL / LED_COUNT + offset) & 0xFF);
    Rgb color = wheel(wheelPos);
    setFramePixel(i, color.r, color.g, color.b);
  }
}

void renderDawnBloom(uint32_t now) {
  float period = 72000.0f;
  float sweep = fmodf(now, period) / period;
  Rgb warm = kelvinToRgb(2850);
  Rgb dawn = blendColor(warm, settings.color, 0.55f);
  float denominator = LED_COUNT > 1 ? LED_COUNT - 1 : 1;

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / denominator;
    float local = smoothstep(sweep - 0.35f, sweep + 0.22f, x);
    float breathe = (sinf(now * 0.00028f + x * TWO_PI) + 1.0f) * 0.5f;
    uint8_t brightness = clampByte(35.0f + local * 170.0f + breathe * 38.0f);
    setPixelWithBrightness(i, dawn, brightness);
  }
}

void renderMoonTide(uint32_t now) {
  Rgb moonTint = {190, 220, 255};
  Rgb moon = blendColor(settings.color, moonTint, 0.20f);
  float denominator = LED_COUNT > 1 ? LED_COUNT - 1 : 1;

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / denominator;
    float low = (sinf(x * TWO_PI * 1.15f - now * 0.00031f) + 1.0f) * 0.5f;
    float high = (sinf(x * TWO_PI * 3.1f + now * 0.00019f) + 1.0f) * 0.5f;
    uint8_t brightness = clampByte(28.0f + low * 118.0f + high * 54.0f);
    setPixelWithBrightness(i, moon, brightness);
  }
}

void renderVelvetAurora(uint32_t now) {
  Rgb coolTint = {120, 190, 255};
  Rgb warmTint = {255, 210, 155};
  Rgb cool = blendColor(settings.color, coolTint, 0.28f);
  Rgb warm = blendColor(settings.color, warmTint, 0.18f);
  bool usePalette = settings.paletteEnabled && currentModeSupportsPalette() && getActivePalette() != nullptr;
  float denominator = LED_COUNT > 1 ? LED_COUNT - 1 : 1;

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / denominator;
    float ribbon = (sinf(x * TWO_PI * 2.0f + now * 0.00023f) + 1.0f) * 0.5f;
    float veil = (sinf(x * TWO_PI * 0.55f - now * 0.00009f) + 1.0f) * 0.5f;
    Rgb color;
    if (usePalette) {
      uint16_t driftedPosition = static_cast<uint16_t>((i + (now / 180UL)) % LED_COUNT);
      RgbPixel ribbonColor = samplePalette(driftedPosition, LED_COUNT > 1 ? LED_COUNT - 1 : 1);
      RgbPixel accentColor = getWeightedPaletteColor(static_cast<uint8_t>((i * 29 + now / 97UL) & 0xFF));
      Rgb paletteRibbon = {ribbonColor.r, ribbonColor.g, ribbonColor.b};
      Rgb paletteAccent = {accentColor.r, accentColor.g, accentColor.b};
      color = blendColor(paletteRibbon, paletteAccent, ribbon * 0.36f);
    } else {
      color = blendColor(cool, warm, ribbon * 0.45f);
    }
    uint8_t brightness = clampByte(42.0f + powf(ribbon, 1.5f) * 110.0f + veil * 46.0f);
    setPixelWithBrightness(i, color, brightness);
  }
}

void renderLanternDrift(uint32_t now) {
  Rgb lantern = blendColor(settings.color, kelvinToRgb(2400), 0.22f);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float value = 30.0f;
    for (uint8_t layer = 0; layer < 5; layer++) {
      float center = fmodf((now * (0.000018f + layer * 0.000006f) * LED_COUNT) + layer * 13.0f, LED_COUNT + 22.0f) - 11.0f;
      float width = 5.0f + layer * 1.7f;
      float d = fabsf(static_cast<float>(i) - center);
      value += expf(-(d * d) / (2.0f * width * width)) * (78.0f + layer * 9.0f);
    }
    float breathe = (sinf(now * 0.00017f + i * 0.21f) + 1.0f) * 0.5f;
    setPixelWithBrightness(i, lantern, clampByte(value + breathe * 18.0f));
  }
}

void renderRainGlass(uint32_t now) {
  Rgb glassTint = {210, 235, 255};
  Rgb glass = blendColor(settings.color, glassTint, 0.18f);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float value = 24.0f;
    for (uint8_t layer = 0; layer < 6; layer++) {
      float speed = 0.000028f + layer * 0.000011f;
      float position = fmodf(now * speed * LED_COUNT + layer * 17.0f, LED_COUNT + 18.0f) - 9.0f;
      float d = fabsf(static_cast<float>(i) - position);
      float bead = expf(-(d * d) / (2.0f * 1.1f));
      float trail = clamp01((position - i) / 8.0f) * expf(-fabsf(position - i) / 8.0f);
      value += bead * 130.0f + trail * 58.0f;
    }
    float pane = (sinf(i * 0.63f + now * 0.00008f) + 1.0f) * 11.0f;
    setPixelWithBrightness(i, glass, clampByte(value + pane));
  }
}

void renderSatinBreathing(uint32_t now) {
  float phase = phaseFromMillis(now, 11000UL);
  float envelope;
  if (phase < 0.38f) {
    envelope = easeInOut01(phase / 0.38f);
  } else if (phase < 0.50f) {
    envelope = 1.0f;
  } else {
    envelope = 1.0f - easeInOut01((phase - 0.50f) / 0.50f);
  }
  uint8_t baseLevel = clamp8(46 + static_cast<int>(envelope * 172.0f));
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warmPeak = warmWhiteApprox(2850);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    RgbPixel color = sampleActivePaletteOrColor(i, maxPosition);
    color = blendRgb(color, warmPeak, static_cast<uint8_t>(envelope * 32.0f));
    int texture = static_cast<int>((static_cast<uint16_t>(smoothHash8(i * 11U, now, 997UL)) * 17U) / 256U) - 8;
    setFramePixelRgb(i, scaleRgb(color, clamp8(baseLevel + texture)));
  }
}

void renderPaperLanternRow(uint32_t now) {
  uint8_t lanternCount = static_cast<uint8_t>(constrain(LED_COUNT / 10, 3, 8));
  float maxPosition = LED_COUNT > 1 ? LED_COUNT - 1.0f : 1.0f;
  uint16_t maxPalettePosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warm = warmWhiteApprox(2550);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float level = 18.0f;
    RgbPixel color = warm;
    for (uint8_t lantern = 0; lantern < lanternCount; lantern++) {
      float slot = lanternCount <= 1 ? 0.0f : static_cast<float>(lantern) / static_cast<float>(lanternCount - 1);
      float sway = sinf(now * (0.00009f + lantern * 0.000014f) + lantern * 1.7f) * 1.5f;
      float center = slot * maxPosition + sway;
      float radius = max(3.0f, maxPosition / (lanternCount * 1.7f));
      float d = fabsf(static_cast<float>(i) - center);
      float falloff = clamp01(1.0f - (d / radius));
      falloff = falloff * falloff * (3.0f - 2.0f * falloff);
      float breathe = 0.72f + 0.28f * (sineEase8(phaseFromMillis(now + lantern * 733UL, 9000UL + lantern * 1100UL)) / 255.0f);
      level += falloff * breathe * 152.0f;
      if (falloff > 0.45f) {
        color = sampleActivePaletteOrColor(static_cast<uint16_t>(slot * maxPalettePosition), maxPalettePosition);
      }
    }
    color = blendRgb(color, warm, 150);
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderLibraryLamp(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warm = warmWhiteApprox(settings.usingWhiteTemperature ? settings.whiteKelvin : 3400);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    RgbPixel paletteColor = sampleActivePaletteOrColor(i, maxPosition);
    RgbPixel color = settings.paletteEnabled && currentModeSupportsPalette()
      ? blendRgb(warm, paletteColor, 34)
      : warm;
    uint8_t slow = triangleWave8(phaseFromMillis(now + hash8(i * 19U) * 73UL, 150000UL));
    int texture = static_cast<int>(hash8(i * 23U) % 9) - 4 + static_cast<int>(slow / 80);
    setFramePixelRgb(i, scaleRgb(color, clamp8(224 + texture)));
  }
}

void renderSolarNoon(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warm = warmWhiteApprox(4550);
  RgbPixel cool = warmWhiteApprox(settings.usingWhiteTemperature ? settings.whiteKelvin : 5400);
  float drift = (sinf(now * 0.000018f) + 1.0f) * 0.5f;
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    uint8_t amount = static_cast<uint8_t>((static_cast<uint32_t>(i) * 255UL) / maxPosition);
    amount = lerp8(amount, 255 - amount, static_cast<uint8_t>(drift * 42.0f));
    RgbPixel color = blendRgb(warm, cool, amount);
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      color = blendRgb(color, sampleActivePaletteOrColor(i, maxPosition), 20);
    }
    setFramePixelRgb(i, scaleRgb(color, 245));
  }
}

void renderGoldenHour(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel sun = {255, 174, 72};
  RgbPixel cream = warmWhiteApprox(3100);
  float drift = (sinf(now * 0.000035f) + 1.0f) * 0.5f;
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float source = clamp01(1.0f - fabsf(x - drift) * 1.35f);
    uint8_t amount = static_cast<uint8_t>((1.0f - source) * 185.0f);
    RgbPixel color = blendRgb(sun, cream, amount);
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      color = blendRgb(color, sampleActivePaletteOrColor(i, maxPosition), 42);
    }
    uint8_t level = clamp8(82 + static_cast<int>(source * 138.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderDreamAquarium(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel deep = {18, 66, 100};
  RgbPixel aqua = {62, 190, 190};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float waveA = (sinf(x * TWO_PI * 1.5f - now * 0.00027f) + 1.0f) * 0.5f;
    float waveB = (sinf(x * TWO_PI * 4.4f + now * 0.00013f) + 1.0f) * 0.5f;
    RgbPixel color = blendRgb(deep, aqua, clampByte(88.0f + waveA * 118.0f));
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      uint16_t drifted = static_cast<uint16_t>((i + (now / 210UL)) % LED_COUNT);
      color = blendRgb(color, sampleActivePaletteOrColor(drifted, maxPosition), 96);
    }
    uint8_t level = clamp8(54 + static_cast<int>(waveA * 74.0f + waveB * 45.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderCircuitGlow(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel base = {6, 24, 34};
  RgbPixel trace = settings.paletteEnabled && currentModeSupportsPalette()
    ? sampleActivePaletteOrColor((now / 80UL) % LED_COUNT, maxPosition)
    : RgbPixel{22, 255, 162};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float level = 22.0f + static_cast<float>(hash8(i * 41U) % 14);
    for (uint8_t lane = 0; lane < 4; lane++) {
      uint16_t period = static_cast<uint16_t>(1800U + lane * 470U);
      uint16_t head = static_cast<uint16_t>(((now + lane * 417UL) % period) * LED_COUNT / period);
      uint16_t distance = i > head ? i - head : head - i;
      distance = min(distance, static_cast<uint16_t>(LED_COUNT - distance));
      if (distance < 7) {
        level += (7 - distance) * (15.0f + lane * 2.0f);
      }
    }
    RgbPixel color = blendRgb(base, trace, clamp8(static_cast<int>(level * 1.35f)));
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level + 40.0f))));
  }
}

void renderWindowSunrise(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float reach = phaseFromMillis(now, 150000UL);
  RgbPixel amber = {255, 134, 54};
  RgbPixel daylight = warmWhiteApprox(3650);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float boundary = reach * 1.28f - 0.12f;
    float lit = 1.0f - clamp01((x - boundary) / 0.48f);
    lit = lit * lit * (3.0f - 2.0f * lit);
    uint8_t shimmer = sineApprox8(now + i * 37UL, 17000UL, hash8(i * 13U) * 257U);
    RgbPixel palette = sampleNamedOrActivePalette("peach-sky", i, maxPosition);
    RgbPixel sunrise = blendRgb(amber, daylight, clamp8(static_cast<int>(reach * 145.0f + x * 42.0f)));
    sunrise = blendRgb(sunrise, palette, 26);
    uint8_t level = clamp8(24 + static_cast<int>(lit * 184.0f) + static_cast<int>(shimmer / 18));
    setFramePixelRgb(i, scaleRgb(sunrise, level));
  }
}

void renderEmberQuilt(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warm = {255, 128, 55};
  RgbPixel cream = warmWhiteApprox(2550);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float level = 32.0f;
    RgbPixel color = blendRgb(warm, cream, 48);
    for (uint8_t zone = 0; zone < 5; zone++) {
      float center = (static_cast<float>(zone) + 0.5f) * static_cast<float>(maxPosition) / 5.0f;
      center += sinf(now * (0.000021f + zone * 0.000004f) + zone * 1.9f) * 3.5f;
      float width = max(5.0f, static_cast<float>(LED_COUNT) * (0.16f + zone * 0.012f));
      float d = fabsf(static_cast<float>(i) - center);
      float falloff = clamp01(1.0f - d / width);
      falloff = falloff * falloff * (3.0f - 2.0f * falloff);
      float breathe = 0.72f + 0.28f * (sineApprox8(now + zone * 991UL, 38000UL + zone * 3100UL, 0) / 255.0f);
      level += falloff * breathe * 72.0f;
      if (falloff > 0.35f) {
        RgbPixel sampled = sampleNamedOrActivePalette("candle", static_cast<uint16_t>(zone * maxPosition / 5), maxPosition);
        color = blendRgb(color, sampled, static_cast<uint8_t>(falloff * 58.0f));
      }
    }
    uint8_t texture = static_cast<uint8_t>((static_cast<uint16_t>(smoothHash8(i * 29U, now, 1600UL)) * 11U) / 256U);
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level) + texture)));
  }
}

void renderPorcelainSky(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel porcelain = coolWhiteApprox(5100);
  RgbPixel pearl = warmWhiteApprox(4200);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float drift = (sinf(x * TWO_PI * 0.72f + now * 0.000022f) + 1.0f) * 0.5f;
    float cloud = (sinf(x * TWO_PI * 1.9f - now * 0.000014f) + 1.0f) * 0.5f;
    RgbPixel color = blendRgb(porcelain, pearl, clamp8(static_cast<int>(42.0f + drift * 70.0f)));
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      color = blendRgb(color, sampleActivePaletteOrColor(i, maxPosition), 34);
    }
    uint8_t level = clamp8(176 + static_cast<int>(cloud * 34.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderHearthline(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel base = {150, 54, 18};
  RgbPixel amber = {255, 150, 54};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float level = 44.0f;
    RgbPixel color = blendRgb(base, amber, 78);
    for (uint8_t band = 0; band < 4; band++) {
      float period = 25000.0f + band * 9300.0f;
      float center = fmodf((now + band * 5100UL) / period * (LED_COUNT + 18.0f), LED_COUNT + 18.0f) - 9.0f;
      float width = 6.0f + band * 2.4f;
      float d = fabsf(static_cast<float>(i) - center);
      float falloff = expf(-(d * d) / (2.0f * width * width));
      level += falloff * (62.0f + band * 10.0f);
      if (falloff > 0.18f) {
        RgbPixel sampled = sampleNamedOrActivePalette("warm-linen", static_cast<uint16_t>((i + band * 7) % LED_COUNT), maxPosition);
        color = blendRgb(color, sampled, static_cast<uint8_t>(falloff * 48.0f));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderEveningCurrent(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel dusk = {74, 92, 136};
  RgbPixel warm = {255, 166, 100};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float swell = (sinf(x * TWO_PI * 1.15f - now * 0.000105f) + 1.0f) * 0.5f;
    float ripple = (sinf(x * TWO_PI * 3.4f + now * 0.000061f) + 1.0f) * 0.5f;
    RgbPixel color = settings.paletteEnabled && currentModeSupportsPalette()
      ? sampleActivePaletteOrColor(static_cast<uint16_t>((i + now / 280UL) % LED_COUNT), maxPosition)
      : blendRgb(dusk, warm, 54);
    color = blendRgb(color, warm, static_cast<uint8_t>(swell * 32.0f));
    uint8_t level = clamp8(58 + static_cast<int>(swell * 86.0f + ripple * 34.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderStarlitLinen(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  uint8_t starCount = static_cast<uint8_t>(constrain(LED_COUNT / 7, 5, 12));
  RgbPixel base = sampleNamedOrActivePalette("warm-linen", 0, maxPosition);
  RgbPixel highlight = warmWhiteApprox(3000);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float level = 24.0f + static_cast<float>(hash8(i * 17U) % 8);
    RgbPixel color = blendRgb(base, sampleActivePaletteOrColor(i, maxPosition), settings.paletteEnabled ? 46 : 0);
    for (uint8_t star = 0; star < starCount; star++) {
      uint16_t center = static_cast<uint16_t>((hash8(star * 47U) * LED_COUNT) / 255U);
      float d = fabsf(static_cast<float>(i) - static_cast<float>(center));
      float soft = expf(-(d * d) / 5.4f);
      uint8_t phase = sineApprox8(now + star * 2200UL, 36000UL + star * 1400UL, hash8(star * 31U) * 257U);
      level += soft * (static_cast<float>(phase) / 255.0f) * 58.0f;
      if (soft > 0.08f) {
        color = blendRgb(color, highlight, static_cast<uint8_t>(soft * phase / 5));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderDeepSeaGlass(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel deep = {8, 42, 76};
  RgbPixel glass = {74, 176, 194};
  RgbPixel glint = {170, 232, 245};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float depth = (sinf(x * TWO_PI * 1.35f - now * 0.000074f) + 1.0f) * 0.5f;
    float shimmer = (sinf(x * TWO_PI * 5.1f + now * 0.000121f) + 1.0f) * 0.5f;
    RgbPixel color = blendRgb(deep, glass, clamp8(static_cast<int>(75.0f + depth * 118.0f)));
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      color = blendRgb(color, sampleActivePaletteOrColor(static_cast<uint16_t>((i + now / 360UL) % LED_COUNT), maxPosition), 72);
    }
    color = blendRgb(color, glint, static_cast<uint8_t>(shimmer * 26.0f));
    uint8_t level = clamp8(46 + static_cast<int>(depth * 72.0f + shimmer * 30.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderQuietNorthernLights(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel night = {8, 18, 38};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float veilA = (sinf(x * TWO_PI * 1.2f + now * 0.000055f) + 1.0f) * 0.5f;
    float veilB = (sinf(x * TWO_PI * 2.7f - now * 0.000038f) + 1.0f) * 0.5f;
    RgbPixel green = {74, 228, 156};
    RgbPixel violet = {120, 116, 255};
    RgbPixel ribbon = settings.paletteEnabled && currentModeSupportsPalette()
      ? sampleActivePaletteOrColor(static_cast<uint16_t>((i + now / 420UL) % LED_COUNT), maxPosition)
      : blendRgb(green, violet, static_cast<uint8_t>(veilB * 135.0f));
    uint8_t fold = clamp8(static_cast<int>((veilA * veilA) * 175.0f + veilB * 42.0f));
    RgbPixel color = blendRgb(night, ribbon, fold);
    uint8_t level = clamp8(32 + static_cast<int>(veilA * 76.0f + veilB * 28.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderCandleHall(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  uint8_t candleCount = static_cast<uint8_t>(constrain(LED_COUNT / 12, 3, 7));
  RgbPixel shadow = {42, 16, 8};
  RgbPixel flame = warmWhiteApprox(2350);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float level = 16.0f;
    RgbPixel color = shadow;
    for (uint8_t candle = 0; candle < candleCount; candle++) {
      float slot = candleCount <= 1 ? 0.0f : static_cast<float>(candle) / static_cast<float>(candleCount - 1);
      float center = slot * static_cast<float>(maxPosition);
      center += sinf(now * (0.000035f + candle * 0.000006f) + candle * 1.7f) * 1.4f;
      float width = max(2.2f, static_cast<float>(LED_COUNT) * 0.045f);
      float d = fabsf(static_cast<float>(i) - center);
      float falloff = clamp01(1.0f - d / width);
      falloff = falloff * falloff * (3.0f - 2.0f * falloff);
      uint8_t pulse = softPulseEnvelope8(now + candle * 1200UL, 13000UL + candle * 1700UL, hash8(candle * 31U) * 257U);
      float flicker = 0.72f + static_cast<float>(pulse) / 910.0f;
      level += falloff * flicker * 164.0f;
      if (falloff > 0.08f) {
        RgbPixel candleColor = sampleNamedOrActivePalette("candle", static_cast<uint16_t>(slot * maxPosition), maxPosition);
        candleColor = blendRgb(candleColor, flame, 72);
        color = blendRgb(color, candleColor, static_cast<uint8_t>(falloff * 190.0f));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderSlowPrism(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  uint16_t offset = static_cast<uint16_t>((now % 210000UL) * maxPosition / 210000UL);
  RgbPixel anchor = {settings.color.r, settings.color.g, settings.color.b};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    uint16_t position = static_cast<uint16_t>((i + offset) % LED_COUNT);
    float x = static_cast<float>(position) / static_cast<float>(maxPosition);
    RgbPixel color;
    if (settings.paletteEnabled && currentModeSupportsPalette() && getActivePalette() != nullptr) {
      color = samplePalette(position, maxPosition);
    } else {
      Rgb wheelA = wheel(static_cast<uint8_t>((position * 255UL / maxPosition + 24U) & 0xFF));
      Rgb wheelB = wheel(static_cast<uint8_t>((position * 255UL / maxPosition + 208U) & 0xFF));
      RgbPixel companionA = {wheelA.r, wheelA.g, wheelA.b};
      RgbPixel companionB = {wheelB.r, wheelB.g, wheelB.b};
      RgbPixel companion = blendRgb(companionA, companionB, static_cast<uint8_t>(x * 255.0f));
      color = blendRgb(anchor, companion, 82);
    }
    float prism = (sinf(x * TWO_PI * 1.2f + now * 0.000024f) + 1.0f) * 0.5f;
    color = blendRgb(anchor, color, 160);
    uint8_t level = clamp8(92 + static_cast<int>(prism * 72.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderGardenShade(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel sun = warmWhiteApprox(3900);
  RgbPixel leaf = {96, 126, 62};
  RgbPixel gold = {224, 188, 92};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float breeze = (sinf(x * TWO_PI * 1.25f + now * 0.000045f) + 1.0f) * 0.5f;
    float shade = (sinf(x * TWO_PI * 3.2f - now * 0.000032f) + 1.0f) * 0.5f;
    RgbPixel natural = blendRgb(leaf, gold, clamp8(static_cast<int>(breeze * 120.0f)));
    RgbPixel palette = sampleNamedOrActivePalette("garden", i, maxPosition);
    natural = blendRgb(natural, palette, 58);
    RgbPixel color = blendRgb(sun, natural, clamp8(static_cast<int>(42.0f + shade * 76.0f)));
    uint8_t level = clamp8(122 + static_cast<int>(breeze * 48.0f) - static_cast<int>(shade * 18.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderSnowfallHush(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  uint8_t particleCount = static_cast<uint8_t>(constrain(LED_COUNT / 8, 5, 13));
  RgbPixel base = sampleNamedOrActivePalette("moonlight", 0, maxPosition);
  RgbPixel snow = coolWhiteApprox(6200);
  if (settings.paletteEnabled && currentModeSupportsPalette()) {
    snow = blendRgb(snow, sampleActivePaletteOrColor(maxPosition / 2, maxPosition), 58);
  }
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float level = 18.0f + static_cast<float>(hash8(i * 23U) % 8);
    RgbPixel color = blendRgb(base, snow, 28);
    for (uint8_t particle = 0; particle < particleCount; particle++) {
      float seed = static_cast<float>(hash8(particle * 41U)) / 255.0f;
      float drift = fmodf(seed + now * (0.0000018f + particle * 0.00000017f), 1.0f);
      float center = drift * static_cast<float>(maxPosition);
      float d = fabsf(static_cast<float>(i) - center);
      d = min(d, static_cast<float>(LED_COUNT) - d);
      float width = 1.8f + static_cast<float>(hash8(particle * 19U) % 3);
      float soft = expf(-(d * d) / (2.0f * width * width));
      uint8_t envelope = softPulseEnvelope8(now + particle * 1700UL, 41000UL + particle * 1300UL, hash8(particle * 29U) * 257U);
      level += soft * static_cast<float>(envelope) * 0.29f;
      if (soft > 0.04f) {
        color = blendRgb(color, snow, static_cast<uint8_t>(soft * envelope / 3));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderObservatoryGlow(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel night = sampleNamedOrActivePalette("moonlight", 0, maxPosition);
  night = blendRgb(RgbPixel{4, 12, 34}, night, 72);
  RgbPixel moon = coolWhiteApprox(5600);
  float moonCenter = phaseFromMillis(now, 420000UL) * static_cast<float>(maxPosition);
  uint8_t starCount = static_cast<uint8_t>(constrain(LED_COUNT / 9, 5, 12));
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float d = fabsf(static_cast<float>(i) - moonCenter);
    d = min(d, static_cast<float>(LED_COUNT) - d);
    float glow = expf(-(d * d) / (2.0f * max(5.0f, LED_COUNT * 0.11f) * max(5.0f, LED_COUNT * 0.11f)));
    float level = 20.0f + glow * 112.0f;
    RgbPixel color = blendRgb(night, moon, static_cast<uint8_t>(glow * 150.0f));
    for (uint8_t star = 0; star < starCount; star++) {
      uint16_t center = static_cast<uint16_t>((hash8(star * 53U) * maxPosition) / 255U);
      float sd = fabsf(static_cast<float>(i) - static_cast<float>(center));
      float soft = expf(-(sd * sd) / 3.2f);
      float nearMoon = clamp01(1.0f - fabsf(static_cast<float>(center) - moonCenter) / max(8.0f, LED_COUNT * 0.24f));
      level += soft * (12.0f + nearMoon * 38.0f);
      if (soft > 0.08f) {
        color = blendRgb(color, moon, static_cast<uint8_t>(soft * (38.0f + nearMoon * 62.0f)));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderRainyWindow(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel pane = sampleNamedOrActivePalette("storm-blue", 0, maxPosition);
  RgbPixel droplet = {160, 206, 226};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float level = 24.0f + (sinf(x * TWO_PI * 1.4f + now * 0.000036f) + 1.0f) * 12.0f;
    RgbPixel color = blendRgb(RgbPixel{10, 28, 46}, pane, 120);
    for (uint8_t drop = 0; drop < 7; drop++) {
      float seed = static_cast<float>(hash8(drop * 37U)) / 255.0f;
      float position = fmodf(seed + now * (0.0000028f + drop * 0.00000024f), 1.0f) * static_cast<float>(maxPosition);
      float beadDistance = fabsf(static_cast<float>(i) - position);
      float bead = expf(-(beadDistance * beadDistance) / 2.8f);
      float trail = clamp01((position - static_cast<float>(i)) / 7.5f) * expf(-fabsf(position - static_cast<float>(i)) / 7.5f);
      level += bead * 62.0f + trail * 34.0f;
      if (bead > 0.05f || trail > 0.04f) {
        color = blendRgb(color, droplet, clamp8(static_cast<int>((bead + trail) * 82.0f)));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderCampfireBlanket(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel ember = {178, 58, 18};
  RgbPixel blanket = warmWhiteApprox(2450);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float waveA = (sinf(x * TWO_PI * 1.7f - now * 0.000086f) + 1.0f) * 0.5f;
    float waveB = (sinf(x * TWO_PI * 4.1f + now * 0.000053f) + 1.0f) * 0.5f;
    RgbPixel palette = sampleNamedOrActivePalette("candle", i, maxPosition);
    RgbPixel color = blendRgb(ember, blanket, clamp8(static_cast<int>(84.0f + waveA * 78.0f)));
    color = blendRgb(color, palette, 52);
    uint8_t level = clamp8(50 + static_cast<int>(waveA * 70.0f + waveB * 32.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderNorthernWhisper(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel night = {5, 12, 32};
  RgbPixel green = {64, 210, 145};
  RgbPixel blue = {78, 128, 224};
  RgbPixel violet = {134, 104, 226};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float veilA = (sinf(x * TWO_PI * 0.92f + now * 0.000034f) + 1.0f) * 0.5f;
    float veilB = (sinf(x * TWO_PI * 2.1f - now * 0.000024f) + 1.0f) * 0.5f;
    RgbPixel ribbon = blendRgb(green, blue, static_cast<uint8_t>(veilA * 130.0f));
    ribbon = blendRgb(ribbon, violet, static_cast<uint8_t>(veilB * 72.0f));
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      ribbon = blendRgb(ribbon, sampleActivePaletteOrColor(static_cast<uint16_t>((i + now / 520UL) % LED_COUNT), maxPosition), 96);
    }
    float fold = veilA * veilA * 0.54f + veilB * 0.18f;
    RgbPixel color = blendRgb(night, ribbon, clamp8(static_cast<int>(fold * 180.0f)));
    uint8_t level = clamp8(24 + static_cast<int>(fold * 86.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderStormOutside(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel base = sampleNamedOrActivePalette("storm-blue", 0, maxPosition);
  RgbPixel cloud = {72, 96, 124};
  RgbPixel lightning = coolWhiteApprox(7200);
  uint8_t flashRaw = sineApprox8(now, 53000UL, 11000U);
  uint8_t flash = flashRaw > 224 ? static_cast<uint8_t>((flashRaw - 224) * 8U) : 0;
  if (settings.nightGuardEnabled) {
    flash = flash / 4;
  }
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float roll = (sinf(x * TWO_PI * 0.8f - now * 0.000044f) + 1.0f) * 0.5f;
    float rain = (sinf(x * TWO_PI * 7.0f + now * 0.00018f) + 1.0f) * 0.5f;
    RgbPixel color = blendRgb(RgbPixel{4, 14, 28}, base, 118);
    color = blendRgb(color, cloud, static_cast<uint8_t>(roll * 68.0f));
    color = blendRgb(color, lightning, flash);
    uint8_t level = clamp8(22 + static_cast<int>(roll * 48.0f + rain * 20.0f + flash / 3));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void initPulse(uint8_t index, uint32_t now) {
  uint16_t centerLimit = LED_COUNT > 0 ? LED_COUNT : 1;
  uint16_t widthLimit = LED_COUNT / 4;
  if (widthLimit < 8) {
    widthLimit = 8;
  }

  pulses[index].center = random(0, centerLimit);
  pulses[index].width = random(4, widthLimit);

  uint32_t requestedMax = static_cast<uint32_t>(settings.slowPulseMaxSec) * 1000UL;
  uint32_t minDuration = static_cast<uint32_t>(settings.slowPulseMaxSec) * 550UL;
  if (minDuration < 1000UL) {
    minDuration = 1000UL;
  }
  if (requestedMax <= minDuration) {
    requestedMax = minDuration + 1UL;
  }

  pulses[index].durationMs = random(minDuration, requestedMax);
  pulses[index].bornMs = now;
  pulses[index].seed = random(0, 255);
}

void initSlowWave(uint8_t index, uint32_t now) {
  slowWaves[index].direction = random(0, 2) == 0 ? -1.0f : 1.0f;
  uint16_t widthLimit = LED_COUNT / 5;
  if (widthLimit < 8) {
    widthLimit = 8;
  }
  slowWaves[index].width = random(5, widthLimit);
  slowWaves[index].durationMs = random(16000UL, 42000UL);
  slowWaves[index].bornMs = now;
  slowWaves[index].seed = random(0, 255);
}

void fillScaled(Rgb color, uint8_t brightness) {
  Rgb scaled = scaleColor(color, brightness);
  fillFrame(scaled.r, scaled.g, scaled.b);
}

void clearStrip() {
  clearFrame();
  outputFrameToStrip();
}

void setPixelWithBrightness(uint16_t pixel, Rgb color, uint8_t brightness) {
  Rgb scaled = scaleColor(color, brightness);
  setFramePixel(pixel, scaled.r, scaled.g, scaled.b);
}

Rgb scaleColor(Rgb color, uint8_t brightness) {
  return {
    static_cast<uint8_t>((static_cast<uint16_t>(color.r) * brightness) / 255),
    static_cast<uint8_t>((static_cast<uint16_t>(color.g) * brightness) / 255),
    static_cast<uint8_t>((static_cast<uint16_t>(color.b) * brightness) / 255)
  };
}

Rgb blendColor(Rgb a, Rgb b, float amount) {
  amount = clamp01(amount);
  return {
    clampByte(a.r + (b.r - a.r) * amount),
    clampByte(a.g + (b.g - a.g) * amount),
    clampByte(a.b + (b.b - a.b) * amount)
  };
}

Rgb kelvinToRgb(uint16_t kelvin) {
  float temp = constrain(kelvin, 1000, 40000) / 100.0f;
  float red;
  float green;
  float blue;

  if (temp <= 66.0f) {
    red = 255.0f;
    green = 99.4708025861f * logf(temp) - 161.1195681661f;
    if (temp <= 19.0f) {
      blue = 0.0f;
    } else {
      blue = 138.5177312231f * logf(temp - 10.0f) - 305.0447927307f;
    }
  } else {
    red = 329.698727446f * powf(temp - 60.0f, -0.1332047592f);
    green = 288.1221695283f * powf(temp - 60.0f, -0.0755148492f);
    blue = 255.0f;
  }

  return {clampByte(red), clampByte(green), clampByte(blue)};
}

Rgb wheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return {static_cast<uint8_t>(255 - pos * 3), 0, static_cast<uint8_t>(pos * 3)};
  }
  if (pos < 170) {
    pos -= 85;
    return {0, static_cast<uint8_t>(pos * 3), static_cast<uint8_t>(255 - pos * 3)};
  }
  pos -= 170;
  return {static_cast<uint8_t>(pos * 3), static_cast<uint8_t>(255 - pos * 3), 0};
}

uint8_t clampByte(float value) {
  if (value <= 0.0f) return 0;
  if (value >= 255.0f) return 255;
  return static_cast<uint8_t>(value + 0.5f);
}

float clamp01(float value) {
  if (value <= 0.0f) return 0.0f;
  if (value >= 1.0f) return 1.0f;
  return value;
}

float smoothstep(float edge0, float edge1, float x) {
  float t = clamp01((x - edge0) / (edge1 - edge0));
  return t * t * (3.0f - 2.0f * t);
}

#endif
