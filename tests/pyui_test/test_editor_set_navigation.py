"""Tests for keyboard-ready Editor Set and Set Item navigation."""

import unittest
from pathlib import Path
from unittest.mock import Mock, patch

from PySide6.QtWidgets import QApplication

from app.project.data import PROJECT_VERSION_AS1, PROJECT_VERSION_AS2R
from app.ui.main_window import MainWindow
from app.ui.panels.decoration_stamp_shelf import DecorationStampShelfPanel
from app.ui.panels.inspector import InspectorPanel
from app.ui.panels.left_shelf import SHELF_MODE_DECORATIONS, SHELF_MODE_WALLS
from app.ui.panels.theme_shelf import ThemeShelfPanel
from app.ui.tools.drawing_modes import DrawingMode


CEILING_CONFIG = {
    "min_layer_count": 1,
    "max_layer_count": 20,
    "default_standard_layer_count": 13,
    "default_lab_layer_count": 6,
}


class DrawingHotkeyHarness:
    """Minimal receiver for MainWindow's number-key drawing-mode seam."""

    def __init__(self, shelf_mode: int) -> None:
        self.shelf_panel = Mock()
        self.shelf_panel.get_shelf_mode.return_value = shelf_mode
        self.drawing_toolbar = Mock()
        self._on_drawing_mode_changed = Mock()


class EditorSetNavigationTest(unittest.TestCase):
    """Verify boundaries, highlighting, memory and shelf-only tools."""

    @classmethod
    def setUpClass(cls) -> None:
        cls.application = QApplication.instance()
        if cls.application is None:
            cls.application = QApplication([])

    def test_wall_set_navigation_stops_and_remembers_each_family(self) -> None:
        """Wall cards should highlight, clamp and restore AS1/AS2 selections."""
        profiles = [
            {"wall_type": 0, "short_label": "Standard", "description": "A"},
            {"wall_type": 2, "short_label": "Dark", "description": "B"},
            {"wall_type": 3, "short_label": "Set 1", "description": "C"},
            {"wall_type": 6, "short_label": "Set 2", "description": "D"},
        ]
        selected_types = []

        with patch("app.ui.panels.theme_shelf.get_default_wall_type", return_value=0):
            with patch.object(ThemeShelfPanel, "_load_sorted_profiles", return_value=profiles):
                with patch.object(ThemeShelfPanel, "_add_wall_set_legend"):
                    panel = ThemeShelfPanel()

                    def record_wall_type(wall_type: int, name: str) -> None:
                        del name
                        selected_types.append(wall_type)

                    panel.wall_set_selected.connect(record_wall_type)
                    panel.select_relative_wall_set(-1)
                    self.assertEqual(selected_types, [])

                    panel.select_relative_wall_set(1)
                    panel.select_relative_wall_set(1)
                    self.assertEqual(selected_types, [2])
                    self.assertTrue(panel._cards_by_wall_type[2].property("selected"))

                    panel.set_project_version(PROJECT_VERSION_AS2R)
                    panel.select_relative_wall_set(1)
                    self.assertEqual(panel.selected_wall_type, 6)

                    panel.set_project_version(PROJECT_VERSION_AS1)
                    self.assertEqual(panel.selected_wall_type, 2)

        panel.close()

    def test_decoration_series_navigation_stops_and_highlights(self) -> None:
        """Decoration cards should initialize once and never wrap."""
        series_items = [
            self._series("first", "First", "first01"),
            self._series("second", "Second", "second01"),
        ]
        selected_profiles = []

        with patch(
            "app.ui.panels.decoration_stamp_shelf.get_decoration_stamp_series_items",
            return_value=series_items,
        ):
            with patch(
                "app.ui.panels.decoration_stamp_shelf.get_decoration_stamp_series_thumbnail_path",
                return_value=Path("missing.png"),
            ):
                panel = DecorationStampShelfPanel()

                def record_profile_id(profile_id: str, name: str) -> None:
                    del name
                    selected_profiles.append(profile_id)

                panel.stamp_selected.connect(record_profile_id)

                panel.ensure_selection()
                panel.select_relative_series(-1)
                panel.select_relative_series(1)
                panel.select_relative_series(1)

                self.assertEqual(selected_profiles, ["first01", "second01"])
                self.assertEqual(panel.selected_series_id, "second")
                self.assertTrue(panel._cards_by_series_id["second"].property("selected"))

        panel.close()

    def test_inspector_remembers_component_per_wall_set(self) -> None:
        """Returning to a wall set should restore its previous combo item."""
        def drawable_parts(wall_type: int) -> list:
            if wall_type == 0:
                return [(0, "wall_body", "Wall"), (0, "door", "Door")]

            return [(1, "wall_body", "Lab Wall"), (1, "laser", "Laser")]

        def wall_profile(wall_type: int) -> dict:
            return {
                "short_label": f"Wall {wall_type}",
                "dir_a_vid": 1,
                "dir_b_vid": 2,
                "pillar_vid": 3,
            }

        with patch("app.ui.panels.inspector.get_default_wall_type", return_value=0):
            with patch("app.ui.panels.inspector.get_wall_profile", side_effect=wall_profile):
                with patch(
                    "app.ui.panels.inspector.get_wall_set_drawable_parts",
                    side_effect=drawable_parts,
                ):
                    with patch(
                        "app.ui.panels.inspector.get_wall_component_preview_path",
                        return_value=Path("missing.png"),
                    ):
                        inspector = InspectorPanel(CEILING_CONFIG)
                        inspector.component_combo.setCurrentIndex(1)
                        inspector.set_wall_set(1, "Lab")
                        inspector.set_wall_set(0, "Standard")

                        self.assertEqual(inspector.component_combo.currentIndex(), 1)
                        self.assertEqual(inspector.component_combo.currentText(), "Door")

        inspector.close()

    def test_number_tools_respect_shelf_context(self) -> None:
        """Select and eraser should remain exclusive to their intended shelves."""
        wall_harness = DrawingHotkeyHarness(SHELF_MODE_WALLS)
        MainWindow._activate_drawing_mode_from_hotkey(wall_harness, DrawingMode.SELECT)
        wall_harness._on_drawing_mode_changed.assert_not_called()

        decoration_harness = DrawingHotkeyHarness(SHELF_MODE_DECORATIONS)
        MainWindow._activate_drawing_mode_from_hotkey(
            decoration_harness,
            DrawingMode.ERASER,
        )
        decoration_harness._on_drawing_mode_changed.assert_not_called()

        MainWindow._activate_drawing_mode_from_hotkey(
            decoration_harness,
            DrawingMode.POLYLINE,
        )
        decoration_harness.drawing_toolbar.set_mode.assert_called_once_with(
            DrawingMode.POLYLINE
        )
        decoration_harness._on_drawing_mode_changed.assert_called_once_with(
            DrawingMode.POLYLINE
        )

    def _series(self, series_id: str, label: str, profile_id: str) -> dict:
        """Build one compact decoration-series fixture."""
        return {
            "series_id": series_id,
            "label": label,
            "description": label,
            "default_profile_id": profile_id,
            "variant_count": 1,
        }


if __name__ == "__main__":
    unittest.main()
