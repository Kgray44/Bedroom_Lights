#ifndef BEDROOM_LED_CONTROLLER_PALETTE_CONTROLS_H
#define BEDROOM_LED_CONTROLLER_PALETTE_CONTROLS_H

String cleanPaletteName(const String& rawName) {
  String name = rawName;
  name.trim();
  while (name.length() > MAX_PALETTE_NAME_LENGTH) {
    name.remove(name.length() - 1);
  }
  return name;
}

String safePaletteIdToken(const String& value) {
  String token;
  token.reserve(value.length());
  bool previousDash = false;

  for (uint16_t i = 0; i < value.length() && token.length() < MAX_PALETTE_ID_LENGTH; i++) {
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
    token = "palette";
  }
  return token;
}

String paletteColorHex(const PaletteColor& color) {
  char hex[8];
  snprintf(hex, sizeof(hex), "#%02X%02X%02X", color.r, color.g, color.b);
  return String(hex);
}

String paletteColorHex(const RgbPixel& color) {
  char hex[8];
  snprintf(hex, sizeof(hex), "#%02X%02X%02X", color.r, color.g, color.b);
  return String(hex);
}

bool parsePaletteColor(const String& raw, PaletteColor& color) {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  if (!parseHexColor(raw, r, g, b)) {
    return false;
  }
  color.r = r;
  color.g = g;
  color.b = b;
  color.weight = 255;
  return true;
}

int findPaletteById(const String& id) {
  for (uint8_t i = 0; i < paletteCount; i++) {
    if (palettes[i].id == id) {
      return i;
    }
  }
  return -1;
}

int findPaletteByIdConst(const char* id) {
  if (id == nullptr) {
    return -1;
  }
  for (uint8_t i = 0; i < paletteCount; i++) {
    if (palettes[i].id == id) {
      return i;
    }
  }
  return -1;
}

bool paletteNameExists(const String& name, int ignoreIndex) {
  String needle = cleanPaletteName(name);
  needle.toLowerCase();
  for (uint8_t i = 0; i < paletteCount; i++) {
    if (static_cast<int>(i) == ignoreIndex) {
      continue;
    }
    String candidate = cleanPaletteName(palettes[i].name);
    candidate.toLowerCase();
    if (candidate == needle) {
      return true;
    }
  }
  return false;
}

String makePaletteId(const String& name) {
  String base = safePaletteIdToken(name);
  String candidate = base;
  uint8_t suffix = 2;
  while (findPaletteById(candidate) >= 0) {
    candidate = base;
    candidate += '-';
    candidate += suffix;
    suffix++;
    while (candidate.length() > MAX_PALETTE_ID_LENGTH) {
      candidate.remove(candidate.length() - 1);
    }
  }
  return candidate;
}

RgbPixel fallbackPalettePixel() {
  return {settings.color.r, settings.color.g, settings.color.b};
}

bool addPalette(
  const String& id,
  const String& name,
  const String& category,
  bool builtin,
  bool calm,
  bool sleepSafe,
  bool colorful,
  bool warm,
  bool cool,
  const PaletteColor* colors,
  uint8_t colorCount
) {
  if (paletteCount >= MAX_PALETTES ||
      id.length() == 0 ||
      name.length() == 0 ||
      colorCount < MIN_PALETTE_COLORS ||
      colorCount > MAX_PALETTE_COLORS ||
      findPaletteById(id) >= 0) {
    return false;
  }

  ColorPalette palette;
  palette.id = safePaletteIdToken(id);
  palette.name = cleanPaletteName(name);
  palette.colorCount = colorCount;
  for (uint8_t i = 0; i < MAX_PALETTE_COLORS; i++) {
    palette.colors[i] = i < colorCount ? colors[i] : PaletteColor{0, 0, 0, 255};
  }
  palette.builtin = builtin;
  palette.category = category.length() > 0 ? category : (builtin ? "Built-in" : "Custom");
  palette.calm = calm;
  palette.sleepSafe = sleepSafe;
  palette.colorful = colorful;
  palette.warm = warm;
  palette.cool = cool;

  palettes[paletteCount] = palette;
  paletteCount++;
  return true;
}

