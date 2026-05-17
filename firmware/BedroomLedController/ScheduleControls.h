#ifndef BEDROOM_LED_CONTROLLER_SCHEDULE_CONTROLS_H
#define BEDROOM_LED_CONTROLLER_SCHEDULE_CONTROLS_H

const uint32_t VALID_EPOCH_MINIMUM = 1609459200UL;

String cleanScheduleName(const String& value) {
  String name = value;
  name.trim();
  name.replace("\"", "'");
  while (name.indexOf("  ") >= 0) {
    name.replace("  ", " ");
  }
  if (name.length() > MAX_SCHEDULE_NAME_LENGTH) {
    name.remove(MAX_SCHEDULE_NAME_LENGTH);
  }
  return name;
}

String safeScheduleIdToken(const String& value) {
  String clean;
  clean.reserve(value.length());
  bool previousDash = false;
  for (uint16_t i = 0; i < value.length(); i++) {
    char c = value[i];
    if (isalnum(c)) {
      clean += static_cast<char>(tolower(c));
      previousDash = false;
    } else if ((c == '-' || c == '_' || c == ' ') && !previousDash && clean.length() > 0) {
      clean += '-';
      previousDash = true;
    }
  }
  while (clean.endsWith("-")) {
    clean.remove(clean.length() - 1);
  }
  if (clean.length() > MAX_SCHEDULE_ID_LENGTH) {
    clean.remove(MAX_SCHEDULE_ID_LENGTH);
  }
  return clean;
}

int findScheduleById(const String& id) {
  for (uint8_t i = 0; i < scheduleCount; i++) {
    if (schedules[i].id == id) {
      return i;
    }
  }
  return -1;
}

String makeScheduleId(const String& name) {
  String base = safeScheduleIdToken(name);
  if (base.length() == 0) {
    base = "schedule";
  }

  String candidate = base;
  uint8_t suffix = 2;
  while (findScheduleById(candidate) >= 0) {
    candidate = base;
    candidate += '-';
    candidate += suffix++;
  }
  return candidate;
}

String normalizedScheduleType(const String& value) {
  String type = value;
  type.trim();
  type.toLowerCase();
  if (type == "action") {
    return "action";
  }
  return "scene";
}

bool isScheduleActionKnown(const String& actionName) {
  String token = normalizeActionToken(actionName);
  return token == "off" || token == "warmdimnow" || token == "warmdim" || token == "panicwarm";
}

TimerActionType scheduleActionToTimerAction(const String& actionName) {
  String token = normalizeActionToken(actionName);
  if (token == "off") {
    return TIMER_ACTION_OFF;
  }
  if (token == "panicwarm") {
    return TIMER_ACTION_PANIC_WARM;
  }
  if (token == "warmdimnow" || token == "warmdim") {
    return TIMER_ACTION_WARM_DIM;
  }
  return TIMER_ACTION_NONE;
}

String canonicalScheduleAction(const String& actionName) {
  String token = normalizeActionToken(actionName);
  if (token == "off") {
    return "off";
  }
  if (token == "panicwarm") {
    return "panicWarm";
  }
  if (token == "warmdimnow" || token == "warmdim") {
    return "warmDimNow";
  }
  return "";
}

String targetDisplayName(const ScheduleEntry& entry) {
  if (entry.targetType == "scene") {
    int index = findSceneById(entry.targetId);
    return index >= 0 ? scenes[index].name : entry.targetId;
  }
  if (entry.targetId == "off") {
    return "Off";
  }
  if (entry.targetId == "panicWarm") {
    return "Panic Warm";
  }
  return "Warm Dim Now";
}

bool validateScheduleEntry(const ScheduleEntry& entry, String& error) {
  if (entry.id.length() == 0 || entry.id.length() > MAX_SCHEDULE_ID_LENGTH) {
    error = "Schedule id is invalid";
    return false;
  }
  if (entry.name.length() == 0 || entry.name.length() > MAX_SCHEDULE_NAME_LENGTH) {
    error = "Schedule name is required";
    return false;
  }
  if (entry.daysMask == 0 || entry.daysMask > 0x7F) {
    error = "Select at least one day";
    return false;
  }
  if (entry.minuteOfDay > 1439) {
    error = "Schedule time is invalid";
    return false;
  }
  if (entry.targetType != "scene" && entry.targetType != "action") {
    error = "Schedule target type is invalid";
    return false;
  }
  if (entry.targetId.length() == 0 || entry.targetId.length() > MAX_SCHEDULE_TARGET_LENGTH) {
    error = "Schedule target is required";
    return false;
  }
  if (entry.fadeSeconds > TIMER_MAX_MINUTES * 60U) {
    error = "Schedule fade is too long";
    return false;
  }
  if (entry.targetType == "scene" && findSceneById(entry.targetId) < 0) {
    error = "Scene not found";
    return false;
  }
  if (entry.targetType == "action" && !isScheduleActionKnown(entry.targetId)) {
    error = "Unknown action";
    return false;
  }
  return true;
}

