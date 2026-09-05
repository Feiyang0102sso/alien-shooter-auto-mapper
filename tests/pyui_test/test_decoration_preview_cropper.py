"""Tests for automatic decoration preview job discovery."""

import unittest
from pathlib import Path

from tools.decoration_pipeline.decoration_preview_cropper import (
    build_series_cover_output_path,
    is_series_cover_stem,
)


class DecorationPreviewCropperTest(unittest.TestCase):
    """Verify that cover images cannot be confused with room screenshots."""

    def test_supported_series_cover_names(self) -> None:
        """Plain and Windows duplicate cover names should both be accepted."""
        self.assertTrue(is_series_cover_stem("machines00"))
        self.assertTrue(is_series_cover_stem("00"))
        self.assertTrue(is_series_cover_stem("00 (1)"))
        self.assertTrue(is_series_cover_stem("00 (10)"))

    def test_timestamp_room_screenshot_is_not_a_series_cover(self) -> None:
        """A timestamp ending in 00 remains available for map pairing."""
        self.assertFalse(is_series_cover_stem("屏幕截图 2026-09-04 201500"))

    def test_numbered_folder_builds_canonical_cover_name(self) -> None:
        """The exported filename should match the UI series-card lookup."""
        folder = Path("06vehicle_workshop")
        output_path = build_series_cover_output_path(folder)
        self.assertEqual(output_path.name, "vehicle_workshop00_thumbnail.webp")

        underscore_folder = Path("99_other_rooms")
        underscore_output_path = build_series_cover_output_path(underscore_folder)
        self.assertEqual(underscore_output_path.name, "other_rooms00_thumbnail.webp")


if __name__ == "__main__":
    unittest.main()
