#ifndef BEDROOM_LED_CONTROLLER_DIAGNOSTIC_TESTS_H
#define BEDROOM_LED_CONTROLLER_DIAGNOSTIC_TESTS_H

bool isDiagnosticTestName(const String& testName) {
  return testName == "red" ||
         testName == "green" ||
         testName == "blue" ||
         testName == "whiteLow" ||
         testName == "pixelWalk" ||
         testName == "alternating" ||
         testName == "clear";
}

void startDiagnosticTest(const String& testName) {
  cancelTransition(false);
  if (!diagnosticTest.active) {
    previousSettingsBeforeTest = settings;
    hasPreviousSettingsBeforeTest = true;
  }

  diagnosticTest.active = true;
  diagnosticTest.name = testName;
  diagnosticTest.startedMs = millis();
  diagnosticTest.lastStepMs = diagnosticTest.startedMs;
  diagnosticTest.pixelIndex = 0;
  settings.masterBrightness = 255;
  ledsDirty = true;
}

void restorePreviousLightingState() {
  diagnosticTest.active = false;
  diagnosticTest.name = "";
  diagnosticTest.startedMs = 0;
  diagnosticTest.lastStepMs = 0;
  diagnosticTest.pixelIndex = 0;

  if (hasPreviousSettingsBeforeTest) {
    settings = previousSettingsBeforeTest;
    hasPreviousSettingsBeforeTest = false;
    resetAnimationState();
  }

  ledsDirty = true;
}

void renderDiagnosticTestToFrame(uint32_t now) {
  clearFrame();

  if (diagnosticTest.name == "red") {
    fillFrame(DIAGNOSTIC_TEST_BRIGHTNESS, 0, 0);
    return;
  }
  if (diagnosticTest.name == "green") {
    fillFrame(0, DIAGNOSTIC_TEST_BRIGHTNESS, 0);
    return;
  }
  if (diagnosticTest.name == "blue") {
    fillFrame(0, 0, DIAGNOSTIC_TEST_BRIGHTNESS);
    return;
  }
  if (diagnosticTest.name == "whiteLow") {
    fillFrame(DIAGNOSTIC_TEST_BRIGHTNESS, DIAGNOSTIC_TEST_BRIGHTNESS, DIAGNOSTIC_TEST_BRIGHTNESS);
    return;
  }
  if (diagnosticTest.name == "alternating") {
    uint8_t offset = static_cast<uint8_t>((now / 500UL) & 0x01);
    for (uint16_t i = 0; i < LED_COUNT; i++) {
      if (((i + offset) & 0x01) == 0) {
        setFramePixel(i, DIAGNOSTIC_TEST_BRIGHTNESS, 0, 0);
      } else {
        setFramePixel(i, 0, 0, DIAGNOSTIC_TEST_BRIGHTNESS);
      }
    }
    return;
  }
  if (diagnosticTest.name == "pixelWalk") {
    if (millis() - diagnosticTest.lastStepMs >= DIAGNOSTIC_PIXEL_WALK_INTERVAL_MS) {
      diagnosticTest.pixelIndex = (diagnosticTest.pixelIndex + 1) % LED_COUNT;
      diagnosticTest.lastStepMs = millis();
    }
    setFramePixel(diagnosticTest.pixelIndex, DIAGNOSTIC_TEST_BRIGHTNESS, DIAGNOSTIC_TEST_BRIGHTNESS, DIAGNOSTIC_TEST_BRIGHTNESS);
    return;
  }
}

void handleApiTest() {
  if (!server.hasArg("name")) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"Missing test name\"}");
    return;
  }

  String testName = server.arg("name");
  if (testName == "restore") {
    restorePreviousLightingState();
    server.send(200, "application/json", "{\"ok\":true,\"testActive\":false,\"testName\":\"restore\"}");
    return;
  }

  if (!isDiagnosticTestName(testName)) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"Unknown test name\"}");
    return;
  }

  startDiagnosticTest(testName);

  String json;
  json.reserve(90);
  json += F("{\"ok\":true,\"testActive\":true,\"testName\":\"");
  json += escapeJson(diagnosticTest.name);
  json += F("\"}");
  server.send(200, "application/json", json);
}

#endif
