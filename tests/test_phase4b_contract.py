import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = ROOT / "docs" / "generated" / "BedroomLedController_Phase_4B_Report.md"
PHASE_COMPLETION = ROOT / "docs" / "generated" / "Phase_Completion.md"


PHASE4B_MODES = [
    ("windowSunrise", "windowsunrise", "MODE_WINDOW_SUNRISE", "Window Sunrise", "renderWindowSunrise", "renderPreviewWindowSunrise"),
    ("emberQuilt", "emberquilt", "MODE_EMBER_QUILT", "Ember Quilt", "renderEmberQuilt", "renderPreviewEmberQuilt"),
    ("porcelainSky", "porcelainsky", "MODE_PORCELAIN_SKY", "Porcelain Sky", "renderPorcelainSky", "renderPreviewPorcelainSky"),
    ("hearthline", "hearthline", "MODE_HEARTHLINE", "Hearthline", "renderHearthline", "renderPreviewHearthline"),
    ("eveningCurrent", "eveningcurrent", "MODE_EVENING_CURRENT", "Evening Current", "renderEveningCurrent", "renderPreviewEveningCurrent"),
    ("starlitLinen", "starlitlinen", "MODE_STARLIT_LINEN", "Starlit Linen", "renderStarlitLinen", "renderPreviewStarlitLinen"),
    ("deepSeaGlass", "deepseaglass", "MODE_DEEP_SEA_GLASS", "Deep Sea Glass", "renderDeepSeaGlass", "renderPreviewDeepSeaGlass"),
    ("quietNorthernLights", "quietnorthernlights", "MODE_QUIET_NORTHERN_LIGHTS", "Quiet Northern Lights", "renderQuietNorthernLights", "renderPreviewQuietNorthernLights"),
]


