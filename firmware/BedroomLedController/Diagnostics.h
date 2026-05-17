#ifndef BEDROOM_LED_CONTROLLER_DIAGNOSTICS_H
#define BEDROOM_LED_CONTROLLER_DIAGNOSTICS_H

const char DIAGNOSTICS_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Bedroom LED Diagnostics</title>
  <style>
    :root {
      color-scheme: dark;
      --bg: #101214;
      --panel: #181c20;
      --panel-2: #20262b;
      --line: rgba(255,255,255,.12);
      --text: #f4f7f8;
      --muted: #a9b4bb;
      --accent: #8bd0ff;
      --warn: #ffc680;
      --bad: #ff9a9a;
    }
    * { box-sizing: border-box; }
    body { margin: 0; min-height: 100vh; background: #101214; color: var(--text); font-family: system-ui, -apple-system, Segoe UI, sans-serif; }
    main { width: min(1040px, calc(100% - 28px)); margin: 0 auto; padding: 26px 0 34px; }
    header { display: flex; justify-content: space-between; align-items: end; gap: 16px; padding-bottom: 18px; }
    h1 { margin: 0; font-size: clamp(1.6rem, 4vw, 2.7rem); letter-spacing: 0; }
    a { color: var(--accent); }
    .links { display: flex; gap: 12px; flex-wrap: wrap; color: var(--muted); }
    .grid { display: grid; grid-template-columns: repeat(2, minmax(0, 1fr)); gap: 14px; }
    section { border: 1px solid var(--line); background: var(--panel); border-radius: 8px; padding: 16px; }
    h2 { margin: 0 0 12px; font-size: 1rem; letter-spacing: 0; }
    dl { display: grid; grid-template-columns: minmax(120px, .72fr) 1fr; gap: 8px 12px; margin: 0; }
    dt { color: var(--muted); }
    dd { margin: 0; overflow-wrap: anywhere; }
    button, input { min-height: 40px; border: 1px solid var(--line); border-radius: 8px; font: inherit; }
    button { color: #101214; background: var(--accent); border: 0; font-weight: 720; cursor: pointer; padding: 9px 12px; }
    input { width: 100%; color: var(--text); background: var(--panel-2); padding: 8px 10px; }
    label { display: grid; gap: 6px; color: var(--muted); font-size: .86rem; }
    .form-grid, .test-grid { display: grid; grid-template-columns: repeat(4, minmax(0, 1fr)); gap: 10px; align-items: end; }
    .wide { grid-column: 1 / -1; }
    .warnings { display: grid; gap: 8px; }
    .warning { border: 1px solid rgba(255,198,128,.34); color: var(--warn); background: rgba(255,198,128,.08); border-radius: 8px; padding: 10px; }
    .ok { color: #9df0c0; }
    .bad { color: var(--bad); }
    .note { color: var(--muted); line-height: 1.5; margin: 12px 0 0; font-size: .88rem; }
    @media (max-width: 780px) {
      header { display: block; }
      .links { margin-top: 10px; }
      .grid, .form-grid, .test-grid { grid-template-columns: 1fr; }
    }
  </style>
</head>
<body>
<main>
  <header>
    <div>
      <h1>Bedroom LED Diagnostics</h1>
      <div class="note">Live controller, power estimate, and wiring test tools.</div>
    </div>
    <nav class="links"><a href="/">Controls</a><a href="/schedule">Schedule</a><a href="/ota">OTA</a></nav>
  </header>

  <div class="grid">
    <section>
      <h2>Controller Status</h2>
      <dl id="controller"></dl>
    </section>
    <section>
      <h2>LED Configuration</h2>
      <dl id="led"></dl>
    </section>
    <section>
      <h2>Current Lighting State</h2>
      <dl id="lighting"></dl>
    </section>
    <section>
      <h2>Settings / Persistence</h2>
      <dl id="settings"></dl>
    </section>
    <section>
      <h2>Output Pipeline</h2>
      <dl id="pipeline"></dl>
    </section>
    <section>
      <h2>Transitions</h2>
      <dl id="transitions"></dl>
    </section>
    <section>
      <h2>Palettes</h2>
      <dl id="palettes"></dl>
    </section>
    <section>
      <h2>Surprise Me</h2>
      <dl id="surprise"></dl>
    </section>
    <section>
      <h2>Time / Schedule</h2>
      <dl id="schedule"></dl>
    </section>
    <section>
      <h2>Power Estimate</h2>
      <dl id="power"></dl>
      <p class="note">These values are estimates based on the common WS2812 worst-case assumption of about 60 mA per LED at full RGB white. The controller does not measure real current draw.</p>
    </section>
    <section class="wide">
      <h2>Power Supply</h2>
      <div class="form-grid">
        <label>Voltage (mV)<input id="supplyMillivolts" type="number" min="3000" max="24000" step="100"></label>
        <label>Current (mA)<input id="supplyMilliamps" type="number" min="100" max="30000" step="100"></label>
        <label>Safety margin (%)<input id="supplySafetyMarginPercent" type="number" min="0" max="90" step="1"></label>
        <label>Controller overhead (mA)<input id="controllerOverheadMilliamps" type="number" min="0" max="2000" step="10"></label>
      </div>
      <div class="form-grid" style="margin-top:10px">
        <button id="savePower" type="button">Save Power Settings</button>
        <button id="applyCap" type="button">Apply Recommended Brightness Cap</button>
      </div>
    </section>
    <section class="wide">
      <h2>Configuration Warnings</h2>
      <div id="warnings" class="warnings"></div>
    </section>
    <section class="wide">
      <h2>LED Test Tools</h2>
      <dl id="testState"></dl>
      <div class="test-grid">
        <button data-test="red" type="button">Red Test</button>
        <button data-test="green" type="button">Green Test</button>
        <button data-test="blue" type="button">Blue Test</button>
        <button data-test="whiteLow" type="button">Safe White Test</button>
        <button data-test="pixelWalk" type="button">Pixel Walk</button>
        <button data-test="alternating" type="button">Alternating Pixels</button>
        <button data-test="clear" type="button">Clear / Off</button>
        <button data-test="restore" type="button">Return to Previous State</button>
      </div>
    </section>
  </div>
</main>

<script>
const $ = (id) => document.getElementById(id);
let lastPower = null;

function rows(target, entries) {
  target.innerHTML = entries.map(([key, value]) => `<dt>${key}</dt><dd>${value}</dd>`).join('');
}

function fmtMa(value) {
  return `${value} mA`;
}

function fmtMw(value) {
  return `${(value / 1000).toFixed(2)} W`;
}

async function refreshDiagnostics() {
  const diagnostics = await fetch('/api/diagnostics', { cache: 'no-store' }).then((response) => response.json());
  const power = await fetch('/api/power', { cache: 'no-store' }).then((response) => response.json());
  lastPower = power;

  rows($('controller'), [
    ['Hostname', diagnostics.hostname],
    ['IP Address', diagnostics.ip],
    ['Wi-Fi', diagnostics.wifiConnected ? 'Connected' : 'Access point / offline'],
    ['SSID', diagnostics.ssid || 'Unavailable'],
    ['Signal', `${diagnostics.rssi} dBm`],
    ['Uptime', `${Math.floor(diagnostics.uptimeMs / 1000)} s`],
    ['Free Heap', `${diagnostics.freeHeap} bytes`],
    ['Reset Reason', diagnostics.resetReason]
  ]);

  rows($('led'), [
    ['LED Count', diagnostics.ledCount],
    ['LED Pin', diagnostics.ledPin],
    ['Strip Type', diagnostics.stripType],
    ['Master Brightness', `${diagnostics.masterBrightness} / 255 (${diagnostics.masterBrightnessPercent}%)`],
    ['Effective Brightness', `${diagnostics.effectiveBrightness} / 255`],
    ['Night Guard Cap', diagnostics.nightGuardBrightnessCapped ? 'Active' : 'Inactive'],
    ['Boot Pin Risk', diagnostics.ledPinBootSensitive ? 'Yes' : 'No']
  ]);

  rows($('lighting'), [
    ['Mode', diagnostics.mode],
    ['Mode Display', diagnostics.modeDisplayName],
    ['Mode Category', diagnostics.modeCategory],
    ['Mode Tags', (diagnostics.modeTags || []).join(', ') || 'none'],
    ['Last Scene', diagnostics.lastLoadedSceneName || 'none'],
    ['Color', diagnostics.colorHex],
    ['White Temperature', `${diagnostics.whiteKelvin}K`],
    ['White Source', diagnostics.usingWhiteTemperature ? 'Active' : 'Inactive'],
    ['Gamma Correction', diagnostics.gammaEnabled ? 'Enabled' : 'Disabled'],
    ['RGB Gains', `R ${diagnostics.redGain} / G ${diagnostics.greenGain} / B ${diagnostics.blueGain}`],
    ['Night Guard', diagnostics.nightGuardEnabled ? `Enabled, max ${diagnostics.nightGuardMaxBrightness}` : 'Off'],
    ['Block Flashing', diagnostics.nightGuardBlockFlashingModes ? 'Yes' : 'No'],
    ['Timer active', diagnostics.timerActive ? 'Yes' : 'No'],
    ['Timer remaining', `${Math.ceil((diagnostics.timerRemainingMs || 0) / 60000)} min`],
    ['Bedtime default', `${diagnostics.bedtimeFadeDefaultMinutes} min to ${diagnostics.bedtimeFadeDefaultTarget}`],
    ['Boot Behavior', diagnostics.bootBehavior]
  ]);

  rows($('settings'), [
    ['Path', diagnostics.settingsPath],
    ['Loaded', diagnostics.settingsLoadedOk ? 'OK' : 'Problem'],
    ['Load Status', diagnostics.settingsLoadStatus],
    ['Saved', diagnostics.settingsSaveOk ? 'OK' : diagnostics.settingsSaveStatus],
    ['Save Status', diagnostics.settingsSaveStatus],
    ['Pending Save', diagnostics.settingsDirty ? 'Yes' : 'No'],
    ['Last Save ms', diagnostics.lastSettingsSaveMs],
    ['Favorites', `${diagnostics.favoritesCount} / ${diagnostics.maxFavorites}`],
    ['Favorite Load Status', diagnostics.favoriteLoadStatus],
    ['Favorite Save Status', diagnostics.favoriteSaveStatus]
  ]);

  rows($('pipeline'), [
    ['Frame Buffer', diagnostics.frameBufferActive ? 'Active' : 'Unavailable'],
    ['Brightness Stage', 'Master brightness applied centrally before output'],
    ['Gamma', diagnostics.gammaEnabled ? 'Enabled' : 'Disabled'],
    ['Calibration', `R ${diagnostics.redGain} / G ${diagnostics.greenGain} / B ${diagnostics.blueGain}`],
    ['Output Path', diagnostics.outputPath]
  ]);

  rows($('transitions'), [
    ['Enabled', diagnostics.transitionsEnabled ? 'Yes' : 'No'],
    ['Style', diagnostics.transitionStyle],
    ['Duration', `${diagnostics.transitionDurationMs} ms`],
    ['Transition active', diagnostics.transitionActive ? 'Yes' : 'No'],
    ['Transition progress', `${diagnostics.transitionProgressPercent}%`],
    ['Buffer memory', `${diagnostics.transitionBufferBytes} bytes`]
  ]);

  rows($('palettes'), [
    ['Enabled', diagnostics.paletteEnabled ? 'Yes' : 'No'],
    ['Active Palette', `${diagnostics.activePaletteName || 'None'} (${diagnostics.activePaletteId || 'none'})`],
    ['Palette Count', `${diagnostics.paletteCount} / ${diagnostics.maxPalettes}`],
    ['Current Mode Support', diagnostics.currentModeSupportsPalette ? 'Yes' : 'No'],
    ['Palette Role', diagnostics.currentModePaletteRole || 'none'],
    ['Path', diagnostics.palettesPath],
    ['Load Status', diagnostics.paletteLoadStatus],
    ['Save Status', diagnostics.paletteSaveStatus]
  ]);

  rows($('surprise'), [
    ['Last Mood', diagnostics.lastSurpriseMood || 'none'],
    ['Last Summary', diagnostics.lastSurpriseSummary || 'none'],
    ['Last Status', diagnostics.lastSurpriseStatus || 'none']
  ]);

  rows($('schedule'), [
    ['Time Synced', diagnostics.timeSynced ? 'Yes' : 'No'],
    ['Current Local Time', diagnostics.currentLocalTime || 'not synced'],
    ['UTC Offset', `${diagnostics.localUtcOffsetMinutes} minutes`],
    ['Schedule Count', `${diagnostics.scheduleCount} / ${diagnostics.maxScheduleEntries}`],
    ['Next Schedule', diagnostics.nextScheduleSummary || 'none'],
    ['Last Schedule Status', diagnostics.lastScheduleStatus || 'none'],
    ['Schedule Path', diagnostics.schedulesPath],
    ['Load Status', diagnostics.scheduleLoadStatus],
    ['Save Status', diagnostics.scheduleSaveStatus]
  ]);

  rows($('power'), [
    ['Worst-case LED Current', fmtMa(power.maxLedCurrentMilliamps)],
    ['Worst-case LED Power', fmtMw(power.maxLedPowerMilliwatts)],
    ['Estimated LED Current', fmtMa(power.estimatedLedCurrentMilliamps)],
    ['Estimated Total Current', fmtMa(power.estimatedTotalCurrentMilliamps)],
    ['Usable Supply Current', fmtMa(power.usableSupplyCurrentMilliamps)],
    ['Recommended Cap', `${power.recommendedBrightnessCap} / 255`],
    ['Current Brightness', `${power.currentMasterBrightness} / 255`],
    ['Above Cap', power.aboveRecommendedCap ? 'Yes' : 'No']
  ]);

  $('supplyMillivolts').value = power.supplyMillivolts;
  $('supplyMilliamps').value = power.supplyMilliamps;
  $('supplySafetyMarginPercent').value = power.supplySafetyMarginPercent;
  $('controllerOverheadMilliamps').value = power.controllerOverheadMilliamps;

  const warningList = diagnostics.warnings || [];
  $('warnings').innerHTML = warningList.length
    ? warningList.map((warning) => `<div class="warning"><strong>${warning.code}</strong><br>${warning.message}</div>`).join('')
    : '<div class="ok">No active warnings.</div>';

  rows($('testState'), [
    ['Active', diagnostics.testActive ? 'Yes' : 'No'],
    ['Test Name', diagnostics.testName || 'none'],
    ['Safe Test Brightness', `${diagnostics.diagnosticTestBrightness} / 255`]
  ]);
}

async function callSet(params) {
  const query = new URLSearchParams(params).toString();
  await fetch(`/set?${query}`, { cache: 'no-store' });
  await refreshDiagnostics();
}

$('savePower').addEventListener('click', () => callSet({
  supplyMillivolts: $('supplyMillivolts').value,
  supplyMilliamps: $('supplyMilliamps').value,
  supplySafetyMarginPercent: $('supplySafetyMarginPercent').value,
  controllerOverheadMilliamps: $('controllerOverheadMilliamps').value
}).catch(console.error));

$('applyCap').addEventListener('click', () => {
  const cap = lastPower ? lastPower.recommendedBrightnessCap : 0;
  callSet({ applyRecommendedBrightnessCap: 1, masterBrightness: cap }).catch(console.error);
});

document.querySelectorAll('[data-test]').forEach((button) => {
  button.addEventListener('click', () => {
    fetch(`/api/test?name=${encodeURIComponent(button.dataset.test)}`, { cache: 'no-store' })
      .then(refreshDiagnostics)
      .catch(console.error);
  });
});

refreshDiagnostics().catch(console.error);
setInterval(() => refreshDiagnostics().catch(console.error), 3000);
</script>
</body>
</html>
)rawliteral";

String boolJson(bool value) {
  return value ? F("true") : F("false");
}

String escapeJson(const String& value) {
  String escaped;
  escaped.reserve(value.length() + 4);
  for (uint16_t i = 0; i < value.length(); i++) {
    char c = value[i];
    if (c == '"' || c == '\\') {
      escaped += '\\';
    }
    if (c >= 32) {
      escaped += c;
    }
  }
  return escaped;
}

String ipAddressString() {
  if (WiFi.status() == WL_CONNECTED) {
    return WiFi.localIP().toString();
  }
  return WiFi.softAPIP().toString();
}

String colorHexString() {
  char hex[7];
  snprintf(hex, sizeof(hex), "%02X%02X%02X", settings.color.r, settings.color.g, settings.color.b);
  return String(hex);
}

uint8_t masterBrightnessPercent() {
  return static_cast<uint8_t>((static_cast<uint16_t>(settings.masterBrightness) * 100U + 127U) / 255U);
}

uint32_t safeSubtractCurrent(uint32_t value, uint32_t subtract) {
  return value > subtract ? value - subtract : 0;
}

PowerEstimate buildPowerEstimate() {
  PowerEstimate estimate;
  estimate.maxLedCurrentMilliamps = static_cast<uint32_t>(LED_COUNT) * 60UL;
  estimate.maxLedPowerMilliwatts =
    (static_cast<uint32_t>(settings.supplyMillivolts) * estimate.maxLedCurrentMilliamps) / 1000UL;
  estimate.estimatedLedCurrentMilliamps =
    (estimate.maxLedCurrentMilliamps * static_cast<uint32_t>(getEffectiveBrightness())) / 255UL;
  estimate.estimatedTotalCurrentMilliamps =
    estimate.estimatedLedCurrentMilliamps + settings.controllerOverheadMilliamps;

  uint8_t margin = constrain(settings.supplySafetyMarginPercent, 0, 90);
  estimate.usableSupplyCurrentMilliamps =
    (static_cast<uint32_t>(settings.supplyMilliamps) * static_cast<uint32_t>(100 - margin)) / 100UL;
  estimate.safeLedCurrentMilliamps =
    safeSubtractCurrent(estimate.usableSupplyCurrentMilliamps, settings.controllerOverheadMilliamps);

  if (estimate.maxLedCurrentMilliamps == 0) {
    estimate.recommendedBrightnessCap = 0;
  } else {
    uint32_t cap =
      (estimate.safeLedCurrentMilliamps * 255UL) / estimate.maxLedCurrentMilliamps;
    estimate.recommendedBrightnessCap = static_cast<uint8_t>(constrain(cap, 0UL, 255UL));
  }

  estimate.aboveRecommendedCap = getEffectiveBrightness() > estimate.recommendedBrightnessCap;
  return estimate;
}

void appendWarning(String& json, bool& first, const String& code, const String& message) {
  if (!first) {
    json += ',';
  }
  first = false;
  json += F("{\"code\":\"");
  json += escapeJson(code);
  json += F("\",\"message\":\"");
  json += escapeJson(message);
  json += F("\"}");
}

String buildWarningsJson(const PowerEstimate& estimate) {
  String json;
  json.reserve(700);
  json += '[';
  bool first = true;

  if (estimate.aboveRecommendedCap) {
    appendWarning(json, first, F("power_brightness"), F("Current brightness may exceed the configured power supply safety estimate."));
  }
  if (LED_PIN == D3) {
    appendWarning(json, first, F("gpio0_boot_pin"), F("D3/GPIO0 can affect ESP8266 boot mode if pulled low during startup."));
  }
  if (WiFi.status() == WL_CONNECTED && WiFi.RSSI() < -75) {
    appendWarning(json, first, F("weak_wifi"), F("Wi-Fi signal is weak. OTA and web control may be unreliable."));
  }
  if (ESP.getFreeHeap() < 18000) {
    appendWarning(json, first, F("low_heap"), F("Free heap is low. Large pages or complex animations may become unstable."));
  }
  if (!settingsLoadedOk ||
      settingsLoadStatus.indexOf("failed") >= 0 ||
      settingsLoadStatus.indexOf("corrupt") >= 0 ||
      settingsSaveStatus.indexOf("failed") >= 0 ||
      settingsSaveStatus.indexOf("incomplete") >= 0 ||
      settingsSaveStatus.indexOf("unavailable") >= 0) {
    appendWarning(json, first, F("settings_storage"), F("Settings storage reported an error. Changes may not survive reboot."));
  }
  if (paletteLoadStatus.indexOf("corrupt") >= 0 ||
      paletteLoadStatus.indexOf("failed") >= 0 ||
      paletteSaveStatus.indexOf("incomplete") >= 0 ||
      paletteSaveStatus.indexOf("unavailable") >= 0) {
    appendWarning(json, first, F("palette_storage"), F("Palette storage reported an error. Built-in palettes remain available."));
  }
  if (!isTimeSynced()) {
    appendWarning(json, first, F("time_unsynced"), F("Schedules are paused until the controller has a valid clock."));
  }
  if (scheduleLoadStatus.indexOf("corrupt") >= 0 ||
      scheduleLoadStatus.indexOf("failed") >= 0 ||
      scheduleSaveStatus.indexOf("incomplete") >= 0 ||
      scheduleSaveStatus.indexOf("unavailable") >= 0) {
    appendWarning(json, first, F("schedule_storage"), F("Schedule storage reported an error. Saved routines may not survive reboot."));
  }
  if (estimate.maxLedCurrentMilliamps > settings.supplyMilliamps) {
    appendWarning(json, first, F("full_white_current"), F("Full-white worst case is above configured supply capacity."));
  }

  json += ']';
  return json;
}

String buildPowerJson() {
  PowerEstimate estimate = buildPowerEstimate();
  String json;
  json.reserve(760);
  json += R"json({"ok":true,"estimateOnly":true)json";
  json += R"json(,"ledCount":)json";
  json += LED_COUNT;
  json += R"json(,"supplyMillivolts":)json";
  json += settings.supplyMillivolts;
  json += R"json(,"supplyMilliamps":)json";
  json += settings.supplyMilliamps;
  json += R"json(,"supplySafetyMarginPercent":)json";
  json += settings.supplySafetyMarginPercent;
  json += R"json(,"controllerOverheadMilliamps":)json";
  json += settings.controllerOverheadMilliamps;
  json += R"json(,"safetyMarginPercent":)json";
  json += settings.supplySafetyMarginPercent;
  json += R"json(,"maxLedCurrentMilliamps":)json";
  json += estimate.maxLedCurrentMilliamps;
  json += R"json(,"maxLedPowerMilliwatts":)json";
  json += estimate.maxLedPowerMilliwatts;
  json += R"json(,"estimatedLedCurrentMilliamps":)json";
  json += estimate.estimatedLedCurrentMilliamps;
  json += R"json(,"estimatedTotalCurrentMilliamps":)json";
  json += estimate.estimatedTotalCurrentMilliamps;
  json += R"json(,"usableSupplyCurrentMilliamps":)json";
  json += estimate.usableSupplyCurrentMilliamps;
  json += R"json(,"safeLedCurrentMilliamps":)json";
  json += estimate.safeLedCurrentMilliamps;
  json += R"json(,"recommendedBrightnessCap":)json";
  json += estimate.recommendedBrightnessCap;
  json += R"json(,"currentMasterBrightness":)json";
  json += settings.masterBrightness;
  json += R"json(,"effectiveBrightness":)json";
  json += getEffectiveBrightness();
  json += R"json(,"nightGuardBrightnessCapped":)json";
  json += boolJson(isNightGuardBrightnessCapped());
  json += R"json(,"aboveRecommendedCap":)json";
  json += boolJson(estimate.aboveRecommendedCap);
  json += F("}");
  return json;
}

void noteFreeHeap(uint32_t freeHeap) {
  if (minFreeHeapSeen == 0 || freeHeap < minFreeHeapSeen) {
    minFreeHeapSeen = freeHeap;
  }
}

void updateResourceStats() {
  uint32_t nowMs = millis();
  if (resourceStatsLastSampleMs == 0 ||
      (uint32_t)(nowMs - resourceStatsLastSampleMs) >= 1000UL) {
    noteFreeHeap(ESP.getFreeHeap());
    resourceStatsLastSampleMs = nowMs;
  }
}

uint32_t beginEndpointHeapMetric(const char* route) {
  uint32_t heapBefore = ESP.getFreeHeap();
  noteFreeHeap(heapBefore);
  return heapBefore;
}

int findEndpointHeapMetric(const char* route) {
  for (uint8_t i = 0; i < endpointHeapMetricCount; i++) {
    if (strncmp(endpointHeapMetrics[i].route, route, sizeof(endpointHeapMetrics[i].route)) == 0) {
      return i;
    }
  }
  return -1;
}

void finishEndpointHeapMetric(const char* route, uint32_t heapBeforeBytes, uint32_t payloadBytes) {
  uint32_t heapAfter = ESP.getFreeHeap();
  noteFreeHeap(heapAfter);

  int index = findEndpointHeapMetric(route);
  if (index < 0) {
    index = endpointHeapMetricCount < MAX_ENDPOINT_HEAP_METRICS
      ? endpointHeapMetricCount++
      : MAX_ENDPOINT_HEAP_METRICS - 1;
  }

  EndpointHeapMetric& metric = endpointHeapMetrics[index];
  copyFixedCString(metric.route, sizeof(metric.route), route);
  metric.measuredAtMs = millis();
  metric.heapBeforeBytes = heapBeforeBytes;
  metric.heapAfterBytes = heapAfter;
  metric.heapMinimumBytes = min(heapBeforeBytes, heapAfter);
  metric.payloadBytes = payloadBytes;
}

String buildEndpointHeapMetricsJson() {
  String json;
  json.reserve(120 + endpointHeapMetricCount * 180);
  json += '[';
  for (uint8_t i = 0; i < endpointHeapMetricCount; i++) {
    if (i > 0) {
      json += ',';
    }
    int32_t delta = static_cast<int32_t>(endpointHeapMetrics[i].heapAfterBytes) -
                    static_cast<int32_t>(endpointHeapMetrics[i].heapBeforeBytes);
    json += R"json({"route":")json";
    json += escapeJson(String(endpointHeapMetrics[i].route));
    json += R"json(","measuredAtMs":)json";
    json += endpointHeapMetrics[i].measuredAtMs;
    json += R"json(,"heapBeforeBytes":)json";
    json += endpointHeapMetrics[i].heapBeforeBytes;
    json += R"json(,"heapAfterBytes":)json";
    json += endpointHeapMetrics[i].heapAfterBytes;
    json += R"json(,"heapMinimumBytes":)json";
    json += endpointHeapMetrics[i].heapMinimumBytes;
    json += R"json(,"heapDeltaBytes":)json";
    json += delta;
    json += R"json(,"payloadBytes":)json";
    json += endpointHeapMetrics[i].payloadBytes;
    json += '}';
  }
  json += ']';
  return json;
}

