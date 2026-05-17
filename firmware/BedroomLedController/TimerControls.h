#ifndef BEDROOM_LED_CONTROLLER_TIMER_CONTROLS_H
#define BEDROOM_LED_CONTROLLER_TIMER_CONTROLS_H

String timerModeKey(TimerModeType mode) {
  switch (mode) {
    case TIMER_MODE_DELAY_THEN_ACTION: return "delay";
    case TIMER_MODE_FADE_TO_ACTION: return "fade";
    case TIMER_MODE_NONE:
    default: return "none";
  }
}

String timerActionKey(TimerActionType action) {
  switch (action) {
    case TIMER_ACTION_OFF: return "off";
    case TIMER_ACTION_WARM_DIM: return "warmDimNow";
    case TIMER_ACTION_SCENE: return "scene";
    case TIMER_ACTION_PANIC_WARM: return "panicWarm";
    case TIMER_ACTION_NONE:
    default: return "none";
  }
}

void clearActiveTimer() {
  activeTimer.active = false;
  activeTimer.mode = TIMER_MODE_NONE;
  activeTimer.action = TIMER_ACTION_NONE;
  activeTimer.targetSceneId = "";
  activeTimer.startedMs = 0;
  activeTimer.durationMs = 0;
  activeTimer.fadeDurationMs = 0;
  activeTimer.fadeActive = false;
  activeTimer.completed = false;
  activeTimer.cancelled = false;
  activeTimer.bedtimeFadeActive = false;
  activeTimer.lastFadeStepMs = 0;
}

bool timerIsActive() {
  return activeTimer.active;
}

uint32_t getTimerElapsedMs() {
  if (!activeTimer.active) {
    return 0;
  }
  return (uint32_t)(millis() - activeTimer.startedMs);
}

uint32_t getTimerRemainingMs() {
  if (!activeTimer.active) {
    return 0;
  }
  uint32_t elapsed = getTimerElapsedMs();
  if (elapsed >= activeTimer.durationMs) {
    return 0;
  }
  return activeTimer.durationMs - elapsed;
}

uint8_t getTimerProgressPercent() {
  if (!activeTimer.active || activeTimer.durationMs == 0) {
    return 0;
  }
  uint32_t elapsed = getTimerElapsedMs();
  if (elapsed >= activeTimer.durationMs) {
    return 100;
  }
  return static_cast<uint8_t>((elapsed * 100UL) / activeTimer.durationMs);
}

TimerLightingState captureTimerLightingState() {
  TimerLightingState state;
  state.mode = settings.mode;
  state.color = settings.color;
  state.whiteKelvin = settings.whiteKelvin;
  state.usingWhiteTemperature = settings.usingWhiteTemperature;
  state.masterBrightness = settings.masterBrightness;
  state.paletteEnabled = settings.paletteEnabled;
  state.activePaletteId = settings.activePaletteId;
  return state;
}

void restoreTimerLightingState(const TimerLightingState& state) {
  settings.mode = state.mode;
  settings.color = state.color;
  settings.whiteKelvin = state.whiteKelvin;
  settings.usingWhiteTemperature = state.usingWhiteTemperature;
  settings.masterBrightness = state.masterBrightness;
  settings.paletteEnabled = state.paletteEnabled;
  settings.activePaletteId = state.activePaletteId;
  resetAnimationState();
  ledsDirty = true;
}

uint8_t interpolateByte(uint8_t start, uint8_t target, float amount) {
  return clampByte(start + (target - start) * amount);
}

uint16_t interpolateKelvin(uint16_t start, uint16_t target, float amount) {
  return static_cast<uint16_t>(constrain(start + (target - start) * amount, 2700.0f, 6500.0f));
}

