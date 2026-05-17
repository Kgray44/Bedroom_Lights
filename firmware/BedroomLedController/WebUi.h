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
      <div class="sub">D1 mini WS2812 strip on D3 | <a href="/schedule">Schedule</a> | <a href="/diagnostics">Diagnostics</a> | <a href="/ota">OTA</a> | <a href="/api-docs">API</a></div>
    </div>
    <div class="pill" id="status">Connecting...</div>
  </header>

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
      </div>
      <div class="mode-description" id="modeDescription"></div>
      <div class="mode-tags" id="modeTags"></div>
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

function renderPreviewSolid(state, timeMs) {
  const base = previewBaseColor(state);
  return makePreviewPixels(() => base);
}

function renderPreviewStrobe(state, timeMs) {
  const seconds = Math.max(.1, Number(state?.strobeDelay || 0.5));
  const on = Math.floor(timeMs / (seconds * 1000)) % 2 === 0;
  const base = previewBaseColor(state);
  return makePreviewPixels(() => on ? base : previewBlack());
}

function renderPreviewFlash(state, timeMs) {
  const seconds = Math.max(.1, Number(state?.flashOffDelay || 0.5));
  const on = (timeMs % (seconds * 2000)) < 120;
  const base = previewBaseColor(state);
  return makePreviewPixels(() => on ? base : previewBlack());
}

function renderPreviewChase(state, timeMs) {
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
  const base = previewBaseColor(state);
  const period = Math.max(1, Number(state?.wavePeriod || 12)) * 1000;
  const phase = (timeMs % period) / period;
  return makePreviewPixels((index, position) => {
    const wave = (Math.sin((position + phase) * Math.PI * 2) + 1) / 2;
    return blendRgb(scaleRgb(base, 50), base, wave);
  });
}

function renderPreviewSlowWave(state, timeMs) {
  return renderPreviewWave({ ...state, wavePeriod: Math.max(10, Number(state?.wavePeriod || 18)) }, timeMs * .7);
}

function renderPreviewSlowPulse(state, timeMs) {
  const base = previewBaseColor(state);
  const period = Math.max(2, Number(state?.slowPulseMax || 20)) * 1000;
  const pulse = (Math.sin((timeMs % period) / period * Math.PI * 2) + 1) / 2;
  const dim = Math.max(18, (state?.effectiveBrightness ?? 180) * .16);
  const bright = state?.effectiveBrightness ?? state?.masterBrightness ?? 180;
  return makePreviewPixels(() => scaleRgb(base, dim + (bright - dim) * pulse));
}

function renderPreviewRainbow(state, timeMs) {
  const period = Math.max(1, Number(state?.rainbowPeriod || 12)) * 1000;
  const phase = (timeMs % period) / period;
  return makePreviewPixels((index, position) => scaleRgb(hsvToRgb((position + phase) * 360, .88, 1), state?.effectiveBrightness ?? 180));
}

function renderPreviewDawnBloom(state, timeMs) {
  const warm = scaleRgb({ r: 255, g: 166, b: 86 }, state?.effectiveBrightness ?? 180);
  const rose = scaleRgb({ r: 255, g: 116, b: 132 }, state?.effectiveBrightness ?? 180);
  return makePreviewPixels((index, position) => blendRgb(warm, rose, (Math.sin(position * Math.PI + timeMs / 3800) + 1) / 2));
}

function renderPreviewMoonTide(state, timeMs) {
  const base = scaleRgb({ r: 112, g: 172, b: 255 }, state?.effectiveBrightness ?? 180);
  const deep = scaleRgb({ r: 18, g: 42, b: 86 }, state?.effectiveBrightness ?? 180);
  return makePreviewPixels((index, position) => blendRgb(deep, base, (Math.sin(position * Math.PI * 2 + timeMs / 2600) + 1) / 2));
}

function renderPreviewVelvetAurora(state, timeMs) {
  const colors = activePreviewPaletteColors(state);
  const base = previewBaseColor(state);
  return makePreviewPixels((index, position) => scaleRgb(
    paletteOrBase(state, position + timeMs / 9000 + Math.sin(position * 8 + timeMs / 2400) * .04, base),
    state?.effectiveBrightness ?? 180
  ));
}

function renderPreviewLanternDrift(state, timeMs) {
  const amber = scaleRgb({ r: 255, g: 177, b: 92 }, state?.effectiveBrightness ?? 180);
  const ember = scaleRgb({ r: 255, g: 94, b: 42 }, state?.effectiveBrightness ?? 180);
  return makePreviewPixels((index, position) => blendRgb(amber, ember, .18 + .28 * ((Math.sin(timeMs / 1000 + position * 17) + 1) / 2)));
}

