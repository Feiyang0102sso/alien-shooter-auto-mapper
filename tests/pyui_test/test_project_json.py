"""Tests for project JSON version migration and export rules."""

import unittest

from app.project.data import (
    PROJECT_VERSION_AS1,
    PROJECT_VERSION_AS2R,
    ProjectData,
    resolve_as1_ceiling_layer_counts,
)
from app.project.io import build_project_json_data, parse_project_data


class ProjectJsonTest(unittest.TestCase):
    """Verify project JSON compatibility and version-specific fields."""

    ceiling_config = {
        "min_layer_count": 1,
        "max_layer_count": 50,
        "default_standard_layer_count": 13,
        "default_lab_layer_count": 6,
    }

    def test_legacy_project_defaults_to_as1(self) -> None:
        """A legacy JSON object should gain the AS1 version in memory."""
        json_data = {}

        project_data = parse_project_data(json_data)

        self.assertEqual(project_data.version, PROJECT_VERSION_AS1)
        self.assertEqual(json_data["version"], PROJECT_VERSION_AS1)
        layer_counts = resolve_as1_ceiling_layer_counts(
            project_data,
            self.ceiling_config,
        )
        self.assertEqual(layer_counts, (13, 6))

    def test_unknown_project_version_is_rejected(self) -> None:
        """An explicit unknown version must not silently fall back to AS1."""
        with self.assertRaisesRegex(ValueError, "Unsupported project version"):
            parse_project_data({"version": "AS3"})

    def test_as1_export_places_version_first_and_keeps_door_option(self) -> None:
        """AS1 JSON should retain its supported global door state."""
        project_data = ProjectData(version=PROJECT_VERSION_AS1, is_door_open=True)

        json_data = build_project_json_data(project_data)

        self.assertEqual(next(iter(json_data)), "version")
        self.assertEqual(json_data["version"], PROJECT_VERSION_AS1)
        self.assertTrue(json_data["is_door_open"])

    def test_as1_ceiling_layers_round_trip(self) -> None:
        """AS1 should preserve both project-wide ceiling layer counts."""
        project_data = ProjectData(
            version=PROJECT_VERSION_AS1,
            as1_standard_ceiling_layer_count=17,
            as1_lab_ceiling_layer_count=8,
        )

        json_data = build_project_json_data(project_data)
        imported_project = parse_project_data(json_data)

        self.assertEqual(json_data["as1_standard_ceiling_layer_count"], 17)
        self.assertEqual(json_data["as1_lab_ceiling_layer_count"], 8)
        self.assertEqual(imported_project.as1_standard_ceiling_layer_count, 17)
        self.assertEqual(imported_project.as1_lab_ceiling_layer_count, 8)

    def test_invalid_as1_ceiling_layer_count_is_rejected(self) -> None:
        """Imported layer counts outside DLL limits should fail validation."""
        project_data = parse_project_data(
            {
                "version": PROJECT_VERSION_AS1,
                "as1_standard_ceiling_layer_count": 51,
            }
        )

        with self.assertRaisesRegex(ValueError, "between 1 and 50"):
            resolve_as1_ceiling_layer_counts(project_data, self.ceiling_config)

        with self.assertRaisesRegex(ValueError, "must be an integer"):
            parse_project_data(
                {
                    "version": PROJECT_VERSION_AS1,
                    "as1_lab_ceiling_layer_count": 2.5,
                }
            )

    def test_as2r_ignores_and_omits_global_door_option(self) -> None:
        """AS2R should neither load nor export the AS1-only option."""
        imported_project = parse_project_data(
            {
                "version": PROJECT_VERSION_AS2R,
                "is_door_open": True,
            }
        )
        exported_json = build_project_json_data(imported_project)

        self.assertFalse(imported_project.is_door_open)
        self.assertNotIn("is_door_open", exported_json)
        self.assertNotIn("as1_standard_ceiling_layer_count", exported_json)
        self.assertNotIn("as1_lab_ceiling_layer_count", exported_json)


if __name__ == "__main__":
    unittest.main()
