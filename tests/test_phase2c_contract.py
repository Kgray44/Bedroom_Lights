from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_2C_Report.md"
PHASE_COMPLETION = PROJECT_ROOT / "docs" / "generated" / "Phase_Completion.md"


class Phase2CTimersBedtimeFadeContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {path.name: path.read_text(encoding="utf-8") for path in cls.sketch_files}
        cls.sketch = "\n".join(cls.files.values())

    def test_timer_state_model_is_runtime_only_and_serviced(self):
        types = self.files["ControllerTypes.h"]
        for required in [
            "enum TimerActionType",
            "TIMER_ACTION_NONE",
            "TIMER_ACTION_OFF",
            "TIMER_ACTION_WARM_DIM",
            "TIMER_ACTION_SCENE",
            "TIMER_ACTION_PANIC_WARM",
            "enum TimerModeType",
            "TIMER_MODE_DELAY_THEN_ACTION",
            "TIMER_MODE_FADE_TO_ACTION",
            "struct TimerLightingState",
            "struct ActiveTimerState",
            "bool active",
            "TimerModeType mode",
            "TimerActionType action",
            "String targetSceneId",
            "uint32_t startedMs",
            "uint32_t durationMs",
            "uint32_t fadeDurationMs",
            "bool fadeActive",
            "bool completed",
            "bool cancelled",
        ]:
            self.assertIn(required, types)

        self.assertIn("TimerControls.h", self.sketch)
        self.assertIn("ActiveTimerState activeTimer", self.files["BedroomLedController.ino"])
        self.assertIn("String lastTimerStatus", self.files["BedroomLedController.ino"])
        self.assertIn("serviceActiveTimer();", self.files["BedroomLedController.ino"])
        self.assertNotIn("activeTimer", self.files["SettingsStorage.h"])

    def test_bedtime_defaults_persist_but_active_timer_does_not(self):
        types = self.files["ControllerTypes.h"]
        settings = self.files["SettingsStorage.h"]
        for required in [
            "uint16_t bedtimeFadeDefaultMinutes",
            "String bedtimeFadeDefaultTarget",
            "bool bedtimeFadeTurnOffAtEnd",
        ]:
            self.assertIn(required, types)
        for required in [
            "settings.bedtimeFadeDefaultMinutes = 30",
            'settings.bedtimeFadeDefaultTarget = "warmDim"',
            "settings.bedtimeFadeTurnOffAtEnd = false",
            'readJsonInt(json, "bedtimeFadeDefaultMinutes"',
            'readJsonString(json, "bedtimeFadeDefaultTarget"',
            'readJsonBool(json, "bedtimeFadeTurnOffAtEnd"',
            'json += F(",\\"bedtimeFadeDefaultMinutes\\":")',
            'json += F(",\\"bedtimeFadeDefaultTarget\\":\\"")',
            'json += F(",\\"bedtimeFadeTurnOffAtEnd\\":")',
        ]:
            self.assertIn(required, settings)

    def test_timer_helpers_fade_engine_and_actions_exist(self):
        timers = self.files.get("TimerControls.h", "")
        for required in [
            "void clearActiveTimer()",
            "bool timerIsActive()",
            "uint32_t getTimerElapsedMs()",
            "uint32_t getTimerRemainingMs()",
            "uint8_t getTimerProgressPercent()",
            "bool startTimer(",
            "bool cancelActiveTimer(",
            "void serviceActiveTimer()",
            "(uint32_t)(millis() - activeTimer.startedMs)",
            "captureTimerLightingState",
            "buildTimerTargetState",
            "applyFadeStep",
            "completeActiveTimer",
            "runActionByName(\"off\"",
            "runActionByName(\"warmDimNow\"",
            "loadSceneById(activeTimer.targetSceneId",
            "restoreTimerLightingState(activeTimer.restoreState)",
            "markSettingsDirty()",
        ]:
            self.assertIn(required, timers)

    def test_timer_api_and_bedtime_api_routes_exist(self):
        for required in [
            'server.on("/api/timer", HTTP_GET, handleApiTimer)',
            'server.on("/api/timer/start", HTTP_GET, handleApiTimerStart)',
            'server.on("/api/timer/cancel", HTTP_GET, handleApiTimerCancel)',
            'server.on("/api/bedtime/start", HTTP_GET, handleApiBedtimeStart)',
            'server.on("/api/bedtime/cancel", HTTP_GET, handleApiBedtimeCancel)',
            "void handleApiTimer()",
            "void handleApiTimerStart()",
            "void handleApiTimerCancel()",
            "void handleApiBedtimeStart()",
            "void handleApiBedtimeCancel()",
            "buildTimerJson",
            "Timer started",
            "Timer cancelled",
            "Bedtime fade started",
            "Invalid timer minutes",
            "Unknown timer mode",
            "Unknown timer action",
            "Scene id is required",
        ]:
            self.assertIn(required, self.sketch)

    def test_timer_state_is_visible_in_state_and_diagnostics(self):
        for required in [
            '"timerActive":',
            '"timerMode":"',
            '"timerAction":"',
            '"timerTargetSceneId":"',
            '"timerDurationMs":',
            '"timerElapsedMs":',
            '"timerRemainingMs":',
            '"timerProgressPercent":',
            '"timerFadeActive":',
            '"bedtimeFadeActive":',
            '"lastTimerStatus":"',
            '"bedtimeFadeDefaultMinutes":',
            '"bedtimeFadeDefaultTarget":"',
            '"bedtimeFadeTurnOffAtEnd":',
        ]:
            self.assertIn(required, self.sketch)
        diagnostics = self.files["Diagnostics.h"]
        for required in [
            "Timer active",
            "Timer remaining",
            "Bedtime default",
            "lastTimerStatus",
        ]:
            self.assertIn(required, diagnostics)

    def test_main_ui_has_timer_and_bedtime_fade_controls(self):
        web_ui = self.files["WebUi.h"]
        for required in [
            "<h2>Sleep Timer</h2>",
            'id="timerDuration"',
            'data-timer-minutes="15"',
            'data-timer-minutes="30"',
            'data-timer-minutes="60"',
            'data-timer-minutes="120"',
            'id="timerMode"',
            'id="timerAction"',
            'id="timerScene"',
            'id="startTimer"',
            'id="cancelTimer"',
            'id="timerStatus"',
            "<h2>Bedtime Fade</h2>",
            'id="bedtimeMinutes"',
            'id="bedtimeTarget"',
            'id="startBedtimeFade"',
            "apiFetchJson('/api/timer'",
            "apiFetchJson(`/api/timer/start?",
            "apiFetchJson('/api/timer/cancel'",
            "apiFetchJson(`/api/bedtime/start?",
            "renderTimerStatus",
            "refreshTimer",
            "populateTimerScenes",
        ]:
            self.assertIn(required, web_ui)

    def test_interactions_preserve_existing_features(self):
        timers = self.files.get("TimerControls.h", "")
        for required in [
            "diagnosticTest.active = false",
            "isModeBlockedByNightGuard",
            "Night Guard blocks this timer scene",
            "findSceneById(sceneId)",
            "runActionByName",
            "loadSceneById",
            "settingsDirty",
            "fade step does not save settings",
        ]:
            self.assertIn(required, timers)
        for required in [
            'server.on("/api/favorites", HTTP_GET, handleApiFavorites)',
            'server.on("/api/action", HTTP_GET, handleApiAction)',
            'server.on("/api/nightguard", HTTP_GET, handleApiNightGuard)',
            'server.on("/api/scenes/load", HTTP_GET, handleApiScenesLoad)',
            'httpUpdater.setup(&server, "/update", OTA_USER, OTA_PASS)',
            "ArduinoOTA.handle()",
        ]:
            self.assertIn(required, self.sketch)

    def test_generated_report_and_phase_completion_are_updated(self):
        self.assertTrue(REPORT.exists(), "Phase 2C report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 2C",
            "Timer state model",
            "Timer actions supported",
            "Timer UI behavior",
            "/api/timer",
            "/api/timer/start",
            "/api/timer/cancel",
            "/api/bedtime/start",
            "Fade engine behavior",
            "Bedtime fade behavior",
            "bedtimeFadeDefaultMinutes",
            "Active timers do not persist across reboot",
            "flash-wear",
            "not physically tested on LEDs",
            "Phase 2D",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 2C",
            "one-shot timer",
            "bedtime fade",
            "/api/timer",
            "/api/bedtime/start",
            "active timers do not persist",
        ]:
            self.assertIn(required, completion)

    def test_sketch_copy_is_mirrored_and_code_only(self):
        worktree_code = {
            path.name: path.read_text(encoding="utf-8")
            for path in sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        }
        arduino_code = {
            path.name: path.read_text(encoding="utf-8")
            for path in sorted(ARDUINO_SKETCH_DIR.glob("*.ino")) + sorted(ARDUINO_SKETCH_DIR.glob("*.h"))
        }
        self.assertEqual(worktree_code, arduino_code)
        self.assertEqual([], list(ARDUINO_SKETCH_DIR.glob("*.md")))


if __name__ == "__main__":
    unittest.main()