bool addBuiltInPalette(
  const String& id,
  const String& name,
  const String& category,
  bool calm,
  bool sleepSafe,
  bool colorful,
  bool warm,
  bool cool,
  const char* c1,
  const char* c2,
  const char* c3,
  const char* c4,
  const char* c5
) {
  PaletteColor colors[MAX_PALETTE_COLORS];
  const char* rawColors[MAX_PALETTE_COLORS] = {c1, c2, c3, c4, c5};
  uint8_t colorCount = 0;
  for (uint8_t i = 0; i < MAX_PALETTE_COLORS; i++) {
    if (rawColors[i] == nullptr || strlen(rawColors[i]) == 0) {
      continue;
    }
    PaletteColor parsed;
    if (!parsePaletteColor(String(rawColors[i]), parsed)) {
      return false;
    }
    colors[colorCount] = parsed;
    colorCount++;
  }
  return addPalette(id, name, category, true, calm, sleepSafe, colorful, warm, cool, colors, colorCount);
}

uint8_t addDefaultBuiltInPalettes() {
  uint8_t before = paletteCount;
  addBuiltInPalette("candle", "Candle", "Warm", true, true, false, true, false, "#FFB15A", "#FF7A2F", "#FFD6A0", "#FFF0C8", "");
  addBuiltInPalette("ocean", "Ocean", "Water", true, false, false, false, true, "#0B5D7A", "#1EA7A8", "#4FD3C4", "#B8FFF2", "");
  addBuiltInPalette("garden", "Garden", "Natural", true, false, false, false, false, "#6E8F4E", "#A8B96A", "#D7C46A", "#F2DFA0", "");
  addBuiltInPalette("moonlight", "Moonlight", "Night", true, true, false, false, true, "#DDEBFF", "#8FB8FF", "#405C9A", "#172B5A", "");
  addBuiltInPalette("peach-sky", "Peach Sky", "Evening", true, false, false, true, false, "#FFD0A8", "#FF9C8A", "#FFE7C7", "#F7B7D2", "");
  addBuiltInPalette("soft-neon", "Soft Neon", "Colorful", false, false, true, false, true, "#4DEEEA", "#74EE15", "#FFE700", "#F000FF", "");
  addBuiltInPalette("aurora", "Aurora", "Colorful", true, false, true, false, true, "#3EF6B0", "#4DA3FF", "#9D6CFF", "#D6F8FF", "");
  addBuiltInPalette("warm-linen", "Warm Linen", "Everyday", true, true, false, true, false, "#FFE6B8", "#FFD9A0", "#F4C27A", "#FFF3D6", "");
  addBuiltInPalette("storm-blue", "Storm Blue", "Weather", true, false, false, false, true, "#102033", "#1F3A5A", "#416A8A", "#A7C7D9", "");
  addBuiltInPalette("circuit-glow", "Circuit Glow", "Experimental", false, false, true, false, true, "#0AFF9D", "#00B8FF", "#0A2A3A", "#D8FFF3", "");
  return paletteCount - before;
}

void loadDefaultPalettes() {
  paletteCount = 0;
  addDefaultBuiltInPalettes();
  paletteLoadStatus = "defaults active";
}

void ensureActivePaletteAvailable() {
  if (findPaletteById(settings.activePaletteId) >= 0) {
    return;
  }
  settings.activePaletteId = findPaletteById("warm-linen") >= 0
    ? "warm-linen"
    : (paletteCount > 0 ? palettes[0].id : "");
  if (settings.activePaletteId.length() == 0) {
    settings.paletteEnabled = false;
  }
}

ColorPalette* getActivePalette() {
  if (!settings.paletteEnabled) {
    return nullptr;
  }
  int index = findPaletteById(settings.activePaletteId);
  if (index < 0 || palettes[index].colorCount < MIN_PALETTE_COLORS) {
    return nullptr;
  }
  return &palettes[index];
}

String activePaletteName() {
  int index = findPaletteById(settings.activePaletteId);
  return index >= 0 ? palettes[index].name : String("None");
}

