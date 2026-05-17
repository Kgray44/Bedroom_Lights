#ifndef BEDROOM_LED_CONTROLLER_SCHEDULE_UI_H
#define BEDROOM_LED_CONTROLLER_SCHEDULE_UI_H

const char SCHEDULE_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Bedroom LED Schedules</title>
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
      --accent-2: #ffc680;
      --bad: #ff9a9a;
    }
    * { box-sizing: border-box; }
    body { margin: 0; min-height: 100vh; background: var(--bg); color: var(--text); font-family: system-ui, -apple-system, Segoe UI, sans-serif; }
    main { width: min(1080px, calc(100% - 28px)); margin: 0 auto; padding: 26px 0 34px; }
    header { display: flex; justify-content: space-between; align-items: end; gap: 16px; padding-bottom: 18px; }
    h1 { margin: 0; font-size: clamp(1.7rem, 4vw, 2.8rem); letter-spacing: 0; }
    h2 { margin: 0 0 12px; font-size: 1rem; letter-spacing: 0; }
    a { color: var(--accent); }
    .links { display: flex; gap: 12px; flex-wrap: wrap; color: var(--muted); }
    .grid { display: grid; grid-template-columns: minmax(320px, .72fr) 1fr; gap: 14px; align-items: start; }
    section { border: 1px solid var(--line); background: var(--panel); border-radius: 8px; padding: 16px; }
    label { display: grid; gap: 6px; color: var(--muted); font-size: .86rem; }
    input, select, button { min-height: 40px; border-radius: 8px; border: 1px solid var(--line); font: inherit; }
    input, select { width: 100%; color: var(--text); background: var(--panel-2); padding: 8px 10px; }
    button { cursor: pointer; color: #101214; background: var(--accent); border: 0; font-weight: 720; padding: 8px 11px; }
    button.secondary { color: var(--text); background: var(--panel-2); border: 1px solid var(--line); }
    .form { display: grid; gap: 12px; }
    .two { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
    .days { display: grid; grid-template-columns: repeat(7, minmax(0, 1fr)); gap: 7px; }
    .day { min-height: 38px; border: 1px solid var(--line); border-radius: 8px; background: var(--panel-2); color: var(--text); display: grid; place-items: center; font-weight: 700; cursor: pointer; }
    .day input { display: none; }
    .day:has(input:checked) { background: var(--accent); color: #101214; border-color: transparent; }
    .schedule-list { display: grid; gap: 10px; }
    .schedule-card { border: 1px solid var(--line); background: var(--panel-2); border-radius: 8px; padding: 12px; display: grid; gap: 9px; }
    .schedule-title { display: flex; justify-content: space-between; gap: 10px; align-items: center; font-weight: 720; }
    .meta { color: var(--muted); font-size: .88rem; line-height: 1.45; overflow-wrap: anywhere; }
    .actions { display: grid; grid-template-columns: repeat(5, minmax(0, 1fr)); gap: 8px; }
    .status { min-height: 22px; color: var(--accent-2); margin-top: 10px; font-size: .9rem; }
    .bad { color: var(--bad); }
    .note { color: var(--muted); line-height: 1.5; margin: 10px 0 0; font-size: .88rem; }
    @media (max-width: 820px) {
      header { display: block; }
      .links { margin-top: 10px; }
      .grid, .two, .actions { grid-template-columns: 1fr; }
      .days { grid-template-columns: repeat(4, minmax(0, 1fr)); }
    }
  </style>
</head>
<body>
<main>
  <header>
    <div>
      <h1>Schedules</h1>
      <div class="note">Local weekly routines run only after the controller has a valid clock.</div>
    </div>
    <nav class="links"><a href="/">Controls</a><a href="/diagnostics">Diagnostics</a><a href="/ota">OTA</a></nav>
  </header>

  <div class="grid">
    <section>
      <h2>Clock</h2>
      <div class="two">
        <label>UTC offset minutes
          <input id="localUtcOffsetMinutes" type="number" min="-720" max="840" step="1">
        </label>
        <label>Time status
          <input id="timeStatus" type="text" readonly>
        </label>
      </div>
      <div class="two" style="margin-top:10px">
        <button id="saveOffset" type="button">Save Offset</button>
        <button id="syncTime" type="button" class="secondary">Sync Time</button>
      </div>
      <p class="note" id="timeDetail">Loading time status...</p>
    </section>

    <section>
      <h2>Scene And Action Links</h2>
      <p class="note">Use copyable local-network links for QR labels, NFC tags, or bookmarks. QR code generation and NFC hardware writing are deferred; these URLs are ready for those tools.</p>
      <div class="two" style="margin-top:10px">
        <button type="button" id="copyOffLink" class="secondary">Copy Off Link</button>
        <button type="button" id="copyWarmLink" class="secondary">Copy Warm Dim Link</button>
      </div>
      <p class="note" id="linkStatus"></p>
    </section>

    <section>
      <h2>Edit Schedule</h2>
      <form id="scheduleForm" class="form">
        <input id="scheduleId" type="hidden">
        <label>Name
          <input id="scheduleName" maxlength="32" placeholder="Evening Cozy">
        </label>
        <div class="two">
          <label>Time
            <input id="scheduleTime" type="time" value="21:30">
          </label>
          <label>Target type
            <select id="targetType">
              <option value="scene">Scene</option>
              <option value="action">Action</option>
            </select>
          </label>
        </div>
        <label>Target
          <select id="targetId"></select>
        </label>
        <div class="days" aria-label="Days">
          <label class="day"><input type="checkbox" data-day-bit="0" checked>Sun</label>
          <label class="day"><input type="checkbox" data-day-bit="1" checked>Mon</label>
          <label class="day"><input type="checkbox" data-day-bit="2" checked>Tue</label>
          <label class="day"><input type="checkbox" data-day-bit="3" checked>Wed</label>
          <label class="day"><input type="checkbox" data-day-bit="4" checked>Thu</label>
          <label class="day"><input type="checkbox" data-day-bit="5" checked>Fri</label>
          <label class="day"><input type="checkbox" data-day-bit="6" checked>Sat</label>
        </div>
        <div class="two">
          <label>Fade seconds
            <input id="fadeSeconds" type="number" min="0" max="14400" step="1" value="0">
          </label>
          <label>Enabled
            <select id="enabled">
              <option value="1">Enabled</option>
              <option value="0">Disabled</option>
            </select>
          </label>
        </div>
        <label><span><input id="useTransition" type="checkbox" checked> Use normal transitions when not fading</span></label>
        <div class="two">
          <button type="submit">Save Schedule</button>
          <button id="newSchedule" type="button" class="secondary">New</button>
        </div>
      </form>
      <div class="status" id="formStatus"></div>
    </section>

    <section>
      <h2>Saved Schedules</h2>
      <div id="scheduleList" class="schedule-list"></div>
    </section>
  </div>
</main>

<script>
const $ = (id) => document.getElementById(id);
let schedules = [];
let scenes = [];
const ACTIONS = [
  ['off', 'Off'],
  ['warmDimNow', 'Warm Dim Now'],
  ['panicWarm', 'Panic Warm']
];
const SCHEDULE_ENDPOINTS = [
  '/api/schedule/save',
  '/api/schedule/delete',
  '/api/schedule/toggle',
  '/api/schedule/skip',
  '/api/schedule/run',
  '/api/time/set',
  '/api/time/sync'
];

function qs(values) {
  return new URLSearchParams(values).toString();
}

function escapeHtml(value) {
  return String(value)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

function setMessage(id, message, isError = false) {
  $(id).textContent = message || '';
  $(id).classList.toggle('bad', !!isError);
}

function dayMaskFromForm() {
  let mask = 0;
  document.querySelectorAll('[data-day-bit]').forEach((box) => {
    if (box.checked) {
      mask |= 1 << Number(box.dataset.dayBit);
    }
  });
  return mask;
}

function setDays(mask) {
  document.querySelectorAll('[data-day-bit]').forEach((box) => {
    box.checked = !!(mask & (1 << Number(box.dataset.dayBit)));
  });
}

function timeToMinute(value) {
  const [hours, minutes] = String(value || '00:00').split(':').map(Number);
  return Math.max(0, Math.min(1439, (hours || 0) * 60 + (minutes || 0)));
}

function minuteToTime(value) {
  const minute = Number(value || 0);
  return `${String(Math.floor(minute / 60)).padStart(2, '0')}:${String(minute % 60).padStart(2, '0')}`;
}

function daySummary(mask) {
  const labels = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];
  if (mask === 127) return 'Every day';
  return labels.filter((_, index) => mask & (1 << index)).join(', ') || 'No days';
}

function linkForScene(id) {
  return `${window.location.origin}/go?scene=${encodeURIComponent(id)}`;
}

function linkForAction(action) {
  return `${window.location.origin}/go?action=${encodeURIComponent(action)}`;
}

async function copyText(value) {
  await navigator.clipboard.writeText(value);
  setMessage('linkStatus', `Copied ${value}`);
}

function populateTargets() {
  if ($('targetType').value === 'action') {
    $('targetId').innerHTML = ACTIONS.map(([id, label]) => `<option value="${id}">${label}</option>`).join('');
    return;
  }
  $('targetId').innerHTML = scenes.map((scene) => `<option value="${escapeHtml(scene.id)}">${escapeHtml(scene.name)}</option>`).join('') || '<option value="">No scenes saved</option>';
}

function clearForm() {
  $('scheduleId').value = '';
  $('scheduleName').value = '';
  $('scheduleTime').value = '21:30';
  $('targetType').value = 'scene';
  setDays(127);
  $('fadeSeconds').value = '0';
  $('enabled').value = '1';
  $('useTransition').checked = true;
  populateTargets();
  setMessage('formStatus', '');
}

function editSchedule(entry) {
  $('scheduleId').value = entry.id || '';
  $('scheduleName').value = entry.name || '';
  $('scheduleTime').value = minuteToTime(entry.minuteOfDay);
  $('targetType').value = entry.targetType || 'scene';
  populateTargets();
  $('targetId').value = entry.targetId || '';
  setDays(entry.daysMask || 0);
  $('fadeSeconds').value = entry.fadeSeconds || 0;
  $('enabled').value = entry.enabled ? '1' : '0';
  $('useTransition').checked = entry.useTransition !== false;
}

function renderSchedules(payload) {
  schedules = payload.schedules || [];
  if (!schedules.length) {
    $('scheduleList').innerHTML = '<p class="note">No schedules saved yet.</p>';
    return;
  }

  $('scheduleList').innerHTML = schedules.map((entry) => {
    const targetLabel = entry.targetType === 'scene'
      ? `Scene ${entry.targetName || entry.targetId}`
      : `Action ${entry.targetId}`;
    const link = entry.targetType === 'scene' ? linkForScene(entry.targetId) : linkForAction(entry.targetId);
    return `
      <div class="schedule-card">
        <div class="schedule-title">
          <span>${escapeHtml(entry.name)}</span>
          <span>${entry.enabled ? 'On' : 'Off'}</span>
        </div>
        <div class="meta">${daySummary(entry.daysMask)} at ${minuteToTime(entry.minuteOfDay)}<br>${escapeHtml(targetLabel)}${entry.fadeSeconds ? `, fade ${entry.fadeSeconds}s` : ''}${entry.skipNext ? '<br>Skip next is armed' : ''}<br>${escapeHtml(link)}</div>
        <div class="actions">
          <button type="button" class="secondary" data-edit="${escapeHtml(entry.id)}">Edit</button>
          <button type="button" class="secondary" data-toggle="${escapeHtml(entry.id)}">${entry.enabled ? 'Disable' : 'Enable'}</button>
          <button type="button" class="secondary" data-skip="${escapeHtml(entry.id)}">Skip Next</button>
          <button type="button" class="secondary" data-run="${escapeHtml(entry.id)}">Run Now</button>
          <button type="button" class="secondary" data-delete="${escapeHtml(entry.id)}">Delete</button>
        </div>
      </div>
    `;
  }).join('');
}

async function refreshTime() {
  const payload = await fetch('/api/time', { cache: 'no-store' }).then((response) => response.json());
  $('localUtcOffsetMinutes').value = payload.localUtcOffsetMinutes ?? -240;
  $('timeStatus').value = payload.timeStatus || 'unknown';
  $('timeDetail').textContent = payload.timeSynced
    ? `Local time: ${payload.currentLocalTime}`
    : 'Schedules paused: time not synced';
}

async function refreshSchedules() {
  const payload = await fetch('/api/schedule', { cache: 'no-store' }).then((response) => response.json());
  if (!payload.ok) {
    setMessage('formStatus', payload.error || 'Schedule load failed', true);
    return;
  }
  renderSchedules(payload);
}

async function refreshScenes() {
  const payload = await fetch('/api/scenes', { cache: 'no-store' }).then((response) => response.json());
  scenes = payload.scenes || [];
  populateTargets();
}

async function saveSchedule(event) {
  event.preventDefault();
  const params = {
    id: $('scheduleId').value,
    name: $('scheduleName').value,
    enabled: $('enabled').value,
    daysMask: dayMaskFromForm(),
    minuteOfDay: timeToMinute($('scheduleTime').value),
    targetType: $('targetType').value,
    targetId: $('targetId').value,
    fadeSeconds: $('fadeSeconds').value,
    useTransition: $('useTransition').checked ? 1 : 0
  };
  const payload = await fetch(`/api/schedule/save?${qs(params)}`, { cache: 'no-store' }).then((response) => response.json());
  if (!payload.ok) {
    setMessage('formStatus', payload.error || 'Schedule save failed', true);
    return;
  }
  setMessage('formStatus', payload.message || 'Schedule saved');
  renderSchedules(payload);
}

async function callSchedule(endpoint, id) {
  const payload = await fetch(`/api/schedule/${endpoint}?${qs({ id })}`, { cache: 'no-store' }).then((response) => response.json());
  if (!payload.ok) {
    setMessage('formStatus', payload.error || 'Schedule action failed', true);
    return;
  }
  setMessage('formStatus', payload.message || 'Schedule updated');
  if (payload.schedules) renderSchedules(payload);
}

$('scheduleForm').addEventListener('submit', saveSchedule);
$('newSchedule').addEventListener('click', clearForm);
$('targetType').addEventListener('change', populateTargets);
$('saveOffset').addEventListener('click', async () => {
  const payload = await fetch(`/api/time/set?${qs({ localUtcOffsetMinutes: $('localUtcOffsetMinutes').value })}`, { cache: 'no-store' }).then((response) => response.json());
  setMessage('formStatus', payload.ok ? 'Clock offset saved' : (payload.error || 'Offset save failed'), !payload.ok);
  await refreshTime();
});
$('syncTime').addEventListener('click', async () => {
  const payload = await fetch('/api/time/sync', { cache: 'no-store' }).then((response) => response.json());
  setMessage('formStatus', payload.message || payload.error || 'Time sync requested', !payload.ok);
  await refreshTime();
});
$('copyOffLink').addEventListener('click', () => copyText(linkForAction('off')).catch(console.error));
$('copyWarmLink').addEventListener('click', () => copyText(linkForAction('warmDimNow')).catch(console.error));
$('scheduleList').addEventListener('click', (event) => {
  const button = event.target.closest('button');
  if (!button) return;
  const id = button.dataset.edit || button.dataset.toggle || button.dataset.skip || button.dataset.run || button.dataset.delete;
  if (!id) return;
  if (button.dataset.edit) {
    const entry = schedules.find((item) => item.id === id);
    if (entry) editSchedule(entry);
  } else if (button.dataset.toggle) {
    callSchedule('toggle', id).catch(console.error);
  } else if (button.dataset.skip) {
    callSchedule('skip', id).catch(console.error);
  } else if (button.dataset.run) {
    callSchedule('run', id).catch(console.error);
  } else if (button.dataset.delete) {
    callSchedule('delete', id).catch(console.error);
  }
});

refreshTime().catch(console.error);
refreshScenes().then(refreshSchedules).catch(console.error);
setInterval(() => refreshTime().catch(console.error), 30000);
</script>
</body>
</html>
)rawliteral";

#endif
