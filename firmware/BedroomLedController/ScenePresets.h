#ifndef BEDROOM_LED_CONTROLLER_SCENE_PRESETS_H
#define BEDROOM_LED_CONTROLLER_SCENE_PRESETS_H

String cleanSceneName(const String& rawName) {
  String name = rawName;
  name.trim();
  while (name.length() > MAX_SCENE_NAME_LENGTH) {
    name.remove(name.length() - 1);
  }
  return name;
}

String sceneColorHex(const ScenePreset& scene) {
  char hex[8];
  snprintf(hex, sizeof(hex), "#%02X%02X%02X", scene.red, scene.green, scene.blue);
  return String(hex);
}

String safeSceneIdToken(const String& value) {
  String token;
  token.reserve(value.length());
  bool previousDash = false;

  for (uint16_t i = 0; i < value.length() && token.length() < MAX_SCENE_ID_LENGTH; i++) {
    char c = static_cast<char>(tolower(value[i]));
    bool usable = (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
    if (usable) {
      token += c;
      previousDash = false;
    } else if (!previousDash && token.length() > 0) {
      token += '-';
      previousDash = true;
    }
  }

  while (token.endsWith("-")) {
    token.remove(token.length() - 1);
  }
  if (token.length() == 0) {
    token = "scene";
  }
  return token;
}

int findSceneById(const String& id) {
  for (uint8_t i = 0; i < sceneCount; i++) {
    if (scenes[i].id == id) {
      return i;
    }
  }
  return -1;
}

bool sceneNameExists(const String& name, int ignoreIndex) {
  String needle = cleanSceneName(name);
  needle.toLowerCase();
  for (uint8_t i = 0; i < sceneCount; i++) {
    if (static_cast<int>(i) == ignoreIndex) {
      continue;
    }
    String candidate = cleanSceneName(scenes[i].name);
    candidate.toLowerCase();
    if (candidate == needle) {
      return true;
    }
  }
  return false;
}

String makeSceneId(const String& name) {
  String base = safeSceneIdToken(name);
  String candidate = base;
  uint8_t suffix = 2;

  while (findSceneById(candidate) >= 0) {
    candidate = base;
    candidate += '-';
    candidate += suffix;
    suffix++;
    while (candidate.length() > MAX_SCENE_ID_LENGTH) {
      candidate.remove(candidate.length() - 1);
    }
  }
  return candidate;
}

ScenePreset captureCurrentScene(const String& id, const String& name, bool builtin) {
  ScenePreset scene;
  scene.id = id;
  scene.name = cleanSceneName(name);
  scene.category = builtin ? "Built-in" : "Custom";
  scene.mode = modeKey(settings.mode);
  scene.red = settings.color.r;
  scene.green = settings.color.g;
  scene.blue = settings.color.b;
  scene.whiteKelvin = settings.whiteKelvin;
  scene.usingWhiteTemperature = settings.usingWhiteTemperature;
  scene.masterBrightness = settings.masterBrightness;
  scene.strobeDelaySec = settings.strobeDelaySec;
  scene.flashOffDelaySec = settings.flashOffDelaySec;
  scene.chasePeriodSec = settings.chasePeriodSec;
  scene.wavePeriodSec = settings.wavePeriodSec;
  scene.slowPulseCount = settings.slowPulseCount;
  scene.slowPulseMaxSec = settings.slowPulseMaxSec;
  scene.rainbowPeriodSec = settings.rainbowPeriodSec;
  scene.candleHallFlickerAmount = settings.candleHallFlickerAmount;
  scene.animationStrength = settings.animationStrength;
  scene.builtin = builtin;
  scene.isFavoriteCandidate = false;
  scene.isSleepSafe = false;
  scene.isFocusFriendly = false;
  scene.paletteEnabled = settings.paletteEnabled;
  scene.paletteId = settings.activePaletteId;
  return scene;
}

bool addScenePreset(const ScenePreset& scene) {
  if (sceneCount >= MAX_SCENES || scene.id.length() == 0 || scene.name.length() == 0) {
    return false;
  }
  scenes[sceneCount] = scene;
  sceneCount++;
  return true;
}

bool addBuiltInScene(
  const String& id,
  const String& name,
  const String& category,
  const String& mode,
  uint8_t red,
  uint8_t green,
  uint8_t blue,
  uint16_t whiteKelvin,
  bool usingWhiteTemperature,
  uint8_t masterBrightness,
  bool isFavoriteCandidate,
  bool isSleepSafe,
  bool isFocusFriendly
) {
  if (findSceneById(id) >= 0) {
    return false;
  }

  Mode parsedMode;
  if (!tryModeFromString(mode, parsedMode)) {
    parsedMode = MODE_SOLID;
  }

  ScenePreset scene;
  scene.id = id;
  scene.name = cleanSceneName(name);
  scene.category = category;
  scene.mode = modeKey(parsedMode);
  scene.red = red;
  scene.green = green;
  scene.blue = blue;
  scene.whiteKelvin = constrain(whiteKelvin, 2700, 6500);
  scene.usingWhiteTemperature = usingWhiteTemperature;
  scene.masterBrightness = masterBrightness;
  scene.strobeDelaySec = 1.0f;
  scene.flashOffDelaySec = 1.0f;
  scene.chasePeriodSec = 5;
  scene.wavePeriodSec = 12;
  scene.slowPulseCount = 5;
  scene.slowPulseMaxSec = 20;
  scene.rainbowPeriodSec = 10;
  scene.candleHallFlickerAmount = 3;
  scene.animationStrength = 128;
  scene.builtin = true;
  scene.isFavoriteCandidate = isFavoriteCandidate;
  scene.isSleepSafe = isSleepSafe;
  scene.isFocusFriendly = isFocusFriendly;
  scene.paletteEnabled = false;
  scene.paletteId = "warm-linen";
  return addScenePreset(scene);
}

uint8_t addDefaultBuiltInScenes() {
  uint8_t before = sceneCount;
  addBuiltInScene("warm-dim", "Warm Dim", "Calm", "solid", 255, 178, 109, 2700, true, 45, true, true, false);
  addBuiltInScene("reading", "Reading", "Focus", "solid", 255, 228, 190, 4000, true, 160, true, false, true);
  addBuiltInScene("sleep", "Sleep", "Sleep", "solid", 255, 142, 72, 2700, true, 22, true, true, false);
  addBuiltInScene("movie", "Movie", "Calm", "moonTide", 80, 125, 205, 3000, false, 48, true, true, false);
  addBuiltInScene("focus", "Focus", "Focus", "solid", 245, 250, 255, 5000, true, 180, true, false, true);
  addBuiltInScene("study-focus", "Study / Focus", "Focus", "solid", 246, 250, 255, 5000, true, 170, true, false, true);
  addBuiltInScene("workbench-mode", "Workbench Mode", "Utility", "solid", 255, 255, 245, 5500, true, 205, true, false, true);
  addBuiltInScene("sera-call", "Sera Call", "Everyday", "lanternDrift", 255, 185, 120, 3200, false, 92, true, true, true);
  addBuiltInScene("cozy-goblin-mode", "Cozy Goblin Mode", "Calm", "solid", 255, 150, 78, 2700, true, 38, true, true, false);
  addBuiltInScene("rain-glass", "Rain Glass", "Weather", "rainGlass", 170, 220, 255, 3000, false, 110, true, true, false);
  addBuiltInScene("moon-tide", "Moon Tide", "Calm", "moonTide", 160, 205, 255, 3000, false, 90, true, true, false);
  addBuiltInScene("lantern-drift", "Lantern Drift", "Calm", "lanternDrift", 255, 170, 82, 2700, false, 105, true, true, true);
  addBuiltInScene("dawn-bloom", "Dawn Bloom", "Calm", "dawnBloom", 255, 180, 112, 3000, false, 85, true, true, true);
  addBuiltInScene("satin-breathing", "Satin Breathing", "Sleep", "satinBreathing", 255, 150, 88, 2700, true, 38, false, true, false);
  addBuiltInScene("library-lamp", "Library Lamp", "Focus", "libraryLamp", 255, 224, 184, 3400, true, 155, false, true, true);
  addBuiltInScene("solar-noon", "Solar Noon", "Focus", "solarNoon", 244, 250, 255, 5200, true, 190, false, false, true);
  return sceneCount - before;
}

void loadDefaultScenes() {
  sceneCount = 0;
  addDefaultBuiltInScenes();
  sceneLoadStatus = "defaults active";
}

void appendSceneWarning(String& json, bool& first, const char* code, const String& message) {
  if (!first) {
    json += ',';
  }
  first = false;
  json += R"json({"code":")json";
  json += code;
  json += R"json(","message":")json";
  json += escapeJson(message);
  json += F("\"}");
}

