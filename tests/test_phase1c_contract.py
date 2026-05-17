from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_1C_Report.md"


class Phase1CDiagnosticsContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {path.name: path.read_text(encoding="utf-8") for path in cls.sketch_files}
        cls.sketch = "\n".join(cls.files.values())

    def test_diagnostics_routes_and_page_exist(self):
        for required in [
            "Diagnostics.h",
            "server.on(\"/diagnostics\", HTTP_GET, handleDiagnosticsPage)",
            "server.on(\"/api/diagnostics\", HTTP_GET, handleApiDiagnostics)",
            "server.on(\"/api/power\", HTTP_GET, handleApiPower)",
            "server.on(\"/api/test\", HTTP_GET, handleApiTest)",
            "const char DIAGNOSTICS_HTML[] PROGMEM",
            "fetch('/api/diagnostics'",
            "fetch('/api/power'",
            "href=\"/diagnostics\"",
        ]:
            self.assertIn(required, self.sketch)

    def test_power_settings_are_persisted_and_settable(self):
        for required in [
            "supplyMillivolts",
            "supplyMilliamps",
            "supplySafetyMarginPercent",
            "controllerOverheadMilliamps",
            "settings.supplyMillivolts = 5000",
            "settings.supplyMilliamps = 5000",
            "settings.supplySafetyMarginPercent = 20",
            "settings.controllerOverheadMilliamps = 200",
            "server.hasArg(\"supplyMillivolts\")",
            "server.hasArg(\"supplyMilliamps\")",
            "server.hasArg(\"supplySafetyMarginPercent\")",
            "server.hasArg(\"controllerOverheadMilliamps\")",
        ]:
            self.assertIn(required, self.sketch)

    def test_power_estimate_helpers_and_json_fields_exist(self):
        for required in [
            "struct PowerEstimate",
            "buildPowerEstimate()",
            "recommendedBrightnessCap",
            "aboveRecommendedCap",
            "maxLedCurrentMilliamps",
            "estimatedTotalCurrentMilliamps",
            "usableSupplyCurrentMilliamps",
            "\"estimateOnly\":true",
            "\"recommendedBrightnessCap\":",
            "\"currentMasterBrightness\":",
            "\"aboveRecommendedCap\":",
        ]:
            self.assertIn(required, self.sketch)

    def test_warning_generation_covers_required_risks(self):
        diagnostics = self.files.get("Diagnostics.h", "")
        for required in [
            "buildWarningsJson",
            "power_brightness",
            "gpio0_boot_pin",
            "weak_wifi",
            "low_heap",
            "settings_storage",
            "full_white_current",
        ]:
            self.assertIn(required, diagnostics)

    def test_led_test_state_machine_and_rendering_exist(self):
        for required in [
            "struct DiagnosticTestState",
            "DiagnosticTestState diagnosticTest",
            "ControllerSettings previousSettingsBeforeTest",
            "startDiagnosticTest",
            "restorePreviousLightingState",
            "renderDiagnosticTestToFrame",
            "diagnosticTest.lastStepMs",
            "diagnosticTest.pixelIndex",
            "DIAGNOSTIC_TEST_BRIGHTNESS",
            "\"pixelWalk\"",
            "\"alternating\"",
            "\"whiteLow\"",
            "\"restore\"",
        ]:
            self.assertIn(required, self.sketch)
        self.assertIn("renderDiagnosticTestToFrame(now)", self.files["LedRendering.h"])
        self.assertIn("millis() - diagnosticTest.lastStepMs", self.sketch)

    def test_diagnostics_json_exposes_required_runtime_fields(self):
        diagnostics = self.files.get("Diagnostics.h", "")
        for required in [
            "\"ok\":true",
            "\"hostname\":\"",
            "\"wifiConnected\":",
            "\"rssi\":",
            "\"uptimeMs\":",
            "\"freeHeap\":",
            "\"ledCount\":",
            "\"ledPin\":\"D3/GPIO0\"",
            "\"mode\":\"",
            "\"colorHex\":\"#",
            "\"masterBrightness\":",
            "\"gammaEnabled\":",
            "\"redGain\":",
            "\"settingsDirty\":",
            "\"testActive\":",
            "\"warnings\":",
        ]:
            self.assertIn(required, diagnostics)

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

    def test_generated_report_records_scope_and_limits(self):
        self.assertTrue(REPORT.exists(), "Phase 1C generated report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 1C",
            "/diagnostics",
            "/api/diagnostics",
            "/api/power",
            "60 mA",
            "LED test",
            "not physically tested",
            "does not measure actual current",
            "Phase 1D",
        ]:
            self.assertIn(required, report)


if __name__ == "__main__":
    unittest.main()
