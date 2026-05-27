#ifndef BEDROOM_LED_CONTROLLER_MANAGEMENT_CONTROLS_H
#define BEDROOM_LED_CONTROLLER_MANAGEMENT_CONTROLS_H

bool requestConfirmed() {
  return server.hasArg("confirm") && server.arg("confirm") == "YES";
}

String buildBackupExportJson() {
  String scenesJson = buildScenesJson();
  String palettesJson = buildPalettesJson();
  String favoritesJson = buildFavoritesJson();
  String scheduleJson = buildScheduleExportJson();
  String diagnosticsJson = buildDiagnosticsJson();

  String json;
  json.reserve(
    720 +
    scenesJson.length() +
    palettesJson.length() +
    favoritesJson.length() +
    scheduleJson.length() +
    diagnosticsJson.length()
  );
  json += R"json({"ok":true,"backupSchema":1,"project":"BedroomLedController","projectVersion":"Phase 4D")json";
  json += R"json(,"boardTarget":")json";
  json += escapeJson(boardProfileName());
  json += R"json(","boardFamily":")json";
  json += boardFamilyName();
  json += R"json(","compileFqbn":")json";
  json += ESP32S3_N16R8_FQBN;
  json += R"json(","compileOptions":")json";
  json += ESP32S3_N16R8_OPTIONS;
  json += R"json(","stripProfile":")json";
  json += escapeJson(ACTIVE_HARDWARE_PROFILE.profileName);
  json += R"json(","configuredLedCount":)json";
  json += getConfiguredLedCount();
  json += R"json(,"importStatus":"Full backup import validates schema; use selective import routes for applied restores")json";
  json += R"json(,"excludes":["wifiCredentials","otaPassword","runtimeTimerState","runtimeTransitionState"])json";
  json += R"json(,"scenes":)json";
  json += scenesJson;
  json += R"json(,"palettes":)json";
  json += palettesJson;
  json += R"json(,"favorites":)json";
  json += favoritesJson;
  json += R"json(,"schedule":)json";
  json += scheduleJson;
  json += R"json(,"diagnostics":)json";
  json += diagnosticsJson;
  json += F("}");
  return json;
}

void sendBackupExportChunk(const String& chunk, uint32_t& payloadBytes) {
  server.sendContent(chunk);
  payloadBytes += chunk.length();
  yield();
}

void streamBackupExportJson() {
  uint32_t heapBefore = beginEndpointHeapMetric("/api/backup/export");
  uint32_t payloadBytes = 0;
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "application/json", "");

  sendBackupExportChunk(
    F("{\"ok\":true,\"backupSchema\":1,\"project\":\"BedroomLedController\",\"projectVersion\":\"Phase 4D\""
      ",\"boardTarget\":\""),
    payloadBytes
  );
  sendBackupExportChunk(String(boardProfileName()), payloadBytes);
  sendBackupExportChunk(F("\",\"boardFamily\":\""), payloadBytes);
  sendBackupExportChunk(String(boardFamilyName()), payloadBytes);
  sendBackupExportChunk(F("\",\"compileFqbn\":\""), payloadBytes);
  sendBackupExportChunk(String(ESP32S3_N16R8_FQBN), payloadBytes);
  sendBackupExportChunk(F("\",\"compileOptions\":\""), payloadBytes);
  sendBackupExportChunk(String(ESP32S3_N16R8_OPTIONS), payloadBytes);
  sendBackupExportChunk(F("\",\"stripProfile\":\""), payloadBytes);
  sendBackupExportChunk(String(ACTIVE_HARDWARE_PROFILE.profileName), payloadBytes);
  sendBackupExportChunk(F("\",\"configuredLedCount\":"), payloadBytes);
  sendBackupExportChunk(String(getConfiguredLedCount()), payloadBytes);
  sendBackupExportChunk(
    F(",\"importStatus\":\"Full backup import validates schema; use selective import routes for applied restores\""
      ",\"excludes\":[\"wifiCredentials\",\"otaPassword\",\"runtimeTimerState\",\"runtimeTransitionState\"]"
      ",\"scenes\":"),
    payloadBytes
  );
  sendScenesJsonDocumentChunks(payloadBytes);
  sendBackupExportChunk(F(",\"palettes\":"), payloadBytes);
  sendPalettesJsonDocumentChunks(payloadBytes);
  sendBackupExportChunk(F(",\"favorites\":"), payloadBytes);
  sendBackupExportChunk(buildFavoritesJson(), payloadBytes);
  sendBackupExportChunk(F(",\"schedule\":"), payloadBytes);
  sendBackupExportChunk(buildScheduleExportJson(), payloadBytes);
  sendBackupExportChunk(
    F(",\"diagnostics\":{\"ok\":true,\"omittedFromBackup\":true,"
      "\"reason\":\"Live diagnostics are available at /api/diagnostics; full diagnostics are omitted from backup export to reduce ESP8266 heap pressure\"}"),
    payloadBytes
  );
  sendBackupExportChunk(F("}"), payloadBytes);

  finishEndpointHeapMetric("/api/backup/export", heapBefore, payloadBytes);
}