String buildSceneWarningsJson(const ScenePreset& scene) {
  String json;
  json.reserve(180);
  json += '[';
  bool first = true;

  Mode parsedMode;
  if (!tryModeFromString(scene.mode, parsedMode)) {
    appendSceneWarning(json, first, "missingMode", "Scene mode is not available in this firmware");
  }
  if (scene.paletteEnabled && findPaletteById(scene.paletteId) < 0) {
    appendSceneWarning(json, first, "missingPalette", "Scene palette is not available");
  }
  if (isModeBlockedByNightGuard(scene.mode)) {
    appendSceneWarning(json, first, "nightGuardBlocked", nightGuardBlockMessage(scene.mode));
  }

  PowerEstimate estimate = buildPowerEstimate();
  if (scene.masterBrightness > estimate.recommendedBrightnessCap && estimate.recommendedBrightnessCap < 255) {
    String message = "Scene brightness is above the current powerBrightness recommendation of ";
    message += estimate.recommendedBrightnessCap;
    message += " / 255";
    appendSceneWarning(json, first, "powerBrightness", message);
  }

  json += ']';
  return json;
}

String buildSingleSceneJson(const ScenePreset& scene) {
  String json;
  json.reserve(650);
  json += F("{\"id\":\"");
  json += escapeJson(scene.id);
  json += F("\",\"name\":\"");
  json += escapeJson(scene.name);
  json += R"json(","category":")json";
  json += escapeJson(scene.category);
  json += R"json(","mode":")json";
  json += escapeJson(scene.mode);
  json += F("\",\"red\":");
  json += scene.red;
  json += F(",\"green\":");
  json += scene.green;
  json += F(",\"blue\":");
  json += scene.blue;
  json += F(",\"colorHex\":\"");
  json += sceneColorHex(scene);
  json += F("\",\"whiteKelvin\":");
  json += scene.whiteKelvin;
  json += F(",\"usingWhiteTemperature\":");
  json += boolJson(scene.usingWhiteTemperature);
  json += F(",\"masterBrightness\":");
  json += scene.masterBrightness;
  json += F(",\"strobeDelaySec\":");
  json += String(scene.strobeDelaySec, 1);
  json += F(",\"flashOffDelaySec\":");
  json += String(scene.flashOffDelaySec, 1);
  json += F(",\"chasePeriodSec\":");
  json += scene.chasePeriodSec;
  json += F(",\"wavePeriodSec\":");
  json += scene.wavePeriodSec;
  json += F(",\"slowPulseCount\":");
  json += scene.slowPulseCount;
  json += F(",\"slowPulseMaxSec\":");
  json += scene.slowPulseMaxSec;
  json += F(",\"rainbowPeriodSec\":");
  json += scene.rainbowPeriodSec;
  json += F(",\"candleHallFlickerAmount\":");
  json += scene.candleHallFlickerAmount;
  json += F(",\"animationStrength\":");
  json += scene.animationStrength;
  json += F(",\"animationIntensity\":");
  json += scene.animationStrength;
  json += F(",\"builtin\":");
  json += boolJson(scene.builtin);
  json += R"json(,"isFavoriteCandidate":)json";
  json += boolJson(scene.isFavoriteCandidate);
  json += R"json(,"isSleepSafe":)json";
  json += boolJson(scene.isSleepSafe);
  json += R"json(,"isFocusFriendly":)json";
  json += boolJson(scene.isFocusFriendly);
  json += F(",\"paletteEnabled\":");
  json += boolJson(scene.paletteEnabled);
  json += F(",\"paletteId\":\"");
  json += escapeJson(scene.paletteId);
  json += R"json(","warnings":)json";
  json += buildSceneWarningsJson(scene);
  json += F("}");
  return json;
}