bool isTimeSynced() {
  time_t epoch = time(nullptr);
  if (epoch > static_cast<time_t>(VALID_EPOCH_MINIMUM)) {
    if (!timeSync.timeSynced) {
      timeSync.lastSyncSuccessEpoch = static_cast<uint32_t>(epoch);
      timeSync.lastSyncSuccessMs = millis();
      timeSync.timeStatus = "synced";
    }
    timeSync.timeSynced = true;
    return true;
  }
  timeSync.timeSynced = false;
  return false;
}

void beginTimeSync(bool manual) {
  if (WiFi.status() != WL_CONNECTED) {
    timeSync.timeSynced = false;
    timeSync.lastSyncFailureMs = millis();
    timeSync.timeStatus = manual ? "manual sync unavailable; Wi-Fi offline" : "waiting for Wi-Fi before time sync";
    return;
  }

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  timeSync.syncRequested = true;
  timeSync.lastSyncAttemptMs = millis();
  timeSync.timeStatus = manual ? "manual sync requested" : "sync requested";
}

void serviceTimeSync() {
  const bool synced = isTimeSynced();
  const uint32_t nowMs = millis();

  if (synced) {
    if (timeSync.lastSyncSuccessMs == 0 ||
        (uint32_t)(nowMs - timeSync.lastSyncSuccessMs) >= TIME_SYNC_REFRESH_MS) {
      beginTimeSync(false);
    }
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    timeSync.timeStatus = "not synced; Wi-Fi offline";
    return;
  }

  if (timeSync.lastSyncAttemptMs == 0 ||
      (uint32_t)(nowMs - timeSync.lastSyncAttemptMs) >= TIME_SYNC_RETRY_MS) {
    beginTimeSync(false);
  } else if (timeSync.timeStatus.length() == 0) {
    timeSync.timeStatus = "sync pending";
  }
}

time_t currentLocalEpoch() {
  time_t epoch = time(nullptr);
  return epoch + static_cast<time_t>(settings.localUtcOffsetMinutes) * 60;
}

uint16_t currentLocalDayIndex() {
  return static_cast<uint16_t>(currentLocalEpoch() / 86400L);
}

uint8_t currentLocalWeekday() {
  time_t localEpoch = currentLocalEpoch();
  struct tm localTime;
  gmtime_r(&localEpoch, &localTime);
  return static_cast<uint8_t>(localTime.tm_wday);
}

uint16_t currentLocalMinuteOfDay() {
  time_t localEpoch = currentLocalEpoch();
  struct tm localTime;
  gmtime_r(&localEpoch, &localTime);
  return static_cast<uint16_t>(localTime.tm_hour * 60 + localTime.tm_min);
}

String currentLocalTimeString() {
  if (!isTimeSynced()) {
    return "not synced";
  }

  time_t localEpoch = currentLocalEpoch();
  struct tm localTime;
  gmtime_r(&localEpoch, &localTime);
  char buffer[24];
  snprintf(
    buffer,
    sizeof(buffer),
    "%04d-%02d-%02d %02d:%02d:%02d",
    localTime.tm_year + 1900,
    localTime.tm_mon + 1,
    localTime.tm_mday,
    localTime.tm_hour,
    localTime.tm_min,
    localTime.tm_sec
  );
  return String(buffer);
}

String minuteOfDayLabel(uint16_t minuteOfDay) {
  char buffer[6];
  snprintf(buffer, sizeof(buffer), "%02u:%02u", minuteOfDay / 60, minuteOfDay % 60);
  return String(buffer);
}

String weekdayLabel(uint8_t weekday) {
  switch (weekday % 7) {
    case 0: return "Sunday";
    case 1: return "Monday";
    case 2: return "Tuesday";
    case 3: return "Wednesday";
    case 4: return "Thursday";
    case 5: return "Friday";
    default: return "Saturday";
  }
}

uint32_t scheduleOccurrenceKey(uint16_t dayIndex, uint16_t minuteOfDay) {
  return static_cast<uint32_t>(dayIndex) * 1440UL + minuteOfDay;
}

