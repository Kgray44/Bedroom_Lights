import json
import subprocess
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
GENERATED = ROOT / "docs" / "generated"
SIM_OUTPUT = GENERATED / "mode_simulations"
SIMULATOR = ROOT / "tools" / "simulate_modes.py"
REPORT = GENERATED / "BedroomLedController_Mode_Overhaul_Report.md"


MODE_RENDERERS = {
    "Solid": ("MODE_SOLID", "renderSolid"),
    "Strobe": ("MODE_STROBE", "renderStrobe"),
    "Flash": ("MODE_FLASH", "renderFlash"),
    "Chase": ("MODE_CHASE", "renderChase"),
    "Wave": ("MODE_WAVE", "renderWave"),
    "Slow Wave": ("MODE_SLOW_WAVE", "renderSlowWave"),
    "Slow Pulse": ("MODE_SLOW_PULSE", "renderSlowPulse"),
    "Rainbow": ("MODE_RAINBOW", "renderRainbow"),
    "Dawn Bloom": ("MODE_DAWN_BLOOM", "renderDawnBloom"),
    "Moon Tide": ("MODE_MOON_TIDE", "renderMoonTide"),
    "Velvet Aurora": ("MODE_VELVET_AURORA", "renderVelvetAurora"),
    "Lantern Drift": ("MODE_LANTERN_DRIFT", "renderLanternDrift"),
    "Rain Glass": ("MODE_RAIN_GLASS", "renderRainGlass"),
    "Satin Breathing": ("MODE_SATIN_BREATHING", "renderSatinBreathing"),
    "Paper Lantern Row": ("MODE_PAPER_LANTERN_ROW", "renderPaperLanternRow"),
    "Library Lamp": ("MODE_LIBRARY_LAMP", "renderLibraryLamp"),
    "Solar Noon": ("MODE_SOLAR_NOON", "renderSolarNoon"),
    "Golden Hour": ("MODE_GOLDEN_HOUR", "renderGoldenHour"),
    "Dream Aquarium": ("MODE_DREAM_AQUARIUM", "renderDreamAquarium"),
    "Circuit Glow": ("MODE_CIRCUIT_GLOW", "renderCircuitGlow"),
    "Window Sunrise": ("MODE_WINDOW_SUNRISE", "renderWindowSunrise"),
    "Ember Quilt": ("MODE_EMBER_QUILT", "renderEmberQuilt"),
    "Porcelain Sky": ("MODE_PORCELAIN_SKY", "renderPorcelainSky"),
    "Hearthline": ("MODE_HEARTHLINE", "renderHearthline"),
    "Evening Current": ("MODE_EVENING_CURRENT", "renderEveningCurrent"),
    "Starlit Linen": ("MODE_STARLIT_LINEN", "renderStarlitLinen"),
    "Deep Sea Glass": ("MODE_DEEP_SEA_GLASS", "renderDeepSeaGlass"),
    "Quiet Northern Lights": ("MODE_QUIET_NORTHERN_LIGHTS", "renderQuietNorthernLights"),
    "Candle Hall": ("MODE_CANDLE_HALL", "renderCandleHall"),
    "Slow Prism": ("MODE_SLOW_PRISM", "renderSlowPrism"),
    "Garden Shade": ("MODE_GARDEN_SHADE", "renderGardenShade"),
    "Snowfall Hush": ("MODE_SNOWFALL_HUSH", "renderSnowfallHush"),
    "Observatory Glow": ("MODE_OBSERVATORY_GLOW", "renderObservatoryGlow"),
    "Rainy Window": ("MODE_RAINY_WINDOW", "renderRainyWindow"),
    "Campfire Blanket": ("MODE_CAMPFIRE_BLANKET", "renderCampfireBlanket"),
    "Northern Whisper": ("MODE_NORTHERN_WHISPER", "renderNorthernWhisper"),
    "Storm Outside": ("MODE_STORM_OUTSIDE", "renderStormOutside"),
}


class FullModeAnimationOverhaulContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }
        cls.sketch = "\n".join(cls.files.values())

    def test_animation_strength_is_primary_persisted_api_and_ui_setting(self):
        types = self.files["ControllerTypes.h"]
        settings = self.files["SettingsStorage.h"]
        routes = self.files["WebRoutes.h"]
        forwards = self.files["ForwardDeclarations.h"]
        web_ui = self.files["WebUi.h"]
        scenes = self.files["ScenePresets.h"]

        self.assertIn("uint8_t animationStrength;", types)
        self.assertIn("settings.animationStrength = 128;", settings)
        self.assertIn('"animationStrength"', settings)
        self.assertIn("settings.animationStrength = constrain(intValue, 0, 255);", settings)
        self.assertIn("scene.animationStrength = constrain(intValue, 0, 255);", scenes)
        self.assertIn("scene.animationStrength = settings.animationStrength;", scenes)
        self.assertIn("scene.animationStrength = 128;", scenes)
        self.assertIn('R"json(,"animationStrength":)json"', routes)
        self.assertIn('R"json(,"animationStrengthPercent":)json"', routes)
        self.assertIn('server.on("/api/animation", HTTP_GET, handleApiAnimation);', routes)
        self.assertIn('server.on("/api/intensity", HTTP_GET, handleApiIntensity);', routes)
        self.assertIn("void handleApiAnimation();", forwards)
        self.assertIn('if (server.hasArg("animationStrength") || server.hasArg("animationIntensity"))', routes)
        self.assertIn('if (!server.hasArg("strength") && !server.hasArg("value"))', routes)
        self.assertIn("Invalid animation strength value", routes)
        self.assertIn("setAnimationStrength", routes)
        self.assertIn('"message":"Animation strength updated"', routes)

        self.assertEqual(1, web_ui.count('id="animationStrength"'))
        self.assertNotIn('id="animationIntensity"', web_ui)
        self.assertIn("Animation Strength", web_ui)
        self.assertIn("Subtle", web_ui)
        self.assertIn("Expressive", web_ui)
        self.assertIn("updateAnimationStrengthReadout", web_ui)
        self.assertIn("state.animationStrength", web_ui)
        self.assertIn("sendAnimationStrengthSoon(event.target.value)", web_ui)
        self.assertIn("/api/animation", web_ui)

    def test_strength_helpers_exist_and_each_mode_uses_them(self):
        render = self.files["LedRendering.h"]
        forwards = self.files["ForwardDeclarations.h"]
        for helper in [
            "float animationStrength01()",
            "float animationStrengthCentered()",
            "float strengthCurve(float minValue, float maxValue, float curve)",
            "float strengthLinear(float minValue, float maxValue)",
            "float strengthEase(float minValue, float maxValue)",
            "uint8_t strengthByte(uint8_t minValue, uint8_t maxValue)",
            "float modeSpeedMultiplier(Mode mode)",
            "float modeMotionMultiplier(Mode mode)",
            "float modeContrastMultiplier(Mode mode)",
            "float modeDensityMultiplier(Mode mode)",
            "float modeColorSpreadMultiplier(Mode mode)",
            "uint8_t modeFloorForStrength(uint8_t low, uint8_t high)",
            "uint8_t modeCeilingForStrength(uint8_t low, uint8_t high)",
        ]:
            self.assertIn(helper, render)
            self.assertIn(helper.replace(" {", ";").rstrip("{"), forwards)

        for mode_name, (_, renderer) in MODE_RENDERERS.items():
            start = render.index(f"void {renderer}")
            end = render.find("\nvoid ", start + 1)
            if end < 0:
                end = len(render)
            body = render[start:end]
            if mode_name == "Solid":
                self.assertNotIn("animationStrength", body)
                continue
            self.assertRegex(
                body,
                r"animationStrength|strength[A-Z]|mode(Speed|Motion|Contrast|Density|ColorSpread)Multiplier",
                f"{mode_name} does not appear to consume Animation Strength",
            )

    def test_named_modes_have_specific_lifecycle_models(self):
        render = self.files["LedRendering.h"]
        qnl = render[render.index("void renderQuietNorthernLights"):render.index("void renderCandleHall")]
        for required in [
            "struct AuroraRibbon",
            "MAX_AURORA_RIBBONS",
            "auroraRibbonContribution",
            "activeRibbonCount",
            "targetOpacity",
            "lifeDuration",
            "foldPhase",
            "foldSpeed",
            "direction",
            "respawn",
        ]:
            self.assertIn(required, qnl)
        self.assertNotIn("auroraWaveLayerCount", qnl)

        for renderer, required_tokens in {
            "renderRainGlass": ["dropletCount", "life", "bead", "trail", "dissolve", "spawn"],
            "renderRainyWindow": ["dropletCount", "life", "bead", "trail", "linger", "paneGlow"],
            "renderCandleHall": ["candleCount", "coreFalloff", "halo", "smoothedFlicker", "flickerAmount"],
            "renderStarlitLinen": ["starCount", "neighbor", "soft", "fade", "highlightBrightness"],
            "renderSnowfallHush": ["particleCount", "particleRadius", "fade", "neighbor", "drift"],
            "renderStormOutside": ["cloudRollSpeed", "flashSoftness", "postFlashFade", "nightGuard"],
        }.items():
            start = render.index(f"void {renderer}")
            end = render.find("\nvoid ", start + 1)
            body = render[start:end if end > 0 else len(render)]
            for token in required_tokens:
                self.assertIn(token, body, f"{renderer} missing {token}")

    def test_no_renderer_bypasses_output_pipeline_or_runtime_brightness(self):
        render = self.files["LedRendering.h"]
        mode_block = render[render.index("void renderCurrentModeToFrame"):render.index("void initPulse")]
        self.assertNotIn("strip.show", mode_block)
        self.assertNotIn("strip.setBrightness", mode_block)
        self.assertNotIn("delay(", mode_block)

        output = render[render.index("void outputFrameToStrip"):render.index("void resetAnimationState")]
        self.assertIn("applyChannelGain", output)
        self.assertIn("scaleByMasterBrightness", output)
        self.assertIn("gamma8", output)
        self.assertIn("strip.show()", output)

    def test_browser_preview_uses_animation_strength_for_all_modes(self):
        web_ui = self.files["WebUi.h"]
        self.assertIn("function previewStrength01", web_ui)
        for _, renderer in MODE_RENDERERS.values():
            preview = renderer.replace("render", "renderPreview")
            if preview == "renderPreviewSolid":
                continue
            start = web_ui.index(f"function {preview}")
            end = web_ui.find("\nfunction ", start + 1)
            body = web_ui[start:end if end > 0 else len(web_ui)]
            self.assertIn("previewStrength01", body, f"{preview} ignores Animation Strength")

    def test_simulator_generates_visual_proof_metrics_and_report(self):
        self.assertTrue(SIMULATOR.exists(), "tools/simulate_modes.py is missing")
        subprocess.run(
            [sys.executable, str(SIMULATOR), "--quiet"],
            cwd=ROOT,
            check=True,
            text=True,
            capture_output=True,
        )

        for path in [
            SIM_OUTPUT / "index.html",
            SIM_OUTPUT / "contact_sheet.html",
            SIM_OUTPUT / "mode_metrics.json",
            REPORT,
        ]:
            self.assertTrue(path.exists(), f"{path} was not generated")

        metrics = json.loads((SIM_OUTPUT / "mode_metrics.json").read_text(encoding="utf-8"))
        self.assertEqual([0, 64, 128, 192, 255], metrics["strengthLevels"])
        self.assertEqual([0, 2, 5, 10, 20, 40], metrics["timeSamplesSeconds"])
        for mode_name in MODE_RENDERERS:
            slug = mode_name.lower().replace(" ", "-")
            self.assertIn(slug, metrics["modes"])
            self.assertTrue((SIM_OUTPUT / f"{slug}.html").exists(), f"{mode_name} HTML missing")
            if mode_name != "Solid":
                mode_metrics = metrics["modes"][slug]
                self.assertTrue(mode_metrics["strengthResponseProven"], f"{mode_name} strength response failed")
                self.assertGreater(
                    mode_metrics["strengths"]["128"]["estimatedMotionScore"],
                    0,
                    f"{mode_name} has no normal-strength motion metric",
                )

        report = REPORT.read_text(encoding="utf-8")
        self.assertIn("Quiet Northern Lights no longer uses a simple global sine/blob as its primary visual model.", report)
        self.assertIn("Physical LED test status: not physically tested in this pass.", report)
        self.assertIn("Mode | Strength response proven? | Visual proof generated? | Metrics passed? | Notes", report)


if __name__ == "__main__":
    unittest.main()
