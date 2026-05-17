from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_2B_Report.md"
PHASE_COMPLETION = PROJECT_ROOT / "docs" / "generated" / "Phase_Completion.md"


class Phase2BFavoritesPanicWarmNightGuardContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {path.name: path.read_text(encoding="utf-8") for path in cls.sketch_files}
        cls.sketch = "\n".join(cls.files.values())

    def test_favorite_model_storage_and_defaults_exist(self):
        for required in [
            "ComfortControls.h",
            "struct FavoriteItem",
            "FavoriteItem favorites[MAX_FAVORITES]",
            "uint8_t favoriteCount",
            'const char* FAVORITES_PATH = "/favorites.json"',
            "const uint8_t MAX_FAVORITES = 8",
            "String favoriteLoadStatus",
            "String favoriteSaveStatus",
            "loadFavorites();",
            "loadDefaultFavorites()",
            "saveFavorites()",
            "buildFavoritesJson()",
            "addDefaultSceneFavorite",
            '"fav-off"',
            '"fav-warm-dim-now"',
            '"reading"',
            '"sleep"',
            '"movie"',
            '"focus"',
            '"rain-glass"',
            '"moon-tide"',
        ]:
            self.assertIn(required, self.sketch)

    def test_favorite_api_routes_and_loader_exist(self):
        for required in [
            'server.on("/api/favorites", HTTP_GET, handleApiFavorites)',
            'server.on("/api/favorites/load", HTTP_GET, handleApiFavoritesLoad)',
            'server.on("/api/favorites/reset", HTTP_GET, handleApiFavoritesReset)',
            "void handleApiFavorites()",
            "void handleApiFavoritesLoad()",
            "void handleApiFavoritesReset()",
            "loadFavoriteById",
            "Favorite id is required",
            "Favorite not found",
            "Favorite target scene not found",
            "Favorite loaded",
            "Favorites reset",
            '"available":',
            '"maxFavorites":',
            '"favorites":[',
        ]:
            self.assertIn(required, self.sketch)

    def test_central_actions_panic_warm_and_off_exist(self):
        comfort = self.files.get("ComfortControls.h", "")
        for required in [
            "bool runActionByName(const String& actionName)",
            "bool runActionByName(const String& actionName, String& error)",
            "bool runWarmDimNow()",
            "bool runOffAction()",
            "diagnosticTest.active = false",
            "setModeByName(\"solid\")",
            "setWhiteTemperature(2700)",
            "setMasterBrightness(WARM_DIM_BRIGHTNESS)",
            "setActiveColor(0, 0, 0)",
            '"warmDimNow"',
            '"panicWarm"',
            '"off"',
            "lastActionStatus",
        ]:
            self.assertIn(required, comfort)
        for required in [
            'server.on("/api/action", HTTP_GET, handleApiAction)',
            'server.on("/api/panic-warm", HTTP_GET, handleApiPanicWarm)',
            "void handleApiAction()",
            "void handleApiPanicWarm()",
            "Action name is required",
            "Action completed",
            "Warm Dim Now",
        ]:
            self.assertIn(required, self.sketch)

    def test_night_guard_settings_persistence_and_effective_brightness_exist(self):
        types = self.files["ControllerTypes.h"]
        settings = self.files["SettingsStorage.h"]
        rendering = self.files["LedRendering.h"]
        for required in [
            "bool nightGuardEnabled",
            "uint8_t nightGuardMaxBrightness",
            "bool nightGuardBlockFlashingModes",
            "bool nightGuardForceWarmOnPanic",
            "bool nightGuardPreferWarmTemperature",
        ]:
            self.assertIn(required, types)
        for required in [
            "settings.nightGuardEnabled = false",
            "settings.nightGuardMaxBrightness = NIGHT_GUARD_DEFAULT_MAX_BRIGHTNESS",
            "settings.nightGuardBlockFlashingModes = true",
            "settings.nightGuardForceWarmOnPanic = true",
            "settings.nightGuardPreferWarmTemperature = false",
            'readJsonBool(json, "nightGuardEnabled"',
            'readJsonInt(json, "nightGuardMaxBrightness"',
            'readJsonBool(json, "nightGuardBlockFlashingModes"',
            'readJsonBool(json, "nightGuardPreferWarmTemperature"',
            'json += F(",\\"nightGuardEnabled\\":")',
            'json += F(",\\"nightGuardMaxBrightness\\":")',
            'json += F(",\\"nightGuardBlockFlashingModes\\":")',
        ]:
            self.assertIn(required, settings)
        for required in [
            "uint8_t getEffectiveBrightness()",
            "return min(settings.masterBrightness, settings.nightGuardMaxBrightness)",
            "getEffectiveBrightness()",
        ]:
            self.assertIn(required, self.sketch)
        self.assertIn("getEffectiveBrightness()", rendering)
        self.assertNotIn("settings.masterBrightness) / 255U", rendering)

    def test_night_guard_api_state_diagnostics_and_blocking_exist(self):
        for required in [
            'server.on("/api/nightguard", HTTP_GET, handleApiNightGuard)',
            'server.on("/api/nightguard/set", HTTP_GET, handleApiNightGuardSet)',
            "void handleApiNightGuard()",
            "void handleApiNightGuardSet()",
            "setNightGuardEnabled",
            "setNightGuardMaxBrightness",
            "setNightGuardBlockFlashingModes",
            "setNightGuardPreferWarmTemperature",
            "Invalid Night Guard brightness",
            "Night Guard updated",
            "isModeBlockedByNightGuard",
            "Night Guard blocks",
            "MODE_STROBE",
            "MODE_FLASH",
            "loadSceneById",
        ]:
            self.assertIn(required, self.sketch)
        for required in [
            '"nightGuardEnabled":',
            '"nightGuardMaxBrightness":',
            '"nightGuardBlockFlashingModes":',
            '"nightGuardPreferWarmTemperature":',
            '"effectiveBrightness":',
            '"nightGuardBrightnessCapped":',
            '"favoritesCount":',
        ]:
            self.assertIn(required, self.sketch)

    def test_main_ui_has_favorite_panic_warm_and_night_guard_controls(self):
        web_ui = self.files["WebUi.h"]
        for required in [
            "<h2>Comfort</h2>",
            'id="favoriteBar"',
            'id="favoriteMessage"',
            'id="panicWarm"',
            "Warm Dim Now",
            "apiFetchJson('/api/favorites'",
            "apiFetchJson(`/api/favorites/load?",
            "apiFetchJson('/api/favorites/reset'",
            "apiFetchJson(`/api/action?",
            "renderFavorites",
            "refreshFavorites",
            "<h2>Night Guard</h2>",
            'id="nightGuardEnabled"',
            'id="nightGuardMaxBrightness"',
            'id="nightGuardBlockFlashing"',
            'id="nightGuardPreferWarm"',
            'id="nightGuardNote"',
            "apiFetchJson(`/api/nightguard/set?",
            "updateNightGuardControls",
            "Night Guard is limiting output",
        ]:
            self.assertIn(required, web_ui)

    def test_generated_report_and_phase_completion_are_updated(self):
        self.assertTrue(REPORT.exists(), "Phase 2B report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 2B",
            "Favorite bar behavior",
            "/favorites.json",
            "/api/favorites",
            "/api/favorites/load",
            "/api/favorites/reset",
            "Warm Dim Now",
            "/api/action",
            "/api/panic-warm",
            "Off action",
            "Night Guard settings",
            "effectiveBrightness",
            "Night Guard harsh-mode blocking",
            "State/diagnostics additions",
            "Persistence changes",
            "Interaction with scenes",
            "not physically tested on LEDs",
            "Phase 2C",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 2B",
            "favorite bar",
            "Warm Dim Now",
            "Off action",
            "Night Guard",
            "/favorites.json",
            "/api/nightguard",
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