String buildSceneExportJson(const ScenePreset& scene) {
  String json;
  json.reserve(120 + buildSingleSceneJson(scene).length());
  json += R"json({"ok":true,"sceneSchema":1,"exportType":"scene","scene":)json";
  json += buildSingleSceneJson(scene);
  json += F("}");
  return json;
}

String buildScenesJson() {
  String json;
  json.reserve(420 + (sceneCount * 340));
  json += F("{\"ok\":true,\"version\":1,\"count\":");
  json += sceneCount;
  json += F(",\"maxScenes\":");
  json += MAX_SCENES;
  json += F(",\"path\":\"");
  json += SCENES_PATH;
  json += F("\",\"loadStatus\":\"");
  json += escapeJson(sceneLoadStatus);
  json += F("\",\"saveStatus\":\"");
  json += escapeJson(sceneSaveStatus);
  json += F("\",\"scenes\":[");
  for (uint8_t i = 0; i < sceneCount; i++) {
    if (i > 0) {
      json += ',';
    }
    json += buildSingleSceneJson(scenes[i]);
  }
  json += F("]}");
  return json;
}

void sendJsonContentChunk(const String& chunk, uint32_t& payloadBytes) {
  server.sendContent(chunk);
  payloadBytes += chunk.length();
  yield();
}

