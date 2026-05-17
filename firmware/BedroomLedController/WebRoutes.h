#ifndef BEDROOM_LED_CONTROLLER_WEB_ROUTES_H
#define BEDROOM_LED_CONTROLLER_WEB_ROUTES_H

bool parseRequestInt(const String& raw, int& value) {
  String cleaned = raw;
  cleaned.trim();
  if (cleaned.length() == 0) {
    return false;
  }
  char* tail = nullptr;
  long parsed = strtol(cleaned.c_str(), &tail, 10);
  if (tail == cleaned.c_str()) {
    return false;
  }
  while (*tail != '\0') {
    if (!isspace(*tail)) {
      return false;
    }
    tail++;
  }
  value = static_cast<int>(parsed);
  return true;
}

bool parseRequestFloat(const String& raw, float& value) {
  String cleaned = raw;
  cleaned.trim();
  if (cleaned.length() == 0) {
    return false;
  }
  char* tail = nullptr;
  double parsed = strtod(cleaned.c_str(), &tail);
  if (tail == cleaned.c_str()) {
    return false;
  }
  while (*tail != '\0') {
    if (!isspace(*tail)) {
      return false;
    }
    tail++;
  }
  value = static_cast<float>(parsed);
  return true;
}

bool parseRequestBool(const String& raw, bool& value) {
  String cleaned = raw;
  cleaned.trim();
  cleaned.toLowerCase();
  if (cleaned == "1" || cleaned == "true" || cleaned == "on" || cleaned == "yes") {
    value = true;
    return true;
  }
  if (cleaned == "0" || cleaned == "false" || cleaned == "off" || cleaned == "no") {
    value = false;
    return true;
  }
  return false;
}

bool parseIntArg(const String& name, int& out) {
  if (!server.hasArg(name)) {
    return false;
  }
  return parseRequestInt(server.arg(name), out);
}

