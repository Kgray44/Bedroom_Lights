#ifndef BEDROOM_LED_CONTROLLER_MODE_METADATA_H
#define BEDROOM_LED_CONTROLLER_MODE_METADATA_H

struct ModeMetadata {
  Mode mode;
  const char* id;
  const char* displayName;
  const char* description;
  const char* bestUsedFor;
  const char* category;
  bool animated;
  bool calm;
  bool sleepSafe;
  bool focusFriendly;
  bool colorful;
  bool flashing;
  bool utility;
  bool experimental;
  bool blockedByNightGuard;
  bool supportsPalette;
  const char* preferredPaletteRole;
};

const ModeMetadata MODE_METADATA[] = {
  {MODE_SOLID, "solid", "Solid", "A steady color or white temperature.", "Everyday color, direct white temperature, and saved practical scenes.", "Everyday", false, true, true, true, false, false, true, false, false, false, "none"},
  {MODE_STROBE, "strobe", "Strobe", "High-contrast on/off flashes.", "Short diagnostic or attention moments when Night Guard is off.", "Alert", true, false, false, false, false, true, true, false, true, false, "none"},
  {MODE_FLASH, "flash", "Flash", "A repeating flash with configurable off time.", "Utility signaling when Night Guard is off.", "Alert", true, false, false, false, false, true, true, false, true, false, "none"},
  {MODE_CHASE, "chase", "Chase", "A moving point of the active color across the strip.", "Playful motion or quick visual confirmation.", "Motion", true, false, false, false, false, false, false, false, false, false, "none"},
  {MODE_WAVE, "wave", "Wave", "A smooth moving brightness wave using the active color.", "Animated ambient color without harsh flashing.", "Motion", true, true, false, false, false, false, false, false, false, false, "none"},
  {MODE_SLOW_WAVE, "slowWave", "Slow Wave", "A slower, softer version of the wave motion.", "Calm background motion at low brightness.", "Calm", true, true, true, false, false, false, false, false, false, true, "gradient"},
  {MODE_SLOW_PULSE, "slowPulse", "Slow Pulse", "Soft pulses that bloom and fade over time.", "Low-energy calm lighting and wind-down scenes.", "Calm", true, true, true, false, false, false, false, false, false, true, "ambient"},
  {MODE_RAINBOW, "rainbow", "Rainbow", "A color-cycling rainbow animation.", "Colorful showcase moments.", "Colorful", true, false, false, false, true, false, false, false, false, false, "none"},
  {MODE_DAWN_BLOOM, "dawnBloom", "Dawn Bloom", "Warm light slowly blooms brighter like sunrise.", "Gentle morning or soft wake lighting.", "Calm", true, true, true, true, false, false, false, false, false, false, "none"},
  {MODE_MOON_TIDE, "moonTide", "Moon Tide", "Cool blue movement with a soft tide rhythm.", "Movie, low-brightness ambiance, and night-safe calm.", "Calm", true, true, true, false, true, false, false, false, false, true, "ambient"},
  {MODE_VELVET_AURORA, "velvetAurora", "Velvet Aurora", "Layered color motion with a richer aurora feel.", "Colorful ambient background without flashing.", "Colorful", true, true, false, false, true, false, false, true, false, true, "ribbon"},
  {MODE_LANTERN_DRIFT, "lanternDrift", "Lantern Drift", "Warm drifting amber light with a lantern feel.", "Cozy evening, reading corners, and relaxed calls.", "Calm", true, true, true, true, false, false, false, false, false, true, "ambient"},
  {MODE_RAIN_GLASS, "rainGlass", "Rain Glass", "Cool glints that feel like rain on glass.", "Weather mood, relaxing background light, and low-focus ambiance.", "Weather", true, true, true, false, true, false, false, false, false, true, "sparkle"},
  {MODE_SATIN_BREATHING, "satinBreathing", "Satin Breathing", "A slow whole-strip breathing mode with soft fabric-like texture.", "Bedtime, calm evenings, and low-stimulation ambience.", "Sleep", true, true, true, false, false, false, false, false, false, true, "ambient"},
  {MODE_PAPER_LANTERN_ROW, "paperLanternRow", "Paper Lantern Row", "Large warm lantern blobs with soft edges and subtle independent drift.", "Cozy evenings, relaxed calls, and decorative warm background light.", "Calm", true, true, true, false, false, false, false, false, false, true, "warm"},
  {MODE_LIBRARY_LAMP, "libraryLamp", "Library Lamp", "Stable warm reading light with very faint texture.", "Reading, homework, desk work, and calm focus.", "Everyday", true, true, true, true, false, false, false, false, false, true, "warm"},
  {MODE_SOLAR_NOON, "solarNoon", "Solar Noon", "Stable neutral daylight with a very slow refined gradient.", "Daytime focus, cleaning, electronics work, and practical bright ambience.", "Focus", true, false, false, true, false, false, false, false, false, true, "neutral"},
  {MODE_GOLDEN_HOUR, "goldenHour", "Golden Hour", "Warm amber light that spills across the strip like low sun.", "Morning starts, evening wind-down, and gentle everyday warmth.", "Calm", true, true, true, false, false, false, false, false, false, true, "warm"},
  {MODE_DREAM_AQUARIUM, "dreamAquarium", "Dream Aquarium", "Cool watery motion with soft drifting shimmer.", "Relaxed ambient color, water mood, and calm background movement.", "Weather", true, true, false, false, true, false, false, false, false, true, "water"},
  {MODE_CIRCUIT_GLOW, "circuitGlow", "Circuit Glow", "Soft trace-like pulses moving through a cool low base.", "Experimental ambient motion and colorful tech mood without flashing.", "Experimental", true, false, false, false, true, false, false, true, false, true, "trace"},
  {MODE_WINDOW_SUNRISE, "windowSunrise", "Window Sunrise", "Directional warm morning light spreading across the strip like sunlight from one side.", "Gentle mornings, warm wake scenes, and everyday sunrise ambience.", "Morning", true, true, true, true, false, false, false, false, false, true, "warm"},
  {MODE_EMBER_QUILT, "emberQuilt", "Ember Quilt", "Low-contrast warm patches that breathe under a soft fabric-like texture.", "Cozy evenings, bedtime wind-down, and low-stress warm ambience.", "Sleep", true, true, true, false, false, false, false, false, false, true, "warm"},
  {MODE_PORCELAIN_SKY, "porcelainSky", "Porcelain Sky", "Pale refined cloud-glass gradients with very slow daytime movement.", "Daytime calm, focus, and subtle polished background light.", "Everyday", true, true, true, true, false, false, false, false, false, true, "neutral"},
  {MODE_HEARTHLINE, "hearthline", "Hearthline", "Restrained amber bands over a stable warm base for distant hearth warmth.", "Movie nights, quiet evening rooms, and warm background scenes.", "Evening", true, true, true, false, false, false, false, false, false, true, "warm"},
  {MODE_EVENING_CURRENT, "eveningCurrent", "Evening Current", "Long overlapping currents of coherent light with no harsh valleys.", "Calm evening motion, river-like ambience, and low-brightness scenes.", "Evening", true, true, true, false, false, false, false, false, false, true, "ambient"},
  {MODE_STARLIT_LINEN, "starlitLinen", "Starlit Linen", "Soft sparse highlights blooming gently over a steady low base.", "Night mode, sleep-safe ambience, and tiny low-brightness accents.", "Sleep", true, true, true, false, false, false, false, false, false, true, "ambient"},
  {MODE_DEEP_SEA_GLASS, "deepSeaGlass", "Deep Sea Glass", "Cool layered glassy water motion with soft depth and shimmer.", "Water mood, quiet focus, and deep calm background motion.", "Weather", true, true, true, false, true, false, false, false, false, true, "water"},
  {MODE_QUIET_NORTHERN_LIGHTS, "quietNorthernLights", "Quiet Northern Lights", "Slow soft aurora ribbons that fold through calm color without flashing.", "Colorful calm ambience, evening scenes, and gentle aurora color.", "Colorful", true, true, true, false, true, false, false, false, false, true, "ribbon"},
  {MODE_CANDLE_HALL, "candleHall", "Candle Hall", "Soft candle clusters arranged like distant warm hallway light.", "Cozy evenings, sleep-safe warm scenes, and restrained candle ambience.", "Evening", true, true, true, false, false, false, false, false, false, true, "warm"},
  {MODE_SLOW_PRISM, "slowPrism", "Slow Prism", "A slow tasteful companion-color gradient built from the active color or palette.", "Colorful calm scenes, low-contrast focus color, and palette showcase ambience.", "Colorful", true, true, true, true, true, false, false, false, false, true, "prism"},
  {MODE_GARDEN_SHADE, "gardenShade", "Garden Shade", "Muted green-gold bands drifting over warm daylight like filtered leaf shade.", "Daytime calm, nature mood, and focus-friendly background light.", "Nature", true, true, false, true, false, false, false, false, false, true, "natural"},
  {MODE_SNOWFALL_HUSH, "snowfallHush", "Snowfall Hush", "Quiet pale drifting points over a cool or warm-dust background.", "Sleep-safe winter calm, low-brightness weather scenes, and muted night ambience.", "Sleep", true, true, true, false, false, false, false, false, false, true, "cool"},
  {MODE_OBSERVATORY_GLOW, "observatoryGlow", "Observatory Glow", "A very slow moonlike glow with subtle fixed star gradients.", "Night ambience, celestial sleep-safe scenes, and calm low motion.", "Sleep", true, true, true, false, true, false, false, false, false, true, "celestial"},
  {MODE_RAINY_WINDOW, "rainyWindow", "Rainy Window", "Cool soft window light with tiny long-fading droplet trails.", "Weather mood, rainy evening ambience, and gentle sleep-safe background motion.", "Weather", true, true, true, false, true, false, false, false, false, true, "weather"},
  {MODE_CAMPFIRE_BLANKET, "campfireBlanket", "Campfire Blanket", "Restrained warm fire movement softened into an even cozy blanket.", "Cozy evenings, bedtime warmth, and movie-night background scenes.", "Evening", true, true, true, false, false, false, false, false, false, true, "warm"},
  {MODE_NORTHERN_WHISPER, "northernWhisper", "Northern Whisper", "Barely moving aurora color ribbons with capped brightness and soft folds.", "Sleep-safe colorful ambience, calm aurora scenes, and low-brightness color.", "Colorful", true, true, true, false, true, false, false, false, false, true, "ribbon"},
  {MODE_STORM_OUTSIDE, "stormOutside", "Storm Outside", "Low storm-blue ambience with rare softened lightning pulses.", "Moody weather scenes and evening ambience without hard flashes.", "Weather", true, true, false, false, true, false, false, false, false, true, "storm"}
};