RgbPixel getPaletteColor(uint8_t index) {
  ColorPalette* palette = getActivePalette();
  if (palette == nullptr || palette->colorCount == 0) {
    return fallbackPalettePixel();
  }
  PaletteColor color = palette->colors[index % palette->colorCount];
  return {color.r, color.g, color.b};
}

RgbPixel blendPaletteColors(RgbPixel a, RgbPixel b, uint8_t amount) {
  return {
    static_cast<uint8_t>(a.r + (((int16_t)b.r - a.r) * amount) / 255),
    static_cast<uint8_t>(a.g + (((int16_t)b.g - a.g) * amount) / 255),
    static_cast<uint8_t>(a.b + (((int16_t)b.b - a.b) * amount) / 255)
  };
}

RgbPixel samplePalette(uint16_t position, uint16_t maxPosition) {
  ColorPalette* palette = getActivePalette();
  if (palette == nullptr || palette->colorCount < MIN_PALETTE_COLORS) {
    return fallbackPalettePixel();
  }
  if (maxPosition == 0 || palette->colorCount == 1) {
    PaletteColor color = palette->colors[0];
    return {color.r, color.g, color.b};
  }

  uint32_t scaled =
    (static_cast<uint32_t>(position) * static_cast<uint32_t>(palette->colorCount - 1) * 255UL) /
    maxPosition;
  uint8_t segment = static_cast<uint8_t>(scaled / 255UL);
  uint8_t amount = static_cast<uint8_t>(scaled % 255UL);
  if (segment >= palette->colorCount - 1) {
    PaletteColor color = palette->colors[palette->colorCount - 1];
    return {color.r, color.g, color.b};
  }
  PaletteColor aColor = palette->colors[segment];
  PaletteColor bColor = palette->colors[segment + 1];
  RgbPixel a = {aColor.r, aColor.g, aColor.b};
  RgbPixel b = {bColor.r, bColor.g, bColor.b};
  return blendPaletteColors(a, b, amount);
}

RgbPixel getWeightedPaletteColor(uint8_t selector) {
  ColorPalette* palette = getActivePalette();
  if (palette == nullptr || palette->colorCount < MIN_PALETTE_COLORS) {
    return fallbackPalettePixel();
  }

  uint16_t totalWeight = 0;
  for (uint8_t i = 0; i < palette->colorCount; i++) {
    totalWeight += palette->colors[i].weight == 0 ? 1 : palette->colors[i].weight;
  }
  uint16_t target = (static_cast<uint16_t>(selector) * totalWeight) / 256U;
  uint16_t running = 0;
  for (uint8_t i = 0; i < palette->colorCount; i++) {
    running += palette->colors[i].weight == 0 ? 1 : palette->colors[i].weight;
    if (target < running) {
      PaletteColor color = palette->colors[i];
      return {color.r, color.g, color.b};
    }
  }

  PaletteColor color = palette->colors[palette->colorCount - 1];
  return {color.r, color.g, color.b};
}

String buildPaletteTagsJson(const ColorPalette& palette) {
  String json;
  json.reserve(92);
  json += '[';
  bool first = true;
  if (palette.warm) appendModeTag(json, first, "warm");
  if (palette.cool) appendModeTag(json, first, "cool");
  if (palette.calm) appendModeTag(json, first, "calm");
  if (palette.sleepSafe) appendModeTag(json, first, "sleepSafe");
  if (palette.colorful) appendModeTag(json, first, "colorful");
  json += ']';
  return json;
}

