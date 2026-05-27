#ifndef BEDROOM_LED_CONTROLLER_WEB_UI_H
#define BEDROOM_LED_CONTROLLER_WEB_UI_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Bedroom LED Controller</title>
  <style>
    :root {
      color-scheme: dark;
      --bg: #101214;
      --panel: #181c20;
      --panel-2: #20262b;
      --line: rgba(255,255,255,.11);
      --text: #f4f7f8;
      --muted: #a9b4bb;
      --accent: #8bd0ff;
      --accent-2: #ffc680;
      --shadow: 0 24px 80px rgba(0,0,0,.35);
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      min-height: 100vh;
      font-family: Inter, ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
      background:
        radial-gradient(circle at 15% 0%, rgba(139,208,255,.12), transparent 30rem),
        linear-gradient(145deg, #0e1113 0%, #14171b 52%, #0d1011 100%);
      color: var(--text);
    }
    main {
      width: min(1040px, calc(100% - 28px));
      margin: 0 auto;
      padding: 28px 0 34px;
    }
    header {
      display: flex;
      justify-content: space-between;
      align-items: end;
      gap: 18px;
      padding: 10px 0 20px;
    }
    h1 {
      margin: 0;
      font-size: clamp(1.7rem, 4vw, 3.1rem);
      font-weight: 720;
      letter-spacing: 0;
    }
    .sub {
      margin-top: 7px;
      color: var(--muted);
      font-size: .98rem;
    }
    .pill {
      border: 1px solid var(--line);
      background: rgba(255,255,255,.05);
      color: var(--text);
      border-radius: 999px;
      padding: 9px 13px;
      font-size: .88rem;
      white-space: nowrap;
    }
    .global-status {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
      min-height: 42px;
      margin: 0 0 18px;
      border: 1px solid var(--line);
      border-radius: 8px;
      background: rgba(255,255,255,.05);
      color: var(--text);
      padding: 9px 12px;
      font-size: .92rem;
    }
    .global-status.pending {
      border-color: rgba(139,208,255,.42);
      background: rgba(139,208,255,.10);
    }
    .global-status.success {
      border-color: rgba(142,232,172,.38);
      background: rgba(142,232,172,.10);
    }
    .global-status.error {
      border-color: rgba(255,154,154,.48);
      background: rgba(255,154,154,.10);
      color: #ffd1d1;
    }
    .grid {
      display: grid;
      grid-template-columns: minmax(280px, 1fr) minmax(300px, 1.15fr);
      gap: 18px;
      align-items: stretch;
    }
    section {
      border: 1px solid var(--line);
      background: color-mix(in srgb, var(--panel), transparent 2%);
      border-radius: 8px;
      box-shadow: var(--shadow);
      padding: 18px;
    }
    h2 {
      margin: 0 0 14px;
      font-size: 1.02rem;
      letter-spacing: 0;
      font-weight: 680;
    }
    .wide { grid-column: 1 / -1; }
    .control-stack {
      display: grid;
      gap: 16px;
    }
    .color-row {
      display: grid;
      grid-template-columns: 150px 1fr;
      gap: 18px;
      align-items: center;
    }
    input[type="color"] {
      width: 150px;
      height: 150px;
      border: 0;
      padding: 0;
      background: transparent;
      cursor: pointer;
    }
    input[type="range"] {
      width: 100%;
      accent-color: var(--accent);
    }
    .brightness-readout {
      display: flex;
      justify-content: space-between;
      gap: 12px;
      color: var(--text);
      font-size: 1.1rem;
      font-weight: 700;
    }
    .summary-grid {
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 12px;
    }
    .summary-item {
      border: 1px solid var(--line);
      background: var(--panel-2);
      border-radius: 8px;
      padding: 11px;
      min-height: 74px;
    }
    .summary-label {
      color: var(--muted);
      font-size: .78rem;
      margin-bottom: 7px;
    }
    .summary-value {
      color: var(--text);
      font-size: .98rem;
      font-weight: 700;
      overflow-wrap: anywhere;
    }
    .swatch {
      width: 100%;
      min-height: 72px;
      border-radius: 8px;
      border: 1px solid var(--line);
      background: #ffb26d;
    }
    label {
      display: grid;
      gap: 7px;
      color: var(--muted);
      font-size: .86rem;
    }
    select, button, input[type="text"], input[type="number"], textarea {
      width: 100%;
      border: 1px solid var(--line);
      border-radius: 8px;
      background: var(--panel-2);
      color: var(--text);
      min-height: 42px;
      padding: 9px 11px;
      font: inherit;
    }
    button {
      cursor: pointer;
      color: #101214;
      background: linear-gradient(135deg, var(--accent), #eef9ff);
      border: 0;
      font-weight: 720;
    }
    button:disabled {
      cursor: wait;
      opacity: .62;
    }
    .inline-retry {
      width: auto;
      min-width: 88px;
      margin-top: 8px;
      padding: 8px 10px;
      min-height: 36px;
    }
    textarea {
      min-height: 112px;
      resize: vertical;
      font-family: ui-monospace, SFMono-Regular, Consolas, "Liberation Mono", monospace;
      font-size: .82rem;
      line-height: 1.4;
    }
    .white-wrap {
      display: grid;
      grid-template-columns: 78px 1fr;
      gap: 20px;
      align-items: center;
      min-height: 278px;
    }
    .temp-rail {
      display: grid;
      place-items: center;
      height: 260px;
      border-radius: 8px;
      border: 1px solid var(--line);
      background: linear-gradient(to top, #ffad62 0%, #ffd6a3 27%, #fff3dc 48%, #f7fbff 70%, #d8e8ff 100%);
    }
    #kelvin {
      writing-mode: vertical-rl;
      direction: rtl;
      width: 42px;
      height: 230px;
      accent-color: #f8fbff;
    }
    .temp-value {
      font-size: 2rem;
      font-weight: 720;
      color: var(--text);
    }
    .mode-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 14px;
      align-items: end;
    }
    .mode-filter-bar {
      display: flex;
      flex-wrap: wrap;
      gap: 8px;
      margin-bottom: 14px;
    }
    .filter-chip {
      width: auto;
      min-width: 74px;
      color: var(--text);
      background: var(--panel-2);
      border: 1px solid var(--line);
      font-weight: 680;
    }
    .filter-chip.active {
      color: #101214;
      background: linear-gradient(135deg, var(--accent), #eef9ff);
      border-color: transparent;
    }
    .mode-description {
      margin-top: 12px;
      color: var(--muted);
      line-height: 1.5;
    }
    .mode-tags {
      display: flex;
      flex-wrap: wrap;
      gap: 7px;
      margin-top: 10px;
    }
    .tag {
      border: 1px solid var(--line);
      background: rgba(255,255,255,.05);
      border-radius: 999px;
      padding: 5px 8px;
      color: var(--muted);
      font-size: .78rem;
    }
    .section-label {
      color: var(--muted);
      font-size: .78rem;
      font-weight: 700;
      text-transform: uppercase;
      margin-bottom: 10px;
    }
    .calibration-grid {
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 14px;
      align-items: end;
    }
    .toggle-row {
      min-height: 42px;
      display: flex;
      align-items: center;
      gap: 10px;
      color: var(--text);
    }
    .toggle-row input {
      width: 18px;
      height: 18px;
      accent-color: var(--accent);
    }
    .comfort-actions {
      display: grid;
      grid-template-columns: minmax(150px, 190px) 1fr;
      gap: 12px;
      align-items: start;
    }
    .favorite-bar {
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
      align-items: stretch;
    }
    .favorite-button {
      width: auto;
      min-width: 102px;
      display: inline-grid;
      grid-template-columns: auto 1fr;
      gap: 8px;
      align-items: center;
    }
    .favorite-button.unavailable {
      opacity: .48;
      cursor: not-allowed;
    }
    .favorite-swatch {
      width: 16px;
      height: 16px;
      border-radius: 999px;
      border: 1px solid rgba(0,0,0,.25);
      background: #ffb26d;
    }
    .favorite-message {
      min-height: 20px;
      color: var(--accent-2);
      font-size: .88rem;
      margin-top: 10px;
    }
    .nightguard-grid {
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 14px;
      align-items: end;
    }
    .timer-grid {
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 14px;
      align-items: end;
    }
    .quick-buttons {
      display: flex;
      flex-wrap: wrap;
      gap: 8px;
    }
    .quick-buttons button {
      width: auto;
      min-width: 86px;
    }
    .timer-status {
      min-height: 22px;
      color: var(--accent-2);
      font-size: .9rem;
      margin-top: 12px;
    }
    .mode-control { display: none; }
    .mode-control.show { display: grid; }
    .scene-input-row {
      display: grid;
      grid-template-columns: minmax(180px, 1fr) 180px;
      gap: 12px;
      align-items: end;
    }
    .scene-list {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 12px;
      margin-top: 14px;
    }
    .scene-card {
      border: 1px solid var(--line);
      background: var(--panel-2);
      border-radius: 8px;
      padding: 12px;
      display: grid;
      gap: 10px;
    }
    .scene-title {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 10px;
      font-weight: 720;
    }
    .scene-meta {
      color: var(--muted);
      font-size: .84rem;
      line-height: 1.45;
    }
    .scene-swatch {
      width: 28px;
      height: 28px;
      border-radius: 8px;
      border: 1px solid var(--line);
      flex: 0 0 auto;
    }
    .scene-actions {
      display: grid;
      grid-template-columns: repeat(3, minmax(0, 1fr));
      gap: 8px;
    }
    .scene-message {
      min-height: 20px;
      color: var(--accent-2);
      font-size: .88rem;
      margin-top: 10px;
    }
    .palette-grid {
      display: grid;
      grid-template-columns: minmax(220px, .72fr) 1fr;
      gap: 14px;
      align-items: start;
    }
    .palette-editor {
      display: grid;
      grid-template-columns: minmax(180px, 1fr) repeat(5, minmax(44px, 64px)) 120px;
      gap: 10px;
      align-items: end;
      margin-top: 14px;
    }
    .palette-list {
      display: grid;
      gap: 10px;
    }
    .palette-card {
      border: 1px solid var(--line);
      background: var(--panel-2);
      border-radius: 8px;
      padding: 11px;
      display: grid;
      gap: 9px;
    }
    .palette-title {
      display: flex;
      justify-content: space-between;
      gap: 10px;
      align-items: center;
      font-weight: 720;
    }
    .palette-swatches {
      display: grid;
      grid-auto-flow: column;
      grid-auto-columns: minmax(26px, 1fr);
      gap: 5px;
      min-height: 30px;
    }
    .palette-swatch {
      border: 1px solid var(--line);
      border-radius: 8px;
      min-height: 30px;
    }
    .palette-actions {
      display: grid;
      grid-template-columns: repeat(3, minmax(0, 1fr));
      gap: 8px;
    }
    .palette-message {
      min-height: 20px;
      color: var(--accent-2);
      font-size: .88rem;
      margin-top: 10px;
    }
    .palette-editor input[type="color"] {
      width: 100%;
      height: 42px;
    }
    .management-grid {
      display: grid;
      grid-template-columns: minmax(220px, .7fr) 1fr;
      gap: 14px;
      align-items: start;
    }
    .backup-message {
      min-height: 20px;
      color: var(--accent-2);
      font-size: .88rem;
      margin-top: 10px;
    }
    .preview-wrap {
      display: grid;
      gap: 10px;
    }
    .preview-canvas {
      width: 100%;
      height: 72px;
      border: 1px solid var(--line);
      border-radius: 8px;
      background: #080a0b;
      display: block;
    }
    .preview-meta {
      display: flex;
      justify-content: space-between;
      gap: 12px;
      align-items: center;
      flex-wrap: wrap;
      color: var(--muted);
      font-size: .9rem;
    }
    .surprise-grid {
      display: grid;
      grid-template-columns: minmax(180px, .65fr) minmax(150px, .45fr) minmax(180px, .55fr);
      gap: 12px;
      align-items: end;
    }
    .surprise-message {
      min-height: 22px;
      color: var(--accent-2);
      font-size: .9rem;
      margin-top: 12px;
    }
    .note {
      color: var(--muted);
      font-size: .86rem;
      line-height: 1.5;
      margin: 12px 0 0;
    }
    a { color: var(--accent); }
    @media (max-width: 760px) {
      header { display: block; }
      .pill { display: inline-block; margin-top: 12px; }
      .grid, .summary-grid, .color-row, .mode-grid, .calibration-grid, .comfort-actions, .nightguard-grid, .timer-grid, .scene-input-row, .scene-list, .palette-grid, .palette-editor, .surprise-grid, .management-grid { grid-template-columns: 1fr; }
      .white-wrap { grid-template-columns: 68px 1fr; }
      input[type="color"] { width: 128px; height: 128px; }
      .palette-editor input[type="color"] { width: 100%; height: 54px; }
    }
  </style>
</head>
<body>
<main>
  <header>
    <div>
      <h1>Bedroom LED Controller</h1>
      <div class="sub">ESP32-S3 / ESP8266 addressable strip controller | <a href="/schedule">Schedule</a> | <a href="/diagnostics">Diagnostics</a> | <a href="/ota">OTA</a> | <a href="/api-docs">API</a></div>
    </div>
    <div class="pill" id="status">Connecting...</div>
  </header>
  <div class="global-status pending" id="globalStatus" role="status" aria-live="polite">Connecting...</div>

  <div class="grid">
    <section class="wide">
      <h2>Current Light</h2>
      <div class="summary-grid" id="currentSummary"></div>
    </section>

    <section class="wide">
      <h2>Live Preview</h2>
      <div class="preview-wrap">
        <canvas id="previewCanvas" class="preview-canvas" width="720" height="72"></canvas>
        <div class="preview-meta">
          <span id="previewModeLabel">Preview loading...</span>
          <label class="toggle-row">
            <input id="previewPause" type="checkbox">
            Pause Preview
          </label>
        </div>
        <div class="note" id="previewStatus">Preview: approximate browser-side visualization. The real LED strip may differ slightly.</div>
      </div>
    </section>

    <section class="wide">
      <h2>Master Brightness</h2>
      <div class="control-stack">
        <label>Global output cap
          <input id="masterBrightness" type="range" min="0" max="255" value="180" aria-label="Master brightness">
        </label>
        <div class="brightness-readout">
          <span><span id="masterBrightnessValue">180</span> / 255</span>
          <span id="masterBrightnessPercent">71%</span>
        </div>
      </div>
      <p class="note">This cap is applied in the final output pipeline after mode rendering and calibration.</p>
    </section>

    <section class="wide">
      <h2>Quick Actions</h2><!-- <h2>Comfort</h2> -->
      <div class="comfort-actions">
        <button id="panicWarm" type="button">Warm Dim Now</button>
        <div>
          <div class="quick-buttons" style="margin-bottom:10px">
            <button type="button" data-action-link="/go?action=off">Copy Off Link</button>
            <button type="button" data-action-link="/go?action=warmDimNow">Copy Warm Dim Link</button>
          </div>
          <div class="favorite-bar" id="favoriteBar"></div>
          <div class="favorite-message" id="favoriteMessage"></div>
          <button id="resetFavorites" type="button" style="margin-top:10px;max-width:190px">Reset Favorites</button>
        </div>
      </div>
    </section>

    <section class="wide">
      <h2>Surprise Me</h2>
      <div class="surprise-grid">
        <label>Mood
          <select id="surpriseMood">
            <option value="calm">Calm</option>
            <option value="evening">Evening</option>
            <option value="sleepSafe">Sleep-safe</option>
            <option value="focus">Focus</option>
            <option value="colorful">Colorful</option>
            <option value="cozy">Cozy</option>
            <option value="weather">Weather</option>
            <option value="nature">Nature</option>
          </select>
        </label>
        <button id="surpriseButton" type="button">Surprise Me</button>
        <button id="saveSurpriseScene" type="button">Save Surprise as Scene</button>
      </div>
      <div class="surprise-message" id="surpriseSummary">No surprise applied yet.</div>
    </section>

    <section>
      <h2>Color and White Temperature</h2>
      <div class="control-stack">
        <div class="color-row">
          <input id="colorPicker" type="color" value="#ffb26d" aria-label="Strip color">
          <div>
            <div class="swatch" id="swatch"></div>
            <p class="note">The last color input becomes the active strip color. Animations use this color unless Rainbow is selected.</p>
          </div>
        </div>
      </div>
    </section>

    <section class="wide">
      <h2>Output Calibration</h2>
      <div class="calibration-grid">
        <label>Gamma
          <span class="toggle-row">
            <input id="gammaEnabled" type="checkbox" checked>
            Enabled
          </span>
        </label>
        <label>Red gain
          <input id="redGain" type="range" min="0" max="255" value="255" aria-label="Red gain">
        </label>
        <label>Green gain
          <input id="greenGain" type="range" min="0" max="255" value="255" aria-label="Green gain">
        </label>
        <label>Blue gain
          <input id="blueGain" type="range" min="0" max="255" value="255" aria-label="Blue gain">
        </label>
        <button id="resetCalibration" type="button">Reset Calibration</button>
      </div>
      <p class="note">RGB gains are applied before master brightness and gamma. Defaults keep the strip neutral.</p>
    </section>

    <section>
      <h2>White Temperature</h2>
      <div class="white-wrap">
        <div class="temp-rail">
          <input id="kelvin" type="range" min="2700" max="6500" step="50" value="3000" aria-label="White temperature">
        </div>
        <div>
          <div class="temp-value"><span id="kelvinText">3000</span>K</div>
          <p class="note">Warm bedroom white at the bottom, crisp daylight at the top.</p>
        </div>
      </div>
    </section>

    <section class="wide">
      <h2>Mode</h2>
      <div class="mode-filter-bar" id="modeFilterBar"></div>
      <div class="mode-grid">
        <label>Mode
          <select id="mode">
            <option value="solid">Solid color</option>
            <option value="strobe">Strobe</option>
            <option value="flash">Flash</option>
            <option value="chase">Chase</option>
            <option value="wave">Wave</option>
            <option value="slowWave">Slow Wave</option>
            <option value="slowPulse">Slow Pulse</option>
            <option value="rainbow">Rainbow</option>
            <option value="dawnBloom">Dawn Bloom</option>
            <option value="moonTide">Moon Tide</option>
            <option value="velvetAurora">Velvet Aurora</option>
            <option value="lanternDrift">Lantern Drift</option>
            <option value="rainGlass">Rain Glass</option>
            <option value="satinBreathing">Satin Breathing</option>
            <option value="paperLanternRow">Paper Lantern Row</option>
            <option value="libraryLamp">Library Lamp</option>
            <option value="solarNoon">Solar Noon</option>
            <option value="goldenHour">Golden Hour</option>
            <option value="dreamAquarium">Dream Aquarium</option>
            <option value="circuitGlow">Circuit Glow</option>
            <option value="windowSunrise">Window Sunrise</option>
            <option value="emberQuilt">Ember Quilt</option>
            <option value="porcelainSky">Porcelain Sky</option>
            <option value="hearthline">Hearthline</option>
            <option value="eveningCurrent">Evening Current</option>
            <option value="starlitLinen">Starlit Linen</option>
            <option value="deepSeaGlass">Deep Sea Glass</option>
            <option value="quietNorthernLights">Quiet Northern Lights</option>
            <option value="candleHall">Candle Hall</option>
            <option value="slowPrism">Slow Prism</option>
            <option value="gardenShade">Garden Shade</option>
            <option value="snowfallHush">Snowfall Hush</option>
            <option value="observatoryGlow">Observatory Glow</option>
            <option value="rainyWindow">Rainy Window</option>
            <option value="campfireBlanket">Campfire Blanket</option>
            <option value="northernWhisper">Northern Whisper</option>
            <option value="stormOutside">Storm Outside</option>
          </select>
        </label>

        <label>Boot behavior
          <select id="bootBehavior">
            <option value="restore">Restore last state</option>
            <option value="startOff">Start off</option>
            <option value="warmDim">Start warm dim</option>
          </select>
        </label>

        <label>Animation Strength
          <input id="animationStrength" type="range" min="0" max="255" step="1" value="128" aria-label="Animation Strength">
          <span id="animationStrengthValue">Balanced - 128 / 255 - 50%</span>
          <span class="note">Subtle to Expressive</span>
        </label>

        <label class="mode-control" data-for="strobe">Strobe delay
          <select id="strobeDelay"></select>
        </label>
        <label class="mode-control" data-for="flash">Flash off delay
          <select id="flashOffDelay"></select>
        </label>
        <label class="mode-control" data-for="chase">Chase loop time
          <select id="chasePeriod"></select>
        </label>
        <label class="mode-control" data-for="wave">Wave loop time
          <select id="wavePeriod"></select>
        </label>
        <label class="mode-control" data-for="slowPulse">Pulse count
          <select id="slowPulseCount"></select>
        </label>
        <label class="mode-control" data-for="slowPulse">Pulse life
          <select id="slowPulseMax"></select>
        </label>
        <label class="mode-control" data-for="rainbow">Rainbow loop time
          <select id="rainbowPeriod"></select>
        </label>
        <label class="mode-control" data-for="candleHall">Candle flicker
          <input id="candleHallFlickerAmount" type="range" min="0" max="20" step="1" value="3" aria-label="Candle Hall flicker amount">
          <span id="candleHallFlickerAmountValue">3 / 20</span>
        </label>
      </div>
      <div class="mode-description" id="modeDescription"></div>
      <div class="mode-tags" id="modeTags"></div>
      <p class="note">Animation Strength controls motion, contrast, shimmer, and color spread inside animated modes. Master Brightness still controls total light output.</p>
      <p class="note">OTA updates are available from <a href="/ota">the OTA page</a>. Setup tools are on <a href="/diagnostics">Diagnostics</a>.</p>
    </section>

    <section class="wide">
      <h2>Palettes</h2>
      <div class="palette-grid">
        <div class="control-stack">
          <label>Enable for compatible modes
            <span class="toggle-row">
              <input id="paletteEnabled" type="checkbox">
              Enabled
            </span>
          </label>
          <label>Active palette
            <select id="activePaletteId"></select>
          </label>
          <div class="palette-swatches" id="activePaletteSwatches"></div>
          <div class="note" id="paletteSupportNote">Palette support loading...</div>
          <button id="resetPalettes" type="button">Reset Palettes</button>
        </div>
        <div>
          <div class="palette-list" id="paletteList"></div>
          <div class="palette-message" id="paletteMessage"></div>
        </div>
      </div>
      <div class="palette-editor">
        <label>Name
          <input id="paletteName" type="text" maxlength="32" placeholder="Soft Blue">
        </label>
        <label>Color 1<input id="paletteColor1" type="color" value="#FFE6B8"></label>
        <label>Color 2<input id="paletteColor2" type="color" value="#FFD9A0"></label>
        <label>Color 3<input id="paletteColor3" type="color" value="#F4C27A"></label>
        <label>Color 4<input id="paletteColor4" type="color" value="#FFF3D6"></label>
        <label>Color 5<input id="paletteColor5" type="color" value="#000000"></label>
        <button id="savePalette" type="button">Save Palette</button>
      </div>
    </section>

    <section class="wide">
      <h2>Night Guard</h2>
      <div class="nightguard-grid">
        <label>Enable
          <span class="toggle-row">
            <input id="nightGuardEnabled" type="checkbox">
            Night Guard
          </span>
        </label>
        <label>Max brightness
          <input id="nightGuardMaxBrightness" type="range" min="0" max="255" value="80" aria-label="Night Guard max brightness">
        </label>
        <label>Block flashing
          <span class="toggle-row">
            <input id="nightGuardBlockFlashing" type="checkbox" checked>
            Strobe / Flash
          </span>
        </label>
        <label>Prefer warm
          <span class="toggle-row">
            <input id="nightGuardPreferWarm" type="checkbox">
            Warm bias
          </span>
        </label>
      </div>
      <p class="note" id="nightGuardNote">Night Guard is off.</p>
    </section>

    <section class="wide">
      <div class="section-label">Timers / Bedtime</div>
      <h2>Sleep Timer</h2>
      <div class="timer-grid">
        <label>Minutes
          <input id="timerDuration" type="number" min="1" max="240" value="30">
        </label>
        <label>Timer mode
          <select id="timerMode">
            <option value="delay">Wait, then action</option>
            <option value="fade">Gradually fade</option>
          </select>
        </label>
        <label>End action
          <select id="timerAction">
            <option value="off">Turn Off</option>
            <option value="warmDimNow">Warm Dim Now</option>
            <option value="scene">Load Scene</option>
          </select>
        </label>
        <label>Scene
          <select id="timerScene"></select>
        </label>
      </div>
      <div class="quick-buttons" style="margin-top:12px">
        <button type="button" data-timer-minutes="15">15 min</button>
        <button type="button" data-timer-minutes="30">30 min</button>
        <button type="button" data-timer-minutes="60">1 hour</button>
        <button type="button" data-timer-minutes="120">2 hours</button>
      </div>
      <div class="timer-grid" style="margin-top:12px">
        <button id="startTimer" type="button">Start Timer</button>
        <button id="cancelTimer" type="button">Cancel Timer</button>
      </div>
      <div class="timer-status" id="timerStatus">No active timer.</div>
    </section>

    <section class="wide">
      <h2>Bedtime Fade</h2>
      <div class="timer-grid">
        <label>Duration
          <select id="bedtimeMinutes">
            <option value="10">10 min</option>
            <option value="20">20 min</option>
            <option value="30" selected>30 min</option>
            <option value="45">45 min</option>
            <option value="60">60 min</option>
          </select>
        </label>
        <label>Target
          <select id="bedtimeTarget">
            <option value="warmDim">Warm Dim</option>
            <option value="sleep">Sleep scene</option>
            <option value="off">Off</option>
          </select>
        </label>
        <button id="startBedtimeFade" type="button">Start Bedtime Fade</button>
        <button id="cancelBedtimeFade" type="button">Cancel</button>
      </div>
    </section>

    <section class="wide">
      <h2>Scenes</h2>
      <div class="scene-input-row">
        <label>Scene name
          <input id="sceneName" type="text" maxlength="32" placeholder="Reading">
        </label>
        <button id="saveScene" type="button">Save Current Scene</button>
      </div>
      <div class="scene-input-row" style="margin-top:12px">
        <label>Import Scene JSON
          <textarea id="sceneImportJson" spellcheck="false"></textarea>
        </label>
        <div class="control-stack">
          <button id="importSceneJson" type="button">Import Scene JSON</button>
          <button id="restoreBuiltInScenes" type="button">Restore Built-In Scenes</button>
        </div>
      </div>
      <div class="note" id="sceneCount">Scenes loading...</div>
      <div class="scene-message" id="sceneMessage"></div>
      <div class="scene-list" id="sceneList"></div>
    </section>

    <section class="wide">
      <h2>Transitions</h2>
      <div class="calibration-grid">
        <label>Enable smooth transitions
          <span class="toggle-row">
            <input id="transitionsEnabled" type="checkbox">
            Enabled
          </span>
        </label>
        <label>Duration
          <select id="transitionDurationMs">
            <option value="0">Instant</option>
            <option value="500">0.5 s</option>
            <option value="1500">1.5 s</option>
            <option value="3000">3 s</option>
            <option value="5000">5 s</option>
          </select>
        </label>
        <label>Style
          <select id="transitionStyle">
            <option value="fade">Fade</option>
          </select>
        </label>
        <div class="timer-status" id="transitionStatus">Transitions ready.</div>
      </div>
    </section>

    <section class="wide">
      <div class="section-label">Advanced / Diagnostics</div>
      <h2>Backup / Maintenance</h2>
      <div class="management-grid">
        <div class="control-stack">
          <button id="exportFullBackup" type="button">Export Full Backup</button>
          <button id="importFullBackup" type="button">Import Full Backup</button>
          <button id="factoryReset" type="button">Factory Reset</button>
          <button type="button" onclick="location.href='/diagnostics'">Diagnostics</button>
          <button type="button" onclick="location.href='/ota'">OTA</button>
          <button type="button" onclick="location.href='/api-docs'">API Docs</button>
        </div>
        <label>Backup JSON
          <textarea id="backupExportJson" spellcheck="false"></textarea>
        </label>
      </div>
      <div class="backup-message" id="backupMessage"></div>
    </section>
  </div>
</main>

<script>
const $ = (id) => document.getElementById(id);
let debounceTimer = null;
let sceneCache = [];
let favoriteCache = [];
let paletteCache = [];
let editingPaletteId = '';
let timerPoll = null;
let modeMetadata = [];
let activeModeFilter = 'all';
let lastState = null;
const PREVIEW_PIXELS = 48;
let previewState = null;
let previewPaused = false;
let previewLastDrawMs = 0;
const previewReducedMotion = window.matchMedia && window.matchMedia('(prefers-reduced-motion: reduce)').matches;
let globalStatusClearTimer = null;
let heavyRequestQueue = Promise.resolve();
let heavyRequestActive = false;
let heavyRequestQueued = false;
let nightGuardDebounceTimer = null;
let animationStrengthDebounceTimer = null;
let uiHydrating = true;
let stateLoaded = false;

const HEAVY_ENDPOINTS = [
  '/api/scenes',
  '/api/palettes',
  '/api/modes',
  '/api/backup/export',
  '/api/diagnostics',
  '/api/resources'
];

const MODE_FILTERS = [
  ['all', 'All'],
  ['everyday', 'Everyday'],
  ['calm', 'Calm'],
  ['sleepSafe', 'Sleep-safe'],
  ['focusFriendly', 'Focus'],
  ['colorful', 'Colorful'],
  ['flashing', 'Flashing'],
  ['utility', 'Utility'],
  ['weather', 'Weather']
];

function addOptions(select, start, end, step, suffix, decimals = 0) {
  for (let value = start; value <= end + 0.0001; value += step) {
    const option = document.createElement('option');
    const label = Number(value).toFixed(decimals);
    option.value = label;
    option.textContent = `${label}${suffix}`;
    select.appendChild(option);
  }
}

addOptions($('strobeDelay'), 0.1, 10, 0.1, ' s', 1);
addOptions($('flashOffDelay'), 0.1, 10, 0.1, ' s', 1);
addOptions($('chasePeriod'), 1, 15, 1, ' s');
addOptions($('wavePeriod'), 5, 30, 1, ' s');
addOptions($('slowPulseCount'), 1, 15, 1, ' pulses');
addOptions($('slowPulseMax'), 1, 60, 1, ' s');
addOptions($('rainbowPeriod'), 1, 30, 1, ' s');

function qs(params) {
  return Object.entries(params)
    .filter(([, value]) => value !== undefined)
    .map(([key, value]) => `${encodeURIComponent(key)}=${encodeURIComponent(value)}`)
    .join('&');
}

function delay(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

function showGlobalStatus(message, type = 'pending', autoClearMs) {
  const target = $('globalStatus');
  if (!target) {
    return;
  }
  clearTimeout(globalStatusClearTimer);
  target.textContent = message || '';
  target.className = `global-status ${type}`;
  const clearAfter = autoClearMs ?? (type === 'success' ? 3200 : 0);
  if (clearAfter > 0) {
    globalStatusClearTimer = setTimeout(() => {
      if (target.textContent === message) {
        target.textContent = lastState ? 'Connected' : '';
        target.className = 'global-status success';
      }
    }, clearAfter);
  }
}

function logApiFailure(url, statusCode, error, rawText = '') {
  const rawTextPreview = String(rawText || '').slice(0, 200);
  console.warn('API request failed', {
    url,
    statusCode,
    error: error?.message || String(error || 'Request failed'),
    rawTextPreview,
    time: new Date().toISOString()
  });
}

function endpointPath(url) {
  return String(url || '').split('?')[0];
}

function isHeavyEndpoint(url) {
  return HEAVY_ENDPOINTS.includes(endpointPath(url));
}

async function runQueuedHeavyRequest(work) {
  if (heavyRequestActive || heavyRequestQueued) {
    showGlobalStatus('Controller busy - retrying', 'pending');
  }
  heavyRequestQueued = true;
  const run = heavyRequestQueue.catch(() => {}).then(async () => {
    heavyRequestQueued = false;
    heavyRequestActive = true;
    try {
      return await work();
    } finally {
      heavyRequestActive = false;
    }
  });
  heavyRequestQueue = run.catch(() => {});
  return run;
}

async function apiFetchJson(url, options = {}) {
  const {
    label = url,
    timeoutMs = 4500,
    retries = 0,
    retryDelayMs = 250,
    showStatus = false,
    expectOk = true,
    heavy = isHeavyEndpoint(url)
  } = options;

  const doRequest = async () => {
    let attempt = 0;
    while (attempt <= retries) {
      const controller = new AbortController();
      const timeoutId = setTimeout(() => controller.abort(), timeoutMs);
      let statusCode = 0;
      let rawText = '';
      try {
        const response = await fetch(url, { cache: 'no-store', signal: controller.signal });
        statusCode = response.status;
        rawText = await response.text();
        let payload = {};
        try {
          payload = rawText ? JSON.parse(rawText) : {};
        } catch (error) {
          logApiFailure(url, statusCode, error, rawText);
          return {
            ok: false,
            error: `${label} returned invalid JSON.`,
            statusCode,
            rawText
          };
        }
        if (!response.ok) {
          const error = payload.error || payload.message || `${label} failed with HTTP ${statusCode}.`;
          logApiFailure(url, statusCode, error, rawText);
          return { ok: false, error, statusCode, rawText, payload };
        }
        if (expectOk && payload && payload.ok === false) {
          const error = payload.error || payload.message || `${label} failed.`;
          logApiFailure(url, statusCode, error, rawText);
          return { ok: false, error, statusCode, rawText, payload };
        }
        return { ok: true, payload, statusCode };
      } catch (error) {
        const timedOut = error && error.name === 'AbortError';
        logApiFailure(url, statusCode, error, rawText);
        if (attempt < retries) {
          if (showStatus) {
            showGlobalStatus('Controller busy - retrying', 'pending');
          }
          await delay(retryDelayMs);
          attempt++;
          continue;
        }
        return {
          ok: false,
          error: timedOut ? `${label} timed out.` : `${label} did not respond.`,
          statusCode,
          rawText
        };
      } finally {
        clearTimeout(timeoutId);
      }
    }
    return { ok: false, error: `${label} failed.`, statusCode: 0, rawText: '' };
  };

  const result = heavy ? await runQueuedHeavyRequest(doRequest) : await doRequest();
  if (!result.ok && showStatus) {
    showGlobalStatus(result.error || `${label} failed.`, 'error');
  }
  return result;
}

function renderUnavailableSection(containerId, message, retrySection) {
  $(containerId).innerHTML = `
    <div class="note">
      ${escapeHtml(message)}
      <br>
      <button class="inline-retry" type="button" data-retry-section="${retrySection}">Retry</button>
    </div>
  `;
}

function canMutateFromUi(action = 'update') {
  if (uiHydrating) {
    console.warn('Blocked UI mutation during hydration', { action });
    return false;
  }
  if (!stateLoaded) {
    showGlobalStatus('State unavailable - controls preserved', 'error');
    console.warn('Blocked UI mutation before state loaded', { action });
    return false;
  }
  return true;
}

function setControlsDisabled(ids, disabled) {
  ids.forEach((id) => {
    const control = $(id);
    if (control) {
      control.disabled = disabled;
    }
  });
}

function setButtonBusy(id, busy, busyText) {
  const button = $(id);
  if (!button) {
    return () => {};
  }
  const previousText = button.textContent;
  button.disabled = busy;
  if (busy && busyText) {
    button.textContent = busyText;
  }
  return () => {
    button.disabled = false;
    button.textContent = previousText;
  };
}

function messageForParams(params) {
  if ('masterBrightness' in params) return ['Saving brightness...', 'Brightness saved'];
  if ('animationStrength' in params) return ['Saving Animation Strength...', 'Animation Strength saved'];
  if ('color' in params) return ['Saving color...', 'Color saved'];
  if ('kelvin' in params) return ['Saving white temperature...', 'White temperature saved'];
  if ('mode' in params) return ['Changing mode...', 'Mode changed'];
  if ('gammaEnabled' in params) return ['Saving gamma...', 'Gamma saved'];
  if ('redGain' in params || 'greenGain' in params || 'blueGain' in params || 'resetCalibration' in params) {
    return ['Saving calibration...', 'Calibration saved'];
  }
  return ['Saving setting...', 'Setting saved'];
}

function hexToRgb(hex) {
  const clean = String(hex || '').replace('#', '').trim();
  if (clean.length !== 6) {
    return { r: 255, g: 178, b: 109 };
  }
  return {
    r: parseInt(clean.slice(0, 2), 16) || 0,
    g: parseInt(clean.slice(2, 4), 16) || 0,
    b: parseInt(clean.slice(4, 6), 16) || 0
  };
}

function rgbToCss(rgb) {
  return `rgb(${Math.round(rgb.r)}, ${Math.round(rgb.g)}, ${Math.round(rgb.b)})`;
}

function scaleRgb(rgb, brightness) {
  const scale = Math.max(0, Math.min(255, Number(brightness ?? 255))) / 255;
  return {
    r: Math.max(0, Math.min(255, rgb.r * scale)),
    g: Math.max(0, Math.min(255, rgb.g * scale)),
    b: Math.max(0, Math.min(255, rgb.b * scale))
  };
}

function blendRgb(a, b, amount) {
  const t = Math.max(0, Math.min(1, Number(amount || 0)));
  return {
    r: a.r + (b.r - a.r) * t,
    g: a.g + (b.g - a.g) * t,
    b: a.b + (b.b - a.b) * t
  };
}

function samplePaletteColor(colors, position) {
  if (!colors || !colors.length) {
    return null;
  }
  const palette = colors.map(hexToRgb);
  if (palette.length === 1) {
    return palette[0];
  }
  const p = ((Number(position || 0) % 1) + 1) % 1;
  const scaled = p * palette.length;
  const index = Math.floor(scaled) % palette.length;
  const next = (index + 1) % palette.length;
  return blendRgb(palette[index], palette[next], scaled - index);
}

function kelvinToApproxRgb(kelvin) {
  const temp = Math.max(1000, Math.min(40000, Number(kelvin || 3000))) / 100;
  let red;
  let green;
  let blue;
  if (temp <= 66) {
    red = 255;
    green = 99.4708025861 * Math.log(temp) - 161.1195681661;
    blue = temp <= 19 ? 0 : 138.5177312231 * Math.log(temp - 10) - 305.0447927307;
  } else {
    red = 329.698727446 * Math.pow(temp - 60, -0.1332047592);
    green = 288.1221695283 * Math.pow(temp - 60, -0.0755148492);
    blue = 255;
  }
  return {
    r: Math.max(0, Math.min(255, red)),
    g: Math.max(0, Math.min(255, green)),
    b: Math.max(0, Math.min(255, blue))
  };
}

function activePreviewPaletteColors(state = previewState || lastState) {
  const active = paletteById(state?.activePaletteId);
  return active?.colors || [];
}

function previewBaseColor(state = previewState || lastState) {
  const base = state?.usingWhiteTemperature
    ? kelvinToApproxRgb(state?.kelvin)
    : hexToRgb(state?.hex || '#ffb26d');
  return scaleRgb(base, state?.effectiveBrightness ?? state?.masterBrightness ?? 180);
}

function previewBlack() {
  return { r: 0, g: 0, b: 0 };
}

function makePreviewPixels(renderer) {
  return Array.from({ length: PREVIEW_PIXELS }, (_, index) => renderer(index, index / PREVIEW_PIXELS));
}

function hsvToRgb(hue, saturation, value) {
  const h = ((hue % 360) + 360) % 360;
  const c = value * saturation;
  const x = c * (1 - Math.abs((h / 60) % 2 - 1));
  const m = value - c;
  let rgb = { r: 0, g: 0, b: 0 };
  if (h < 60) rgb = { r: c, g: x, b: 0 };
  else if (h < 120) rgb = { r: x, g: c, b: 0 };
  else if (h < 180) rgb = { r: 0, g: c, b: x };
  else if (h < 240) rgb = { r: 0, g: x, b: c };
  else if (h < 300) rgb = { r: x, g: 0, b: c };
  else rgb = { r: c, g: 0, b: x };
  return {
    r: (rgb.r + m) * 255,
    g: (rgb.g + m) * 255,
    b: (rgb.b + m) * 255
  };
}

function paletteOrBase(state, position, fallback) {
  return samplePaletteColor(activePreviewPaletteColors(state), position) || fallback;
}

function previewStrength01(state = previewState || lastState) {
  const raw = Number(state?.animationStrength ?? state?.animationIntensity ?? 128);
  return Math.max(0, Math.min(255, raw)) / 255;
}

function previewStrengthEase(state = previewState || lastState) {
  const s = previewStrength01(state);
  return s * s * (3 - 2 * s);
}

function renderPreviewSolid(state, timeMs) {
  const base = previewBaseColor(state);
  return makePreviewPixels(() => base);
}

function renderPreviewStrobe(state, timeMs) {
  const strength = previewStrength01(state);
  const seconds = Math.max(.1, Number(state?.strobeDelay || 0.5));
  const on = Math.floor(timeMs / (seconds * 1000)) % 2 === 0;
  const base = previewBaseColor(state);
  return makePreviewPixels(() => on ? base : previewBlack());
}

function renderPreviewFlash(state, timeMs) {
  const strength = previewStrength01(state);
  const seconds = Math.max(.1, Number(state?.flashOffDelay || 0.5));
  const on = (timeMs % (seconds * 2000)) < 120;
  const base = previewBaseColor(state);
  return makePreviewPixels(() => on ? base : previewBlack());
}

function renderPreviewChase(state, timeMs) {
  const strength = previewStrength01(state);
  const base = previewBaseColor(state);
  const period = Math.max(1, Number(state?.chasePeriod || 5)) * 1000;
  const head = Math.floor((timeMs % period) / period * PREVIEW_PIXELS);
  return makePreviewPixels((index) => {
    const distance = Math.min(Math.abs(index - head), PREVIEW_PIXELS - Math.abs(index - head));
    const intensity = distance === 0 ? 1 : distance === 1 ? .42 : .12;
    return scaleRgb(base, (state?.effectiveBrightness ?? 180) * intensity);
  });
}

function renderPreviewWave(state, timeMs) {
  const strength = previewStrength01(state);
  const base = previewBaseColor(state);
  const period = Math.max(1, Number(state?.wavePeriod || 12)) * 1000;
  const phase = (timeMs % period) / period;
  return makePreviewPixels((index, position) => {
    const wave = (Math.sin((position + phase) * Math.PI * 2) + 1) / 2;
    return blendRgb(scaleRgb(base, 50), base, wave);
  });
}

function renderPreviewSlowWave(state, timeMs) {
  const strength = previewStrength01(state);
  return renderPreviewWave({ ...state, wavePeriod: Math.max(10, Number(state?.wavePeriod || 18)) }, timeMs * .7);
}

function renderPreviewSlowPulse(state, timeMs) {
  const strength = previewStrength01(state);
  const base = previewBaseColor(state);
  const period = Math.max(2, Number(state?.slowPulseMax || 20)) * 1000;
  const pulse = (Math.sin((timeMs % period) / period * Math.PI * 2) + 1) / 2;
  const dim = Math.max(18, (state?.effectiveBrightness ?? 180) * .16);
  const bright = state?.effectiveBrightness ?? state?.masterBrightness ?? 180;
  return makePreviewPixels(() => scaleRgb(base, dim + (bright - dim) * pulse));
}

function renderPreviewRainbow(state, timeMs) {
  const strength = previewStrength01(state);
  const period = Math.max(1, Number(state?.rainbowPeriod || 12)) * 1000;
  const phase = (timeMs % period) / period;
  return makePreviewPixels((index, position) => scaleRgb(hsvToRgb((position + phase) * 360, .88, 1), state?.effectiveBrightness ?? 180));
}

function renderPreviewDawnBloom(state, timeMs) {
  const strength = previewStrength01(state);
  const warm = scaleRgb({ r: 255, g: 166, b: 86 }, state?.effectiveBrightness ?? 180);
  const rose = scaleRgb({ r: 255, g: 116, b: 132 }, state?.effectiveBrightness ?? 180);
  return makePreviewPixels((index, position) => blendRgb(warm, rose, (Math.sin(position * Math.PI + timeMs / 3800) + 1) / 2));
}

function renderPreviewMoonTide(state, timeMs) {
  const strength = previewStrength01(state);
  const base = scaleRgb({ r: 112, g: 172, b: 255 }, state?.effectiveBrightness ?? 180);
  const deep = scaleRgb({ r: 18, g: 42, b: 86 }, state?.effectiveBrightness ?? 180);
  return makePreviewPixels((index, position) => blendRgb(deep, base, (Math.sin(position * Math.PI * 2 + timeMs / 2600) + 1) / 2));
}

function renderPreviewVelvetAurora(state, timeMs) {
  const strength = previewStrength01(state);
  const colors = activePreviewPaletteColors(state);
  const base = previewBaseColor(state);
  return makePreviewPixels((index, position) => scaleRgb(
    paletteOrBase(state, position + timeMs / 9000 + Math.sin(position * 8 + timeMs / 2400) * .04, base),
    state?.effectiveBrightness ?? 180
  ));
}

function renderPreviewLanternDrift(state, timeMs) {
  const strength = previewStrength01(state);
  const amber = scaleRgb({ r: 255, g: 177, b: 92 }, state?.effectiveBrightness ?? 180);
  const ember = scaleRgb({ r: 255, g: 94, b: 42 }, state?.effectiveBrightness ?? 180);
  return makePreviewPixels((index, position) => blendRgb(amber, ember, .18 + .28 * ((Math.sin(timeMs / 1000 + position * 17) + 1) / 2)));
}

function renderPreviewRainGlass(state, timeMs) {
  const strength = previewStrength01(state);
  const base = scaleRgb({ r: 52, g: 98, b: 132 }, state?.effectiveBrightness ?? 180);
  const glint = scaleRgb({ r: 174, g: 226, b: 255 }, state?.effectiveBrightness ?? 180);
  return makePreviewPixels((index, position) => {
    const dropA = Math.abs((((timeMs / 3600) + position) % 1) - .5);
    const dropB = Math.abs((((timeMs / 5100) + position * 1.7) % 1) - .5);
    const sparkle = dropA < .035 || dropB < .022 ? .78 : .12;
    return blendRgb(base, glint, sparkle);
  });
}

function renderPreviewSatinBreathing(state, timeMs) {
  const strength = previewStrength01(state);
  const phase = (timeMs % 11000) / 11000;
  const envelope = phase < .38
    ? (1 - Math.cos((phase / .38) * Math.PI)) / 2
    : phase < .5 ? 1 : 1 - ((1 - Math.cos(((phase - .5) / .5) * Math.PI)) / 2);
  const base = previewBaseColor(state);
  const warm = scaleRgb({ r: 255, g: 186, b: 112 }, state?.effectiveBrightness ?? 180);
  return makePreviewPixels((index) => {
    const texture = ((index * 17 + Math.floor(timeMs / 900)) % 9 - 4) / 80;
    return scaleRgb(blendRgb(base, warm, envelope * .18), (state?.effectiveBrightness ?? 180) * (.28 + envelope * .68 + texture));
  });
}

function renderPreviewPaperLanternRow(state, timeMs) {
  const strength = previewStrength01(state);
  const warm = scaleRgb({ r: 255, g: 178, b: 88 }, state?.effectiveBrightness ?? 180);
  const count = 5;
  return makePreviewPixels((index, position) => {
    let level = .08;
    let color = warm;
    for (let lantern = 0; lantern < count; lantern++) {
      const center = count === 1 ? .5 : lantern / (count - 1);
      const sway = Math.sin(timeMs / (7200 + lantern * 900) + lantern) * .025;
      const distance = Math.abs(position - center - sway);
      const falloff = Math.max(0, 1 - distance / .18);
      const breathe = .78 + .22 * Math.sin(timeMs / (3400 + lantern * 400) + lantern);
      level += falloff * falloff * breathe * .62;
      const sampled = samplePaletteColor(activePreviewPaletteColors(state), center);
      if (sampled && falloff > .35) color = blendRgb(sampled, warm, .58);
    }
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * Math.min(1, level));
  });
}

function renderPreviewLibraryLamp(state, timeMs) {
  const strength = previewStrength01(state);
  const warm = scaleRgb(kelvinToApproxRgb(state?.kelvin || 3400), state?.effectiveBrightness ?? 180);
  const colors = activePreviewPaletteColors(state);
  return makePreviewPixels((index, position) => {
    const sampled = samplePaletteColor(colors, position);
    const texture = ((index * 13) % 7) / 140;
    const color = sampled ? blendRgb(warm, sampled, .12) : warm;
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.88 + texture));
  });
}