const uint8_t MODE_METADATA_COUNT = sizeof(MODE_METADATA) / sizeof(MODE_METADATA[0]);

const ModeMetadata* findModeMetadata(Mode mode) {
  for (uint8_t i = 0; i < MODE_METADATA_COUNT; i++) {
    if (MODE_METADATA[i].mode == mode) {
      return &MODE_METADATA[i];
    }
  }
  return nullptr;
}

const ModeMetadata* findModeMetadataById(const String& id) {
  for (uint8_t i = 0; i < MODE_METADATA_COUNT; i++) {
    if (id == MODE_METADATA[i].id) {
      return &MODE_METADATA[i];
    }
  }

  Mode parsedMode;
  if (tryModeFromString(id, parsedMode)) {
    return findModeMetadata(parsedMode);
  }
  return nullptr;
}

String modeDisplayName(Mode mode) {
  const ModeMetadata* metadata = findModeMetadata(mode);
  return metadata != nullptr ? String(metadata->displayName) : modeKey(mode);
}

String modeCategory(Mode mode) {
  const ModeMetadata* metadata = findModeMetadata(mode);
  return metadata != nullptr ? String(metadata->category) : String("Unknown");
}

bool modeSupportsPalette(Mode mode) {
  const ModeMetadata* metadata = findModeMetadata(mode);
  return metadata != nullptr && metadata->supportsPalette;
}

