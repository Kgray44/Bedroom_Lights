import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = ROOT / "docs" / "generated" / "BedroomLedController_Phase_4C_Report.md"
PHASE_COMPLETION = ROOT / "docs" / "generated" / "Phase_Completion.md"


PHASE4C_MODES = [
    ("candleHall", "candlehall", "MODE_CANDLE_HALL", "Candle Hall", "renderCandleHall", "renderPreviewCandleHall"),
    ("slowPrism", "slowprism", "MODE_SLOW_PRISM", "Slow Prism", "renderSlowPrism", "renderPreviewSlowPrism"),
    ("gardenShade", "gardenshade", "MODE_GARDEN_SHADE", "Garden Shade", "renderGardenShade", "renderPreviewGardenShade"),
    ("snowfallHush", "snowfallhush", "MODE_SNOWFALL_HUSH", "Snowfall Hush", "renderSnowfallHush", "renderPreviewSnowfallHush"),
    ("observatoryGlow", "observatoryglow", "MODE_OBSERVATORY_GLOW", "Observatory Glow", "renderObservatoryGlow", "renderPreviewObservatoryGlow"),
    ("rainyWindow", "rainywindow", "MODE_RAINY_WINDOW", "Rainy Window", "renderRainyWindow", "renderPreviewRainyWindow"),
    ("campfireBlanket", "campfireblanket", "MODE_CAMPFIRE_BLANKET", "Campfire Blanket", "renderCampfireBlanket", "renderPreviewCampfireBlanket"),
    ("northernWhisper", "northernwhisper", "MODE_NORTHERN_WHISPER", "Northern Whisper", "renderNorthernWhisper", "renderPreviewNorthernWhisper"),
    ("stormOutside", "stormoutside", "MODE_STORM_OUTSIDE", "Storm Outside", "renderStormOutside", "renderPreviewStormOutside"),
]