function renderPreviewSolarNoon(state, timeMs) {
  const strength = previewStrength01(state);
  const warm = kelvinToApproxRgb(4550);
  const cool = kelvinToApproxRgb(5400);
  const drift = (Math.sin(timeMs / 55000) + 1) / 2;
  return makePreviewPixels((index, position) => {
    const amount = Math.max(0, Math.min(1, position * .86 + drift * .14));
    return scaleRgb(blendRgb(warm, cool, amount), state?.effectiveBrightness ?? 180);
  });
}

function renderPreviewGoldenHour(state, timeMs) {
  const strength = previewStrength01(state);
  const sun = { r: 255, g: 174, b: 72 };
  const cream = kelvinToApproxRgb(3100);
  const source = (Math.sin(timeMs / 33000) + 1) / 2;
  return makePreviewPixels((index, position) => {
    const glow = Math.max(0, 1 - Math.abs(position - source) * 1.35);
    const palette = samplePaletteColor(activePreviewPaletteColors(state), position);
    const color = palette ? blendRgb(blendRgb(sun, cream, 1 - glow), palette, .16) : blendRgb(sun, cream, 1 - glow);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.36 + glow * .58));
  });
}

function renderPreviewDreamAquarium(state, timeMs) {
  const strength = previewStrength01(state);
  const deep = { r: 18, g: 66, b: 100 };
  const aqua = { r: 62, g: 190, b: 190 };
  return makePreviewPixels((index, position) => {
    const wave = (Math.sin(position * Math.PI * 3 - timeMs / 3000) + 1) / 2;
    const shimmer = (Math.sin(position * Math.PI * 9 + timeMs / 5200) + 1) / 2;
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), position + timeMs / 18000);
    const color = sampled ? blendRgb(blendRgb(deep, aqua, wave), sampled, .36) : blendRgb(deep, aqua, wave);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.34 + wave * .28 + shimmer * .18));
  });
}