void sendScenesJsonDocumentChunks(uint32_t& payloadBytes) {
  String header;
  header.reserve(220);
  header += F("{\"ok\":true,\"version\":1,\"count\":");
  header += sceneCount;
  header += F(",\"maxScenes\":");
  header += MAX_SCENES;
  header += F(",\"path\":\"");
  header += SCENES_PATH;
  header += F("\",\"loadStatus\":\"");
  header += escapeJson(sceneLoadStatus);
  header += F("\",\"saveStatus\":\"");
  header += escapeJson(sceneSaveStatus);
  header += F("\",\"scenes\":[");
  sendJsonContentChunk(header, payloadBytes);

  for (uint8_t i = 0; i < sceneCount; i++) {
    if (i > 0) {
      sendJsonContentChunk(F(","), payloadBytes);
    }
    sendJsonContentChunk(buildSingleSceneJson(scenes[i]), payloadBytes);
  }

  sendJsonContentChunk(F("]}"), payloadBytes);
}

void streamScenesJson() {
  uint32_t heapBefore = beginEndpointHeapMetric("/api/scenes");
  uint32_t payloadBytes = 0;
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "application/json", "");
  sendScenesJsonDocumentChunks(payloadBytes);
  finishEndpointHeapMetric("/api/scenes", heapBefore, payloadBytes);
}

bool saveScenes() {
  if (!settingsStorageReady) {
    sceneSaveStatus = "skipped; LittleFS unavailable";
    return false;
  }

  File file = LittleFS.open(SCENES_PATH, "w");
  if (!file) {
    sceneSaveStatus = "open failed";
    return false;
  }

  String json = buildScenesJson();
  bool saved = file.print(json) == json.length();
  file.close();
  sceneSaveStatus = saved ? "saved" : "write incomplete";
  return saved;
}