bool isHexDigit(char c) {
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

uint8_t hexNibble(char c) {
  if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
  if (c >= 'a' && c <= 'f') return static_cast<uint8_t>(10 + c - 'a');
  if (c >= 'A' && c <= 'F') return static_cast<uint8_t>(10 + c - 'A');
  return 0;
}

bool parseHexColor(const String& hex, uint8_t& r, uint8_t& g, uint8_t& b) {
  String cleaned = hex;
  cleaned.trim();
  cleaned.replace("#", "");
  if (cleaned.length() != 6) {
    return false;
  }
  for (uint8_t i = 0; i < 6; i++) {
    if (!isHexDigit(cleaned[i])) {
      return false;
    }
  }
  r = static_cast<uint8_t>((hexNibble(cleaned[0]) << 4) | hexNibble(cleaned[1]));
  g = static_cast<uint8_t>((hexNibble(cleaned[2]) << 4) | hexNibble(cleaned[3]));
  b = static_cast<uint8_t>((hexNibble(cleaned[4]) << 4) | hexNibble(cleaned[5]));
  return true;
}

String normalizedModeToken(const String& key) {
  String normalized;
  normalized.reserve(key.length());
  for (uint16_t i = 0; i < key.length(); i++) {
    char c = key[i];
    if (c == ' ' || c == '-' || c == '_') {
      continue;
    }
    normalized += static_cast<char>(tolower(c));
  }
  return normalized;
}

bool tryModeFromString(const String& key, Mode& mode) {
  String token = normalizedModeToken(key);
  if (token == "solid" || token == "solidcolor") {
    mode = MODE_SOLID;
    return true;
  }
  if (token == "strobe") {
    mode = MODE_STROBE;
    return true;
  }
  if (token == "flash") {
    mode = MODE_FLASH;
    return true;
  }
  if (token == "chase") {
    mode = MODE_CHASE;
    return true;
  }
  if (token == "wave") {
    mode = MODE_WAVE;
    return true;
  }
  if (token == "slowwave") {
    mode = MODE_SLOW_WAVE;
    return true;
  }
  if (token == "slowpulse") {
    mode = MODE_SLOW_PULSE;
    return true;
  }
  if (token == "rainbow") {
    mode = MODE_RAINBOW;
    return true;
  }
  if (token == "dawnbloom") {
    mode = MODE_DAWN_BLOOM;
    return true;
  }
  if (token == "moontide") {
    mode = MODE_MOON_TIDE;
    return true;
  }
  if (token == "velvetaurora") {
    mode = MODE_VELVET_AURORA;
    return true;
  }
  if (token == "lanterndrift") {
    mode = MODE_LANTERN_DRIFT;
    return true;
  }
  if (token == "rainglass") {
    mode = MODE_RAIN_GLASS;
    return true;
  }
  if (token == "satinbreathing") {
    mode = MODE_SATIN_BREATHING;
    return true;
  }
  if (token == "paperlanternrow") {
    mode = MODE_PAPER_LANTERN_ROW;
    return true;
  }
  if (token == "librarylamp") {
    mode = MODE_LIBRARY_LAMP;
    return true;
  }
  if (token == "solarnoon") {
    mode = MODE_SOLAR_NOON;
    return true;
  }
  if (token == "goldenhour") {
    mode = MODE_GOLDEN_HOUR;
    return true;
  }
  if (token == "dreamaquarium") {
    mode = MODE_DREAM_AQUARIUM;
    return true;
  }
  if (token == "circuitglow") {
    mode = MODE_CIRCUIT_GLOW;
    return true;
  }
  if (token == "windowsunrise") {
    mode = MODE_WINDOW_SUNRISE;
    return true;
  }
  if (token == "emberquilt") {
    mode = MODE_EMBER_QUILT;
    return true;
  }
  if (token == "porcelainsky") {
    mode = MODE_PORCELAIN_SKY;
    return true;
  }
  if (token == "hearthline") {
    mode = MODE_HEARTHLINE;
    return true;
  }
  if (token == "eveningcurrent") {
    mode = MODE_EVENING_CURRENT;
    return true;
  }
  if (token == "starlitlinen") {
    mode = MODE_STARLIT_LINEN;
    return true;
  }
  if (token == "deepseaglass") {
    mode = MODE_DEEP_SEA_GLASS;
    return true;
  }
  if (token == "quietnorthernlights") {
    mode = MODE_QUIET_NORTHERN_LIGHTS;
    return true;
  }
  if (token == "candlehall") {
    mode = MODE_CANDLE_HALL;
    return true;
  }
  if (token == "slowprism") {
    mode = MODE_SLOW_PRISM;
    return true;
  }
  if (token == "gardenshade") {
    mode = MODE_GARDEN_SHADE;
    return true;
  }
  if (token == "snowfallhush") {
    mode = MODE_SNOWFALL_HUSH;
    return true;
  }
  if (token == "observatoryglow") {
    mode = MODE_OBSERVATORY_GLOW;
    return true;
  }
  if (token == "rainywindow") {
    mode = MODE_RAINY_WINDOW;
    return true;
  }
  if (token == "campfireblanket") {
    mode = MODE_CAMPFIRE_BLANKET;
    return true;
  }
  if (token == "northernwhisper") {
    mode = MODE_NORTHERN_WHISPER;
    return true;
  }
  if (token == "stormoutside") {
    mode = MODE_STORM_OUTSIDE;
    return true;
  }
  return false;
}

Mode modeFromString(const String& key) {
  Mode mode;
  if (tryModeFromString(key, mode)) {
    return mode;
  }
  return MODE_SOLID;
}

String modeKey(Mode mode) {
  switch (mode) {
    case MODE_STROBE: return "strobe";
    case MODE_FLASH: return "flash";
    case MODE_CHASE: return "chase";
    case MODE_WAVE: return "wave";
    case MODE_SLOW_WAVE: return "slowWave";
    case MODE_SLOW_PULSE: return "slowPulse";
    case MODE_RAINBOW: return "rainbow";
    case MODE_DAWN_BLOOM: return "dawnBloom";
    case MODE_MOON_TIDE: return "moonTide";
    case MODE_VELVET_AURORA: return "velvetAurora";
    case MODE_LANTERN_DRIFT: return "lanternDrift";
    case MODE_RAIN_GLASS: return "rainGlass";
    case MODE_SATIN_BREATHING: return "satinBreathing";
    case MODE_PAPER_LANTERN_ROW: return "paperLanternRow";
    case MODE_LIBRARY_LAMP: return "libraryLamp";
    case MODE_SOLAR_NOON: return "solarNoon";
    case MODE_GOLDEN_HOUR: return "goldenHour";
    case MODE_DREAM_AQUARIUM: return "dreamAquarium";
    case MODE_CIRCUIT_GLOW: return "circuitGlow";
    case MODE_WINDOW_SUNRISE: return "windowSunrise";
    case MODE_EMBER_QUILT: return "emberQuilt";
    case MODE_PORCELAIN_SKY: return "porcelainSky";
    case MODE_HEARTHLINE: return "hearthline";
    case MODE_EVENING_CURRENT: return "eveningCurrent";
    case MODE_STARLIT_LINEN: return "starlitLinen";
    case MODE_DEEP_SEA_GLASS: return "deepSeaGlass";
    case MODE_QUIET_NORTHERN_LIGHTS: return "quietNorthernLights";
    case MODE_CANDLE_HALL: return "candleHall";
    case MODE_SLOW_PRISM: return "slowPrism";
    case MODE_GARDEN_SHADE: return "gardenShade";
    case MODE_SNOWFALL_HUSH: return "snowfallHush";
    case MODE_OBSERVATORY_GLOW: return "observatoryGlow";
    case MODE_RAINY_WINDOW: return "rainyWindow";
    case MODE_CAMPFIRE_BLANKET: return "campfireBlanket";
    case MODE_NORTHERN_WHISPER: return "northernWhisper";
    case MODE_STORM_OUTSIDE: return "stormOutside";
    case MODE_SOLID:
    default: return "solid";
  }
}

void setMode(Mode mode) {
  if (settings.mode != mode) {
    settings.mode = mode;
    resetAnimationState();
  }
  ledsDirty = true;
}

bool setMasterBrightness(uint8_t value) {
  if (settings.masterBrightness == value) {
    return false;
  }
  settings.masterBrightness = value;
  clearLoadedSceneTracking();
  applyMasterBrightness();
  markSettingsDirty();
  return true;
}

bool setActiveColor(uint8_t r, uint8_t g, uint8_t b) {
  bool changed =
    settings.color.r != r ||
    settings.color.g != g ||
    settings.color.b != b ||
    settings.usingWhiteTemperature ||
    settings.whiteKelvin != 0;

  if (!changed) {
    return false;
  }

  settings.color = {r, g, b};
  settings.whiteKelvin = 0;
  settings.usingWhiteTemperature = false;
  clearLoadedSceneTracking();
  ledsDirty = true;
  markSettingsDirty();
  return true;
}

bool setWhiteTemperature(uint16_t kelvin) {
  uint16_t clampedKelvin = constrain(kelvin, 2700, 6500);
  Rgb nextColor = kelvinToRgb(clampedKelvin);
  bool changed =
    settings.whiteKelvin != clampedKelvin ||
    !settings.usingWhiteTemperature ||
    settings.color.r != nextColor.r ||
    settings.color.g != nextColor.g ||
    settings.color.b != nextColor.b;

  if (!changed) {
    return false;
  }

  settings.whiteKelvin = clampedKelvin;
  settings.color = nextColor;
  settings.usingWhiteTemperature = true;
  clearLoadedSceneTracking();
  ledsDirty = true;
  markSettingsDirty();
  return true;
}

bool setModeByName(const String& modeName, String& error) {
  Mode nextMode;
  if (!tryModeFromString(modeName, nextMode)) {
    error = "Unknown mode";
    return false;
  }
  if (isModeBlockedByNightGuard(modeName)) {
    error = nightGuardBlockMessage(modeName);
    return false;
  }
  if (settings.mode == nextMode) {
    return true;
  }
  clearLoadedSceneTracking();
  setMode(nextMode);
  markSettingsDirty();
  return true;
}

bool setModeByName(const String& modeName) {
  String error;
  return setModeByName(modeName, error);
}

bool setGammaEnabled(bool enabled) {
  if (settings.gammaEnabled == enabled) {
    return false;
  }
  settings.gammaEnabled = enabled;
  ledsDirty = true;
  markSettingsDirty();
  return true;
}

bool setRgbGains(uint8_t rGain, uint8_t gGain, uint8_t bGain) {
  if (settings.redGain == rGain && settings.greenGain == gGain && settings.blueGain == bGain) {
    return false;
  }
  settings.redGain = rGain;
  settings.greenGain = gGain;
  settings.blueGain = bGain;
  ledsDirty = true;
  markSettingsDirty();
  return true;
}

bool setPowerConfig(
  uint16_t supplyMillivolts,
  uint16_t supplyMilliamps,
  uint8_t supplySafetyMarginPercent,
  uint16_t controllerOverheadMilliamps
) {
  uint16_t nextMillivolts = constrain(supplyMillivolts, 3000, 24000);
  uint16_t nextMilliamps = constrain(supplyMilliamps, 100, 30000);
  uint8_t nextMargin = constrain(supplySafetyMarginPercent, 0, 90);
  uint16_t nextOverhead = constrain(controllerOverheadMilliamps, 0, 2000);

  if (settings.supplyMillivolts == nextMillivolts &&
      settings.supplyMilliamps == nextMilliamps &&
      settings.supplySafetyMarginPercent == nextMargin &&
      settings.controllerOverheadMilliamps == nextOverhead) {
    return false;
  }

  settings.supplyMillivolts = nextMillivolts;
  settings.supplyMilliamps = nextMilliamps;
  settings.supplySafetyMarginPercent = nextMargin;
  settings.controllerOverheadMilliamps = nextOverhead;
  markSettingsDirty();
  return true;
}

bool setBootBehaviorValue(const String& bootBehavior) {
  String nextBootBehavior = normalizeBootBehavior(bootBehavior);
  if (settings.bootBehavior == nextBootBehavior) {
    return false;
  }
  settings.bootBehavior = nextBootBehavior;
  markSettingsDirty();
  return true;
}

bool setFloatSetting(float& target, float value, float minValue, float maxValue) {
  float clampedValue = constrain(value, minValue, maxValue);
  if (fabsf(target - clampedValue) < 0.001f) {
    return false;
  }
  target = clampedValue;
  markSettingsDirty();
  return true;
}

bool setByteSetting(uint8_t& target, int value, int minValue, int maxValue, bool resetAnimation) {
  uint8_t clampedValue = static_cast<uint8_t>(constrain(value, minValue, maxValue));
  if (target == clampedValue) {
    return false;
  }
  target = clampedValue;
  if (resetAnimation) {
    resetAnimationState();
  }
  markSettingsDirty();
  return true;
}

String buildStateJson() {
  String ip = WiFi.status() == WL_CONNECTED
    ? WiFi.localIP().toString()
    : WiFi.softAPIP().toString();
  uint16_t shownKelvin = settings.whiteKelvin == 0 ? 3000 : settings.whiteKelvin;
  const ModeMetadata* metadata = findModeMetadata(settings.mode);
  String modeDisplayName = metadata != nullptr ? String(metadata->displayName) : modeKey(settings.mode);
  String modeCategory = metadata != nullptr ? String(metadata->category) : String("Unknown");

  String json;
  json.reserve(2200);
  json += R"json({"ok":true)json";
  json += R"json(,"hostname":")json";
  json += escapeJson(DEVICE_HOSTNAME);
  json += R"json(","ip":")json";
  json += escapeJson(ip);
  json += R"json(","mode":")json";
  json += modeKey(settings.mode);
  json += R"json(","modeDisplayName":")json";
  json += escapeJson(modeDisplayName);
  json += R"json(","modeCategory":")json";
  json += escapeJson(modeCategory);
  json += R"json(","modeTags":)json";
  json += currentModeTagsJson();
  json += R"json(,"currentModeSupportsPalette":)json";
  json += boolJson(currentModeSupportsPalette());
  json += R"json(,"currentModePaletteRole":")json";
  json += escapeJson(currentModePaletteRole());
  json += R"json(","paletteEnabled":)json";
  json += boolJson(settings.paletteEnabled);
  json += R"json(,"activePaletteId":")json";
  json += escapeJson(settings.activePaletteId);
  json += R"json(","activePaletteName":")json";
  json += escapeJson(activePaletteName());
  json += R"json(")json";
  json += R"json(","hex":")json";
  json += colorHexString();
  json += R"json(","kelvin":)json";
  json += shownKelvin;
  json += R"json(,"usingWhiteTemperature":)json";
  json += boolJson(settings.usingWhiteTemperature);
  json += R"json(,"masterBrightness":)json";
  json += settings.masterBrightness;
  json += R"json(,"masterBrightnessPercent":)json";
  json += masterBrightnessPercent();
  json += R"json(,"effectiveBrightness":)json";
  json += getEffectiveBrightness();
  json += R"json(,"nightGuardEffectiveBrightness":)json";
  json += getEffectiveBrightness();
  json += R"json(,"nightGuardEnabled":)json";
  json += boolJson(settings.nightGuardEnabled);
  json += R"json(,"nightGuardMaxBrightness":)json";
  json += settings.nightGuardMaxBrightness;
  json += R"json(,"nightGuardBlockFlashingModes":)json";
  json += boolJson(settings.nightGuardBlockFlashingModes);
  json += R"json(,"nightGuardForceWarmOnPanic":)json";
  json += boolJson(settings.nightGuardForceWarmOnPanic);
  json += R"json(,"nightGuardPreferWarmTemperature":)json";
  json += boolJson(settings.nightGuardPreferWarmTemperature);
  json += R"json(,"nightGuardBrightnessCapped":)json";
  json += boolJson(isNightGuardBrightnessCapped());
  json += R"json(,"favoritesCount":)json";
  json += favoriteCount;
  json += R"json(,"maxFavorites":)json";
  json += MAX_FAVORITES;
  json += R"json(,"lastActionStatus":")json";
  json += escapeJson(lastActionStatus);
  json += R"json(")json";
  json += R"json(,"lastSurpriseSummary":")json";
  json += escapeJson(lastSurpriseSummary);
  json += R"json(","lastSurpriseMood":")json";
  json += escapeJson(lastSurpriseMood);
  json += R"json(","lastSurpriseStatus":")json";
  json += escapeJson(lastSurpriseStatus);
  json += R"json(")json";
  json += R"json(,"lastLoadedSceneId":")json";
  json += escapeJson(lastLoadedSceneId);
  json += R"json(","lastLoadedSceneName":")json";
  json += escapeJson(lastLoadedSceneName);
  json += R"json(","currentStateMatchesLoadedScene":)json";
  json += boolJson(currentStateMatchesLoadedScene);
  json += R"json(,"transitionsEnabled":)json";
  json += boolJson(settings.transitionsEnabled);
  json += R"json(,"transitionStyle":")json";
  json += escapeJson(normalizeTransitionStyle(settings.transitionStyle));
  json += R"json(","transitionDurationMs":)json";
  json += settings.transitionDurationMs;
  json += R"json(,"transitionActive":)json";
  json += boolJson(transitionState.active);
  json += R"json(,"transitionProgressPercent":)json";
  json += getTransitionProgressPercent();
  json += R"json(,"timerActive":)json";
  json += boolJson(activeTimer.active);
  json += R"json(,"timerMode":")json";
  json += timerModeKey(activeTimer.mode);
  json += R"json(","timerAction":")json";
  json += timerActionKey(activeTimer.action);
  json += R"json(","timerTargetSceneId":")json";
  json += escapeJson(activeTimer.targetSceneId);
  json += R"json(","timerDurationMs":)json";
  json += activeTimer.durationMs;
  json += R"json(,"timerElapsedMs":)json";
  json += getTimerElapsedMs();
  json += R"json(,"timerRemainingMs":)json";
  json += getTimerRemainingMs();
  json += R"json(,"timerProgressPercent":)json";
  json += getTimerProgressPercent();
  json += R"json(,"timerFadeActive":)json";
  json += boolJson(activeTimer.fadeActive);
  json += R"json(,"bedtimeFadeActive":)json";
  json += boolJson(activeTimer.bedtimeFadeActive);
  json += R"json(,"lastTimerStatus":")json";
  json += escapeJson(lastTimerStatus);
  json += R"json(","timeSynced":)json";
  json += boolJson(isTimeSynced());
  json += R"json(,"currentLocalTime":")json";
  json += escapeJson(currentLocalTimeString());
  json += R"json(","localUtcOffsetMinutes":)json";
  json += settings.localUtcOffsetMinutes;
  json += R"json(,"scheduleCount":)json";
  json += scheduleCount;
  json += R"json(,"nextScheduleSummary":")json";
  json += escapeJson(nextScheduleSummary());
  json += R"json(","lastScheduleStatus":")json";
  json += escapeJson(lastScheduleStatus);
  json += R"json(","bedtimeFadeDefaultMinutes":)json";
  json += settings.bedtimeFadeDefaultMinutes;
  json += R"json(,"bedtimeFadeDefaultTarget":")json";
  json += escapeJson(settings.bedtimeFadeDefaultTarget);
  json += R"json(","bedtimeFadeTurnOffAtEnd":)json";
  json += boolJson(settings.bedtimeFadeTurnOffAtEnd);
  json += R"json(,"gammaEnabled":)json";
  json += boolJson(settings.gammaEnabled);
  json += R"json(,"redGain":)json";
  json += settings.redGain;
  json += R"json(,"greenGain":)json";
  json += settings.greenGain;
  json += R"json(,"blueGain":)json";
  json += settings.blueGain;
  json += R"json(,"supplyMillivolts":)json";
  json += settings.supplyMillivolts;
  json += R"json(,"supplyMilliamps":)json";
  json += settings.supplyMilliamps;
  json += R"json(,"supplySafetyMarginPercent":)json";
  json += settings.supplySafetyMarginPercent;
  json += R"json(,"controllerOverheadMilliamps":)json";
  json += settings.controllerOverheadMilliamps;
  json += R"json(,"bootBehavior":")json";
  json += escapeJson(normalizeBootBehavior(settings.bootBehavior));
  json += R"json(","strobeDelay":)json";
  json += String(settings.strobeDelaySec, 1);
  json += R"json(,"flashOffDelay":)json";
  json += String(settings.flashOffDelaySec, 1);
  json += R"json(,"chasePeriod":)json";
  json += settings.chasePeriodSec;
  json += R"json(,"wavePeriod":)json";
  json += settings.wavePeriodSec;
  json += R"json(,"slowPulseCount":)json";
  json += settings.slowPulseCount;
  json += R"json(,"slowPulseMax":)json";
  json += settings.slowPulseMaxSec;
  json += R"json(,"rainbowPeriod":)json";
  json += settings.rainbowPeriodSec;
  json += R"json(,"settingsLoadStatus":")json";
  json += escapeJson(settingsLoadStatus);
  json += R"json(","settingsSaveStatus":")json";
  json += escapeJson(settingsSaveStatus);
  json += R"json(","testActive":)json";
  json += boolJson(diagnosticTest.active);
  json += R"json(,"testName":")json";
  json += escapeJson(diagnosticTest.name);
  json += R"json(","diagnosticTestBrightness":)json";
  json += DIAGNOSTIC_TEST_BRIGHTNESS;
  json += F("}");
  return json;
}

