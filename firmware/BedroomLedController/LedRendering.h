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

constexpr float CHASE_SEGMENT_LENGTH_MM = 180.0f;
constexpr float CHASE_GAP_LENGTH_MM = 240.0f;
constexpr float WAVE_LONG_WAVELENGTH_MM = 3600.0f;
constexpr float WAVE_SHORT_WAVELENGTH_MM = 1900.0f;
constexpr float MOON_TIDE_LONG_WAVELENGTH_MM = 4300.0f;
constexpr float MOON_TIDE_RIPPLE_WAVELENGTH_MM = 1450.0f;
constexpr float DEEP_SEA_CAUSTIC_WAVELENGTH_MM = 980.0f;
constexpr float LANTERN_GLOW_RADIUS_MM = 95.0f;
constexpr float LANTERN_DRIFT_MARGIN_MM = 120.0f;
constexpr float RAIN_GLASS_DROPLET_RADIUS_MM = 18.0f;
constexpr float RAIN_GLASS_TRAIL_LENGTH_MM = 92.0f;
constexpr float PAPER_LANTERN_SPACING_MM = 720.0f;
constexpr float PAPER_LANTERN_SWAY_MM = 28.0f;
constexpr float EMBER_QUILT_GLOW_RADIUS_MM = 760.0f;
constexpr float HEARTHLINE_BAND_WIDTH_MM = 64.0f;
constexpr float STAR_HALO_RADIUS_MM = 28.0f;
constexpr float SNOW_HALO_RADIUS_MM = 34.0f;
constexpr float OBSERVATORY_MOON_GLOW_RADIUS_MM = 720.0f;
constexpr float RAINY_WINDOW_DROPLET_RADIUS_MM = 20.0f;
constexpr float RAINY_WINDOW_TRAIL_LENGTH_MM = 96.0f;
constexpr float AURORA_BASE_WAVELENGTH_MM = 2600.0f;
constexpr float AURORA_RIBBON_WIDTH_MM = 520.0f;
constexpr float CANDLE_CORE_WIDTH_MM = 28.0f;
constexpr float CANDLE_GLOW_RADIUS_MM = 120.0f;
constexpr float CANDLE_SPACING_MIN_MM = 680.0f;
constexpr float SLOW_PULSE_MIN_WIDTH_MM = 80.0f;
constexpr float SLOW_PULSE_MAX_WIDTH_MM = 620.0f;
constexpr float SLOW_WAVE_MIN_WIDTH_MM = 130.0f;
constexpr float SLOW_WAVE_MAX_WIDTH_MM = 1050.0f;

uint16_t getConfiguredLedCount() {
  return LED_COUNT;
}

uint16_t lastPixelIndex() {
  uint16_t count = getConfiguredLedCount();
  return count > 0 ? count - 1 : 0;
}

float getPixelsPerMeter() {
  if (STRIP_LENGTH_MM == 0) {
    return LED_DENSITY_PER_METER > 0.0f ? LED_DENSITY_PER_METER : 1.0f;
  }
  return (static_cast<float>(getConfiguredLedCount()) * 1000.0f) / static_cast<float>(STRIP_LENGTH_MM);
}

float getMmPerPixel() {
  uint16_t count = getConfiguredLedCount();
  if (count == 0) {
    return 1000.0f;
  }
  return static_cast<float>(STRIP_LENGTH_MM) / static_cast<float>(count);
}

float pixelsForPhysicalLength(float mm) {
  if (mm <= 0.0f) {
    return 0.0f;
  }
  float mmPerPixel = getMmPerPixel();
  if (mmPerPixel <= 0.0f) {
    return 1.0f;
  }
  return mm / mmPerPixel;
}

uint16_t mmToPixels(float mm) {
  if (mm <= 0.0f) {
    return 0;
  }
  float pixels = pixelsForPhysicalLength(mm);
  if (mm > 0.0f && pixels < 1.0f) {
    pixels = 1.0f;
  }
  uint16_t rounded = static_cast<uint16_t>(pixels + 0.5f);
  return constrain(rounded, static_cast<uint16_t>(1), getConfiguredLedCount());
}

uint16_t cmToPixels(float cm) {
  return mmToPixels(cm * 10.0f);
}

uint16_t metersToPixels(float meters) {
  return mmToPixels(meters * 1000.0f);
}

float pixelToMm(uint16_t pixel) {
  return static_cast<float>(clampPixelIndex(pixel)) * getMmPerPixel();
}

float pixelToMeters(uint16_t pixel) {
  return pixelToMm(pixel) / 1000.0f;
}

float normalizedPosition01(uint16_t pixel) {
  uint16_t maxIndex = lastPixelIndex();
  if (maxIndex == 0) {
    return 0.0f;
  }
  return static_cast<float>(clampPixelIndex(pixel)) / static_cast<float>(maxIndex);
}

float physicalXmm(uint16_t pixel) {
  return normalizedPosition01(pixel) * static_cast<float>(STRIP_LENGTH_MM);
}

uint16_t clampPixelIndex(int32_t index) {
  if (index <= 0) {
    return 0;
  }
  uint16_t maxIndex = lastPixelIndex();
  if (index >= static_cast<int32_t>(maxIndex)) {
    return maxIndex;
  }
  return static_cast<uint16_t>(index);
}

bool pixelInRange(int32_t index) {
  return index >= 0 && index < static_cast<int32_t>(getConfiguredLedCount());
}

float cyclicPixelDistance(float a, float b) {
  float count = static_cast<float>(max(static_cast<uint16_t>(1), getConfiguredLedCount()));
  float d = fabsf(a - b);
  return min(d, count - d);
}

float cyclicPhysicalDistanceMm(float aMm, float bMm) {
  float length = max(1.0f, static_cast<float>(STRIP_LENGTH_MM));
  float d = fabsf(aMm - bMm);
  return min(d, length - d);
}

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

float animationStrengthRaw01() {
  return clamp01(static_cast<float>(settings.animationStrength) / 255.0f);
}

float animationStrength01() {
  float raw = animationStrengthRaw01();
  return raw * raw * (3.0f - 2.0f * raw);
}

float animationStrengthCentered() {
  return animationStrength01() * 2.0f - 1.0f;
}

float strengthCurve(float minValue, float maxValue, float curve) {
  float raw = animationStrengthRaw01();
  float shaped = curve <= 0.01f ? raw : powf(raw, curve);
  return minValue + (maxValue - minValue) * clamp01(shaped);
}

float strengthLinear(float minValue, float maxValue) {
  float raw = animationStrengthRaw01();
  return minValue + (maxValue - minValue) * raw;
}

float strengthEase(float minValue, float maxValue) {
  return minValue + (maxValue - minValue) * animationStrength01();
}

uint8_t strengthByte(uint8_t minValue, uint8_t maxValue) {
  return clampByte(strengthEase(static_cast<float>(minValue), static_cast<float>(maxValue)));
}

float modeSpeedMultiplier(Mode mode) {
  float low = 0.28f;
  float high = 1.85f;
  if (mode == MODE_LIBRARY_LAMP || mode == MODE_SOLAR_NOON) {
    low = 0.04f;
    high = 0.30f;
  } else if (mode == MODE_NORTHERN_WHISPER || mode == MODE_OBSERVATORY_GLOW || mode == MODE_SNOWFALL_HUSH) {
    low = 0.12f;
    high = 0.86f;
  } else if (mode == MODE_STROBE || mode == MODE_FLASH || mode == MODE_CHASE || mode == MODE_RAINBOW || mode == MODE_CIRCUIT_GLOW) {
    low = 0.55f;
    high = 2.30f;
  }
  if (settings.nightGuardEnabled && (mode == MODE_RAINBOW || mode == MODE_STORM_OUTSIDE)) {
    high *= 0.62f;
  }
  return strengthCurve(low, high, 0.78f);
}

float modeMotionMultiplier(Mode mode) {
  float low = 0.10f;
  float high = 1.00f;
  if (mode == MODE_LIBRARY_LAMP || mode == MODE_SOLAR_NOON) {
    low = 0.00f;
    high = 0.18f;
  } else if (mode == MODE_STORM_OUTSIDE || mode == MODE_NORTHERN_WHISPER || mode == MODE_SNOWFALL_HUSH) {
    low = 0.08f;
    high = 0.72f;
  }
  return strengthEase(low, high);
}

float modeContrastMultiplier(Mode mode) {
  float low = 0.18f;
  float high = 1.0f;
  if (mode == MODE_LIBRARY_LAMP || mode == MODE_SOLAR_NOON) {
    low = 0.02f;
    high = 0.16f;
  } else if (mode == MODE_NORTHERN_WHISPER || mode == MODE_SNOWFALL_HUSH || mode == MODE_OBSERVATORY_GLOW) {
    low = 0.10f;
    high = 0.70f;
  }
  return strengthEase(low, high);
}

float modeDensityMultiplier(Mode mode) {
  float low = 0.28f;
  float high = 1.0f;
  if (mode == MODE_LIBRARY_LAMP || mode == MODE_SOLAR_NOON) {
    low = 0.04f;
    high = 0.18f;
  } else if (mode == MODE_STROBE || mode == MODE_FLASH) {
    low = 0.55f;
    high = 1.0f;
  }
  return strengthCurve(low, high, 0.72f);
}

float modeColorSpreadMultiplier(Mode mode) {
  float low = 0.10f;
  float high = 1.0f;
  if (mode == MODE_LIBRARY_LAMP || mode == MODE_SOLAR_NOON) {
    low = 0.00f;
    high = 0.12f;
  } else if (mode == MODE_NORTHERN_WHISPER || mode == MODE_SNOWFALL_HUSH || mode == MODE_OBSERVATORY_GLOW) {
    low = 0.08f;
    high = 0.58f;
  }
  if (settings.nightGuardEnabled && mode == MODE_RAINBOW) {
    high *= 0.70f;
  }
  return strengthEase(low, high);
}

uint8_t modeFloorForStrength(uint8_t low, uint8_t high) {
  return strengthByte(low, high);
}

uint8_t modeCeilingForStrength(uint8_t low, uint8_t high) {
  return strengthByte(low, high);
}

float mapIntensityFloat(float low, float high) {
  return strengthEase(low, high);
}

uint8_t mapIntensity8(uint8_t low, uint8_t high) {
  return strengthByte(low, high);
}

