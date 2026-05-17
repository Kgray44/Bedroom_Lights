#ifndef BEDROOM_LED_CONTROLLER_COMFORT_CONTROLS_H
#define BEDROOM_LED_CONTROLLER_COMFORT_CONTROLS_H

String cleanFavoriteLabel(const String& rawLabel) {
  String label = rawLabel;
  label.trim();
  while (label.length() > MAX_FAVORITE_LABEL_LENGTH) {
    label.remove(label.length() - 1);
  }
  return label;
}

String safeFavoriteToken(const String& value, uint8_t maxLength) {
  String token;
  token.reserve(value.length());
  bool previousDash = false;

  for (uint16_t i = 0; i < value.length() && token.length() < maxLength; i++) {
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
  return token;
}

String normalizeActionToken(const String& value) {
  String token;
  token.reserve(value.length());
  for (uint16_t i = 0; i < value.length(); i++) {
    char c = static_cast<char>(tolower(value[i]));
    if (c == ' ' || c == '-' || c == '_') {
      continue;
    }
    token += c;
  }
  return token;
}

String normalizeFavoriteType(const String& value) {
  String type = value;
  type.trim();
  type.toLowerCase();
  if (type == "scene" || type == "action" || type == "mode") {
    return type;
  }
  return "scene";
}

int findFavoriteById(const String& id) {
  for (uint8_t i = 0; i < favoriteCount; i++) {
    if (favorites[i].id == id) {
      return i;
    }
  }
  return -1;
}

bool isSupportedActionName(const String& actionName) {
  String token = normalizeActionToken(actionName);
  return token == "off" ||
         token == "warmdimnow" ||
         token == "panicwarm" ||
         token == "warmnow";
}

bool favoriteTargetAvailable(const FavoriteItem& favorite) {
  if (favorite.type == "scene") {
    return findSceneById(favorite.target) >= 0;
  }
  if (favorite.type == "action") {
    return isSupportedActionName(favorite.target);
  }
  if (favorite.type == "mode") {
    Mode parsedMode;
    return tryModeFromString(favorite.target, parsedMode) &&
           !isModeBlockedByNightGuard(favorite.target);
  }
  return false;
}

bool addFavoriteItem(
  const String& id,
  const String& label,
  const String& type,
  const String& target
) {
  if (favoriteCount >= MAX_FAVORITES) {
    return false;
  }

  FavoriteItem favorite;
  favorite.id = safeFavoriteToken(id, MAX_FAVORITE_ID_LENGTH);
  favorite.label = cleanFavoriteLabel(label);
  favorite.type = normalizeFavoriteType(type);
  favorite.target = safeFavoriteToken(target, MAX_FAVORITE_TARGET_LENGTH);

  if (favorite.id.length() == 0 || favorite.label.length() == 0 || favorite.target.length() == 0) {
    return false;
  }
  if (findFavoriteById(favorite.id) >= 0) {
    return false;
  }

  favorites[favoriteCount] = favorite;
  favoriteCount++;
  return true;
}

bool addDefaultActionFavorite(const String& id, const String& label, const String& actionName) {
  return addFavoriteItem(id, label, "action", actionName);
}

bool addDefaultSceneFavorite(const String& sceneId, const String& label) {
  if (findSceneById(sceneId) < 0) {
    return false;
  }
  String id = "fav-";
  id += sceneId;
  return addFavoriteItem(id, label, "scene", sceneId);
}

void loadDefaultFavorites() {
  favoriteCount = 0;
  addDefaultActionFavorite("fav-off", "Off", "off");
  addDefaultActionFavorite("fav-warm-dim-now", "Warm Dim Now", "warmDimNow");
  addDefaultSceneFavorite("cozy-goblin-mode", "Cozy Goblin");
  addDefaultSceneFavorite("sleep", "Sleep");
  addDefaultSceneFavorite("study-focus", "Study / Focus");
  addDefaultSceneFavorite("movie", "Movie");
  addDefaultSceneFavorite("rain-glass", "Rain Glass");
  addDefaultSceneFavorite("workbench-mode", "Workbench");
  favoriteLoadStatus = "defaults active";
}

String buildSingleFavoriteJson(const FavoriteItem& favorite) {
  String json;
  json.reserve(220);
  json += R"json({"id":")json";
  json += escapeJson(favorite.id);
  json += R"json(","label":")json";
  json += escapeJson(favorite.label);
  json += R"json(","type":")json";
  json += escapeJson(favorite.type);
  json += R"json(","target":")json";
  json += escapeJson(favorite.target);
  json += R"json(","available":)json";
  json += boolJson(favoriteTargetAvailable(favorite));
  json += F("}");
  return json;
}

