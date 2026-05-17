#ifndef BEDROOM_LED_CONTROLLER_SURPRISE_CONTROLS_H
#define BEDROOM_LED_CONTROLLER_SURPRISE_CONTROLS_H

String normalizeSurpriseMood(String mood) {
  mood.trim();
  mood.toLowerCase();
  mood.replace("-", "");
  mood.replace("_", "");
  mood.replace(" ", "");
  if (mood == "sleep" || mood == "sleepsafe") return "sleepSafe";
  if (mood == "night") return "sleepSafe";
  if (mood == "colourful") return "colorful";
  if (mood == "weather" || mood == "rain" || mood == "storm") return "weather";
  if (mood == "nature" || mood == "natural" || mood == "garden") return "nature";
  if (mood == "evening") return "evening";
  if (mood == "focus") return "focus";
  if (mood == "colorful") return "colorful";
  if (mood == "cozy") return "cozy";
  if (mood == "calm" || mood.length() == 0) return "calm";
  return mood;
}

bool isSupportedSurpriseMood(const String& mood) {
  return mood == "calm" ||
         mood == "evening" ||
         mood == "sleepSafe" ||
         mood == "focus" ||
         mood == "colorful" ||
         mood == "cozy" ||
         mood == "weather" ||
         mood == "nature";
}

String surpriseMoodLabel(const String& mood) {
  if (mood == "sleepSafe") return "Sleep-safe";
  if (mood == "evening") return "Evening";
  if (mood == "focus") return "Focus";
  if (mood == "colorful") return "Colorful";
  if (mood == "cozy") return "Cozy";
  if (mood == "weather") return "Weather";
  if (mood == "nature") return "Nature";
  return "Calm";
}

String buildSurpriseOptionsJson() {
  String json;
  json.reserve(520);
  json += R"json({"ok":true,"moods":[)json";
  json += R"json({"id":"calm","label":"Calm"},{"id":"evening","label":"Evening"},{"id":"sleepSafe","label":"Sleep-safe"},{"id":"focus","label":"Focus"},{"id":"colorful","label":"Colorful"},{"id":"cozy","label":"Cozy"},{"id":"weather","label":"Weather"},{"id":"nature","label":"Nature"}])json";
  json += R"json(,"rules":["never strobe","never flash","exclude flashing metadata","cap by Night Guard","cap by configured power estimate","use existing modes only","use valid palettes only"]})json";
  return json;
}

uint8_t capSurpriseBrightness(uint8_t requested, bool& powerCapped, bool& nightGuardCapped) {
  uint8_t capped = requested;
  powerCapped = false;
  nightGuardCapped = false;

  PowerEstimate estimate = buildPowerEstimate();
  if (estimate.recommendedBrightnessCap > 0 && capped > estimate.recommendedBrightnessCap) {
    capped = estimate.recommendedBrightnessCap;
    powerCapped = true;
  }
  if (settings.nightGuardEnabled && capped > settings.nightGuardMaxBrightness) {
    capped = settings.nightGuardMaxBrightness;
    nightGuardCapped = true;
  }
  return capped;
}

uint8_t surpriseBrightnessForMood(const String& mood, bool& powerCapped, bool& nightGuardCapped) {
  uint8_t minimum = 50;
  uint8_t maximum = 140;

  if (mood == "sleepSafe") {
    minimum = 10;
    maximum = 45;
  } else if (mood == "cozy") {
    minimum = 25;
    maximum = 80;
  } else if (mood == "evening") {
    minimum = 40;
    maximum = 110;
  } else if (mood == "focus") {
    minimum = 120;
    maximum = 190;
  } else if (mood == "colorful") {
    minimum = 60;
    maximum = 150;
  } else if (mood == "weather") {
    minimum = 35;
    maximum = 115;
  } else if (mood == "nature") {
    minimum = 70;
    maximum = 150;
  }

  uint8_t requested = static_cast<uint8_t>(random(minimum, maximum + 1));
  return capSurpriseBrightness(requested, powerCapped, nightGuardCapped);
}

Mode pickModeFromList(const Mode* modes, uint8_t count) {
  for (uint8_t attempts = 0; attempts < 12; attempts++) {
    Mode candidate = modes[random(0, count)];
    String key = modeKey(candidate);
    if (candidate == MODE_STROBE || candidate == MODE_FLASH) {
      continue;
    }
    if (modeIsFlashing(key)) {
      continue;
    }
    if (settings.nightGuardEnabled && modeBlockedByNightGuard(key)) {
      continue;
    }
    return candidate;
  }
  return MODE_SOLID;
}

