#ifndef BEDROOM_LED_CONTROLLER_FORWARD_DECLARATIONS_H
#define BEDROOM_LED_CONTROLLER_FORWARD_DECLARATIONS_H

struct ModeMetadata;

void setupWifi();
void setupOta();
void setupRoutes();
void handleRoot();
void handleOtaPage();
void handleDiagnosticsPage();
void handleSchedulePage();
void handleApiState();
void handleApiModes();
void handleApiTransitions();
void handleApiTransitionsSet();
void handleApiPalettes();
void handleApiPalettesSelect();
void handleApiPalettesSave();
void handleApiPalettesUpdate();
void handleApiPalettesDelete();
void handleApiPalettesReset();
void handleApiPalettesExport();
void handleApiPalettesImport();
void handleApiPalettesResetBuiltins();
void handleApiSurprise();
void handleApiSurpriseOptions();
void handleApiBrightness();
void handleApiColor();
void handleApiTemperature();
void handleApiMode();
void handleApiScenes();
void handleApiScenesSave();
void handleApiScenesLoad();
void handleApiScenesRename();
void handleApiScenesDelete();
void handleApiScenesDuplicate();
void handleApiScenesExport();
void handleApiScenesImport();
void handleApiScenesResetBuiltins();
void handleApiFavorites();
void handleApiFavoritesLoad();
void handleApiFavoritesReset();
void handleApiAction();
void handleApiPanicWarm();
void handleApiNightGuard();
void handleApiNightGuardSet();
void handleApiTimer();
void handleApiTimerStart();
void handleApiTimerCancel();
void handleApiBedtimeStart();
void handleApiBedtimeCancel();
void handleApiTime();
void handleApiTimeSync();
void handleApiTimeSet();
void handleApiSchedule();
void handleApiScheduleSave();
void handleApiScheduleDelete();
void handleApiScheduleToggle();
void handleApiScheduleSkip();
void handleApiScheduleRun();
void handleApiScheduleExport();
void handleApiScheduleImport();
void handleApiScheduleReset();
void handleApiDiagnostics();
void handleApiResources();
void handleApiPower();
void handleApiTest();
void handleApiBackupExport();
void handleApiBackupImport();
void handleApiReset();
void handleApiDocsPage();
void handleGoLink();
void handleSet();
void handleState();
void handleNotFound();

