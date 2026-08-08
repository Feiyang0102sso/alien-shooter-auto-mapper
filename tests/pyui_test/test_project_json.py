"""Tests for project JSON version migration and export rules."""

import unittest

from app.project.data import PROJECT_VERSION_AS1, PROJECT_VERSION_AS2R, ProjectData
from app.project.io import build_project_json_data, parse_project_data


class ProjectJsonTest(unittest.TestCase):
    """Verify project JSON compatibility and version-specific fields."""

    def test_legacy_project_defaults_to_as1(self) -> None:
        """A legacy JSON object should gain the AS1 version in memory."""
        json_data = {}

        project_data = parse_project_data(json_data)

        self.assertEqual(project_data.version, PROJECT_VERSION_AS1)
        self.assertEqual(json_data["version"], PROJECT_VERSION_AS1)

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


if __name__ == "__main__":
    unittest.main()