String buildFavoritesJson() {
  String json;
  json.reserve(340 + favoriteCount * 190);
  json += R"json({"ok":true,"version":1,"count":)json";
  json += favoriteCount;
  json += R"json(,"maxFavorites":)json";
  json += MAX_FAVORITES;
  json += R"json(,"path":")json";
  json += FAVORITES_PATH;
  json += R"json(","loadStatus":")json";
  json += escapeJson(favoriteLoadStatus);
  json += R"json(","saveStatus":")json";
  json += escapeJson(favoriteSaveStatus);
  json += R"json(","favorites":[)json";
  for (uint8_t i = 0; i < favoriteCount; i++) {
    if (i > 0) {
      json += ',';
    }
    json += buildSingleFavoriteJson(favorites[i]);
  }
  json += F("]}");
  return json;
}

bool saveFavorites() {
  if (!settingsStorageReady) {
    favoriteSaveStatus = "skipped; LittleFS unavailable";
    return false;
  }

  File file = LittleFS.open(FAVORITES_PATH, "w");
  if (!file) {
    favoriteSaveStatus = "open failed";
    return false;
  }

  String json = buildFavoritesJson();
  bool saved = file.print(json) == json.length();
  file.close();
  favoriteSaveStatus = saved ? "saved" : "write incomplete";
  return saved;
}

bool loadFavoriteObject(const String& objectJson, FavoriteItem& favorite) {
  String textValue;

  favorite.id = "";
  favorite.label = "";
  favorite.type = "scene";
  favorite.target = "";

  if (readJsonString(objectJson, "id", textValue)) {
    favorite.id = safeFavoriteToken(textValue, MAX_FAVORITE_ID_LENGTH);
  }
  if (readJsonString(objectJson, "label", textValue)) {
    favorite.label = cleanFavoriteLabel(textValue);
  }
  if (readJsonString(objectJson, "type", textValue)) {
    favorite.type = normalizeFavoriteType(textValue);
  }
  if (readJsonString(objectJson, "target", textValue)) {
    favorite.target = safeFavoriteToken(textValue, MAX_FAVORITE_TARGET_LENGTH);
  }

  return favorite.id.length() > 0 &&
         favorite.label.length() > 0 &&
         favorite.target.length() > 0;
}

void loadFavorites() {
  if (!settingsStorageReady) {
    loadDefaultFavorites();
    favoriteLoadStatus = "LittleFS unavailable; defaults active";
    return;
  }

  if (!LittleFS.exists(FAVORITES_PATH)) {
    loadDefaultFavorites();
    favoriteLoadStatus = "missing; defaults active";
    saveFavorites();
    return;
  }

  File file = LittleFS.open(FAVORITES_PATH, "r");
  if (!file) {
    loadDefaultFavorites();
    favoriteLoadStatus = "open failed; defaults active";
    return;
  }

  String json = file.readString();
  file.close();
  json.trim();
  int favoritesStart = json.indexOf("\"favorites\":[");
  if (!json.startsWith("{") || favoritesStart < 0) {
    loadDefaultFavorites();
    favoriteLoadStatus = "corrupt; defaults active";
    return;
  }

  favoriteCount = 0;
  int position = json.indexOf('{', favoritesStart);
  while (position >= 0 && favoriteCount < MAX_FAVORITES) {
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

    FavoriteItem favorite;
    if (loadFavoriteObject(json.substring(position, end + 1), favorite) &&
        findFavoriteById(favorite.id) < 0) {
      favorites[favoriteCount] = favorite;
      favoriteCount++;
    }
    position = json.indexOf('{', end + 1);
  }

  if (favoriteCount == 0) {
    loadDefaultFavorites();
    favoriteLoadStatus = "empty; defaults active";
  } else {
    favoriteLoadStatus = "loaded";
  }
}