String nextScheduleSummary() {
  if (!isTimeSynced()) {
    return "Schedules paused: time not synced";
  }
  if (scheduleCount == 0) {
    return "No schedules saved";
  }

  time_t localEpoch = currentLocalEpoch();
  struct tm localTime;
  gmtime_r(&localEpoch, &localTime);
  uint8_t currentWeekday = static_cast<uint8_t>(localTime.tm_wday);
  uint16_t currentMinute = static_cast<uint16_t>(localTime.tm_hour * 60 + localTime.tm_min);
  int32_t bestDelta = 32767;
  int bestIndex = -1;
  uint8_t bestWeekday = currentWeekday;

  for (uint8_t i = 0; i < scheduleCount; i++) {
    if (!schedules[i].enabled || schedules[i].daysMask == 0) {
      continue;
    }
    for (uint8_t offset = 0; offset < 8; offset++) {
      uint8_t weekday = static_cast<uint8_t>((currentWeekday + offset) % 7);
      if ((schedules[i].daysMask & (1 << weekday)) == 0) {
        continue;
      }
      int32_t delta = static_cast<int32_t>(offset) * 1440L + schedules[i].minuteOfDay - currentMinute;
      if (delta <= 0) {
        continue;
      }
      if (delta < bestDelta) {
        bestDelta = delta;
        bestIndex = i;
        bestWeekday = weekday;
      }
      break;
    }
  }

  if (bestIndex < 0) {
    return "No enabled schedules";
  }

  String when = bestDelta < 1440 ? "today" : weekdayLabel(bestWeekday);
  return schedules[bestIndex].name + " " + when + " at " + minuteOfDayLabel(schedules[bestIndex].minuteOfDay);
}

String buildTimeJson() {
  String json;
  json.reserve(360);
  json += R"json({"ok":true,"timeSynced":)json";
  json += boolJson(isTimeSynced());
  json += R"json(,"currentEpoch":)json";
  json += static_cast<uint32_t>(time(nullptr));
  json += R"json(,"currentLocalTime":")json";
  json += escapeJson(currentLocalTimeString());
  json += R"json(","localUtcOffsetMinutes":)json";
  json += settings.localUtcOffsetMinutes;
  json += R"json(,"timeStatus":")json";
  json += escapeJson(timeSync.timeStatus);
  json += R"json(","lastSyncAttemptMs":)json";
  json += timeSync.lastSyncAttemptMs;
  json += R"json(,"lastSyncSuccessEpoch":)json";
  json += timeSync.lastSyncSuccessEpoch;
  json += F("}");
  return json;
}

bool loadScheduleObject(const String& objectJson, ScheduleEntry& entry) {
  String textValue;
  int intValue;
  bool boolValue;

  entry.id = "";
  entry.name = "";
  entry.enabled = true;
  entry.daysMask = 0x7F;
  entry.minuteOfDay = 0;
  entry.targetType = "scene";
  entry.targetId = "";
  entry.fadeSeconds = 0;
  entry.useTransition = true;
  entry.skipNext = false;
  entry.lastRunEpoch = 0;
  entry.lastRunOccurrenceKey = 0;

  if (readJsonString(objectJson, "id", textValue)) {
    entry.id = safeScheduleIdToken(textValue);
  }
  if (readJsonString(objectJson, "name", textValue)) {
    entry.name = cleanScheduleName(textValue);
  }
  if (readJsonBool(objectJson, "enabled", boolValue)) {
    entry.enabled = boolValue;
  }
  if (readJsonInt(objectJson, "daysMask", intValue)) {
    entry.daysMask = static_cast<uint8_t>(constrain(intValue, 0, 127));
  }
  if (readJsonInt(objectJson, "minuteOfDay", intValue)) {
    entry.minuteOfDay = static_cast<uint16_t>(constrain(intValue, 0, 1439));
  }
  if (readJsonString(objectJson, "targetType", textValue)) {
    entry.targetType = normalizedScheduleType(textValue);
  }
  if (readJsonString(objectJson, "targetId", textValue)) {
    textValue.trim();
    if (entry.targetType == "action") {
      entry.targetId = canonicalScheduleAction(textValue);
    } else {
      entry.targetId = safeSceneIdToken(textValue);
    }
  }
  if (readJsonInt(objectJson, "fadeSeconds", intValue)) {
    entry.fadeSeconds = static_cast<uint16_t>(constrain(intValue, 0, TIMER_MAX_MINUTES * 60));
  }
  if (readJsonBool(objectJson, "useTransition", boolValue)) {
    entry.useTransition = boolValue;
  }
  if (readJsonBool(objectJson, "skipNext", boolValue)) {
    entry.skipNext = boolValue;
  }
  if (readJsonInt(objectJson, "lastRunEpoch", intValue)) {
    entry.lastRunEpoch = static_cast<uint32_t>(max(0, intValue));
  }
  if (readJsonInt(objectJson, "lastRunOccurrenceKey", intValue)) {
    entry.lastRunOccurrenceKey = static_cast<uint32_t>(max(0, intValue));
  }

  if (entry.id.length() == 0 && entry.name.length() > 0) {
    entry.id = makeScheduleId(entry.name);
  }

  String error;
  return validateScheduleEntry(entry, error);
}

