#ifndef BEDROOM_LED_CONTROLLER_CONFIG_H
#define BEDROOM_LED_CONTROLLER_CONFIG_H

// ----------------------------
// User settings
// ----------------------------
const char* WIFI_SSID = "Apple Network 85064d";
const char* WIFI_PASS = "12344321";

const char* DEVICE_HOSTNAME = "bedroom-leds";
const char* OTA_USER = "admin";
const char* OTA_PASS = "bedroom-leds";

const char* FALLBACK_AP_SSID = "BedroomLED-Setup";
const char* FALLBACK_AP_PASS = "bedroomleds";

const uint16_t LED_COUNT = 60;         // 6 ft is often about 55-110 LEDs. Adjust to your strip.
const uint8_t LED_PIN = D3;            // D3 on a D1 mini is GPIO0.
const uint8_t MASTER_BRIGHTNESS = 180; // Conservative Phase 1A default. Raise only if the supply can handle it.

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

const uint16_t WARM_DIM_KELVIN = 2400;
const uint8_t WARM_DIM_BRIGHTNESS = 45;
const uint8_t DIAGNOSTIC_TEST_BRIGHTNESS = 64;
const uint16_t DIAGNOSTIC_PIXEL_WALK_INTERVAL_MS = 180;

#endif