String buildSinglePaletteJson(const ColorPalette& palette) {
  String json;
  json.reserve(420);
  json += R"json({"id":")json";
  json += escapeJson(palette.id);
  json += R"json(","name":")json";
  json += escapeJson(palette.name);
  json += R"json(","builtin":)json";
  json += boolJson(palette.builtin);
  json += R"json(,"category":")json";
  json += escapeJson(palette.category);
  json += R"json(","colorCount":)json";
  json += palette.colorCount;
  json += R"json(,"calm":)json";
  json += boolJson(palette.calm);
  json += R"json(,"sleepSafe":)json";
  json += boolJson(palette.sleepSafe);
  json += R"json(,"colorful":)json";
  json += boolJson(palette.colorful);
  json += R"json(,"warm":)json";
  json += boolJson(palette.warm);
  json += R"json(,"cool":)json";
  json += boolJson(palette.cool);
  json += R"json(,"tags":)json";
  json += buildPaletteTagsJson(palette);
  json += R"json(,"colors":[)json";
  for (uint8_t i = 0; i < palette.colorCount; i++) {
    if (i > 0) {
      json += ',';
    }
    json += '"';
    json += paletteColorHex(palette.colors[i]);
    json += '"';
  }
  json += R"json(],"weights":[)json";
  for (uint8_t i = 0; i < palette.colorCount; i++) {
    if (i > 0) {
      json += ',';
    }
    json += palette.colors[i].weight;
  }
  json += F("]}");
  return json;
}

String buildPalettesJson() {
  String json;
  json.reserve(520 + paletteCount * 330);
  json += R"json({"ok":true,"version":1,"paletteEnabled":)json";
  json += boolJson(settings.paletteEnabled);
  json += R"json(,"activePaletteId":")json";
  json += escapeJson(settings.activePaletteId);
  json += R"json(","activePaletteName":")json";
  json += escapeJson(activePaletteName());
  json += R"json(","currentModeSupportsPalette":)json";
  json += boolJson(currentModeSupportsPalette());
  json += R"json(,"currentModePaletteRole":")json";
  json += escapeJson(currentModePaletteRole());
  json += R"json(","count":)json";
  json += paletteCount;
  json += R"json(,"maxPalettes":)json";
  json += MAX_PALETTES;
  json += R"json(,"path":")json";
  json += PALETTES_PATH;
  json += R"json(","loadStatus":")json";
  json += escapeJson(paletteLoadStatus);
  json += R"json(","saveStatus":")json";
  json += escapeJson(paletteSaveStatus);
  json += R"json(","palettes":[)json";
  for (uint8_t i = 0; i < paletteCount; i++) {
    if (i > 0) {
      json += ',';
    }
    json += buildSinglePaletteJson(palettes[i]);
  }
  json += F("]}");
  return json;
}

String buildPaletteExportJson(const ColorPalette& palette) {
  String json;
  String paletteJson = buildSinglePaletteJson(palette);
  json.reserve(120 + paletteJson.length());
  json += R"json({"ok":true,"paletteSchema":1,"exportType":"palette","palette":)json";
  json += paletteJson;
  json += F("}");
  return json;
}

uint8_t readPaletteColorsArray(const String& objectJson, PaletteColor* colors) {
  int arrayStart = objectJson.indexOf("\"colors\":[");
  if (arrayStart < 0) {
    return 0;
  }
  arrayStart = objectJson.indexOf('[', arrayStart);
  int arrayEnd = objectJson.indexOf(']', arrayStart);
  if (arrayStart < 0 || arrayEnd < 0) {
    return 0;
  }

  uint8_t colorCount = 0;
  int position = arrayStart + 1;
  while (position < arrayEnd && colorCount < MAX_PALETTE_COLORS) {
    int quoteStart = objectJson.indexOf('"', position);
    if (quoteStart < 0 || quoteStart >= arrayEnd) {
      break;
    }
    int quoteEnd = objectJson.indexOf('"', quoteStart + 1);
    if (quoteEnd < 0 || quoteEnd > arrayEnd) {
      break;
    }
    PaletteColor color;
    if (parsePaletteColor(objectJson.substring(quoteStart + 1, quoteEnd), color)) {
      colors[colorCount] = color;
      colorCount++;
    }
    position = quoteEnd + 1;
  }
  return colorCount;
}