class Phase4CRemainingModeLibraryBatch2ContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in WORKTREE_SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }

    def test_phase4c_modes_are_registered_and_api_selectable(self):
        types = self.files["ControllerTypes.h"]
        routes = self.files["WebRoutes.h"]
        web_ui = self.files["WebUi.h"]

        for mode_id, token, enum_name, display_name, _, _ in PHASE4C_MODES:
            self.assertIn(enum_name, types)
            self.assertIn(f'token == "{token}"', routes)
            self.assertIn(f'case {enum_name}: return "{mode_id}"', routes)
            self.assertIn(f'<option value="{mode_id}">{display_name}</option>', web_ui)

        for dashed in [
            "candle-hall",
            "slow-prism",
            "garden-shade",
            "snowfall-hush",
            "observatory-glow",
            "rainy-window",
            "campfire-blanket",
            "northern-whisper",
            "storm-outside",
        ]:
            self.assertIn(dashed.replace("-", ""), routes)

    def test_metadata_covers_phase4c_modes_with_safe_flags(self):
        metadata = self.files["ModeMetadata.h"]

        for mode_id, _, enum_name, display_name, _, _ in PHASE4C_MODES:
            self.assertIn(f'{{{enum_name}, "{mode_id}", "{display_name}"', metadata)
            row_start = metadata.index(f'{{{enum_name}, "{mode_id}", "{display_name}"')
            row_end = metadata.index("}", row_start)
            row = metadata[row_start:row_end]
            self.assertIn("true, true", row, f"{mode_id} should be animated and calm or gentle")
            self.assertIn("false, false,", row, f"{mode_id} should not be hard-flashing or Night Guard blocked")
            self.assertIn("true,", row, f"{mode_id} should include palette support")
            self.assertNotIn('"Alert"', row)

        for required in [
            "Evening",
            "Colorful",
            "Weather",
            "Nature",
            "Sleep",
            "Calm",
            "palette",
            "sleepSafe",
            "focusFriendly",
        ]:
            self.assertIn(required, metadata)

    def test_render_dispatch_and_algorithms_exist_without_output_bypass(self):
        render = self.files["LedRendering.h"]
        forwards = self.files["ForwardDeclarations.h"]

        for required in [
            "uint8_t softPulseEnvelope8(uint32_t now, uint32_t periodMs, uint16_t phaseOffset)",
            "RgbPixel sampleNamedOrActivePalette(const char* fallbackPaletteId, uint16_t position, uint16_t maxPosition)",
            "uint8_t sineApprox8(uint32_t now, uint32_t periodMs, uint16_t phaseOffset)",
            "uint8_t triangleWave8(uint32_t now, uint32_t periodMs)",
            "hash8",
        ]:
            self.assertIn(required, render)

        for _, _, enum_name, _, render_func, _ in PHASE4C_MODES:
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
            self.assertNotIn("random(", body)

    def test_candle_hall_flicker_amount_is_adjustable_and_persisted(self):
        types = self.files["ControllerTypes.h"]
        storage = self.files["SettingsStorage.h"]
        routes = self.files["WebRoutes.h"]
        render = self.files["LedRendering.h"]
        web_ui = self.files["WebUi.h"]

        self.assertIn("uint8_t candleHallFlickerAmount;", types)
        self.assertIn("settings.candleHallFlickerAmount = 3;", storage)
        self.assertIn('"candleHallFlickerAmount"', storage)
        self.assertIn("settings.candleHallFlickerAmount = constrain(intValue, 0, 20);", storage)
        self.assertIn('R"json(,"candleHallFlickerAmount":)json"', routes)
        self.assertIn('if (server.hasArg("candleHallFlickerAmount"))', routes)
        self.assertIn("setByteSetting(settings.candleHallFlickerAmount, requestedAmount, 0, 20, false);", routes)

        start = render.index("void renderCandleHall")
        body = render[start:render.index("void renderSlowPrism", start)]
        self.assertIn("settings.candleHallFlickerAmount", body)
        self.assertIn("flickerRange", body)
        self.assertIn("flickerFloor", body)
        self.assertIn("candleSeed", body)
        self.assertIn("brightnessScale", body)
        self.assertIn("wobbleAmplitude", body)
        self.assertIn("secondaryPulse", body)
        self.assertIn("microFlicker", body)

        self.assertIn('data-for="candleHall"', web_ui)
        self.assertIn('id="candleHallFlickerAmount"', web_ui)
        self.assertIn('max="20"', web_ui)
        self.assertIn("candleHallFlickerAmountValue", web_ui)
        self.assertIn("state.candleHallFlickerAmount", web_ui)
        self.assertIn("sendSoon({ candleHallFlickerAmount: event.target.value })", web_ui)
        self.assertIn("Number(state?.candleHallFlickerAmount ?? 3)", web_ui)
        self.assertIn("Math.min(20", web_ui)
        self.assertIn("candleTraits", web_ui)
        self.assertIn("brightnessScale", web_ui)

    def test_preview_and_surprise_me_include_phase4c_modes(self):
        web_ui = self.files["WebUi.h"]
        surprise = self.files["SurpriseControls.h"]

        for mode_id, _, enum_name, _, _, preview_func in PHASE4C_MODES:
            self.assertIn(f"function {preview_func}", web_ui)
            self.assertIn(f"if (mode === '{mode_id}') return {preview_func}(state, timeMs);", web_ui)
            self.assertIn(enum_name, surprise)

        for required in [
            '"id":"weather","label":"Weather"',
            '"id":"nature","label":"Nature"',
            "MODE_GARDEN_SHADE",
            "MODE_SNOWFALL_HUSH",
            "MODE_OBSERVATORY_GLOW",
            "MODE_RAINY_WINDOW",
            "MODE_CAMPFIRE_BLANKET",
            "MODE_NORTHERN_WHISPER",
            "MODE_STORM_OUTSIDE",
            "modeSupportsPalette(candidate.mode)",
            "modeIsFlashing",
            "modeBlockedByNightGuard",
        ]:
            self.assertIn(required, surprise + web_ui)

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
        self.assertTrue(REPORT.exists(), "Phase 4C report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 4C",
            "Files changed",
            "New modes implemented",
            "Candle Hall",
            "Slow Prism",
            "Garden Shade",
            "Snowfall Hush",
            "Observatory Glow",
            "Rainy Window",
            "Campfire Blanket",
            "Northern Whisper",
            "Storm Outside",
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
            "Phase 4D",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 4C",
            "Candle Hall",
            "Slow Prism",
            "Garden Shade",
            "Snowfall Hush",
            "Observatory Glow",
            "Rainy Window",
            "Campfire Blanket",
            "Northern Whisper",
            "Storm Outside",
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