function renderPreviewCircuitGlow(state, timeMs) {
  const strength = previewStrength01(state);
  const base = { r: 6, g: 24, b: 34 };
  const trace = samplePaletteColor(activePreviewPaletteColors(state), timeMs / 9000) || { r: 22, g: 255, b: 162 };
  return makePreviewPixels((index, position) => {
    let pulse = .08;
    for (let lane = 0; lane < 4; lane++) {
      const head = ((timeMs / (1800 + lane * 470)) + lane * .23) % 1;
      const distance = Math.min(Math.abs(position - head), 1 - Math.abs(position - head));
      pulse += Math.max(0, 1 - distance / .13) * .34;
    }
    return scaleRgb(blendRgb(base, trace, Math.min(1, pulse)), (state?.effectiveBrightness ?? 180) * Math.min(.82, .18 + pulse));
  });
}

function renderPreviewWindowSunrise(state, timeMs) {
  const strength = previewStrength01(state);
  const amber = { r: 255, g: 134, b: 54 };
  const daylight = kelvinToApproxRgb(3650);
  const reach = (timeMs % 150000) / 150000;
  return makePreviewPixels((index, position) => {
    const boundary = reach * 1.28 - .12;
    const lit = Math.max(0, Math.min(1, 1 - (position - boundary) / .48));
    const smooth = lit * lit * (3 - 2 * lit);
    const palette = samplePaletteColor(activePreviewPaletteColors(state), position) || daylight;
    const color = blendRgb(blendRgb(amber, daylight, reach * .58 + position * .15), palette, .1);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.16 + smooth * .76));
  });
}

