"""Tests for wall set drawable-part aggregation and legend states."""

import unittest
from unittest.mock import patch

from app.editor import drawable_parts
from app.ui.panels.theme_shelf import LEGEND_DOOR_STYLES


class DrawablePartsTest(unittest.TestCase):
    """Verify wall set aggregation and fixed legend state rules."""

    def test_regular_wall_set_keeps_dll_drawable_parts(self) -> None:
        """A regular wall set should preserve the DLL order and labels."""
        test_parts = [
            ("wall_body", "墙体主体"),
            ("active_door", "活动门"),
        ]

        with patch.object(drawable_parts, "get_drawable_parts", return_value=test_parts):
            items = drawable_parts.get_wall_set_drawable_parts(0)

        self.assertEqual(
            items,
            [
                (0, "wall_body", "墙体主体"),
                (0, "active_door", "活动门"),
            ],
        )

    def test_as2_set1_combines_three_walls_and_two_doors(self) -> None:
        """AS2 Set 1 should expose all five choices represented by its card."""
        test_parts = [
            ("wall_body", "墙体主体"),
            ("as2_door_closed", "门（关闭）"),
            ("as2_door_open", "门（打开）"),
        ]

        with patch.object(drawable_parts, "get_drawable_parts", return_value=test_parts):
            items = drawable_parts.get_wall_set_drawable_parts(3)

        self.assertEqual(
            items,
            [
                (3, "wall_body", "墙体主体 1"),
                (4, "wall_body", "墙体主体 2"),
                (5, "wall_body", "墙体主体 3"),
                (3, "as2_door_closed", "门（关闭）"),
                (3, "as2_door_open", "门（打开）"),
            ],
        )

    def test_active_door_legend_always_uses_hollow_dot(self) -> None:
        """The active-door legend is a stable category marker, not live state."""
        active_door_style = LEGEND_DOOR_STYLES["active_door"]

        self.assertTrue(active_door_style[2])


if __name__ == "__main__":
    unittest.main()