uint8_t getEffectiveBrightness() {
  if (transitionState.active) {
    return transitionEffectiveBrightness();
  }
  return configuredEffectiveBrightness();
}

bool isNightGuardBrightnessCapped() {
  return settings.nightGuardEnabled &&
         settings.masterBrightness > settings.nightGuardMaxBrightness;
}

bool isHarshModeName(const String& modeName) {
  return modeIsFlashing(modeName);
}

bool isModeBlockedByNightGuard(const String& modeName) {
  return settings.nightGuardEnabled &&
         settings.nightGuardBlockFlashingModes &&
         modeBlockedByNightGuard(modeName);
}

String nightGuardBlockMessage(const String& modeName) {
  String label = modeName;
  const ModeMetadata* metadata = findModeMetadataById(modeName);
  if (metadata != nullptr) {
    label = metadata->displayName;
  }
  return String("Night Guard blocks ") + label + " while enabled";
}

void forceNightGuardSafeModeIfNeeded() {
  if (isModeBlockedByNightGuard(modeKey(settings.mode))) {
    setMode(MODE_SOLID);
    lastActionStatus = "nightGuardSafeMode";
    markSettingsDirty();
  }
}

bool setNightGuardEnabled(bool enabled) {
  if (settings.nightGuardEnabled == enabled) {
    return false;
  }
  settings.nightGuardEnabled = enabled;
  forceNightGuardSafeModeIfNeeded();
  ledsDirty = true;
  markSettingsDirty();
  return true;
}

bool setNightGuardMaxBrightness(uint8_t value) {
  if (settings.nightGuardMaxBrightness == value) {
    return false;
  }
  settings.nightGuardMaxBrightness = value;
  ledsDirty = true;
  markSettingsDirty();
  return true;
}

bool setNightGuardBlockFlashingModes(bool enabled) {
  if (settings.nightGuardBlockFlashingModes == enabled) {
    return false;
  }
  settings.nightGuardBlockFlashingModes = enabled;
  forceNightGuardSafeModeIfNeeded();
  markSettingsDirty();
  return true;
}

bool setNightGuardPreferWarmTemperature(bool enabled) {
  if (settings.nightGuardPreferWarmTemperature == enabled) {
    return false;
  }
  settings.nightGuardPreferWarmTemperature = enabled;
  markSettingsDirty();
  return true;
}

String buildNightGuardJson() {
  String json;
  json.reserve(220);
  json += F("{\"ok\":true,\"enabled\":");
  json += boolJson(settings.nightGuardEnabled);
  json += F(",\"maxBrightness\":");
  json += settings.nightGuardMaxBrightness;
  json += F(",\"blockFlashing\":");
  json += boolJson(settings.nightGuardBlockFlashingModes);
  json += F(",\"preferWarm\":");
  json += boolJson(settings.nightGuardPreferWarmTemperature);
  json += F(",\"effectiveBrightness\":");
  json += getEffectiveBrightness();
  json += F(",\"brightnessCapped\":");
  json += boolJson(isNightGuardBrightnessCapped());
  json += F("}");
  return json;
}

bool runWarmDimNow() {
  diagnosticTest.active = false;
  diagnosticTest.name = "";
  setModeByName("solid");
  setWhiteTemperature(2700);
  setMasterBrightness(WARM_DIM_BRIGHTNESS);
  lastActionStatus = "warmDimNow";
  markSettingsDirty();
  beginTransitionToCurrentState();
  ledsDirty = true;
  return true;
}

bool runOffAction() {
  diagnosticTest.active = false;
  diagnosticTest.name = "";
  setModeByName("solid");
  setActiveColor(0, 0, 0);
  setMasterBrightness(0);
  lastActionStatus = "off";
  markSettingsDirty();
  beginTransitionToCurrentState();
  ledsDirty = true;
  return true;
}

bool runActionByName(const String& actionName, String& error) {
  String token = normalizeActionToken(actionName);
  if (token == "off") {
    return runOffAction();
  }
  if (token == "warmdimnow" || token == "panicwarm" || token == "warmnow") {
    return runWarmDimNow();
  }

  error = "Unknown action";
  return false;
}