bool buildTimerTargetState(TimerActionType action, const String& sceneId, TimerLightingState& targetState, String& error) {
  targetState = captureTimerLightingState();

  if (action == TIMER_ACTION_OFF) {
    targetState.mode = MODE_SOLID;
    targetState.color = {0, 0, 0};
    targetState.whiteKelvin = 0;
    targetState.usingWhiteTemperature = false;
    targetState.masterBrightness = 0;
    return true;
  }

  if (action == TIMER_ACTION_WARM_DIM || action == TIMER_ACTION_PANIC_WARM) {
    targetState.mode = MODE_SOLID;
    targetState.whiteKelvin = 2700;
    targetState.usingWhiteTemperature = true;
    targetState.color = kelvinToRgb(2700);
    targetState.masterBrightness = WARM_DIM_BRIGHTNESS;
    return true;
  }

  if (action == TIMER_ACTION_SCENE) {
    int sceneIndex = findSceneById(sceneId);
    if (sceneIndex < 0) {
      error = "Scene not found";
      return false;
    }
    if (isModeBlockedByNightGuard(scenes[sceneIndex].mode)) {
      error = "Night Guard blocks this timer scene";
      return false;
    }
    targetState.mode = modeFromString(scenes[sceneIndex].mode);
    targetState.color = {scenes[sceneIndex].red, scenes[sceneIndex].green, scenes[sceneIndex].blue};
    targetState.whiteKelvin = scenes[sceneIndex].whiteKelvin;
    targetState.usingWhiteTemperature = scenes[sceneIndex].usingWhiteTemperature;
    targetState.masterBrightness = scenes[sceneIndex].masterBrightness;
    targetState.paletteEnabled = scenes[sceneIndex].paletteEnabled;
    targetState.activePaletteId = scenes[sceneIndex].paletteId;
    return true;
  }

  error = "Unknown timer action";
  return false;
}

void applyFadeStep() {
  if (!activeTimer.active || !activeTimer.fadeActive) {
    return;
  }

  uint32_t elapsed = getTimerElapsedMs();
  float progress = activeTimer.fadeDurationMs == 0
    ? 1.0f
    : clamp01(static_cast<float>(elapsed) / static_cast<float>(activeTimer.fadeDurationMs));

  settings.mode = MODE_SOLID;
  settings.masterBrightness = interpolateByte(
    activeTimer.startState.masterBrightness,
    activeTimer.targetState.masterBrightness,
    progress
  );

  if (activeTimer.targetState.usingWhiteTemperature) {
    settings.whiteKelvin = interpolateKelvin(
      activeTimer.startState.whiteKelvin == 0 ? 3000 : activeTimer.startState.whiteKelvin,
      activeTimer.targetState.whiteKelvin,
      progress
    );
    settings.color = kelvinToRgb(settings.whiteKelvin);
    settings.usingWhiteTemperature = true;
  } else {
    settings.color = {
      interpolateByte(activeTimer.startState.color.r, activeTimer.targetState.color.r, progress),
      interpolateByte(activeTimer.startState.color.g, activeTimer.targetState.color.g, progress),
      interpolateByte(activeTimer.startState.color.b, activeTimer.targetState.color.b, progress)
    };
    settings.whiteKelvin = 0;
    settings.usingWhiteTemperature = false;
  }

  activeTimer.lastFadeStepMs = millis();
  ledsDirty = true;
  // fade step does not save settings; final action or explicit default changes own persistence.
  bool settingsDirtySnapshot = settingsDirty;
  settingsDirty = settingsDirtySnapshot;
}