void sendJsonOk(const String& message, const String& stateJson) {
  String json;
  json.reserve(stateJson.length() + message.length() + 40);
  json += R"json({"ok":true,"message":")json";
  json += escapeJson(message);
  json += R"json(","state":)json";
  json += stateJson;
  json += F("}");
  server.send(200, "application/json", json);
}

void sendJsonError(int statusCode, const String& error) {
  String json;
  json.reserve(error.length() + 28);
  json += R"json({"ok":false,"error":")json";
  json += escapeJson(error);
  json += F("\"}");
  server.send(statusCode, "application/json", json);
}

void setupWifi() {
  WiFi.hostname(DEVICE_HOSTNAME);

  const bool hasCredentials =
    String(WIFI_SSID) != "YOUR_WIFI_NAME" &&
    String(WIFI_PASS) != "YOUR_WIFI_PASSWORD" &&
    strlen(WIFI_SSID) > 0;

  if (hasCredentials) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    const uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
      delay(250);
      yield();
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(FALLBACK_AP_SSID, FALLBACK_AP_PASS);
  }
}

void setupOta() {
  ArduinoOTA.setHostname(DEVICE_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASS);
  ArduinoOTA.begin();
}

void setupRoutes() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/ota", HTTP_GET, handleOtaPage);
  server.on("/diagnostics", HTTP_GET, handleDiagnosticsPage);
  server.on("/schedule", HTTP_GET, handleSchedulePage);
  server.on("/api-docs", HTTP_GET, handleApiDocsPage);
  server.on("/api/state", HTTP_GET, handleApiState);
  server.on("/api/modes", HTTP_GET, handleApiModes);
  server.on("/api/transitions", HTTP_GET, handleApiTransitions);
  server.on("/api/transitions/set", HTTP_GET, handleApiTransitionsSet);
  server.on("/api/palettes", HTTP_GET, handleApiPalettes);
  server.on("/api/palettes/select", HTTP_GET, handleApiPalettesSelect);
  server.on("/api/palettes/save", HTTP_GET, handleApiPalettesSave);
  server.on("/api/palettes/update", HTTP_GET, handleApiPalettesUpdate);
  server.on("/api/palettes/delete", HTTP_GET, handleApiPalettesDelete);
  server.on("/api/palettes/reset", HTTP_GET, handleApiPalettesReset);
  server.on("/api/palettes/export", HTTP_GET, handleApiPalettesExport);
  server.on("/api/palettes/import", HTTP_GET, handleApiPalettesImport);
  server.on("/api/palettes/reset-builtins", HTTP_GET, handleApiPalettesResetBuiltins);
  server.on("/api/surprise", HTTP_GET, handleApiSurprise);
  server.on("/api/surprise/options", HTTP_GET, handleApiSurpriseOptions);
  server.on("/api/brightness", HTTP_GET, handleApiBrightness);
  server.on("/api/color", HTTP_GET, handleApiColor);
  server.on("/api/temperature", HTTP_GET, handleApiTemperature);
  server.on("/api/mode", HTTP_GET, handleApiMode);
  server.on("/api/scenes", HTTP_GET, handleApiScenes);
  server.on("/api/scenes/save", HTTP_GET, handleApiScenesSave);
  server.on("/api/scenes/load", HTTP_GET, handleApiScenesLoad);
  server.on("/api/scenes/rename", HTTP_GET, handleApiScenesRename);
  server.on("/api/scenes/delete", HTTP_GET, handleApiScenesDelete);
  server.on("/api/scenes/duplicate", HTTP_GET, handleApiScenesDuplicate);
  server.on("/api/scenes/export", HTTP_GET, handleApiScenesExport);
  server.on("/api/scenes/import", HTTP_GET, handleApiScenesImport);
  server.on("/api/scenes/reset-builtins", HTTP_GET, handleApiScenesResetBuiltins);
  server.on("/api/favorites", HTTP_GET, handleApiFavorites);
  server.on("/api/favorites/load", HTTP_GET, handleApiFavoritesLoad);
  server.on("/api/favorites/reset", HTTP_GET, handleApiFavoritesReset);
  server.on("/api/action", HTTP_GET, handleApiAction);
  server.on("/api/panic-warm", HTTP_GET, handleApiPanicWarm);
  server.on("/api/nightguard", HTTP_GET, handleApiNightGuard);
  server.on("/api/nightguard/set", HTTP_GET, handleApiNightGuardSet);
  server.on("/api/timer", HTTP_GET, handleApiTimer);
  server.on("/api/timer/start", HTTP_GET, handleApiTimerStart);
  server.on("/api/timer/cancel", HTTP_GET, handleApiTimerCancel);
  server.on("/api/bedtime/start", HTTP_GET, handleApiBedtimeStart);
  server.on("/api/bedtime/cancel", HTTP_GET, handleApiBedtimeCancel);
  server.on("/api/time", HTTP_GET, handleApiTime);
  server.on("/api/time/sync", HTTP_GET, handleApiTimeSync);
  server.on("/api/time/set", HTTP_GET, handleApiTimeSet);
  server.on("/api/schedule", HTTP_GET, handleApiSchedule);
  server.on("/api/schedule/save", HTTP_GET, handleApiScheduleSave);
  server.on("/api/schedule/delete", HTTP_GET, handleApiScheduleDelete);
  server.on("/api/schedule/toggle", HTTP_GET, handleApiScheduleToggle);
  server.on("/api/schedule/skip", HTTP_GET, handleApiScheduleSkip);
  server.on("/api/schedule/run", HTTP_GET, handleApiScheduleRun);
  server.on("/api/schedule/export", HTTP_GET, handleApiScheduleExport);
  server.on("/api/schedule/import", HTTP_GET, handleApiScheduleImport);
  server.on("/api/schedule/reset", HTTP_GET, handleApiScheduleReset);
  server.on("/api/diagnostics", HTTP_GET, handleApiDiagnostics);
  server.on("/api/power", HTTP_GET, handleApiPower);
  server.on("/api/test", HTTP_GET, handleApiTest);
  server.on("/api/backup/export", HTTP_GET, handleApiBackupExport);
  server.on("/api/backup/import", HTTP_GET, handleApiBackupImport);
  server.on("/api/reset", HTTP_GET, handleApiReset);
  server.on("/go", HTTP_GET, handleGoLink);
  server.on("/set", HTTP_GET, handleSet);
  server.on("/state", HTTP_GET, handleState);
  server.onNotFound(handleNotFound);

  httpUpdater.setup(&server, "/update", OTA_USER, OTA_PASS);
}