void applyDefaultSettings();
bool readJsonString(const String& json, const char* key, String& value);
bool readJsonInt(const String& json, const char* key, int& value);
bool readJsonBool(const String& json, const char* key, bool& value);
bool readJsonFloat(const String& json, const char* key, float& value);
int findJsonValueStart(const String& json, const char* key);
int findMatchingJsonClose(const String& json, int start, char openChar, char closeChar);
bool extractJsonObjectForKey(const String& json, const char* key, String& objectJson);
bool extractJsonArrayForKey(const String& json, const char* key, String& arrayJson);
bool extractFirstJsonObject(const String& json, String& objectJson);
void copyFixedCString(char* destination, size_t destinationSize, const char* value);
void copyFixedString(char* destination, size_t destinationSize, const String& value);
bool fixedEquals(const char* value, const String& expected);
String fixedString(const char* value);
void loadSettings();
void saveSettings();
void markSettingsDirty();
void serviceSettingsSave();
void applyBootBehavior();
PowerEstimate buildPowerEstimate();
String buildStateJson();
String buildModesJson();
String buildModeMetadataJson(const ModeMetadata& metadata);
String buildModeTagsJson(const ModeMetadata& metadata);
String currentModeTagsJson();
String modeDisplayName(Mode mode);
String modeCategory(Mode mode);
bool modeSupportsPalette(Mode mode);
bool currentModeSupportsPalette();
String currentModePaletteRole();
String normalizeTransitionStyle(String style);
String buildTransitionsJson();
String buildPalettesJson();
String buildPaletteExportJson(const ColorPalette& palette);
bool importPaletteFromJson(const String& rawJson, String& outId, String& error);
uint8_t resetBuiltInPalettes();
String normalizeSurpriseMood(String mood);
bool isSupportedSurpriseMood(const String& mood);
String buildSurpriseOptionsJson();
String buildSurpriseCandidateJson(const SurpriseCandidate& candidate, bool applied);
void beginTimeSync(bool manual);
void serviceTimeSync();
bool isTimeSynced();
String buildTimeJson();
String currentLocalTimeString();
String nextScheduleSummary();
String buildScheduleJson();
String buildScheduleExportJson();
bool importScheduleFromJson(const String& rawJson, String& error);
bool resetSchedules(String& error);
void loadSchedules();
bool saveSchedules();
void serviceScheduler();
uint32_t scheduleOccurrenceKey(uint16_t dayIndex, uint16_t minuteOfDay);
bool validateScheduleEntry(const ScheduleEntry& entry, String& error);
bool executeScheduleTarget(ScheduleEntry& entry, bool manual, String& error);
uint8_t surpriseBrightnessForMood(const String& mood, bool& powerCapped, bool& nightGuardCapped);
uint8_t capSurpriseBrightness(uint8_t requested, bool& powerCapped, bool& nightGuardCapped);
Mode selectSurpriseMode(const String& mood);
String selectSurprisePaletteId(const String& mood);
bool generateSurpriseCandidate(const String& mood, SurpriseCandidate& candidate, String& error);
bool applySurpriseCandidate(const SurpriseCandidate& candidate, String& error);
String activePaletteName();
void loadDefaultPalettes();
void loadPalettes();
bool savePalettes();
int findPaletteById(const String& id);
int findPaletteByIdConst(const char* id);
ColorPalette* getActivePalette();
RgbPixel getPaletteColor(uint8_t index);
RgbPixel samplePalette(uint16_t position, uint16_t maxPosition);
RgbPixel blendPaletteColors(RgbPixel a, RgbPixel b, uint8_t amount);
RgbPixel getWeightedPaletteColor(uint8_t selector);
bool parsePaletteColor(const String& raw, PaletteColor& color);
String paletteColorHex(const PaletteColor& color);
String paletteColorHex(const RgbPixel& color);
String safePaletteIdToken(const String& value);
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
);
bool addPaletteFromRequest(String& outId, String& error);
bool updatePaletteFromRequest(const String& id, String& error);
bool deletePaletteById(const String& id, String& error);
String buildScenesJson();
String buildSingleSceneJson(const ScenePreset& scene);
String buildSceneWarningsJson(const ScenePreset& scene);
String buildSceneExportJson(const ScenePreset& scene);
bool duplicateSceneById(const String& id, const String& requestedName, String& outId, String& error);
bool importSceneFromJson(const String& rawJson, String& outId, String& error);
uint8_t restoreBuiltInScenes();
String buildFavoritesJson();
String buildNightGuardJson();
String buildTimerJson();
String timerModeKey(TimerModeType mode);
String timerActionKey(TimerActionType action);
String buildPowerJson();
String buildWarningsJson(const PowerEstimate& estimate);
String buildDiagnosticsJson();
String buildResourcesJson();
uint32_t beginEndpointHeapMetric(const char* route);
void finishEndpointHeapMetric(const char* route, uint32_t heapBeforeBytes, uint32_t payloadBytes);
String buildEndpointHeapMetricsJson();
void updateResourceStats();
String buildBackupExportJson();
void streamBackupExportJson();
void updateLeds();
void renderCurrentModeToFrame(uint32_t now);
void renderSatinBreathing(uint32_t now);
void renderPaperLanternRow(uint32_t now);
void renderLibraryLamp(uint32_t now);
void renderSolarNoon(uint32_t now);
void renderGoldenHour(uint32_t now);
void renderDreamAquarium(uint32_t now);
void renderCircuitGlow(uint32_t now);
void renderWindowSunrise(uint32_t now);
void renderEmberQuilt(uint32_t now);
void renderPorcelainSky(uint32_t now);
void renderHearthline(uint32_t now);
void renderEveningCurrent(uint32_t now);
void renderStarlitLinen(uint32_t now);
void renderDeepSeaGlass(uint32_t now);
void renderQuietNorthernLights(uint32_t now);
void renderCandleHall(uint32_t now);
void renderSlowPrism(uint32_t now);
void renderGardenShade(uint32_t now);
void renderSnowfallHush(uint32_t now);
void renderObservatoryGlow(uint32_t now);
void renderRainyWindow(uint32_t now);
void renderCampfireBlanket(uint32_t now);
void renderNorthernWhisper(uint32_t now);
void renderStormOutside(uint32_t now);
void renderDiagnosticTestToFrame(uint32_t now);
void startDiagnosticTest(const String& testName);
void restorePreviousLightingState();
void sendJsonOk(const String& message, const String& stateJson);
void sendJsonError(int statusCode, const String& error);
bool parseRequestInt(const String& raw, int& value);
bool parseRequestFloat(const String& raw, float& value);
bool parseRequestBool(const String& raw, bool& value);
bool parseIntArg(const String& name, int& out);
bool parseHexColor(const String& hex, uint8_t& r, uint8_t& g, uint8_t& b);
bool tryModeFromString(const String& key, Mode& mode);
void recordMutation(const char* route, const char* action, uint8_t beforeBrightness, Mode beforeMode);
bool setMasterBrightness(uint8_t value);
bool setActiveColor(uint8_t r, uint8_t g, uint8_t b);
bool setWhiteTemperature(uint16_t kelvin);
bool setModeByName(const String& modeName);
bool setModeByName(const String& modeName, String& error);
bool setGammaEnabled(bool enabled);
bool setRgbGains(uint8_t rGain, uint8_t gGain, uint8_t bGain);
bool setPowerConfig(
  uint16_t supplyMillivolts,
  uint16_t supplyMilliamps,
  uint8_t supplySafetyMarginPercent,
  uint16_t controllerOverheadMilliamps
);
bool setBootBehaviorValue(const String& bootBehavior);
bool setFloatSetting(float& target, float value, float minValue, float maxValue);
bool setByteSetting(uint8_t& target, int value, int minValue, int maxValue, bool resetAnimation);
void loadScenes();
bool saveScenes();
void loadDefaultScenes();
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
);
ScenePreset captureCurrentScene(const String& id, const String& name, bool builtin);
bool applyScene(const ScenePreset& scene);
bool saveCurrentStateAsScene(const String& name, String& outId, String& error);
bool loadSceneById(const String& id, String& error);
bool renameScene(const String& id, const String& name, String& error);
bool deleteScene(const String& id, String& error);
int findSceneById(const String& id);
bool sceneNameExists(const String& name, int ignoreIndex);
String makeSceneId(const String& name);
void loadFavorites();
bool saveFavorites();
void loadDefaultFavorites();
bool addDefaultSceneFavorite(const String& sceneId, const String& label);
bool loadFavoriteById(const String& id, String& error);
int findFavoriteById(const String& id);
bool favoriteTargetAvailable(const FavoriteItem& favorite);
bool runActionByName(const String& actionName);
bool runActionByName(const String& actionName, String& error);
bool runWarmDimNow();
bool runOffAction();
uint8_t getEffectiveBrightness();
bool isNightGuardBrightnessCapped();
bool isModeBlockedByNightGuard(const String& modeName);
uint8_t configuredEffectiveBrightness();
uint8_t transitionEffectiveBrightness();
uint32_t getTransitionElapsedMs();
uint8_t getTransitionProgressPercent();
bool isSupportedTransitionStyle(const String& style);
bool isSupportedTransitionDuration(uint16_t durationMs);
void copyFrame(const RgbPixel* source, RgbPixel* destination);
uint8_t lerp8(uint8_t start, uint8_t target, uint8_t amount);
uint8_t clamp8(int value);
RgbPixel blendRgb(RgbPixel a, RgbPixel b, uint8_t amount);
RgbPixel scaleRgb(RgbPixel color, uint8_t scale);
RgbPixel warmWhiteApprox(uint16_t kelvin);
RgbPixel coolWhiteApprox(uint16_t kelvin);
RgbPixel sampleActivePaletteOrColor(uint16_t position, uint16_t maxPosition);
RgbPixel sampleNamedOrActivePalette(const char* fallbackPaletteId, uint16_t position, uint16_t maxPosition);
float phaseFromMillis(uint32_t now, uint32_t periodMs);
float easeInOut01(float t);
uint8_t sineEase8(float phase);
uint8_t smoothStep8(uint8_t t);
uint8_t triangleWave8(float phase);
uint8_t triangleWave8(uint32_t now, uint32_t periodMs);
uint8_t sineApprox8(uint32_t now, uint32_t periodMs, uint16_t phaseOffset);
uint8_t softPulseEnvelope8(uint32_t now, uint32_t periodMs, uint16_t phaseOffset);
uint8_t hash8(uint16_t x);
uint8_t smoothHash8(uint16_t seed, uint32_t now, uint16_t periodMs);
uint8_t approach8(uint8_t current, uint8_t target, uint8_t maxStep);
bool shouldApplyTemporalSmoothing(Mode mode);
uint8_t smoothingStepForMode(Mode mode);
void resetTemporalSmoothing();
void applyTemporalSmoothingToFrame();
void blendFrames(const RgbPixel* startFrame, const RgbPixel* endFrame, RgbPixel* outputFrame, uint8_t amount);
void beginTransitionToCurrentState();
void cancelTransition(bool renderTargetFrame);
bool renderTransitionFrame(uint32_t now);
bool modeIsFlashing(const String& modeName);
bool modeBlockedByNightGuard(const String& modeName);
const ModeMetadata* findModeMetadataById(const String& id);
const ModeMetadata* findModeMetadata(Mode mode);
String nightGuardBlockMessage(const String& modeName);
bool setNightGuardEnabled(bool enabled);
bool setNightGuardMaxBrightness(uint8_t value);
bool setNightGuardBlockFlashingModes(bool enabled);
bool setNightGuardPreferWarmTemperature(bool enabled);
void clearActiveTimer();
bool timerIsActive();
uint32_t getTimerElapsedMs();
uint32_t getTimerRemainingMs();
uint8_t getTimerProgressPercent();
bool startTimer(
  TimerModeType mode,
  TimerActionType action,
  uint16_t minutes,
  const String& sceneId,
  bool bedtimeFade,
  String& error
);
bool cancelActiveTimer(String& message);
void serviceActiveTimer();
TimerLightingState captureTimerLightingState();
void restoreTimerLightingState(const TimerLightingState& state);
void setMode(Mode mode);
Mode modeFromString(const String& key);
String modeKey(Mode mode);
void rememberLoadedScene(const ScenePreset& scene);
void clearLoadedSceneTracking();

uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b);
Rgb kelvinToRgb(uint16_t kelvin);
Rgb wheel(uint8_t pos);
Rgb scaleColor(Rgb color, uint8_t brightness);
Rgb blendColor(Rgb a, Rgb b, float amount);
uint8_t clampByte(float value);
float clamp01(float value);
float smoothstep(float edge0, float edge1, float x);

void applyMasterBrightness();
void clearFrame();
void setFramePixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void setFramePixelColor(uint16_t index, uint32_t color);
void fillFrame(uint8_t r, uint8_t g, uint8_t b);
void fillFrameColor(uint32_t color);
void outputFrameToStrip();
uint8_t applyChannelGain(uint8_t channelValue, uint8_t gain);
uint8_t scaleByMasterBrightness(uint8_t channelValue);
uint8_t gamma8(uint8_t value);
void fillScaled(Rgb color, uint8_t brightness);
void clearStrip();
void renderSolid();
void renderStrobe(uint32_t now);
void renderFlash(uint32_t now);
void renderChase(uint32_t now);
void renderWave(uint32_t now);
void renderSlowWave(uint32_t now);
void renderSlowPulse(uint32_t now);
void renderRainbow(uint32_t now);
void renderDawnBloom(uint32_t now);
void renderMoonTide(uint32_t now);
void renderVelvetAurora(uint32_t now);
void renderLanternDrift(uint32_t now);
void renderRainGlass(uint32_t now);
void initPulse(uint8_t index, uint32_t now);
void initSlowWave(uint8_t index, uint32_t now);
void resetAnimationState();
void setPixelWithBrightness(uint16_t pixel, Rgb color, uint8_t brightness);

#endif