function renderPreviewEmberQuilt(state, timeMs) {
  const strength = previewStrength01(state);
  const warm = { r: 255, g: 128, b: 55 };
  const cream = kelvinToApproxRgb(2550);
  return makePreviewPixels((index, position) => {
    let level = .18;
    let color = blendRgb(warm, cream, .24);
    for (let zone = 0; zone < 5; zone++) {
      const center = (zone + .5) / 5 + Math.sin(timeMs / (46000 + zone * 5000) + zone) * .045;
      const falloff = Math.max(0, 1 - Math.abs(position - center) / (.18 + zone * .012));
      const breathe = .74 + .26 * Math.sin(timeMs / (18000 + zone * 1400) + zone);
      level += falloff * falloff * breathe * .36;
      const sampled = samplePaletteColor(activePreviewPaletteColors(state), zone / 5);
      if (sampled && falloff > .35) color = blendRgb(color, sampled, .14);
    }
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * Math.min(.8, level));
  });
}

function renderPreviewPorcelainSky(state, timeMs) {
  const strength = previewStrength01(state);
  const porcelain = kelvinToApproxRgb(5100);
  const pearl = kelvinToApproxRgb(4200);
  return makePreviewPixels((index, position) => {
    const drift = (Math.sin(position * Math.PI * 1.45 + timeMs / 45000) + 1) / 2;
    const cloud = (Math.sin(position * Math.PI * 3.8 - timeMs / 70000) + 1) / 2;
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), position);
    let color = blendRgb(porcelain, pearl, .18 + drift * .28);
    if (sampled) color = blendRgb(color, sampled, .13);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.74 + cloud * .13));
  });
}

function renderPreviewHearthline(state, timeMs) {
  const strength = previewStrength01(state);
  const base = { r: 150, g: 54, b: 18 };
  const amber = { r: 255, g: 150, b: 54 };
  return makePreviewPixels((index, position) => {
    let level = .22;
    let warmth = .36;
    for (let band = 0; band < 4; band++) {
      const center = ((timeMs / (25000 + band * 9300)) + band * .31) % 1;
      const distance = Math.min(Math.abs(position - center), 1 - Math.abs(position - center));
      const falloff = Math.exp(-(distance * distance) / (.006 + band * .002));
      level += falloff * (.28 + band * .04);
      warmth += falloff * .18;
    }
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), position);
    const color = sampled ? blendRgb(blendRgb(base, amber, warmth), sampled, .12) : blendRgb(base, amber, warmth);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * Math.min(.82, level));
  });
}

function renderPreviewEveningCurrent(state, timeMs) {
  const strength = previewStrength01(state);
  const dusk = { r: 74, g: 92, b: 136 };
  const warm = { r: 255, g: 166, b: 100 };
  return makePreviewPixels((index, position) => {
    const swell = (Math.sin(position * Math.PI * 2.3 - timeMs / 9500) + 1) / 2;
    const ripple = (Math.sin(position * Math.PI * 6.8 + timeMs / 16400) + 1) / 2;
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), position + timeMs / 30000);
    const color = sampled ? blendRgb(sampled, warm, swell * .13) : blendRgb(dusk, warm, .22 + swell * .12);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.28 + swell * .34 + ripple * .13));
  });
}

function renderPreviewStarlitLinen(state, timeMs) {
  const strength = previewStrength01(state);
  const base = samplePaletteColor(activePreviewPaletteColors(state), 0) || { r: 255, g: 186, b: 122 };
  const highlight = kelvinToApproxRgb(3000);
  return makePreviewPixels((index, position) => {
    let level = .13 + ((index * 17) % 8) / 220;
    for (let star = 0; star < 9; star++) {
      const center = ((star * 37) % 97) / 97;
      const distance = Math.min(Math.abs(position - center), 1 - Math.abs(position - center));
      const soft = Math.exp(-(distance * distance) / .0018);
      const phase = (Math.sin(timeMs / (18000 + star * 700) + star * 1.7) + 1) / 2;
      level += soft * phase * .33;
    }
    return scaleRgb(blendRgb(base, highlight, Math.min(.35, level)), (state?.effectiveBrightness ?? 180) * Math.min(.55, level));
  });
}

function renderPreviewDeepSeaGlass(state, timeMs) {
  const strength = previewStrength01(state);
  const deep = { r: 8, g: 42, b: 76 };
  const glass = { r: 74, g: 176, b: 194 };
  const glint = { r: 170, g: 232, b: 245 };
  return makePreviewPixels((index, position) => {
    const depth = (Math.sin(position * Math.PI * 2.7 - timeMs / 13500) + 1) / 2;
    const shimmer = (Math.sin(position * Math.PI * 10.2 + timeMs / 8300) + 1) / 2;
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), position + timeMs / 36000);
    let color = blendRgb(deep, glass, .32 + depth * .46);
    if (sampled) color = blendRgb(color, sampled, .28);
    color = blendRgb(color, glint, shimmer * .1);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.22 + depth * .28 + shimmer * .12));
  });
}

function renderPreviewQuietNorthernLights(state, timeMs) {
  const rawIntensity = previewStrength01(state);
  const intensity = Math.pow(rawIntensity, 1.18);
  const auroraWaveLayerCount = rawIntensity < .25 ? 2 : rawIntensity < .55 ? 3 : rawIntensity < .82 ? 4 : 5;
  const smooth01 = (value) => {
    const x = Math.max(0, Math.min(1, value));
    return x * x * (3 - 2 * x);
  };
  const smooth = (a, b, value) => {
    const x = Math.max(0, Math.min(1, (value - a) / (b - a)));
    return smooth01(x);
  };
  const easeSlowAccelFastDecel = (value) => {
    const x = Math.max(0, Math.min(1, value));
    if (x < .65) return .65 * smooth01(x / .65);
    const u = (x - .65) / .35;
    return .65 + .35 * (1 - Math.pow(1 - u, 3));
  };
  const softRidge = (waveValue, width) => {
    const x = (waveValue - (1 - width)) / Math.max(.05, width);
    return smooth01(x);
  };
  const fract = (value) => value - Math.floor(value);
  const night = { r: 3, g: 8, b: 22 };
  const horizon = { r: 7, g: 20, b: 42 };
  const anchor = state?.usingWhiteTemperature
    ? kelvinToApproxRgb(state?.kelvin)
    : hexToRgb(state?.hex || '#ffb26d');
  const spread = .09 + intensity * .54;
  const auroraGreen = blendRgb(anchor, { r: 64, g: 230, b: 145 }, spread);
  const cyan = blendRgb(anchor, { r: 70, g: 210, b: 245 }, Math.min(.82, spread + .06));
  const teal = blendRgb(anchor, { r: 75, g: 245, b: 210 }, Math.min(.80, spread + .04));
  const violet = blendRgb(anchor, { r: 125, g: 105, b: 245 }, Math.min(.86, spread + rawIntensity * .13));
  const pale = blendRgb(anchor, { r: 185, g: 250, b: 225 }, .07 + intensity * .25);
  const paletteColors = state?.paletteEnabled ? activePreviewPaletteColors(state) : [];
  const brightness = state?.effectiveBrightness ?? state?.masterBrightness ?? 180;
  const t = timeMs / 1000;
  return makePreviewPixels((index, position) => {
    const breath = (Math.sin(position * Math.PI * .84 + t * (.020 + intensity * .010)) + 1) / 2;
    let color = blendRgb(night, horizon, .09 + breath * .17);
    color = blendRgb(color, anchor, .02 + intensity * .04);
    let level = .050 + breath * (.018 + intensity * .030);
    for (let layer = 0; layer < auroraWaveLayerCount; layer += 1) {
      const seed = previewHashUnit(layer * 71 + 37);
      const seedPhase = seed * Math.PI * 2 + layer * 1.21;
      const lifeRate = .0010 + seed * .0007 + intensity * .0008;
      const lifePosition = t * lifeRate + seed * .73 + layer * .19;
      const cycle = fract(lifePosition);
      const generation = Math.floor(lifePosition);
      const generationSeed = previewHashUnit(seed * 255 + generation * 29 + layer * 11);
      const fadeIn = smooth(0, .24, cycle);
      const fadeOut = 1 - smooth(.68, 1, cycle);
      const lifeEnvelope = fadeIn * fadeOut;
      if (lifeEnvelope <= .003) continue;
      const motionCycle = .5 + .5 * Math.sin(t * (.007 + seed * .011 + intensity * .006) + seedPhase + generationSeed);
      const shapedMotion = easeSlowAccelFastDecel(motionCycle);
      let phaseShift = ((layer % 2) ? -1 : 1) * (shapedMotion * 2 - 1) * (.80 + intensity * 1.25);
      phaseShift += Math.sin(t * (.004 + generationSeed * .004) + seedPhase * 1.83) * (.20 + intensity * .28);
      const spatialFrequency = Math.PI * 2 * (.95 + layer * .33 + generationSeed * .42 + intensity * .32);
      const warpFrequency = Math.PI * 2 * (.70 + seed * .64 + layer * .18);
      const foldFrequency = Math.PI * 2 * (2.2 + generationSeed * 1.7 + layer * .48);
      const warp = (.07 + intensity * .07) * Math.sin(position * warpFrequency + t * (.012 + seed * .018 + intensity * .010) + seedPhase)
        + (.025 + intensity * .035) * Math.sin(position * foldFrequency * .43 - t * (.007 + seed * .011) + seedPhase * 1.47);
      const fieldPhase = position * spatialFrequency + phaseShift + warp;
      const waveValue = Math.sin(fieldPhase);
      const ridge = softRidge(waveValue, .92 - intensity * .34);
      const folds = .72 + (.06 + intensity * .24) * Math.sin(position * foldFrequency + t * (.018 + generationSeed * .032 + intensity * .022) + seedPhase);
      const fineCurtain = .90 + .10 * Math.sin(position * foldFrequency * 2.7 - t * (.011 + generationSeed * .020) + seedPhase * 2.11);
      const waveContribution = Math.max(0, Math.min(1, ridge * folds * fineCurtain * lifeEnvelope * (.40 + intensity * .52)));
      const sampled = samplePaletteColor(paletteColors, fract(position * .46 + seed + generationSeed * .31 + layer * .17 + phaseShift * .026));
      let waveColor = sampled;
      if (!waveColor && layer % 5 === 0) waveColor = blendRgb(auroraGreen, cyan, .19 + generationSeed * .28);
      if (!waveColor && layer % 5 === 1) waveColor = blendRgb(cyan, teal, .18 + seed * .27);
      if (!waveColor && layer % 5 === 2) waveColor = blendRgb(teal, violet, .13 + rawIntensity * .28);
      if (!waveColor && layer % 5 === 3) waveColor = blendRgb(auroraGreen, pale, .14 + generationSeed * .25);
      if (!waveColor) waveColor = blendRgb(anchor, violet, .15 + rawIntensity * .34);
      color = blendRgb(color, waveColor, waveContribution * (.12 + intensity * .34));
      level += waveContribution * (.11 + intensity * .24);
    }
    const shimmer = (Math.sin(position * Math.PI * 2 * (4.4 + intensity * 1.8) + t * (.012 + intensity * .018)) + 1) / 2;
    level += shimmer * intensity * .018;
    color = blendRgb(color, pale, shimmer * intensity * .024);
    return scaleRgb(color, brightness * Math.min(.62, level));
  });
}

function previewHashUnit(seed) {
  const raw = Math.sin(seed * 12.9898) * 43758.5453;
  return raw - Math.floor(raw);
}

function renderPreviewCandleHall(state, timeMs) {
  const strength = previewStrength01(state);
  const shadow = { r: 42, g: 16, b: 8 };
  const flame = kelvinToApproxRgb(2350);
  const amount = Math.max(0, Math.min(20, Number(state?.candleHallFlickerAmount ?? 3))) / 20;
  const candleTraits = Array.from({ length: 6 }, (_, candle) => {
    const seed = candle * 53 + 17;
    const brightnessVariance = .26 + amount * .30;
    return {
      brightnessScale: 1 - brightnessVariance * .5 + previewHashUnit(seed + 19) * brightnessVariance,
      wobble: .006 + amount * .014 + previewHashUnit(seed + 41) * .012,
      width: .046 + previewHashUnit(seed + 149) * .036,
      primaryPeriod: 9000 + previewHashUnit(seed + 113) * 5900,
      secondaryPeriod: 1800 + previewHashUnit(seed + 131) * 4300,
      microPeriod: 90 + previewHashUnit(seed + 167) * 160,
      phase: previewHashUnit(seed + 79) * Math.PI * 2,
      secondaryPhase: previewHashUnit(seed + 97) * Math.PI * 2,
      warmth: .17 + previewHashUnit(seed + 181) * .36
    };
  });
  return makePreviewPixels((index, position) => {
    let level = .08;
    let color = shadow;
    for (let candle = 0; candle < candleTraits.length; candle++) {
      const trait = candleTraits[candle];
      let center = candle / Math.max(1, candleTraits.length - 1);
      center += Math.sin(timeMs / (28000 / (trait.wobble * 70)) + trait.phase) * trait.wobble;
      center += Math.sin(timeMs / (12000 / (trait.wobble * 95)) + trait.secondaryPhase) * trait.wobble * .28 * amount;
      const falloff = Math.max(0, 1 - Math.abs(position - center) / trait.width);
      const primary = (Math.sin(timeMs / trait.primaryPeriod + trait.phase) + 1) / 2;
      const secondary = (Math.sin(timeMs / trait.secondaryPeriod + trait.secondaryPhase) + 1) / 2;
      const microFlicker = previewHashUnit(candle * 91 + Math.floor(timeMs / trait.microPeriod));
      const pulse = (.86 - amount * .34) + (.08 + amount * .92) * (primary * .55 + secondary * .30 + microFlicker * .15);
      level += falloff * falloff * pulse * trait.brightnessScale * .58;
      const sampled = samplePaletteColor(activePreviewPaletteColors(state), candle / 5) || flame;
      color = blendRgb(color, blendRgb(sampled, flame, trait.warmth), falloff * (.48 + amount * .18));
    }
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * Math.min(.92, level));
  });
}

