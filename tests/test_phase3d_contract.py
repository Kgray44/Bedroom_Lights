import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = ROOT / "docs" / "generated" / "BedroomLedController_Phase_3D_Report.md"
PHASE_COMPLETION = ROOT / "docs" / "generated" / "Phase_Completion.md"


NEW_MODES = [
    ("satinBreathing", "MODE_SATIN_BREATHING", "Satin Breathing", "renderSatinBreathing", "renderPreviewSatinBreathing"),
    ("paperLanternRow", "MODE_PAPER_LANTERN_ROW", "Paper Lantern Row", "renderPaperLanternRow", "renderPreviewPaperLanternRow"),
    ("libraryLamp", "MODE_LIBRARY_LAMP", "Library Lamp", "renderLibraryLamp", "renderPreviewLibraryLamp"),
    ("solarNoon", "MODE_SOLAR_NOON", "Solar Noon", "renderSolarNoon", "renderPreviewSolarNoon"),
    ("goldenHour", "MODE_GOLDEN_HOUR", "Golden Hour", "renderGoldenHour", "renderPreviewGoldenHour"),
    ("dreamAquarium", "MODE_DREAM_AQUARIUM", "Dream Aquarium", "renderDreamAquarium", "renderPreviewDreamAquarium"),
    ("circuitGlow", "MODE_CIRCUIT_GLOW", "Circuit Glow", "renderCircuitGlow", "renderPreviewCircuitGlow"),
]


class Phase3DNewModeBatchContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in WORKTREE_SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }

    def test_new_modes_are_registered_and_api_selectable(self):
        types = self.files["ControllerTypes.h"]
        routes = self.files["WebRoutes.h"]
        web_ui = self.files["WebUi.h"]

        for mode_id, enum_name, display_name, _, _ in NEW_MODES:
            token = mode_id.lower()
            self.assertIn(enum_name, types)
            self.assertIn(f'token == "{token}"', routes)
            self.assertIn(f'case {enum_name}: return "{mode_id}"', routes)
            self.assertIn(f'<option value="{mode_id}">{display_name}</option>', web_ui)

    def test_metadata_covers_all_new_modes_without_night_guard_blocking(self):
        metadata = self.files["ModeMetadata.h"]
        for mode_id, enum_name, display_name, _, _ in NEW_MODES:
            self.assertIn(f'{{{enum_name}, "{mode_id}", "{display_name}"', metadata)
            self.assertIn("description", metadata)
            self.assertIn("bestUsedFor", metadata)
            self.assertIn("supportsPalette", metadata)
            row_start = metadata.index(f'{{{enum_name}, "{mode_id}", "{display_name}"')
            row_end = metadata.index("}", row_start)
            row = metadata[row_start:row_end]
            self.assertIn("false, false,", row, f"{mode_id} should not be flashing or Night Guard blocked")
            self.assertNotIn('"Alert"', row)

        for category in ["Sleep", "Calm", "Everyday", "Focus", "Weather", "Experimental"]:
            self.assertIn(f'"{category}"', metadata)

    def test_rendering_helpers_and_new_mode_dispatch_exist(self):
        render = self.files["LedRendering.h"]
        for required in [
            "uint8_t clamp8(int value)",
            "uint8_t lerp8(uint8_t a, uint8_t b, uint8_t amount)",
            "RgbPixel blendRgb(RgbPixel a, RgbPixel b, uint8_t amount)",
            "RgbPixel scaleRgb(RgbPixel color, uint8_t scale)",
            "RgbPixel warmWhiteApprox(uint16_t kelvin)",
            "RgbPixel sampleActivePaletteOrColor(uint16_t position, uint16_t maxPosition)",
            "float phaseFromMillis(uint32_t now, uint32_t periodMs)",
            "uint8_t sineEase8(float phase)",
            "uint8_t triangleWave8(float phase)",
            "uint8_t hash8(uint16_t x)",
        ]:
            self.assertIn(required, render)

        for _, enum_name, _, render_func, _ in NEW_MODES:
            self.assertIn(f"case {enum_name}: {render_func}(now); break;", render)
            self.assertIn(f"void {render_func}(uint32_t now)", render)

        self.assertNotIn("delay(", render)
        for render_func in [mode[3] for mode in NEW_MODES]:
            start = render.index(f"void {render_func}(uint32_t now)")
            end = render.find("\nvoid ", start + 1)
            body = render[start:] if end < 0 else render[start:end]
            self.assertNotIn("strip.show", body)
            self.assertNotIn("String ", body)
            self.assertNotIn("new ", body)

    def test_palette_scene_surprise_and_preview_integration_exist(self):
        metadata = self.files["ModeMetadata.h"]
        scenes = self.files["ScenePresets.h"]
        surprise = self.files["SurpriseControls.h"]
        web_ui = self.files["WebUi.h"]

        for mode_id, enum_name, display_name, _, preview_func in NEW_MODES:
            self.assertIn(mode_id, metadata)
            self.assertIn(enum_name, surprise)
            self.assertIn(f"function {preview_func}", web_ui)
            self.assertIn(f"if (mode === '{mode_id}') return {preview_func}(state, timeMs);", web_ui)

        for required in [
            "scene.mode = modeKey(settings.mode)",
            "tryModeFromString(scene.mode, parsedMode)",
            "setModeByName(scene.mode)",
            "scene.paletteEnabled = settings.paletteEnabled",
            "scene.paletteId = settings.activePaletteId",
            "satinBreathing",
            "libraryLamp",
            "solarNoon",
        ]:
            self.assertIn(required, scenes)

        for required in [
            "MODE_SATIN_BREATHING",
            "MODE_PAPER_LANTERN_ROW",
            "MODE_GOLDEN_HOUR",
            "MODE_DREAM_AQUARIUM",
            "MODE_LIBRARY_LAMP",
            "MODE_SOLAR_NOON",
            "MODE_CIRCUIT_GLOW",
            "modeSupportsPalette(candidate.mode)",
            "modeIsFlashing",
            "modeBlockedByNightGuard",
        ]:
            self.assertIn(required, surprise)

        self.assertNotIn("WebSocket(", web_ui)
        self.assertNotIn("EventSource(", web_ui)
        self.assertNotIn("/api/preview-frame", web_ui)

    def test_report_and_phase_completion_are_updated(self):
        self.assertTrue(REPORT.exists(), "Phase 3D report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 3D",
            "Files changed",
            "New modes implemented",
            "Satin Breathing",
            "Paper Lantern Row",
            "Library Lamp",
            "Solar Noon",
            "Golden Hour",
            "Dream Aquarium",
            "Circuit Glow",
            "Palette behavior",
            "Scene compatibility",
            "Browser preview behavior",
            "Surprise Me integration",
            "Night Guard behavior",
            "Timer/transition interaction",
            "Existing mode cleanup",
            "Resource report",
            "Free heap after /api/state",
            "not measured",
            "Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.",
            "Phase 4A",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 3D",
            "Satin Breathing",
            "Paper Lantern Row",
            "Library Lamp",
            "Solar Noon",
            "Golden Hour",
            "Dream Aquarium",
            "Circuit Glow",
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
