"""Tests for wall preview asset coverage."""

import unittest

from app.ui.previews.wall_assets import (
    AS1_PREVIEW_ROOT,
    AS2_PREVIEW_ROOT,
    get_wall_component_preview_path,
    get_wall_set_preview_path,
)


class WallPreviewAssetsTest(unittest.TestCase):
    """Verify AS1 compatibility and complete AS2 preview registration."""

    def test_wall_set_cards_have_existing_representative_images(self) -> None:
        """Every visible AS1 and AS2 wall set should have a card image."""
        visible_wall_types = [0, 2, 1, 3, 6, 7, 8, 9, 10, 11, 12, 13]

        for wall_type in visible_wall_types:
            preview_path = get_wall_set_preview_path(wall_type)
            self.assertIsNotNone(preview_path)
            self.assertTrue(preview_path.is_file())

        set1_preview_path = get_wall_set_preview_path(3)
        self.assertEqual(set1_preview_path.name, "wall_set1_1.webp")

    def test_every_as1_image_remains_mapped_to_a_component(self) -> None:
        """The refactor should preserve all existing AS1 component previews."""
        part_ids_by_wall_type = {
            0: [
                "wall_body",
                "active_door",
                "dead_door_closed",
                "dead_door_jammed",
                "dead_door_open",
            ],
            1: [
                "wall_body",
                "lab_laser_closed",
                "lab_laser_open",
                "lab_decoration_door",
            ],
            2: [
                "wall_body",
                "active_door",
                "dead_door_closed",
                "dead_door_jammed",
                "dead_door_open",
            ],
        }

        mapped_paths = set()
        for wall_type, part_ids in part_ids_by_wall_type.items():
            for part_id in part_ids:
                preview_path = get_wall_component_preview_path(wall_type, part_id)
                self.assertIsNotNone(preview_path)
                mapped_paths.add(preview_path)

        asset_paths = set(AS1_PREVIEW_ROOT.glob("*/*.webp"))
        self.assertEqual(mapped_paths, asset_paths)

    def test_every_as2_image_is_mapped_to_a_component(self) -> None:
        """All AS2 wall body and door images should be reachable by the inspector."""
        mapped_paths = set()
        wall_type = 3
        while wall_type <= 13:
            wall_body_path = get_wall_component_preview_path(wall_type, "wall_body")
            closed_door_path = get_wall_component_preview_path(wall_type, "as2_door_closed")
            open_door_path = get_wall_component_preview_path(wall_type, "as2_door_open")

            self.assertIsNotNone(wall_body_path)
            self.assertIsNotNone(closed_door_path)
            self.assertIsNotNone(open_door_path)

            mapped_paths.add(wall_body_path)
            mapped_paths.add(closed_door_path)
            mapped_paths.add(open_door_path)
            wall_type += 1

        asset_paths = set(AS2_PREVIEW_ROOT.glob("wall_set*/*.webp"))
        self.assertEqual(mapped_paths, asset_paths)
        self.assertEqual(len(asset_paths), 29)

    def test_unknown_component_has_no_preview(self) -> None:
        """Unknown resource keys should preserve the existing missing-preview behavior."""
        preview_path = get_wall_component_preview_path(999, "wall_body")
        self.assertIsNone(preview_path)


if __name__ == "__main__":
    unittest.main()
