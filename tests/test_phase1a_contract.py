from pathlib import Path
import re
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
SKETCH = WORKTREE_SKETCH_DIR / "BedroomLedController.ino"
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_1A_Report.md"


class Phase1AContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.sketch = "\n".join(path.read_text(encoding="utf-8") for path in cls.sketch_files)

    def test_sketch_is_subdivided_and_mirrored(self):
        header_names = {path.name for path in WORKTREE_SKETCH_DIR.glob("*.h")}
        self.assertGreaterEqual(len(header_names), 4)
        self.assertIn("ControllerTypes.h", header_names)
        self.assertIn("SettingsStorage.h", header_names)
        self.assertIn("WebUi.h", header_names)
        self.assertIn("LedRendering.h", header_names)

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

    def test_runtime_settings_are_centralized(self):
        self.assertIn("struct ControllerSettings", self.sketch)
        self.assertRegex(self.sketch, r"ControllerSettings\s+settings\s*;")
        self.assertNotRegex(self.sketch, r"\bMode\s+currentMode\s*=")
        self.assertNotRegex(self.sketch, r"\bRgb\s+currentColor\s*=")
        self.assertNotRegex(self.sketch, r"\buint16_t\s+currentKelvin\s*=")

    def test_master_brightness_is_ui_state_and_settable(self):
        for required in [
            "masterBrightness",
            "masterBrightnessPercent",
            "id=\"masterBrightness\"",
            "id=\"masterBrightnessValue\"",
            "id=\"masterBrightnessPercent\"",
            "server.hasArg(\"masterBrightness\")",
            "applyMasterBrightness()",
        ]:
            self.assertIn(required, self.sketch)
        self.assertIn("server.hasArg(\"brightness\")", self.sketch)

    def test_littlefs_persistence_is_debounced(self):
        for required in [
            "#include <LittleFS.h>",
            "const char* SETTINGS_PATH = \"/settings.json\"",
            "SETTINGS_SAVE_DEBOUNCE_MS",
            "loadSettings()",
            "saveSettings()",
            "markSettingsDirty()",
            "serviceSettingsSave()",
            "LittleFS.begin()",
        ]:
            self.assertIn(required, self.sketch)
        self.assertRegex(self.sketch, r"if\s*\(\s*settingsDirty\s*&&")

    def test_boot_behavior_is_exposed_and_persisted(self):
        for required in [
            "bootBehavior",
            "restore",
            "startOff",
            "warmDim",
            "id=\"bootBehavior\"",
            "applyBootBehavior()",
        ]:
            self.assertIn(required, self.sketch)

    def test_generated_report_records_compile_and_limits(self):
        self.assertTrue(REPORT.exists(), "Phase 1A generated report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 1A",
            "Compiled",
            "not physically tested",
            "Phase 1B",
            "LittleFS",
            "master brightness",
        ]:
            self.assertIn(required, report)


if __name__ == "__main__":
    unittest.main()