bool loadSceneObject(const String& objectJson, ScenePreset& scene) {
  String textValue;
  int intValue;
  float floatValue;
  bool boolValue;

  scene.id = "";
  scene.name = "";
  scene.category = "Custom";
  scene.mode = "solid";
  scene.red = 255;
  scene.green = 178;
  scene.blue = 109;
  scene.whiteKelvin = 3000;
  scene.usingWhiteTemperature = true;
  scene.masterBrightness = MASTER_BRIGHTNESS;
  scene.strobeDelaySec = 1.0f;
  scene.flashOffDelaySec = 1.0f;
  scene.chasePeriodSec = 5;
  scene.wavePeriodSec = 12;
  scene.slowPulseCount = 5;
  scene.slowPulseMaxSec = 20;
  scene.rainbowPeriodSec = 10;
  scene.candleHallFlickerAmount = 3;
  scene.animationStrength = 128;
  scene.builtin = false;
  scene.isFavoriteCandidate = false;
  scene.isSleepSafe = false;
  scene.isFocusFriendly = false;
  scene.paletteEnabled = false;
  scene.paletteId = "warm-linen";

  if (readJsonString(objectJson, "id", textValue)) {
    scene.id = safeSceneIdToken(textValue);
  }
  if (readJsonString(objectJson, "name", textValue)) {
    scene.name = cleanSceneName(textValue);
  }
  if (readJsonString(objectJson, "category", textValue)) {
    textValue.trim();
    scene.category = textValue.length() > 0 ? textValue : "Custom";
  }
  if (readJsonString(objectJson, "mode", textValue)) {
    Mode parsedMode;
    if (!tryModeFromString(textValue, parsedMode)) {
      return false;
    }
    scene.mode = modeKey(parsedMode);
  }
  if (readJsonInt(objectJson, "red", intValue)) {
    scene.red = constrain(intValue, 0, 255);
  }
  if (readJsonInt(objectJson, "green", intValue)) {
    scene.green = constrain(intValue, 0, 255);
  }
  if (readJsonInt(objectJson, "blue", intValue)) {
    scene.blue = constrain(intValue, 0, 255);
  }
  if (readJsonInt(objectJson, "whiteKelvin", intValue)) {
    scene.whiteKelvin = constrain(intValue, 2700, 6500);
  }
  if (readJsonBool(objectJson, "usingWhiteTemperature", boolValue)) {
    scene.usingWhiteTemperature = boolValue;
  }
  if (readJsonInt(objectJson, "masterBrightness", intValue)) {
    scene.masterBrightness = constrain(intValue, 0, 255);
  }
  if (readJsonFloat(objectJson, "strobeDelaySec", floatValue)) {
    scene.strobeDelaySec = constrain(floatValue, 0.1f, 10.0f);
  }
  if (readJsonFloat(objectJson, "flashOffDelaySec", floatValue)) {
    scene.flashOffDelaySec = constrain(floatValue, 0.1f, 10.0f);
  }
  if (readJsonInt(objectJson, "chasePeriodSec", intValue)) {
    scene.chasePeriodSec = constrain(intValue, 1, 15);
  }
  if (readJsonInt(objectJson, "wavePeriodSec", intValue)) {
    scene.wavePeriodSec = constrain(intValue, 5, 30);
  }
  if (readJsonInt(objectJson, "slowPulseCount", intValue)) {
    scene.slowPulseCount = constrain(intValue, 1, 15);
  }
  if (readJsonInt(objectJson, "slowPulseMaxSec", intValue)) {
    scene.slowPulseMaxSec = constrain(intValue, 1, 60);
  }
  if (readJsonInt(objectJson, "rainbowPeriodSec", intValue)) {
    scene.rainbowPeriodSec = constrain(intValue, 1, 30);
  }
  if (readJsonInt(objectJson, "candleHallFlickerAmount", intValue)) {
    scene.candleHallFlickerAmount = constrain(intValue, 0, 20);
  }
  if (readJsonInt(objectJson, "animationStrength", intValue)) {
    scene.animationStrength = constrain(intValue, 0, 255);
  } else if (readJsonInt(objectJson, "animationIntensity", intValue)) {
    scene.animationStrength = constrain(intValue, 0, 255);
  }
  if (readJsonBool(objectJson, "builtin", boolValue)) {
    scene.builtin = boolValue;
  }
  if (readJsonBool(objectJson, "isFavoriteCandidate", boolValue)) {
    scene.isFavoriteCandidate = boolValue;
  }
  if (readJsonBool(objectJson, "isSleepSafe", boolValue)) {
    scene.isSleepSafe = boolValue;
  }
  if (readJsonBool(objectJson, "isFocusFriendly", boolValue)) {
    scene.isFocusFriendly = boolValue;
  }
  if (readJsonBool(objectJson, "paletteEnabled", boolValue)) {
    scene.paletteEnabled = boolValue;
  }
  if (readJsonString(objectJson, "paletteId", textValue)) {
    scene.paletteId = safePaletteIdToken(textValue);
  }

  return scene.id.length() > 0 && scene.name.length() > 0;
}

