#ifndef BEDROOM_LED_CONTROLLER_TRANSITION_CONTROLS_H
#define BEDROOM_LED_CONTROLLER_TRANSITION_CONTROLS_H

String normalizeTransitionStyle(String style) {
  style.trim();
  style.toLowerCase();
  if (style == "fade") {
    return "fade";
  }
  return "fade";
}

bool isSupportedTransitionStyle(const String& style) {
  String normalized = normalizeTransitionStyle(style);
  return normalized == style || style == "Fade" || style == "FADE";
}

bool isSupportedTransitionDuration(uint16_t durationMs) {
  return durationMs == 0 ||
         durationMs == 500 ||
         durationMs == 1500 ||
         durationMs == 3000 ||
         durationMs == 5000;
}

uint8_t configuredEffectiveBrightness() {
  if (settings.nightGuardEnabled) {
    return min(settings.masterBrightness, settings.nightGuardMaxBrightness);
  }
  return settings.masterBrightness;
}

uint32_t getTransitionElapsedMs() {
  if (!transitionState.active) {
    return 0;
  }
  return (uint32_t)(millis() - transitionState.startedMs);
}

uint8_t getTransitionProgressPercent() {
  if (!transitionState.active || transitionState.durationMs == 0) {
    return transitionState.active ? 100 : 0;
  }
  uint32_t elapsed = getTransitionElapsedMs();
  if (elapsed >= transitionState.durationMs) {
    return 100;
  }
  return static_cast<uint8_t>((elapsed * 100UL) / transitionState.durationMs);
}

uint8_t transitionProgressAmount() {
  if (!transitionState.active || transitionState.durationMs == 0) {
    return 255;
  }
  uint32_t elapsed = getTransitionElapsedMs();
  if (elapsed >= transitionState.durationMs) {
    return 255;
  }
  return static_cast<uint8_t>((elapsed * 255UL) / transitionState.durationMs);
}

uint8_t transitionEffectiveBrightness() {
  if (!transitionState.active) {
    return configuredEffectiveBrightness();
  }
  return lerp8(
    transitionState.startEffectiveBrightness,
    transitionState.targetEffectiveBrightness,
    transitionProgressAmount()
  );
}

void copyFrame(const RgbPixel* source, RgbPixel* destination) {
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    destination[i] = source[i];
  }
}

uint8_t lerp8(uint8_t start, uint8_t target, uint8_t amount) {
  return static_cast<uint8_t>(start + ((static_cast<int16_t>(target) - static_cast<int16_t>(start)) * amount) / 255);
}

void blendFrames(const RgbPixel* startFrame, const RgbPixel* endFrame, RgbPixel* outputFrame, uint8_t amount) {
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    outputFrame[i].r = lerp8(startFrame[i].r, endFrame[i].r, amount);
    outputFrame[i].g = lerp8(startFrame[i].g, endFrame[i].g, amount);
    outputFrame[i].b = lerp8(startFrame[i].b, endFrame[i].b, amount);
  }
}

void cancelTransition(bool renderTargetFrame) {
  transitionState.active = false;
  transitionState.targetStateApplied = false;
  transitionState.startedMs = 0;
  transitionState.durationMs = 0;
  if (renderTargetFrame) {
    renderCurrentModeToFrame(millis());
  }
  ledsDirty = true;
}

void beginTransitionToCurrentState() {
  if (transitionsSuppressed) {
    cancelTransition(true);
    return;
  }

  if (activeTimer.active && !transitionTimerCompletionActive) {
    clearActiveTimer();
    lastTimerStatus = "timer cancelled by manual lighting change";
  }

  copyFrame(frameBuffer, previousFrame);

  if (diagnosticTest.active) {
    diagnosticTest.active = false;
    diagnosticTest.name = "";
  }

  if (!settings.transitionsEnabled ||
      settings.transitionDurationMs == 0 ||
      normalizeTransitionStyle(settings.transitionStyle) != "fade") {
    cancelTransition(true);
    return;
  }

  uint32_t now = millis();
  renderCurrentModeToFrame(now);
  copyFrame(frameBuffer, targetFrame);

  transitionState.active = true;
  transitionState.style = "fade";
  transitionState.startedMs = now;
  transitionState.durationMs = settings.transitionDurationMs;
  transitionState.targetStateApplied = true;
  transitionState.startEffectiveBrightness = lastOutputEffectiveBrightness;
  transitionState.targetEffectiveBrightness = configuredEffectiveBrightness();
  ledsDirty = true;
}

bool renderTransitionFrame(uint32_t now) {
  if (!transitionState.active) {
    return false;
  }

  if (getTransitionElapsedMs() >= transitionState.durationMs) {
    transitionState.active = false;
    transitionState.targetStateApplied = true;
    renderCurrentModeToFrame(now);
    return true;
  }

  renderCurrentModeToFrame(now);
  copyFrame(frameBuffer, targetFrame);
  blendFrames(previousFrame, targetFrame, frameBuffer, transitionProgressAmount());
  // transition progress does not save settings; only requested state/settings changes do.
  return true;
}

String buildTransitionsJson() {
  String json;
  json.reserve(320);
  json += R"json({"ok":true,"enabled":)json";
  json += boolJson(settings.transitionsEnabled);
  json += R"json(,"style":")json";
  json += escapeJson(normalizeTransitionStyle(settings.transitionStyle));
  json += R"json(","durationMs":)json";
  json += settings.transitionDurationMs;
  json += R"json(,"active":)json";
  json += boolJson(transitionState.active);
  json += R"json(,"progressPercent":)json";
  json += getTransitionProgressPercent();
  json += R"json(,"elapsedMs":)json";
  json += getTransitionElapsedMs();
  json += R"json(,"targetStateApplied":)json";
  json += boolJson(transitionState.targetStateApplied);
  json += R"json(,"bufferBytes":)json";
  json += static_cast<uint16_t>(sizeof(previousFrame) + sizeof(targetFrame) + sizeof(frameBuffer));
  json += F("}");
  return json;
}

void handleApiTransitions() {
  server.send(200, "application/json", buildTransitionsJson());
}

void handleApiTransitionsSet() {
  if (server.hasArg("enabled")) {
    bool enabled;
    if (!parseRequestBool(server.arg("enabled"), enabled)) {
      sendJsonError(400, "Invalid transitions enabled value");
      return;
    }
    settings.transitionsEnabled = enabled;
  }

  if (server.hasArg("durationMs")) {
    int duration;
    if (!parseIntArg("durationMs", duration) ||
        duration < 0 ||
        duration > 5000 ||
        !isSupportedTransitionDuration(static_cast<uint16_t>(duration))) {
      sendJsonError(400, "Invalid transition duration");
      return;
    }
    settings.transitionDurationMs = static_cast<uint16_t>(duration);
  }

  if (server.hasArg("style")) {
    String style = server.arg("style");
    style.trim();
    style.toLowerCase();
    if (!isSupportedTransitionStyle(style)) {
      sendJsonError(400, "Unknown transition style");
      return;
    }
    settings.transitionStyle = normalizeTransitionStyle(style);
  }

  markSettingsDirty();

  String json;
  json.reserve(620);
  json += F("{\"ok\":true,\"message\":\"Transitions updated\",\"transitions\":");
  json += buildTransitionsJson();
  json += F(",\"state\":");
  json += buildStateJson();
  json += F("}");
  server.send(200, "application/json", json);
}

#endif