function renderPreviewSlowPrism(state, timeMs) {
  const strength = previewStrength01(state);
  const anchor = previewBaseColor(state);
  return makePreviewPixels((index, position) => {
    const shifted = (position + timeMs / 210000) % 1;
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), shifted);
    const companion = hsvToRgb((shifted + .08) * 360, .52, .58);
    const color = blendRgb(anchor, sampled || companion, .62);
    const prism = (Math.sin(shifted * Math.PI * 2.4 + timeMs / 42000) + 1) / 2;
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.36 + prism * .28));
  });
}

function renderPreviewGardenShade(state, timeMs) {
  const strength = previewStrength01(state);
  const sun = kelvinToApproxRgb(3900);
  const leaf = { r: 96, g: 126, b: 62 };
  const gold = { r: 224, g: 188, b: 92 };
  return makePreviewPixels((index, position) => {
    const breeze = (Math.sin(position * Math.PI * 2.5 + timeMs / 22000) + 1) / 2;
    const shade = (Math.sin(position * Math.PI * 6.4 - timeMs / 31000) + 1) / 2;
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), position);
    let natural = blendRgb(leaf, gold, breeze * .47);
    if (sampled) natural = blendRgb(natural, sampled, .2);
    const color = blendRgb(sun, natural, .16 + shade * .3);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.48 + breeze * .18 - shade * .06));
  });
}

function renderPreviewSnowfallHush(state, timeMs) {
  const strength = previewStrength01(state);
  const base = samplePaletteColor(activePreviewPaletteColors(state), 0) || { r: 70, g: 96, b: 132 };
  const snow = kelvinToApproxRgb(6200);
  return makePreviewPixels((index, position) => {
    let level = .09 + ((index * 23) % 8) / 260;
    let color = blendRgb(base, snow, .12);
    for (let particle = 0; particle < 10; particle++) {
      const center = (((particle * 37) % 101) / 101 + timeMs * (0.0000018 + particle * 0.00000018)) % 1;
      const distance = Math.min(Math.abs(position - center), 1 - Math.abs(position - center));
      const soft = Math.exp(-(distance * distance) / .0022);
      const envelope = (Math.sin(timeMs / (20500 + particle * 650) + particle) + 1) / 2;
      level += soft * envelope * .28;
      color = blendRgb(color, snow, Math.min(.22, soft * envelope));
    }
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * Math.min(.46, level));
  });
}

function renderPreviewObservatoryGlow(state, timeMs) {
  const strength = previewStrength01(state);
  const night = { r: 4, g: 12, b: 34 };
  const moon = kelvinToApproxRgb(5600);
  const moonCenter = (timeMs % 420000) / 420000;
  return makePreviewPixels((index, position) => {
    const distance = Math.min(Math.abs(position - moonCenter), 1 - Math.abs(position - moonCenter));
    const glow = Math.exp(-(distance * distance) / .028);
    let level = .1 + glow * .46;
    let color = blendRgb(night, moon, glow * .58);
    for (let star = 0; star < 8; star++) {
      const center = ((star * 41) % 97) / 97;
      const sd = Math.min(Math.abs(position - center), 1 - Math.abs(position - center));
      const soft = Math.exp(-(sd * sd) / .0018);
      level += soft * (.04 + glow * .08);
      color = blendRgb(color, moon, soft * .15);
    }
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * Math.min(.62, level));
  });
}

function renderPreviewRainyWindow(state, timeMs) {
  const strength = previewStrength01(state);
  const pane = samplePaletteColor(activePreviewPaletteColors(state), 0) || { r: 31, g: 58, b: 90 };
  const dropColor = { r: 160, g: 206, b: 226 };
  return makePreviewPixels((index, position) => {
    let level = .12 + ((Math.sin(position * Math.PI * 2.8 + timeMs / 28000) + 1) / 2) * .06;
    let color = blendRgb({ r: 10, g: 28, b: 46 }, pane, .47);
    for (let drop = 0; drop < 7; drop++) {
      const center = (((drop * 43) % 103) / 103 + timeMs * (0.0000028 + drop * 0.00000024)) % 1;
      const ahead = center - position;
      const bead = Math.exp(-(ahead * ahead) / .0012);
      const trail = Math.max(0, ahead) * Math.exp(-Math.abs(ahead) * 16);
      level += bead * .22 + trail * .8;
      color = blendRgb(color, dropColor, Math.min(.24, bead * .18 + trail * .8));
    }
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * Math.min(.58, level));
  });
}

function renderPreviewCampfireBlanket(state, timeMs) {
  const strength = previewStrength01(state);
  const ember = { r: 178, g: 58, b: 18 };
  const blanket = kelvinToApproxRgb(2450);
  return makePreviewPixels((index, position) => {
    const waveA = (Math.sin(position * Math.PI * 3.4 - timeMs / 11600) + 1) / 2;
    const waveB = (Math.sin(position * Math.PI * 8.2 + timeMs / 18800) + 1) / 2;
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), position);
    let color = blendRgb(ember, blanket, .33 + waveA * .31);
    if (sampled) color = blendRgb(color, sampled, .2);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.2 + waveA * .28 + waveB * .13));
  });
}

function renderPreviewNorthernWhisper(state, timeMs) {
  const strength = previewStrength01(state);
  const night = { r: 5, g: 12, b: 32 };
  const green = { r: 64, g: 210, b: 145 };
  const blue = { r: 78, g: 128, b: 224 };
  const violet = { r: 134, g: 104, b: 226 };
  return makePreviewPixels((index, position) => {
    const veilA = (Math.sin(position * Math.PI * 1.84 + timeMs / 30000) + 1) / 2;
    const veilB = (Math.sin(position * Math.PI * 4.2 - timeMs / 42000) + 1) / 2;
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), position + timeMs / 52000);
    let ribbon = blendRgb(blendRgb(green, blue, veilA * .5), violet, veilB * .28);
    if (sampled) ribbon = blendRgb(ribbon, sampled, .38);
    const fold = veilA * veilA * .54 + veilB * .18;
    return scaleRgb(blendRgb(night, ribbon, fold), (state?.effectiveBrightness ?? 180) * (.1 + fold * .34));
  });
}

function renderPreviewStormOutside(state, timeMs) {
  const strength = previewStrength01(state);
  const base = samplePaletteColor(activePreviewPaletteColors(state), 0) || { r: 16, g: 32, b: 51 };
  const cloud = { r: 72, g: 96, b: 124 };
  const lightning = kelvinToApproxRgb(7200);
  const flashWave = (Math.sin(timeMs / 8435) + 1) / 2;
  const flash = flashWave > .88 ? (flashWave - .88) / .12 * .42 : 0;
  return makePreviewPixels((index, position) => {
    const roll = (Math.sin(position * Math.PI * 1.6 - timeMs / 23000) + 1) / 2;
    const rain = (Math.sin(position * Math.PI * 14 + timeMs / 5600) + 1) / 2;
    let color = blendRgb({ r: 4, g: 14, b: 28 }, base, .46);
    color = blendRgb(color, cloud, roll * .27);
    color = blendRgb(color, lightning, flash);
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * (.09 + roll * .19 + rain * .08 + flash * .18));
  });
}

function renderPreviewFallback(state, timeMs) {
  return renderPreviewSolid(state, timeMs);
}

function renderPreviewPixels(state, timeMs) {
  const mode = state?.mode || 'solid';
  const strength = previewStrength01(state);
  timeMs = timeMs * (.35 + strength * 1.35);
  if (mode === 'solid') return renderPreviewSolid(state, timeMs);
  if (mode === 'strobe') return renderPreviewStrobe(state, timeMs);
  if (mode === 'flash') return renderPreviewFlash(state, timeMs);
  if (mode === 'chase') return renderPreviewChase(state, timeMs);
  if (mode === 'wave') return renderPreviewWave(state, timeMs);
  if (mode === 'slowWave') return renderPreviewSlowWave(state, timeMs);
  if (mode === 'slowPulse') return renderPreviewSlowPulse(state, timeMs);
  if (mode === 'rainbow') return renderPreviewRainbow(state, timeMs);
  if (mode === 'dawnBloom') return renderPreviewDawnBloom(state, timeMs);
  if (mode === 'moonTide') return renderPreviewMoonTide(state, timeMs);
  if (mode === 'velvetAurora') return renderPreviewVelvetAurora(state, timeMs);
  if (mode === 'lanternDrift') return renderPreviewLanternDrift(state, timeMs);
  if (mode === 'rainGlass') return renderPreviewRainGlass(state, timeMs);
  if (mode === 'satinBreathing') return renderPreviewSatinBreathing(state, timeMs);
  if (mode === 'paperLanternRow') return renderPreviewPaperLanternRow(state, timeMs);
  if (mode === 'libraryLamp') return renderPreviewLibraryLamp(state, timeMs);
  if (mode === 'solarNoon') return renderPreviewSolarNoon(state, timeMs);
  if (mode === 'goldenHour') return renderPreviewGoldenHour(state, timeMs);
  if (mode === 'dreamAquarium') return renderPreviewDreamAquarium(state, timeMs);
  if (mode === 'circuitGlow') return renderPreviewCircuitGlow(state, timeMs);
  if (mode === 'windowSunrise') return renderPreviewWindowSunrise(state, timeMs);
  if (mode === 'emberQuilt') return renderPreviewEmberQuilt(state, timeMs);
  if (mode === 'porcelainSky') return renderPreviewPorcelainSky(state, timeMs);
  if (mode === 'hearthline') return renderPreviewHearthline(state, timeMs);
  if (mode === 'eveningCurrent') return renderPreviewEveningCurrent(state, timeMs);
  if (mode === 'starlitLinen') return renderPreviewStarlitLinen(state, timeMs);
  if (mode === 'deepSeaGlass') return renderPreviewDeepSeaGlass(state, timeMs);
  if (mode === 'quietNorthernLights') return renderPreviewQuietNorthernLights(state, timeMs);
  if (mode === 'candleHall') return renderPreviewCandleHall(state, timeMs);
  if (mode === 'slowPrism') return renderPreviewSlowPrism(state, timeMs);
  if (mode === 'gardenShade') return renderPreviewGardenShade(state, timeMs);
  if (mode === 'snowfallHush') return renderPreviewSnowfallHush(state, timeMs);
  if (mode === 'observatoryGlow') return renderPreviewObservatoryGlow(state, timeMs);
  if (mode === 'rainyWindow') return renderPreviewRainyWindow(state, timeMs);
  if (mode === 'campfireBlanket') return renderPreviewCampfireBlanket(state, timeMs);
  if (mode === 'northernWhisper') return renderPreviewNorthernWhisper(state, timeMs);
  if (mode === 'stormOutside') return renderPreviewStormOutside(state, timeMs);
  return renderPreviewFallback(state, timeMs);
}

function drawPreviewStrip(pixels) {
  const canvas = $('previewCanvas');
  const context = canvas.getContext('2d');
  const pixelWidth = canvas.width / PREVIEW_PIXELS;
  context.clearRect(0, 0, canvas.width, canvas.height);
  pixels.forEach((pixel, index) => {
    context.fillStyle = rgbToCss(pixel);
    context.fillRect(Math.floor(index * pixelWidth), 0, Math.ceil(pixelWidth) + 1, canvas.height);
  });
}

function updatePreviewLabels(state = previewState || lastState) {
  if (!state) {
    return;
  }
  const palette = state.paletteEnabled ? `, ${state.activePaletteName || state.activePaletteId}` : '';
  $('previewModeLabel').textContent = `${state.modeDisplayName || state.mode}${palette} at ${state.effectiveBrightness ?? state.masterBrightness}/255`;
}

function refreshPreviewFromCachedState() {
  if (lastState) {
    previewState = lastState;
    updatePreviewLabels(lastState);
    $('previewStatus').textContent = 'Preview: approximate browser-side visualization from cached controller state.';
    return true;
  }
  $('previewStatus').innerHTML = 'Preview state unavailable. <button class="inline-retry" type="button" data-retry-section="preview">Retry</button>';
  return false;
}

async function refreshPreviewState() {
  if (lastState) {
    return refreshPreviewFromCachedState();
  }
  const ok = await refreshStateSafe({ showStatus: false });
  if (!ok) {
    $('previewStatus').innerHTML = 'Preview state unavailable. <button class="inline-retry" type="button" data-retry-section="preview">Retry</button>';
    return false;
  }
  return refreshPreviewFromCachedState();
}

function drawPreview(timestamp) {
  if (document.visibilityState === 'hidden') {
    requestAnimationFrame(drawPreview);
    return;
  }
  const state = previewState || lastState;
  const minimumFrameMs = previewReducedMotion || previewPaused ? 450 : 33;
  if (state && timestamp - previewLastDrawMs >= minimumFrameMs) {
    previewLastDrawMs = timestamp;
    const time = previewPaused ? 0 : timestamp;
    drawPreviewStrip(renderPreviewPixels(state, time));
  }
  requestAnimationFrame(drawPreview);
}

async function send(params) {
  if (!canMutateFromUi('set')) {
    return false;
  }
  const [pendingMessage, successMessage] = messageForParams(params);
  showGlobalStatus(pendingMessage, 'pending');
  const result = await apiFetchJson(`/set?${qs(params)}`, {
    label: 'Setting update',
    timeoutMs: 4500,
    retries: 1,
    showStatus: false
  });
  if (!result.ok) {
    showGlobalStatus(result.error || 'Update failed', 'error');
    await refreshStateSafe({ showStatus: false });
    return false;
  }
  applyState(result.payload.state || result.payload);
  showGlobalStatus(successMessage, 'success');
  return true;
}

function sendSoon(params) {
  if (!canMutateFromUi('set soon')) {
    return;
  }
  clearTimeout(debounceTimer);
  const [pendingMessage] = messageForParams(params);
  showGlobalStatus(pendingMessage, 'pending');
  debounceTimer = setTimeout(() => send(params).catch(console.error), 160);
}

function updateBrightnessReadout(raw, percent) {
  $('masterBrightnessValue').textContent = raw;
  $('masterBrightnessPercent').textContent = `${percent}%`;
}

function animationStrengthBand(value) {
  if (value <= 50) return 'Subtle';
  if (value <= 110) return 'Subtle';
  if (value <= 180) return 'Balanced';
  if (value <= 230) return 'Expressive';
  return 'Maximum expression';
}

function updateAnimationStrengthReadout(value) {
  const raw = Math.max(0, Math.min(255, Number(value) || 0));
  const percent = Math.round(raw * 100 / 255);
  $('animationStrengthValue').textContent = `${animationStrengthBand(raw)} - ${raw} / 255 - ${percent}%`;
}

async function sendAnimationStrength(value) {
  if (!canMutateFromUi('animation strength')) {
    return false;
  }
  const raw = Math.max(0, Math.min(255, Number(value) || 0));
  showGlobalStatus('Saving Animation Strength...', 'pending');
  const result = await apiFetchJson(`/api/animation?${qs({ strength: raw })}`, {
    label: 'Animation Strength update',
    timeoutMs: 4500,
    retries: 1
  });
  if (!result.ok) {
    showGlobalStatus(result.error || 'Animation Strength update failed.', 'error');
    return false;
  }
  const payload = result.payload || {};
  const nextStrength = Number(payload.animationStrength ?? raw);
  if (lastState) {
    lastState = {
      ...lastState,
      animationStrength: nextStrength,
      animationStrengthPercent: Math.round(nextStrength * 100 / 255),
      animationIntensity: nextStrength,
      animationIntensityPercent: Math.round(nextStrength * 100 / 255)
    };
    previewState = lastState;
    updatePreviewLabels(lastState);
  }
  showGlobalStatus('Animation Strength saved', 'success');
  return true;
}

function sendAnimationStrengthSoon(value) {
  if (!canMutateFromUi('animation strength')) {
    return;
  }
  clearTimeout(animationStrengthDebounceTimer);
  const raw = Math.max(0, Math.min(255, Number(value) || 0));
  if (lastState) {
    lastState = {
      ...lastState,
      animationStrength: raw,
      animationStrengthPercent: Math.round(raw * 100 / 255),
      animationIntensity: raw,
      animationIntensityPercent: Math.round(raw * 100 / 255)
    };
    previewState = lastState;
  }
  showGlobalStatus('Saving Animation Strength...', 'pending');
  animationStrengthDebounceTimer = setTimeout(() => {
    sendAnimationStrength(raw).catch(console.error);
  }, 180);
}