String scheduleEntryJson(const ScheduleEntry& entry) {
  String json;
  json.reserve(420);
  json += F("{\"id\":\"");
  json += escapeJson(entry.id);
  json += F("\",\"name\":\"");
  json += escapeJson(entry.name);
  json += F("\",\"enabled\":");
  json += boolJson(entry.enabled);
  json += F(",\"daysMask\":");
  json += entry.daysMask;
  json += F(",\"minuteOfDay\":");
  json += entry.minuteOfDay;
  json += F(",\"targetType\":\"");
  json += escapeJson(entry.targetType);
  json += F("\",\"targetId\":\"");
  json += escapeJson(entry.targetId);
  json += F("\",\"targetName\":\"");
  json += escapeJson(targetDisplayName(entry));
  json += F("\",\"fadeSeconds\":");
  json += entry.fadeSeconds;
  json += F(",\"useTransition\":");
  json += boolJson(entry.useTransition);
  json += F(",\"skipNext\":");
  json += boolJson(entry.skipNext);
  json += F(",\"lastRunEpoch\":");
  json += entry.lastRunEpoch;
  json += F(",\"lastRunOccurrenceKey\":");
  json += entry.lastRunOccurrenceKey;
  json += F("}");
  return json;
}

String buildScheduleJson() {
  String json;
  json.reserve(640 + scheduleCount * 440);
  json += R"json({"ok":true,"count":)json";
  json += scheduleCount;
  json += R"json(,"maxSchedules":)json";
  json += MAX_SCHEDULE_ENTRIES;
  json += R"json(,"schedulesPath":")json";
  json += SCHEDULES_PATH;
  json += R"json(","scheduleLoadStatus":")json";
  json += escapeJson(scheduleLoadStatus);
  json += R"json(","scheduleSaveStatus":")json";
  json += escapeJson(scheduleSaveStatus);
  json += R"json(","timeSynced":)json";
  json += boolJson(isTimeSynced());
  json += R"json(,"currentLocalTime":")json";
  json += escapeJson(currentLocalTimeString());
  json += R"json(","localUtcOffsetMinutes":)json";
  json += settings.localUtcOffsetMinutes;
  json += R"json(,"nextScheduleSummary":")json";
  json += escapeJson(nextScheduleSummary());
  json += R"json(","lastScheduleStatus":")json";
  json += escapeJson(lastScheduleStatus);
  json += R"json(","schedules":[)json";
  for (uint8_t i = 0; i < scheduleCount; i++) {
    if (i > 0) {
      json += ',';
    }
    json += scheduleEntryJson(schedules[i]);
  }
  json += F("]}");
  return json;
}

String buildScheduleExportJson() {
  String json;
  json.reserve(220 + scheduleCount * 420);
  json += R"json({"ok":true,"scheduleSchema":1,"exportType":"schedule","count":)json";
  json += scheduleCount;
  json += R"json(,"maxSchedules":)json";
  json += MAX_SCHEDULE_ENTRIES;
  json += R"json(,"schedules":[)json";
  for (uint8_t i = 0; i < scheduleCount; i++) {
    if (i > 0) {
      json += ',';
    }
    json += scheduleEntryJson(schedules[i]);
  }
  json += F("]}");
  return json;
}

bool scheduleIdExistsIn(const ScheduleEntry* entries, uint8_t count, const String& id) {
  for (uint8_t i = 0; i < count; i++) {
    if (entries[i].id == id) {
      return true;
    }
  }
  return false;
}

bool importScheduleFromJson(const String& rawJson, String& error) {
  String arrayJson;
  String trimmed = rawJson;
  trimmed.trim();
  if (!extractJsonArrayForKey(trimmed, "schedules", arrayJson)) {
    if (trimmed.startsWith("[")) {
      arrayJson = trimmed;
    } else {
      error = "Schedule JSON must include a schedules array";
      return false;
    }
  }

  ScheduleEntry imported[MAX_SCHEDULE_ENTRIES];
  uint8_t importedCount = 0;
  int position = arrayJson.indexOf('{');
  while (position >= 0 && importedCount < MAX_SCHEDULE_ENTRIES) {
    int end = findMatchingJsonClose(arrayJson, position, '{', '}');
    if (end < 0) {
      error = "Schedule JSON is incomplete";
      return false;
    }
    ScheduleEntry entry;
    if (!loadScheduleObject(arrayJson.substring(position, end + 1), entry)) {
      error = "Schedule JSON contains an invalid entry";
      return false;
    }
    if (scheduleIdExistsIn(imported, importedCount, entry.id)) {
      error = "Schedule JSON contains duplicate ids";
      return false;
    }
    imported[importedCount++] = entry;
    position = arrayJson.indexOf('{', end + 1);
  }

  if (position >= 0 && importedCount >= MAX_SCHEDULE_ENTRIES) {
    error = "Schedule limit reached";
    return false;
  }

  ScheduleEntry previous[MAX_SCHEDULE_ENTRIES];
  uint8_t previousCount = scheduleCount;
  for (uint8_t i = 0; i < scheduleCount; i++) {
    previous[i] = schedules[i];
  }

  scheduleCount = importedCount;
  for (uint8_t i = 0; i < importedCount; i++) {
    schedules[i] = imported[i];
  }

  if (!saveSchedules()) {
    scheduleCount = previousCount;
    for (uint8_t i = 0; i < previousCount; i++) {
      schedules[i] = previous[i];
    }
    error = "Schedule save failed";
    return false;
  }
  scheduleLoadStatus = importedCount == 0 ? "imported empty" : "imported";
  return true;
}

