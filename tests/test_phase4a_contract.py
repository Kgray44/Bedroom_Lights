import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = ROOT / "docs" / "generated" / "BedroomLedController_Phase_4A_Report.md"
PHASE_COMPLETION = ROOT / "docs" / "generated" / "Phase_Completion.md"


class Phase4ASchedulerSceneLinksContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in WORKTREE_SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }

    def test_time_settings_model_and_ntp_service_exist(self):
        config = self.files["Config.h"]
        types = self.files["ControllerTypes.h"]
        sketch = self.files["BedroomLedController.ino"]
        settings = self.files["SettingsStorage.h"]
        schedule = self.files.get("ScheduleControls.h", "")

        for required in [
            "const char* SCHEDULES_PATH = \"/schedule.json\"",
            "const uint8_t MAX_SCHEDULE_ENTRIES = 8",
            "const int16_t DEFAULT_LOCAL_UTC_OFFSET_MINUTES = -240",
            "const uint32_t TIME_SYNC_RETRY_MS",
            "const uint32_t TIME_SYNC_REFRESH_MS",
        ]:
            self.assertIn(required, config)

        for required in [
            "int16_t localUtcOffsetMinutes",
            "struct TimeSyncState",
            "bool timeSynced",
            "uint32_t lastSyncAttemptMs",
            "uint32_t lastSyncSuccessEpoch",
            "char timeStatus[64]",
        ]:
            self.assertIn(required, types)

        self.assertIn("#include <time.h>", sketch)
        self.assertIn("TimeSyncState timeSync", sketch)
        self.assertIn("serviceTimeSync();", sketch)
        self.assertIn("settings.localUtcOffsetMinutes = DEFAULT_LOCAL_UTC_OFFSET_MINUTES", settings)
        self.assertIn('"localUtcOffsetMinutes"', settings)

        for required in [
            "void beginTimeSync(bool manual)",
            "void serviceTimeSync()",
            "bool isTimeSynced()",
            "String buildTimeJson()",
            "void handleApiTime()",
            "void handleApiTimeSync()",
            "void handleApiTimeSet()",
            "configTime(0, 0",
            "time(nullptr)",
        ]:
            self.assertIn(required, schedule)

    def test_schedule_model_storage_and_engine_exist(self):
        types = self.files["ControllerTypes.h"]
        sketch = self.files["BedroomLedController.ino"]
        schedule = self.files.get("ScheduleControls.h", "")

        for required in [
            "struct ScheduleEntry",
            "String id",
            "String name",
            "bool enabled",
            "uint8_t daysMask",
            "uint16_t minuteOfDay",
            "String targetType",
            "String targetId",
            "uint16_t fadeSeconds",
            "bool useTransition",
            "bool skipNext",
            "uint32_t lastRunEpoch",
            "uint32_t lastRunOccurrenceKey",
        ]:
            self.assertIn(required, types)

        self.assertIn("ScheduleEntry schedules[MAX_SCHEDULE_ENTRIES]", sketch)
        self.assertIn("uint8_t scheduleCount = 0", sketch)
        self.assertIn("loadSchedules();", sketch)
        self.assertIn("serviceScheduler();", sketch)

        for required in [
            "bool saveSchedules()",
            "void loadSchedules()",
            "bool loadScheduleObject",
            "String buildScheduleJson()",
            "bool validateScheduleEntry",
            "bool executeScheduleTarget",
            "void serviceScheduler()",
            "uint32_t scheduleOccurrenceKey",
            "skipNext",
            "clearActiveTimer();",
            "diagnosticTest.active = false",
            "loadSceneById",
            "runActionByName",
            "startTimer(TIMER_MODE_FADE_TO_ACTION",
        ]:
            self.assertIn(required, schedule)

    def test_schedule_routes_ui_diagnostics_and_state_exist(self):
        routes = self.files["WebRoutes.h"]
        diagnostics = self.files["Diagnostics.h"]
        web_ui = self.files["WebUi.h"]
        schedule_ui = self.files.get("ScheduleUi.h", "")

        for required in [
            'server.on("/schedule", HTTP_GET, handleSchedulePage)',
            'server.on("/api/time", HTTP_GET, handleApiTime)',
            'server.on("/api/time/sync", HTTP_GET, handleApiTimeSync)',
            'server.on("/api/time/set", HTTP_GET, handleApiTimeSet)',
            'server.on("/api/schedule", HTTP_GET, handleApiSchedule)',
            'server.on("/api/schedule/save", HTTP_GET, handleApiScheduleSave)',
            'server.on("/api/schedule/delete", HTTP_GET, handleApiScheduleDelete)',
            'server.on("/api/schedule/toggle", HTTP_GET, handleApiScheduleToggle)',
            'server.on("/api/schedule/skip", HTTP_GET, handleApiScheduleSkip)',
            'server.on("/api/schedule/run", HTTP_GET, handleApiScheduleRun)',
            'server.on("/go", HTTP_GET, handleGoLink)',
        ]:
            self.assertIn(required, routes)

        for required in [
            '"timeSynced":',
            '"currentLocalTime":"',
            '"localUtcOffsetMinutes":',
            '"scheduleCount":',
            '"nextScheduleSummary":"',
            '"lastScheduleStatus":"',
        ]:
            self.assertIn(required, routes)
            self.assertIn(required, diagnostics)

        for required in [
            '<a href="/schedule">Schedule</a>',
            "Next schedule",
            "Schedules paused: time not synced",
            "copySceneLink",
            "copyActionLink",
            "/go?scene=",
            "/go?action=off",
            "/go?action=warmDimNow",
        ]:
            self.assertIn(required, web_ui)

        for required in [
            "const char SCHEDULE_HTML[] PROGMEM",
            "<h1>Schedules</h1>",
            "id=\"scheduleList\"",
            "id=\"scheduleForm\"",
            "id=\"localUtcOffsetMinutes\"",
            "id=\"syncTime\"",
            "data-day-bit=\"0\"",
            "fetch('/api/schedule'",
            "/api/schedule/save",
            "/api/schedule/delete",
            "/api/schedule/toggle",
            "/api/schedule/skip",
            "/api/schedule/run",
            "/api/time/set",
            "/api/time/sync",
            "QR",
            "NFC",
        ]:
            self.assertIn(required, schedule_ui)

    def test_scene_action_links_reuse_existing_logic_and_support_json(self):
        schedule = self.files.get("ScheduleControls.h", "")
        for required in [
            "void handleGoLink()",
            "format=json",
            "server.hasArg(\"scene\")",
            "server.hasArg(\"action\")",
            "loadSceneById(sceneId, error)",
            "runActionByName(actionName, error)",
            "sendGoHtml",
            "local network",
            "Scene Loaded",
            "Action Run",
        ]:
            self.assertIn(required, schedule)

    def test_report_and_phase_completion_are_updated(self):
        self.assertTrue(REPORT.exists(), "Phase 4A report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 4A",
            "Files changed",
            "Time sync implementation",
            "Timezone/offset behavior",
            "Schedule data model",
            "/schedule.json",
            "Schedule API endpoints",
            "Schedule UI/editor behavior",
            "Schedule execution logic",
            "Skip next",
            "Scene/action link routes",
            "QR/NFC-friendly URL behavior",
            "QR code generation was deferred",
            "Local-network security notes",
            "Night Guard",
            "Resource report",
            "Free heap after /api/schedule",
            "not measured",
            "Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.",
            "Phase 4B",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 4A",
            "local weekly scheduler",
            "/api/schedule",
            "/schedule.json",
            "/go",
            "scene/action links",
            "resource report",
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