void loadScenes() {
  if (!settingsStorageReady) {
    loadDefaultScenes();
    sceneLoadStatus = "LittleFS unavailable; defaults active";
    return;
  }

  if (!LittleFS.exists(SCENES_PATH)) {
    loadDefaultScenes();
    sceneLoadStatus = "missing; defaults active";
    saveScenes();
    return;
  }

  File file = LittleFS.open(SCENES_PATH, "r");
  if (!file) {
    loadDefaultScenes();
    sceneLoadStatus = "open failed; defaults active";
    return;
  }

  String json = file.readString();
  file.close();
  json.trim();
  int scenesStart = json.indexOf("\"scenes\":[");
  if (!json.startsWith("{") || scenesStart < 0) {
    loadDefaultScenes();
    sceneLoadStatus = "corrupt; defaults active";
    return;
  }

  sceneCount = 0;
  int position = json.indexOf('{', scenesStart);
  while (position >= 0 && sceneCount < MAX_SCENES) {
    int depth = 0;
    int end = -1;
    for (int i = position; i < static_cast<int>(json.length()); i++) {
      if (json[i] == '{') {
        depth++;
      } else if (json[i] == '}') {
        depth--;
        if (depth == 0) {
          end = i;
          break;
        }
      }
    }

    if (end < 0) {
      break;
    }

    ScenePreset scene;
    if (loadSceneObject(json.substring(position, end + 1), scene) && findSceneById(scene.id) < 0) {
      addScenePreset(scene);
    }
    position = json.indexOf('{', end + 1);
  }

  if (sceneCount == 0) {
    loadDefaultScenes();
    sceneLoadStatus = "empty; defaults active";
  } else {
    uint8_t addedDefaults = addDefaultBuiltInScenes();
    if (addedDefaults > 0) {
      sceneLoadStatus = "loaded; built-ins merged";
      saveScenes();
    } else {
      sceneLoadStatus = "loaded";
    }
  }
}

void rememberLoadedScene(const ScenePreset& scene) {
  lastLoadedSceneId = scene.id;
  lastLoadedSceneName = scene.name;
  currentStateMatchesLoadedScene = true;
}

void clearLoadedSceneTracking() {
  lastLoadedSceneId = "";
  lastLoadedSceneName = "";
  currentStateMatchesLoadedScene = false;
}

void applyScenePaletteReference(const ScenePreset& scene) {
  if (!scene.paletteEnabled) {
    settings.paletteEnabled = false;
    return;
  }

  if (findPaletteById(scene.paletteId) >= 0) {
    settings.paletteEnabled = true;
    settings.activePaletteId = scene.paletteId;
    return;
  }

  settings.paletteEnabled = false;
  lastActionStatus = "scene palette unavailable";
}

bool applyScene(const ScenePreset& scene) {
  Mode parsedMode;
  if (!tryModeFromString(scene.mode, parsedMode)) {
    return false;
  }
  if (isModeBlockedByNightGuard(scene.mode)) {
    return false;
  }

  diagnosticTest.active = false;
  diagnosticTest.name = "";

  if (scene.usingWhiteTemperature) {
    setWhiteTemperature(scene.whiteKelvin);
  } else {
    setActiveColor(scene.red, scene.green, scene.blue);
  }

  setMasterBrightness(scene.masterBrightness);
  setFloatSetting(settings.strobeDelaySec, scene.strobeDelaySec, 0.1f, 10.0f);
  setFloatSetting(settings.flashOffDelaySec, scene.flashOffDelaySec, 0.1f, 10.0f);
  setByteSetting(settings.chasePeriodSec, scene.chasePeriodSec, 1, 15, false);
  setByteSetting(settings.wavePeriodSec, scene.wavePeriodSec, 5, 30, false);
  setByteSetting(settings.slowPulseCount, scene.slowPulseCount, 1, 15, true);
  setByteSetting(settings.slowPulseMaxSec, scene.slowPulseMaxSec, 1, 60, true);
  setByteSetting(settings.rainbowPeriodSec, scene.rainbowPeriodSec, 1, 30, false);
  setByteSetting(settings.candleHallFlickerAmount, scene.candleHallFlickerAmount, 0, 20, false);
  setByteSetting(settings.animationStrength, scene.animationStrength, 0, 255, false);
  applyScenePaletteReference(scene);
  setModeByName(scene.mode);
  markSettingsDirty();
  ledsDirty = true;
  return true;
}

bool saveCurrentStateAsScene(const String& name, String& outId, String& error) {
  String cleanName = cleanSceneName(name);
  if (cleanName.length() == 0) {
    error = "Scene name is required";
    return false;
  }
  if (sceneNameExists(cleanName, -1)) {
    error = "Duplicate scene name";
    return false;
  }
  if (sceneCount >= MAX_SCENES) {
    error = "Scene limit reached";
    return false;
  }

  outId = makeSceneId(cleanName);
  ScenePreset scene = captureCurrentScene(outId, cleanName, false);
  scenes[sceneCount] = scene;
  sceneCount++;

  if (!saveScenes()) {
    sceneCount--;
    error = "Scene save failed";
    return false;
  }

  return true;
}

