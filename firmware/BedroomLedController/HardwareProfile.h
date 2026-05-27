#ifndef BEDROOM_LED_CONTROLLER_HARDWARE_PROFILE_H
#define BEDROOM_LED_CONTROLLER_HARDWARE_PROFILE_H

struct LedHardwareProfile {
  const char* profileName;
  const char* boardProfile;
  const char* stripType;
  uint16_t stripLengthMm;
  float ledDensityPerMeter;
  uint16_t ledCountOverride;
  uint8_t dataPin;
  uint16_t targetFrameIntervalMs;
  uint16_t supplyMillivolts;
  uint16_t supplyMilliamps;
  uint16_t controllerOverheadMilliamps;
  uint8_t safetyMarginPercent;
  float milliampsPerPixelFullWhite;
  const char* estimateSource;
};

struct FilesystemInfoSnapshot {
  uint32_t totalBytes;
  uint32_t usedBytes;
};

constexpr uint16_t STRIP_LENGTH_MM = 5000;
constexpr float LED_DENSITY_PER_METER = 160.0f;
constexpr uint16_t LED_COUNT_OVERRIDE = 800;
constexpr float LEGACY_BASELINE_DENSITY_PER_METER = 60.0f;
constexpr uint8_t LED_DATA_PIN = 5;
constexpr uint16_t TARGET_FRAME_INTERVAL_MS = 33;
constexpr uint8_t DEFAULT_MASTER_BRIGHTNESS = 60;
constexpr uint16_t DEFAULT_SUPPLY_MILLIVOLTS = 5000;
constexpr uint16_t DEFAULT_SUPPLY_MILLIAMPS = 5000;
constexpr uint16_t DEFAULT_CONTROLLER_OVERHEAD_MILLIAMPS = 200;
constexpr uint8_t DEFAULT_SUPPLY_SAFETY_MARGIN_PERCENT = 20;
constexpr float POWER_ESTIMATE_MILLIAMPS_PER_PIXEL = 18.75f;
constexpr const char* POWER_ESTIMATE_SOURCE = "manufacturerRecommendation";
constexpr const char* ESP32S3_N16R8_FQBN = "esp32:esp32:esp32s3";
constexpr const char* ESP32S3_N16R8_OPTIONS = "FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB";

constexpr uint16_t calculateLedCountForDensity(uint16_t lengthMm, float densityPerMeter) {
  return densityPerMeter <= 0.0f
    ? 1
    : static_cast<uint16_t>(((static_cast<float>(lengthMm) / 1000.0f) * densityPerMeter) + 0.5f);
}

constexpr uint16_t calculateConfiguredLedCount(
  uint16_t lengthMm,
  float densityPerMeter,
  uint16_t overrideCount
) {
  return overrideCount > 0 ? overrideCount : calculateLedCountForDensity(lengthMm, densityPerMeter);
}

constexpr uint16_t LED_COUNT_AT_60_PER_METER = calculateLedCountForDensity(STRIP_LENGTH_MM, 60.0f);
constexpr uint16_t LED_COUNT_AT_160_PER_METER = calculateLedCountForDensity(STRIP_LENGTH_MM, 160.0f);
constexpr uint16_t LED_COUNT_AT_180_PER_METER = calculateLedCountForDensity(STRIP_LENGTH_MM, 180.0f);

constexpr LedHardwareProfile ACTIVE_HARDWARE_PROFILE = {
  "BTF FCOB 5V 160LED/m 5m",
  "ESP32-S3-N16R8",
  "BTF-LIGHTING FCOB COB XGB1338(WS2812B) RGB GRB",
  STRIP_LENGTH_MM,
  LED_DENSITY_PER_METER,
  LED_COUNT_OVERRIDE,
  LED_DATA_PIN,
  TARGET_FRAME_INTERVAL_MS,
  DEFAULT_SUPPLY_MILLIVOLTS,
  DEFAULT_SUPPLY_MILLIAMPS,
  DEFAULT_CONTROLLER_OVERHEAD_MILLIAMPS,
  DEFAULT_SUPPLY_SAFETY_MARGIN_PERCENT,
  POWER_ESTIMATE_MILLIAMPS_PER_PIXEL,
  POWER_ESTIMATE_SOURCE
};

const char* boardFamilyName() {
#if defined(ESP8266)
  return "ESP8266";
#elif defined(ESP32)
#if defined(CONFIG_IDF_TARGET_ESP32S3)
  return "ESP32-S3";
#else
  return "ESP32";
#endif
#else
  return "unsupported";
#endif
}

const char* boardProfileName() {
#if defined(ESP8266)
  return "D1 mini / ESP8266";
#elif defined(ESP32)
  return ACTIVE_HARDWARE_PROFILE.boardProfile;
#else
  return "Unsupported board";
#endif
}

uint32_t platformRandomSeed() {
#if defined(ESP8266)
  return ESP.getCycleCount();
#elif defined(ESP32)
  return esp_random();
#else
  return 1;
#endif
}

bool filesystemBegin() {
#if defined(ESP8266)
  return LittleFS.begin();
#elif defined(ESP32)
  return LittleFS.begin(false);
#else
  return false;
#endif
}

bool readFilesystemInfo(FilesystemInfoSnapshot& info) {
#if defined(ESP8266)
  FSInfo fsInfo;
  if (!LittleFS.info(fsInfo)) {
    info = {0, 0};
    return false;
  }
  info.totalBytes = fsInfo.totalBytes;
  info.usedBytes = fsInfo.usedBytes;
  return true;
#elif defined(ESP32)
  info.totalBytes = static_cast<uint32_t>(LittleFS.totalBytes());
  info.usedBytes = static_cast<uint32_t>(LittleFS.usedBytes());
  return info.totalBytes > 0;
#else
  info = {0, 0};
  return false;
#endif
}

uint32_t platformFreeHeap() {
  return ESP.getFreeHeap();
}

uint32_t platformMaxFreeBlockSize() {
#if defined(ESP8266)
  return ESP.getMaxFreeBlockSize();
#elif defined(ESP32)
  return ESP.getMaxAllocHeap();
#else
  return 0;
#endif
}

bool platformHasPsram() {
#if defined(ESP32)
  return ESP.getPsramSize() > 0;
#else
  return false;
#endif
}

uint32_t platformPsramSizeBytes() {
#if defined(ESP32)
  return ESP.getPsramSize();
#else
  return 0;
#endif
}

uint8_t platformHeapFragmentationPercent() {
#if defined(ESP8266)
  return ESP.getHeapFragmentation();
#else
  return 0;
#endif
}

uint32_t platformSketchSize() {
  return ESP.getSketchSize();
}

uint32_t platformFreeSketchSpace() {
  return ESP.getFreeSketchSpace();
}

String platformResetReason() {
#if defined(ESP8266)
  return ESP.getResetReason();
#elif defined(ESP32)
  return String(static_cast<uint8_t>(esp_reset_reason()));
#else
  return String("unsupported");
#endif
}

bool isLedPinBootSensitive() {
#if defined(ESP8266)
  return LED_DATA_PIN == 0 || LED_DATA_PIN == 2 || LED_DATA_PIN == 15;
#else
  return false;
#endif
}

String ledPinLabel() {
  String label = "GPIO";
  label += LED_DATA_PIN;
#if defined(ESP8266)
  if (LED_DATA_PIN == 0) {
    label += " / D3";
  } else if (LED_DATA_PIN == 5) {
    label += " / D1";
  }
#endif
  return label;
}

#endif
