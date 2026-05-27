import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = ROOT / "docs" / "generated" / "BedroomLedController_ESP32S3_N16R8_Target_Report.md"


class Esp32S3N16R8TargetContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }
        cls.sketch = "\n".join(cls.files.values())

    def test_primary_target_profile_is_explicitly_esp32s3_n16r8(self):
        profile = self.files["HardwareProfile.h"]
        ino = self.files["BedroomLedController.ino"]

        for required in [
            'constexpr const char* ESP32S3_N16R8_FQBN = "esp32:esp32:esp32s3";',
            "ESP32S3_N16R8_OPTIONS",
            "FlashSize=16M",
            "PSRAM=opi",
            "PartitionScheme=app3M_fat9M_16MB",
            "#if defined(CONFIG_IDF_TARGET_ESP32S3)",
            'return "ESP32-S3";',
            "platformPsramSizeBytes()",
            "platformHasPsram()",
            "ESP.getPsramSize()",
            '"ESP32-S3-N16R8"',
        ]:
            self.assertIn(required, profile)

        self.assertIn("Board: ESP32-S3-N16R8", ino)
        self.assertIn("#elif defined(ESP32)", ino)
        self.assertIn("#include <WiFi.h>", ino)
        self.assertIn("#include <WebServer.h>", ino)
        self.assertIn("#include <HTTPUpdateServer.h>", ino)
        self.assertIn("WebServer server(80);", ino)

    def test_diagnostics_and_state_expose_esp32s3_target_truth(self):
        diagnostics = self.files["Diagnostics.h"]
        routes = self.files["WebRoutes.h"]
        management = self.files["ManagementControls.h"]

        for required in [
            "psramPresent",
            "psramSizeBytes",
            "ESP32S3_N16R8_FQBN",
            "ESP32S3_N16R8_OPTIONS",
            "boardFamily",
            "boardProfile",
        ]:
            self.assertIn(required, diagnostics + routes + management)

        self.assertIn("ESP32-S3 / N16R8 target", management)
        self.assertIn("Arduino CLI compile target", management)

    def test_report_and_mirror_are_updated_for_target_switch(self):
        self.assertTrue(REPORT.exists(), "ESP32-S3 target report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "ESP32-S3-N16R8 target migration",
            "esp32:esp32:esp32s3",
            "FlashSize=16M",
            "PSRAM=opi",
            "Python tests",
            "ESP32-S3 compile",
            "D1 mini / ESP8266",
            "not physically tested",
        ]:
            self.assertIn(required, report)

        worktree_code = {
            path.name: path.read_text(encoding="utf-8")
            for path in sorted(SKETCH_DIR.glob("*.ino")) + sorted(SKETCH_DIR.glob("*.h"))
        }
        arduino_code = {
            path.name: path.read_text(encoding="utf-8")
            for path in sorted(ARDUINO_SKETCH_DIR.glob("*.ino")) + sorted(ARDUINO_SKETCH_DIR.glob("*.h"))
        }
        self.assertEqual(worktree_code, arduino_code)


if __name__ == "__main__":
    unittest.main()
