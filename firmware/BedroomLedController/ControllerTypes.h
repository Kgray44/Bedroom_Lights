#ifndef BEDROOM_LED_CONTROLLER_TYPES_H
#define BEDROOM_LED_CONTROLLER_TYPES_H

struct Rgb {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct RgbPixel {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct PaletteColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t weight;
};

struct ColorPalette {
  String id;
  String name;
  uint8_t colorCount;
  PaletteColor colors[MAX_PALETTE_COLORS];
  bool builtin;
  String category;
  bool calm;
  bool sleepSafe;
  bool colorful;
  bool warm;
  bool cool;
};

enum Mode {
  MODE_SOLID,
  MODE_STROBE,
  MODE_FLASH,
  MODE_CHASE,
  MODE_WAVE,
  MODE_SLOW_WAVE,
  MODE_SLOW_PULSE,
  MODE_RAINBOW,
  MODE_DAWN_BLOOM,
  MODE_MOON_TIDE,
  MODE_VELVET_AURORA,
  MODE_LANTERN_DRIFT,
  MODE_RAIN_GLASS,
  MODE_SATIN_BREATHING,
  MODE_PAPER_LANTERN_ROW,
  MODE_LIBRARY_LAMP,
  MODE_SOLAR_NOON,
  MODE_GOLDEN_HOUR,
  MODE_DREAM_AQUARIUM,
  MODE_CIRCUIT_GLOW,
  MODE_WINDOW_SUNRISE,
  MODE_EMBER_QUILT,
  MODE_PORCELAIN_SKY,
  MODE_HEARTHLINE,
  MODE_EVENING_CURRENT,
  MODE_STARLIT_LINEN,
  MODE_DEEP_SEA_GLASS,
  MODE_QUIET_NORTHERN_LIGHTS,
  MODE_CANDLE_HALL,
  MODE_SLOW_PRISM,
  MODE_GARDEN_SHADE,
  MODE_SNOWFALL_HUSH,
  MODE_OBSERVATORY_GLOW,
  MODE_RAINY_WINDOW,
  MODE_CAMPFIRE_BLANKET,
  MODE_NORTHERN_WHISPER,
  MODE_STORM_OUTSIDE
};

struct ControllerSettings {
  Mode mode;
  Rgb color;
  uint16_t whiteKelvin;
  bool usingWhiteTemperature;
  uint8_t masterBrightness;
  float strobeDelaySec;
  float flashOffDelaySec;
  uint8_t chasePeriodSec;
  uint8_t wavePeriodSec;
  uint8_t slowPulseCount;
  uint8_t slowPulseMaxSec;
  uint8_t rainbowPeriodSec;
  String bootBehavior;
  bool gammaEnabled;
  uint8_t redGain;
  uint8_t greenGain;
  uint8_t blueGain;
  uint16_t supplyMillivolts;
  uint16_t supplyMilliamps;
  uint8_t supplySafetyMarginPercent;
  uint16_t controllerOverheadMilliamps;
  bool nightGuardEnabled;
  uint8_t nightGuardMaxBrightness;
  bool nightGuardBlockFlashingModes;
  bool nightGuardForceWarmOnPanic;
  bool nightGuardPreferWarmTemperature;
  uint16_t bedtimeFadeDefaultMinutes;
  String bedtimeFadeDefaultTarget;
  bool bedtimeFadeTurnOffAtEnd;
  bool transitionsEnabled;
  uint16_t transitionDurationMs;
  String transitionStyle;
  bool paletteEnabled;
  String activePaletteId;
  int16_t localUtcOffsetMinutes;
};

struct Pulse {
  float center;
  float width;
  uint32_t bornMs;
  uint32_t durationMs;
  uint8_t seed;
};

struct CrashWave {
  float direction;
  float width;
  uint32_t bornMs;
  uint32_t durationMs;
  uint8_t seed;
};

struct DiagnosticTestState {
  bool active;
  String name;
  uint32_t startedMs;
  uint32_t lastStepMs;
  uint16_t pixelIndex;
};

struct EndpointHeapMetric {
  char route[40];
  uint32_t measuredAtMs;
  uint32_t heapBeforeBytes;
  uint32_t heapAfterBytes;
  uint32_t heapMinimumBytes;
  uint32_t payloadBytes;
};

struct PowerEstimate {
  uint32_t maxLedCurrentMilliamps;
  uint32_t maxLedPowerMilliwatts;
  uint32_t estimatedLedCurrentMilliamps;
  uint32_t estimatedTotalCurrentMilliamps;
  uint32_t usableSupplyCurrentMilliamps;
  uint32_t safeLedCurrentMilliamps;
  uint8_t recommendedBrightnessCap;
  bool aboveRecommendedCap;
};

struct ScenePreset {
  String id;
  String name;
  String category;
  String mode;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint16_t whiteKelvin;
  bool usingWhiteTemperature;
  uint8_t masterBrightness;
  float strobeDelaySec;
  float flashOffDelaySec;
  uint8_t chasePeriodSec;
  uint8_t wavePeriodSec;
  uint8_t slowPulseCount;
  uint8_t slowPulseMaxSec;
  uint8_t rainbowPeriodSec;
  bool builtin;
  bool isFavoriteCandidate;
  bool isSleepSafe;
  bool isFocusFriendly;
  bool paletteEnabled;
  String paletteId;
};

struct FavoriteItem {
  char id[MAX_FAVORITE_ID_LENGTH + 1];
  char label[MAX_FAVORITE_LABEL_LENGTH + 1];
  char type[12];
  char target[MAX_FAVORITE_TARGET_LENGTH + 1];
};

struct TimeSyncState {
  bool timeSynced;
  bool syncRequested;
  uint32_t lastSyncAttemptMs;
  uint32_t lastSyncSuccessMs;
  uint32_t lastSyncFailureMs;
  uint32_t lastSyncSuccessEpoch;
  char timeStatus[64];
};

struct ScheduleEntry {
  String id;
  String name;
  bool enabled;
  uint8_t daysMask;
  uint16_t minuteOfDay;
  String targetType;
  String targetId;
  uint16_t fadeSeconds;
  bool useTransition;
  bool skipNext;
  uint32_t lastRunEpoch;
  uint32_t lastRunOccurrenceKey;
};

enum TimerActionType {
  TIMER_ACTION_NONE,
  TIMER_ACTION_OFF,
  TIMER_ACTION_WARM_DIM,
  TIMER_ACTION_SCENE,
  TIMER_ACTION_PANIC_WARM
};

enum TimerModeType {
  TIMER_MODE_NONE,
  TIMER_MODE_DELAY_THEN_ACTION,
  TIMER_MODE_FADE_TO_ACTION
};

struct TimerLightingState {
  Mode mode;
  Rgb color;
  uint16_t whiteKelvin;
  bool usingWhiteTemperature;
  uint8_t masterBrightness;
  bool paletteEnabled;
  String activePaletteId;
};

struct ActiveTimerState {
  bool active;
  TimerModeType mode;
  TimerActionType action;
  String targetSceneId;
  uint32_t startedMs;
  uint32_t durationMs;
  uint32_t fadeDurationMs;
  bool fadeActive;
  bool completed;
  bool cancelled;
  bool bedtimeFadeActive;
  uint32_t lastFadeStepMs;
  TimerLightingState startState;
  TimerLightingState targetState;
  TimerLightingState restoreState;
};

struct TransitionState {
  bool active;
  String style;
  uint32_t startedMs;
  uint16_t durationMs;
  bool targetStateApplied;
  uint8_t startEffectiveBrightness;
  uint8_t targetEffectiveBrightness;
};

struct SurpriseCandidate {
  String mood;
  Mode mode;
  Rgb color;
  bool usingWhiteTemperature;
  uint16_t whiteKelvin;
  uint8_t brightness;
  bool paletteEnabled;
  String paletteId;
  bool powerCapped;
  bool nightGuardCapped;
};

#endif
