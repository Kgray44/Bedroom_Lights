#ifndef BEDROOM_LED_CONTROLLER_SETTINGS_STORAGE_H
#define BEDROOM_LED_CONTROLLER_SETTINGS_STORAGE_H

String normalizeBootBehavior(String value) {
  value.trim();
  if (value == "startOff" || value == "warmDim") {
    return value;
  }
  return "restore";
}

bool readJsonString(const String& json, const char* key, String& value) {
  String marker = String("\"") + key + "\":\"";
  int start = json.indexOf(marker);
  if (start < 0) {
    return false;
  }
  start += marker.length();
  int end = json.indexOf('"', start);
  if (end < 0) {
    return false;
  }
  value = json.substring(start, end);
  return true;
}

bool readJsonNumberToken(const String& json, const char* key, String& token) {
  String marker = String("\"") + key + "\":";
  int start = json.indexOf(marker);
  if (start < 0) {
    return false;
  }
  start += marker.length();
  while (start < static_cast<int>(json.length()) && isspace(json[start])) {
    start++;
  }
  int end = start;
  while (end < static_cast<int>(json.length()) && json[end] != ',' && json[end] != '}') {
    end++;
  }
  token = json.substring(start, end);
  token.trim();
  return token.length() > 0;
}

bool readJsonInt(const String& json, const char* key, int& value) {
  String token;
  if (!readJsonNumberToken(json, key, token)) {
    return false;
  }
  char* tail = nullptr;
  long parsed = strtol(token.c_str(), &tail, 10);
  if (tail == token.c_str()) {
    return false;
  }
  value = static_cast<int>(parsed);
  return true;
}

bool readJsonFloat(const String& json, const char* key, float& value) {
  String token;
  if (!readJsonNumberToken(json, key, token)) {
    return false;
  }
  char* tail = nullptr;
  double parsed = strtod(token.c_str(), &tail);
  if (tail == token.c_str()) {
    return false;
  }
  value = static_cast<float>(parsed);
  return true;
}

bool readJsonBool(const String& json, const char* key, bool& value) {
  String token;
  if (!readJsonNumberToken(json, key, token)) {
    return false;
  }
  if (token == "true") {
    value = true;
    return true;
  }
  if (token == "false") {
    value = false;
    return true;
  }
  return false;
}

void applyDefaultSettings() {
  settings.mode = MODE_SOLID;
  settings.color = {255, 178, 109};
  settings.whiteKelvin = 3000;
  settings.usingWhiteTemperature = true;
  settings.masterBrightness = MASTER_BRIGHTNESS;
  settings.strobeDelaySec = 1.0f;
  settings.flashOffDelaySec = 1.0f;
  settings.chasePeriodSec = 5;
  settings.wavePeriodSec = 12;
  settings.slowPulseCount = 5;
  settings.slowPulseMaxSec = 20;
  settings.rainbowPeriodSec = 10;
  settings.bootBehavior = "restore";
  settings.gammaEnabled = true;
  settings.redGain = 255;
  settings.greenGain = 255;
  settings.blueGain = 255;
  settings.supplyMillivolts = 5000;
  settings.supplyMilliamps = 5000;
  settings.supplySafetyMarginPercent = 20;
  settings.controllerOverheadMilliamps = 200;
  settings.nightGuardEnabled = false;
  settings.nightGuardMaxBrightness = NIGHT_GUARD_DEFAULT_MAX_BRIGHTNESS;
  settings.nightGuardBlockFlashingModes = true;
  settings.nightGuardForceWarmOnPanic = true;
  settings.nightGuardPreferWarmTemperature = false;
  settings.bedtimeFadeDefaultMinutes = 30;
  settings.bedtimeFadeDefaultTarget = "warmDim";
  settings.bedtimeFadeTurnOffAtEnd = false;
  settings.transitionsEnabled = true;
  settings.transitionDurationMs = 1500;
  settings.transitionStyle = "fade";
  settings.paletteEnabled = false;
  settings.activePaletteId = "warm-linen";
  settings.localUtcOffsetMinutes = DEFAULT_LOCAL_UTC_OFFSET_MINUTES;
}