float smoothstep01(float x) {
  float t = clamp01(x);
  return t * t * (3.0f - 2.0f * t);
}

float smoothstepEdge(float edge0, float edge1, float x) {
  if (fabsf(edge1 - edge0) < 0.00001f) {
    return x >= edge1 ? 1.0f : 0.0f;
  }
  return smoothstep01((x - edge0) / (edge1 - edge0));
}

float fractf(float value) {
  return value - floorf(value);
}

float mixFloat(float a, float b, float t) {
  return a + (b - a) * clamp01(t);
}

float easeSlowAccelFastDecel(float x) {
  float t = clamp01(x);
  if (t < 0.65f) {
    float u = t / 0.65f;
    return 0.65f * smoothstep01(u);
  }
  float u = (t - 0.65f) / 0.35f;
  return 0.65f + 0.35f * (1.0f - powf(1.0f - u, 3.0f));
}

float softRidge(float waveValue, float width) {
  float usableWidth = max(0.05f, width);
  float x = (waveValue - (1.0f - usableWidth)) / usableWidth;
  return smoothstep01(x);
}

RgbPixel addSoftRgb(RgbPixel base, RgbPixel add, float amount) {
  float t = clamp01(amount);
  return {
    clampByte(static_cast<float>(base.r) + static_cast<float>(add.r) * t * (1.0f - static_cast<float>(base.r) / 255.0f)),
    clampByte(static_cast<float>(base.g) + static_cast<float>(add.g) * t * (1.0f - static_cast<float>(base.g) / 255.0f)),
    clampByte(static_cast<float>(base.b) + static_cast<float>(add.b) * t * (1.0f - static_cast<float>(base.b) / 255.0f))
  };
}

float softBand(float distance, float width) {
  if (width <= 0.0f) {
    return 0.0f;
  }
  float x = 1.0f - clamp01(distance / width);
  float soft = x * x * (3.0f - 2.0f * x);
  return soft * soft;
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

  if (animatedMode && now - lastFrameMs < TARGET_FRAME_INTERVAL_MS && !ledsDirty) {
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
  float speed = modeSpeedMultiplier(MODE_STROBE);
  float contrast = modeContrastMultiplier(MODE_STROBE);
  uint32_t delayMs = static_cast<uint32_t>((settings.strobeDelaySec * 1000.0f) / max(0.45f, speed));
  delayMs = constrain(delayMs, 55UL, 6000UL);
  if (now - strobeLastToggleMs >= delayMs) {
    strobeOn = !strobeOn;
    strobeLastToggleMs = now;
  }
  if (strobeOn) {
    fillScaled(settings.color, modeCeilingForStrength(150, 255));
  } else {
    fillScaled(settings.color, static_cast<uint8_t>(strengthLinear(0.0f, 18.0f) * (1.0f - contrast * 0.72f)));
  }
}

void renderFlash(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_FLASH);
  float contrast = modeContrastMultiplier(MODE_FLASH);
  uint32_t onMs = static_cast<uint32_t>(strengthLinear(180.0f, 520.0f));
  uint32_t offMs = static_cast<uint32_t>((settings.flashOffDelaySec * 1000.0f) / max(0.42f, speed));
  offMs = constrain(offMs, 120UL, 9000UL);
  uint32_t cycleMs = onMs + offMs;
  uint32_t position = cycleMs == 0 ? 0 : (now % cycleMs);
  float envelope = 0.0f;
  if (position < onMs) {
    float p = static_cast<float>(position) / static_cast<float>(max(static_cast<uint32_t>(1), onMs));
    float fadeIn = smoothstepEdge(0.0f, 0.18f, p);
    float fadeOut = 1.0f - smoothstepEdge(0.76f, 1.0f, p);
    envelope = fadeIn * fadeOut;
  }
  uint8_t floor = static_cast<uint8_t>(strengthLinear(2.0f, 0.0f));
  uint8_t peak = modeCeilingForStrength(96, 255);
  fillScaled(settings.color, clamp8(static_cast<int>(floor + envelope * peak * (0.40f + contrast * 0.60f))));
}

void renderChase(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_CHASE);
  float motion = modeMotionMultiplier(MODE_CHASE);
  float contrast = modeContrastMultiplier(MODE_CHASE);
  uint8_t cometCount = static_cast<uint8_t>(constrain(static_cast<int>(1 + strengthEase(0.0f, 3.3f)), 1, 4));
  float period = max(900.0f, (settings.chasePeriodSec < 1 ? 1 : settings.chasePeriodSec) * 1000.0f / speed);
  float trailLengthMm = strengthLinear(520.0f, 1050.0f);
  float headWidthMm = strengthLinear(120.0f, 54.0f);
  uint8_t backgroundFloor = modeFloorForStrength(10, 26);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float level = backgroundFloor;
    for (uint8_t comet = 0; comet < cometCount; comet++) {
      float offset = static_cast<float>(comet) / static_cast<float>(cometCount);
      float headMm = fractf((static_cast<float>(now) / period) + offset) * static_cast<float>(STRIP_LENGTH_MM);
      float forwardDistance = headMm - xMm;
      if (forwardDistance < 0.0f) {
        forwardDistance += static_cast<float>(STRIP_LENGTH_MM);
      }
      float headDistance = cyclicPhysicalDistanceMm(xMm, headMm);
      float head = expf(-(headDistance * headDistance) / (2.0f * headWidthMm * headWidthMm));
      float trail = forwardDistance <= trailLengthMm ? expf(-forwardDistance / max(1.0f, trailLengthMm * 0.38f)) : 0.0f;
      level += head * strengthLinear(104.0f, 235.0f) + trail * (62.0f + motion * 82.0f);
      if (settings.animationStrength > 190 && comet == 0) {
        float echoDistance = forwardDistance - trailLengthMm * 0.55f;
        if (echoDistance > 0.0f && echoDistance < trailLengthMm * 0.26f) {
          level += (1.0f - echoDistance / (trailLengthMm * 0.26f)) * 38.0f * contrast;
        }
      }
    }
    setPixelWithBrightness(i, settings.color, clampByte(level));
  }
}

void renderWave(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_WAVE);
  float contrast = modeContrastMultiplier(MODE_WAVE);
  float period = (settings.wavePeriodSec < 5 ? 5 : settings.wavePeriodSec) * 1000.0f / max(0.25f, speed);
  float phase = fmodf(now, period) / period * TWO_PI;
  float floor = strengthLinear(118.0f, 38.0f);
  float amplitude = strengthEase(28.0f, 166.0f);
  float secondaryAmount = strengthEase(8.0f, 64.0f);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float a = (sinf((xMm / WAVE_LONG_WAVELENGTH_MM * TWO_PI) - phase) + 1.0f) * 0.5f;
    float b = (sinf((xMm / WAVE_SHORT_WAVELENGTH_MM * TWO_PI) + phase * 0.55f) + 1.0f) * 0.5f;
    float crest = powf(a, strengthLinear(1.35f, 2.45f));
    uint8_t brightness = clampByte(floor + crest * amplitude + b * secondaryAmount * contrast);
    setPixelWithBrightness(i, settings.color, brightness);
  }
}

void renderSlowWave(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_SLOW_WAVE);
  float contrast = modeContrastMultiplier(MODE_SLOW_WAVE);
  uint8_t swellLayerCount = static_cast<uint8_t>(constrain(static_cast<int>(2 + strengthEase(0.0f, 2.0f)), 2, 4));
  float brightnessMap[LED_COUNT];
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    brightnessMap[i] = strengthLinear(34.0f, 18.0f);
  }

  for (uint8_t layer = 0; layer < swellLayerCount; layer++) {
    CrashWave& wave = slowWaves[layer];
    float t = static_cast<float>(now - wave.bornMs) * speed / static_cast<float>(wave.durationMs);

    if (t >= 1.0f) {
      initSlowWave(layer, now);
      t = 0.0f;
    }

    float build = smoothstep(0.0f, 0.72f, t);
    float crash = smoothstep(0.72f, 0.86f, t);
    float fade = 1.0f - smoothstep(0.84f, 1.0f, t);
    float travel = powf(clamp01(t / 0.80f), 2.4f);
    float pos = wave.direction > 0.0f
      ? (-wave.width + travel * (static_cast<float>(LED_COUNT) + wave.width * 2.0f))
      : (static_cast<float>(LED_COUNT) + wave.width - travel * (static_cast<float>(LED_COUNT) + wave.width * 2.0f));
    float energy = (0.15f + build * (0.38f + contrast * 0.52f) + crash * strengthEase(0.06f, 0.55f)) * fade;
    float foamWidth = wave.width * (1.0f + crash * strengthEase(0.25f, 1.8f));

    for (uint16_t i = 0; i < LED_COUNT; i++) {
      float d = fabsf(static_cast<float>(i) - pos);
      float crest = expf(-(d * d) / (2.0f * wave.width * wave.width));
      float foam = expf(-(d * d) / (2.0f * foamWidth * foamWidth)) * crash * 0.55f;
      brightnessMap[i] += (crest + foam) * energy * strengthLinear(88.0f, 210.0f);
    }
  }

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    setPixelWithBrightness(i, settings.color, clampByte(brightnessMap[i]));
  }
}