bool resetSchedules(String& error) {
  ScheduleEntry previous[MAX_SCHEDULE_ENTRIES];
  uint8_t previousCount = scheduleCount;
  for (uint8_t i = 0; i < scheduleCount; i++) {
    previous[i] = schedules[i];
  }

  scheduleCount = 0;
  if (!saveSchedules()) {
    scheduleCount = previousCount;
    for (uint8_t i = 0; i < previousCount; i++) {
      schedules[i] = previous[i];
    }
    error = "Schedule save failed";
    return false;
  }
  scheduleLoadStatus = "reset empty";
  lastScheduleStatus = "schedules reset";
  return true;
}

bool saveSchedules() {
  if (!settingsStorageReady) {
    settingsStorageReady = LittleFS.begin();
  }
  if (!settingsStorageReady) {
    scheduleSaveStatus = "skipped; LittleFS unavailable";
    return false;
  }

  File file = LittleFS.open(SCHEDULES_PATH, "w");
  if (!file) {
    scheduleSaveStatus = "open failed";
    return false;
  }

  String json;
  json.reserve(80 + scheduleCount * 380);
  json += F("{\"version\":1,\"schedules\":[");
  for (uint8_t i = 0; i < scheduleCount; i++) {
    if (i > 0) {
      json += ',';
    }
    json += scheduleEntryJson(schedules[i]);
  }
  json += F("]}");

  bool ok = file.print(json) == json.length();
  file.close();
  scheduleSaveStatus = ok ? "saved" : "write incomplete";
  return ok;
}

void loadSchedules() {
  scheduleCount = 0;
  scheduleLoadStatus = "empty";

  if (!settingsStorageReady) {
    settingsStorageReady = LittleFS.begin();
  }
  if (!settingsStorageReady) {
    scheduleLoadStatus = "LittleFS unavailable; schedules disabled";
    return;
  }
  if (!LittleFS.exists(SCHEDULES_PATH)) {
    scheduleLoadStatus = "missing; no schedules";
    return;
  }

  File file = LittleFS.open(SCHEDULES_PATH, "r");
  if (!file) {
    scheduleLoadStatus = "open failed; schedules disabled";
    return;
  }
  String json = file.readString();
  file.close();
  json.trim();

  int schedulesStart = json.indexOf("\"schedules\":[");
  if (!json.startsWith("{") || schedulesStart < 0) {
    scheduleLoadStatus = "corrupt; schedules disabled";
    return;
  }

  int position = json.indexOf('{', schedulesStart);
  while (position >= 0 && scheduleCount < MAX_SCHEDULE_ENTRIES) {
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

    ScheduleEntry entry;
    if (loadScheduleObject(json.substring(position, end + 1), entry) && findScheduleById(entry.id) < 0) {
      schedules[scheduleCount++] = entry;
    }
    position = json.indexOf('{', end + 1);
  }

  scheduleLoadStatus = scheduleCount == 0 ? "empty; no schedules" : "loaded";
}

