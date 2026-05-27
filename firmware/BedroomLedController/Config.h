#ifndef BEDROOM_LED_CONTROLLER_CONFIG_H
#define BEDROOM_LED_CONTROLLER_CONFIG_H

// ----------------------------
// User settings
// ----------------------------
#if __has_include("Secrets.h")
#include "Secrets.h"
#else
#include "Secrets.example.h"
#endif

#ifndef BEDROOM_WIFI_SSID
#define BEDROOM_WIFI_SSID "YOUR_WIFI_NAME"
#endif
#ifndef BEDROOM_WIFI_PASS
#define BEDROOM_WIFI_PASS "YOUR_WIFI_PASSWORD"
#endif
#ifndef BEDROOM_OTA_USER
#define BEDROOM_OTA_USER "admin"
#endif
#ifndef BEDROOM_OTA_PASS
#define BEDROOM_OTA_PASS "CHANGE_ME_OTA_PASSWORD"
#endif
#ifndef BEDROOM_FALLBACK_AP_PASS
#define BEDROOM_FALLBACK_AP_PASS "bedroomleds"
#endif

const char* WIFI_SSID = BEDROOM_WIFI_SSID;
const char* WIFI_PASS = BEDROOM_WIFI_PASS;

const char* DEVICE_HOSTNAME = "bedroom-leds";
const char* OTA_USER = BEDROOM_OTA_USER;
const char* OTA_PASS = BEDROOM_OTA_PASS;

const char* FALLBACK_AP_SSID = "BedroomLED-Setup";
const char* FALLBACK_AP_PASS = BEDROOM_FALLBACK_AP_PASS;

#include "HardwareProfile.h"

constexpr uint16_t LED_COUNT = calculateConfiguredLedCount(
  STRIP_LENGTH_MM,
  LED_DENSITY_PER_METER,
  LED_COUNT_OVERRIDE
);
constexpr uint8_t LED_PIN = LED_DATA_PIN;
constexpr uint8_t MASTER_BRIGHTNESS = DEFAULT_MASTER_BRIGHTNESS;

const char* SETTINGS_PATH = "/settings.json";
const char* SCENES_PATH = "/scenes.json";
const char* FAVORITES_PATH = "/favorites.json";
const char* PALETTES_PATH = "/palettes.json";
const char* SCHEDULES_PATH = "/schedule.json";
const uint32_t SETTINGS_SAVE_DEBOUNCE_MS = 1000;
const uint8_t MAX_SCENES = 16;
const uint8_t MAX_SCENE_NAME_LENGTH = 32;
const uint8_t MAX_SCENE_ID_LENGTH = 28;
const uint8_t MAX_FAVORITES = 8;
const uint8_t MAX_FAVORITE_LABEL_LENGTH = 24;
const uint8_t MAX_FAVORITE_ID_LENGTH = 28;
const uint8_t MAX_FAVORITE_TARGET_LENGTH = 28;
const uint8_t MAX_PALETTES = 16;
const uint8_t MAX_PALETTE_COLORS = 5;
const uint8_t MIN_PALETTE_COLORS = 2;
const uint8_t MAX_PALETTE_NAME_LENGTH = 32;
const uint8_t MAX_PALETTE_ID_LENGTH = 28;
const uint8_t MAX_SCHEDULE_ENTRIES = 8;
const uint8_t MAX_SCHEDULE_NAME_LENGTH = 32;
const uint8_t MAX_SCHEDULE_ID_LENGTH = 28;
const uint8_t MAX_SCHEDULE_TARGET_LENGTH = 32;
const uint8_t NIGHT_GUARD_DEFAULT_MAX_BRIGHTNESS = 80;
const uint16_t TIMER_MAX_MINUTES = 240;
const uint16_t TIMER_DEFAULT_MINUTES = 30;
const uint32_t TIMER_FADE_STEP_MS = 1000;
const int16_t DEFAULT_LOCAL_UTC_OFFSET_MINUTES = -240;
const uint32_t TIME_SYNC_RETRY_MS = 10UL * 60UL * 1000UL;
const uint32_t TIME_SYNC_REFRESH_MS = 12UL * 60UL * 60UL * 1000UL;
const uint32_t SCHEDULER_CHECK_MS = 1000;
const uint8_t MAX_ENDPOINT_HEAP_METRICS = 10;

const uint16_t WARM_DIM_KELVIN = 2400;
const uint8_t WARM_DIM_BRIGHTNESS = 45;
const uint8_t DIAGNOSTIC_TEST_BRIGHTNESS = 64;
const uint16_t DIAGNOSTIC_PIXEL_WALK_INTERVAL_MS = 180;
const bool ENABLE_MOTION_SMOOTHING = true;
const uint8_t DEFAULT_CALM_SMOOTHING = 5;
const uint8_t DEFAULT_WEATHER_SMOOTHING = 7;
const uint8_t DEFAULT_COLORFUL_SMOOTHING = 8;
const uint8_t DEFAULT_MOTION_SMOOTHING = 10;

#endif