Mode selectSurpriseMode(const String& mood) {
  const Mode calmModes[] = {MODE_GARDEN_SHADE, MODE_SNOWFALL_HUSH, MODE_OBSERVATORY_GLOW, MODE_RAINY_WINDOW, MODE_CAMPFIRE_BLANKET, MODE_NORTHERN_WHISPER, MODE_PORCELAIN_SKY, MODE_EVENING_CURRENT, MODE_DEEP_SEA_GLASS, MODE_EMBER_QUILT, MODE_WINDOW_SUNRISE, MODE_SATIN_BREATHING, MODE_PAPER_LANTERN_ROW, MODE_GOLDEN_HOUR, MODE_DREAM_AQUARIUM, MODE_MOON_TIDE, MODE_RAIN_GLASS, MODE_LANTERN_DRIFT, MODE_SLOW_WAVE, MODE_SLOW_PULSE, MODE_VELVET_AURORA};
  const Mode eveningModes[] = {MODE_CANDLE_HALL, MODE_CAMPFIRE_BLANKET, MODE_RAINY_WINDOW, MODE_OBSERVATORY_GLOW, MODE_STORM_OUTSIDE, MODE_HEARTHLINE, MODE_EVENING_CURRENT, MODE_EMBER_QUILT, MODE_WINDOW_SUNRISE, MODE_QUIET_NORTHERN_LIGHTS, MODE_PAPER_LANTERN_ROW, MODE_GOLDEN_HOUR, MODE_SATIN_BREATHING, MODE_DREAM_AQUARIUM, MODE_LANTERN_DRIFT, MODE_MOON_TIDE, MODE_DAWN_BLOOM, MODE_SLOW_PULSE};
  const Mode sleepModes[] = {MODE_CANDLE_HALL, MODE_SNOWFALL_HUSH, MODE_OBSERVATORY_GLOW, MODE_CAMPFIRE_BLANKET, MODE_NORTHERN_WHISPER, MODE_RAINY_WINDOW, MODE_EMBER_QUILT, MODE_STARLIT_LINEN, MODE_EVENING_CURRENT, MODE_DEEP_SEA_GLASS, MODE_SATIN_BREATHING, MODE_PAPER_LANTERN_ROW, MODE_GOLDEN_HOUR, MODE_SOLID, MODE_LANTERN_DRIFT, MODE_MOON_TIDE, MODE_RAIN_GLASS};
  const Mode focusModes[] = {MODE_GARDEN_SHADE, MODE_SLOW_PRISM, MODE_PORCELAIN_SKY, MODE_WINDOW_SUNRISE, MODE_LIBRARY_LAMP, MODE_SOLAR_NOON, MODE_SOLID, MODE_DAWN_BLOOM, MODE_WAVE};
  const Mode colorfulModes[] = {MODE_SLOW_PRISM, MODE_NORTHERN_WHISPER, MODE_OBSERVATORY_GLOW, MODE_QUIET_NORTHERN_LIGHTS, MODE_DEEP_SEA_GLASS, MODE_DREAM_AQUARIUM, MODE_CIRCUIT_GLOW, MODE_VELVET_AURORA, MODE_RAINBOW, MODE_SLOW_WAVE, MODE_RAIN_GLASS};
  const Mode cozyModes[] = {MODE_CANDLE_HALL, MODE_CAMPFIRE_BLANKET, MODE_RAINY_WINDOW, MODE_SNOWFALL_HUSH, MODE_EMBER_QUILT, MODE_HEARTHLINE, MODE_WINDOW_SUNRISE, MODE_EVENING_CURRENT, MODE_PAPER_LANTERN_ROW, MODE_GOLDEN_HOUR, MODE_SATIN_BREATHING, MODE_LANTERN_DRIFT, MODE_SLOW_PULSE, MODE_SOLID, MODE_MOON_TIDE};
  const Mode weatherModes[] = {MODE_GARDEN_SHADE, MODE_SNOWFALL_HUSH, MODE_RAINY_WINDOW, MODE_STORM_OUTSIDE, MODE_DEEP_SEA_GLASS, MODE_RAIN_GLASS, MODE_DREAM_AQUARIUM};
  const Mode natureModes[] = {MODE_GARDEN_SHADE, MODE_SNOWFALL_HUSH, MODE_RAINY_WINDOW, MODE_CAMPFIRE_BLANKET, MODE_WINDOW_SUNRISE, MODE_GOLDEN_HOUR};

  if (mood == "evening") return pickModeFromList(eveningModes, sizeof(eveningModes) / sizeof(eveningModes[0]));
  if (mood == "sleepSafe") return pickModeFromList(sleepModes, sizeof(sleepModes) / sizeof(sleepModes[0]));
  if (mood == "focus") return pickModeFromList(focusModes, sizeof(focusModes) / sizeof(focusModes[0]));
  if (mood == "colorful") return pickModeFromList(colorfulModes, sizeof(colorfulModes) / sizeof(colorfulModes[0]));
  if (mood == "cozy") return pickModeFromList(cozyModes, sizeof(cozyModes) / sizeof(cozyModes[0]));
  if (mood == "weather") return pickModeFromList(weatherModes, sizeof(weatherModes) / sizeof(weatherModes[0]));
  if (mood == "nature") return pickModeFromList(natureModes, sizeof(natureModes) / sizeof(natureModes[0]));
  return pickModeFromList(calmModes, sizeof(calmModes) / sizeof(calmModes[0]));
}