void readPaletteWeightsArray(const String& objectJson, PaletteColor* colors, uint8_t colorCount) {
  int arrayStart = objectJson.indexOf("\"weights\":[");
  if (arrayStart < 0) {
    return;
  }
  arrayStart = objectJson.indexOf('[', arrayStart);
  int arrayEnd = objectJson.indexOf(']', arrayStart);
  if (arrayStart < 0 || arrayEnd < 0) {
    return;
  }

  uint8_t index = 0;
  int position = arrayStart + 1;
  while (position < arrayEnd && index < colorCount) {
    int nextComma = objectJson.indexOf(',', position);
    if (nextComma < 0 || nextComma > arrayEnd) {
      nextComma = arrayEnd;
    }
    String token = objectJson.substring(position, nextComma);
    token.trim();
    if (token.length() > 0) {
      int parsed = token.toInt();
      colors[index].weight = static_cast<uint8_t>(constrain(parsed, 1, 255));
      index++;
    }
    position = nextComma + 1;
  }
}

bool loadPaletteObject(const String& objectJson, ColorPalette& palette) {
  String textValue;
  bool boolValue;
  PaletteColor colors[MAX_PALETTE_COLORS];

  palette.id = "";
  palette.name = "";
  palette.category = "Custom";
  palette.builtin = false;
  palette.calm = false;
  palette.sleepSafe = false;
  palette.colorful = false;
  palette.warm = false;
  palette.cool = false;
  palette.colorCount = readPaletteColorsArray(objectJson, colors);
  readPaletteWeightsArray(objectJson, colors, palette.colorCount);

  if (readJsonString(objectJson, "id", textValue)) {
    palette.id = safePaletteIdToken(textValue);
  }
  if (readJsonString(objectJson, "name", textValue)) {
    palette.name = cleanPaletteName(textValue);
  }
  if (readJsonString(objectJson, "category", textValue)) {
    textValue.trim();
    palette.category = textValue.length() > 0 ? textValue : "Custom";
  }
  if (readJsonBool(objectJson, "builtin", boolValue)) {
    palette.builtin = boolValue;
  }
  if (readJsonBool(objectJson, "calm", boolValue)) {
    palette.calm = boolValue;
  }
  if (readJsonBool(objectJson, "sleepSafe", boolValue)) {
    palette.sleepSafe = boolValue;
  }
  if (readJsonBool(objectJson, "colorful", boolValue)) {
    palette.colorful = boolValue;
  }
  if (readJsonBool(objectJson, "warm", boolValue)) {
    palette.warm = boolValue;
  }
  if (readJsonBool(objectJson, "cool", boolValue)) {
    palette.cool = boolValue;
  }

  if (palette.id.length() == 0 || palette.name.length() == 0 ||
      palette.colorCount < MIN_PALETTE_COLORS || palette.colorCount > MAX_PALETTE_COLORS) {
    return false;
  }

  for (uint8_t i = 0; i < MAX_PALETTE_COLORS; i++) {
    palette.colors[i] = i < palette.colorCount ? colors[i] : PaletteColor{0, 0, 0, 255};
  }
  return true;
}

bool savePalettes() {
  if (!settingsStorageReady) {
    paletteSaveStatus = "skipped; LittleFS unavailable";
    return false;
  }

  File file = LittleFS.open(PALETTES_PATH, "w");
  if (!file) {
    paletteSaveStatus = "open failed";
    return false;
  }

  String json = buildPalettesJson();
  bool saved = file.print(json) == json.length();
  file.close();
  paletteSaveStatus = saved ? "saved" : "write incomplete";
  return saved;
}