void completeActiveTimer() {
  if (!activeTimer.active) {
    return;
  }

  TimerActionType action = activeTimer.action;
  String sceneId = activeTimer.targetSceneId;
  bool bedtime = activeTimer.bedtimeFadeActive;
  bool timerFadeOwnsFinalApply = activeTimer.fadeActive;
  String error;
  bool ok = false;

  transitionTimerCompletionActive = true;
  bool previousSuppression = transitionsSuppressed;
  // timer fade owns final apply; avoid stacking the smooth transition engine on top of it.
  transitionsSuppressed = previousSuppression || timerFadeOwnsFinalApply;

  if (action == TIMER_ACTION_OFF) {
    ok = runActionByName("off", error);
  } else if (action == TIMER_ACTION_WARM_DIM) {
    ok = runActionByName("warmDimNow", error);
  } else if (action == TIMER_ACTION_PANIC_WARM) {
    ok = runActionByName("panicWarm", error);
  } else if (action == TIMER_ACTION_SCENE) {
    ok = loadSceneById(activeTimer.targetSceneId, error);
  }

  transitionsSuppressed = previousSuppression;
  transitionTimerCompletionActive = false;

  activeTimer.completed = ok;
  lastTimerStatus = ok
    ? (bedtime ? "bedtime fade completed" : "timer completed")
    : (String("timer failed: ") + error);
  clearActiveTimer();
}

bool validateTimerStart(TimerModeType mode, TimerActionType action, uint16_t minutes, const String& sceneId, String& error) {
  if (minutes == 0 || minutes > TIMER_MAX_MINUTES) {
    error = "Invalid timer minutes";
    return false;
  }
  if (mode != TIMER_MODE_DELAY_THEN_ACTION && mode != TIMER_MODE_FADE_TO_ACTION) {
    error = "Unknown timer mode";
    return false;
  }
  if (action == TIMER_ACTION_NONE) {
    error = "Unknown timer action";
    return false;
  }
  if (action == TIMER_ACTION_SCENE) {
    if (sceneId.length() == 0) {
      error = "Scene id is required";
      return false;
    }
    if (findSceneById(sceneId) < 0) {
      error = "Scene not found";
      return false;
    }
  }
  return true;
}

bool startTimer(
  TimerModeType mode,
  TimerActionType action,
  uint16_t minutes,
  const String& sceneId,
  bool bedtimeFade,
  String& error
) {
  if (!validateTimerStart(mode, action, minutes, sceneId, error)) {
    return false;
  }

  TimerLightingState targetState;
  if (!buildTimerTargetState(action, sceneId, targetState, error)) {
    return false;
  }

  diagnosticTest.active = false;
  diagnosticTest.name = "";

  activeTimer.restoreState = captureTimerLightingState();
  activeTimer.startState = activeTimer.restoreState;
  activeTimer.targetState = targetState;
  activeTimer.active = true;
  activeTimer.mode = mode;
  activeTimer.action = action;
  activeTimer.targetSceneId = sceneId;
  activeTimer.startedMs = millis();
  activeTimer.durationMs = static_cast<uint32_t>(minutes) * 60000UL;
  activeTimer.fadeDurationMs = activeTimer.durationMs;
  activeTimer.fadeActive = mode == TIMER_MODE_FADE_TO_ACTION;
  activeTimer.completed = false;
  activeTimer.cancelled = false;
  activeTimer.bedtimeFadeActive = bedtimeFade;
  activeTimer.lastFadeStepMs = 0;
  lastTimerStatus = bedtimeFade ? "bedtime fade started" : "timer started";

  if (activeTimer.fadeActive) {
    applyFadeStep();
  }
  return true;
}

bool cancelActiveTimer(String& message) {
  if (!activeTimer.active) {
    message = "No active timer";
    lastTimerStatus = "no active timer";
    return false;
  }

  if (activeTimer.fadeActive) {
    restoreTimerLightingState(activeTimer.restoreState);
  }
  activeTimer.cancelled = true;
  clearActiveTimer();
  message = "Timer cancelled";
  lastTimerStatus = "timer cancelled";
  return true;
}

void serviceActiveTimer() {
  if (!activeTimer.active) {
    return;
  }

  uint32_t elapsed = getTimerElapsedMs();
  if (activeTimer.fadeActive &&
      (elapsed >= activeTimer.durationMs ||
       (uint32_t)(millis() - activeTimer.lastFadeStepMs) >= TIMER_FADE_STEP_MS)) {
    applyFadeStep();
  }

  if (elapsed >= activeTimer.durationMs) {
    completeActiveTimer();
  }
}