void handleRoot() {
  server.send_P(200, PSTR("text/html"), INDEX_HTML);
}

void handleOtaPage() {
  String ip = WiFi.status() == WL_CONNECTED
    ? WiFi.localIP().toString()
    : WiFi.softAPIP().toString();

  String page;
  page.reserve(1800);
  page += F("<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>");
  page += F("<title>Bedroom LED OTA</title><style>");
  page += F("body{margin:0;background:#101214;color:#f4f7f8;font-family:system-ui,-apple-system,Segoe UI,sans-serif}");
  page += F("main{max-width:760px;margin:0 auto;padding:30px 16px}section{border:1px solid rgba(255,255,255,.12);background:#181c20;border-radius:8px;padding:18px}");
  page += F("a.button{display:inline-block;margin-top:12px;background:#8bd0ff;color:#101214;padding:11px 14px;border-radius:8px;text-decoration:none;font-weight:700}");
  page += F("code{background:#20262b;border-radius:6px;padding:2px 5px;color:#ffc680}p{line-height:1.5;color:#a9b4bb}</style></head><body><main>");
  page += F("<h1>Bedroom LED OTA</h1><section>");
  page += F("<p>Web firmware upload is available here. Username: <code>");
  page += OTA_USER;
  page += F("</code>. Password: <code>");
  page += OTA_PASS;
  page += F("</code>.</p><a class='button' href='/update'>Open Web Updater</a>");
  page += F("<p>Arduino IDE OTA is also enabled. Look for <code>");
  page += DEVICE_HOSTNAME;
  page += F("</code> on the same network. Current IP: <code>");
  page += ip;
  page += F("</code>.</p><p><a href='/'>Back to controls</a> | <a href='/diagnostics'>Diagnostics</a></p></section></main></body></html>");
  server.send(200, "text/html", page);
}