void applyBootBehavior() {
  settings.bootBehavior = normalizeBootBehavior(settings.bootBehavior);

  if (settings.bootBehavior == "startOff") {
    settings.mode = MODE_SOLID;
    settings.color = {0, 0, 0};
    settings.whiteKelvin = 0;
    settings.usingWhiteTemperature = false;
    settings.masterBrightness = 0;
  } else if (settings.bootBehavior == "warmDim") {
    settings.mode = MODE_SOLID;
    settings.whiteKelvin = WARM_DIM_KELVIN;
    settings.usingWhiteTemperature = true;
    settings.color = kelvinToRgb(WARM_DIM_KELVIN);
    settings.masterBrightness = WARM_DIM_BRIGHTNESS;
  }

  ledsDirty = true;
}

void loadSettings() {
  settingsStorageReady = LittleFS.begin();
  if (!settingsStorageReady) {
    settingsLoadedOk = false;
    settingsLoadStatus = "LittleFS unavailable; defaults active";
    return;
  }

  if (!LittleFS.exists(SETTINGS_PATH)) {
    settingsLoadedOk = true;
    settingsLoadStatus = "missing; defaults active";
    return;
  }

  File file = LittleFS.open(SETTINGS_PATH, "r");
  if (!file) {
    settingsLoadedOk = false;
    settingsLoadStatus = "open failed; defaults active";
    return;
  }

  String json = file.readString();
  file.close();
  json.trim();
  if (!json.startsWith("{")) {
    settingsLoadedOk = false;
    settingsLoadStatus = "corrupt; defaults active";
    return;
  }

  String textValue;
  int intValue;
  float floatValue;
  bool boolValue;

  if (readJsonString(json, "mode", textValue)) {
    settings.mode = modeFromString(textValue);
  }
  if (readJsonInt(json, "red", intValue)) {
    settings.color.r = constrain(intValue, 0, 255);
  }
  if (readJsonInt(json, "green", intValue)) {
    settings.color.g = constrain(intValue, 0, 255);
  }
  if (readJsonInt(json, "blue", intValue)) {
    settings.color.b = constrain(intValue, 0, 255);
  }
  if (readJsonInt(json, "whiteKelvin", intValue)) {
    settings.whiteKelvin = constrain(intValue, 0, 6500);
  }
  if (readJsonBool(json, "usingWhiteTemperature", boolValue)) {
    settings.usingWhiteTemperature = boolValue;
  }
  if (readJsonInt(json, "masterBrightness", intValue)) {
    settings.masterBrightness = constrain(intValue, 0, 255);
  }
  if (readJsonFloat(json, "strobeDelaySec", floatValue)) {
    settings.strobeDelaySec = constrain(floatValue, 0.1f, 10.0f);
  }
  if (readJsonFloat(json, "flashOffDelaySec", floatValue)) {
    settings.flashOffDelaySec = constrain(floatValue, 0.1f, 10.0f);
  }
  if (readJsonInt(json, "chasePeriodSec", intValue)) {
    settings.chasePeriodSec = constrain(intValue, 1, 15);
  }
  if (readJsonInt(json, "wavePeriodSec", intValue)) {
    settings.wavePeriodSec = constrain(intValue, 5, 30);
  }
  if (readJsonInt(json, "slowPulseCount", intValue)) {
    settings.slowPulseCount = constrain(intValue, 1, 15);
  }
  if (readJsonInt(json, "slowPulseMaxSec", intValue)) {
    settings.slowPulseMaxSec = constrain(intValue, 1, 60);
  }
  if (readJsonInt(json, "rainbowPeriodSec", intValue)) {
    settings.rainbowPeriodSec = constrain(intValue, 1, 30);
  }
  if (readJsonString(json, "bootBehavior", textValue)) {
    settings.bootBehavior = normalizeBootBehavior(textValue);
  }
  if (readJsonBool(json, "gammaEnabled", boolValue)) {
    settings.gammaEnabled = boolValue;
  }
  if (readJsonInt(json, "redGain", intValue)) {
    settings.redGain = constrain(intValue, 0, 255);
  }
  if (readJsonInt(json, "greenGain", intValue)) {
    settings.greenGain = constrain(intValue, 0, 255);
  }
  if (readJsonInt(json, "blueGain", intValue)) {
    settings.blueGain = constrain(intValue, 0, 255);
  }
  if (readJsonInt(json, "supplyMillivolts", intValue)) {
    settings.supplyMillivolts = constrain(intValue, 3000, 24000);
  }
  if (readJsonInt(json, "supplyMilliamps", intValue)) {
    settings.supplyMilliamps = constrain(intValue, 100, 30000);
  }
  if (readJsonInt(json, "supplySafetyMarginPercent", intValue)) {
    settings.supplySafetyMarginPercent = constrain(intValue, 0, 90);
  }
  if (readJsonInt(json, "controllerOverheadMilliamps", intValue)) {
    settings.controllerOverheadMilliamps = constrain(intValue, 0, 2000);
  }
  if (readJsonBool(json, "nightGuardEnabled", boolValue)) {
    settings.nightGuardEnabled = boolValue;
  }
  if (readJsonInt(json, "nightGuardMaxBrightness", intValue)) {
    settings.nightGuardMaxBrightness = constrain(intValue, 0, 255);
  }
  if (readJsonBool(json, "nightGuardBlockFlashingModes", boolValue)) {
    settings.nightGuardBlockFlashingModes = boolValue;
  }
  if (readJsonBool(json, "nightGuardForceWarmOnPanic", boolValue)) {
    settings.nightGuardForceWarmOnPanic = boolValue;
  }
  if (readJsonBool(json, "nightGuardPreferWarmTemperature", boolValue)) {
    settings.nightGuardPreferWarmTemperature = boolValue;
  }
  if (readJsonInt(json, "bedtimeFadeDefaultMinutes", intValue)) {
    settings.bedtimeFadeDefaultMinutes = constrain(intValue, 1, TIMER_MAX_MINUTES);
  }
  if (readJsonString(json, "bedtimeFadeDefaultTarget", textValue)) {
    textValue.trim();
    if (textValue == "warmDim" || textValue == "off" || textValue == "sleep") {
      settings.bedtimeFadeDefaultTarget = textValue;
    }
  }
  if (readJsonBool(json, "bedtimeFadeTurnOffAtEnd", boolValue)) {
    settings.bedtimeFadeTurnOffAtEnd = boolValue;
  }
  if (readJsonBool(json, "transitionsEnabled", boolValue)) {
    settings.transitionsEnabled = boolValue;
  }
  if (readJsonInt(json, "transitionDurationMs", intValue)) {
    uint16_t duration = static_cast<uint16_t>(constrain(intValue, 0, 5000));
    if (isSupportedTransitionDuration(duration)) {
      settings.transitionDurationMs = duration;
    }
  }
  if (readJsonString(json, "transitionStyle", textValue)) {
    textValue.trim();
    textValue.toLowerCase();
    if (isSupportedTransitionStyle(textValue)) {
      settings.transitionStyle = normalizeTransitionStyle(textValue);
    }
  }
  if (readJsonBool(json, "paletteEnabled", boolValue)) {
    settings.paletteEnabled = boolValue;
  }
  if (readJsonString(json, "activePaletteId", textValue)) {
    settings.activePaletteId = safePaletteIdToken(textValue);
  }
  if (readJsonInt(json, "localUtcOffsetMinutes", intValue)) {
    settings.localUtcOffsetMinutes = constrain(intValue, -720, 840);
  }

  settingsLoadedOk = true;
  settingsLoadStatus = "loaded";
}