bool paletteMatchesMood(const ColorPalette& palette, const String& mood) {
  if (mood == "sleepSafe") return palette.sleepSafe || (palette.warm && palette.calm);
  if (mood == "cozy" || mood == "evening") return palette.warm || palette.sleepSafe;
  if (mood == "colorful") return palette.colorful;
  if (mood == "focus") return !palette.colorful && (palette.calm || palette.cool || palette.warm);
  if (mood == "weather") return palette.cool || palette.category == "Weather" || palette.category == "Night";
  if (mood == "nature") return palette.category == "Natural" || (palette.calm && !palette.colorful);
  return palette.calm;
}

String selectSurprisePaletteId(const String& mood) {
  uint8_t matches[MAX_PALETTES];
  uint8_t matchCount = 0;
  for (uint8_t i = 0; i < paletteCount; i++) {
    if (paletteMatchesMood(palettes[i], mood)) {
      matches[matchCount] = i;
      matchCount++;
    }
  }
  if (matchCount == 0) {
    return "warm-linen";
  }
  return palettes[matches[random(0, matchCount)]].id;
}

Rgb surpriseColorForMood(const String& mood) {
  if (mood == "sleepSafe") return {255, 138, 64};
  if (mood == "cozy") return {255, 156, 88};
  if (mood == "evening") return {255, 176, 112};
  if (mood == "focus") return {245, 250, 255};
  if (mood == "colorful") return {120, 190, 255};
  if (mood == "weather") return {88, 142, 184};
  if (mood == "nature") return {168, 190, 106};
  return {160, 205, 255};
}

bool surpriseUsesWhiteTemperature(const String& mood, Mode mode) {
  return mood == "focus" || mode == MODE_LIBRARY_LAMP || mode == MODE_SOLAR_NOON || mode == MODE_PORCELAIN_SKY || (mood == "sleepSafe" && mode == MODE_SOLID);
}

uint16_t surpriseKelvinForMood(const String& mood) {
  if (mood == "focus") return static_cast<uint16_t>(random(4200, 5201));
  if (mood == "sleepSafe") return 2700;
  if (mood == "cozy" || mood == "evening") return 3000;
  return 3200;
}

void applySurpriseModeDefaults(Mode mode, const String& mood) {
  if (mode == MODE_SLOW_PULSE) {
    settings.slowPulseCount = mood == "sleepSafe" ? 3 : 5;
    settings.slowPulseMaxSec = mood == "colorful" ? 18 : 28;
  } else if (mode == MODE_SLOW_WAVE || mode == MODE_WAVE) {
    settings.wavePeriodSec = mood == "colorful" ? 12 : 18;
  } else if (mode == MODE_RAINBOW) {
    settings.rainbowPeriodSec = 18;
  }
}

bool generateSurpriseCandidate(const String& rawMood, SurpriseCandidate& candidate, String& error) {
  String mood = normalizeSurpriseMood(rawMood);
  if (!isSupportedSurpriseMood(mood)) {
    error = "Unknown surprise mood";
    return false;
  }

  candidate.mood = mood;
  candidate.mode = selectSurpriseMode(mood);
  String modeName = modeKey(candidate.mode);
  if (candidate.mode == MODE_STROBE || candidate.mode == MODE_FLASH ||
      modeIsFlashing(modeName) ||
      (settings.nightGuardEnabled && modeBlockedByNightGuard(modeName))) {
    error = "No safe Surprise Me candidate exists for this mood";
    return false;
  }

  candidate.usingWhiteTemperature = surpriseUsesWhiteTemperature(mood, candidate.mode);
  candidate.whiteKelvin = surpriseKelvinForMood(mood);
  candidate.color = candidate.usingWhiteTemperature ? kelvinToRgb(candidate.whiteKelvin) : surpriseColorForMood(mood);
  candidate.brightness = surpriseBrightnessForMood(mood, candidate.powerCapped, candidate.nightGuardCapped);
  candidate.paletteEnabled = modeSupportsPalette(candidate.mode) && mood != "focus";
  candidate.paletteId = candidate.paletteEnabled ? selectSurprisePaletteId(mood) : settings.activePaletteId;
  if (candidate.paletteEnabled && findPaletteById(candidate.paletteId) < 0) {
    candidate.paletteEnabled = false;
    candidate.paletteId = settings.activePaletteId;
  }
  return true;
}