void renderSlowPulse(uint32_t now) {
  float motion = modeMotionMultiplier(MODE_SLOW_PULSE);
  float contrast = modeContrastMultiplier(MODE_SLOW_PULSE);
  uint8_t activePulseCount = static_cast<uint8_t>(constrain(
    static_cast<int>(1 + static_cast<float>(settings.slowPulseCount) * modeDensityMultiplier(MODE_SLOW_PULSE)),
    1,
    static_cast<int>(MAX_PULSES)
  ));
  uint8_t brightnessMap[LED_COUNT];
  bool used[256] = {false};

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float value = strengthLinear(52.0f, 32.0f);

    for (uint8_t p = 0; p < activePulseCount; p++) {
      Pulse& pulse = pulses[p];
      float t = static_cast<float>(now - pulse.bornMs) * (0.55f + motion) / static_cast<float>(pulse.durationMs);
      if (t >= 1.0f) {
        initPulse(p, now);
        t = 0.0f;
      }

      float centerDrift = sinf((now * (0.000035f + motion * 0.00014f)) + pulse.seed) * pixelsForPhysicalLength(strengthLinear(8.0f, 76.0f));
      float distance = fabsf(static_cast<float>(i) - (pulse.center + centerDrift));
      float spatial = clamp01(1.0f - (distance / (pulse.width * strengthLinear(1.25f, 0.82f))));
      spatial = spatial * spatial * (3.0f - 2.0f * spatial);
      float temporal = sinf(t * PI);
      value = max(value, strengthLinear(52.0f, 34.0f) + spatial * temporal * strengthLinear(52.0f, 216.0f) * (0.45f + contrast * 0.55f));
    }

    value += (static_cast<float>(smoothHash8(i * 37U, now, 713UL)) / 255.0f) * strengthLinear(0.6f, 5.0f);
    uint8_t candidate = clampByte(value);
    uint8_t floor = modeFloorForStrength(26, 40);
    if (candidate < floor) {
      candidate = floor;
    }

    if (LED_COUNT <= 216 && used[candidate]) {
      for (uint16_t offset = 1; offset < 216; offset++) {
        int up = candidate + offset;
        int down = candidate - offset;
        if (up <= 255 && !used[up]) {
          candidate = up;
          break;
        }
        if (down >= floor && !used[down]) {
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
  float speed = modeSpeedMultiplier(MODE_RAINBOW);
  float spread = modeColorSpreadMultiplier(MODE_RAINBOW);
  uint32_t periodMs = static_cast<uint32_t>(max(450.0f, (static_cast<float>(settings.rainbowPeriodSec) * 1000.0f) / max(0.32f, speed)));
  uint16_t offset = static_cast<uint16_t>((now % periodMs) * 256UL / periodMs);
  uint8_t pastelBlend = static_cast<uint8_t>(strengthLinear(150.0f, settings.nightGuardEnabled ? 54.0f : 0.0f));

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    uint8_t wheelPos = static_cast<uint8_t>((normalizedPosition01(i) * (120.0f + spread * 168.0f) + offset)) & 0xFF;
    Rgb color = wheel(wheelPos);
    color = blendColor(color, Rgb{220, 225, 255}, static_cast<float>(pastelBlend) / 255.0f);
    setFramePixel(i, color.r, color.g, color.b);
  }
}

void renderDawnBloom(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_DAWN_BLOOM);
  float motion = modeMotionMultiplier(MODE_DAWN_BLOOM);
  float period = 72000.0f / max(0.20f, speed);
  float sweep = fmodf(now, period) / period;
  Rgb warm = kelvinToRgb(2850);
  Rgb dawn = blendColor(warm, settings.color, strengthLinear(0.18f, 0.62f));
  float shimmerAmount = strengthEase(0.0f, 24.0f);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = normalizedPosition01(i);
    float local = smoothstep(sweep - strengthLinear(0.12f, 0.42f), sweep + strengthLinear(0.09f, 0.28f), x);
    float breathe = (sinf(now * (0.00010f + motion * 0.00030f) + x * TWO_PI) + 1.0f) * 0.5f;
    uint8_t brightness = clampByte(strengthLinear(50.0f, 32.0f) + local * strengthEase(54.0f, 174.0f) + breathe * shimmerAmount);
    setPixelWithBrightness(i, dawn, brightness);
  }
}

void renderMoonTide(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_MOON_TIDE);
  float contrast = modeContrastMultiplier(MODE_MOON_TIDE);
  Rgb moonTint = {190, 220, 255};
  Rgb moon = blendColor(settings.color, moonTint, strengthLinear(0.12f, 0.34f));

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float low = (sinf(xMm / MOON_TIDE_LONG_WAVELENGTH_MM * TWO_PI - now * 0.00031f * speed) + 1.0f) * 0.5f;
    float high = (sinf(xMm / MOON_TIDE_RIPPLE_WAVELENGTH_MM * TWO_PI + now * 0.00019f * speed) + 1.0f) * 0.5f;
    uint8_t brightness = clampByte(strengthLinear(44.0f, 26.0f) + low * strengthEase(28.0f, 126.0f) + high * strengthEase(5.0f, 58.0f) * contrast);
    setPixelWithBrightness(i, moon, brightness);
  }
}

void renderVelvetAurora(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_VELVET_AURORA);
  float contrast = modeContrastMultiplier(MODE_VELVET_AURORA);
  float colorSpread = modeColorSpreadMultiplier(MODE_VELVET_AURORA);
  Rgb coolTint = {120, 190, 255};
  Rgb warmTint = {255, 210, 155};
  Rgb cool = blendColor(settings.color, coolTint, 0.10f + colorSpread * 0.30f);
  Rgb warm = blendColor(settings.color, warmTint, 0.08f + colorSpread * 0.22f);
  bool usePalette = settings.paletteEnabled && currentModeSupportsPalette() && getActivePalette() != nullptr;

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float ribbon = (sinf(xMm / 2500.0f * TWO_PI + now * 0.00023f * speed) + 1.0f) * 0.5f;
    float veil = (sinf(xMm / 5200.0f * TWO_PI - now * 0.00009f * speed) + 1.0f) * 0.5f;
    float fold = (sinf(xMm / 720.0f * TWO_PI + now * 0.00012f * speed) + 1.0f) * 0.5f;
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
    uint8_t brightness = clampByte(strengthLinear(36.0f, 42.0f) + powf(ribbon, 1.5f) * strengthEase(36.0f, 126.0f) + veil * strengthEase(14.0f, 54.0f) + fold * 22.0f * contrast);
    setPixelWithBrightness(i, color, brightness);
  }
}

void renderLanternDrift(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_LANTERN_DRIFT);
  float motion = modeMotionMultiplier(MODE_LANTERN_DRIFT);
  float contrast = modeContrastMultiplier(MODE_LANTERN_DRIFT);
  Rgb lantern = blendColor(settings.color, kelvinToRgb(2400), 0.22f);
  uint8_t lanternCount = static_cast<uint8_t>(constrain(static_cast<int>(2 + strengthEase(1.0f, 4.0f)), 3, 6));
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float value = strengthLinear(42.0f, 26.0f);
    for (uint8_t layer = 0; layer < lanternCount; layer++) {
      float speedMmPerMs = (0.006f + layer * 0.004f) * speed;
      float spanMm = static_cast<float>(STRIP_LENGTH_MM) + LANTERN_DRIFT_MARGIN_MM * 2.0f;
      float centerMm = fmodf(now * speedMmPerMs + layer * 380.0f, spanMm) - LANTERN_DRIFT_MARGIN_MM;
      centerMm += sinf(now * (0.00003f + layer * 0.000011f) * speed + layer) * PAPER_LANTERN_SWAY_MM * motion;
      float width = LANTERN_GLOW_RADIUS_MM + layer * 34.0f + strengthLinear(80.0f, 18.0f);
      float d = fabsf(xMm - centerMm);
      value += expf(-(d * d) / (2.0f * width * width)) * (strengthLinear(36.0f, 86.0f) + layer * 9.0f) * (0.55f + contrast * 0.45f);
    }
    float breathe = (sinf(now * (0.00007f + motion * 0.00017f) + xMm / 72.0f) + 1.0f) * 0.5f;
    setPixelWithBrightness(i, lantern, clampByte(value + breathe * strengthEase(2.0f, 24.0f)));
  }
}

void renderRainGlass(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_RAIN_GLASS);
  float contrast = modeContrastMultiplier(MODE_RAIN_GLASS);
  float density = modeDensityMultiplier(MODE_RAIN_GLASS);
  Rgb glassTint = {210, 235, 255};
  Rgb glass = blendColor(settings.color, glassTint, 0.18f);
  float dropletRadiusMm = getMmPerPixel() * static_cast<float>(mmToPixels(strengthLinear(12.0f, RAIN_GLASS_DROPLET_RADIUS_MM)));
  float trailLengthMm = getMmPerPixel() * static_cast<float>(mmToPixels(strengthLinear(38.0f, RAIN_GLASS_TRAIL_LENGTH_MM * 1.8f)));
  uint8_t dropletCount = static_cast<uint8_t>(constrain(static_cast<int>(2 + 8.0f * density), 2, 10));

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float value = strengthLinear(34.0f, 22.0f);
    for (uint8_t layer = 0; layer < dropletCount; layer++) {
      float life = fractf((static_cast<float>(now) * (0.000014f + layer * 0.000002f) * speed) + static_cast<float>(hash8(layer * 37U)) / 255.0f);
      float spawn = static_cast<float>(hash8(layer * 61U + 13U)) / 255.0f;
      float fade = smoothstepEdge(0.02f, 0.18f, life) * (1.0f - smoothstepEdge(0.72f, 1.0f, life));
      float speedMmPerMs = (0.010f + layer * 0.004f) * speed;
      float spanMm = static_cast<float>(STRIP_LENGTH_MM) + trailLengthMm * 2.0f;
      float positionMm = fmodf(now * speedMmPerMs + (spawn + layer * 0.21f) * spanMm, spanMm) - trailLengthMm;
      float d = fabsf(xMm - positionMm);
      float bead = expf(-(d * d) / (2.0f * dropletRadiusMm * dropletRadiusMm));
      float trail = clamp01((positionMm - xMm) / trailLengthMm) * expf(-fabsf(positionMm - xMm) / trailLengthMm);
      float dissolve = 1.0f - smoothstepEdge(0.74f, 1.0f, life);
      value += (bead * strengthEase(42.0f, 138.0f) + trail * strengthEase(18.0f, 72.0f)) * fade * dissolve * (0.55f + contrast * 0.45f);
    }
    float pane = (sinf(xMm / 62.0f + now * 0.00008f * speed) + 1.0f) * strengthEase(2.0f, 13.0f);
    setPixelWithBrightness(i, glass, clampByte(value + pane));
  }
}