void sendManagementJson(const String& message) {
  String json;
  json.reserve(520);
  json += R"json({"ok":true,"message":")json";
  json += escapeJson(message);
  json += R"json(","state":)json";
  json += buildStateJson();
  json += R"json(,"scenes":)json";
  json += buildScenesJson();
  json += R"json(,"palettes":)json";
  json += buildPalettesJson();
  json += R"json(,"favorites":)json";
  json += buildFavoritesJson();
  json += R"json(,"schedule":)json";
  json += buildScheduleJson();
  json += F("}");
  server.send(200, "application/json", json);
}

void handleApiBackupExport() {
  streamBackupExportJson();
}

void handleApiBackupImport() {
  if (!requestConfirmed()) {
    sendJsonError(400, "confirm=YES is required");
    return;
  }
  if (!server.hasArg("json")) {
    sendJsonError(400, "Backup JSON is required");
    return;
  }
  int schemaVersion = 0;
  if (!readJsonInt(server.arg("json"), "backupSchema", schemaVersion) || schemaVersion != 1) {
    sendJsonError(400, "Backup schema is not supported");
    return;
  }

  String json;
  json.reserve(260);
  json += R"json({"ok":true,"applied":false,"message":"Backup validated; use selective import routes for applied restore on the D1 mini"})json";
  server.send(200, "application/json", json);
}

void handleApiReset() {
  if (!server.hasArg("target")) {
    sendJsonError(400, "target is required");
    return;
  }

  String target = server.arg("target");
  target.trim();
  if (target == "scenes-builtins") {
    restoreBuiltInScenes();
    sendManagementJson("Built-in scenes restored");
    return;
  }
  if (target == "palettes-builtins") {
    resetBuiltInPalettes();
    sendManagementJson("Built-in palettes restored");
    return;
  }
  if (target == "favorites") {
    loadDefaultFavorites();
    saveFavorites();
    sendManagementJson("Favorites reset");
    return;
  }
  if (target == "schedule") {
    if (!requestConfirmed()) {
      sendJsonError(400, "confirm=YES is required");
      return;
    }
    String error;
    if (!resetSchedules(error)) {
      sendJsonError(500, error);
      return;
    }
    sendManagementJson("Schedules reset");
    return;
  }
  if (target == "settings") {
    if (!requestConfirmed()) {
      sendJsonError(400, "confirm=YES is required");
      return;
    }
    applyDefaultSettings();
    saveSettings();
    ledsDirty = true;
    sendManagementJson("Settings reset");
    return;
  }
  if (target == "factory") {
    if (!requestConfirmed()) {
      sendJsonError(400, "confirm=YES is required");
      return;
    }
    clearActiveTimer();
    diagnosticTest.active = false;
    diagnosticTest.name = "";
    clearLoadedSceneTracking();
    applyDefaultSettings();
    loadDefaultPalettes();
    loadDefaultScenes();
    loadDefaultFavorites();
    scheduleCount = 0;
    saveSettings();
    savePalettes();
    saveScenes();
    saveFavorites();
    String error;
    resetSchedules(error);
    ensureActivePaletteAvailable();
    ledsDirty = true;
    sendManagementJson("Factory reset complete");
    return;
  }

  sendJsonError(400, "Unknown reset target");
}