String buildSurpriseCandidateJson(const SurpriseCandidate& candidate, bool applied) {
  String json;
  json.reserve(520);
  json += R"json({"ok":true,"applied":)json";
  json += boolJson(applied);
  json += R"json(,"mood":")json";
  json += escapeJson(candidate.mood);
  json += R"json(","moodLabel":")json";
  json += escapeJson(surpriseMoodLabel(candidate.mood));
  json += R"json(","mode":")json";
  json += modeKey(candidate.mode);
  json += R"json(","modeDisplayName":")json";
  json += escapeJson(modeDisplayName(candidate.mode));
  json += R"json(","paletteEnabled":)json";
  json += boolJson(candidate.paletteEnabled);
  json += R"json(,"paletteId":")json";
  json += escapeJson(candidate.paletteId);
  json += R"json(","brightness":)json";
  json += candidate.brightness;
  json += R"json(,"usingWhiteTemperature":)json";
  json += boolJson(candidate.usingWhiteTemperature);
  json += R"json(,"whiteKelvin":)json";
  json += candidate.whiteKelvin;
  json += R"json(,"hex":"#)json";
  char hex[7];
  snprintf(hex, sizeof(hex), "%02X%02X%02X", candidate.color.r, candidate.color.g, candidate.color.b);
  json += hex;
  json += R"json(","powerCapped":)json";
  json += boolJson(candidate.powerCapped);
  json += R"json(,"nightGuardCapped":)json";
  json += boolJson(candidate.nightGuardCapped);
  json += R"json(,"message":"Generated )json";
  json += escapeJson(candidate.mood);
  json += R"json( lighting state"})json";
  return json;
}

bool applySurpriseCandidate(const SurpriseCandidate& candidate, String& error) {
  String modeName = modeKey(candidate.mode);
  if (candidate.mode == MODE_STROBE || candidate.mode == MODE_FLASH ||
      modeIsFlashing(modeName) ||
      (settings.nightGuardEnabled && modeBlockedByNightGuard(modeName))) {
    error = "Unsafe Surprise Me candidate rejected";
    return false;
  }

  diagnosticTest.active = false;
  diagnosticTest.name = "";
  if (activeTimer.active) {
    clearActiveTimer();
    lastTimerStatus = "timer cancelled by Surprise Me";
  }

  if (candidate.usingWhiteTemperature) {
    setWhiteTemperature(candidate.whiteKelvin);
  } else {
    setActiveColor(candidate.color.r, candidate.color.g, candidate.color.b);
  }
  setMasterBrightness(candidate.brightness);
  settings.paletteEnabled = candidate.paletteEnabled;
  settings.activePaletteId = candidate.paletteId;
  applySurpriseModeDefaults(candidate.mode, candidate.mood);
  if (!setModeByName(modeName, error)) {
    return false;
  }

  lastSurpriseMood = candidate.mood;
  lastSurpriseSummary = String(surpriseMoodLabel(candidate.mood)) + " - " + modeDisplayName(candidate.mode);
  if (candidate.paletteEnabled) {
    lastSurpriseSummary += " - ";
    lastSurpriseSummary += activePaletteName();
  }
  lastSurpriseSummary += " - ";
  lastSurpriseSummary += candidate.brightness;
  lastSurpriseSummary += "/255";
  lastSurpriseStatus = candidate.powerCapped || candidate.nightGuardCapped
    ? "applied with brightness cap"
    : "applied";
  lastActionStatus = "surprise";

  clearLoadedSceneTracking();
  markSettingsDirty();
  ledsDirty = true;
  beginTransitionToCurrentState();
  return true;
}

void handleApiSurprise() {
  if (!server.hasArg("mood")) {
    sendJsonError(400, "Surprise mood is required");
    return;
  }

  String mood = normalizeSurpriseMood(server.arg("mood"));
  if (!isSupportedSurpriseMood(mood)) {
    sendJsonError(400, "Unknown surprise mood");
    return;
  }

  bool applyNow = true;
  if (server.hasArg("apply")) {
    if (!parseRequestBool(server.arg("apply"), applyNow)) {
      sendJsonError(400, "Invalid Surprise Me apply value");
      return;
    }
  }

  SurpriseCandidate candidate;
  String error;
  if (!generateSurpriseCandidate(mood, candidate, error)) {
    sendJsonError(400, error);
    return;
  }

  if (applyNow && !applySurpriseCandidate(candidate, error)) {
    sendJsonError(400, error);
    return;
  }

  String json = buildSurpriseCandidateJson(candidate, applyNow);
  if (applyNow) {
    json.remove(json.length() - 1);
    json += R"json(,"state":)json";
    json += buildStateJson();
    json += '}';
  }
  server.send(200, "application/json", json);
}

void handleApiSurpriseOptions() {
  server.send(200, "application/json", buildSurpriseOptionsJson());
}

#endif