String uniqueSceneName(const String& requestedName, int ignoreIndex) {
  String baseName = cleanSceneName(requestedName);
  if (baseName.length() == 0) {
    baseName = "Imported Scene";
  }
  if (!sceneNameExists(baseName, ignoreIndex)) {
    return baseName;
  }

  for (uint8_t suffix = 2; suffix < 100; suffix++) {
    String suffixText = String(" ") + suffix;
    String candidate = baseName;
    while (candidate.length() + suffixText.length() > MAX_SCENE_NAME_LENGTH) {
      candidate.remove(candidate.length() - 1);
    }
    candidate += suffixText;
    if (!sceneNameExists(candidate, ignoreIndex)) {
      return candidate;
    }
  }

  return makeSceneId(baseName);
}

bool duplicateSceneById(const String& id, const String& requestedName, String& outId, String& error) {
  int index = findSceneById(id);
  if (index < 0) {
    error = "Scene not found";
    return false;
  }
  if (sceneCount >= MAX_SCENES) {
    error = "Scene limit reached";
    return false;
  }

  String nextName = requestedName;
  nextName.trim();
  if (nextName.length() == 0) {
    nextName = scenes[index].name + " Copy";
  }
  nextName = uniqueSceneName(nextName, -1);

  ScenePreset scene = scenes[index];
  scene.id = makeSceneId(nextName);
  scene.name = nextName;
  scene.category = "Custom";
  scene.builtin = false;

  scenes[sceneCount] = scene;
  sceneCount++;
  if (!saveScenes()) {
    sceneCount--;
    error = "Scene save failed";
    return false;
  }
  outId = scene.id;
  return true;
}

bool importSceneFromJson(const String& rawJson, String& outId, String& error) {
  if (sceneCount >= MAX_SCENES) {
    error = "Scene limit reached";
    return false;
  }

  String objectJson;
  if (!extractJsonObjectForKey(rawJson, "scene", objectJson) &&
      !extractFirstJsonObject(rawJson, objectJson)) {
    error = "Scene JSON object is required";
    return false;
  }

  ScenePreset scene;
  if (!loadSceneObject(objectJson, scene)) {
    error = "Scene JSON is invalid";
    return false;
  }

  scene.builtin = false;
  scene.category = scene.category.length() > 0 ? scene.category : "Custom";
  scene.name = uniqueSceneName(scene.name, -1);
  scene.id = makeSceneId(scene.name);
  if (scene.paletteEnabled && findPaletteById(scene.paletteId) < 0) {
    scene.paletteEnabled = false;
    scene.paletteId = "warm-linen";
  }

  scenes[sceneCount] = scene;
  sceneCount++;
  if (!saveScenes()) {
    sceneCount--;
    error = "Scene save failed";
    return false;
  }
  outId = scene.id;
  return true;
}

uint8_t restoreBuiltInScenes() {
  uint8_t writeIndex = 0;
  for (uint8_t i = 0; i < sceneCount; i++) {
    if (!scenes[i].builtin) {
      scenes[writeIndex++] = scenes[i];
    }
  }
  sceneCount = writeIndex;
  uint8_t addedDefaults = addDefaultBuiltInScenes();
  saveScenes();
  sceneLoadStatus = "built-ins restored";
  return addedDefaults;
}

bool loadSceneById(const String& id, String& error) {
  int index = findSceneById(id);
  if (index < 0) {
    error = "Scene not found";
    return false;
  }
  if (isModeBlockedByNightGuard(scenes[index].mode)) {
    const ModeMetadata* metadata = findModeMetadataById(scenes[index].mode);
    String label = metadata != nullptr ? String(metadata->displayName) : scenes[index].mode;
    error = String("Night Guard blocks this scene because it uses ") + label;
    return false;
  }
  if (!applyScene(scenes[index])) {
    error = "Scene mode is unavailable";
    return false;
  }
  rememberLoadedScene(scenes[index]);
  beginTransitionToCurrentState();
  return true;
}