bool currentModeSupportsPalette() {
  return modeSupportsPalette(settings.mode);
}

String currentModePaletteRole() {
  const ModeMetadata* metadata = findModeMetadata(settings.mode);
  return metadata != nullptr ? String(metadata->preferredPaletteRole) : String("none");
}

void appendModeTag(String& json, bool& first, const char* tag) {
  if (!first) {
    json += ',';
  }
  first = false;
  json += '"';
  json += tag;
  json += '"';
}

String buildModeTagsJson(const ModeMetadata& metadata) {
  String json;
  json.reserve(110);
  json += '[';
  bool first = true;
  if (metadata.animated) appendModeTag(json, first, "animated");
  if (metadata.calm) appendModeTag(json, first, "calm");
  if (metadata.sleepSafe) appendModeTag(json, first, "sleepSafe");
  if (metadata.focusFriendly) appendModeTag(json, first, "focusFriendly");
  if (metadata.colorful) appendModeTag(json, first, "colorful");
  if (metadata.flashing) appendModeTag(json, first, "flashing");
  if (metadata.utility) appendModeTag(json, first, "utility");
  if (metadata.experimental) appendModeTag(json, first, "experimental");
  if (metadata.blockedByNightGuard) appendModeTag(json, first, "blockedByNightGuard");
  if (metadata.supportsPalette) appendModeTag(json, first, "palette");
  json += ']';
  return json;
}