void renderSatinBreathing(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_SATIN_BREATHING);
  float contrast = modeContrastMultiplier(MODE_SATIN_BREATHING);
  uint32_t breathPeriod = static_cast<uint32_t>(strengthLinear(16000.0f, 8500.0f) / max(0.28f, speed));
  float phase = phaseFromMillis(now, breathPeriod);
  float envelope;
  if (phase < 0.38f) {
    envelope = easeInOut01(phase / 0.38f);
  } else if (phase < 0.50f) {
    envelope = 1.0f;
  } else {
    envelope = 1.0f - easeInOut01((phase - 0.50f) / 0.50f);
  }
  uint8_t baseLevel = clamp8(static_cast<int>(strengthLinear(104.0f, 46.0f) + envelope * strengthEase(18.0f, 174.0f)));
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warmPeak = warmWhiteApprox(2850);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    RgbPixel color = sampleActivePaletteOrColor(i, maxPosition);
    color = blendRgb(color, warmPeak, static_cast<uint8_t>(envelope * strengthEase(8.0f, 40.0f)));
    int texture = static_cast<int>((static_cast<uint16_t>(smoothHash8(i * 11U, now, 997UL)) * static_cast<uint16_t>(strengthEase(4.0f, 22.0f))) / 256U) - static_cast<int>(strengthEase(2.0f, 11.0f));
    texture = static_cast<int>(texture * (0.42f + contrast * 0.58f));
    setFramePixelRgb(i, scaleRgb(color, clamp8(baseLevel + texture)));
  }
}

void renderPaperLanternRow(uint32_t now) {
  float motion = modeMotionMultiplier(MODE_PAPER_LANTERN_ROW);
  float contrast = modeContrastMultiplier(MODE_PAPER_LANTERN_ROW);
  uint8_t lanternCount = static_cast<uint8_t>(constrain(
    static_cast<int>((static_cast<float>(STRIP_LENGTH_MM) / PAPER_LANTERN_SPACING_MM) + 0.5f + strengthEase(0.0f, 1.0f)),
    3,
    8
  ));
  float maxPositionMm = static_cast<float>(STRIP_LENGTH_MM);
  float lanternGlowMm = getMmPerPixel() * static_cast<float>(mmToPixels(LANTERN_GLOW_RADIUS_MM));
  uint16_t maxPalettePosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warm = warmWhiteApprox(2550);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float level = 18.0f;
    RgbPixel color = warm;
    for (uint8_t lantern = 0; lantern < lanternCount; lantern++) {
      float slot = lanternCount <= 1 ? 0.0f : static_cast<float>(lantern) / static_cast<float>(lanternCount - 1);
      float sway = sinf(now * (0.000028f + motion * 0.00010f + lantern * 0.000014f) + lantern * 1.7f) * PAPER_LANTERN_SWAY_MM * (0.2f + motion);
      float centerMm = slot * maxPositionMm + sway;
      float radius = max(lanternGlowMm, maxPositionMm / (lanternCount * strengthLinear(1.35f, 1.95f)));
      float d = fabsf(xMm - centerMm);
      float falloff = clamp01(1.0f - (d / radius));
      falloff = falloff * falloff * (3.0f - 2.0f * falloff);
      float breathe = 1.0f - strengthEase(0.02f, 0.28f) + strengthEase(0.02f, 0.28f) * (sineEase8(phaseFromMillis(now + lantern * 733UL, 9000UL + lantern * 1100UL)) / 255.0f);
      level += falloff * breathe * strengthEase(80.0f, 158.0f) * (0.55f + contrast * 0.45f);
      if (falloff > 0.45f) {
        color = sampleActivePaletteOrColor(static_cast<uint16_t>(slot * maxPalettePosition), maxPalettePosition);
      }
    }
    color = blendRgb(color, warm, 150);
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderLibraryLamp(uint32_t now) {
  float motion = modeMotionMultiplier(MODE_LIBRARY_LAMP);
  float contrast = modeContrastMultiplier(MODE_LIBRARY_LAMP);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warm = warmWhiteApprox(settings.usingWhiteTemperature ? settings.whiteKelvin : 3400);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    RgbPixel paletteColor = sampleActivePaletteOrColor(i, maxPosition);
    RgbPixel color = settings.paletteEnabled && currentModeSupportsPalette()
      ? blendRgb(warm, paletteColor, 34)
      : warm;
    uint8_t slow = triangleWave8(phaseFromMillis(now + hash8(i * 19U) * 73UL, static_cast<uint32_t>(150000UL / max(0.05f, 0.4f + motion))));
    int texture = static_cast<int>((hash8(i * 23U) % 9) - 4) * contrast + static_cast<int>(slow / strengthLinear(200.0f, 90.0f));
    setFramePixelRgb(i, scaleRgb(color, clamp8(224 + texture)));
  }
}

void renderSolarNoon(uint32_t now) {
  float motion = modeMotionMultiplier(MODE_SOLAR_NOON);
  float contrast = modeContrastMultiplier(MODE_SOLAR_NOON);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warm = warmWhiteApprox(4550);
  RgbPixel cool = warmWhiteApprox(settings.usingWhiteTemperature ? settings.whiteKelvin : 5400);
  float drift = (sinf(now * (0.000004f + motion * 0.000032f)) + 1.0f) * 0.5f;
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    uint8_t amount = static_cast<uint8_t>((static_cast<uint32_t>(i) * 255UL) / maxPosition);
    amount = lerp8(amount, 255 - amount, static_cast<uint8_t>(drift * strengthEase(4.0f, 44.0f)));
    RgbPixel color = blendRgb(warm, cool, amount);
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      color = blendRgb(color, sampleActivePaletteOrColor(i, maxPosition), static_cast<uint8_t>(8.0f + contrast * 16.0f));
    }
    setFramePixelRgb(i, scaleRgb(color, 245));
  }
}

void renderGoldenHour(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_GOLDEN_HOUR);
  float contrast = modeContrastMultiplier(MODE_GOLDEN_HOUR);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel sun = {255, 174, 72};
  RgbPixel cream = warmWhiteApprox(3100);
  float drift = (sinf(now * 0.000035f * speed) + 1.0f) * 0.5f;
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = normalizedPosition01(i);
    float source = clamp01(1.0f - fabsf(x - drift) * strengthLinear(1.05f, 1.70f));
    uint8_t amount = static_cast<uint8_t>((1.0f - source) * strengthEase(90.0f, 190.0f));
    RgbPixel color = blendRgb(sun, cream, amount);
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      color = blendRgb(color, sampleActivePaletteOrColor(i, maxPosition), 42);
    }
    uint8_t level = clamp8(static_cast<int>(strengthLinear(104.0f, 76.0f) + source * strengthEase(54.0f, 148.0f) + contrast * 12.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderDreamAquarium(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_DREAM_AQUARIUM);
  float contrast = modeContrastMultiplier(MODE_DREAM_AQUARIUM);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel deep = {18, 66, 100};
  RgbPixel aqua = {62, 190, 190};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float waveA = (sinf(xMm / 3300.0f * TWO_PI - now * 0.00027f * speed) + 1.0f) * 0.5f;
    float waveB = (sinf(xMm / 1080.0f * TWO_PI + now * 0.00013f * speed) + 1.0f) * 0.5f;
    float caustic = powf((sinf(xMm / 420.0f * TWO_PI + now * 0.00019f * speed) + 1.0f) * 0.5f, 4.0f);
    RgbPixel color = blendRgb(deep, aqua, clampByte(72.0f + waveA * strengthEase(50.0f, 128.0f)));
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      uint16_t drifted = static_cast<uint16_t>((i + (now / 210UL)) % LED_COUNT);
      color = blendRgb(color, sampleActivePaletteOrColor(drifted, maxPosition), 96);
    }
    uint8_t level = clamp8(static_cast<int>(strengthLinear(58.0f, 42.0f) + waveA * strengthEase(28.0f, 78.0f) + waveB * strengthEase(8.0f, 44.0f) + caustic * contrast * 34.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderCircuitGlow(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_CIRCUIT_GLOW);
  float contrast = modeContrastMultiplier(MODE_CIRCUIT_GLOW);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel base = {6, 24, 34};
  RgbPixel trace = settings.paletteEnabled && currentModeSupportsPalette()
    ? sampleActivePaletteOrColor((now / 80UL) % LED_COUNT, maxPosition)
    : RgbPixel{22, 255, 162};
  uint8_t signalCount = static_cast<uint8_t>(constrain(static_cast<int>(1 + 5.0f * modeDensityMultiplier(MODE_CIRCUIT_GLOW)), 1, 6));
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float level = strengthLinear(18.0f, 26.0f) + static_cast<float>(hash8(i * 41U) % 14);
    for (uint8_t lane = 0; lane < signalCount; lane++) {
      uint16_t period = static_cast<uint16_t>((2600U + lane * 520U) / max(0.45f, speed));
      uint16_t head = static_cast<uint16_t>(((now + lane * 417UL) % period) * LED_COUNT / period);
      float distance = cyclicPixelDistance(static_cast<float>(i), static_cast<float>(head));
      float traceWidth = pixelsForPhysicalLength(strengthLinear(24.0f, 68.0f));
      if (distance < traceWidth) {
        float trail = (traceWidth - distance) / max(1.0f, traceWidth);
        level += trail * trail * strengthEase(34.0f, 86.0f) * (0.45f + contrast * 0.55f);
      }
    }
    RgbPixel color = blendRgb(base, trace, clamp8(static_cast<int>(level * 1.35f)));
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level + 40.0f))));
  }
}

void renderWindowSunrise(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_WINDOW_SUNRISE);
  float contrast = modeContrastMultiplier(MODE_WINDOW_SUNRISE);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float reach = phaseFromMillis(now, static_cast<uint32_t>(150000UL / max(0.2f, speed)));
  RgbPixel amber = {255, 134, 54};
  RgbPixel daylight = warmWhiteApprox(3650);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float boundary = reach * 1.28f - 0.12f;
    float lit = 1.0f - clamp01((x - boundary) / 0.48f);
    lit = lit * lit * (3.0f - 2.0f * lit);
    uint8_t shimmer = sineApprox8(now + i * 37UL, static_cast<uint32_t>(strengthLinear(26000.0f, 12000.0f)), hash8(i * 13U) * 257U);
    RgbPixel palette = sampleNamedOrActivePalette("peach-sky", i, maxPosition);
    RgbPixel sunrise = blendRgb(amber, daylight, clamp8(static_cast<int>(reach * strengthEase(70.0f, 150.0f) + x * 42.0f)));
    sunrise = blendRgb(sunrise, palette, 26);
    uint8_t level = clamp8(static_cast<int>(strengthLinear(34.0f, 20.0f) + lit * strengthEase(70.0f, 190.0f) + static_cast<int>(shimmer / strengthLinear(80.0f, 12.0f)) * contrast));
    setFramePixelRgb(i, scaleRgb(sunrise, level));
  }
}