bool executeScheduleTarget(ScheduleEntry& entry, bool manual, String& error) {
  if (!manual && !isTimeSynced()) {
    error = "Time is not synced";
    return false;
  }

  if (!validateScheduleEntry(entry, error)) {
    return false;
  }

  if (activeTimer.active) {
    clearActiveTimer();
    lastTimerStatus = manual ? "timer cancelled by schedule run now" : "timer cancelled by schedule";
  }
  diagnosticTest.active = false;
  diagnosticTest.name = "";

  if (entry.fadeSeconds > 0) {
    uint16_t fadeMinutes = static_cast<uint16_t>((entry.fadeSeconds + 59U) / 60U);
    fadeMinutes = static_cast<uint16_t>(constrain(fadeMinutes, 1, TIMER_MAX_MINUTES));
    TimerActionType timerAction = entry.targetType == "scene"
      ? TIMER_ACTION_SCENE
      : scheduleActionToTimerAction(entry.targetId);
    String sceneId = entry.targetType == "scene" ? entry.targetId : "";
    if (!startTimer(TIMER_MODE_FADE_TO_ACTION, timerAction, fadeMinutes, sceneId, false, error)) {
      lastScheduleStatus = String("schedule failed: ") + error;
      return false;
    }
    lastScheduleStatus = String(manual ? "manual schedule fade started: " : "schedule fade started: ") + entry.name;
    return true;
  }

  bool previousSuppression = transitionsSuppressed;
  transitionsSuppressed = previousSuppression || !entry.useTransition;
  bool ok = false;
  if (entry.targetType == "scene") {
    String sceneId = entry.targetId;
    ok = loadSceneById(sceneId, error);
  } else {
    String actionName = entry.targetId;
    ok = runActionByName(actionName, error);
  }
  transitionsSuppressed = previousSuppression;

  lastScheduleStatus = ok
    ? (String(manual ? "manual schedule ran: " : "schedule ran: ") + entry.name)
    : (String("schedule failed: ") + error);
  return ok;
}

void serviceScheduler() {
  if ((uint32_t)(millis() - schedulerLastCheckMs) < SCHEDULER_CHECK_MS) {
    return;
  }
  schedulerLastCheckMs = millis();

  if (!isTimeSynced()) {
    return;
  }

  uint8_t weekday = currentLocalWeekday();
  uint16_t minuteOfDay = currentLocalMinuteOfDay();
  uint32_t occurrence = scheduleOccurrenceKey(currentLocalDayIndex(), minuteOfDay);

  for (uint8_t i = 0; i < scheduleCount; i++) {
    ScheduleEntry& entry = schedules[i];
    if (!entry.enabled || (entry.daysMask & (1 << weekday)) == 0 || entry.minuteOfDay != minuteOfDay) {
      continue;
    }
    if (entry.lastRunOccurrenceKey == occurrence) {
      continue;
    }

    entry.lastRunOccurrenceKey = occurrence;
    entry.lastRunEpoch = static_cast<uint32_t>(time(nullptr));
    if (entry.skipNext) {
      entry.skipNext = false;
      lastScheduleStatus = String("schedule skipped once: ") + entry.name;
      saveSchedules();
      continue;
    }

    String error;
    executeScheduleTarget(entry, false, error);
    saveSchedules();
  }
}

void sendSchedulePayload(const String& message) {
  String json = buildScheduleJson();
  int insertAt = json.indexOf('{') + 1;
  json = json.substring(0, insertAt) + "\"message\":\"" + escapeJson(message) + "\"," + json.substring(insertAt);
  server.send(200, "application/json", json);
}

void handleSchedulePage() {
  server.send_P(200, PSTR("text/html"), SCHEDULE_HTML);
}

void handleApiTime() {
  server.send(200, "application/json", buildTimeJson());
}

void handleApiTimeSync() {
  beginTimeSync(true);
  String json;
  json.reserve(520);
  json += F("{\"ok\":true,\"message\":\"Time sync requested\",\"time\":");
  json += buildTimeJson();
  json += F("}");
  server.send(200, "application/json", json);
}

void handleApiTimeSet() {
  if (!server.hasArg("localUtcOffsetMinutes")) {
    sendJsonError(400, "localUtcOffsetMinutes is required");
    return;
  }
  int offset;
  if (!parseIntArg("localUtcOffsetMinutes", offset)) {
    sendJsonError(400, "Invalid UTC offset");
    return;
  }
  settings.localUtcOffsetMinutes = static_cast<int16_t>(constrain(offset, -720, 840));
  markSettingsDirty();
  serviceTimeSync();
  String json;
  json.reserve(540);
  json += F("{\"ok\":true,\"message\":\"Time offset saved\",\"time\":");
  json += buildTimeJson();
  json += F("}");
  server.send(200, "application/json", json);
}

void handleApiSchedule() {
  server.send(200, "application/json", buildScheduleJson());
}

