from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[1]
SKETCH_DIR = ROOT / "firmware" / "BedroomLedController"
DOCS_DIR = ROOT / "docs" / "generated"


class ReleaseHardeningContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.files = {
            path.name: path.read_text(encoding="utf-8")
            for path in SKETCH_DIR.glob("*")
            if path.suffix in {".ino", ".h"}
        }
        cls.sketch = "\n".join(cls.files.values())

    def test_tracked_source_does_not_contain_real_local_secrets(self):
        config = self.files["Config.h"]
        secrets_example = self.files.get("Secrets.example.h", "")
        gitignore = (ROOT / ".gitignore").read_text(encoding="utf-8")

        for pattern in [
            r'WIFI_SSID\s*=\s*"[^"]+"',
            r'WIFI_PASS\s*=\s*"[^"]+"',
            r'OTA_PASS\s*=\s*"[^"]+"',
        ]:
            self.assertIsNone(re.search(pattern, config))

        for required in [
            '#if __has_include("Secrets.h")',
            '#include "Secrets.h"',
            '#include "Secrets.example.h"',
            "BEDROOM_WIFI_SSID",
            "BEDROOM_WIFI_PASS",
            "BEDROOM_OTA_PASS",
        ]:
            self.assertIn(required, config)

        for required in [
            "#define BEDROOM_WIFI_SSID \"YOUR_WIFI_NAME\"",
            "#define BEDROOM_WIFI_PASS \"YOUR_WIFI_PASSWORD\"",
            "#define BEDROOM_OTA_PASS \"CHANGE_ME_OTA_PASSWORD\"",
        ]:
            self.assertIn(required, secrets_example)

        self.assertIn("firmware/BedroomLedController/Secrets.h", gitignore)

    def test_root_readme_indexes_docs_and_release_workflow(self):
        readme = (ROOT / "README.md").read_text(encoding="utf-8")
        for required in [
            "Bedroom LED Controller",
            "firmware/BedroomLedController",
            "docs/generated",
            "docs/user-phase-docs",
            "Secrets.example.h",
            "Secrets.h",
            "arduino-cli compile --fqbn esp8266:esp8266:d1_mini",
            "python -m unittest discover -s tests -v",
            "D1 mini",
            "visual output was not independently observed",
        ]:
            self.assertIn(required, readme)

    def test_runtime_heap_metrics_are_exposed_in_diagnostics(self):
        types = self.files["ControllerTypes.h"]
        ino = self.files["BedroomLedController.ino"]
        diagnostics = self.files["Diagnostics.h"]
        routes = self.files["WebRoutes.h"]
        management = self.files["ManagementControls.h"]

        for required in [
            "struct EndpointHeapMetric",
            "char route[40]",
            "uint32_t heapBeforeBytes",
            "uint32_t heapAfterBytes",
            "uint32_t heapMinimumBytes",
            "uint32_t payloadBytes",
            "char timeStatus[64]",
            "char id[MAX_FAVORITE_ID_LENGTH + 1]",
            "char label[MAX_FAVORITE_LABEL_LENGTH + 1]",
            "char type[12]",
            "char target[MAX_FAVORITE_TARGET_LENGTH + 1]",
        ]:
            self.assertIn(required, types)
        self.assertNotIn("String timeStatus", types)

        for required in [
            "EndpointHeapMetric endpointHeapMetrics[MAX_ENDPOINT_HEAP_METRICS]",
            "uint32_t minFreeHeapSeen",
        ]:
            self.assertIn(required, ino)

        for required in [
            "uint32_t beginEndpointHeapMetric",
            "void finishEndpointHeapMetric",
            "String buildEndpointHeapMetricsJson",
            '"freeHeapNow"',
            '"maxFreeBlockSize"',
            '"heapFragmentationPercent"',
            '"minFreeHeapSinceBoot"',
            '"endpointHeapMetrics"',
            '"heapBeforeBytes"',
            '"heapAfterBytes"',
            '"heapMinimumBytes"',
            '"heapDeltaBytes"',
            '"payloadBytes"',
        ]:
            self.assertIn(required, diagnostics)

        for route in [
            "/api/state",
            "/api/scenes",
            "/api/palettes",
            "/api/schedule",
            "/api/diagnostics",
        ]:
            self.assertIn(f'beginEndpointHeapMetric("{route}")', self.sketch)
            self.assertIn(f'finishEndpointHeapMetric("{route}"', self.sketch)

        self.assertIn("streamBackupExportJson", management)
        self.assertIn("server.setContentLength(CONTENT_LENGTH_UNKNOWN)", management)
        self.assertIn("server.sendContent", management)

    def test_release_hardening_report_records_limits_and_preserves_modes(self):
        report = (DOCS_DIR / "BedroomLedController_Release_Hardening_Report.md").read_text(encoding="utf-8")
        for required in [
            "Release Hardening",
            "Secrets removed from tracked source",
            "Runtime heap metrics",
            "Backup export streaming",
            "Fixed-size buffer migration",
            "Mode visual quality preservation",
            "No lighting modes were added",
            "No renderer math was downgraded",
            "not measured",
            "Compiled and uploaded successfully over USB",
        ]:
            self.assertIn(required, report)


if __name__ == "__main__":
    unittest.main()
