from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = PROJECT_ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
REPORT = PROJECT_ROOT / "docs" / "generated" / "BedroomLedController_Phase_2A_Report.md"
PHASE_COMPLETION = PROJECT_ROOT / "docs" / "generated" / "Phase_Completion.md"


class Phase2AScenePresetContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sketch_files = sorted(WORKTREE_SKETCH_DIR.glob("*.ino")) + sorted(WORKTREE_SKETCH_DIR.glob("*.h"))
        cls.files = {path.name: path.read_text(encoding="utf-8") for path in cls.sketch_files}
        cls.sketch = "\n".join(cls.files.values())

    def test_scene_model_and_storage_are_bounded_and_separate(self):
        for required in [
            "ScenePresets.h",
            "struct ScenePreset",
            "ScenePreset scenes[MAX_SCENES]",
            "uint8_t sceneCount",
            'const char* SCENES_PATH = "/scenes.json"',
            "const uint8_t MAX_SCENES = 16",
            "MAX_SCENE_NAME_LENGTH",
            "String sceneLoadStatus",
            "String sceneSaveStatus",
            "settings.supplyMillivolts",
        ]:
            self.assertIn(required, self.sketch)
        scene_types = self.files["ControllerTypes.h"]
        for required in [
            "String id",
            "String name",
            "String mode",
            "uint8_t red",
            "uint8_t green",
            "uint8_t blue",
            "uint16_t whiteKelvin",
            "bool usingWhiteTemperature",
            "uint8_t masterBrightness",
            "float strobeDelaySec",
            "float flashOffDelaySec",
            "uint8_t chasePeriodSec",
            "uint8_t wavePeriodSec",
            "uint8_t slowPulseCount",
            "uint8_t slowPulseMaxSec",
            "uint8_t rainbowPeriodSec",
            "bool builtin",
        ]:
            self.assertIn(required, scene_types)

    def test_scene_storage_helpers_and_default_scenes_exist(self):
        scenes = self.files.get("ScenePresets.h", "")
        for required in [
            "loadScenes()",
            "saveScenes()",
            "loadDefaultScenes()",
            "addBuiltInScene",
            "captureCurrentScene",
            "applyScene",
            "saveCurrentStateAsScene",
            "loadSceneById",
            "renameScene",
            "deleteScene",
            "findSceneById",
            "sceneNameExists",
            "makeSceneId",
            "buildScenesJson",
            '"Warm Dim"',
            '"Reading"',
            '"Sleep"',
            '"Movie"',
            '"Focus"',
            '"Rain Glass"',
            '"Moon Tide"',
            '"Lantern Drift"',
        ]:
            self.assertIn(required, scenes)
        self.assertIn("loadScenes();", self.files["BedroomLedController.ino"])

    def test_scene_api_routes_and_handlers_exist(self):
        for required in [
            'server.on("/api/scenes", HTTP_GET, handleApiScenes)',
            'server.on("/api/scenes/save", HTTP_GET, handleApiScenesSave)',
            'server.on("/api/scenes/load", HTTP_GET, handleApiScenesLoad)',
            'server.on("/api/scenes/rename", HTTP_GET, handleApiScenesRename)',
            'server.on("/api/scenes/delete", HTTP_GET, handleApiScenesDelete)',
            "void handleApiScenes()",
            "void handleApiScenesSave()",
            "void handleApiScenesLoad()",
            "void handleApiScenesRename()",
            "void handleApiScenesDelete()",
            "Scene limit reached",
            "Scene not found",
            "Scene name is required",
            "Duplicate scene name",
            "Scene saved",
            "Scene loaded",
            "Scene renamed",
            "Scene deleted",
            '"ok":true',
            '"ok":false',
            "application/json",
        ]:
            self.assertIn(required, self.sketch)

    def test_scene_recall_uses_central_state_and_preserves_global_config(self):
        scenes = self.files.get("ScenePresets.h", "")
        for required in [
            "setModeByName(scene.mode)",
            "setWhiteTemperature(scene.whiteKelvin)",
            "setActiveColor(scene.red, scene.green, scene.blue)",
            "setMasterBrightness(scene.masterBrightness)",
            "setFloatSetting(settings.strobeDelaySec",
            "setByteSetting(settings.chasePeriodSec",
            "markSettingsDirty()",
        ]:
            self.assertIn(required, scenes)
        forbidden_scene_fields = [
            "supplyMillivolts;",
            "supplyMilliamps;",
            "supplySafetyMarginPercent;",
            "controllerOverheadMilliamps;",
        ]
        scene_struct_start = self.files["ControllerTypes.h"].index("struct ScenePreset")
        scene_struct = self.files["ControllerTypes.h"][scene_struct_start:self.files["ControllerTypes.h"].index("};", scene_struct_start)]
        for forbidden in forbidden_scene_fields:
            self.assertNotIn(forbidden, scene_struct)

    def test_main_ui_has_scene_management(self):
        web_ui = self.files["WebUi.h"]
        for required in [
            "<h2>Scenes</h2>",
            'id="sceneName"',
            'id="saveScene"',
            'id="sceneList"',
            'id="sceneMessage"',
            "fetch('/api/scenes'",
            "fetch(`/api/scenes/save?",
            "fetch(`/api/scenes/load?",
            "fetch(`/api/scenes/rename?",
            "fetch(`/api/scenes/delete?",
            "renderScenes",
            "refreshScenes",
            "data-scene-load",
            "data-scene-rename",
            "data-scene-delete",
        ]:
            self.assertIn(required, web_ui)

    def test_diagnostics_include_scene_status(self):
        diagnostics = self.files.get("Diagnostics.h", "")
        for required in [
            '"sceneCount":',
            '"maxScenes":',
            '"sceneLoadStatus":"',
            '"sceneSaveStatus":"',
        ]:
            self.assertIn(required, diagnostics)

    def test_generated_report_and_phase_completion_are_updated(self):
        self.assertTrue(REPORT.exists(), "Phase 2A report is missing")
        report = REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 2A",
            "Scene data model",
            "/scenes.json",
            "Built-in default scenes",
            "/api/scenes",
            "/api/scenes/save",
            "/api/scenes/load",
            "/api/scenes/rename",
            "/api/scenes/delete",
            "Settings-vs-scenes separation",
            "Gamma/RGB calibration",
            "not physically tested on LEDs",
            "Phase 2B",
        ]:
            self.assertIn(required, report)

        self.assertTrue(PHASE_COMPLETION.exists(), "Phase_Completion document is missing")
        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 2A",
            "scene presets",
            "save current state",
            "recall",
            "rename",
            "delete",
            "/scenes.json",
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