bool scheduleFromRequest(ScheduleEntry& entry, String& error) {
  int existingIndex = -1;
  if (server.hasArg("id")) {
    String cleanId = safeScheduleIdToken(server.arg("id"));
    if (cleanId.length() > 0) {
      existingIndex = findScheduleById(cleanId);
      if (existingIndex >= 0) {
        entry = schedules[existingIndex];
      } else {
        entry.id = cleanId;
      }
    }
  }

  if (!server.hasArg("name")) {
    error = "Schedule name is required";
    return false;
  }
  entry.name = cleanScheduleName(server.arg("name"));
  if (entry.id.length() == 0) {
    entry.id = makeScheduleId(entry.name);
  }

  bool boolValue;
  int intValue;
  if (server.hasArg("enabled")) {
    if (!parseRequestBool(server.arg("enabled"), boolValue)) {
      error = "Invalid enabled value";
      return false;
    }
    entry.enabled = boolValue;
  }
  if (server.hasArg("daysMask")) {
    if (!parseIntArg("daysMask", intValue)) {
      error = "Invalid days";
      return false;
    }
    entry.daysMask = static_cast<uint8_t>(constrain(intValue, 0, 127));
  }
  if (server.hasArg("minuteOfDay")) {
    if (!parseIntArg("minuteOfDay", intValue)) {
      error = "Invalid time";
      return false;
    }
    entry.minuteOfDay = static_cast<uint16_t>(constrain(intValue, 0, 1439));
  }
  if (server.hasArg("targetType")) {
    entry.targetType = normalizedScheduleType(server.arg("targetType"));
  }
  if (server.hasArg("targetId")) {
    String target = server.arg("targetId");
    target.trim();
    entry.targetId = entry.targetType == "action" ? canonicalScheduleAction(target) : safeSceneIdToken(target);
  }
  if (server.hasArg("fadeSeconds")) {
    if (!parseIntArg("fadeSeconds", intValue)) {
      error = "Invalid fade seconds";
      return false;
    }
    entry.fadeSeconds = static_cast<uint16_t>(constrain(intValue, 0, TIMER_MAX_MINUTES * 60));
  }
  if (server.hasArg("useTransition")) {
    if (!parseRequestBool(server.arg("useTransition"), boolValue)) {
      error = "Invalid transition value";
      return false;
    }
    entry.useTransition = boolValue;
  }

  if (existingIndex < 0) {
    entry.skipNext = false;
    entry.lastRunEpoch = 0;
    entry.lastRunOccurrenceKey = 0;
  }
  return validateScheduleEntry(entry, error);
}

void handleApiScheduleSave() {
  ScheduleEntry entry;
  entry.id = "";
  entry.name = "";
  entry.enabled = true;
  entry.daysMask = 0x7F;
  entry.minuteOfDay = 0;
  entry.targetType = "scene";
  entry.targetId = "";
  entry.fadeSeconds = 0;
  entry.useTransition = true;
  entry.skipNext = false;
  entry.lastRunEpoch = 0;
  entry.lastRunOccurrenceKey = 0;

  String error;
  if (!scheduleFromRequest(entry, error)) {
    sendJsonError(400, error);
    return;
  }

  int index = findScheduleById(entry.id);
  if (index >= 0) {
    schedules[index] = entry;
  } else {
    if (scheduleCount >= MAX_SCHEDULE_ENTRIES) {
      sendJsonError(400, "Schedule limit reached");
      return;
    }
    schedules[scheduleCount++] = entry;
  }

  if (!saveSchedules()) {
    sendJsonError(500, "Schedule save failed");
    return;
  }
  sendSchedulePayload("Schedule saved");
}

void handleApiScheduleDelete() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Schedule id is required");
    return;
  }
  int index = findScheduleById(safeScheduleIdToken(server.arg("id")));
  if (index < 0) {
    sendJsonError(404, "Schedule not found");
    return;
  }
  for (uint8_t i = index; i + 1 < scheduleCount; i++) {
    schedules[i] = schedules[i + 1];
  }
  scheduleCount--;
  if (!saveSchedules()) {
    sendJsonError(500, "Schedule save failed");
    return;
  }
  sendSchedulePayload("Schedule deleted");
}

void handleApiScheduleToggle() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Schedule id is required");
    return;
  }
  int index = findScheduleById(safeScheduleIdToken(server.arg("id")));
  if (index < 0) {
    sendJsonError(404, "Schedule not found");
    return;
  }
  bool enabled;
  if (server.hasArg("enabled")) {
    if (!parseRequestBool(server.arg("enabled"), enabled)) {
      sendJsonError(400, "Invalid enabled value");
      return;
    }
  } else {
    enabled = !schedules[index].enabled;
  }
  schedules[index].enabled = enabled;
  if (!saveSchedules()) {
    sendJsonError(500, "Schedule save failed");
    return;
  }
  sendSchedulePayload(enabled ? "Schedule enabled" : "Schedule disabled");
}

void handleApiScheduleSkip() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Schedule id is required");
    return;
  }
  int index = findScheduleById(safeScheduleIdToken(server.arg("id")));
  if (index < 0) {
    sendJsonError(404, "Schedule not found");
    return;
  }
  bool skip = true;
  if (server.hasArg("skip")) {
    if (!parseRequestBool(server.arg("skip"), skip)) {
      sendJsonError(400, "Invalid skip value");
      return;
    }
  }
  schedules[index].skipNext = skip;
  if (!saveSchedules()) {
    sendJsonError(500, "Schedule save failed");
    return;
  }
  sendSchedulePayload(skip ? "Next occurrence will be skipped" : "Skip next cleared");
}