void saveSettings() {
  if (!settingsStorageReady) {
    settingsSaveOk = false;
    settingsSaveStatus = "skipped; LittleFS unavailable";
    settingsDirty = false;
    return;
  }

  File file = LittleFS.open(SETTINGS_PATH, "w");
  if (!file) {
    settingsSaveOk = false;
    settingsSaveStatus = "open failed";
    settingsDirty = false;
    return;
  }

  String json;
  json.reserve(1240);
  // settings schema {"version":7 includes palette state; Phase 4A appends local UTC offset.
  json += F("{\"version\":7");
  json += F(",\"mode\":\"");
  json += modeKey(settings.mode);
  json += F("\",\"red\":");
  json += settings.color.r;
  json += F(",\"green\":");
  json += settings.color.g;
  json += F(",\"blue\":");
  json += settings.color.b;
  json += F(",\"whiteKelvin\":");
  json += settings.whiteKelvin;
  json += F(",\"usingWhiteTemperature\":");
  json += settings.usingWhiteTemperature ? F("true") : F("false");
  json += F(",\"masterBrightness\":");
  json += settings.masterBrightness;
  json += F(",\"strobeDelaySec\":");
  json += String(settings.strobeDelaySec, 1);
  json += F(",\"flashOffDelaySec\":");
  json += String(settings.flashOffDelaySec, 1);
  json += F(",\"chasePeriodSec\":");
  json += settings.chasePeriodSec;
  json += F(",\"wavePeriodSec\":");
  json += settings.wavePeriodSec;
  json += F(",\"slowPulseCount\":");
  json += settings.slowPulseCount;
  json += F(",\"slowPulseMaxSec\":");
  json += settings.slowPulseMaxSec;
  json += F(",\"rainbowPeriodSec\":");
  json += settings.rainbowPeriodSec;
  json += F(",\"bootBehavior\":\"");
  json += normalizeBootBehavior(settings.bootBehavior);
  json += F("\",\"gammaEnabled\":");
  json += settings.gammaEnabled ? F("true") : F("false");
  json += F(",\"redGain\":");
  json += settings.redGain;
  json += F(",\"greenGain\":");
  json += settings.greenGain;
  json += F(",\"blueGain\":");
  json += settings.blueGain;
  json += F(",\"supplyMillivolts\":");
  json += settings.supplyMillivolts;
  json += F(",\"supplyMilliamps\":");
  json += settings.supplyMilliamps;
  json += F(",\"supplySafetyMarginPercent\":");
  json += settings.supplySafetyMarginPercent;
  json += F(",\"controllerOverheadMilliamps\":");
  json += settings.controllerOverheadMilliamps;
  json += F(",\"nightGuardEnabled\":");
  json += settings.nightGuardEnabled ? F("true") : F("false");
  json += F(",\"nightGuardMaxBrightness\":");
  json += settings.nightGuardMaxBrightness;
  json += F(",\"nightGuardBlockFlashingModes\":");
  json += settings.nightGuardBlockFlashingModes ? F("true") : F("false");
  json += F(",\"nightGuardForceWarmOnPanic\":");
  json += settings.nightGuardForceWarmOnPanic ? F("true") : F("false");
  json += F(",\"nightGuardPreferWarmTemperature\":");
  json += settings.nightGuardPreferWarmTemperature ? F("true") : F("false");
  json += F(",\"bedtimeFadeDefaultMinutes\":");
  json += settings.bedtimeFadeDefaultMinutes;
  json += F(",\"bedtimeFadeDefaultTarget\":\"");
  json += escapeJson(settings.bedtimeFadeDefaultTarget);
  json += F("\"");
  json += F(",\"bedtimeFadeTurnOffAtEnd\":");
  json += settings.bedtimeFadeTurnOffAtEnd ? F("true") : F("false");
  json += F(",\"transitionsEnabled\":");
  json += settings.transitionsEnabled ? F("true") : F("false");
  json += F(",\"transitionDurationMs\":");
  json += settings.transitionDurationMs;
  json += F(",\"transitionStyle\":\"");
  json += escapeJson(normalizeTransitionStyle(settings.transitionStyle));
  json += F("\"");
  json += F(",\"paletteEnabled\":");
  json += settings.paletteEnabled ? F("true") : F("false");
  json += F(",\"activePaletteId\":\"");
  json += escapeJson(settings.activePaletteId);
  json += F("\"");
  json += F(",\"localUtcOffsetMinutes\":");
  json += settings.localUtcOffsetMinutes;
  json += F("}");

  if (file.print(json) == json.length()) {
    settingsSaveOk = true;
    lastSettingsSaveMs = millis();
    settingsSaveStatus = "saved";
  } else {
    settingsSaveOk = false;
    settingsSaveStatus = "write incomplete";
  }
  file.close();
  settingsDirty = false;
}

void markSettingsDirty() {
  settingsDirty = true;
  settingsDirtySince = millis();
}

void serviceSettingsSave() {
  if (settingsDirty && millis() - settingsDirtySince >= SETTINGS_SAVE_DEBOUNCE_MS) {
    saveSettings();
  }
}

#endif