void renderEmberQuilt(uint32_t now) {
  float motion = modeMotionMultiplier(MODE_EMBER_QUILT);
  float contrast = modeContrastMultiplier(MODE_EMBER_QUILT);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel warm = {255, 128, 55};
  RgbPixel cream = warmWhiteApprox(2550);
  uint8_t patchCount = static_cast<uint8_t>(constrain(static_cast<int>(3 + strengthEase(0.0f, 3.0f)), 3, 6));
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float level = strengthLinear(48.0f, 30.0f);
    RgbPixel color = blendRgb(warm, cream, 48);
    for (uint8_t zone = 0; zone < patchCount; zone++) {
      float centerMm = (static_cast<float>(zone) + 0.5f) * static_cast<float>(STRIP_LENGTH_MM) / static_cast<float>(patchCount);
      centerMm += sinf(now * (0.000012f + motion * 0.000026f + zone * 0.000004f) + zone * 1.9f) * strengthEase(16.0f, 92.0f);
      float width = EMBER_QUILT_GLOW_RADIUS_MM + zone * 55.0f;
      float d = fabsf(xMm - centerMm);
      float falloff = clamp01(1.0f - d / width);
      falloff = falloff * falloff * (3.0f - 2.0f * falloff);
      float breathe = 1.0f - strengthEase(0.04f, 0.28f) + strengthEase(0.04f, 0.28f) * (sineApprox8(now + zone * 991UL, 38000UL + zone * 3100UL, 0) / 255.0f);
      level += falloff * breathe * strengthEase(34.0f, 78.0f) * (0.55f + contrast * 0.45f);
      if (falloff > 0.35f) {
        RgbPixel sampled = sampleNamedOrActivePalette("candle", static_cast<uint16_t>(zone * maxPosition / 5), maxPosition);
        color = blendRgb(color, sampled, static_cast<uint8_t>(falloff * 58.0f));
      }
    }
    uint8_t texture = static_cast<uint8_t>((static_cast<uint16_t>(smoothHash8(i * 29U, now, 1600UL)) * static_cast<uint16_t>(strengthEase(2.0f, 14.0f))) / 256U);
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level) + texture)));
  }
}

