import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
GENERATED = ROOT / "docs" / "generated"


class CriticalRuntimeRegressionContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }
        cls.sketch = "\n".join(cls.files.values())

    def test_state_json_does_not_emit_extra_quote_after_active_palette_name(self):
        web_routes = self.files["WebRoutes.h"]

        state_start = web_routes.index("String buildStateJson()")
        state_body = web_routes[state_start:web_routes.index("void sendJsonOk", state_start)]

        self.assertIn('R"json(","activePaletteName":")json"', state_body)
        self.assertIn('json += escapeJson(activePaletteName());', state_body)
        self.assertNotIn(
            'json += R"json(")json";\n  json += R"json(","hex":")json";',
            state_body,
        )
        self.assertRegex(
            state_body,
            r'json \+= escapeJson\(activePaletteName\(\)\);\s+json \+= R"json\(","hex":"\)json";',
        )

    def test_ui_hydration_and_state_loaded_guards_block_startup_mutations(self):
        web_ui = self.files["WebUi.h"]

        for required in [
            "let uiHydrating = true;",
            "let stateLoaded = false;",
            "function canMutateFromUi(",
            "if (!canMutateFromUi(",
            "stateLoaded = true;",
            "uiHydrating = true;",
            "uiHydrating = false;",
            "State unavailable - controls preserved",
        ]:
            self.assertIn(required, web_ui)

        mutation_functions = [
            "async function send(params)",
            "function sendSoon(params)",
            "async function saveTransitionControls()",
            "async function selectPalette(",
            "async function startTimerFromControls()",
            "async function startBedtimeFade()",
            "async function runAction(name)",
            "async function updateNightGuardFromControls()",
        ]
        for function_name in mutation_functions:
            start = web_ui.index(function_name)
            snippet = web_ui[start : web_ui.index("\n}", start) + 2]
            self.assertIn("canMutateFromUi", snippet, function_name)

    def test_satin_breathing_uses_monotonic_envelope_not_full_sine_for_exhale(self):
        led = self.files["LedRendering.h"]
        forwards = self.files["ForwardDeclarations.h"]

        self.assertIn("float easeInOut01(float t)", led)
        self.assertIn("float easeInOut01(float t);", forwards)

        start = led.index("void renderSatinBreathing")
        body = led[start : led.index("void renderPaperLanternRow", start)]
        self.assertIn("envelope = easeInOut01(phase / 0.38f);", body)
        self.assertIn("envelope = 1.0f - easeInOut01((phase - 0.50f) / 0.50f);", body)
        self.assertNotIn("sineEase8((phase - 0.50f) / 0.50f)", body)
        self.assertIn("smoothHash8", body)
        self.assertIn("warmPeak", body)
        self.assertIn("sampleActivePaletteOrColor", body)

    def test_temporal_smoothing_resets_on_major_state_changes_and_actions(self):
        web_routes = self.files["WebRoutes.h"]
        comfort = self.files["ComfortControls.h"]
        forwards = self.files["ForwardDeclarations.h"]

        self.assertIn("void resetTemporalSmoothing();", forwards)
        self.assertIn("void resetTemporalSmoothing()", self.files["LedRendering.h"])

        for function_name in [
            "void setMode(Mode mode)",
            "bool setMasterBrightness(uint8_t value)",
            "bool setActiveColor(uint8_t r, uint8_t g, uint8_t b)",
            "bool setWhiteTemperature(uint16_t kelvin)",
        ]:
            start = web_routes.index(function_name)
            snippet = web_routes[start : web_routes.index("\n}", start) + 2]
            self.assertIn("resetTemporalSmoothing();", snippet, function_name)

        for function_name in ["bool runWarmDimNow()", "bool runOffAction()"]:
            start = comfort.index(function_name)
            snippet = comfort[start : comfort.index("\n}", start) + 2]
            self.assertIn("resetTemporalSmoothing();", snippet, function_name)

    def test_mutation_logging_is_exposed_without_strings(self):
        ino = self.files["BedroomLedController.ino"]
        diagnostics = self.files["Diagnostics.h"]
        forwards = self.files["ForwardDeclarations.h"]

        self.assertIn("MutationAudit lastMutation;", ino)
        self.assertIn("void recordMutation(", forwards)
        self.assertIn("lastMutationRoute", diagnostics)
        self.assertIn("lastMutationAction", diagnostics)
        self.assertIn("lastMutationMs", diagnostics)
        self.assertIn("lastMutationBeforeBrightness", diagnostics)
        self.assertIn("lastMutationAfterBrightness", diagnostics)
        struct_match = re.search(
            r"struct\s+MutationAudit\s*\{(?P<body>[\s\S]*?)\};",
            self.files["ControllerTypes.h"],
        )
        self.assertIsNotNone(struct_match)
        self.assertNotIn("String", struct_match.group("body"))

    def test_report_documents_live_findings_and_unproven_items(self):
        report = GENERATED / "BedroomLedController_Critical_Runtime_Regression_Report.md"
        self.assertTrue(report.exists(), "critical runtime regression report is missing")
        text = report.read_text(encoding="utf-8")
        for required in [
            "invalid /api/state JSON",
            "UI-only hydration failure",
            "Satin Breathing envelope",
            "Physical LED visual status:",
            "Random off status:",
            "OTA upload:",
        ]:
            self.assertIn(required, text)


if __name__ == "__main__":
    unittest.main()