class Phase4BRemainingModeLibraryBatch1ContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in WORKTREE_SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }

    def test_phase4b_modes_are_registered_and_api_selectable(self):
        types = self.files["ControllerTypes.h"]
        routes = self.files["WebRoutes.h"]
        web_ui = self.files["WebUi.h"]

        for mode_id, token, enum_name, display_name, _, _ in PHASE4B_MODES:
            self.assertIn(enum_name, types)
            self.assertIn(f'token == "{token}"', routes)
            self.assertIn(f'case {enum_name}: return "{mode_id}"', routes)
            self.assertIn(f'<option value="{mode_id}">{display_name}</option>', web_ui)

        for dashed in [
            "window-sunrise",
            "ember-quilt",
            "porcelain-sky",
            "evening-current",
            "starlit-linen",
            "deep-sea-glass",
            "quiet-northern-lights",
        ]:
            self.assertIn(dashed.replace("-", ""), routes)

    def test_metadata_covers_phase4b_modes_with_safe_flags(self):
        metadata = self.files["ModeMetadata.h"]

        for mode_id, _, enum_name, display_name, _, _ in PHASE4B_MODES:
            self.assertIn(f'{{{enum_name}, "{mode_id}", "{display_name}"', metadata)
            row_start = metadata.index(f'{{{enum_name}, "{mode_id}", "{display_name}"')
            row_end = metadata.index("}", row_start)
            row = metadata[row_start:row_end]
            self.assertIn("true, true", row, f"{mode_id} should be animated and calm where appropriate")
            self.assertIn("false, false,", row, f"{mode_id} should not be flashing or Night Guard blocked")
            self.assertIn("true,", row, f"{mode_id} should include palette support")
            self.assertNotIn('"Alert"', row)

        for required in [
            "Morning",
            "Sleep",
            "Everyday",
            "Focus",
            "Evening",
            "Weather",
            "Colorful",
            "palette",
            "sleepSafe",
            "focusFriendly",
        ]:
            self.assertIn(required, metadata)

    def test_render_dispatch_and_algorithms_exist_without_output_bypass(self):
        render = self.files["LedRendering.h"]
        forwards = self.files["ForwardDeclarations.h"]

        for required in [
            "RgbPixel coolWhiteApprox(uint16_t kelvin)",
            "RgbPixel sampleNamedOrActivePalette(const char* fallbackPaletteId, uint16_t position, uint16_t maxPosition)",
            "uint8_t smoothStep8(uint8_t t)",
            "uint8_t triangleWave8(uint32_t now, uint32_t periodMs)",
            "uint8_t sineApprox8(uint32_t now, uint32_t periodMs, uint16_t phaseOffset)",
            "hash8",
        ]:
            self.assertIn(required, render)

        for _, _, enum_name, _, render_func, _ in PHASE4B_MODES:
            self.assertIn(f"void {render_func}(uint32_t now);", forwards)
            self.assertIn(f"case {enum_name}: {render_func}(now); break;", render)
            self.assertIn(f"void {render_func}(uint32_t now)", render)
            start = render.index(f"void {render_func}(uint32_t now)")
            end = render.find("\nvoid ", start + 1)
            body = render[start:] if end < 0 else render[start:end]
            self.assertNotIn("strip.show", body)
            self.assertNotIn("delay(", body)
            self.assertNotIn("String ", body)
            self.assertNotIn("new ", body)

    def test_preview_and_surprise_me_include_phase4b_modes(self):
        web_ui = self.files["WebUi.h"]
        surprise = self.files["SurpriseControls.h"]

        for mode_id, _, enum_name, _, _, preview_func in PHASE4B_MODES:
            self.assertIn(f"function {preview_func}", web_ui)
            self.assertIn(f"if (mode === '{mode_id}') return {preview_func}(state, timeMs);", web_ui)
            self.assertIn(enum_name, surprise)

        for required in [
            "MODE_PORCELAIN_SKY",
            "MODE_EVENING_CURRENT",
            "MODE_DEEP_SEA_GLASS",
            "MODE_EMBER_QUILT",
            "MODE_WINDOW_SUNRISE",
            "MODE_HEARTHLINE",
            "MODE_STARLIT_LINEN",
            "MODE_QUIET_NORTHERN_LIGHTS",
            "modeSupportsPalette(candidate.mode)",
            "modeIsFlashing",
            "modeBlockedByNightGuard",
        ]:
            self.assertIn(required, surprise)

        self.assertNotIn("WebSocket(", web_ui)
        self.assertNotIn("EventSource(", web_ui)
        self.assertNotIn("/api/preview-frame", web_ui)

    def test_scene_scheduler_and_existing_route_compatibility_are_preserved(self):
        scenes = self.files["ScenePresets.h"]
        schedule = self.files["ScheduleControls.h"]
        routes = self.files["WebRoutes.h"]

        for required in [
            "scene.mode = modeKey(settings.mode)",
            "tryModeFromString(scene.mode, parsedMode)",
            "setModeByName(scene.mode)",
            "scene.paletteEnabled = settings.paletteEnabled",
            "scene.paletteId = settings.activePaletteId",
        ]:
            self.assertIn(required, scenes)

        for required in [
            "loadSceneById(sceneId, error)",
            "startTimer(TIMER_MODE_FADE_TO_ACTION",
            "executeScheduleTarget",
            "server.on(\"/schedule\", HTTP_GET, handleSchedulePage)",
            "server.on(\"/api/schedule\", HTTP_GET, handleApiSchedule)",
            "server.on(\"/go\", HTTP_GET, handleGoLink)",
            "server.on(\"/api/modes\", HTTP_GET, handleApiModes)",
        ]:
            self.assertIn(required, schedule + routes)

    def test_report_and_phase_completion_are_updated(self):
        self.assertTrue(REPORT.exists(), "Phase 4B report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 4B",
            "Files changed",
            "New modes implemented",
            "Window Sunrise",
            "Ember Quilt",
            "Porcelain Sky",
            "Hearthline",
            "Evening Current",
            "Starlit Linen",
            "Deep Sea Glass",
            "Quiet Northern Lights",
            "Algorithm summary",
            "Metadata/tags",
            "Palette behavior",
            "Scene compatibility behavior",
            "Browser preview behavior",
            "Surprise Me integration",
            "Scheduler/timer/bedtime compatibility",
            "Night Guard behavior",
            "Transition behavior",
            "Existing mode cleanup",
            "Resource report",
            "Free heap after /api/schedule",
            "not measured",
            "Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.",
            "Phase 4C",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 4B",
            "Window Sunrise",
            "Ember Quilt",
            "Porcelain Sky",
            "Hearthline",
            "Evening Current",
            "Starlit Linen",
            "Deep Sea Glass",
            "Quiet Northern Lights",
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