bool renameScene(const String& id, const String& name, String& error) {
  int index = findSceneById(id);
  if (index < 0) {
    error = "Scene not found";
    return false;
  }

  String cleanName = cleanSceneName(name);
  if (cleanName.length() == 0) {
    error = "Scene name is required";
    return false;
  }
  if (sceneNameExists(cleanName, index)) {
    error = "Duplicate scene name";
    return false;
  }

  String previousName = scenes[index].name;
  scenes[index].name = cleanName;
  if (!saveScenes()) {
    scenes[index].name = previousName;
    error = "Scene save failed";
    return false;
  }
  if (lastLoadedSceneId == id) {
    lastLoadedSceneName = scenes[index].name;
  }
  return true;
}

bool deleteScene(const String& id, String& error) {
  int index = findSceneById(id);
  if (index < 0) {
    error = "Scene not found";
    return false;
  }
  if (lastLoadedSceneId == id) {
    clearLoadedSceneTracking();
  }

  for (uint8_t i = index; i + 1 < sceneCount; i++) {
    scenes[i] = scenes[i + 1];
  }
  sceneCount--;

  if (!saveScenes()) {
    error = "Scene save failed";
    return false;
  }
  return true;
}

void sendSceneActionOk(const String& message, const String& sceneId) {
  String json;
  json.reserve(140 + sceneId.length() + message.length());
  json += F("{\"ok\":true,\"message\":\"");
  json += escapeJson(message);
  json += F("\",\"sceneId\":\"");
  json += escapeJson(sceneId);
  json += F("\",\"scenes\":");
  json += buildScenesJson();
  json += F("}");
  server.send(200, "application/json", json);
}

void handleApiScenes() {
  streamScenesJson();
}

void handleApiScenesSave() {
  if (!server.hasArg("name")) {
    sendJsonError(400, "Scene name is required");
    return;
  }

  String sceneId;
  String error;
  if (!saveCurrentStateAsScene(server.arg("name"), sceneId, error)) {
    sendJsonError(400, error);
    return;
  }

  sendSceneActionOk("Scene saved", sceneId);
}

void handleApiScenesLoad() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Scene id is required");
    return;
  }

  String error;
  String id = server.arg("id");
  uint8_t beforeBrightness = settings.masterBrightness;
  Mode beforeMode = settings.mode;
  if (!loadSceneById(id, error)) {
    sendJsonError(404, error);
    return;
  }

  recordMutation("/api/scenes/load", id.c_str(), beforeBrightness, beforeMode);
  sendJsonOk("Scene loaded", buildStateJson());
}

void handleApiScenesRename() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Scene id is required");
    return;
  }
  if (!server.hasArg("name")) {
    sendJsonError(400, "Scene name is required");
    return;
  }

  String error;
  String id = server.arg("id");
  if (!renameScene(id, server.arg("name"), error)) {
    sendJsonError(400, error);
    return;
  }

  sendSceneActionOk("Scene renamed", id);
}

void handleApiScenesDelete() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Scene id is required");
    return;
  }

  String error;
  String id = server.arg("id");
  if (!deleteScene(id, error)) {
    sendJsonError(404, error);
    return;
  }

  sendSceneActionOk("Scene deleted", id);
}

void handleApiScenesDuplicate() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Scene id is required");
    return;
  }

  String outId;
  String error;
  String requestedName = server.hasArg("name") ? server.arg("name") : "";
  if (!duplicateSceneById(server.arg("id"), requestedName, outId, error)) {
    sendJsonError(400, error);
    return;
  }

  sendSceneActionOk("Scene duplicated", outId);
}

void handleApiScenesExport() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Scene id is required");
    return;
  }
  int index = findSceneById(server.arg("id"));
  if (index < 0) {
    sendJsonError(404, "Scene not found");
    return;
  }
  server.send(200, "application/json", buildSceneExportJson(scenes[index]));
}

void handleApiScenesImport() {
  if (!server.hasArg("json")) {
    sendJsonError(400, "Scene JSON is required");
    return;
  }
  String outId;
  String error;
  if (!importSceneFromJson(server.arg("json"), outId, error)) {
    sendJsonError(400, error);
    return;
  }
  sendSceneActionOk("Scene imported", outId);
}

void handleApiScenesResetBuiltins() {
  uint8_t restored = restoreBuiltInScenes();
  String message = "Built-in scenes restored";
  if (restored > 0) {
    message += " (";
    message += restored;
    message += " added)";
  }
  sendSceneActionOk(message, "");
}

#endif
