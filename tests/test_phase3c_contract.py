from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_3C_Report.md"
PHASE_COMPLETION = PROJECT_ROOT / "docs" / "generated" / "Phase_Completion.md"


class Phase3CBrowserPreviewSurpriseContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {path.name: path.read_text(encoding="utf-8") for path in cls.sketch_files}
        cls.sketch = "\n".join(cls.files.values())

    def test_surprise_model_globals_and_setup_are_wired(self):
        types = self.files["ControllerTypes.h"]
        for required in [
            "struct SurpriseCandidate",
            "String mood",
            "Mode mode",
            "Rgb color",
            "bool usingWhiteTemperature",
            "uint16_t whiteKelvin",
            "uint8_t brightness",
            "bool paletteEnabled",
            "String paletteId",
            "bool powerCapped",
            "bool nightGuardCapped",
        ]:
            self.assertIn(required, types)

        ino = self.files["BedroomLedController.ino"]
        for required in [
            '#include "SurpriseControls.h"',
            'String lastSurpriseSummary = "none"',
            'String lastSurpriseMood = "none"',
            'String lastSurpriseStatus = "none"',
        ]:
            self.assertIn(required, ino)

    def test_surprise_controls_endpoint_and_safety_rules_exist(self):
        self.assertIn("SurpriseControls.h", self.files)
        surprise = self.files["SurpriseControls.h"]
        for required in [
            "String normalizeSurpriseMood",
            "bool isSupportedSurpriseMood",
            "String buildSurpriseOptionsJson",
            "String buildSurpriseCandidateJson",
            "uint8_t surpriseBrightnessForMood",
            "uint8_t capSurpriseBrightness",
            "Mode selectSurpriseMode",
            "String selectSurprisePaletteId",
            "bool generateSurpriseCandidate",
            "bool applySurpriseCandidate",
            "void handleApiSurprise()",
            "void handleApiSurpriseOptions()",
            "modeIsFlashing",
            "modeBlockedByNightGuard",
            "MODE_STROBE",
            "MODE_FLASH",
            "buildPowerEstimate()",
            "nightGuardMaxBrightness",
            "diagnosticTest.active = false",
            "clearActiveTimer();",
            "beginTransitionToCurrentState();",
            "Unknown surprise mood",
        ]:
            self.assertIn(required, surprise)

        for mood in ["calm", "evening", "sleepSafe", "focus", "colorful", "cozy"]:
            self.assertIn(mood, surprise)

    def test_surprise_routes_state_and_diagnostics_exist(self):
        for required in [
            'server.on("/api/surprise", HTTP_GET, handleApiSurprise)',
            'server.on("/api/surprise/options", HTTP_GET, handleApiSurpriseOptions)',
            '"lastSurpriseSummary":"',
            '"lastSurpriseMood":"',
            '"lastSurpriseStatus":"',
            "handleApiSurprise",
            "handleApiSurpriseOptions",
        ]:
            self.assertIn(required, self.sketch)

        diagnostics = self.files["Diagnostics.h"]
        for required in [
            "Surprise Me",
            "lastSurpriseSummary",
            "lastSurpriseMood",
            "lastSurpriseStatus",
        ]:
            self.assertIn(required, diagnostics)

    def test_browser_preview_ui_is_canvas_based_and_browser_only(self):
        web_ui = self.files["WebUi.h"]
        for required in [
            "<h2>Live Preview</h2>",
            'id="previewCanvas"',
            'id="previewModeLabel"',
            'id="previewStatus"',
            'id="previewPause"',
            "const PREVIEW_PIXELS = 48",
            "requestAnimationFrame(drawPreview)",
            "document.visibilityState",
            "prefers-reduced-motion",
            "refreshPreviewState",
            "refreshPreviewFromCachedState",
            "setInterval(() => refreshStateSafe({ showStatus: false }).catch(console.warn), 15000)",
            "drawPreviewStrip",
            "Preview: approximate browser-side visualization",
        ]:
            self.assertIn(required, web_ui)

        for forbidden in ["WebSocket(", "EventSource(", "/api/preview-frame", "previewFrames"]:
            self.assertNotIn(forbidden, web_ui)

    def test_browser_preview_helpers_and_mode_renderers_exist(self):
        web_ui = self.files["WebUi.h"]
        for required in [
            "function hexToRgb",
            "function rgbToCss",
            "function scaleRgb",
            "function blendRgb",
            "function samplePaletteColor",
            "function kelvinToApproxRgb",
            "function activePreviewPaletteColors",
            "function previewBaseColor",
            "function renderPreviewSolid",
            "function renderPreviewStrobe",
            "function renderPreviewFlash",
            "function renderPreviewChase",
            "function renderPreviewWave",
            "function renderPreviewSlowWave",
            "function renderPreviewSlowPulse",
            "function renderPreviewRainbow",
            "function renderPreviewDawnBloom",
            "function renderPreviewMoonTide",
            "function renderPreviewVelvetAurora",
            "function renderPreviewLanternDrift",
            "function renderPreviewRainGlass",
            "function renderPreviewFallback",
        ]:
            self.assertIn(required, web_ui)

    def test_surprise_ui_and_save_scene_flow_exist(self):
        web_ui = self.files["WebUi.h"]
        for required in [
            "<h2>Surprise Me</h2>",
            'id="surpriseMood"',
            'id="surpriseButton"',
            'id="surpriseSummary"',
            'id="saveSurpriseScene"',
            '<option value="calm"',
            '<option value="evening"',
            '<option value="sleepSafe"',
            '<option value="focus"',
            '<option value="colorful"',
            '<option value="cozy"',
            "runSurprise",
            "apiFetchJson(`/api/surprise?",
            "saveSurpriseAsScene",
            "/api/scenes/save",
        ]:
            self.assertIn(required, web_ui)

    def test_report_and_phase_completion_are_updated(self):
        self.assertTrue(REPORT.exists(), "Phase 3C report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 3C",
            "Browser preview architecture",
            "Preview rendering method",
            "requestAnimationFrame",
            "Modes with dedicated preview approximations",
            "Surprise Me generator architecture",
            "Surprise moods",
            "Safety rules",
            "Palette integration",
            "Power cap behavior",
            "Resource report",
            "Free heap after /api/palettes",
            "not measured",
            "not physically tested on LEDs",
            "Phase 3D",
        ]:
            self.assertIn(required, report)
        self.assertNotIn("WebSocket LED frame", report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 3C",
            "Live Preview",
            "Surprise Me",
            "/api/surprise",
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