String buildDiagnosticsJson() {
  PowerEstimate estimate = buildPowerEstimate();
  String ssid = WiFi.status() == WL_CONNECTED ? WiFi.SSID() : String("");
  String displayName = modeDisplayName(settings.mode);
  String category = modeCategory(settings.mode);
  FSInfo fsInfo;
  bool fsInfoAvailable = settingsStorageReady && LittleFS.info(fsInfo);
  uint32_t freeHeapNow = ESP.getFreeHeap();
  noteFreeHeap(freeHeapNow);
  String json;
  json.reserve(3940);
  json += R"json({"ok":true)json";
  json += R"json(,"projectVersion":"Phase 4D")json";
  json += R"json(,"boardTarget":"esp8266:esp8266:d1_mini")json";
  json += R"json(,"hostname":")json";
  json += escapeJson(DEVICE_HOSTNAME);
  json += R"json(","ip":")json";
  json += escapeJson(ipAddressString());
  json += R"json(","wifiConnected":)json";
  json += boolJson(WiFi.status() == WL_CONNECTED);
  json += R"json(,"ssid":")json";
  json += escapeJson(ssid);
  json += R"json(","rssi":)json";
  json += WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0;
  json += R"json(,"uptimeMs":)json";
  json += millis();
  json += R"json(,"freeHeap":)json";
  json += freeHeapNow;
  json += R"json(,"freeHeapNow":)json";
  json += freeHeapNow;
  json += R"json(,"maxFreeBlockSize":)json";
  json += ESP.getMaxFreeBlockSize();
  json += R"json(,"heapFragmentationPercent":)json";
  json += ESP.getHeapFragmentation();
  json += R"json(,"minFreeHeapSinceBoot":)json";
  json += minFreeHeapSeen;
  json += R"json(,"sketchSizeBytes":)json";
  json += ESP.getSketchSize();
  json += R"json(,"freeSketchSpaceBytes":)json";
  json += ESP.getFreeSketchSpace();
  json += R"json(,"littleFsMeasured":)json";
  json += boolJson(fsInfoAvailable);
  json += R"json(,"littleFsTotalBytes":)json";
  json += fsInfoAvailable ? fsInfo.totalBytes : 0;
  json += R"json(,"littleFsUsedBytes":)json";
  json += fsInfoAvailable ? fsInfo.usedBytes : 0;
  json += R"json(,"littleFsFreeBytes":)json";
  json += fsInfoAvailable && fsInfo.totalBytes >= fsInfo.usedBytes ? fsInfo.totalBytes - fsInfo.usedBytes : 0;
  json += R"json(,"otaRoutePresent":true)json";
  json += R"json(,"browserUpdateRoutePresent":true)json";
  json += R"json(,"resetReason":")json";
  json += escapeJson(ESP.getResetReason());
  json += R"json(","ledCount":)json";
  json += LED_COUNT;
  json += R"json(,"ledPin":"D3/GPIO0")json";
  json += R"json(,"ledPinBootSensitive":)json";
  json += boolJson(LED_PIN == D3);
  json += R"json(,"stripType":"WS2812 / NeoPixel GRB")json";
  json += R"json(,"mode":")json";
  json += modeKey(settings.mode);
  json += R"json(","modeDisplayName":")json";
  json += escapeJson(displayName);
  json += R"json(","modeCategory":")json";
  json += escapeJson(category);
  json += R"json(","modeTags":)json";
  json += currentModeTagsJson();
  json += R"json(","currentModeSupportsPalette":)json";
  json += boolJson(currentModeSupportsPalette());
  json += R"json(,"currentModePaletteRole":")json";
  json += escapeJson(currentModePaletteRole());
  json += R"json(","paletteEnabled":)json";
  json += boolJson(settings.paletteEnabled);
  json += R"json(,"activePaletteId":")json";
  json += escapeJson(settings.activePaletteId);
  json += R"json(","activePaletteName":")json";
  json += escapeJson(activePaletteName());
  json += R"json(","palettesPath":")json";
  json += PALETTES_PATH;
  json += R"json(","paletteCount":)json";
  json += paletteCount;
  json += R"json(,"maxPalettes":)json";
  json += MAX_PALETTES;
  json += R"json(,"paletteLoadStatus":")json";
  json += escapeJson(paletteLoadStatus);
  json += R"json(","paletteSaveStatus":")json";
  json += escapeJson(paletteSaveStatus);
  json += R"json(","colorHex":"#)json";
  json += colorHexString();
  json += R"json(","whiteKelvin":)json";
  json += settings.whiteKelvin;
  json += R"json(,"usingWhiteTemperature":)json";
  json += boolJson(settings.usingWhiteTemperature);
  json += R"json(,"masterBrightness":)json";
  json += settings.masterBrightness;
  json += R"json(,"masterBrightnessPercent":)json";
  json += masterBrightnessPercent();
  json += R"json(,"effectiveBrightness":)json";
  json += getEffectiveBrightness();
  json += R"json(,"nightGuardEnabled":)json";
  json += boolJson(settings.nightGuardEnabled);
  json += R"json(,"nightGuardMaxBrightness":)json";
  json += settings.nightGuardMaxBrightness;
  json += R"json(,"nightGuardBlockFlashingModes":)json";
  json += boolJson(settings.nightGuardBlockFlashingModes);
  json += R"json(,"nightGuardPreferWarmTemperature":)json";
  json += boolJson(settings.nightGuardPreferWarmTemperature);
  json += R"json(,"nightGuardBrightnessCapped":)json";
  json += boolJson(isNightGuardBrightnessCapped());
  json += R"json(,"gammaEnabled":)json";
  json += boolJson(settings.gammaEnabled);
  json += R"json(,"redGain":)json";
  json += settings.redGain;
  json += R"json(,"greenGain":)json";
  json += settings.greenGain;
  json += R"json(,"blueGain":)json";
  json += settings.blueGain;
  json += R"json(,"bootBehavior":")json";
  json += escapeJson(settings.bootBehavior);
  json += R"json(","settingsPath":")json";
  json += SETTINGS_PATH;
  json += R"json(","settingsLoadedOk":)json";
  json += boolJson(settingsLoadedOk);
  json += R"json(,"settingsSaveOk":)json";
  json += boolJson(settingsSaveOk);
  json += R"json(,"settingsDirty":)json";
  json += boolJson(settingsDirty);
  json += R"json(,"settingsLoadStatus":")json";
  json += escapeJson(settingsLoadStatus);
  json += R"json(","settingsSaveStatus":")json";
  json += escapeJson(settingsSaveStatus);
  json += R"json(","lastSettingsSaveMs":)json";
  json += lastSettingsSaveMs;
  json += R"json(,"frameBufferActive":true)json";
  json += R"json(,"outputPath":"frame buffer to outputFrameToStrip to strip.show")json";
  json += R"json(,"testActive":)json";
  json += boolJson(diagnosticTest.active);
  json += R"json(,"testName":")json";
  json += escapeJson(diagnosticTest.name);
  json += R"json(","diagnosticTestBrightness":)json";
  json += DIAGNOSTIC_TEST_BRIGHTNESS;
  json += R"json(,"sceneCount":)json";
  json += sceneCount;
  json += R"json(,"maxScenes":)json";
  json += MAX_SCENES;
  json += R"json(,"sceneLoadStatus":")json";
  json += escapeJson(sceneLoadStatus);
  json += R"json(","sceneSaveStatus":")json";
  json += escapeJson(sceneSaveStatus);
  json += R"json(","favoritesPath":")json";
  json += FAVORITES_PATH;
  json += R"json(","favoritesCount":)json";
  json += favoriteCount;
  json += R"json(,"maxFavorites":)json";
  json += MAX_FAVORITES;
  json += R"json(,"favoriteLoadStatus":")json";
  json += escapeJson(favoriteLoadStatus);
  json += R"json(","favoriteSaveStatus":")json";
  json += escapeJson(favoriteSaveStatus);
  json += R"json(","lastActionStatus":")json";
  json += escapeJson(lastActionStatus);
  json += R"json(","lastSurpriseSummary":")json";
  json += escapeJson(lastSurpriseSummary);
  json += R"json(","lastSurpriseMood":")json";
  json += escapeJson(lastSurpriseMood);
  json += R"json(","lastSurpriseStatus":")json";
  json += escapeJson(lastSurpriseStatus);
  json += R"json(","lastLoadedSceneId":")json";
  json += escapeJson(lastLoadedSceneId);
  json += R"json(","lastLoadedSceneName":")json";
  json += escapeJson(lastLoadedSceneName);
  json += R"json(","currentStateMatchesLoadedScene":)json";
  json += boolJson(currentStateMatchesLoadedScene);
  json += R"json(","timerActive":)json";
  json += boolJson(activeTimer.active);
  json += R"json(,"timerMode":")json";
  json += timerModeKey(activeTimer.mode);
  json += R"json(","timerAction":")json";
  json += timerActionKey(activeTimer.action);
  json += R"json(","timerTargetSceneId":")json";
  json += escapeJson(activeTimer.targetSceneId);
  json += R"json(","timerDurationMs":)json";
  json += activeTimer.durationMs;
  json += R"json(,"timerElapsedMs":)json";
  json += getTimerElapsedMs();
  json += R"json(,"timerRemainingMs":)json";
  json += getTimerRemainingMs();
  json += R"json(,"timerProgressPercent":)json";
  json += getTimerProgressPercent();
  json += R"json(,"timerFadeActive":)json";
  json += boolJson(activeTimer.fadeActive);
  json += R"json(,"bedtimeFadeActive":)json";
  json += boolJson(activeTimer.bedtimeFadeActive);
  json += R"json(,"lastTimerStatus":")json";
  json += escapeJson(lastTimerStatus);
  json += R"json(","timeSynced":)json";
  json += boolJson(isTimeSynced());
  json += R"json(,"currentLocalTime":")json";
  json += escapeJson(currentLocalTimeString());
  json += R"json(","localUtcOffsetMinutes":)json";
  json += settings.localUtcOffsetMinutes;
  json += R"json(,"scheduleCount":)json";
  json += scheduleCount;
  json += R"json(,"maxScheduleEntries":)json";
  json += MAX_SCHEDULE_ENTRIES;
  json += R"json(,"schedulesPath":")json";
  json += SCHEDULES_PATH;
  json += R"json(","scheduleLoadStatus":")json";
  json += escapeJson(scheduleLoadStatus);
  json += R"json(","scheduleSaveStatus":")json";
  json += escapeJson(scheduleSaveStatus);
  json += R"json(","nextScheduleSummary":")json";
  json += escapeJson(nextScheduleSummary());
  json += R"json(","lastScheduleStatus":")json";
  json += escapeJson(lastScheduleStatus);
  json += R"json(","bedtimeFadeDefaultMinutes":)json";
  json += settings.bedtimeFadeDefaultMinutes;
  json += R"json(,"bedtimeFadeDefaultTarget":")json";
  json += escapeJson(settings.bedtimeFadeDefaultTarget);
  json += R"json(","bedtimeFadeTurnOffAtEnd":)json";
  json += boolJson(settings.bedtimeFadeTurnOffAtEnd);
  json += R"json(,"transitionsEnabled":)json";
  json += boolJson(settings.transitionsEnabled);
  json += R"json(,"transitionStyle":")json";
  json += escapeJson(normalizeTransitionStyle(settings.transitionStyle));
  json += R"json(","transitionDurationMs":)json";
  json += settings.transitionDurationMs;
  json += R"json(,"transitionActive":)json";
  json += boolJson(transitionState.active);
  json += R"json(,"transitionProgressPercent":)json";
  json += getTransitionProgressPercent();
  json += R"json(,"transitionBufferBytes":)json";
  json += static_cast<uint16_t>(sizeof(previousFrame) + sizeof(targetFrame) + sizeof(frameBuffer));
  json += R"json(,"recommendedBrightnessCap":)json";
  json += estimate.recommendedBrightnessCap;
  json += R"json(,"warnings":)json";
  json += buildWarningsJson(estimate);
  json += R"json(,"endpointHeapMetrics":)json";
  json += buildEndpointHeapMetricsJson();
  json += F("}");
  return json;
}

void handleDiagnosticsPage() {
  server.send_P(200, PSTR("text/html"), DIAGNOSTICS_HTML);
}

void handleApiDiagnostics() {
  uint32_t heapBefore = beginEndpointHeapMetric("/api/diagnostics");
  String json = buildDiagnosticsJson();
  finishEndpointHeapMetric("/api/diagnostics", heapBefore, json.length());
  server.send(200, "application/json", json);
}

void handleApiPower() {
  server.send(200, "application/json", buildPowerJson());
}

#endif