void loadPalettes() {
  loadDefaultPalettes();
  // palette load/save status is surfaced through diagnostics and /api/palettes.

  if (!settingsStorageReady) {
    paletteLoadStatus = "LittleFS unavailable; defaults active";
    ensureActivePaletteAvailable();
    return;
  }

  if (!LittleFS.exists(PALETTES_PATH)) {
    paletteLoadStatus = "missing; defaults active";
    savePalettes();
    ensureActivePaletteAvailable();
    return;
  }

  File file = LittleFS.open(PALETTES_PATH, "r");
  if (!file) {
    paletteLoadStatus = "open failed; defaults active";
    ensureActivePaletteAvailable();
    return;
  }

  String json = file.readString();
  file.close();
  json.trim();
  int palettesStart = json.indexOf("\"palettes\":[");
  if (!json.startsWith("{") || palettesStart < 0) {
    paletteLoadStatus = "corrupt; defaults active";
    ensureActivePaletteAvailable();
    return;
  }

  uint8_t customLoaded = 0;
  int position = json.indexOf('{', palettesStart);
  while (position >= 0 && paletteCount < MAX_PALETTES) {
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

    ColorPalette palette;
    if (loadPaletteObject(json.substring(position, end + 1), palette) &&
        !palette.builtin &&
        findPaletteById(palette.id) < 0) {
      palettes[paletteCount] = palette;
      paletteCount++;
      customLoaded++;
    }
    position = json.indexOf('{', end + 1);
  }

  ensureActivePaletteAvailable();
  paletteLoadStatus = customLoaded > 0 ? "loaded; built-ins merged" : "loaded; defaults only";
}

bool collectPaletteColorsFromRequest(PaletteColor* colors, uint8_t& colorCount, String& error) {
  colorCount = 0;
  for (uint8_t i = 0; i < MAX_PALETTE_COLORS; i++) {
    String argName = String("c") + String(i + 1);
    if (!server.hasArg(argName)) {
      continue;
    }
    String raw = server.arg(argName);
    raw.trim();
    if (raw.length() == 0) {
      continue;
    }
    PaletteColor color;
    if (!parsePaletteColor(raw, color)) {
      error = "Invalid hex color";
      return false;
    }
    String weightName = String("w") + String(i + 1);
    if (server.hasArg(weightName)) {
      int parsedWeight;
      if (!parseRequestInt(server.arg(weightName), parsedWeight)) {
        error = "Invalid palette weight";
        return false;
      }
      color.weight = static_cast<uint8_t>(constrain(parsedWeight, 1, 255));
    }
    colors[colorCount] = color;
    colorCount++;
  }

  if (colorCount < MIN_PALETTE_COLORS) {
    error = "At least two valid colors are required";
    return false;
  }
  return true;
}

bool addPaletteFromRequest(String& outId, String& error) {
  if (paletteCount >= MAX_PALETTES) {
    error = "Palette limit reached";
    return false;
  }
  if (!server.hasArg("name")) {
    error = "Palette name is required";
    return false;
  }

  String name = cleanPaletteName(server.arg("name"));
  if (name.length() == 0) {
    error = "Palette name is required";
    return false;
  }

  PaletteColor colors[MAX_PALETTE_COLORS];
  uint8_t colorCount;
  if (!collectPaletteColorsFromRequest(colors, colorCount, error)) {
    return false;
  }

  String category = server.hasArg("category") ? cleanPaletteName(server.arg("category")) : "Custom";
  if (category.length() == 0) {
    category = "Custom";
  }
  String id = makePaletteId(name);
  if (paletteNameExists(name, -1)) {
    id = makePaletteId(String("custom-") + name);
  }

  bool calm = server.hasArg("calm") && server.arg("calm") != "0";
  bool sleepSafe = server.hasArg("sleepSafe") && server.arg("sleepSafe") != "0";
  bool colorful = server.hasArg("colorful") && server.arg("colorful") != "0";
  bool warm = server.hasArg("warm") && server.arg("warm") != "0";
  bool cool = server.hasArg("cool") && server.arg("cool") != "0";
  if (!addPalette(id, name, category, false, calm, sleepSafe, colorful, warm, cool, colors, colorCount)) {
    error = "Palette could not be saved";
    return false;
  }
  outId = id;
  return true;
}

String uniquePaletteName(const String& requestedName, int ignoreIndex) {
  String baseName = cleanPaletteName(requestedName);
  if (baseName.length() == 0) {
    baseName = "Imported Palette";
  }
  if (!paletteNameExists(baseName, ignoreIndex)) {
    return baseName;
  }

  for (uint8_t suffix = 2; suffix < 100; suffix++) {
    String suffixText = String(" ") + suffix;
    String candidate = baseName;
    while (candidate.length() + suffixText.length() > MAX_PALETTE_NAME_LENGTH) {
      candidate.remove(candidate.length() - 1);
    }
    candidate += suffixText;
    if (!paletteNameExists(candidate, ignoreIndex)) {
      return candidate;
    }
  }
  return String("Palette ") + millis();
}