function findModeMeta(id) {
  return modeMetadata.find((mode) => mode.id === id) || null;
}

function formatTag(tag) {
  const labels = {
    animated: 'Animated',
    calm: 'Calm',
    sleepSafe: 'Sleep-safe',
    focusFriendly: 'Focus',
    colorful: 'Colorful',
    flashing: 'Flashing',
    utility: 'Utility',
    experimental: 'Experimental',
    blockedByNightGuard: 'Night Guard blocked'
  };
  return labels[tag] || tag;
}

function modeMatchesFilter(mode, filter) {
  if (filter === 'all') return true;
  if (filter === 'everyday') return mode.category === 'Everyday';
  if (filter === 'weather') return mode.category === 'Weather';
  return !!mode[filter];
}

function renderModeFilters() {
  $('modeFilterBar').innerHTML = MODE_FILTERS.map(([key, label]) => `
    <button class="filter-chip ${key === activeModeFilter ? 'active' : ''}" type="button" data-mode-filter="${key}">
      ${label}
    </button>
  `).join('');
}

function renderModeOptions() {
  if (!modeMetadata.length) {
    return;
  }
  const current = $('mode').value || lastState?.mode || 'solid';
  let modes = modeMetadata.filter((mode) => modeMatchesFilter(mode, activeModeFilter));
  if (!modes.find((mode) => mode.id === current)) {
    const selected = findModeMeta(current);
    if (selected) {
      modes = [selected, ...modes];
    }
  }
  $('mode').innerHTML = modes.map((mode) => `
    <option value="${escapeHtml(mode.id)}" data-blocked-night-guard="${mode.blockedByNightGuard ? 'true' : 'false'}">
      ${escapeHtml(mode.displayName)}
    </option>
  `).join('');
  $('mode').value = current;
  updateNightGuardControls(lastState || {});
  updateVisibleControls();
}

function renderSelectedModeMetadata(state = lastState) {
  const selectedId = $('mode').value || state?.mode || 'solid';
  const metadata = findModeMeta(selectedId);
  const displayName = metadata?.displayName || state?.modeDisplayName || selectedId;
  const category = metadata?.category || state?.modeCategory || 'Unknown';
  const bestUsedFor = metadata?.bestUsedFor || '';
  const description = metadata?.description || '';
  const blocked = !!state?.nightGuardEnabled && !!state?.nightGuardBlockFlashingModes && !!metadata?.blockedByNightGuard;
  $('modeDescription').textContent = `${displayName} - ${category}. ${description} ${bestUsedFor ? `Best for: ${bestUsedFor}` : ''}${blocked ? ' Night Guard blocks this mode right now.' : ''}`;
  const tags = metadata?.tags || state?.modeTags || [];
  $('modeTags').innerHTML = tags.map((tag) => `<span class="tag">${escapeHtml(formatTag(tag))}</span>`).join('');
}

function applyModeFilter(filter) {
  activeModeFilter = filter;
  renderModeFilters();
  renderModeOptions();
  renderSelectedModeMetadata();
}

async function refreshModes() {
  return refreshModesSafe();
}

async function refreshModesSafe(options = {}) {
  const result = await apiFetchJson('/api/modes', {
    label: 'Mode list',
    timeoutMs: 6500,
    retries: 1,
    retryDelayMs: 300,
    showStatus: !!options.showStatus
  });
  if (!result.ok) {
    showGlobalStatus('Mode list failed - retrying', 'error');
    return false;
  }
  const payload = result.payload;
  modeMetadata = payload.modes || [];
  renderModeFilters();
  renderModeOptions();
  renderSelectedModeMetadata();
  return true;
}

function renderCurrentSummary(state) {
  const lastLoadedScene = state.lastLoadedSceneName || 'None';
  const sceneSummary = state.lastLoadedSceneName
    ? (state.currentStateMatchesLoadedScene ? lastLoadedScene : `${lastLoadedScene} (changed)`)
    : 'Manual / no scene';
  const timerSummary = state.timerActive
    ? `${formatTimerMs(state.timerRemainingMs)} remaining`
    : 'No active timer';
  const nightGuard = state.nightGuardEnabled
    ? `On, ${state.nightGuardEffectiveBrightness}/255 effective`
    : 'Off';
  const paletteSummary = state.paletteEnabled
    ? `${state.activePaletteName || state.activePaletteId}${state.currentModeSupportsPalette ? '' : ' (saved only)'}`
    : 'Off';
  const scheduleSummary = state.timeSynced
    ? (state.nextScheduleSummary || 'No scheduled events')
    : 'Schedules paused: time not synced';
  $('currentSummary').innerHTML = [
    ['Mode', state.modeDisplayName || state.mode],
    ['Scene', sceneSummary],
    ['Brightness', `${state.masterBrightness}/255, ${state.effectiveBrightness}/255 effective`],
    ['Timer', timerSummary],
    ['Next schedule', scheduleSummary],
    ['Night Guard', nightGuard],
    ['Palette', paletteSummary]
  ].map(([label, value]) => `
    <div class="summary-item">
      <div class="summary-label">${escapeHtml(label)}</div>
      <div class="summary-value">${escapeHtml(value)}</div>
    </div>
  `).join('');
}

function updateTransitionControls(state) {
  $('transitionsEnabled').checked = !!state.transitionsEnabled;
  $('transitionDurationMs').value = String(state.transitionDurationMs ?? 1500);
  $('transitionStyle').value = state.transitionStyle || 'fade';
  $('transitionStatus').textContent = state.transitionActive
    ? `Transitioning: ${state.transitionProgressPercent || 0}%`
    : 'Transitions ready.';
}

async function saveTransitionControls() {
  if (!canMutateFromUi('transition settings')) {
    return false;
  }
  showGlobalStatus('Saving transition settings...', 'pending');
  const result = await apiFetchJson(`/api/transitions/set?${qs({
    enabled: $('transitionsEnabled').checked ? 1 : 0,
    durationMs: $('transitionDurationMs').value,
    style: $('transitionStyle').value
  })}`, {
    label: 'Transition settings',
    timeoutMs: 4500,
    retries: 1
  });
  if (!result.ok) {
    showFavoriteMessage(result.error || 'Transition update failed', true);
    showGlobalStatus(result.error || 'Transition update failed', 'error');
    return false;
  }
  const payload = result.payload;
  if (payload.state) {
    applyState(payload.state);
  } else {
    await refreshStateSafe({ showStatus: false });
  }
  showGlobalStatus('Transition settings saved', 'success');
  return true;
}

function paletteById(id) {
  return paletteCache.find((palette) => palette.id === id);
}

function paletteSwatches(colors = []) {
  return colors.map((color) => `<span class="palette-swatch" style="background:${escapeHtml(color)}"></span>`).join('');
}

function showPaletteMessage(message, isError = false) {
  $('paletteMessage').textContent = message || '';
  $('paletteMessage').style.color = isError ? '#ff9a9a' : 'var(--accent-2)';
}

function updatePaletteControls(state = lastState) {
  if (!state) {
    return;
  }
  $('paletteEnabled').checked = !!state.paletteEnabled;
  $('activePaletteId').value = state.activePaletteId || 'warm-linen';
  const active = paletteById(state.activePaletteId);
  $('activePaletteSwatches').innerHTML = active
    ? paletteSwatches(active.colors || [])
    : '<span class="note">No palette selected.</span>';
  $('paletteSupportNote').textContent = state.currentModeSupportsPalette
    ? `Current mode supports palettes (${state.currentModePaletteRole || 'ambient'}).`
    : 'Current mode uses the selected solid color. Palette is saved but not used by this mode.';
}

function renderPalettes(payload) {
  paletteCache = payload.palettes || [];
  const options = paletteCache
    .map((palette) => `<option value="${escapeHtml(palette.id)}">${escapeHtml(palette.name)}${palette.builtin ? ' (built-in)' : ''}</option>`)
    .join('');
  $('activePaletteId').innerHTML = options || '<option value="">No palettes</option>';
  if (payload.activePaletteId) {
    $('activePaletteId').value = payload.activePaletteId;
  }

  if (!paletteCache.length) {
    $('paletteList').innerHTML = '<p class="note">No palettes available. Reset built-ins or save a palette.</p>';
    updatePaletteControls(lastState || payload);
    return;
  }

  $('paletteList').innerHTML = paletteCache.map((palette) => {
    const labels = [
      palette.builtin ? 'Built-in' : 'Custom',
      palette.category,
      palette.sleepSafe ? 'Sleep-safe' : '',
      palette.calm ? 'Calm' : '',
      palette.colorful ? 'Colorful' : ''
    ].filter(Boolean).join(' - ');
    return `
      <div class="palette-card">
        <div class="palette-title">
          <span>${escapeHtml(palette.name)}</span>
          <span class="tag">${escapeHtml(labels)}</span>
        </div>
        <div class="palette-swatches">${paletteSwatches(palette.colors || [])}</div>
        <div class="palette-actions">
          <button type="button" data-palette-select="${escapeHtml(palette.id)}">Select</button>
          <button type="button" data-palette-edit="${escapeHtml(palette.id)}"${palette.builtin ? ' disabled' : ''}>Edit</button>
          <button type="button" data-palette-delete="${escapeHtml(palette.id)}"${palette.builtin ? ' disabled' : ''}>Delete</button>
        </div>
      </div>
    `;
  }).join('');

  updatePaletteControls(lastState || payload);
}

async function refreshPalettes() {
  return refreshPalettesSafe();
}

async function refreshPalettesSafe(options = {}) {
  const result = await apiFetchJson('/api/palettes', {
    label: 'Palette list',
    timeoutMs: 6500,
    retries: 1,
    retryDelayMs: 300,
    showStatus: !!options.showStatus
  });
  if (!result.ok) {
    renderUnavailableSection('paletteList', 'Palette list unavailable.', 'palettes');
    showPaletteMessage('Palette list unavailable - retry', true);
    return false;
  }
  renderPalettes(result.payload);
  showPaletteMessage('');
  return true;
}

async function selectPalette(id, enabled = $('paletteEnabled').checked) {
  if (!canMutateFromUi('palette selection')) {
    return false;
  }
  showGlobalStatus('Selecting palette...', 'pending');
  const result = await apiFetchJson(`/api/palettes/select?${qs({ id, enabled: enabled ? 1 : 0 })}`, {
    label: 'Palette selection',
    timeoutMs: 4500,
    retries: 1
  });
  if (!result.ok) {
    showPaletteMessage(result.error || 'Palette selection failed', true);
    showGlobalStatus(result.error || 'Palette selection failed', 'error');
    return false;
  }
  const payload = result.payload;
  showPaletteMessage(payload.message || 'Palette selected');
  showGlobalStatus('Palette selected', 'success');
  if (payload.state) {
    applyState(payload.state);
  } else {
    await refreshStateSafe({ showStatus: false });
  }
  await refreshPalettesSafe();
  return true;
}

function editPalette(id) {
  const palette = paletteById(id);
  if (!palette || palette.builtin) {
    return;
  }
  editingPaletteId = id;
  $('paletteName').value = palette.name;
  for (let i = 1; i <= 5; i++) {
    const color = palette.colors?.[i - 1] || '#000000';
    $(`paletteColor${i}`).value = color;
  }
  $('savePalette').textContent = 'Update Palette';
  showPaletteMessage(`Editing ${palette.name}`);
}

async function savePaletteFromForm() {
  if (!canMutateFromUi('palette save')) {
    return false;
  }
  const params = {
    name: $('paletteName').value.trim(),
    c1: $('paletteColor1').value,
    c2: $('paletteColor2').value,
    c3: $('paletteColor3').value,
    c4: $('paletteColor4').value
  };
  const c5 = $('paletteColor5').value;
  if (c5 && c5 !== '#000000') {
    params.c5 = c5;
  }
  const endpoint = editingPaletteId ? '/api/palettes/update' : '/api/palettes/save';
  if (editingPaletteId) {
    params.id = editingPaletteId;
  }
  showGlobalStatus(editingPaletteId ? 'Updating palette...' : 'Saving palette...', 'pending');
  const result = await apiFetchJson(`${endpoint}?${qs(params)}`, {
    label: 'Palette save',
    timeoutMs: 5500,
    retries: 1
  });
  if (!result.ok) {
    showPaletteMessage(result.error || 'Palette save failed', true);
    showGlobalStatus(result.error || 'Palette save failed', 'error');
    return false;
  }
  const payload = result.payload;
  editingPaletteId = '';
  $('savePalette').textContent = 'Save Palette';
  $('paletteName').value = '';
  showPaletteMessage(payload.message || 'Palette saved');
  showGlobalStatus('Palette saved', 'success');
  await refreshPalettesSafe();
  return true;
}

async function deletePalette(id) {
  if (!canMutateFromUi('palette delete')) {
    return false;
  }
  showGlobalStatus('Deleting palette...', 'pending');
  const result = await apiFetchJson(`/api/palettes/delete?${qs({ id })}`, {
    label: 'Palette delete',
    timeoutMs: 4500,
    retries: 1
  });
  if (!result.ok) {
    showPaletteMessage(result.error || 'Palette delete failed', true);
    showGlobalStatus(result.error || 'Palette delete failed', 'error');
    return false;
  }
  const payload = result.payload;
  showPaletteMessage(payload.message || 'Palette deleted');
  showGlobalStatus('Palette deleted', 'success');
  if (payload.state) {
    applyState(payload.state);
  }
  await refreshPalettesSafe();
  return true;
}

async function resetPalettes() {
  if (!canMutateFromUi('palette reset')) {
    return false;
  }
  showGlobalStatus('Resetting palettes...', 'pending');
  const result = await apiFetchJson('/api/palettes/reset', {
    label: 'Palette reset',
    timeoutMs: 6500,
    retries: 1
  });
  if (!result.ok) {
    showPaletteMessage(result.error || 'Palette reset failed', true);
    showGlobalStatus(result.error || 'Palette reset failed', 'error');
    return false;
  }
  const payload = result.payload;
  editingPaletteId = '';
  $('savePalette').textContent = 'Save Palette';
  showPaletteMessage(payload.message || 'Palettes reset');
  showGlobalStatus('Palettes reset', 'success');
  if (payload.state) {
    applyState(payload.state);
  }
  await refreshPalettesSafe();
  return true;
}

function updateVisibleControls() {
  const active = $('mode').value;
  document.querySelectorAll('.mode-control').forEach((el) => {
    el.classList.toggle('show', el.dataset.for === active);
  });
}

function updateNightGuardControls(state) {
  const enabled = !!state.nightGuardEnabled;
  const blockFlashing = !!state.nightGuardBlockFlashingModes;
  $('nightGuardEnabled').checked = enabled;
  $('nightGuardMaxBrightness').value = state.nightGuardMaxBrightness ?? 80;
  $('nightGuardBlockFlashing').checked = blockFlashing;
  $('nightGuardPreferWarm').checked = !!state.nightGuardPreferWarmTemperature;

  const cap = state.nightGuardMaxBrightness ?? 80;
  if (enabled && state.nightGuardBrightnessCapped) {
    $('nightGuardNote').textContent = `Night Guard is limiting output to ${cap} / 255.`;
  } else if (enabled) {
    $('nightGuardNote').textContent = `Night Guard is enabled. Max ${cap} / 255.`;
  } else {
    $('nightGuardNote').textContent = 'Night Guard is off.';
  }

  document.querySelectorAll('#mode option').forEach((option) => {
    const metadata = findModeMeta(option.value);
    const blocked = enabled && blockFlashing && !!metadata?.blockedByNightGuard;
    option.disabled = blocked;
    option.textContent = blocked
      ? `${option.textContent.replace(' (blocked)', '')} (blocked)`
      : option.textContent.replace(' (blocked)', '');
  });
}

