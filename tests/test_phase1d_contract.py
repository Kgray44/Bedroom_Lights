from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
FINAL_REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_1_Final_Report.md"
PHASE_COMPLETION = PROJECT_ROOT / "docs" / "generated" / "Phase_Completion.md"


class Phase1DApiCleanupContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {path.name: path.read_text(encoding="utf-8") for path in cls.sketch_files}
        cls.sketch = "\n".join(cls.files.values())

    def test_future_facing_api_routes_exist_without_removing_legacy_routes(self):
        for required in [
            'server.on("/api/state", HTTP_GET, handleApiState)',
            'server.on("/api/brightness", HTTP_GET, handleApiBrightness)',
            'server.on("/api/color", HTTP_GET, handleApiColor)',
            'server.on("/api/temperature", HTTP_GET, handleApiTemperature)',
            'server.on("/api/mode", HTTP_GET, handleApiMode)',
            'server.on("/api/diagnostics", HTTP_GET, handleApiDiagnostics)',
            'server.on("/api/power", HTTP_GET, handleApiPower)',
            'server.on("/api/test", HTTP_GET, handleApiTest)',
            'server.on("/state", HTTP_GET, handleState)',
            'server.on("/set", HTTP_GET, handleSet)',
            'server.on("/diagnostics", HTTP_GET, handleDiagnosticsPage)',
            'httpUpdater.setup(&server, "/update", OTA_USER, OTA_PASS)',
            'ArduinoOTA.handle()',
        ]:
            self.assertIn(required, self.sketch)

    def test_api_json_helpers_validation_and_shared_mutation_functions_exist(self):
        for required in [
            "void sendJsonOk",
            "void sendJsonError",
            "buildStateJson()",
            "bool parseIntArg(const String& name, int& out)",
            "bool parseHexColor(const String& hex, uint8_t& r, uint8_t& g, uint8_t& b)",
            "bool tryModeFromString(const String& key, Mode& mode)",
            "bool setMasterBrightness(uint8_t value)",
            "bool setActiveColor(uint8_t r, uint8_t g, uint8_t b)",
            "bool setWhiteTemperature(uint16_t kelvin)",
            "bool setModeByName(const String& modeName)",
            "bool setGammaEnabled(bool enabled)",
            "bool setRgbGains(uint8_t rGain, uint8_t gGain, uint8_t bGain)",
            "bool setPowerConfig(",
            "sendJsonError(400",
            "Unknown mode",
            "Invalid color",
            "Invalid brightness value",
        ]:
            self.assertIn(required, self.sketch)

    def test_state_and_api_responses_include_consistent_ok_and_runtime_fields(self):
        for required in [
            '"ok":true',
            '"ok":false',
            '"hostname":"',
            '"mode":"',
            '"hex":"',
            '"kelvin":',
            '"masterBrightness":',
            '"masterBrightnessPercent":',
            '"gammaEnabled":',
            '"redGain":',
            '"bootBehavior":"',
            '"testActive":',
            '"testName":"',
            "application/json",
            "handleApiState",
            "handleApiBrightness",
            "handleApiColor",
            "handleApiTemperature",
            "handleApiMode",
        ]:
            self.assertIn(required, self.sketch)

    def test_set_route_reuses_shared_mutation_helpers(self):
        web_routes = self.files["WebRoutes.h"]
        handle_set = web_routes[web_routes.index("void handleSet()") : web_routes.index("void handleState()")]
        for required in [
            "setActiveColor(",
            "setWhiteTemperature(",
            "setModeByName(",
            "setMasterBrightness(",
            "setGammaEnabled(",
            "setRgbGains(",
            "setPowerConfig(",
            "sendJsonError(",
        ]:
            self.assertIn(required, handle_set)

    def test_final_report_documents_phase_1_hardening(self):
        self.assertTrue(FINAL_REPORT.exists(), "Final Phase 1 report is missing")
        report = FINAL_REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 1",
            "Current route map",
            "/api/state",
            "/api/brightness",
            "/api/color",
            "/api/temperature",
            "/api/mode",
            "Current settings schema",
            "Output pipeline order",
            "Power estimator",
            "does not measure actual current",
            "not physically tested on LEDs",
            "Deferred",
            "Phase 2A",
        ]:
            self.assertIn(required, report)

    def test_phase_completion_document_tracks_all_completed_subphases(self):
        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        doc = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase_Completion",
            "Phase 1A",
            "settings",
            "master brightness",
            "Phase 1B",
            "frame buffer",
            "output pipeline",
            "Phase 1C",
            "diagnostics",
            "power estimator",
            "Phase 1D",
            "local API cleanup",
            "final hardening",
        ]:
            self.assertIn(required, doc)

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