void handleSet() {
  if (server.hasArg("color")) {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    if (!parseHexColor(server.arg("color"), r, g, b)) {
      sendJsonError(400, "Invalid color");
      return;
    }
    setActiveColor(r, g, b);
  }

  if (server.hasArg("kelvin")) {
    int requestedKelvin;
    if (!parseIntArg("kelvin", requestedKelvin)) {
      sendJsonError(400, "Invalid temperature value");
      return;
    }
    setWhiteTemperature(static_cast<uint16_t>(constrain(requestedKelvin, 2700, 6500)));
  }

  if (server.hasArg("mode")) {
    Mode parsedMode;
    if (!tryModeFromString(server.arg("mode"), parsedMode)) {
      sendJsonError(400, "Unknown mode");
      return;
    }
    String error;
    if (!setModeByName(server.arg("mode"), error)) {
      sendJsonError(400, error);
      return;
    }
    beginTransitionToCurrentState();
  }

  if (server.hasArg("masterBrightness") || server.hasArg("brightness")) {
    String rawBrightness = server.hasArg("masterBrightness")
      ? server.arg("masterBrightness")
      : server.arg("brightness");
    int requestedBrightness;
    if (!parseRequestInt(rawBrightness, requestedBrightness)) {
      sendJsonError(400, "Invalid brightness value");
      return;
    }
    setMasterBrightness(static_cast<uint8_t>(constrain(requestedBrightness, 0, 255)));
  }

  if (server.hasArg("gammaEnabled")) {
    bool nextGammaEnabled;
    if (!parseRequestBool(server.arg("gammaEnabled"), nextGammaEnabled)) {
      sendJsonError(400, "Invalid gamma value");
      return;
    }
    setGammaEnabled(nextGammaEnabled);
  }

  if (server.hasArg("redGain") || server.hasArg("greenGain") || server.hasArg("blueGain")) {
    int redGain = settings.redGain;
    int greenGain = settings.greenGain;
    int blueGain = settings.blueGain;
    if (server.hasArg("redGain") && !parseIntArg("redGain", redGain)) {
      sendJsonError(400, "Invalid red gain");
      return;
    }
    if (server.hasArg("greenGain") && !parseIntArg("greenGain", greenGain)) {
      sendJsonError(400, "Invalid green gain");
      return;
    }
    if (server.hasArg("blueGain") && !parseIntArg("blueGain", blueGain)) {
      sendJsonError(400, "Invalid blue gain");
      return;
    }
    setRgbGains(
      static_cast<uint8_t>(constrain(redGain, 0, 255)),
      static_cast<uint8_t>(constrain(greenGain, 0, 255)),
      static_cast<uint8_t>(constrain(blueGain, 0, 255))
    );
  }

  if (server.hasArg("resetCalibration")) {
    setGammaEnabled(true);
    setRgbGains(255, 255, 255);
  }

  if (server.hasArg("supplyMillivolts") ||
      server.hasArg("supplyMilliamps") ||
      server.hasArg("supplySafetyMarginPercent") ||
      server.hasArg("controllerOverheadMilliamps")) {
    int millivolts = settings.supplyMillivolts;
    int milliamps = settings.supplyMilliamps;
    int margin = settings.supplySafetyMarginPercent;
    int overhead = settings.controllerOverheadMilliamps;

    if (server.hasArg("supplyMillivolts") && !parseIntArg("supplyMillivolts", millivolts)) {
      sendJsonError(400, "Invalid supply voltage");
      return;
    }
    if (server.hasArg("supplyMilliamps") && !parseIntArg("supplyMilliamps", milliamps)) {
      sendJsonError(400, "Invalid supply current");
      return;
    }
    if (server.hasArg("supplySafetyMarginPercent") && !parseIntArg("supplySafetyMarginPercent", margin)) {
      sendJsonError(400, "Invalid safety margin");
      return;
    }
    if (server.hasArg("controllerOverheadMilliamps") && !parseIntArg("controllerOverheadMilliamps", overhead)) {
      sendJsonError(400, "Invalid controller overhead");
      return;
    }

    setPowerConfig(
      static_cast<uint16_t>(constrain(millivolts, 3000, 24000)),
      static_cast<uint16_t>(constrain(milliamps, 100, 30000)),
      static_cast<uint8_t>(constrain(margin, 0, 90)),
      static_cast<uint16_t>(constrain(overhead, 0, 2000))
    );
  }

  if (server.hasArg("applyRecommendedBrightnessCap")) {
    PowerEstimate estimate = buildPowerEstimate();
    setMasterBrightness(estimate.recommendedBrightnessCap);
  }

  if (server.hasArg("bootBehavior")) {
    setBootBehaviorValue(server.arg("bootBehavior"));
  }

  if (server.hasArg("nightGuardEnabled")) {
    bool enabled;
    if (!parseRequestBool(server.arg("nightGuardEnabled"), enabled)) {
      sendJsonError(400, "Invalid Night Guard enabled value");
      return;
    }
    setNightGuardEnabled(enabled);
  }
  if (server.hasArg("nightGuardMaxBrightness")) {
    int maxBrightness;
    if (!parseIntArg("nightGuardMaxBrightness", maxBrightness)) {
      sendJsonError(400, "Invalid Night Guard brightness");
      return;
    }
    setNightGuardMaxBrightness(static_cast<uint8_t>(constrain(maxBrightness, 0, 255)));
  }
  if (server.hasArg("nightGuardBlockFlashing")) {
    bool blockFlashing;
    if (!parseRequestBool(server.arg("nightGuardBlockFlashing"), blockFlashing)) {
      sendJsonError(400, "Invalid Night Guard flashing value");
      return;
    }
    setNightGuardBlockFlashingModes(blockFlashing);
  }
  if (server.hasArg("nightGuardPreferWarm")) {
    bool preferWarm;
    if (!parseRequestBool(server.arg("nightGuardPreferWarm"), preferWarm)) {
      sendJsonError(400, "Invalid Night Guard warm preference");
      return;
    }
    setNightGuardPreferWarmTemperature(preferWarm);
  }

  if (server.hasArg("strobeDelay")) {
    float requestedDelay;
    if (!parseRequestFloat(server.arg("strobeDelay"), requestedDelay)) {
      sendJsonError(400, "Invalid strobe delay");
      return;
    }
    setFloatSetting(settings.strobeDelaySec, requestedDelay, 0.1f, 10.0f);
  }
  if (server.hasArg("flashOffDelay")) {
    float requestedDelay;
    if (!parseRequestFloat(server.arg("flashOffDelay"), requestedDelay)) {
      sendJsonError(400, "Invalid flash delay");
      return;
    }
    setFloatSetting(settings.flashOffDelaySec, requestedDelay, 0.1f, 10.0f);
  }
  if (server.hasArg("chasePeriod")) {
    int requestedPeriod;
    if (!parseIntArg("chasePeriod", requestedPeriod)) {
      sendJsonError(400, "Invalid chase period");
      return;
    }
    setByteSetting(settings.chasePeriodSec, requestedPeriod, 1, 15, false);
  }
  if (server.hasArg("wavePeriod")) {
    int requestedPeriod;
    if (!parseIntArg("wavePeriod", requestedPeriod)) {
      sendJsonError(400, "Invalid wave period");
      return;
    }
    setByteSetting(settings.wavePeriodSec, requestedPeriod, 5, 30, false);
  }
  if (server.hasArg("slowPulseCount")) {
    int requestedCount;
    if (!parseIntArg("slowPulseCount", requestedCount)) {
      sendJsonError(400, "Invalid pulse count");
      return;
    }
    setByteSetting(settings.slowPulseCount, requestedCount, 1, 15, true);
  }
  if (server.hasArg("slowPulseMax")) {
    int requestedMax;
    if (!parseIntArg("slowPulseMax", requestedMax)) {
      sendJsonError(400, "Invalid pulse life");
      return;
    }
    setByteSetting(settings.slowPulseMaxSec, requestedMax, 1, 60, true);
  }
  if (server.hasArg("rainbowPeriod")) {
    int requestedPeriod;
    if (!parseIntArg("rainbowPeriod", requestedPeriod)) {
      sendJsonError(400, "Invalid rainbow period");
      return;
    }
    setByteSetting(settings.rainbowPeriodSec, requestedPeriod, 1, 30, false);
  }

  handleState();
}

