from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_2D_Report.md"
PHASE_COMPLETION = PROJECT_ROOT / "docs" / "generated" / "Phase_Completion.md"


class Phase2DModeTagsPracticalScenesUsabilityContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {path.name: path.read_text(encoding="utf-8") for path in cls.sketch_files}
        cls.sketch = "\n".join(cls.files.values())

    def test_mode_metadata_model_covers_all_modes(self):
        self.assertIn("ModeMetadata.h", self.files)
        metadata = self.files["ModeMetadata.h"]
        for required in [
            "struct ModeMetadata",
            "Mode mode",
            "const char* id",
            "const char* displayName",
            "const char* description",
            "const char* bestUsedFor",
            "const char* category",
            "bool animated",
            "bool calm",
            "bool sleepSafe",
            "bool focusFriendly",
            "bool colorful",
            "bool flashing",
            "bool utility",
            "bool experimental",
            "bool blockedByNightGuard",
            "MODE_METADATA[]",
            "MODE_METADATA_COUNT",
        ]:
            self.assertIn(required, metadata)
        for mode_id in [
            "solid",
            "strobe",
            "flash",
            "chase",
            "wave",
            "slowWave",
            "slowPulse",
            "rainbow",
            "dawnBloom",
            "moonTide",
            "velvetAurora",
            "lanternDrift",
            "rainGlass",
        ]:
            self.assertIn(f'"{mode_id}"', metadata)
        for display_name in [
            "Solid",
            "Strobe",
            "Flash",
            "Chase",
            "Wave",
            "Slow Wave",
            "Slow Pulse",
            "Rainbow",
            "Dawn Bloom",
            "Moon Tide",
            "Velvet Aurora",
            "Lantern Drift",
            "Rain Glass",
        ]:
            self.assertIn(f'"{display_name}"', metadata)

    def test_modes_api_and_metadata_helpers_exist(self):
        for required in [
            'server.on("/api/modes", HTTP_GET, handleApiModes)',
            "void handleApiModes()",
            "String buildModesJson()",
            "String buildModeMetadataJson",
            "String buildModeTagsJson",
            "String currentModeTagsJson()",
            "const ModeMetadata* findModeMetadataById",
            "const ModeMetadata* findModeMetadata",
            "bool modeIsFlashing",
            "bool modeBlockedByNightGuard",
            '"modes":[',
            '"tags":',
            '"bestUsedFor":"',
            '"blockedByNightGuard":',
        ]:
            self.assertIn(required, self.sketch)

    def test_night_guard_blocking_uses_metadata(self):
        comfort = self.files["ComfortControls.h"]
        self.assertIn("return modeIsFlashing(modeName);", comfort)
        self.assertIn("modeBlockedByNightGuard(modeName)", comfort)
        self.assertNotIn("return parsedMode == MODE_STROBE || parsedMode == MODE_FLASH;", comfort)
        self.assertIn("metadata->displayName", comfort)

    def test_practical_scene_metadata_and_defaults_exist(self):
        types = self.files["ControllerTypes.h"]
        for required in [
            "String category",
            "bool isFavoriteCandidate",
            "bool isSleepSafe",
            "bool isFocusFriendly",
        ]:
            self.assertIn(required, types)

        scenes = self.files["ScenePresets.h"]
        for required in [
            '"Study / Focus"',
            '"Workbench Mode"',
            '"Sera Call"',
            '"Cozy Goblin Mode"',
            '"Movie"',
            '"category":"',
            '"isFavoriteCandidate":',
            '"isSleepSafe":',
            '"isFocusFriendly":',
            'readJsonString(objectJson, "category"',
            'readJsonBool(objectJson, "isFavoriteCandidate"',
            'readJsonBool(objectJson, "isSleepSafe"',
            'readJsonBool(objectJson, "isFocusFriendly"',
            "rememberLoadedScene(scenes[index])",
        ]:
            self.assertIn(required, scenes)

    def test_favorite_defaults_are_polished_for_phase_2d(self):
        comfort = self.files["ComfortControls.h"]
        for required in [
            '"fav-off"',
            '"fav-warm-dim-now"',
            '"cozy-goblin-mode"',
            '"sleep"',
            '"study-focus"',
            '"movie"',
            '"rain-glass"',
            '"workbench-mode"',
        ]:
            self.assertIn(required, comfort)

    def test_state_and_diagnostics_include_mode_and_scene_summary(self):
        for required in [
            '"modeDisplayName":"',
            '"modeCategory":"',
            '"modeTags":',
            '"lastLoadedSceneId":"',
            '"lastLoadedSceneName":"',
            '"currentStateMatchesLoadedScene":',
            '"nightGuardEffectiveBrightness":',
        ]:
            self.assertIn(required, self.sketch)
        for required in [
            "String lastLoadedSceneId",
            "String lastLoadedSceneName",
            "bool currentStateMatchesLoadedScene",
            "void rememberLoadedScene",
            "void clearLoadedSceneTracking",
        ]:
            self.assertIn(required, self.sketch)
        diagnostics = self.files["Diagnostics.h"]
        for required in [
            "Mode Category",
            "Mode Tags",
            "Last Scene",
            "modeDisplayName",
            "modeCategory",
            "modeTags",
            "lastLoadedSceneName",
        ]:
            self.assertIn(required, diagnostics)

    def test_main_ui_has_summary_mode_filters_and_clear_sections(self):
        web_ui = self.files["WebUi.h"]
        for required in [
            "<h2>Quick Actions</h2>",
            "<h2>Current Light</h2>",
            "Color and White Temperature",
            "<h2>Mode</h2>",
            "<h2>Scenes</h2>",
            "Timers / Bedtime",
            "<h2>Night Guard</h2>",
            "Advanced / Diagnostics",
            'id="currentSummary"',
            'id="modeFilterBar"',
            'id="modeDescription"',
            'id="modeTags"',
            "renderCurrentSummary",
            "refreshModes",
            "renderModeFilters",
            "applyModeFilter",
            "renderModeOptions",
            "renderSelectedModeMetadata",
            "modeMatchesFilter",
            "Sleep-safe",
            "Focus",
            "Colorful",
            "Flashing",
            "Utility",
            "Night Guard blocks this mode",
            "lastLoadedScene",
        ]:
            self.assertIn(required, web_ui)

    def test_generated_report_and_phase_completion_are_updated(self):
        self.assertTrue(REPORT.exists(), "Phase 2D report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 2D",
            "Mode metadata",
            "/api/modes",
            "Mode filter UI",
            "Night Guard metadata",
            "Practical scenes",
            "Study / Focus",
            "Workbench Mode",
            "Sera Call",
            "Cozy Goblin Mode",
            "Current Light summary",
            "lastLoadedSceneId",
            "not physically tested on LEDs",
            "Phase 3A",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 2D",
            "mode metadata",
            "/api/modes",
            "practical scenes",
            "mode filter",
            "current state summary",
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
