from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_3A_Report.md"
PHASE_COMPLETION = PROJECT_ROOT / "docs" / "generated" / "Phase_Completion.md"


class Phase3ASmoothTransitionsContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {path.name: path.read_text(encoding="utf-8") for path in cls.sketch_files}
        cls.sketch = "\n".join(cls.files.values())

    def test_transition_settings_and_runtime_models_exist(self):
        types = self.files["ControllerTypes.h"]
        for required in [
            "bool transitionsEnabled",
            "uint16_t transitionDurationMs",
            "String transitionStyle",
            "struct TransitionState",
            "bool active",
            "String style",
            "uint32_t startedMs",
            "uint16_t durationMs",
            "bool targetStateApplied",
            "uint8_t startEffectiveBrightness",
            "uint8_t targetEffectiveBrightness",
        ]:
            self.assertIn(required, types)

        ino = self.files["BedroomLedController.ino"]
        for required in [
            "TransitionState transitionState",
            "RgbPixel previousFrame[LED_COUNT]",
            "RgbPixel targetFrame[LED_COUNT]",
            "uint8_t lastOutputEffectiveBrightness",
            '#include "TransitionControls.h"',
        ]:
            self.assertIn(required, ino)

    def test_transition_settings_persist_with_safe_defaults(self):
        settings = self.files["SettingsStorage.h"]
        for required in [
            "settings.transitionsEnabled = true",
            "settings.transitionDurationMs = 1500",
            'settings.transitionStyle = "fade"',
            'readJsonBool(json, "transitionsEnabled"',
            'readJsonInt(json, "transitionDurationMs"',
            'readJsonString(json, "transitionStyle"',
            'json += F(",\\"transitionsEnabled\\":")',
            'json += F(",\\"transitionDurationMs\\":")',
            'json += F(",\\"transitionStyle\\":\\"")',
        ]:
            self.assertIn(required, settings)
        self.assertNotIn("transitionState", settings)

    def test_transition_core_uses_static_buffers_and_nonblocking_fade(self):
        self.assertIn("TransitionControls.h", self.files)
        transitions = self.files["TransitionControls.h"]
        for required in [
            "String normalizeTransitionStyle",
            "bool isSupportedTransitionStyle",
            "bool isSupportedTransitionDuration",
            "uint8_t configuredEffectiveBrightness()",
            "uint32_t getTransitionElapsedMs()",
            "uint8_t getTransitionProgressPercent()",
            "uint8_t transitionEffectiveBrightness()",
            "void copyFrame",
            "uint8_t lerp8",
            "void blendFrames",
            "void beginTransitionToCurrentState()",
            "void cancelTransition",
            "bool renderTransitionFrame(uint32_t now)",
            "renderCurrentModeToFrame(now)",
            "blendFrames(previousFrame, targetFrame, frameBuffer",
            "transition progress does not save settings",
        ]:
            self.assertIn(required, transitions)
        self.assertNotIn("delay(", transitions)
        self.assertNotIn("new RgbPixel", transitions)
        self.assertNotIn("malloc", transitions)

    def test_output_pipeline_integrates_transition_once(self):
        rendering = self.files["LedRendering.h"]
        comfort = self.files["ComfortControls.h"]
        for required in [
            "transitionState.active",
            "renderTransitionFrame(now)",
            "outputFrameToStrip();",
            "lastOutputEffectiveBrightness = effectiveBrightness",
        ]:
            self.assertIn(required, rendering)
        self.assertIn("transitionEffectiveBrightness()", comfort)
        self.assertIn("configuredEffectiveBrightness()", self.sketch)
        self.assertNotIn("gamma8(gamma8", self.sketch)

    def test_transition_triggers_and_interactions_are_wired(self):
        for required in [
            "beginTransitionToCurrentState();",
            "loadSceneById",
            "runWarmDimNow",
            "runOffAction",
            "loadFavoriteById",
            "handleApiMode",
            "handleSet",
            "transitionTimerCompletionActive",
            "timer cancelled by manual lighting change",
            "timer fade owns final apply",
            "cancelTransition(false)",
            "startDiagnosticTest",
        ]:
            self.assertIn(required, self.sketch)

    def test_transition_api_state_and_diagnostics_exist(self):
        for required in [
            'server.on("/api/transitions", HTTP_GET, handleApiTransitions)',
            'server.on("/api/transitions/set", HTTP_GET, handleApiTransitionsSet)',
            "void handleApiTransitions()",
            "void handleApiTransitionsSet()",
            "String buildTransitionsJson()",
            "Invalid transition duration",
            "Unknown transition style",
            '"transitionsEnabled":',
            '"transitionStyle":"',
            '"transitionDurationMs":',
            '"transitionActive":',
            '"transitionProgressPercent":',
        ]:
            self.assertIn(required, self.sketch)

        diagnostics = self.files["Diagnostics.h"]
        for required in [
            "Transitions",
            "Transition active",
            "Transition progress",
            "transitionBufferBytes",
        ]:
            self.assertIn(required, diagnostics)

    def test_main_ui_has_transition_controls(self):
        web_ui = self.files["WebUi.h"]
        for required in [
            "<h2>Transitions</h2>",
            'id="transitionsEnabled"',
            'id="transitionDurationMs"',
            'id="transitionStyle"',
            'id="transitionStatus"',
            "updateTransitionControls",
            "saveTransitionControls",
            "apiFetchJson(`/api/transitions/set?",
            "Transitioning:",
            '<option value="fade">Fade</option>',
            '<option value="0">Instant</option>',
            '<option value="500">0.5 s</option>',
            '<option value="1500">1.5 s</option>',
            '<option value="3000">3 s</option>',
            '<option value="5000">5 s</option>',
        ]:
            self.assertIn(required, web_ui)

    def test_generated_report_and_phase_completion_are_updated(self):
        self.assertTrue(REPORT.exists(), "Phase 3A report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 3A",
            "Transition settings added",
            "Transition runtime model",
            "Buffer strategy",
            "Supported transition styles",
            "Fade",
            "Output pipeline integration",
            "Night Guard",
            "Scene/favorite/action integration",
            "Timer/bedtime fade interaction",
            "Diagnostic test interaction",
            "/api/transitions",
            "/api/transitions/set",
            "not physically tested on LEDs",
            "Phase 3B",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 3A",
            "smooth transition",
            "/api/transitions",
            "Fade",
            "transition buffers",
            "runtime-only",
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