void handleState() {
  server.send(200, "application/json", buildStateJson());
}

void handleApiState() {
  server.send(200, "application/json", buildStateJson());
}

void handleApiBrightness() {
  if (!server.hasArg("value")) {
    sendJsonOk("Brightness state", buildStateJson());
    return;
  }

  int requestedBrightness;
  if (!parseIntArg("value", requestedBrightness)) {
    sendJsonError(400, "Invalid brightness value");
    return;
  }

  setMasterBrightness(static_cast<uint8_t>(constrain(requestedBrightness, 0, 255)));
  sendJsonOk("Brightness updated", buildStateJson());
}

void handleApiColor() {
  uint8_t r;
  uint8_t g;
  uint8_t b;

  if (server.hasArg("hex")) {
    if (!parseHexColor(server.arg("hex"), r, g, b)) {
      sendJsonError(400, "Invalid color");
      return;
    }
  } else {
    int red;
    int green;
    int blue;
    if (!server.hasArg("r") || !server.hasArg("g") || !server.hasArg("b") ||
        !parseIntArg("r", red) ||
        !parseIntArg("g", green) ||
        !parseIntArg("b", blue)) {
      sendJsonError(400, "Invalid color");
      return;
    }
    r = static_cast<uint8_t>(constrain(red, 0, 255));
    g = static_cast<uint8_t>(constrain(green, 0, 255));
    b = static_cast<uint8_t>(constrain(blue, 0, 255));
  }

  setActiveColor(r, g, b);
  sendJsonOk("Color updated", buildStateJson());
}

