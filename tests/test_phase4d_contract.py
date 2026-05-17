import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WORKTREE_SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
ARDUINO_SKETCH_DIR = Path(r"C:\Users\kkids\Documents\Arduino\BedroomLedController")
GENERATED = ROOT / "docs" / "generated"
PHASE_REPORT = GENERATED / "BedroomLedController_Phase_4D_Report.md"
FINAL_REPORT = GENERATED / "BedroomLedController_Final_Report.md"
USER_GUIDE = GENERATED / "BedroomLedController_User_Guide.md"
API_REFERENCE = GENERATED / "BedroomLedController_API_Reference.md"
TROUBLESHOOTING = GENERATED / "BedroomLedController_Troubleshooting.md"
RESOURCE_REPORT = GENERATED / "BedroomLedController_Resource_And_Migration_Report.md"
PHASE_COMPLETION = GENERATED / "Phase_Completion.md"


class Phase4DFinalPolishContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in WORKTREE_SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }

    def test_advanced_scene_management_routes_and_helpers_exist(self):
        routes = self.files["WebRoutes.h"]
        forwards = self.files["ForwardDeclarations.h"]
        scenes = self.files["ScenePresets.h"]

        for route, handler in [
            ('"/api/scenes/duplicate"', "handleApiScenesDuplicate"),
            ('"/api/scenes/export"', "handleApiScenesExport"),
            ('"/api/scenes/import"', "handleApiScenesImport"),
            ('"/api/scenes/reset-builtins"', "handleApiScenesResetBuiltins"),
        ]:
            self.assertIn(route, routes)
            self.assertIn(handler, routes)
            self.assertIn(f"void {handler}();", forwards)

        for required in [
            "duplicateSceneById",
            "buildSceneExportJson",
            "importSceneFromJson",
            "restoreBuiltInScenes",
            "buildSceneWarningsJson",
            "missingMode",
            "missingPalette",
            "nightGuardBlocked",
            "powerBrightness",
            '"sceneSchema":1',
            '"warnings":',
        ]:
            self.assertIn(required, scenes)

    def test_palette_schedule_backup_reset_and_api_docs_routes_exist(self):
        routes = self.files["WebRoutes.h"]
        forwards = self.files["ForwardDeclarations.h"]
        management = self.files.get("ManagementControls.h", "")
        palettes = self.files["PaletteControls.h"]
        schedule = self.files["ScheduleControls.h"]
        sketch = self.files["BedroomLedController.ino"]

        self.assertIn('#include "ManagementControls.h"', sketch)
        self.assertTrue(management, "ManagementControls.h should exist")

        for route, handler in [
            ('"/api/palettes/export"', "handleApiPalettesExport"),
            ('"/api/palettes/import"', "handleApiPalettesImport"),
            ('"/api/palettes/reset-builtins"', "handleApiPalettesResetBuiltins"),
            ('"/api/schedule/export"', "handleApiScheduleExport"),
            ('"/api/schedule/import"', "handleApiScheduleImport"),
            ('"/api/schedule/reset"', "handleApiScheduleReset"),
            ('"/api/backup/export"', "handleApiBackupExport"),
            ('"/api/backup/import"', "handleApiBackupImport"),
            ('"/api/reset"', "handleApiReset"),
            ('"/api-docs"', "handleApiDocsPage"),
        ]:
            self.assertIn(route, routes)
            self.assertIn(handler, routes)
            self.assertIn(f"void {handler}();", forwards)

        for required in [
            "buildPaletteExportJson",
            "importPaletteFromJson",
            "resetBuiltInPalettes",
        ]:
            self.assertIn(required, palettes)

        for required in [
            "buildScheduleExportJson",
            "importScheduleFromJson",
            "resetSchedules",
        ]:
            self.assertIn(required, schedule)

        for required in [
            "buildBackupExportJson",
            "handleApiBackupExport",
            "handleApiBackupImport",
            "confirm=YES",
            "target=scenes-builtins",
            "target=factory",
            "BedroomLedController API Reference",
        ]:
            self.assertIn(required, management)

    def test_ui_navigation_management_and_diagnostics_are_polished(self):
        web_ui = self.files["WebUi.h"]
        diagnostics = self.files["Diagnostics.h"]

        for required in [
            "/api-docs",
            "/diagnostics",
            "/schedule",
            "/ota",
            "Duplicate",
            "Export",
            "Import Scene JSON",
            "Restore Built-In Scenes",
            "Backup / Maintenance",
            "Export Full Backup",
            "Import Full Backup",
            "Factory Reset",
            "data-scene-duplicate",
            "data-scene-export",
            "sceneImportJson",
            "backupExportJson",
        ]:
            self.assertIn(required, web_ui)

        for required in [
            '"projectVersion"',
            '"boardTarget"',
            '"littleFsTotalBytes"',
            '"littleFsUsedBytes"',
            '"littleFsFreeBytes"',
            '"sketchSizeBytes"',
            '"freeSketchSpaceBytes"',
            '"otaRoutePresent"',
            '"browserUpdateRoutePresent"',
        ]:
            self.assertIn(required, diagnostics)

    def test_final_docs_and_phase_completion_are_updated(self):
        for path in [
            PHASE_REPORT,
            FINAL_REPORT,
            USER_GUIDE,
            API_REFERENCE,
            TROUBLESHOOTING,
            RESOURCE_REPORT,
            PHASE_COMPLETION,
        ]:
            self.assertTrue(path.exists(), f"{path.name} is missing")

        phase_report = PHASE_REPORT.read_text(encoding="utf-8")
        for required in [
            "Phase 4D",
            "Implemented feature inventory",
            "Route map",
            "Persistent file audit",
            "Advanced scene management",
            "Palette/favorite/schedule polish",
            "Full backup export",
            "Full backup import",
            "Selective resets",
            "API docs/help",
            "Diagnostics hardening",
            "Final resource report",
            "Compiled successfully for the D1 mini / ESP8266 target, but not physically tested on LEDs.",
        ]:
            self.assertIn(required, phase_report)

        for path, required in [
            (FINAL_REPORT, "D1 mini vs ESP32-S3 final recommendation"),
            (USER_GUIDE, "Backup and restore"),
            (API_REFERENCE, "/api/scenes/duplicate"),
            (TROUBLESHOOTING, "D3/GPIO0 boot pin warning"),
            (RESOURCE_REPORT, "Final resource report"),
        ]:
            content = path.read_text(encoding="utf-8")
            self.assertIn(required, content)
            self.assertIn("not measured", content)

        completion = PHASE_COMPLETION.read_text(encoding="utf-8")
        for required in [
            "Phase 4D",
            "advanced scene management",
            "full backup",
            "API reference",
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