bool importPaletteFromJson(const String& rawJson, String& outId, String& error) {
  if (paletteCount >= MAX_PALETTES) {
    error = "Palette limit reached";
    return false;
  }

  String objectJson;
  if (!extractJsonObjectForKey(rawJson, "palette", objectJson) &&
      !extractFirstJsonObject(rawJson, objectJson)) {
    error = "Palette JSON object is required";
    return false;
  }

  ColorPalette palette;
  if (!loadPaletteObject(objectJson, palette)) {
    error = "Palette JSON is invalid";
    return false;
  }

  palette.builtin = false;
  palette.category = palette.category.length() > 0 ? palette.category : "Custom";
  palette.name = uniquePaletteName(palette.name, -1);
  palette.id = makePaletteId(palette.name);
  palettes[paletteCount] = palette;
  paletteCount++;
  if (!savePalettes()) {
    paletteCount--;
    error = "Palette file save failed";
    return false;
  }
  outId = palette.id;
  return true;
}

uint8_t resetBuiltInPalettes() {
  uint8_t writeIndex = 0;
  for (uint8_t i = 0; i < paletteCount; i++) {
    if (!palettes[i].builtin) {
      palettes[writeIndex++] = palettes[i];
    }
  }
  paletteCount = writeIndex;
  uint8_t restored = addDefaultBuiltInPalettes();
  ensureActivePaletteAvailable();
  savePalettes();
  paletteLoadStatus = "built-ins restored";
  return restored;
}

bool updatePaletteFromRequest(const String& id, String& error) {
  int index = findPaletteById(id);
  if (index < 0) {
    error = "Unknown palette";
    return false;
  }
  if (palettes[index].builtin) {
    error = "Built-in palettes cannot be edited";
    return false;
  }

  String name = server.hasArg("name") ? cleanPaletteName(server.arg("name")) : palettes[index].name;
  if (name.length() == 0) {
    error = "Palette name is required";
    return false;
  }

  PaletteColor colors[MAX_PALETTE_COLORS];
  uint8_t colorCount;
  if (!collectPaletteColorsFromRequest(colors, colorCount, error)) {
    return false;
  }

  palettes[index].name = name;
  palettes[index].category = server.hasArg("category") ? cleanPaletteName(server.arg("category")) : palettes[index].category;
  if (palettes[index].category.length() == 0) {
    palettes[index].category = "Custom";
  }
  palettes[index].colorCount = colorCount;
  for (uint8_t i = 0; i < MAX_PALETTE_COLORS; i++) {
    palettes[index].colors[i] = i < colorCount ? colors[i] : PaletteColor{0, 0, 0, 255};
  }
  return true;
}

bool deletePaletteById(const String& id, String& error) {
  int index = findPaletteById(id);
  if (index < 0) {
    error = "Unknown palette";
    return false;
  }
  if (palettes[index].builtin) {
    error = "Built-in palettes cannot be deleted";
    return false;
  }

  bool deletingActive = settings.activePaletteId == id;
  for (uint8_t i = index; i + 1 < paletteCount; i++) {
    palettes[i] = palettes[i + 1];
  }
  paletteCount--;
  if (deletingActive) {
    settings.paletteEnabled = false;
    settings.activePaletteId = "warm-linen";
    ensureActivePaletteAvailable();
    markSettingsDirty();
  }
  return true;
}

void sendPaletteActionOk(const String& message, const String& paletteId) {
  String json;
  json.reserve(180 + message.length() + paletteId.length());
  json += R"json({"ok":true,"message":")json";
  json += escapeJson(message);
  json += R"json(","paletteId":")json";
  json += escapeJson(paletteId);
  json += R"json(","palettes":)json";
  json += buildPalettesJson();
  json += R"json(,"state":)json";
  json += buildStateJson();
  json += F("}");
  server.send(200, "application/json", json);
}

