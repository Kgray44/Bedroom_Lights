from pathlib import Path
import re
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_1B_Report.md"


class Phase1BPipelineContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in cls.sketch_files
        }
        cls.sketch = "\n".join(cls.files.values())

    def test_frame_buffer_and_helpers_exist(self):
        for required in [
            "struct RgbPixel",
            "RgbPixel frameBuffer[LED_COUNT]",
            "void clearFrame()",
            "void setFramePixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b)",
            "void fillFrame(uint8_t r, uint8_t g, uint8_t b)",
            "uint32_t makeColor(uint8_t r, uint8_t g, uint8_t b)",
            "void outputFrameToStrip()",
        ]:
            self.assertIn(required, self.sketch)
        self.assertIn("if (index >= LED_COUNT)", self.sketch)

    def test_output_pipeline_order_and_no_double_brightness(self):
        led = self.files["LedRendering.h"]
        output = re.search(r"void outputFrameToStrip\(\) \{(?P<body>.*?)\n\}", led, re.S)
        self.assertIsNotNone(output, "outputFrameToStrip is missing")
        body = output.group("body")
        ordered = [
            "applyChannelGain",
            "scaleByMasterBrightness",
            "gamma8",
            "strip.setPixelColor",
            "strip.show",
        ]
        positions = [body.find(item) for item in ordered]
        self.assertTrue(all(pos >= 0 for pos in positions), positions)
        self.assertEqual(positions, sorted(positions), "Pipeline order is calibration, brightness, gamma, strip write, show")
        self.assertNotIn("strip.setBrightness(settings.masterBrightness)", self.sketch)

    def test_normal_modes_render_to_frame_without_showing_strip(self):
        led = self.files["LedRendering.h"]
        render_names = [
            "renderSolid",
            "renderStrobe",
            "renderFlash",
            "renderChase",
            "renderWave",
            "renderSlowWave",
            "renderSlowPulse",
            "renderRainbow",
            "renderDawnBloom",
            "renderMoonTide",
            "renderVelvetAurora",
            "renderLanternDrift",
            "renderRainGlass",
        ]
        for name in render_names:
            match = re.search(rf"void {name}\([^)]*\) \{{(?P<body>.*?)\n\}}", led, re.S)
            self.assertIsNotNone(match, name)
            body = match.group("body")
            self.assertNotIn("strip.show", body, name)
            self.assertNotIn("strip.setPixelColor", body, name)
        self.assertIn("renderCurrentModeToFrame", led)
        self.assertIn("outputFrameToStrip();", led)

    def test_gamma_and_calibration_are_settings_ui_state_and_settable(self):
        for required in [
            "gammaEnabled",
            "redGain",
            "greenGain",
            "blueGain",
            "id=\"gammaEnabled\"",
            "id=\"redGain\"",
            "id=\"greenGain\"",
            "id=\"blueGain\"",
            "server.hasArg(\"gammaEnabled\")",
            "server.hasArg(\"redGain\")",
            "server.hasArg(\"greenGain\")",
            "server.hasArg(\"blueGain\")",
            "applyChannelGain",
            "gamma8",
        ]:
            self.assertIn(required, self.sketch)

    def test_gamma_and_calibration_are_persisted(self):
        storage = self.files["SettingsStorage.h"]
        for required in [
            "\"gammaEnabled\"",
            "\"redGain\"",
            "\"greenGain\"",
            "\"blueGain\"",
            "settings.gammaEnabled = true",
            "settings.redGain = 255",
            "settings.greenGain = 255",
            "settings.blueGain = 255",
        ]:
            self.assertIn(required, storage)

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

    def test_generated_report_records_pipeline_and_limits(self):
        self.assertTrue(REPORT.exists(), "Phase 1B generated report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 1B",
            "frame buffer",
            "calibration",
            "master brightness",
            "gamma",
            "Compiled",
            "not physically tested",
            "Phase 1C",
        ]:
            self.assertIn(required, report)


if __name__ == "__main__":
    unittest.main()