function applyState(state) {
  const wasHydrating = uiHydrating;
  uiHydrating = true;
  try {
    lastState = state;
    previewState = state;
    $('status').textContent = `${state.hostname} - ${state.ip} - ${state.mode}`;
    $('mode').value = state.mode;
    $('bootBehavior').value = state.bootBehavior || 'restore';
    $('masterBrightness').value = state.masterBrightness;
    updateBrightnessReadout(state.masterBrightness, state.masterBrightnessPercent);
    const animationStrength = Number(state.animationStrength ?? state.animationIntensity ?? 128);
    $('animationStrength').value = String(animationStrength);
    updateAnimationStrengthReadout(animationStrength);
    $('gammaEnabled').checked = !!state.gammaEnabled;
    $('redGain').value = state.redGain;
    $('greenGain').value = state.greenGain;
    $('blueGain').value = state.blueGain;
    $('kelvin').value = state.kelvin;
    $('kelvinText').textContent = state.kelvin;
    const hex = `#${state.hex}`;
    $('colorPicker').value = hex;
    $('swatch').style.background = hex;
    $('strobeDelay').value = Number(state.strobeDelay).toFixed(1);
    $('flashOffDelay').value = Number(state.flashOffDelay).toFixed(1);
    $('chasePeriod').value = String(state.chasePeriod);
    $('wavePeriod').value = String(state.wavePeriod);
    $('slowPulseCount').value = String(state.slowPulseCount);
    $('slowPulseMax').value = String(state.slowPulseMax);
    $('rainbowPeriod').value = String(state.rainbowPeriod);
    const candleFlickerAmount = Number(state.candleHallFlickerAmount ?? 3);
    $('candleHallFlickerAmount').value = String(candleFlickerAmount);
    $('candleHallFlickerAmountValue').textContent = `${candleFlickerAmount} / 20`;
    $('bedtimeMinutes').value = String(state.bedtimeFadeDefaultMinutes || 30);
    $('bedtimeTarget').value = state.bedtimeFadeDefaultTarget || 'warmDim';
    renderCurrentSummary(state);
    updateTransitionControls(state);
    updateNightGuardControls(state);
    renderSelectedModeMetadata(state);
    updatePaletteControls(state);
    renderTimerStatus(state);
    updatePreviewLabels(state);
    if (state.lastSurpriseSummary && state.lastSurpriseSummary !== 'none') {
      $('surpriseSummary').textContent = state.lastSurpriseSummary;
    }
    updateVisibleControls();
    stateLoaded = true;
  } finally {
    uiHydrating = wasHydrating;
  }
}