bool runActionByName(const String& actionName) {
  String error;
  return runActionByName(actionName, error);
}

bool loadFavoriteById(const String& id, String& error) {
  int index = findFavoriteById(id);
  if (index < 0) {
    error = "Favorite not found";
    return false;
  }

  FavoriteItem favorite = favorites[index];
  if (favorite.type == "scene") {
    if (findSceneById(favorite.target) < 0) {
      error = "Favorite target scene not found";
      return false;
    }
    return loadSceneById(favorite.target, error);
  }
  if (favorite.type == "action") {
    return runActionByName(favorite.target, error);
  }
  if (favorite.type == "mode") {
    if (isModeBlockedByNightGuard(favorite.target)) {
      error = nightGuardBlockMessage(favorite.target);
      return false;
    }
    if (!setModeByName(favorite.target, error)) {
      if (error.length() == 0) {
        error = "Favorite mode is unavailable";
      }
      return false;
    }
    beginTransitionToCurrentState();
    return true;
  }

  error = "Favorite type is unavailable";
  return false;
}

void sendFavoriteActionOk(const String& message) {
  String json;
  json.reserve(260);
  json += F("{\"ok\":true,\"message\":\"");
  json += escapeJson(message);
  json += F("\",\"favorites\":");
  json += buildFavoritesJson();
  json += F(",\"state\":");
  json += buildStateJson();
  json += F("}");
  server.send(200, "application/json", json);
}

void handleApiFavorites() {
  server.send(200, "application/json", buildFavoritesJson());
}

void handleApiFavoritesLoad() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Favorite id is required");
    return;
  }

  String error;
  if (!loadFavoriteById(server.arg("id"), error)) {
    sendJsonError(404, error);
    return;
  }

  sendFavoriteActionOk("Favorite loaded");
}

void handleApiFavoritesReset() {
  loadDefaultFavorites();
  saveFavorites();
  sendFavoriteActionOk("Favorites reset");
}

void handleApiAction() {
  if (!server.hasArg("name")) {
    sendJsonError(400, "Action name is required");
    return;
  }

  String error;
  if (!runActionByName(server.arg("name"), error)) {
    sendJsonError(400, error);
    return;
  }

  sendJsonOk("Action completed", buildStateJson());
}

void handleApiPanicWarm() {
  String error;
  if (!runActionByName("panicWarm", error)) {
    sendJsonError(400, error);
    return;
  }

  sendJsonOk("Warm Dim Now", buildStateJson());
}

void handleApiNightGuard() {
  server.send(200, "application/json", buildNightGuardJson());
}

void handleApiNightGuardSet() {
  if (server.hasArg("enabled")) {
    bool enabled;
    if (!parseRequestBool(server.arg("enabled"), enabled)) {
      sendJsonError(400, "Invalid Night Guard enabled value");
      return;
    }
    setNightGuardEnabled(enabled);
  }

  if (server.hasArg("maxBrightness")) {
    int maxBrightness;
    if (!parseIntArg("maxBrightness", maxBrightness)) {
      sendJsonError(400, "Invalid Night Guard brightness");
      return;
    }
    setNightGuardMaxBrightness(static_cast<uint8_t>(constrain(maxBrightness, 0, 255)));
  }

  if (server.hasArg("blockFlashing")) {
    bool blockFlashing;
    if (!parseRequestBool(server.arg("blockFlashing"), blockFlashing)) {
      sendJsonError(400, "Invalid Night Guard flashing value");
      return;
    }
    setNightGuardBlockFlashingModes(blockFlashing);
  }

  if (server.hasArg("preferWarm")) {
    bool preferWarm;
    if (!parseRequestBool(server.arg("preferWarm"), preferWarm)) {
      sendJsonError(400, "Invalid Night Guard warm preference");
      return;
    }
    setNightGuardPreferWarmTemperature(preferWarm);
  }

  String json;
  json.reserve(420);
  json += F("{\"ok\":true,\"message\":\"Night Guard updated\",\"nightGuard\":");
  json += buildNightGuardJson();
  json += F(",\"state\":");
  json += buildStateJson();
  json += F("}");
  server.send(200, "application/json", json);
}

#endif