TimerModeType timerModeFromRequest(const String& value) {
  String mode = value;
  mode.trim();
  mode.toLowerCase();
  if (mode == "delay" || mode == "wait") {
    return TIMER_MODE_DELAY_THEN_ACTION;
  }
  if (mode == "fade" || mode == "fadetoaction") {
    return TIMER_MODE_FADE_TO_ACTION;
  }
  return TIMER_MODE_NONE;
}

TimerActionType timerActionFromRequest(const String& value) {
  String action = normalizeActionToken(value);
  if (action == "off") {
    return TIMER_ACTION_OFF;
  }
  if (action == "warmdimnow" || action == "warmdim") {
    return TIMER_ACTION_WARM_DIM;
  }
  if (action == "panicwarm") {
    return TIMER_ACTION_PANIC_WARM;
  }
  if (action == "scene") {
    return TIMER_ACTION_SCENE;
  }
  return TIMER_ACTION_NONE;
}

TimerActionType bedtimeActionFromTarget(const String& value, String& sceneId) {
  String target = value;
  target.trim();
  if (target == "off") {
    return TIMER_ACTION_OFF;
  }
  if (target == "warmDim" || target == "warmDimNow" || target == "panicWarm") {
    return TIMER_ACTION_WARM_DIM;
  }
  if (target == "sleep") {
    sceneId = "sleep";
    return TIMER_ACTION_SCENE;
  }
  if (target.startsWith("scene:")) {
    sceneId = target.substring(6);
    return TIMER_ACTION_SCENE;
  }
  return TIMER_ACTION_NONE;
}

String buildTimerJson() {
  String json;
  json.reserve(520);
  json += R"json({"ok":true,"active":)json";
  json += boolJson(activeTimer.active);
  json += R"json(,"mode":")json";
  json += timerModeKey(activeTimer.mode);
  json += R"json(","action":")json";
  json += timerActionKey(activeTimer.action);
  json += R"json(","targetSceneId":")json";
  json += escapeJson(activeTimer.targetSceneId);
  json += R"json(","durationMs":)json";
  json += activeTimer.durationMs;
  json += R"json(,"elapsedMs":)json";
  json += getTimerElapsedMs();
  json += R"json(,"remainingMs":)json";
  json += getTimerRemainingMs();
  json += R"json(,"progressPercent":)json";
  json += getTimerProgressPercent();
  json += R"json(,"fadeActive":)json";
  json += boolJson(activeTimer.fadeActive);
  json += R"json(,"bedtimeFadeActive":)json";
  json += boolJson(activeTimer.bedtimeFadeActive);
  json += R"json(,"lastTimerStatus":")json";
  json += escapeJson(lastTimerStatus);
  json += R"json(","bedtimeFadeDefaultMinutes":)json";
  json += settings.bedtimeFadeDefaultMinutes;
  json += R"json(,"bedtimeFadeDefaultTarget":")json";
  json += escapeJson(settings.bedtimeFadeDefaultTarget);
  json += R"json(","bedtimeFadeTurnOffAtEnd":)json";
  json += boolJson(settings.bedtimeFadeTurnOffAtEnd);
  json += F("}");
  return json;
}

bool parseTimerMinutes(uint16_t& minutes, const String& fallback) {
  String raw = server.hasArg("minutes") ? server.arg("minutes") : fallback;
  int parsed;
  if (!parseRequestInt(raw, parsed)) {
    return false;
  }
  if (parsed < 1 || parsed > TIMER_MAX_MINUTES) {
    return false;
  }
  minutes = static_cast<uint16_t>(parsed);
  return true;
}

void handleApiTimer() {
  server.send(200, "application/json", buildTimerJson());
}