function renderPreviewRainGlass(state, timeMs) {
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
  const warm = kelvinToApproxRgb(4550);
  const cool = kelvinToApproxRgb(5400);
  const drift = (Math.sin(timeMs / 55000) + 1) / 2;
  return makePreviewPixels((index, position) => {
    const amount = Math.max(0, Math.min(1, position * .86 + drift * .14));
    return scaleRgb(blendRgb(warm, cool, amount), state?.effectiveBrightness ?? 180);
  });
}

function renderPreviewGoldenHour(state, timeMs) {
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
  const night = { r: 8, g: 18, b: 38 };
  const green = { r: 74, g: 228, b: 156 };
  const violet = { r: 120, g: 116, b: 255 };
  return makePreviewPixels((index, position) => {
    const veilA = (Math.sin(position * Math.PI * 2.4 + timeMs / 18000) + 1) / 2;
    const veilB = (Math.sin(position * Math.PI * 5.4 - timeMs / 26000) + 1) / 2;
    const sampled = samplePaletteColor(activePreviewPaletteColors(state), position + timeMs / 48000);
    const ribbon = sampled || blendRgb(green, violet, veilB * .55);
    const fold = Math.min(1, veilA * veilA * .68 + veilB * .16);
    return scaleRgb(blendRgb(night, ribbon, fold), (state?.effectiveBrightness ?? 180) * (.16 + fold * .36));
  });
}

function renderPreviewCandleHall(state, timeMs) {
  const shadow = { r: 42, g: 16, b: 8 };
  const flame = kelvinToApproxRgb(2350);
  return makePreviewPixels((index, position) => {
    let level = .08;
    let color = shadow;
    for (let candle = 0; candle < 6; candle++) {
      const center = candle / 5 + Math.sin(timeMs / (28000 + candle * 3500) + candle * 1.7) * .018;
      const falloff = Math.max(0, 1 - Math.abs(position - center) / .065);
      const pulse = .72 + .24 * ((Math.sin(timeMs / (6500 + candle * 600) + candle) + 1) / 2);
      level += falloff * falloff * pulse * .58;
      const sampled = samplePaletteColor(activePreviewPaletteColors(state), candle / 5) || flame;
      color = blendRgb(color, blendRgb(sampled, flame, .28), falloff * .55);
    }
    return scaleRgb(color, (state?.effectiveBrightness ?? 180) * Math.min(.72, level));
  });
}

function renderPreviewSlowPrism(state, timeMs) {
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

async function refreshPreviewState() {
  const response = await fetch('/api/state', { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    $('previewStatus').textContent = payload.error || 'Preview state unavailable.';
    return;
  }
  previewState = payload;
  updatePreviewLabels(payload);
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
  const response = await fetch(`/set?${qs(params)}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showFavoriteMessage(payload.error || 'Update failed', true);
    await refreshState();
    return;
  }
  applyState(payload.state || payload);
}

function sendSoon(params) {
  clearTimeout(debounceTimer);
  debounceTimer = setTimeout(() => send(params).catch(console.error), 70);
}

function updateBrightnessReadout(raw, percent) {
  $('masterBrightnessValue').textContent = raw;
  $('masterBrightnessPercent').textContent = `${percent}%`;
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
  const response = await fetch('/api/modes', { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    return;
  }
  modeMetadata = payload.modes || [];
  renderModeFilters();
  renderModeOptions();
  renderSelectedModeMetadata();
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
  const response = await fetch(`/api/transitions/set?${qs({
    enabled: $('transitionsEnabled').checked ? 1 : 0,
    durationMs: $('transitionDurationMs').value,
    style: $('transitionStyle').value
  })}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showFavoriteMessage(payload.error || 'Transition update failed', true);
    return;
  }
  if (payload.state) {
    applyState(payload.state);
  } else {
    await refreshState();
  }
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
  }).join('') || '<p class="note">No palettes available.</p>';

  updatePaletteControls(lastState || payload);
}

