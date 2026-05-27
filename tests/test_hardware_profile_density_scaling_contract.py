import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = ROOT / "docs" / "generated" / "BedroomLedController_Hardware_Profile_And_Density_Scaling_Report.md"


class HardwareProfileDensityScalingContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }
        cls.sketch = "\n".join(cls.files.values())

    def test_hardware_profile_defaults_and_derived_counts_exist(self):
        self.assertIn("HardwareProfile.h", self.files, "HardwareProfile.h must own board and strip geometry")
        profile = self.files["HardwareProfile.h"]
        config = self.files["Config.h"]
        ino = self.files["BedroomLedController.ino"]
        sketch = self.sketch

        for required in [
            "struct LedHardwareProfile",
            "ACTIVE_HARDWARE_PROFILE",
            "STRIP_LENGTH_MM = 5000",
            "LED_DENSITY_PER_METER = 160.0f",
            "LED_COUNT_OVERRIDE = 800",
            "LEGACY_BASELINE_DENSITY_PER_METER = 60.0f",
            "LED_DATA_PIN = 5",
            "TARGET_FRAME_INTERVAL_MS = 33",
            "POWER_ESTIMATE_MILLIAMPS_PER_PIXEL = 18.75f",
            "POWER_ESTIMATE_SOURCE",
            "manufacturerRecommendation",
            "BTF FCOB 5V 160LED/m 5m",
            "ESP32-S3-N16R8",
            "calculateConfiguredLedCount",
            "LED_COUNT_AT_60_PER_METER",
            "LED_COUNT_AT_160_PER_METER",
            "LED_COUNT_AT_180_PER_METER",
        ]:
            self.assertIn(required, profile)

        for required in [
            '#include "HardwareProfile.h"',
            "constexpr uint16_t LED_COUNT = calculateConfiguredLedCount(",
            "constexpr uint8_t LED_PIN = LED_DATA_PIN",
            "constexpr uint8_t MASTER_BRIGHTNESS = DEFAULT_MASTER_BRIGHTNESS",
        ]:
            self.assertIn(required, config)
        self.assertIn('#include "HardwareProfile.h"', ino)

    def test_esp32_s3_and_esp8266_board_paths_are_conditional(self):
        ino = self.files["BedroomLedController.ino"]
        for required in [
            "#if defined(ESP8266)",
            "#include <ESP8266WiFi.h>",
            "#include <ESP8266WebServer.h>",
            "ESP8266WebServer server(80)",
            "#elif defined(ESP32)",
            "#include <WiFi.h>",
            "#include <WebServer.h>",
            "WebServer server(80)",
            '#error "Unsupported board"',
            "HTTPUpdateServer httpUpdater",
        ]:
            self.assertIn(required, ino)
        for required in [
            "ArduinoOTA.begin()",
            "filesystemBegin()",
            "configureWifiHostname()",
        ]:
            self.assertIn(required, self.sketch)
        self.assertNotIn("#include <ESP8266WiFi.h>\n#include <LittleFS.h>\n#include <math.h>", ino)

    def test_physical_unit_helpers_are_available_and_safe(self):
        render = self.files["LedRendering.h"]
        forwards = self.files["ForwardDeclarations.h"]

        for helper in [
            "uint16_t getConfiguredLedCount()",
            "float getPixelsPerMeter()",
            "float getMmPerPixel()",
            "uint16_t mmToPixels(float mm)",
            "uint16_t cmToPixels(float cm)",
            "uint16_t metersToPixels(float meters)",
            "float pixelToMm(uint16_t pixel)",
            "float pixelToMeters(uint16_t pixel)",
            "float normalizedPosition01(uint16_t pixel)",
            "float physicalXmm(uint16_t pixel)",
            "uint16_t clampPixelIndex(int32_t index)",
            "bool pixelInRange(int32_t index)",
            "float cyclicPixelDistance(float a, float b)",
            "float cyclicPhysicalDistanceMm(float aMm, float bMm)",
            "float pixelsForPhysicalLength(float mm)",
            "uint16_t lastPixelIndex()",
        ]:
            self.assertIn(helper, render)
            self.assertIn(helper + ";", forwards)

        for required in [
            "if (mm > 0.0f && pixels < 1.0f)",
            "constrain",
            "getConfiguredLedCount()",
            "LED_COUNT",
            "STRIP_LENGTH_MM",
        ]:
            self.assertIn(required, render)

    def test_density_sensitive_modes_use_physical_units_not_raw_pixel_widths(self):
        render = self.files["LedRendering.h"]

        required_tokens = [
            "CANDLE_CORE_WIDTH_MM",
            "CANDLE_GLOW_RADIUS_MM",
            "CANDLE_SPACING_MIN_MM",
            "AURORA_BASE_WAVELENGTH_MM",
            "AURORA_RIBBON_WIDTH_MM",
            "RAIN_GLASS_DROPLET_RADIUS_MM",
            "RAIN_GLASS_TRAIL_LENGTH_MM",
            "RAINY_WINDOW_TRAIL_LENGTH_MM",
            "SNOW_HALO_RADIUS_MM",
            "STAR_HALO_RADIUS_MM",
            "LANTERN_GLOW_RADIUS_MM",
            "PAPER_LANTERN_SPACING_MM",
            "MOON_TIDE_LONG_WAVELENGTH_MM",
            "DEEP_SEA_CAUSTIC_WAVELENGTH_MM",
            "CHASE_SEGMENT_LENGTH_MM",
            "SLOW_PULSE_MIN_WIDTH_MM",
            "SLOW_WAVE_MIN_WIDTH_MM",
        ]
        for token in required_tokens:
            self.assertIn(token, render)

        mode_slices = {
            "renderCandleHall": ("renderCandleHall", "renderSlowPrism"),
            "renderQuietNorthernLights": ("renderQuietNorthernLights", "renderCandleHall"),
            "renderRainGlass": ("renderRainGlass", "renderSatinBreathing"),
            "renderRainyWindow": ("renderRainyWindow", "renderCampfireBlanket"),
            "renderPaperLanternRow": ("renderPaperLanternRow", "renderLibraryLamp"),
            "renderSnowfallHush": ("renderSnowfallHush", "renderObservatoryGlow"),
            "renderObservatoryGlow": ("renderObservatoryGlow", "renderRainyWindow"),
        }
        for mode_name, (start_name, end_name) in mode_slices.items():
            start = render.index(f"void {start_name}")
            end = render.index(f"void {end_name}", start + 1)
            body = render[start:end]
            self.assertIn("physicalXmm", body, mode_name)
            self.assertTrue(
                "mmToPixels" in body or "pixelsForPhysicalLength" in body or "cyclicPhysicalDistanceMm" in body,
                f"{mode_name} should convert physical dimensions",
            )
            self.assertNotIn("strip.show", body, mode_name)
            self.assertNotIn("delay(", body, mode_name)

        candle = render[render.index("void renderCandleHall"):render.index("void renderSlowPrism")]
        self.assertNotIn("static_cast<float>(LED_COUNT) * (0.034f", candle)
        rain = render[render.index("void renderRainGlass"):render.index("void renderSatinBreathing")]
        self.assertNotIn("now * speed * LED_COUNT", rain)
        self.assertNotIn(" / 8.0f", rain)

    def test_power_state_and_diagnostics_expose_profile_truth(self):
        diagnostics = self.files["Diagnostics.h"]
        routes = self.files["WebRoutes.h"]
        web_ui = self.files["WebUi.h"]
        types = self.files["ControllerTypes.h"]

        for required in [
            "float milliampsPerPixelFullWhite;",
            "const char* estimateSource;",
        ]:
            self.assertIn(required, types)

        for required in [
            "ACTIVE_HARDWARE_PROFILE.milliampsPerPixelFullWhite",
            "ACTIVE_HARDWARE_PROFILE.estimateSource",
            "configuredLedCount",
            "stripProfile",
            "stripLengthMm",
            "ledDensityPerMeter",
            "mmPerPixel",
            "targetFrameIntervalMs",
            "powerEstimateMilliampsPerPixel",
            "powerEstimateSource",
            "boardFamily",
            "boardProfile",
            "dataPin",
        ]:
            self.assertIn(required, diagnostics + routes)

        power_body = diagnostics[diagnostics.index("PowerEstimate buildPowerEstimate()"):diagnostics.index("void appendWarning", diagnostics.index("PowerEstimate buildPowerEstimate()"))]
        self.assertNotIn("* 60UL", power_body)
        self.assertIn("estimate.milliampsPerPixelFullWhite", power_body)
        self.assertIn("estimate.estimateSource", power_body)

        for label in [
            "Board Profile",
            "Strip Profile",
            "Strip Length",
            "LED Density",
            "mm / Pixel",
            "Frame Interval",
            "Estimate Source",
            "mA / Pixel",
        ]:
            self.assertIn(label, diagnostics + web_ui)

    def test_report_documents_density_math_and_physical_testing_boundary(self):
        self.assertTrue(REPORT.exists(), "Hardware profile and density scaling report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Hardware profile and density-scaling migration",
            "5m x 160 LEDs/m = 800 pixels",
            "5m x 180 LEDs/m = 900 pixels",
            "5m x 60 LEDs/m = 300 pixels",
            "100 mm = 6 px at 60 LEDs/m",
            "100 mm = 16 px at 160 LEDs/m",
            "100 mm = 18 px at 180 LEDs/m",
            "Candle Hall",
            "Quiet Northern Lights",
            "Rain Glass",
            "not physically tested",
            "OTA upload not performed",
        ]:
            self.assertIn(required, report)

    def test_sketch_copy_is_mirrored_and_code_only(self):
        worktree_code = {
            path.name: path.read_text(encoding="utf-8")
            for path in sorted(SKETCH_DIR.glob("*.ino")) + sorted(SKETCH_DIR.glob("*.h"))
        }
        arduino_code = {
            path.name: path.read_text(encoding="utf-8")
            for path in sorted(ARDUINO_SKETCH_DIR.glob("*.ino")) + sorted(ARDUINO_SKETCH_DIR.glob("*.h"))
        }
        self.assertEqual(worktree_code, arduino_code)
        self.assertEqual([], list(ARDUINO_SKETCH_DIR.glob("*.md")))


if __name__ == "__main__":
    unittest.main()
