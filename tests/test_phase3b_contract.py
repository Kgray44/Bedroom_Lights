from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_3B_Report.md"
PHASE_COMPLETION = PROJECT_ROOT / "docs" / "generated" / "Phase_Completion.md"


class Phase3BPaletteBuilderContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {path.name: path.read_text(encoding="utf-8") for path in cls.sketch_files}
        cls.sketch = "\n".join(cls.files.values())

    def test_palette_models_and_settings_exist(self):
        types = self.files["ControllerTypes.h"]
        for required in [
            "struct PaletteColor",
            "uint8_t r",
            "uint8_t g",
            "uint8_t b",
            "uint8_t weight",
            "struct ColorPalette",
            "String id",
            "String name",
            "uint8_t colorCount",
            "PaletteColor colors[MAX_PALETTE_COLORS]",
            "bool builtin",
            "String category",
            "bool calm",
            "bool sleepSafe",
            "bool colorful",
            "bool warm",
            "bool cool",
            "bool paletteEnabled",
            "String activePaletteId",
        ]:
            self.assertIn(required, types)

        for required in [
            "bool paletteEnabled",
            "String paletteId",
        ]:
            self.assertIn(required, types)

    def test_palette_config_globals_and_setup_are_wired(self):
        config = self.files["Config.h"]
        for required in [
            'const char* PALETTES_PATH = "/palettes.json"',
            "const uint8_t MAX_PALETTES = 16",
            "const uint8_t MAX_PALETTE_COLORS = 5",
            "const uint8_t MIN_PALETTE_COLORS = 2",
            "MAX_PALETTE_NAME_LENGTH",
            "MAX_PALETTE_ID_LENGTH",
        ]:
            self.assertIn(required, config)

        ino = self.files["BedroomLedController.ino"]
        for required in [
            "ColorPalette palettes[MAX_PALETTES]",
            "uint8_t paletteCount",
            "String paletteLoadStatus",
            "String paletteSaveStatus",
            '#include "PaletteControls.h"',
            "loadPalettes();",
        ]:
            self.assertIn(required, ino)

    def test_palette_controls_file_has_helpers_builtins_and_storage(self):
        self.assertIn("PaletteControls.h", self.files)
        palette = self.files["PaletteControls.h"]
        for required in [
            "void loadDefaultPalettes()",
            "void loadPalettes()",
            "bool savePalettes()",
            "String buildPalettesJson()",
            "int findPaletteById",
            "ColorPalette* getActivePalette()",
            "RgbPixel getPaletteColor(uint8_t index)",
            "RgbPixel samplePalette(uint16_t position, uint16_t maxPosition)",
            "RgbPixel blendPaletteColors(RgbPixel a, RgbPixel b, uint8_t amount)",
            "RgbPixel getWeightedPaletteColor(uint8_t selector)",
            "bool parsePaletteColor",
            "String paletteColorHex",
            "String safePaletteIdToken",
            "bool addBuiltInPalette",
            "bool addPaletteFromRequest",
            "bool updatePaletteFromRequest",
            "bool deletePaletteById",
            "handleApiPalettes",
            "handleApiPalettesSelect",
            "handleApiPalettesSave",
            "handleApiPalettesUpdate",
            "handleApiPalettesDelete",
            "handleApiPalettesReset",
            "PALETTES_PATH",
            "palette load/save status",
        ]:
            self.assertIn(required, palette)

        for builtin in [
            "Candle",
            "Ocean",
            "Garden",
            "Moonlight",
            "Peach Sky",
            "Soft Neon",
            "Aurora",
            "Warm Linen",
            "Storm Blue",
            "Circuit Glow",
            "warm-linen",
        ]:
            self.assertIn(builtin, palette)

    def test_palette_api_routes_and_state_exist(self):
        for required in [
            'server.on("/api/palettes", HTTP_GET, handleApiPalettes)',
            'server.on("/api/palettes/select", HTTP_GET, handleApiPalettesSelect)',
            'server.on("/api/palettes/save", HTTP_GET, handleApiPalettesSave)',
            'server.on("/api/palettes/update", HTTP_GET, handleApiPalettesUpdate)',
            'server.on("/api/palettes/delete", HTTP_GET, handleApiPalettesDelete)',
            'server.on("/api/palettes/reset", HTTP_GET, handleApiPalettesReset)',
            '"paletteEnabled":',
            '"activePaletteId":"',
            '"activePaletteName":"',
            '"currentModeSupportsPalette":',
            '"maxPalettes":',
            '"palettes":[',
            "Unknown palette",
            "At least two valid colors are required",
            "Built-in palettes cannot be deleted",
        ]:
            self.assertIn(required, self.sketch)

    def test_mode_metadata_marks_palette_compatibility(self):
        metadata = self.files["ModeMetadata.h"]
        for required in [
            "bool supportsPalette",
            "const char* preferredPaletteRole",
            "bool modeSupportsPalette(Mode mode)",
            "bool currentModeSupportsPalette()",
            '"supportsPalette":',
            '"preferredPaletteRole":"',
            "velvetAurora",
            "moonTide",
            "lanternDrift",
            "rainGlass",
            "slowWave",
            "slowPulse",
        ]:
            self.assertIn(required, metadata)

    def test_palette_rendering_is_integrated_into_existing_modes(self):
        rendering = self.files["LedRendering.h"]
        for required in [
            "renderVelvetAurora",
            "samplePalette",
            "getWeightedPaletteColor",
            "settings.paletteEnabled",
            "currentModeSupportsPalette",
        ]:
            self.assertIn(required, rendering)

    def test_scene_palette_references_are_saved_loaded_and_applied(self):
        scenes = self.files["ScenePresets.h"]
        for required in [
            "scene.paletteEnabled = settings.paletteEnabled",
            "scene.paletteId = settings.activePaletteId",
            'json += F(",\\"paletteEnabled\\":")',
            'json += F(",\\"paletteId\\":\\"")',
            'readJsonBool(objectJson, "paletteEnabled"',
            'readJsonString(objectJson, "paletteId"',
            "applyScenePaletteReference",
            "findPaletteById(scene.paletteId)",
        ]:
            self.assertIn(required, scenes)

    def test_palette_ui_and_diagnostics_exist(self):
        web_ui = self.files["WebUi.h"]
        for required in [
            "<h2>Palettes</h2>",
            'id="paletteEnabled"',
            'id="activePaletteId"',
            'id="activePaletteSwatches"',
            'id="paletteSupportNote"',
            'id="paletteList"',
            'id="paletteMessage"',
            'id="paletteName"',
            'id="paletteColor1"',
            'id="paletteColor5"',
            "refreshPalettes",
            "renderPalettes",
            "savePaletteFromForm",
            "editPalette",
            "deletePalette",
            "resetPalettes",
            "apiFetchJson('/api/palettes'",
        ]:
            self.assertIn(required, web_ui)

        diagnostics = self.files["Diagnostics.h"]
        for required in [
            "Palettes",
            "paletteEnabled",
            "activePaletteName",
            "paletteLoadStatus",
            "paletteSaveStatus",
            "paletteCount",
            "currentModeSupportsPalette",
            "maxPalettes",
        ]:
            self.assertIn(required, diagnostics)

    def test_palette_settings_persist_and_docs_are_updated(self):
        settings = self.files["SettingsStorage.h"]
        for required in [
            "settings.paletteEnabled = false",
            'settings.activePaletteId = "warm-linen"',
            'readJsonBool(json, "paletteEnabled"',
            'readJsonString(json, "activePaletteId"',
            'json += F(",\\"paletteEnabled\\":")',
            'json += F(",\\"activePaletteId\\":\\"")',
            "{\"version\":7",
        ]:
            self.assertIn(required, settings)

        self.assertTrue(REPORT.exists(), "Phase 3B report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 3B",
            "Palette data model",
            "Palette storage",
            "/palettes.json",
            "Built-in palettes",
            "Palette API endpoints",
            "Palette UI behavior",
            "Palette-aware mode metadata",
            "Velvet Aurora",
            "Scene palette reference behavior",
            "not physically tested on LEDs",
            "Phase 3C",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 3B",
            "palette",
            "/api/palettes",
            "/palettes.json",
            "Velvet Aurora",
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