void handleApiPalettes() {
  uint32_t heapBefore = beginEndpointHeapMetric("/api/palettes");
  String json = buildPalettesJson();
  finishEndpointHeapMetric("/api/palettes", heapBefore, json.length());
  server.send(200, "application/json", json);
}

void handleApiPalettesSelect() {
  bool changed = false;
  if (server.hasArg("id")) {
    String id = safePaletteIdToken(server.arg("id"));
    if (findPaletteById(id) < 0) {
      sendJsonError(404, "Unknown palette");
      return;
    }
    if (settings.activePaletteId != id) {
      settings.activePaletteId = id;
      changed = true;
    }
  }

  if (server.hasArg("enabled")) {
    bool enabled;
    if (!parseRequestBool(server.arg("enabled"), enabled)) {
      sendJsonError(400, "Invalid palette enabled value");
      return;
    }
    if (settings.paletteEnabled != enabled) {
      settings.paletteEnabled = enabled;
      changed = true;
    }
  }

  ensureActivePaletteAvailable();
  if (changed) {
    clearLoadedSceneTracking();
    markSettingsDirty();
    ledsDirty = true;
    if (currentModeSupportsPalette()) {
      beginTransitionToCurrentState();
    }
  }
  sendJsonOk("Palette selected", buildStateJson());
}

void handleApiPalettesSave() {
  String paletteId;
  String error;
  if (!addPaletteFromRequest(paletteId, error)) {
    sendJsonError(400, error);
    return;
  }
  if (!savePalettes()) {
    sendJsonError(500, "Palette file save failed");
    return;
  }
  sendPaletteActionOk("Palette saved", paletteId);
}

void handleApiPalettesUpdate() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Palette id is required");
    return;
  }
  String id = safePaletteIdToken(server.arg("id"));
  String error;
  if (!updatePaletteFromRequest(id, error)) {
    sendJsonError(400, error);
    return;
  }
  if (!savePalettes()) {
    sendJsonError(500, "Palette file save failed");
    return;
  }
  if (settings.activePaletteId == id && currentModeSupportsPalette()) {
    ledsDirty = true;
    beginTransitionToCurrentState();
  }
  sendPaletteActionOk("Palette updated", id);
}

void handleApiPalettesDelete() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Palette id is required");
    return;
  }
  String id = safePaletteIdToken(server.arg("id"));
  String error;
  if (!deletePaletteById(id, error)) {
    sendJsonError(400, error);
    return;
  }
  if (!savePalettes()) {
    sendJsonError(500, "Palette file save failed");
    return;
  }
  ledsDirty = true;
  sendPaletteActionOk("Palette deleted", id);
}

void handleApiPalettesReset() {
  loadDefaultPalettes();
  settings.paletteEnabled = false;
  settings.activePaletteId = "warm-linen";
  ensureActivePaletteAvailable();
  savePalettes();
  markSettingsDirty();
  ledsDirty = true;
  sendPaletteActionOk("Palettes reset", settings.activePaletteId);
}

void handleApiPalettesExport() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Palette id is required");
    return;
  }
  int index = findPaletteById(safePaletteIdToken(server.arg("id")));
  if (index < 0) {
    sendJsonError(404, "Unknown palette");
    return;
  }
  server.send(200, "application/json", buildPaletteExportJson(palettes[index]));
}

void handleApiPalettesImport() {
  if (!server.hasArg("json")) {
    sendJsonError(400, "Palette JSON is required");
    return;
  }
  String outId;
  String error;
  if (!importPaletteFromJson(server.arg("json"), outId, error)) {
    sendJsonError(400, error);
    return;
  }
  sendPaletteActionOk("Palette imported", outId);
}

void handleApiPalettesResetBuiltins() {
  uint8_t restored = resetBuiltInPalettes();
  String message = "Built-in palettes restored";
  if (restored > 0) {
    message += " (";
    message += restored;
    message += " added)";
  }
  sendPaletteActionOk(message, settings.activePaletteId);
}

#endif