void handleApiScheduleRun() {
  if (!server.hasArg("id")) {
    sendJsonError(400, "Schedule id is required");
    return;
  }
  int index = findScheduleById(safeScheduleIdToken(server.arg("id")));
  if (index < 0) {
    sendJsonError(404, "Schedule not found");
    return;
  }
  String error;
  if (!executeScheduleTarget(schedules[index], true, error)) {
    sendJsonError(400, error);
    return;
  }
  schedules[index].lastRunEpoch = static_cast<uint32_t>(time(nullptr));
  saveSchedules();
  sendSchedulePayload("Schedule run now complete");
}

void handleApiScheduleExport() {
  server.send(200, "application/json", buildScheduleExportJson());
}

void handleApiScheduleImport() {
  if (!server.hasArg("json")) {
    sendJsonError(400, "Schedule JSON is required");
    return;
  }
  String error;
  if (!importScheduleFromJson(server.arg("json"), error)) {
    sendJsonError(400, error);
    return;
  }
  sendSchedulePayload("Schedule imported");
}

void handleApiScheduleReset() {
  if (!server.hasArg("confirm") || server.arg("confirm") != "YES") {
    sendJsonError(400, "confirm=YES is required");
    return;
  }
  String error;
  if (!resetSchedules(error)) {
    sendJsonError(500, error);
    return;
  }
  sendSchedulePayload("Schedules reset");
}

String escapeHtmlText(const String& value) {
  String escaped;
  escaped.reserve(value.length() + 8);
  for (uint16_t i = 0; i < value.length(); i++) {
    char c = value[i];
    if (c == '&') {
      escaped += F("&amp;");
    } else if (c == '<') {
      escaped += F("&lt;");
    } else if (c == '>') {
      escaped += F("&gt;");
    } else if (c == '"') {
      escaped += F("&quot;");
    } else {
      escaped += c;
    }
  }
  return escaped;
}

void sendGoHtml(const String& title, const String& message, bool ok) {
  String page;
  page.reserve(1100);
  page += F("<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>");
  page += F("<title>");
  page += escapeHtmlText(title);
  page += F("</title><style>body{margin:0;background:#101214;color:#f4f7f8;font-family:system-ui,-apple-system,Segoe UI,sans-serif}");
  page += F("main{max-width:640px;margin:0 auto;padding:34px 16px}section{border:1px solid rgba(255,255,255,.12);background:#181c20;border-radius:8px;padding:18px}");
  page += F("a{color:#8bd0ff}.status{color:");
  page += ok ? F("#9df0c0") : F("#ff9a9a");
  page += F("}</style></head><body><main><section><h1>");
  page += escapeHtmlText(title);
  page += F("</h1><p class='status'>");
  page += escapeHtmlText(message);
  page += F("</p><p>These /go links are intended for local network QR and NFC helpers only. Add <code>format=json</code> for automation.</p>");
  page += F("<p><a href='/'>Controls</a> | <a href='/schedule'>Schedule</a></p></section></main></body></html>");
  server.send(ok ? 200 : 400, "text/html", page);
}

void sendGoJson(bool ok, const String& title, const String& message) {
  String json;
  json.reserve(420);
  json += F("{\"ok\":");
  json += boolJson(ok);
  json += F(",\"message\":\"");
  json += escapeJson(message);
  json += F("\",\"title\":\"");
  json += escapeJson(title);
  json += F("\",\"state\":");
  json += buildStateJson();
  json += F("}");
  server.send(ok ? 200 : 400, "application/json", json);
}

void handleGoLink() {
  bool wantsJson = server.hasArg("format") && server.arg("format") == "json"; // format=json
  String error;
  bool ok = false;
  String title = "Link Error";
  String message = "Missing scene or action";

  if (server.hasArg("scene")) {
    String sceneId = server.arg("scene");
    ok = loadSceneById(sceneId, error);
    title = ok ? "Scene Loaded" : "Scene Link Failed";
    message = ok ? String("Loaded scene ") + sceneId : error;
  } else if (server.hasArg("action")) {
    String actionName = server.arg("action");
    ok = runActionByName(actionName, error);
    title = ok ? "Action Run" : "Action Link Failed";
    message = ok ? String("Ran action ") + actionName : error;
  }

  if (wantsJson) {
    sendGoJson(ok, title, message);
    return;
  }
  sendGoHtml(title, message, ok);
}

#endif