void handleApiDocsPage() {
  String page;
  page.reserve(5200);
  page += F("<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>");
  page += F("<title>BedroomLedController API Reference</title><style>");
  page += F("body{margin:0;background:#101214;color:#f4f7f8;font-family:system-ui,-apple-system,Segoe UI,sans-serif}");
  page += F("main{max-width:980px;margin:0 auto;padding:28px 16px}section{border:1px solid rgba(255,255,255,.12);background:#181c20;border-radius:8px;padding:16px;margin:14px 0}");
  page += F("code{background:#20262b;border-radius:6px;padding:2px 5px;color:#ffc680}a{color:#8bd0ff}li{margin:7px 0;color:#c4ccd2}</style></head><body><main>");
  page += F("<h1>BedroomLedController API Reference</h1>");
  page += F("<p><a href='/'>Controls</a> | <a href='/schedule'>Schedule</a> | <a href='/diagnostics'>Diagnostics</a> | <a href='/ota'>OTA</a></p>");
  page += F("<section><h2>State</h2><ul>");
  page += F("<li>ESP32-S3 / N16R8 target: <code>ESP32-S3-N16R8</code>.</li>");
  page += F("<li>Arduino CLI compile target: <code>");
  page += ESP32S3_N16R8_FQBN;
  page += F(":");
  page += ESP32S3_N16R8_OPTIONS;
  page += F("</code>.</li>");
  page += F("<li><code>GET /api/state</code> and <code>GET /state</code> return current lighting state.</li>");
  page += F("<li><code>GET /api/brightness?value=0..255</code>, <code>/api/color?hex=%23RRGGBB</code>, <code>/api/temperature?kelvin=2700..6500</code>, <code>/api/mode?mode=solid</code>.</li>");
  page += F("<li><code>GET /api/animation?strength=0..255</code> updates Animation Strength; <code>/api/intensity?value=0..255</code> remains as a compatibility alias.</li>");
  page += F("</ul></section><section><h2>Scenes</h2><ul>");
  page += F("<li><code>GET /api/scenes</code>, <code>/api/scenes/save?name=Reading</code>, <code>/api/scenes/load?id=reading</code>.</li>");
  page += F("<li><code>GET /api/scenes/rename?id=reading&amp;name=New</code>, <code>/api/scenes/delete?id=reading</code>.</li>");
  page += F("<li><code>GET /api/scenes/duplicate?id=reading</code>, <code>/api/scenes/export?id=reading</code>, <code>/api/scenes/import?json=...</code>, <code>/api/scenes/reset-builtins</code>.</li>");
  page += F("</ul></section><section><h2>Palettes</h2><ul>");
  page += F("<li><code>GET /api/palettes</code>, <code>/api/palettes/select?id=warm-linen&amp;enabled=1</code>, <code>/api/palettes/save</code>, <code>/api/palettes/update</code>, <code>/api/palettes/delete</code>.</li>");
  page += F("<li><code>GET /api/palettes/export?id=warm-linen</code>, <code>/api/palettes/import?json=...</code>, <code>/api/palettes/reset-builtins</code>, <code>/api/palettes/reset</code>.</li>");
  page += F("</ul></section><section><h2>Favorites, Timers, Schedule</h2><ul>");
  page += F("<li><code>GET /api/favorites</code>, <code>/api/favorites/load?id=fav-off</code>, <code>/api/favorites/reset</code>.</li>");
  page += F("<li><code>GET /api/timer/start</code>, <code>/api/timer/cancel</code>, <code>/api/bedtime/start</code>, <code>/api/bedtime/cancel</code>.</li>");
  page += F("<li><code>GET /api/schedule</code>, <code>/api/schedule/save</code>, <code>/api/schedule/delete</code>, <code>/api/schedule/run</code>, <code>/api/schedule/export</code>, <code>/api/schedule/import?json=...</code>, <code>/api/schedule/reset?confirm=YES</code>.</li>");
  page += F("</ul></section><section><h2>Maintenance</h2><ul>");
  page += F("<li><code>GET /api/backup/export</code> returns scenes, palettes, favorites, schedule, and diagnostics without Wi-Fi credentials or OTA password.</li>");
  page += F("<li><code>GET /api/backup/import?confirm=YES&amp;json=...</code> validates schema only on this firmware build.</li>");
  page += F("<li><code>GET /api/reset?target=scenes-builtins</code>, <code>target=palettes-builtins</code>, <code>target=favorites</code>, <code>target=schedule&amp;confirm=YES</code>, <code>target=factory&amp;confirm=YES</code>.</li>");
  page += F("</ul></section></main></body></html>");
  server.send(200, "text/html", page);
}

#endif