async function refreshPalettes() {
  const response = await fetch('/api/palettes', { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showPaletteMessage(payload.error || 'Palette list failed', true);
    return;
  }
  renderPalettes(payload);
}

async function selectPalette(id, enabled = $('paletteEnabled').checked) {
  const response = await fetch(`/api/palettes/select?${qs({ id, enabled: enabled ? 1 : 0 })}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showPaletteMessage(payload.error || 'Palette selection failed', true);
    return;
  }
  showPaletteMessage(payload.message || 'Palette selected');
  if (payload.state) {
    applyState(payload.state);
  } else {
    await refreshState();
  }
  await refreshPalettes();
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
  const response = await fetch(`${endpoint}?${qs(params)}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showPaletteMessage(payload.error || 'Palette save failed', true);
    return;
  }
  editingPaletteId = '';
  $('savePalette').textContent = 'Save Palette';
  $('paletteName').value = '';
  showPaletteMessage(payload.message || 'Palette saved');
  await refreshPalettes();
}

async function deletePalette(id) {
  const response = await fetch(`/api/palettes/delete?${qs({ id })}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showPaletteMessage(payload.error || 'Palette delete failed', true);
    return;
  }
  showPaletteMessage(payload.message || 'Palette deleted');
  if (payload.state) {
    applyState(payload.state);
  }
  await refreshPalettes();
}

async function resetPalettes() {
  const response = await fetch('/api/palettes/reset', { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showPaletteMessage(payload.error || 'Palette reset failed', true);
    return;
  }
  editingPaletteId = '';
  $('savePalette').textContent = 'Save Palette';
  showPaletteMessage(payload.message || 'Palettes reset');
  if (payload.state) {
    applyState(payload.state);
  }
  await refreshPalettes();
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
  lastState = state;
  previewState = state;
  $('status').textContent = `${state.hostname} - ${state.ip} - ${state.mode}`;
  $('mode').value = state.mode;
  $('bootBehavior').value = state.bootBehavior || 'restore';
  $('masterBrightness').value = state.masterBrightness;
  updateBrightnessReadout(state.masterBrightness, state.masterBrightnessPercent);
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
  const response = await fetch(`/api/scenes/export?${qs({ id: sceneId })}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showSceneMessage(payload.error || 'Scene export failed', true);
    return;
  }
  $('sceneImportJson').value = JSON.stringify(payload, null, 2);
  showSceneMessage('Scene exported');
}

async function duplicateScene(sceneId) {
  const response = await fetch(`/api/scenes/duplicate?${qs({ id: sceneId })}`, { cache: 'no-store' });
  await handleScenePayload(response, 'Scene duplicated');
}

async function importSceneJsonFromTextarea() {
  const json = $('sceneImportJson').value.trim();
  if (!json) {
    showSceneMessage('Scene JSON is required', true);
    return;
  }
  const response = await fetch(`/api/scenes/import?${qs({ json })}`, { cache: 'no-store' });
  await handleScenePayload(response, 'Scene imported');
}

async function restoreBuiltInScenesFromUi() {
  const response = await fetch('/api/scenes/reset-builtins', { cache: 'no-store' });
  await handleScenePayload(response, 'Built-in scenes restored');
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
  const response = await fetch('/api/timer', { cache: 'no-store' });
  const payload = await response.json();
  if (payload.ok) {
    renderTimerStatus(payload);
  }
}

function populateTimerScenes() {
  const options = sceneCache.map((scene) => `<option value="${escapeHtml(scene.id)}">${escapeHtml(scene.name)}</option>`).join('');
  $('timerScene').innerHTML = options || '<option value="">No scenes</option>';
  $('timerScene').disabled = $('timerAction').value !== 'scene' || !sceneCache.length;
}

async function startTimerFromControls() {
  const params = {
    minutes: $('timerDuration').value,
    mode: $('timerMode').value,
    action: $('timerAction').value
  };
  if (params.action === 'scene') {
    params.sceneId = $('timerScene').value;
  }

  const response = await fetch(`/api/timer/start?${qs(params)}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showFavoriteMessage(payload.error || 'Timer failed', true);
    return;
  }
  showFavoriteMessage(payload.message || 'Timer started');
  if (payload.state) {
    applyState(payload.state);
  }
  if (payload.timer) {
    renderTimerStatus(payload.timer);
  }
}

async function cancelTimer() {
  const response = await fetch('/api/timer/cancel', { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showFavoriteMessage(payload.message || payload.error || 'No active timer', true);
    if (payload.timer) {
      renderTimerStatus(payload.timer);
    }
    return;
  }
  showFavoriteMessage(payload.message || 'Timer cancelled');
  if (payload.state) {
    applyState(payload.state);
  }
  if (payload.timer) {
    renderTimerStatus(payload.timer);
  }
}

async function startBedtimeFade() {
  const response = await fetch(`/api/bedtime/start?${qs({
    minutes: $('bedtimeMinutes').value,
    target: $('bedtimeTarget').value,
    saveDefault: 1
  })}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showFavoriteMessage(payload.error || 'Bedtime fade failed', true);
    return;
  }
  showFavoriteMessage(payload.message || 'Bedtime fade started');
  if (payload.state) {
    applyState(payload.state);
  }
  if (payload.timer) {
    renderTimerStatus(payload.timer);
  }
}

async function refreshState() {
  const response = await fetch('/state', { cache: 'no-store' });
  const state = await response.json();
  applyState(state);
}

function renderFavorites(payload) {
  favoriteCache = payload.favorites || [];
  const visibleFavorites = favoriteCache.filter((favorite) => favorite.available);
  if (!visibleFavorites.length) {
    $('favoriteBar').innerHTML = '<p class="note">No favorites available.</p>';
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
  const response = await fetch('/api/favorites', { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showFavoriteMessage(payload.error || 'Favorite list failed', true);
    return;
  }
  renderFavorites(payload);
}

async function handleFavoritePayload(response, successMessage) {
  const payload = await response.json();
  if (!payload.ok) {
    showFavoriteMessage(payload.error || 'Favorite action failed', true);
    await refreshState();
    await refreshFavorites();
    return;
  }
  showFavoriteMessage(successMessage || payload.message || '');
  if (payload.state) {
    applyState(payload.state);
  } else {
    await refreshState();
  }
  await refreshFavorites();
  await refreshScenes();
}

async function runAction(name) {
  const response = await fetch(`/api/action?${qs({ name })}`, { cache: 'no-store' });
  await handleFavoritePayload(response, name === 'warmDimNow' ? 'Warm Dim Now' : 'Action completed');
}

async function updateNightGuardFromControls() {
  const response = await fetch(`/api/nightguard/set?${qs({
    enabled: $('nightGuardEnabled').checked ? 1 : 0,
    maxBrightness: $('nightGuardMaxBrightness').value,
    blockFlashing: $('nightGuardBlockFlashing').checked ? 1 : 0,
    preferWarm: $('nightGuardPreferWarm').checked ? 1 : 0
  })}`, { cache: 'no-store' });
  await handleFavoritePayload(response, 'Night Guard updated');
}

function renderScenes(payload) {
  sceneCache = payload.scenes || [];
  populateTimerScenes();
  $('sceneCount').textContent = `${payload.count || 0} / ${payload.maxScenes || 0} scenes`;
  if (!sceneCache.length) {
    $('sceneList').innerHTML = '<p class="note">No scenes saved.</p>';
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
  const response = await fetch('/api/scenes', { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showSceneMessage(payload.error || 'Scene list failed', true);
    return;
  }
  renderScenes(payload);
}

async function callScene(url, successMessage, refreshLighting = false) {
  const response = await fetch(url, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showSceneMessage(payload.error || 'Scene action failed', true);
    return;
  }
  showSceneMessage(successMessage || payload.message || '');
  if (payload.state) {
    applyState(payload.state);
  } else if (refreshLighting) {
    await refreshState();
  }
  await refreshScenes();
}

async function handleScenePayload(response, successMessage, refreshLighting = false) {
  const payload = await response.json();
  if (!payload.ok) {
    showSceneMessage(payload.error || 'Scene action failed', true);
    return;
  }
  showSceneMessage(successMessage || payload.message || '');
  if (payload.state) {
    applyState(payload.state);
  } else if (refreshLighting) {
    await refreshState();
  }
  await refreshScenes();
}

async function runSurprise() {
  const mood = $('surpriseMood').value;
  $('surpriseSummary').textContent = 'Choosing a safe surprise...';
  const response = await fetch(`/api/surprise?${qs({ mood, apply: 1 })}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    $('surpriseSummary').textContent = payload.error || 'Surprise Me failed';
    $('surpriseSummary').style.color = '#ff9a9a';
    return;
  }
  $('surpriseSummary').style.color = 'var(--accent-2)';
  const caps = [payload.powerCapped ? 'power cap' : '', payload.nightGuardCapped ? 'Night Guard cap' : ''].filter(Boolean).join(', ');
  $('surpriseSummary').textContent = `${payload.moodLabel}: ${payload.modeDisplayName}, brightness ${payload.brightness}/255${payload.paletteEnabled ? `, ${payload.paletteId}` : ''}${caps ? ` (${caps})` : ''}`;
  if (payload.state) {
    applyState(payload.state);
  } else {
    await refreshState();
  }
  await refreshPreviewState();
}

async function saveSurpriseAsScene() {
  const mood = $('surpriseMood').selectedOptions[0]?.textContent || 'Surprise';
  const name = `${mood} Surprise`;
  const response = await fetch(`/api/scenes/save?${qs({ name })}`, { cache: 'no-store' });
  await handleScenePayload(response, 'Surprise scene saved');
}

$('masterBrightness').addEventListener('input', (event) => {
  const raw = Number(event.target.value);
  updateBrightnessReadout(raw, Math.round(raw * 100 / 255));
  sendSoon({ masterBrightness: raw });
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
  fetch(`/api/scenes/save?${qs({ name })}`, { cache: 'no-store' })
    .then((response) => handleScenePayload(response, 'Scene saved'))
    .then(() => $('sceneName').value = '')
    .catch(console.error);
});

$('importSceneJson').addEventListener('click', () => importSceneJsonFromTextarea().catch(console.error));
$('restoreBuiltInScenes').addEventListener('click', () => restoreBuiltInScenesFromUi().catch(console.error));

$('panicWarm').addEventListener('click', () => {
  runAction('warmDimNow').catch(console.error);
});

$('resetFavorites').addEventListener('click', () => {
  fetch('/api/favorites/reset', { cache: 'no-store' })
    .then((response) => handleFavoritePayload(response, 'Favorites reset'))
    .catch(console.error);
});

$('exportFullBackup').addEventListener('click', async () => {
  try {
    const response = await fetch('/api/backup/export', { cache: 'no-store' });
    const payload = await response.json();
    if (!payload.ok) {
      showBackupMessage(payload.error || 'Backup export failed', true);
      return;
    }
    $('backupExportJson').value = JSON.stringify(payload, null, 2);
    showBackupMessage('Full backup exported');
  } catch (error) {
    showBackupMessage('Backup export failed', true);
  }
});

$('importFullBackup').addEventListener('click', async () => {
  const json = $('backupExportJson').value.trim();
  if (!json) {
    showBackupMessage('Backup JSON is required', true);
    return;
  }
  const confirmText = window.prompt('Type YES to validate backup import');
  if (confirmText !== 'YES') {
    return;
  }
  const response = await fetch(`/api/backup/import?${qs({ confirm: 'YES', json })}`, { cache: 'no-store' });
  const payload = await response.json();
  showBackupMessage(payload.message || payload.error || 'Backup import checked', !payload.ok);
});

$('factoryReset').addEventListener('click', async () => {
  const confirmText = window.prompt('Type YES to factory reset');
  if (confirmText !== 'YES') {
    return;
  }
  const response = await fetch(`/api/reset?${qs({ target: 'factory', confirm: 'YES' })}`, { cache: 'no-store' });
  const payload = await response.json();
  if (!payload.ok) {
    showBackupMessage(payload.error || 'Factory reset failed', true);
    return;
  }
  showBackupMessage(payload.message || 'Factory reset complete');
  await refreshState();
  await refreshPalettes();
  await refreshScenes();
  await refreshFavorites();
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
  fetch(`/api/favorites/load?${qs({ id: favoriteId })}`, { cache: 'no-store' })
    .then((response) => handleFavoritePayload(response, 'Favorite loaded'))
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
    fetch(`/api/scenes/load?${qs({ id: loadId })}`, { cache: 'no-store' })
      .then((response) => handleScenePayload(response, 'Scene loaded', true))
      .catch(console.error);
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
      fetch(`/api/scenes/rename?${qs({ id: renameId, name: nextName })}`, { cache: 'no-store' })
        .then((response) => handleScenePayload(response, 'Scene renamed'))
        .catch(console.error);
    }
    return;
  }
  if (deleteId) {
    fetch(`/api/scenes/delete?${qs({ id: deleteId })}`, { cache: 'no-store' })
      .then((response) => handleScenePayload(response, 'Scene deleted'))
      .catch(console.error);
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
  updateNightGuardFromControls().catch(console.error);
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

refreshState()
  .catch(() => $('status').textContent = 'Open the D1 mini IP address again');
refreshModes().catch(console.error);
refreshPalettes().catch(() => showPaletteMessage('Palette list unavailable', true));
refreshScenes().catch(() => showSceneMessage('Scene list unavailable', true));
refreshFavorites().catch(() => showFavoriteMessage('Favorite list unavailable', true));
refreshTimer().catch(console.error);
refreshPreviewState().catch(() => $('previewStatus').textContent = 'Preview state unavailable.');
requestAnimationFrame(drawPreview);
timerPoll = setInterval(() => refreshTimer().catch(console.error), 10000);
setInterval(() => refreshPreviewState().catch(console.error), 2500);
</script>
</body>
</html>
)rawliteral";

#endif