void handleApiTemperature() {
  String argName;
  if (server.hasArg("k")) {
    argName = "k";
  } else if (server.hasArg("kelvin")) {
    argName = "kelvin";
  } else {
    sendJsonOk("Temperature state", buildStateJson());
    return;
  }

  int requestedKelvin;
  if (!parseIntArg(argName, requestedKelvin)) {
    sendJsonError(400, "Invalid temperature value");
    return;
  }

  setWhiteTemperature(static_cast<uint16_t>(constrain(requestedKelvin, 2700, 6500)));
  sendJsonOk("Temperature updated", buildStateJson());
}

void handleApiMode() {
  String requestedMode;
  if (server.hasArg("name")) {
    requestedMode = server.arg("name");
  } else if (server.hasArg("mode")) {
    requestedMode = server.arg("mode");
  } else {
    sendJsonOk("Mode state", buildStateJson());
    return;
  }

  Mode parsedMode;
  if (!tryModeFromString(requestedMode, parsedMode)) {
    sendJsonError(400, "Unknown mode");
    return;
  }

  String error;
  if (!setModeByName(requestedMode, error)) {
    sendJsonError(400, error);
    return;
  }
  beginTransitionToCurrentState();
  sendJsonOk("Mode updated", buildStateJson());
}

void handleNotFound() {
  sendJsonError(404, "Not found");
}

#endif