void renderPorcelainSky(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_PORCELAIN_SKY);
  float contrast = modeContrastMultiplier(MODE_PORCELAIN_SKY);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel porcelain = coolWhiteApprox(5100);
  RgbPixel pearl = warmWhiteApprox(4200);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float drift = (sinf(x * TWO_PI * 0.72f + now * 0.000022f * speed) + 1.0f) * 0.5f;
    float cloud = (sinf(x * TWO_PI * 1.9f - now * 0.000014f * speed) + 1.0f) * 0.5f;
    RgbPixel color = blendRgb(porcelain, pearl, clamp8(static_cast<int>(24.0f + drift * strengthEase(28.0f, 78.0f))));
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      color = blendRgb(color, sampleActivePaletteOrColor(i, maxPosition), 34);
    }
    uint8_t level = clamp8(static_cast<int>(188.0f - contrast * 16.0f + cloud * strengthEase(8.0f, 42.0f)));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderHearthline(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_HEARTHLINE);
  float contrast = modeContrastMultiplier(MODE_HEARTHLINE);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel base = {150, 54, 18};
  RgbPixel amber = {255, 150, 54};
  uint8_t bandCount = static_cast<uint8_t>(constrain(static_cast<int>(2 + strengthEase(0.0f, 3.0f)), 2, 5));
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float level = strengthLinear(58.0f, 40.0f);
    RgbPixel color = blendRgb(base, amber, 78);
    for (uint8_t band = 0; band < bandCount; band++) {
      float period = (25000.0f + band * 9300.0f) / max(0.24f, speed);
      float marginMm = HEARTHLINE_BAND_WIDTH_MM * 1.5f;
      float spanMm = static_cast<float>(STRIP_LENGTH_MM) + marginMm * 2.0f;
      float centerMm = fmodf((now + band * 5100UL) / period * spanMm, spanMm) - marginMm;
      float width = HEARTHLINE_BAND_WIDTH_MM + band * 28.0f;
      float d = fabsf(xMm - centerMm);
      float falloff = expf(-(d * d) / (2.0f * width * width));
      level += falloff * (strengthEase(22.0f, 66.0f) + band * 10.0f) * (0.55f + contrast * 0.45f);
      if (falloff > 0.18f) {
        RgbPixel sampled = sampleNamedOrActivePalette("warm-linen", static_cast<uint16_t>((i + band * 7) % LED_COUNT), maxPosition);
        color = blendRgb(color, sampled, static_cast<uint8_t>(falloff * 48.0f));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderEveningCurrent(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_EVENING_CURRENT);
  float contrast = modeContrastMultiplier(MODE_EVENING_CURRENT);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel dusk = {74, 92, 136};
  RgbPixel warm = {255, 166, 100};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float swell = (sinf(xMm / 4200.0f * TWO_PI - now * 0.000105f * speed) + 1.0f) * 0.5f;
    float ripple = (sinf(xMm / 1400.0f * TWO_PI + now * 0.000061f * speed) + 1.0f) * 0.5f;
    RgbPixel color = settings.paletteEnabled && currentModeSupportsPalette()
      ? sampleActivePaletteOrColor(static_cast<uint16_t>((i + now / 280UL) % LED_COUNT), maxPosition)
      : blendRgb(dusk, warm, 54);
    color = blendRgb(color, warm, static_cast<uint8_t>(swell * 32.0f));
    uint8_t level = clamp8(static_cast<int>(strengthLinear(72.0f, 52.0f) + swell * strengthEase(30.0f, 92.0f) + ripple * strengthEase(6.0f, 38.0f) * contrast));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderStarlitLinen(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_STARLIT_LINEN);
  float contrast = modeContrastMultiplier(MODE_STARLIT_LINEN);
  float density = modeDensityMultiplier(MODE_STARLIT_LINEN);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  uint8_t starCount = static_cast<uint8_t>(constrain(static_cast<int>(3 + (STRIP_LENGTH_MM / 420) * density), 3, 15));
  RgbPixel base = sampleNamedOrActivePalette("warm-linen", 0, maxPosition);
  RgbPixel highlight = warmWhiteApprox(3000);
  float starRadius = STAR_HALO_RADIUS_MM * strengthLinear(0.70f, 1.85f);
  float highlightBrightness = strengthEase(18.0f, 72.0f);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float level = strengthLinear(30.0f, 22.0f) + static_cast<float>(hash8(i * 17U) % 8);
    RgbPixel color = blendRgb(base, sampleActivePaletteOrColor(i, maxPosition), settings.paletteEnabled ? 46 : 0);
    for (uint8_t star = 0; star < starCount; star++) {
      float centerMm = (static_cast<float>(hash8(star * 47U)) * static_cast<float>(STRIP_LENGTH_MM)) / 255.0f;
      float d = cyclicPhysicalDistanceMm(xMm, centerMm);
      float soft = expf(-(d * d) / (2.0f * starRadius * starRadius));
      uint8_t phase = sineApprox8(now + star * 2200UL, static_cast<uint32_t>((36000UL + star * 1400UL) / max(0.18f, speed)), hash8(star * 31U) * 257U);
      float fade = static_cast<float>(phase) / 255.0f;
      level += soft * fade * highlightBrightness * (0.55f + contrast * 0.45f);
      float neighbor = expf(-(d * d) / (2.0f * (starRadius * 2.2f) * (starRadius * 2.2f)));
      level += neighbor * fade * highlightBrightness * 0.18f;
      if (soft > 0.08f) {
        color = blendRgb(color, highlight, static_cast<uint8_t>(soft * phase / 5));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderDeepSeaGlass(uint32_t now) {
  float speed = modeSpeedMultiplier(MODE_DEEP_SEA_GLASS);
  float contrast = modeContrastMultiplier(MODE_DEEP_SEA_GLASS);
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  RgbPixel deep = {8, 42, 76};
  RgbPixel glass = {74, 176, 194};
  RgbPixel glint = {170, 232, 245};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float depth = (sinf(xMm / 3700.0f * TWO_PI - now * 0.000074f * speed) + 1.0f) * 0.5f;
    float shimmer = (sinf(xMm / (DEEP_SEA_CAUSTIC_WAVELENGTH_MM * strengthLinear(1.45f, 0.72f)) * TWO_PI + now * 0.000121f * speed) + 1.0f) * 0.5f;
    float glintPulse = powf((sinf(xMm / 460.0f * TWO_PI - now * 0.00023f * speed) + 1.0f) * 0.5f, 8.0f);
    RgbPixel color = blendRgb(deep, glass, clamp8(static_cast<int>(60.0f + depth * strengthEase(52.0f, 124.0f))));
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      color = blendRgb(color, sampleActivePaletteOrColor(static_cast<uint16_t>((i + now / 360UL) % LED_COUNT), maxPosition), 72);
    }
    color = blendRgb(color, glint, static_cast<uint8_t>((shimmer * strengthEase(6.0f, 30.0f)) + glintPulse * contrast * 42.0f));
    uint8_t level = clamp8(static_cast<int>(strengthLinear(46.0f, 38.0f) + depth * strengthEase(22.0f, 76.0f) + shimmer * strengthEase(4.0f, 34.0f) + glintPulse * contrast * 28.0f));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderQuietNorthernLights(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float t = static_cast<float>(now) * 0.001f;
  float rawStrength = animationStrengthRaw01();
  float strength = animationStrength01();
  const uint8_t MAX_AURORA_RIBBONS = 5;
  struct AuroraRibbon {
    bool active;
    float center;
    float width;
    float velocity;
    float acceleration;
    float opacity;
    float targetOpacity;
    float life;
    float lifeDuration;
    float foldPhase;
    float foldSpeed;
    float colorBlend;
    int8_t direction;
    uint8_t seed;
  };
  AuroraRibbon ribbons[MAX_AURORA_RIBBONS];
  uint8_t activeRibbonCount = rawStrength < 0.18f ? 1 :
                              rawStrength < 0.48f ? 2 :
                              rawStrength < 0.78f ? 3 :
                              rawStrength < 0.94f ? 4 : 5;
  RgbPixel nightBase = {3, 8, 22};
  RgbPixel lowerGlow = {7, 20, 42};
  RgbPixel anchor = {settings.color.r, settings.color.g, settings.color.b};
  uint8_t colorSpread = strengthByte(24, 156);
  RgbPixel auroraGreen = blendRgb(anchor, RgbPixel{64, 230, 145}, colorSpread);
  RgbPixel cyan = blendRgb(anchor, RgbPixel{70, 210, 245}, clamp8(colorSpread + 16));
  RgbPixel teal = blendRgb(anchor, RgbPixel{75, 245, 210}, clamp8(colorSpread + 8));
  RgbPixel violet = blendRgb(anchor, RgbPixel{125, 105, 245}, clamp8(colorSpread + static_cast<int>(rawStrength * 34.0f)));
  RgbPixel pale = blendRgb(anchor, RgbPixel{185, 250, 225}, strengthByte(18, 82));
  float baseLevel = strengthEase(9.0f, 18.0f);
  float baseBreathAmount = strengthEase(3.0f, 8.0f);
  float ribbonBrightness = strengthEase(26.0f, 74.0f);
  float ribbonBlend = strengthEase(0.16f, 0.62f);
  float foldDepth = strengthEase(0.06f, 0.34f);
  float peakLimit = strengthEase(72.0f, 158.0f);
  float shimmerDepth = strengthEase(0.0f, 4.5f);
  float speed = modeSpeedMultiplier(MODE_QUIET_NORTHERN_LIGHTS);

  for (uint8_t slot = 0; slot < MAX_AURORA_RIBBONS; slot++) {
    uint8_t seed = hash8(static_cast<uint16_t>(slot * 73U + 41U));
    float seedUnit = static_cast<float>(seed) / 255.0f;
    float lifeDuration = strengthLinear(58.0f, 24.0f) + seedUnit * 18.0f + static_cast<float>(slot) * 3.0f;
    float lifePosition = (t * max(0.10f, speed) / lifeDuration) + seedUnit * 0.83f + static_cast<float>(slot) * 0.21f;
    float life = fractf(lifePosition);
    bool respawn = life < 0.06f;
    uint16_t generation = static_cast<uint16_t>(floorf(lifePosition));
    uint8_t generationSeed = hash8(static_cast<uint16_t>(seed + generation * 31U + slot * 17U + (respawn ? 9U : 0U)));
    float generationUnit = static_cast<float>(generationSeed) / 255.0f;
    int8_t direction = ((generationSeed + slot) & 1U) ? -1 : 1;
    float fadeIn = smoothstepEdge(0.00f, 0.22f, life);
    float fadeOut = 1.0f - smoothstepEdge(0.70f, 1.00f, life);
    float targetOpacity = (strengthLinear(0.32f, 0.95f) + seedUnit * 0.10f) * fadeIn * fadeOut;
    float reverseWave = sinf(t * (0.045f + generationUnit * 0.028f) + seedUnit * TWO_PI);
    float reverse = reverseWave > 0.42f ? -0.65f : 1.0f;
    float velocity = direction * reverse * (strengthLinear(18.0f, 86.0f) + generationUnit * 28.0f) * speed;
    float acceleration = direction * sinf(t * (0.012f + generationUnit * 0.010f) + seedUnit * TWO_PI) * strengthEase(1.2f, 9.0f);
    float center = fractf(seedUnit + (velocity * t + 0.5f * acceleration * t * t) / max(1.0f, static_cast<float>(STRIP_LENGTH_MM)));

    ribbons[slot].active = slot < activeRibbonCount;
    ribbons[slot].center = center * static_cast<float>(STRIP_LENGTH_MM);
    ribbons[slot].width = strengthLinear(880.0f, 440.0f) + generationUnit * strengthLinear(220.0f, 460.0f);
    ribbons[slot].velocity = velocity;
    ribbons[slot].acceleration = acceleration;
    ribbons[slot].opacity = targetOpacity;
    ribbons[slot].targetOpacity = targetOpacity;
    ribbons[slot].life = life;
    ribbons[slot].lifeDuration = lifeDuration;
    ribbons[slot].foldPhase = seedUnit * TWO_PI + generationUnit * 1.7f;
    ribbons[slot].foldSpeed = (0.018f + generationUnit * 0.032f) * max(0.24f, speed);
    ribbons[slot].colorBlend = fractf(seedUnit + generationUnit * 0.31f + static_cast<float>(slot) * 0.17f);
    ribbons[slot].direction = direction;
    ribbons[slot].seed = seed;
  }

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = normalizedPosition01(i);
    float xMm = physicalXmm(i);
    float skyBreath = 0.5f + 0.5f * sinf(x * TWO_PI * 0.42f + t * (0.016f + strength * 0.012f));
    RgbPixel color = blendRgb(nightBase, lowerGlow, clampByte(22.0f + skyBreath * 44.0f));
    color = blendRgb(color, anchor, strengthByte(5, 18));
    float level = baseLevel + skyBreath * baseBreathAmount;

    for (uint8_t slot = 0; slot < MAX_AURORA_RIBBONS; slot++) {
      AuroraRibbon& ribbon = ribbons[slot];
      if (!ribbon.active || ribbon.opacity <= 0.003f) {
        continue;
      }
      float d = cyclicPhysicalDistanceMm(xMm, ribbon.center);
      float edge = expf(-(d * d) / (2.0f * ribbon.width * ribbon.width));
      float foldFrequency = TWO_PI / max(180.0f, ribbon.width * 0.58f);
      float fold = 0.74f + foldDepth * sinf(xMm * foldFrequency + t * ribbon.foldSpeed + ribbon.foldPhase);
      float curtain = 0.88f + 0.12f * sinf(xMm * foldFrequency * 2.9f - t * ribbon.foldSpeed * 0.66f + ribbon.foldPhase * 1.7f);
      float dissolve = smoothstepEdge(0.02f, 0.20f, ribbon.life) * (1.0f - smoothstepEdge(0.70f, 1.0f, ribbon.life));
      float auroraRibbonContribution = clamp01(edge * fold * curtain * ribbon.opacity * dissolve);
      if (auroraRibbonContribution <= 0.004f) {
        continue;
      }
      RgbPixel waveColor;
      if (settings.paletteEnabled && currentModeSupportsPalette()) {
        float paletteTravel = fractf(x * 0.46f + ribbon.colorBlend + ribbon.life * 0.19f);
        uint16_t palettePosition = static_cast<uint16_t>(paletteTravel * static_cast<float>(maxPosition));
        waveColor = sampleActivePaletteOrColor(palettePosition, maxPosition);
      } else if (slot % 5 == 0) {
        waveColor = blendRgb(auroraGreen, cyan, clampByte(48.0f + ribbon.colorBlend * 72.0f));
      } else if (slot % 5 == 1) {
        waveColor = blendRgb(cyan, teal, clampByte(46.0f + static_cast<float>(ribbon.seed) / 255.0f * 70.0f));
      } else if (slot % 5 == 2) {
        waveColor = blendRgb(teal, violet, clampByte(34.0f + rawStrength * 72.0f));
      } else if (slot % 5 == 3) {
        waveColor = blendRgb(auroraGreen, pale, clampByte(36.0f + ribbon.colorBlend * 64.0f));
      } else {
        waveColor = blendRgb(anchor, violet, clampByte(38.0f + rawStrength * 88.0f));
      }
      color = addSoftRgb(color, waveColor, auroraRibbonContribution * ribbonBlend);
      level += auroraRibbonContribution * ribbonBrightness;
    }

    float shimmer = 0.5f + 0.5f * sinf(x * TWO_PI * (4.4f + strength * 1.8f) + t * (0.012f + strength * 0.018f));
    level += shimmer * shimmerDepth * clamp01(level / peakLimit);
    color = blendRgb(color, pale, clampByte(shimmer * shimmerDepth * 1.2f));
    if (level > peakLimit) {
      level = peakLimit + (level - peakLimit) * 0.18f;
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderCandleHall(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float speed = modeSpeedMultiplier(MODE_CANDLE_HALL);
  float motion = modeMotionMultiplier(MODE_CANDLE_HALL);
  float contrast = modeContrastMultiplier(MODE_CANDLE_HALL);
  uint8_t candleCount = static_cast<uint8_t>(constrain(
    static_cast<int>((static_cast<float>(STRIP_LENGTH_MM) / (CANDLE_SPACING_MIN_MM - strengthEase(0.0f, 140.0f))) + 0.5f),
    3,
    7
  ));
  RgbPixel shadow = {42, 16, 8};
  RgbPixel flame = warmWhiteApprox(2350);
  float flickerAmount = (static_cast<float>(constrain(settings.candleHallFlickerAmount, 0, 20)) / 20.0f) * strengthEase(0.22f, 1.0f);
  float candleCoreFloorMm = getMmPerPixel() * static_cast<float>(mmToPixels(CANDLE_CORE_WIDTH_MM));
  float candleGlowFloorMm = getMmPerPixel() * static_cast<float>(mmToPixels(CANDLE_GLOW_RADIUS_MM));
  float centers[7];
  float widths[7];
  float coreWidths[7];
  float candleLevels[7];
  RgbPixel candleColors[7];

  for (uint8_t candle = 0; candle < candleCount; candle++) {
    uint8_t candleSeed = hash8(candle * 53U + 17U);
    float slot = candleCount <= 1 ? 0.0f : static_cast<float>(candle) / static_cast<float>(candleCount - 1);
    float brightnessVariance = 0.26f + flickerAmount * 0.30f;
    float brightnessScale = 1.0f - brightnessVariance * 0.5f + (static_cast<float>(hash8(candleSeed + 19U)) / 255.0f) * brightnessVariance;
    float wobbleAmplitude = (0.25f + flickerAmount * 1.15f + (static_cast<float>(hash8(candleSeed + 41U)) / 255.0f) * 0.90f) * motion;
    float wobbleRate = (0.000018f + static_cast<float>(hash8(candleSeed + 61U)) * 0.00000009f) * speed;
    uint16_t primaryPhase = static_cast<uint16_t>(hash8(candleSeed + 79U) * 257U);
    uint16_t secondaryPhase = static_cast<uint16_t>(hash8(candleSeed + 97U) * 257U);
    uint32_t primaryPeriod = static_cast<uint32_t>((9000UL + static_cast<uint32_t>(hash8(candleSeed + 113U)) * 23UL) / max(0.25f, speed));
    uint32_t secondaryPeriod = static_cast<uint32_t>((1800UL + static_cast<uint32_t>(hash8(candleSeed + 131U)) * 17UL) / max(0.25f, speed));

    centers[candle] = slot * static_cast<float>(STRIP_LENGTH_MM);
    centers[candle] += sinf(now * wobbleRate + static_cast<float>(primaryPhase) * 0.000095f) * wobbleAmplitude * 14.0f;
    centers[candle] += sinf(now * wobbleRate * 2.7f + static_cast<float>(secondaryPhase) * 0.00012f) * wobbleAmplitude * 4.0f * flickerAmount;
    coreWidths[candle] = max(candleCoreFloorMm, CANDLE_CORE_WIDTH_MM + strengthEase(3.0f, 22.0f) + (static_cast<float>(hash8(candleSeed + 149U)) / 255.0f) * strengthEase(2.0f, 16.0f));
    widths[candle] = max(candleGlowFloorMm, CANDLE_GLOW_RADIUS_MM + strengthEase(8.0f, 92.0f) + (static_cast<float>(hash8(candleSeed + 151U)) / 255.0f) * strengthEase(4.0f, 54.0f));

    uint8_t pulse = softPulseEnvelope8(now + candle * 1200UL, primaryPeriod, primaryPhase);
    uint8_t secondaryPulse = softPulseEnvelope8(now + candle * 700UL, secondaryPeriod, secondaryPhase);
    uint8_t microFlicker = smoothHash8(static_cast<uint16_t>(candleSeed * 43U), now, static_cast<uint16_t>(260U - strengthByte(0, 155) + hash8(candleSeed + 167U)));
    float smoothedFlicker = (static_cast<float>(pulse) * 0.54f + static_cast<float>(secondaryPulse) * 0.32f + static_cast<float>(microFlicker) * 0.14f) / 255.0f;
    float flickerFloor = 0.86f - flickerAmount * 0.34f;
    float flickerRange = 0.08f + flickerAmount * 0.92f;
    candleLevels[candle] = brightnessScale * (flickerFloor + smoothedFlicker * flickerRange) * strengthEase(116.0f, 176.0f);

    RgbPixel candleColor = sampleNamedOrActivePalette("candle", static_cast<uint16_t>(slot * maxPosition), maxPosition);
    candleColors[candle] = blendRgb(candleColor, flame, static_cast<uint8_t>(44U + hash8(candleSeed + 181U) % 92U));
  }

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float level = strengthLinear(9.0f, 13.0f) + static_cast<float>(smoothHash8(i * 9U, now, 9000UL)) * strengthEase(0.010f, 0.060f);
    RgbPixel color = shadow;
    for (uint8_t candle = 0; candle < candleCount; candle++) {
      float d = fabsf(xMm - centers[candle]);
      float glowFalloff = clamp01(1.0f - d / widths[candle]);
      glowFalloff = glowFalloff * glowFalloff * (3.0f - 2.0f * glowFalloff);
      float coreFalloff = clamp01(1.0f - d / coreWidths[candle]);
      coreFalloff = coreFalloff * coreFalloff;
      float halo = glowFalloff * strengthEase(0.42f, 0.82f);
      float falloff = clamp01(halo + coreFalloff * strengthEase(0.48f, 0.68f));
      level += falloff * candleLevels[candle];
      if (falloff > 0.08f) {
        color = blendRgb(color, candleColors[candle], clamp8(static_cast<int>(falloff * (136.0f + flickerAmount * 96.0f) * contrast)));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderSlowPrism(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float speed = modeSpeedMultiplier(MODE_SLOW_PRISM);
  float contrast = modeContrastMultiplier(MODE_SLOW_PRISM);
  float spread = modeColorSpreadMultiplier(MODE_SLOW_PRISM);
  uint32_t periodMs = static_cast<uint32_t>(210000.0f / max(0.18f, speed));
  uint16_t offset = static_cast<uint16_t>((now % periodMs) * maxPosition / periodMs);
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
      RgbPixel companion = blendRgb(companionA, companionB, static_cast<uint8_t>(x * (96.0f + spread * 159.0f)));
      color = blendRgb(anchor, companion, clampByte(36.0f + spread * 112.0f));
    }
    float prism = (sinf(x * TWO_PI * (0.8f + spread * 1.1f) + now * 0.000024f * speed) + 1.0f) * 0.5f;
    color = blendRgb(anchor, color, clampByte(84.0f + spread * 112.0f));
    uint8_t level = clamp8(static_cast<int>(strengthLinear(108.0f, 76.0f) + prism * strengthEase(34.0f, 96.0f) * contrast));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderGardenShade(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float speed = modeSpeedMultiplier(MODE_GARDEN_SHADE);
  float motion = modeMotionMultiplier(MODE_GARDEN_SHADE);
  float contrast = modeContrastMultiplier(MODE_GARDEN_SHADE);
  RgbPixel sun = warmWhiteApprox(3900);
  RgbPixel leaf = {96, 126, 62};
  RgbPixel gold = {224, 188, 92};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = static_cast<float>(i) / static_cast<float>(maxPosition);
    float breeze = (sinf(x * TWO_PI * (0.8f + motion * 0.9f) + now * 0.000045f * speed) + 1.0f) * 0.5f;
    float shade = (sinf(x * TWO_PI * (2.0f + motion * 2.4f) - now * 0.000032f * speed) + 1.0f) * 0.5f;
    RgbPixel natural = blendRgb(leaf, gold, clamp8(static_cast<int>(breeze * strengthEase(48.0f, 132.0f))));
    RgbPixel palette = sampleNamedOrActivePalette("garden", i, maxPosition);
    natural = blendRgb(natural, palette, clampByte(24.0f + strengthEase(20.0f, 78.0f)));
    RgbPixel color = blendRgb(sun, natural, clamp8(static_cast<int>(24.0f + shade * strengthEase(42.0f, 104.0f))));
    uint8_t level = clamp8(static_cast<int>(136.0f + breeze * strengthEase(18.0f, 58.0f) - shade * strengthEase(5.0f, 25.0f) * contrast));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderSnowfallHush(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float speed = modeSpeedMultiplier(MODE_SNOWFALL_HUSH);
  float motion = modeMotionMultiplier(MODE_SNOWFALL_HUSH);
  float contrast = modeContrastMultiplier(MODE_SNOWFALL_HUSH);
  float density = modeDensityMultiplier(MODE_SNOWFALL_HUSH);
  uint8_t particleCount = static_cast<uint8_t>(constrain(static_cast<int>(3.0f + (static_cast<float>(STRIP_LENGTH_MM) / 420.0f) * density), 4, 14));
  RgbPixel base = sampleNamedOrActivePalette("moonlight", 0, maxPosition);
  RgbPixel snow = coolWhiteApprox(6200);
  if (settings.paletteEnabled && currentModeSupportsPalette()) {
    snow = blendRgb(snow, sampleActivePaletteOrColor(maxPosition / 2, maxPosition), 58);
  }
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float level = strengthLinear(17.0f, 22.0f) + static_cast<float>(hash8(i * 23U) % 8) * strengthEase(0.25f, 0.90f);
    RgbPixel color = blendRgb(base, snow, 28);
    float neighbor = 0.0f;
    for (uint8_t particle = 0; particle < particleCount; particle++) {
      float seed = static_cast<float>(hash8(particle * 41U)) / 255.0f;
      float drift = fmodf(seed + now * (0.00000045f + particle * 0.00000008f) * (0.32f + motion), 1.0f);
      float centerMm = drift * static_cast<float>(STRIP_LENGTH_MM);
      float d = cyclicPhysicalDistanceMm(xMm, centerMm);
      float particleRadius = SNOW_HALO_RADIUS_MM + strengthEase(0.0f, 22.0f) + static_cast<float>(hash8(particle * 19U) % 3) * strengthEase(3.0f, 13.0f);
      float soft = expf(-(d * d) / (2.0f * particleRadius * particleRadius));
      uint32_t envelopePeriod = static_cast<uint32_t>((41000UL + particle * 1300UL) / max(0.20f, speed));
      uint8_t envelope = softPulseEnvelope8(now + particle * 1700UL, envelopePeriod, hash8(particle * 29U) * 257U);
      float fade = 0.42f + static_cast<float>(envelope) / 255.0f * 0.58f;
      neighbor += soft * fade;
      level += soft * fade * strengthEase(24.0f, 86.0f) * contrast;
      if (soft > 0.04f) {
        color = blendRgb(color, snow, clamp8(static_cast<int>(soft * fade * strengthEase(42.0f, 128.0f))));
      }
    }
    if (neighbor > 0.0f) {
      color = blendRgb(color, coolWhiteApprox(6600), clampByte(neighbor * strengthEase(10.0f, 44.0f)));
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderObservatoryGlow(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float speed = modeSpeedMultiplier(MODE_OBSERVATORY_GLOW);
  float motion = modeMotionMultiplier(MODE_OBSERVATORY_GLOW);
  float contrast = modeContrastMultiplier(MODE_OBSERVATORY_GLOW);
  float density = modeDensityMultiplier(MODE_OBSERVATORY_GLOW);
  RgbPixel night = sampleNamedOrActivePalette("moonlight", 0, maxPosition);
  night = blendRgb(RgbPixel{4, 12, 34}, night, 72);
  RgbPixel moon = coolWhiteApprox(5600);
  float moonCenterMm = phaseFromMillis(now, static_cast<uint32_t>(420000.0f / max(0.12f, speed))) * static_cast<float>(STRIP_LENGTH_MM);
  uint8_t starCount = static_cast<uint8_t>(constrain(static_cast<int>(4.0f + (static_cast<float>(STRIP_LENGTH_MM) / 460.0f) * density), 5, 14));
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float d = cyclicPhysicalDistanceMm(xMm, moonCenterMm);
    float moonRadius = OBSERVATORY_MOON_GLOW_RADIUS_MM + strengthEase(-110.0f, 210.0f);
    float glow = expf(-(d * d) / (2.0f * moonRadius * moonRadius));
    float level = strengthLinear(21.0f, 16.0f) + glow * strengthEase(64.0f, 128.0f);
    RgbPixel color = blendRgb(night, moon, clampByte(glow * strengthEase(86.0f, 176.0f)));
    for (uint8_t star = 0; star < starCount; star++) {
      float centerMm = (static_cast<float>(hash8(star * 53U)) * static_cast<float>(STRIP_LENGTH_MM)) / 255.0f;
      float sd = cyclicPhysicalDistanceMm(xMm, centerMm);
      float starRadius = STAR_HALO_RADIUS_MM + static_cast<float>(hash8(star * 17U) % 5) * strengthEase(1.0f, 6.0f);
      float soft = expf(-(sd * sd) / (2.0f * starRadius * starRadius));
      float nearMoon = clamp01(1.0f - cyclicPhysicalDistanceMm(centerMm, moonCenterMm) / max(120.0f, static_cast<float>(STRIP_LENGTH_MM) * 0.24f));
      float twinkle = 0.65f + (sineApprox8(now + star * 1300UL, static_cast<uint32_t>(52000.0f / max(0.18f, speed + motion)), hash8(star * 23U) * 257U) / 255.0f) * 0.35f;
      level += soft * twinkle * (strengthEase(7.0f, 20.0f) + nearMoon * strengthEase(16.0f, 48.0f)) * contrast;
      if (soft > 0.08f) {
        color = blendRgb(color, moon, clamp8(static_cast<int>(soft * twinkle * (32.0f + nearMoon * strengthEase(36.0f, 84.0f)))));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderRainyWindow(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float speed = modeSpeedMultiplier(MODE_RAINY_WINDOW);
  float contrast = modeContrastMultiplier(MODE_RAINY_WINDOW);
  float density = modeDensityMultiplier(MODE_RAINY_WINDOW);
  RgbPixel pane = sampleNamedOrActivePalette("storm-blue", 0, maxPosition);
  RgbPixel droplet = {160, 206, 226};
  float dropletRadiusMm = getMmPerPixel() * static_cast<float>(mmToPixels(RAINY_WINDOW_DROPLET_RADIUS_MM + strengthEase(0.0f, 13.0f)));
  float trailLengthMm = getMmPerPixel() * static_cast<float>(mmToPixels(RAINY_WINDOW_TRAIL_LENGTH_MM + strengthEase(8.0f, 86.0f)));
  uint8_t dropletCount = static_cast<uint8_t>(constrain(static_cast<int>(3.0f + 8.0f * density), 4, 11));
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float x = normalizedPosition01(i);
    float xMm = physicalXmm(i);
    float paneGlow = (sinf(x * TWO_PI * (0.8f + density * 0.8f) + now * 0.000026f * speed) + 1.0f) * 0.5f;
    float level = 22.0f + paneGlow * strengthEase(6.0f, 20.0f);
    RgbPixel color = blendRgb(RgbPixel{10, 28, 46}, pane, 120);
    for (uint8_t drop = 0; drop < dropletCount; drop++) {
      float seed = static_cast<float>(hash8(drop * 37U)) / 255.0f;
      float life = fmodf(seed + now * (0.0000012f + drop * 0.00000022f) * speed, 1.0f);
      float spawn = static_cast<float>(hash8(drop * 73U + 11U)) / 255.0f;
      float positionMm = fmodf(spawn + life * (0.34f + 0.78f * speed), 1.0f) * static_cast<float>(STRIP_LENGTH_MM);
      float fade = sinf(life * PI);
      float linger = smoothstep01(1.0f - life) * strengthEase(0.18f, 0.62f);
      float beadDistanceMm = fabsf(xMm - positionMm);
      float bead = expf(-(beadDistanceMm * beadDistanceMm) / (2.0f * dropletRadiusMm * dropletRadiusMm));
      float trail = clamp01((positionMm - xMm) / trailLengthMm) * expf(-fabsf(positionMm - xMm) / trailLengthMm) * (fade + linger);
      bead *= (0.42f + fade * 0.58f);
      level += bead * strengthEase(34.0f, 78.0f) + trail * strengthEase(18.0f, 52.0f) * contrast;
      if (bead > 0.05f || trail > 0.04f) {
        color = blendRgb(color, droplet, clamp8(static_cast<int>((bead + trail) * strengthEase(50.0f, 112.0f))));
      }
    }
    setFramePixelRgb(i, scaleRgb(color, clamp8(static_cast<int>(level))));
  }
}

void renderCampfireBlanket(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float speed = modeSpeedMultiplier(MODE_CAMPFIRE_BLANKET);
  float contrast = modeContrastMultiplier(MODE_CAMPFIRE_BLANKET);
  RgbPixel ember = {178, 58, 18};
  RgbPixel blanket = warmWhiteApprox(2450);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float waveA = (sinf(xMm / 3100.0f * TWO_PI - now * 0.000086f * speed) + 1.0f) * 0.5f;
    float waveB = (sinf(xMm / strengthLinear(1800.0f, 950.0f) * TWO_PI + now * 0.000053f * speed) + 1.0f) * 0.5f;
    RgbPixel palette = sampleNamedOrActivePalette("candle", i, maxPosition);
    RgbPixel color = blendRgb(ember, blanket, clamp8(static_cast<int>(74.0f + waveA * strengthEase(42.0f, 92.0f))));
    color = blendRgb(color, palette, clampByte(28.0f + strengthEase(18.0f, 76.0f)));
    uint8_t level = clamp8(static_cast<int>(strengthLinear(58.0f, 42.0f) + waveA * strengthEase(36.0f, 82.0f) + waveB * strengthEase(12.0f, 42.0f) * contrast));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderNorthernWhisper(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float speed = modeSpeedMultiplier(MODE_NORTHERN_WHISPER);
  float motion = modeMotionMultiplier(MODE_NORTHERN_WHISPER);
  float colorSpread = modeColorSpreadMultiplier(MODE_NORTHERN_WHISPER);
  RgbPixel night = {5, 12, 32};
  RgbPixel green = {64, 210, 145};
  RgbPixel blue = {78, 128, 224};
  RgbPixel violet = {134, 104, 226};
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float veilA = (sinf(xMm / (5400.0f - colorSpread * 1200.0f) * TWO_PI + now * 0.000034f * speed) + 1.0f) * 0.5f;
    float veilB = (sinf(xMm / AURORA_BASE_WAVELENGTH_MM * TWO_PI - now * 0.000024f * (speed + motion * 0.4f)) + 1.0f) * 0.5f;
    RgbPixel ribbon = blendRgb(green, blue, clampByte(veilA * strengthEase(52.0f, 148.0f)));
    ribbon = blendRgb(ribbon, violet, clampByte(veilB * strengthEase(24.0f, 94.0f)));
    if (settings.paletteEnabled && currentModeSupportsPalette()) {
      ribbon = blendRgb(ribbon, sampleActivePaletteOrColor(static_cast<uint16_t>((i + now / 520UL) % LED_COUNT), maxPosition), clampByte(34.0f + colorSpread * 88.0f));
    }
    float fold = veilA * veilA * strengthEase(0.18f, 0.62f) + veilB * strengthEase(0.07f, 0.22f);
    RgbPixel color = blendRgb(night, ribbon, clamp8(static_cast<int>(fold * strengthEase(96.0f, 196.0f))));
    uint8_t level = clamp8(static_cast<int>(strengthLinear(24.0f, 18.0f) + fold * strengthEase(46.0f, 106.0f)));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void renderStormOutside(uint32_t now) {
  uint16_t maxPosition = LED_COUNT > 1 ? LED_COUNT - 1 : 1;
  float speed = modeSpeedMultiplier(MODE_STORM_OUTSIDE);
  float motion = modeMotionMultiplier(MODE_STORM_OUTSIDE);
  float contrast = modeContrastMultiplier(MODE_STORM_OUTSIDE);
  bool nightGuard = settings.nightGuardEnabled;
  RgbPixel base = sampleNamedOrActivePalette("storm-blue", 0, maxPosition);
  RgbPixel cloud = {72, 96, 124};
  RgbPixel lightning = coolWhiteApprox(7200);
  float cloudRollSpeed = 0.000018f + motion * 0.000041f;
  uint8_t flashRaw = sineApprox8(now, static_cast<uint32_t>(53000.0f / max(0.35f, speed)), 11000U);
  float flashSoftness = strengthEase(0.28f, 0.74f);
  float flashGate = flashRaw > 210 ? static_cast<float>(flashRaw - 210) / 45.0f : 0.0f;
  float postFlashFade = smoothstep01(flashGate) * flashSoftness;
  uint8_t flash = clampByte(postFlashFade * strengthEase(30.0f, 138.0f));
  if (nightGuard) {
    flash = flash / 4;
  }
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    float xMm = physicalXmm(i);
    float roll = (sinf(xMm / 6100.0f * TWO_PI - now * cloudRollSpeed) + 1.0f) * 0.5f;
    float rain = (sinf(xMm / strengthLinear(920.0f, 640.0f) * TWO_PI + now * 0.00012f * speed) + 1.0f) * 0.5f;
    RgbPixel color = blendRgb(RgbPixel{4, 14, 28}, base, 118);
    color = blendRgb(color, cloud, clampByte(roll * strengthEase(32.0f, 82.0f)));
    color = blendRgb(color, lightning, flash);
    uint8_t level = clamp8(static_cast<int>(22.0f + roll * strengthEase(24.0f, 58.0f) + rain * strengthEase(5.0f, 22.0f) * contrast + flash / 3));
    setFramePixelRgb(i, scaleRgb(color, level));
  }
}

void initPulse(uint8_t index, uint32_t now) {
  uint16_t centerLimit = LED_COUNT > 0 ? LED_COUNT : 1;
  uint16_t minWidth = mmToPixels(SLOW_PULSE_MIN_WIDTH_MM);
  uint16_t widthLimit = mmToPixels(SLOW_PULSE_MAX_WIDTH_MM);
  if (widthLimit <= minWidth) {
    widthLimit = minWidth + 1;
  }

  pulses[index].center = random(0, centerLimit);
  pulses[index].width = random(minWidth, widthLimit);

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
  uint16_t minWidth = mmToPixels(SLOW_WAVE_MIN_WIDTH_MM);
  uint16_t widthLimit = mmToPixels(SLOW_WAVE_MAX_WIDTH_MM);
  if (widthLimit <= minWidth) {
    widthLimit = minWidth + 1;
  }
  slowWaves[index].width = random(minWidth, widthLimit);
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
