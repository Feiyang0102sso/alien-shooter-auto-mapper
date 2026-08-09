"""Tests for Python project-version mapping to the C API."""

import unittest
from pathlib import Path
from unittest.mock import Mock

from app.binding.client import (
    C_MAP_FORMAT_AS1,
    C_MAP_FORMAT_AS2,
    C_MAP_FORMAT_AS2R,
    AutoMapperLibClient,
)
from app.project.data import (
    PROJECT_VERSION_AS1,
    PROJECT_VERSION_AS2,
    PROJECT_VERSION_AS2R,
    ProjectData,
)


class MapFormatBindingTest(unittest.TestCase):
    """Verify stable map-format values passed through ctypes."""

    def test_client_maps_each_known_format_to_stable_c_value(self) -> None:
        """The Python binding should pass stable public C API values."""
        client = AutoMapperLibClient()

        self.assertEqual(client._get_map_format(PROJECT_VERSION_AS1), C_MAP_FORMAT_AS1)
        self.assertEqual(client._get_map_format(PROJECT_VERSION_AS2), C_MAP_FORMAT_AS2)
        self.assertEqual(client._get_map_format(PROJECT_VERSION_AS2R), C_MAP_FORMAT_AS2R)

    def test_client_passes_random_direction_to_c_api(self) -> None:
        """The client should preserve the UI direction-randomization value."""
        client = AutoMapperLibClient()
        client.lib = Mock()
        client.lib.generate_map_from_segments.return_value = True

        success = client.generate_map(
            Path("random_direction_test.map"),
            ProjectData(),
            random_direction=True,
        )

        self.assertTrue(success)
        call_arguments = client.lib.generate_map_from_segments.call_args
        self.assertTrue(call_arguments.args[-1])


if __name__ == "__main__":
    unittest.main()
