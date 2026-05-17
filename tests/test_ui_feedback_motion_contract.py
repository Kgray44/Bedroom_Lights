import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
GENERATED = ROOT / "docs" / "generated"


class UiFeedbackMotionContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }
        cls.sketch = "\n".join(cls.files.values())

    def test_global_feedback_and_safe_api_helper_exist(self):
        web_ui = self.files["WebUi.h"]

        for required in [
            'id="globalStatus"',
            "function showGlobalStatus(",
            "async function apiFetchJson(",
            "AbortController",
            "cache: 'no-store'",
            "slice(0, 200)",
            "console.warn('API request failed'",
            "statusCode",
            "retryDelayMs",
            "expectOk",
            "Controller busy - retrying",
        ]:
            self.assertIn(required, web_ui)

    def test_startup_loading_is_serialized_and_preview_uses_cached_state(self):
        web_ui = self.files["WebUi.h"]

        for required in [
            "async function bootUi()",
            "await refreshStateSafe({ showStatus: true })",
            "await delay(100)",
            "await refreshModesSafe({ showStatus: false })",
            "await refreshFavoritesSafe()",
            "await refreshScenesSafe()",
            "await refreshPalettesSafe()",
            "await refreshTimerSafe()",
            "refreshPreviewFromCachedState()",
            "requestAnimationFrame(drawPreview)",
        ]:
            self.assertIn(required, web_ui)

        self.assertNotIn("setInterval(() => refreshPreviewState().catch(console.error), 2500)", web_ui)
        self.assertIn("setInterval(() => refreshStateSafe({ showStatus: false }).catch(console.warn), 15000)", web_ui)

    def test_failed_empty_sections_have_retryable_distinct_messages(self):
        web_ui = self.files["WebUi.h"]

        for required in [
            "No favorites configured. Use Reset Favorites to restore defaults.",
            "Favorite list unavailable.",
            "renderUnavailableSection('favoriteBar', 'Favorite list unavailable.', 'favorites')",
            "No scenes saved yet. Save the current light as a scene.",
            "Scene list unavailable.",
            "renderUnavailableSection('sceneList', 'Scene list unavailable.', 'scenes')",
            "No palettes available. Reset built-ins or save a palette.",
            "Palette list unavailable.",
            "renderUnavailableSection('paletteList', 'Palette list unavailable.', 'palettes')",
            "Preview state unavailable.",
            "data-retry-section=\"preview\"",
            "function renderUnavailableSection(",
        ]:
            self.assertIn(required, web_ui)

    def test_surprise_nightguard_and_action_feedback_are_hardened(self):
        web_ui = self.files["WebUi.h"]

        for required in [
            "async function runSurprise()",
            "try {",
            "finally {",
            "Choosing...",
            "Surprise failed - controller did not return valid JSON.",
            "async function updateNightGuardFromControls()",
            "Updating Night Guard...",
            "Night Guard enabled",
            "Night Guard disabled",
            "Night Guard update failed.",
            "debouncedNightGuardUpdate",
            "setControlsDisabled(",
            "Saving brightness...",
            "Brightness saved",
            "Changing mode...",
            "Mode changed",
        ]:
            self.assertIn(required, web_ui)

    def test_motion_smoothing_is_central_bounded_and_bypassed_for_utility_modes(self):
        config = self.files["Config.h"]
        ino = self.files["BedroomLedController.ino"]
        led = self.files["LedRendering.h"]
        forwards = self.files["ForwardDeclarations.h"]

        for required in [
            "ENABLE_MOTION_SMOOTHING",
            "DEFAULT_CALM_SMOOTHING",
            "DEFAULT_WEATHER_SMOOTHING",
            "DEFAULT_COLORFUL_SMOOTHING",
        ]:
            self.assertIn(required, config)

        self.assertIn("RgbPixel smoothedFrame[LED_COUNT]", ino)
        self.assertIn("bool smoothedFrameInitialized", ino)

        for required in [
            "uint8_t smoothHash8(",
            "uint8_t approach8(",
            "bool shouldApplyTemporalSmoothing(Mode mode)",
            "uint8_t smoothingStepForMode(Mode mode)",
            "void applyTemporalSmoothingToFrame()",
            "if (shouldApplyTemporalSmoothing(settings.mode))",
            "MODE_STROBE",
            "MODE_FLASH",
            "diagnosticTest.active",
            "transitionState.active",
        ]:
            self.assertIn(required, led)

        self.assertIn("uint8_t smoothHash8(uint16_t seed, uint32_t now, uint16_t periodMs);", forwards)
        self.assertIn("bool shouldApplyTemporalSmoothing(Mode mode);", forwards)
        self.assertIn("void applyTemporalSmoothingToFrame();", forwards)

    def test_output_pipeline_and_docs_record_honest_status(self):
        led = self.files["LedRendering.h"]

        update_start = led.index("void updateLeds()")
        update_body = led[update_start:led.index("void renderCurrentModeToFrame", update_start)]
        self.assertLess(update_body.index("applyTemporalSmoothingToFrame();"), update_body.index("outputFrameToStrip();"))

        output_start = led.index("void outputFrameToStrip()")
        output_body = led[output_start:led.index("void resetAnimationState()", output_start)]
        self.assertLess(output_body.index("applyChannelGain"), output_body.index("scaleByMasterBrightness"))
        self.assertLess(output_body.index("scaleByMasterBrightness"), output_body.index("gamma8"))
        self.assertLess(output_body.index("gamma8"), output_body.index("strip.show()"))

        for path_name, required in [
            ("BedroomLedController_Live_UI_Feedback_Report.md", "Serialized startup loading"),
            ("BedroomLedController_Motion_Smoothness_Report.md", "Physical LED visual status: not visually retested in this pass"),
            ("BedroomLedController_Final_Report.md", "UI Feedback + Motion Smoothness Pass"),
            ("BedroomLedController_Resource_And_Migration_Report.md", "UI feedback and motion smoothness pass"),
            ("BedroomLedController_Troubleshooting.md", "UI section says unavailable"),
            ("Phase_Completion.md", "UI Feedback + Motion Smoothness"),
        ]:
            path = GENERATED / path_name
            self.assertTrue(path.exists(), f"{path_name} is missing")
            self.assertIn(required, path.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