void handleApiTimerStart() {
  uint8_t beforeBrightness = settings.masterBrightness;
  Mode beforeMode = settings.mode;

  uint16_t minutes;
  if (!parseTimerMinutes(minutes, String(TIMER_DEFAULT_MINUTES))) {
    sendJsonError(400, "Invalid timer minutes");
    return;
  }

  TimerModeType mode = server.hasArg("mode")
    ? timerModeFromRequest(server.arg("mode"))
    : TIMER_MODE_DELAY_THEN_ACTION;
  if (mode == TIMER_MODE_NONE) {
    sendJsonError(400, "Unknown timer mode");
    return;
  }

  TimerActionType action = server.hasArg("action")
    ? timerActionFromRequest(server.arg("action"))
    : TIMER_ACTION_OFF;
  if (action == TIMER_ACTION_NONE) {
    sendJsonError(400, "Unknown timer action");
    return;
  }

  String sceneId = server.hasArg("sceneId") ? server.arg("sceneId") : "";
  String error;
  if (!startTimer(mode, action, minutes, sceneId, false, error)) {
    sendJsonError(400, error);
    return;
  }

  String actionKey = timerActionKey(action);
  recordMutation("/api/timer/start", actionKey.c_str(), beforeBrightness, beforeMode);
  String json;
  json.reserve(760);
  json += F("{\"ok\":true,\"message\":\"Timer started\",\"timer\":");
  json += buildTimerJson();
  json += F(",\"state\":");
  json += buildStateJson();
  json += F("}");
  server.send(200, "application/json", json);
}

void handleApiTimerCancel() {
  uint8_t beforeBrightness = settings.masterBrightness;
  Mode beforeMode = settings.mode;
  String message;
  bool ok = cancelActiveTimer(message);
  if (ok) {
    recordMutation("/api/timer/cancel", "cancel", beforeBrightness, beforeMode);
  }
  String json;
  json.reserve(520);
  json += F("{\"ok\":");
  json += boolJson(ok);
  json += F(",\"message\":\"");
  json += escapeJson(message);
  json += F("\",\"timer\":");
  json += buildTimerJson();
  json += F(",\"state\":");
  json += buildStateJson();
  json += F("}");
  server.send(ok ? 200 : 404, "application/json", json);
}

void handleApiBedtimeStart() {
  uint8_t beforeBrightness = settings.masterBrightness;
  Mode beforeMode = settings.mode;

  uint16_t minutes;
  if (!parseTimerMinutes(minutes, String(settings.bedtimeFadeDefaultMinutes))) {
    sendJsonError(400, "Invalid timer minutes");
    return;
  }

  String target = server.hasArg("target") ? server.arg("target") : settings.bedtimeFadeDefaultTarget;
  String sceneId;
  TimerActionType action = bedtimeActionFromTarget(target, sceneId);
  if (action == TIMER_ACTION_NONE) {
    sendJsonError(400, "Unknown timer action");
    return;
  }

  bool persistDefaults = server.hasArg("saveDefault");
  String error;
  if (!startTimer(TIMER_MODE_FADE_TO_ACTION, action, minutes, sceneId, true, error)) {
    sendJsonError(400, error);
    return;
  }

  if (persistDefaults) {
    settings.bedtimeFadeDefaultMinutes = minutes;
    settings.bedtimeFadeDefaultTarget = target;
    markSettingsDirty();
  }

  String actionKey = timerActionKey(action);
  recordMutation("/api/bedtime/start", actionKey.c_str(), beforeBrightness, beforeMode);
  String json;
  json.reserve(780);
  json += F("{\"ok\":true,\"message\":\"Bedtime fade started\",\"timer\":");
  json += buildTimerJson();
  json += F(",\"state\":");
  json += buildStateJson();
  json += F("}");
  server.send(200, "application/json", json);
}

void handleApiBedtimeCancel() {
  handleApiTimerCancel();
}

#endif