String currentModeTagsJson() {
  const ModeMetadata* metadata = findModeMetadata(settings.mode);
  if (metadata == nullptr) {
    return F("[]");
  }
  return buildModeTagsJson(*metadata);
}

String buildModeMetadataJson(const ModeMetadata& metadata) {
  String json;
  json.reserve(520);
  json += R"json({"id":")json";
  json += escapeJson(String(metadata.id));
  json += R"json(","displayName":")json";
  json += escapeJson(String(metadata.displayName));
  json += R"json(","description":")json";
  json += escapeJson(String(metadata.description));
  json += R"json(","bestUsedFor":")json";
  json += escapeJson(String(metadata.bestUsedFor));
  json += R"json(","category":")json";
  json += escapeJson(String(metadata.category));
  json += R"json(","tags":)json";
  json += buildModeTagsJson(metadata);
  json += R"json(,"animated":)json";
  json += boolJson(metadata.animated);
  json += R"json(,"calm":)json";
  json += boolJson(metadata.calm);
  json += R"json(,"sleepSafe":)json";
  json += boolJson(metadata.sleepSafe);
  json += R"json(,"focusFriendly":)json";
  json += boolJson(metadata.focusFriendly);
  json += R"json(,"colorful":)json";
  json += boolJson(metadata.colorful);
  json += R"json(,"flashing":)json";
  json += boolJson(metadata.flashing);
  json += R"json(,"utility":)json";
  json += boolJson(metadata.utility);
  json += R"json(,"experimental":)json";
  json += boolJson(metadata.experimental);
  json += R"json(,"blockedByNightGuard":)json";
  json += boolJson(metadata.blockedByNightGuard);
  json += R"json(,"supportsPalette":)json";
  json += boolJson(metadata.supportsPalette);
  json += R"json(,"preferredPaletteRole":")json";
  json += escapeJson(String(metadata.preferredPaletteRole));
  json += '"';
  json += '}';
  return json;
}

String buildModesJson() {
  String json;
  json.reserve(360 + (MODE_METADATA_COUNT * 470));
  json += R"json({"ok":true,"version":1,"count":)json";
  json += MODE_METADATA_COUNT;
  json += R"json(,"modes":[)json";
  for (uint8_t i = 0; i < MODE_METADATA_COUNT; i++) {
    if (i > 0) {
      json += ',';
    }
    json += buildModeMetadataJson(MODE_METADATA[i]);
  }
  json += F("]}");
  return json;
}

bool modeIsFlashing(const String& modeName) {
  const ModeMetadata* metadata = findModeMetadataById(modeName);
  return metadata != nullptr && metadata->flashing;
}

bool modeBlockedByNightGuard(const String& modeName) {
  const ModeMetadata* metadata = findModeMetadataById(modeName);
  return metadata != nullptr && metadata->blockedByNightGuard;
}

void handleApiModes() {
  server.send(200, "application/json", buildModesJson());
}

#endif