function escapeHtml(value) {
  return String(value)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

function showSceneMessage(message, isError = false) {
  $('sceneMessage').textContent = message || '';
  $('sceneMessage').style.color = isError ? '#ff9a9a' : 'var(--accent-2)';
}

function showFavoriteMessage(message, isError = false) {
  $('favoriteMessage').textContent = message || '';
  $('favoriteMessage').style.color = isError ? '#ff9a9a' : 'var(--accent-2)';
}

function showBackupMessage(message, isError = false) {
  $('backupMessage').textContent = message || '';
  $('backupMessage').style.color = isError ? '#ff9a9a' : 'var(--accent-2)';
}

async function copySceneLink(sceneId) {
  const link = `${window.location.origin}/go?scene=${encodeURIComponent(sceneId)}`;
  await navigator.clipboard.writeText(link);
  showSceneMessage(`Copied ${link}`);
}

async function exportSceneJson(sceneId) {
  showGlobalStatus('Exporting scene...', 'pending');
  const result = await apiFetchJson(`/api/scenes/export?${qs({ id: sceneId })}`, {
    label: 'Scene export',
    timeoutMs: 5500,
    retries: 1
  });
  if (!result.ok) {
    showSceneMessage(result.error || 'Scene export failed', true);
    showGlobalStatus(result.error || 'Scene export failed', 'error');
    return false;
  }
  const payload = result.payload;
  $('sceneImportJson').value = JSON.stringify(payload, null, 2);
  showSceneMessage('Scene exported');
  showGlobalStatus('Scene exported', 'success');
  return true;
}

async function duplicateScene(sceneId) {
  showGlobalStatus('Duplicating scene...', 'pending');
  await callScene(`/api/scenes/duplicate?${qs({ id: sceneId })}`, 'Scene duplicated');
}

async function importSceneJsonFromTextarea() {
  const json = $('sceneImportJson').value.trim();
  if (!json) {
    showSceneMessage('Scene JSON is required', true);
    return;
  }
  showGlobalStatus('Importing scene...', 'pending');
  await callScene(`/api/scenes/import?${qs({ json })}`, 'Scene imported');
}

async function restoreBuiltInScenesFromUi() {
  showGlobalStatus('Restoring built-in scenes...', 'pending');
  await callScene('/api/scenes/reset-builtins', 'Built-in scenes restored');
}

async function copyActionLink(actionName) {
  const link = `${window.location.origin}/go?action=${encodeURIComponent(actionName)}`;
  await navigator.clipboard.writeText(link);
  showFavoriteMessage(`Copied ${link}`);
}

function formatTimerMs(ms) {
  const totalSeconds = Math.max(0, Math.ceil(Number(ms || 0) / 1000));
  const minutes = Math.floor(totalSeconds / 60);
  const seconds = totalSeconds % 60;
  return `${minutes}:${String(seconds).padStart(2, '0')}`;
}

function renderTimerStatus(payload) {
  const active = payload.active ?? payload.timerActive;
  const mode = payload.mode ?? payload.timerMode ?? 'none';
  const action = payload.action ?? payload.timerAction ?? 'none';
  const remaining = payload.remainingMs ?? payload.timerRemainingMs ?? 0;
  const progress = payload.progressPercent ?? payload.timerProgressPercent ?? 0;
  const fade = payload.fadeActive ?? payload.timerFadeActive;

  if (!active) {
    $('timerStatus').textContent = payload.lastTimerStatus ? `No active timer. Last: ${payload.lastTimerStatus}` : 'No active timer.';
    return;
  }

  $('timerStatus').textContent = `${mode} to ${action} - ${formatTimerMs(remaining)} remaining - ${progress}%${fade ? ' fade' : ''}`;
}

async function refreshTimer() {
  return refreshTimerSafe();
}

async function refreshTimerSafe() {
  const result = await apiFetchJson('/api/timer', {
    label: 'Timer status',
    timeoutMs: 3500,
    retries: 1,
    showStatus: false
  });
  if (!result.ok) {
    $('timerStatus').textContent = 'Timer status unavailable. Retrying quietly.';
    return false;
  }
  renderTimerStatus(result.payload);
  return true;
}

function populateTimerScenes() {
  const options = sceneCache.map((scene) => `<option value="${escapeHtml(scene.id)}">${escapeHtml(scene.name)}</option>`).join('');
  $('timerScene').innerHTML = options || '<option value="">No scenes</option>';
  $('timerScene').disabled = $('timerAction').value !== 'scene' || !sceneCache.length;
}

async function startTimerFromControls() {
  if (!canMutateFromUi('timer start')) {
    return false;
  }
  const params = {
    minutes: $('timerDuration').value,
    mode: $('timerMode').value,
    action: $('timerAction').value
  };
  if (params.action === 'scene') {
    params.sceneId = $('timerScene').value;
  }

  showGlobalStatus('Starting timer...', 'pending');
  setControlsDisabled(['startTimer'], true);
  const result = await apiFetchJson(`/api/timer/start?${qs(params)}`, {
    label: 'Timer start',
    timeoutMs: 4500,
    retries: 1
  });
  setControlsDisabled(['startTimer'], false);
  if (!result.ok) {
    showFavoriteMessage(result.error || 'Timer failed', true);
    showGlobalStatus(result.error || 'Timer failed', 'error');
    return false;
  }
  const payload = result.payload;
  showFavoriteMessage(payload.message || 'Timer started');
  showGlobalStatus('Timer started', 'success');
  if (payload.state) {
    applyState(payload.state);
  }
  if (payload.timer) {
    renderTimerStatus(payload.timer);
  }
  return true;
}

async function cancelTimer() {
  if (!canMutateFromUi('timer cancel')) {
    return false;
  }
  showGlobalStatus('Cancelling timer...', 'pending');
  setControlsDisabled(['cancelTimer', 'cancelBedtimeFade'], true);
  const result = await apiFetchJson('/api/timer/cancel', {
    label: 'Timer cancel',
    timeoutMs: 4000,
    retries: 1,
    expectOk: false
  });
  setControlsDisabled(['cancelTimer', 'cancelBedtimeFade'], false);
  const payload = result.payload || {};
  if (!result.ok || payload.ok === false) {
    showFavoriteMessage(payload.message || payload.error || result.error || 'No active timer', true);
    showGlobalStatus(payload.message || payload.error || result.error || 'Timer cancel failed', result.ok ? 'success' : 'error');
    if (payload.timer) {
      renderTimerStatus(payload.timer);
    }
    return false;
  }
  showFavoriteMessage(payload.message || 'Timer cancelled');
  showGlobalStatus('Timer cancelled', 'success');
  if (payload.state) {
    applyState(payload.state);
  }
  if (payload.timer) {
    renderTimerStatus(payload.timer);
  }
  return true;
}

async function startBedtimeFade() {
  if (!canMutateFromUi('bedtime fade')) {
    return false;
  }
  showGlobalStatus('Starting bedtime fade...', 'pending');
  setControlsDisabled(['startBedtimeFade'], true);
  const result = await apiFetchJson(`/api/bedtime/start?${qs({
    minutes: $('bedtimeMinutes').value,
    target: $('bedtimeTarget').value,
    saveDefault: 1
  })}`, {
    label: 'Bedtime fade',
    timeoutMs: 4500,
    retries: 1
  });
  setControlsDisabled(['startBedtimeFade'], false);
  if (!result.ok) {
    showFavoriteMessage(result.error || 'Bedtime fade failed', true);
    showGlobalStatus(result.error || 'Bedtime fade failed', 'error');
    return false;
  }
  const payload = result.payload;
  showFavoriteMessage(payload.message || 'Bedtime fade started');
  showGlobalStatus('Bedtime fade started', 'success');
  if (payload.state) {
    applyState(payload.state);
  }
  if (payload.timer) {
    renderTimerStatus(payload.timer);
  }
  return true;
}

async function refreshState() {
  return refreshStateSafe();
}

async function refreshStateSafe(options = {}) {
  const result = await apiFetchJson('/api/state', {
    label: 'State',
    timeoutMs: 4500,
    retries: 1,
    retryDelayMs: 250,
    showStatus: !!options.showStatus
  });
  if (!result.ok) {
    if (!lastState) {
      stateLoaded = false;
      $('status').textContent = 'State unavailable - controls preserved';
      showGlobalStatus('State unavailable - controls preserved', 'error');
    }
    if (lastState) {
      refreshPreviewFromCachedState();
    }
    return false;
  }
  applyState(result.payload);
  return true;
}

function renderFavorites(payload) {
  favoriteCache = payload.favorites || [];
  const visibleFavorites = favoriteCache.filter((favorite) => favorite.available);
  if (!visibleFavorites.length) {
    $('favoriteBar').innerHTML = '<p class="note">No favorites configured. Use Reset Favorites to restore defaults.</p>';
    return;
  }

  $('favoriteBar').innerHTML = visibleFavorites.map((favorite) => `
    <button class="favorite-button" type="button" data-favorite-load="${escapeHtml(favorite.id)}">
      <span class="favorite-swatch"></span>
      <span>${escapeHtml(favorite.label)}</span>
    </button>
  `).join('');
}

async function refreshFavorites() {
  return refreshFavoritesSafe();
}

async function refreshFavoritesSafe() {
  const result = await apiFetchJson('/api/favorites', {
    label: 'Favorite list',
    timeoutMs: 4500,
    retries: 1,
    retryDelayMs: 250,
    showStatus: false
  });
  if (!result.ok) {
    renderUnavailableSection('favoriteBar', 'Favorite list unavailable.', 'favorites');
    showFavoriteMessage('Favorite list unavailable - retry', true);
    return false;
  }
  renderFavorites(result.payload);
  showFavoriteMessage('');
  return true;
}

async function handleFavoritePayload(result, successMessage, options = {}) {
  if (!result.ok) {
    showFavoriteMessage(result.error || 'Favorite action failed', true);
    showGlobalStatus(result.error || 'Favorite action failed', 'error');
    await refreshStateSafe({ showStatus: false });
    return false;
  }
  const payload = result.payload;
  showFavoriteMessage(successMessage || payload.message || '');
  showGlobalStatus(successMessage || payload.message || 'Favorite loaded', 'success');
  if (payload.state) {
    applyState(payload.state);
  } else {
    await refreshStateSafe({ showStatus: false });
  }
  if (options.refreshFavorites !== false) {
    await refreshFavoritesSafe();
  }
  return true;
}

async function runAction(name) {
  if (!canMutateFromUi('action')) {
    return false;
  }
  const message = name === 'warmDimNow' ? 'Warm Dim Now' : 'Action completed';
  showGlobalStatus(name === 'warmDimNow' ? 'Applying Warm Dim Now...' : 'Running action...', 'pending');
  const result = await apiFetchJson(`/api/action?${qs({ name })}`, {
    label: message,
    timeoutMs: 4500,
    retries: 1
  });
  await handleFavoritePayload(result, message, { refreshFavorites: false });
}

async function updateNightGuardFromControls() {
  if (!canMutateFromUi('night guard')) {
    return false;
  }
  const previousState = lastState;
  showGlobalStatus('Updating Night Guard...', 'pending');
  setControlsDisabled(['nightGuardEnabled', 'nightGuardMaxBrightness', 'nightGuardBlockFlashing', 'nightGuardPreferWarm'], true);
  const result = await apiFetchJson(`/api/nightguard/set?${qs({
    enabled: $('nightGuardEnabled').checked ? 1 : 0,
    maxBrightness: $('nightGuardMaxBrightness').value,
    blockFlashing: $('nightGuardBlockFlashing').checked ? 1 : 0,
    preferWarm: $('nightGuardPreferWarm').checked ? 1 : 0
  })}`, {
    label: 'Night Guard update',
    timeoutMs: 4500,
    retries: 1
  });
  setControlsDisabled(['nightGuardEnabled', 'nightGuardMaxBrightness', 'nightGuardBlockFlashing', 'nightGuardPreferWarm'], false);
  if (!result.ok) {
    if (previousState) {
      updateNightGuardControls(previousState);
    }
    $('nightGuardNote').textContent = 'Night Guard update failed.';
    showGlobalStatus('Night Guard update failed.', 'error');
    return false;
  }
  const payload = result.payload;
  if (payload.state) {
    applyState(payload.state);
  } else {
    await refreshStateSafe({ showStatus: false });
  }
  const enabled = !!(payload.state?.nightGuardEnabled ?? lastState?.nightGuardEnabled);
  showGlobalStatus(enabled ? 'Night Guard enabled' : 'Night Guard disabled', 'success');
  return true;
}

function debouncedNightGuardUpdate() {
  if (!canMutateFromUi('night guard')) {
    return;
  }
  clearTimeout(nightGuardDebounceTimer);
  $('nightGuardNote').textContent = 'Updating Night Guard...';
  nightGuardDebounceTimer = setTimeout(() => updateNightGuardFromControls().catch(console.error), 220);
}

function renderScenes(payload) {
  sceneCache = payload.scenes || [];
  populateTimerScenes();
  $('sceneCount').textContent = `${payload.count || 0} / ${payload.maxScenes || 0} scenes`;
  if (!sceneCache.length) {
    $('sceneList').innerHTML = '<p class="note">No scenes saved yet. Save the current light as a scene.</p>';
    return;
  }

  $('sceneList').innerHTML = sceneCache.map((scene) => {
    const detail = scene.usingWhiteTemperature
      ? `${scene.mode} - ${scene.whiteKelvin}K - ${scene.masterBrightness}/255`
      : `${scene.mode} - ${scene.colorHex} - ${scene.masterBrightness}/255`;
    const paletteDetail = scene.paletteEnabled ? ` - palette ${scene.paletteId}` : '';
    const sceneTags = [
      scene.category,
      scene.isSleepSafe ? 'Sleep-safe' : '',
      scene.isFocusFriendly ? 'Focus' : ''
    ].filter(Boolean).join(' - ');
    const warningText = (scene.warnings || [])
      .map((warning) => warning.message || warning.code)
      .filter(Boolean)
      .join(' - ');
    return `
      <div class="scene-card">
        <div class="scene-title">
          <span>${escapeHtml(scene.name)}</span>
          <span class="scene-swatch" style="background:${escapeHtml(scene.colorHex)}"></span>
        </div>
        <div class="scene-meta">${escapeHtml(sceneTags)}${sceneTags ? '<br>' : ''}${escapeHtml(detail)}${escapeHtml(paletteDetail)}${scene.builtin ? ' - built-in' : ''}</div>
        ${warningText ? `<div class="scene-meta">${escapeHtml(warningText)}</div>` : ''}
        <div class="scene-actions">
          <button type="button" data-scene-load="${escapeHtml(scene.id)}">Load</button>
          <button type="button" data-scene-duplicate="${escapeHtml(scene.id)}">Duplicate</button>
          <button type="button" data-scene-export="${escapeHtml(scene.id)}">Export</button>
          <button type="button" data-scene-link="${escapeHtml(scene.id)}">Copy Link</button>
          <button type="button" data-scene-rename="${escapeHtml(scene.id)}">Rename</button>
          <button type="button" data-scene-delete="${escapeHtml(scene.id)}">Delete</button>
        </div>
      </div>
    `;
  }).join('');
}

async function refreshScenes() {
  return refreshScenesSafe();
}

async function callScene(url, successMessage, refreshLighting = false) {
  if (!canMutateFromUi('scene action')) {
    return false;
  }
  const result = await apiFetchJson(url, {
    label: successMessage || 'Scene action',
    timeoutMs: 5500,
    retries: 1
  });
  return handleScenePayload(result, successMessage, refreshLighting);
}

async function refreshScenesSafe(options = {}) {
  const result = await apiFetchJson('/api/scenes', {
    label: 'Scene list',
    timeoutMs: 7000,
    retries: 1,
    retryDelayMs: 350,
    showStatus: !!options.showStatus
  });
  if (!result.ok) {
    renderUnavailableSection('sceneList', 'Scene list unavailable.', 'scenes');
    showSceneMessage('Scene list failed - retrying', true);
    return false;
  }
  renderScenes(result.payload);
  showSceneMessage('');
  return true;
}

async function handleScenePayload(result, successMessage, refreshLighting = false) {
  if (!result.ok) {
    showSceneMessage(result.error || 'Scene action failed', true);
    showGlobalStatus(result.error || 'Scene action failed', 'error');
    return false;
  }
  const payload = result.payload;
  showSceneMessage(successMessage || payload.message || '');
  showGlobalStatus(successMessage || payload.message || 'Scene updated', 'success');
  if (payload.state) {
    applyState(payload.state);
  } else if (refreshLighting) {
    await refreshStateSafe({ showStatus: false });
  }
  await refreshScenesSafe();
  return true;
}

async function runSurprise() {
  if (!canMutateFromUi('surprise')) {
    return false;
  }
  const mood = $('surpriseMood').value;
  const releaseButton = setButtonBusy('surpriseButton', true, 'Choosing...');
  $('surpriseSummary').textContent = 'Choosing a safe surprise...';
  $('surpriseSummary').style.color = 'var(--accent-2)';
  showGlobalStatus('Choosing a safe surprise...', 'pending');
  try {
    const result = await apiFetchJson(`/api/surprise?${qs({ mood, apply: 1 })}`, {
      label: 'Surprise Me',
      timeoutMs: 6500,
      retries: 1,
      retryDelayMs: 300
    });
    if (!result.ok) {
      $('surpriseSummary').textContent = 'Surprise failed - controller did not return valid JSON.';
      $('surpriseSummary').style.color = '#ff9a9a';
      showGlobalStatus('Surprise failed - controller did not respond', 'error');
      return false;
    }
    const payload = result.payload;
    if (!payload.ok) {
      $('surpriseSummary').textContent = payload.error || 'Surprise failed - controller did not return valid JSON.';
      $('surpriseSummary').style.color = '#ff9a9a';
      showGlobalStatus(payload.error || 'Surprise failed - controller did not respond', 'error');
      return false;
    }
    $('surpriseSummary').style.color = 'var(--accent-2)';
    const caps = [payload.powerCapped ? 'power cap' : '', payload.nightGuardCapped ? 'Night Guard cap' : ''].filter(Boolean).join(', ');
    $('surpriseSummary').textContent = `${payload.moodLabel}: ${payload.modeDisplayName}, brightness ${payload.brightness}/255${payload.paletteEnabled ? `, ${payload.paletteId}` : ''}${caps ? ` (${caps})` : ''}`;
    if (payload.state) {
      applyState(payload.state);
    } else {
      await refreshStateSafe({ showStatus: false });
    }
    refreshPreviewFromCachedState();
    showGlobalStatus('Surprise applied', 'success');
    return true;
  } catch (error) {
    console.warn('API request failed', {
      url: '/api/surprise',
      statusCode: 0,
      error: error?.message || String(error),
      rawTextPreview: '',
      time: new Date().toISOString()
    });
    $('surpriseSummary').textContent = 'Surprise failed - controller did not return valid JSON.';
    $('surpriseSummary').style.color = '#ff9a9a';
    showGlobalStatus('Surprise failed - controller did not respond', 'error');
    return false;
  } finally {
    releaseButton();
  }
}

async function saveSurpriseAsScene() {
  const mood = $('surpriseMood').selectedOptions[0]?.textContent || 'Surprise';
  const name = `${mood} Surprise`;
  showGlobalStatus('Saving scene...', 'pending');
  await callScene(`/api/scenes/save?${qs({ name })}`, 'Surprise scene saved');
}

$('masterBrightness').addEventListener('input', (event) => {
  const raw = Number(event.target.value);
  updateBrightnessReadout(raw, Math.round(raw * 100 / 255));
  sendSoon({ masterBrightness: raw });
});

$('animationStrength').addEventListener('input', (event) => {
  updateAnimationStrengthReadout(event.target.value);
  sendAnimationStrengthSoon(event.target.value);
});

$('gammaEnabled').addEventListener('change', (event) => {
  send({ gammaEnabled: event.target.checked ? 1 : 0 }).catch(console.error);
});

['redGain', 'greenGain', 'blueGain'].forEach((id) => {
  $(id).addEventListener('input', () => sendSoon({ [id]: $(id).value }));
});

$('resetCalibration').addEventListener('click', () => {
  send({ resetCalibration: 1 }).catch(console.error);
});

$('saveScene').addEventListener('click', () => {
  const name = $('sceneName').value.trim();
  showGlobalStatus('Saving scene...', 'pending');
  callScene(`/api/scenes/save?${qs({ name })}`, 'Scene saved')
    .then(() => $('sceneName').value = '')
    .catch(console.error);
});

$('importSceneJson').addEventListener('click', () => importSceneJsonFromTextarea().catch(console.error));
$('restoreBuiltInScenes').addEventListener('click', () => restoreBuiltInScenesFromUi().catch(console.error));

$('panicWarm').addEventListener('click', () => {
  runAction('warmDimNow').catch(console.error);
});

$('resetFavorites').addEventListener('click', () => {
  if (!canMutateFromUi('favorites reset')) {
    return;
  }
  showGlobalStatus('Resetting favorites...', 'pending');
  apiFetchJson('/api/favorites/reset', {
    label: 'Favorites reset',
    timeoutMs: 4500,
    retries: 1
  })
    .then((result) => handleFavoritePayload(result, 'Favorites reset'))
    .catch(console.error);
});

$('exportFullBackup').addEventListener('click', async () => {
  showGlobalStatus('Exporting full backup...', 'pending');
  const result = await apiFetchJson('/api/backup/export', {
    label: 'Backup export',
    timeoutMs: 9000,
    retries: 1
  });
  if (!result.ok) {
    showBackupMessage(result.error || 'Backup export failed', true);
    showGlobalStatus(result.error || 'Backup export failed', 'error');
    return;
  }
  $('backupExportJson').value = JSON.stringify(result.payload, null, 2);
  showBackupMessage('Full backup exported');
  showGlobalStatus('Full backup exported', 'success');
});

$('importFullBackup').addEventListener('click', async () => {
  if (!canMutateFromUi('backup import')) {
    return;
  }
  const json = $('backupExportJson').value.trim();
  if (!json) {
    showBackupMessage('Backup JSON is required', true);
    return;
  }
  const confirmText = window.prompt('Type YES to validate backup import');
  if (confirmText !== 'YES') {
    return;
  }
  showGlobalStatus('Checking backup import...', 'pending');
  const result = await apiFetchJson(`/api/backup/import?${qs({ confirm: 'YES', json })}`, {
    label: 'Backup import',
    timeoutMs: 9000,
    retries: 0,
    expectOk: false
  });
  const payload = result.payload || {};
  showBackupMessage(payload.message || payload.error || result.error || 'Backup import checked', !result.ok || payload.ok === false);
  showGlobalStatus(payload.message || result.error || 'Backup import checked', result.ok ? 'success' : 'error');
});

$('factoryReset').addEventListener('click', async () => {
  if (!canMutateFromUi('factory reset')) {
    return;
  }
  const confirmText = window.prompt('Type YES to factory reset');
  if (confirmText !== 'YES') {
    return;
  }
  showGlobalStatus('Factory reset running...', 'pending');
  const result = await apiFetchJson(`/api/reset?${qs({ target: 'factory', confirm: 'YES' })}`, {
    label: 'Factory reset',
    timeoutMs: 9000,
    retries: 0
  });
  if (!result.ok) {
    showBackupMessage(result.error || 'Factory reset failed', true);
    showGlobalStatus(result.error || 'Factory reset failed', 'error');
    return;
  }
  const payload = result.payload;
  showBackupMessage(payload.message || 'Factory reset complete');
  showGlobalStatus('Factory reset complete', 'success');
  await refreshStateSafe({ showStatus: false });
  await refreshPalettesSafe();
  await refreshScenesSafe();
  await refreshFavoritesSafe();
});

$('previewPause').addEventListener('change', (event) => {
  previewPaused = event.target.checked;
});

$('surpriseButton').addEventListener('click', () => runSurprise().catch(console.error));
$('saveSurpriseScene').addEventListener('click', () => saveSurpriseAsScene().catch(console.error));

$('modeFilterBar').addEventListener('click', (event) => {
  const filter = event.target.closest('button')?.dataset.modeFilter;
  if (!filter) {
    return;
  }
  applyModeFilter(filter);
});

['transitionsEnabled', 'transitionDurationMs', 'transitionStyle'].forEach((id) => {
  $(id).addEventListener('change', () => saveTransitionControls().catch(console.error));
});

$('paletteEnabled').addEventListener('change', () => {
  selectPalette($('activePaletteId').value, $('paletteEnabled').checked).catch(console.error);
});

$('activePaletteId').addEventListener('change', () => {
  selectPalette($('activePaletteId').value, $('paletteEnabled').checked).catch(console.error);
});

$('savePalette').addEventListener('click', () => savePaletteFromForm().catch(console.error));
$('resetPalettes').addEventListener('click', () => resetPalettes().catch(console.error));

$('paletteList').addEventListener('click', (event) => {
  const button = event.target.closest('button');
  if (!button) {
    return;
  }
  const selectId = button.dataset.paletteSelect;
  const editId = button.dataset.paletteEdit;
  const deleteId = button.dataset.paletteDelete;
  if (selectId) {
    selectPalette(selectId, true).catch(console.error);
  } else if (editId) {
    editPalette(editId);
  } else if (deleteId) {
    deletePalette(deleteId).catch(console.error);
  }
});

$('favoriteBar').addEventListener('click', (event) => {
  const favoriteId = event.target.closest('button')?.dataset.favoriteLoad;
  if (!favoriteId) {
    return;
  }
  if (!canMutateFromUi('favorite load')) {
    return;
  }
  showGlobalStatus('Loading favorite...', 'pending');
  apiFetchJson(`/api/favorites/load?${qs({ id: favoriteId })}`, {
    label: 'Favorite load',
    timeoutMs: 4500,
    retries: 1
  })
    .then((result) => handleFavoritePayload(result, 'Favorite loaded', { refreshFavorites: false }))
    .catch(console.error);
});

document.querySelectorAll('[data-action-link]').forEach((button) => {
  button.addEventListener('click', () => {
    const params = new URLSearchParams(button.dataset.actionLink.split('?')[1] || '');
    copyActionLink(params.get('action') || 'off').catch(console.error);
  });
});

document.querySelectorAll('[data-timer-minutes]').forEach((button) => {
  button.addEventListener('click', () => {
    $('timerDuration').value = button.dataset.timerMinutes;
  });
});

$('timerAction').addEventListener('change', populateTimerScenes);
$('startTimer').addEventListener('click', () => startTimerFromControls().catch(console.error));
$('cancelTimer').addEventListener('click', () => cancelTimer().catch(console.error));
$('startBedtimeFade').addEventListener('click', () => startBedtimeFade().catch(console.error));
$('cancelBedtimeFade').addEventListener('click', () => cancelTimer().catch(console.error));

$('sceneList').addEventListener('click', (event) => {
  const loadId = event.target.dataset.sceneLoad;
  const duplicateId = event.target.dataset.sceneDuplicate;
  const exportId = event.target.dataset.sceneExport;
  const linkId = event.target.dataset.sceneLink;
  const renameId = event.target.dataset.sceneRename;
  const deleteId = event.target.dataset.sceneDelete;

  if (loadId) {
    showGlobalStatus('Loading scene...', 'pending');
    callScene(`/api/scenes/load?${qs({ id: loadId })}`, 'Scene loaded', true).catch(console.error);
    return;
  }
  if (duplicateId) {
    duplicateScene(duplicateId).catch(console.error);
    return;
  }
  if (exportId) {
    exportSceneJson(exportId).catch(console.error);
    return;
  }
  if (linkId) {
    copySceneLink(linkId).catch(console.error);
    return;
  }
  if (renameId) {
    const current = sceneCache.find((scene) => scene.id === renameId);
    const nextName = window.prompt('Scene name', current ? current.name : '');
    if (nextName !== null) {
      showGlobalStatus('Renaming scene...', 'pending');
      callScene(`/api/scenes/rename?${qs({ id: renameId, name: nextName })}`, 'Scene renamed').catch(console.error);
    }
    return;
  }
  if (deleteId) {
    showGlobalStatus('Deleting scene...', 'pending');
    callScene(`/api/scenes/delete?${qs({ id: deleteId })}`, 'Scene deleted').catch(console.error);
  }
});

$('colorPicker').addEventListener('input', (event) => {
  $('swatch').style.background = event.target.value;
  sendSoon({ color: event.target.value });
});

$('kelvin').addEventListener('input', (event) => {
  $('kelvinText').textContent = event.target.value;
  sendSoon({ kelvin: event.target.value });
});

$('mode').addEventListener('change', (event) => {
  updateVisibleControls();
  renderSelectedModeMetadata();
  send({ mode: event.target.value }).catch(console.error);
});

['nightGuardEnabled', 'nightGuardBlockFlashing', 'nightGuardPreferWarm'].forEach((id) => {
  $(id).addEventListener('change', () => updateNightGuardFromControls().catch(console.error));
});

$('nightGuardMaxBrightness').addEventListener('input', () => {
  debouncedNightGuardUpdate();
});

$('bootBehavior').addEventListener('change', (event) => {
  send({ bootBehavior: event.target.value }).catch(console.error);
});

[
  'strobeDelay',
  'flashOffDelay',
  'chasePeriod',
  'wavePeriod',
  'slowPulseCount',
  'slowPulseMax',
  'rainbowPeriod'
].forEach((id) => {
  $(id).addEventListener('change', () => send({ [id]: $(id).value }).catch(console.error));
});

$('candleHallFlickerAmount').addEventListener('input', (event) => {
  $('candleHallFlickerAmountValue').textContent = `${event.target.value} / 20`;
  sendSoon({ candleHallFlickerAmount: event.target.value });
});

document.addEventListener('click', (event) => {
  const retrySection = event.target.closest('[data-retry-section]')?.dataset.retrySection;
  if (!retrySection) {
    return;
  }
  if (retrySection === 'favorites') refreshFavoritesSafe().catch(console.error);
  if (retrySection === 'scenes') refreshScenesSafe({ showStatus: true }).catch(console.error);
  if (retrySection === 'palettes') refreshPalettesSafe({ showStatus: true }).catch(console.error);
  if (retrySection === 'preview') refreshStateSafe({ showStatus: true }).then(refreshPreviewFromCachedState).catch(console.error);
  if (retrySection === 'modes') refreshModesSafe({ showStatus: true }).catch(console.error);
});

async function bootUi() {
  uiHydrating = true;
  try {
    showGlobalStatus('Connecting...', 'pending');

    await refreshStateSafe({ showStatus: true });
    await delay(100);
    await refreshModesSafe({ showStatus: false });
    await delay(100);
    await refreshFavoritesSafe();
    await delay(100);
    await refreshScenesSafe();
    await delay(100);
    await refreshPalettesSafe();
    await delay(100);
    await refreshTimerSafe();

    refreshPreviewFromCachedState();
    showGlobalStatus(stateLoaded ? 'Connected' : 'State unavailable - controls preserved', stateLoaded ? 'success' : 'error');
  } finally {
    uiHydrating = false;
  }
}

requestAnimationFrame(drawPreview);
bootUi().catch((error) => {
  console.warn('UI boot failed', error);
  showGlobalStatus('Controller connection failed - retrying', 'error');
});
timerPoll = setInterval(() => refreshTimer().catch(console.error), 10000);
setInterval(() => refreshStateSafe({ showStatus: false }).catch(console.warn), 15000);
</script>
</body>
</html>
)rawliteral";

#endif
