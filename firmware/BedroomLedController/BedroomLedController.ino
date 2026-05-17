/*
  BedroomLedController

  D1 mini + WS2812 bedroom strip controller.

  Board: LOLIN/WEMOS D1 mini or compatible ESP8266
  LED data pin: D3 / GPIO0
  Required library: Adafruit NeoPixel

  Edit Config.h before the first USB upload.
*/

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <math.h>
#include <time.h>
#include <Adafruit_NeoPixel.h>

#ifndef D3
#define D3 0
#endif

#include "Config.h"
#include "ControllerTypes.h"

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

ControllerSettings settings;
RgbPixel frameBuffer[LED_COUNT];
DiagnosticTestState diagnosticTest;
ControllerSettings previousSettingsBeforeTest;
bool hasPreviousSettingsBeforeTest = false;

const uint8_t MAX_PULSES = 15;
Pulse pulses[MAX_PULSES];
CrashWave slowWaves[4];

uint32_t lastFrameMs = 0;
bool ledsDirty = true;

bool strobeOn = true;
uint32_t strobeLastToggleMs = 0;

bool flashOn = true;
uint32_t flashLastToggleMs = 0;

bool settingsDirty = false;
uint32_t settingsDirtySince = 0;
uint32_t lastSettingsSaveMs = 0;
bool settingsLoadedOk = false;
bool settingsSaveOk = false;
bool settingsStorageReady = false;
String settingsLoadStatus = "not loaded";
String settingsSaveStatus = "not saved";
ScenePreset scenes[MAX_SCENES];
uint8_t sceneCount = 0;
String sceneLoadStatus = "not loaded";
String sceneSaveStatus = "not saved";
ColorPalette palettes[MAX_PALETTES];
uint8_t paletteCount = 0;
String paletteLoadStatus = "not loaded";
String paletteSaveStatus = "not saved";
FavoriteItem favorites[MAX_FAVORITES];
uint8_t favoriteCount = 0;
String favoriteLoadStatus = "not loaded";
String favoriteSaveStatus = "not saved";
String lastActionStatus = "none";
ActiveTimerState activeTimer;
String lastTimerStatus = "none";
String lastLoadedSceneId = "";
String lastLoadedSceneName = "";
bool currentStateMatchesLoadedScene = false;
String lastSurpriseSummary = "none";
String lastSurpriseMood = "none";
String lastSurpriseStatus = "none";
ScheduleEntry schedules[MAX_SCHEDULE_ENTRIES];
uint8_t scheduleCount = 0;
String scheduleLoadStatus = "not loaded";
String scheduleSaveStatus = "not saved";
String lastScheduleStatus = "none";
TimeSyncState timeSync;
EndpointHeapMetric endpointHeapMetrics[MAX_ENDPOINT_HEAP_METRICS];
uint8_t endpointHeapMetricCount = 0;
uint32_t minFreeHeapSeen = 0;
uint32_t resourceStatsLastSampleMs = 0;
uint32_t schedulerLastCheckMs = 0;
TransitionState transitionState;
RgbPixel previousFrame[LED_COUNT];
RgbPixel targetFrame[LED_COUNT];
uint8_t lastOutputEffectiveBrightness = MASTER_BRIGHTNESS;
bool transitionsSuppressed = false;
bool transitionTimerCompletionActive = false;

#include "ForwardDeclarations.h"
#include "JsonHelpers.h"
#include "WebUi.h"
#include "ScheduleUi.h"
#include "Diagnostics.h"
#include "ModeMetadata.h"
#include "PaletteControls.h"
#include "TransitionControls.h"
#include "SettingsStorage.h"
#include "ScenePresets.h"
#include "ComfortControls.h"
#include "TimerControls.h"
#include "SurpriseControls.h"
#include "DiagnosticTests.h"
#include "WebRoutes.h"
#include "ScheduleControls.h"
#include "ManagementControls.h"
#include "LedRendering.h"

void setup() {
  Serial.begin(115200);
  delay(50);

  randomSeed(ESP.getCycleCount());

  applyDefaultSettings();

  strip.begin();
  strip.setBrightness(255);
  clearFrame();
  outputFrameToStrip();

  loadSettings();
  loadPalettes();
  loadScenes();
  loadFavorites();
  loadSchedules();
  applyBootBehavior();
  applyMasterBrightness();

  for (uint8_t i = 0; i < MAX_PULSES; i++) {
    initPulse(i, millis());
  }
  for (uint8_t i = 0; i < 4; i++) {
    initSlowWave(i, millis());
  }

  setupWifi();
  setupOta();
  setupRoutes();
  server.begin();
  beginTimeSync(false);
  updateResourceStats();

  ledsDirty = true;
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  updateResourceStats();
  serviceTimeSync();
  serviceScheduler();
  serviceActiveTimer();
  updateLeds();
  serviceSettingsSave();
  yield();
}
