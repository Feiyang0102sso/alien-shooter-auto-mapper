"""Tests for project-version controls and dependent UI state."""

import unittest
from unittest.mock import Mock, patch

from PySide6.QtWidgets import QApplication, QCheckBox, QMessageBox

from app.project.data import (
    PROJECT_VERSION_AS1,
    PROJECT_VERSION_AS2,
    PROJECT_VERSION_AS2OE,
    PROJECT_VERSION_AS2R,
    supports_ceiling_generation,
)
from app.ui.main_window import MainWindow
from app.ui.panels.left_shelf import LeftShelfPanel
from app.ui.panels.theme_shelf import ThemeShelfPanel


class DoorOptionHarness:
    """Minimal Qt harness for the MainWindow door-option methods."""

    def __init__(self, project_version: str) -> None:
        self.is_door_open_check = QCheckBox()
        self.shelf_panel = Mock()
        self.shelf_panel.get_project_version.return_value = project_version
        self.viewport = Mock()
        self.status_bar = Mock()
        self.is_door_open_check.clicked.connect(self._on_clicked)

    def _on_clicked(self, checked: bool) -> None:
        MainWindow._on_is_door_open_changed(self, checked)

    def _set_checkbox_unavailable(self, checkbox: QCheckBox, unavailable: bool) -> None:
        MainWindow._set_checkbox_unavailable(self, checkbox, unavailable)

    def statusBar(self):
        """Return a status bar mock matching QMainWindow's public seam."""
        return self.status_bar


class CeilingOptionHarness:
    """Minimal Qt harness for the MainWindow ceiling-option methods."""

    def __init__(self, project_version: str) -> None:
        self.ceiling_check = QCheckBox()
        self.shelf_panel = Mock()
        self.shelf_panel.get_project_version.return_value = project_version
        self.inspector = Mock()
        self.status_bar = Mock()
        self.ceiling_check.clicked.connect(self._on_clicked)

    def _on_clicked(self, checked: bool) -> None:
        MainWindow._on_ceiling_changed(self, checked)

    def _set_checkbox_unavailable(self, checkbox: QCheckBox, unavailable: bool) -> None:
        MainWindow._set_checkbox_unavailable(self, checkbox, unavailable)

    def statusBar(self):
        """Return a status bar mock matching QMainWindow's public seam."""
        return self.status_bar


class ProjectVersionUiTest(unittest.TestCase):
    """Verify version selection and version-dependent door controls."""

    def test_as2_family_uses_one_button_and_defaults_to_as2r(self) -> None:
        """The compact family control should avoid one button per AS2 format."""
        application = QApplication.instance()
        if application is None:
            application = QApplication([])

        profiles = [
            {
                "wall_type": 0,
                "short_label": "AS1 Wall",
                "description": "AS1 test wall",
            },
            {
                "wall_type": 3,
                "short_label": "AS2R Wall",
                "description": "AS2R test wall",
            },
        ]
        changed_versions = []

        with patch("app.ui.panels.theme_shelf.get_default_wall_type", return_value=0):
            with patch.object(ThemeShelfPanel, "_load_sorted_profiles", return_value=profiles):
                with patch.object(ThemeShelfPanel, "_add_wall_set_legend"):
                    panel = LeftShelfPanel()

                    panel.project_version_changed.connect(changed_versions.append)
                    panel.as2_button.click()

                    self.assertEqual(len(panel.version_button_group.buttons()), 2)
                    self.assertEqual(panel.as2_button.text(), "AS2")
                    self.assertEqual(panel.get_project_version(), PROJECT_VERSION_AS2R)
                    self.assertEqual(changed_versions, [PROJECT_VERSION_AS2R])
                    self.assertFalse(panel.as2_format_combo.isHidden())

                    panel.as2_format_combo.setCurrentText(PROJECT_VERSION_AS2OE)

                    self.assertEqual(panel.get_project_version(), PROJECT_VERSION_AS2OE)
                    self.assertEqual(changed_versions, [PROJECT_VERSION_AS2R])

        panel.close()
        application.processEvents()

    def test_imported_as2_legacy_project_selects_as2_family_and_exact_format(self) -> None:
        """Loading a project should restore both the family and exact format."""
        application = QApplication.instance()
        if application is None:
            application = QApplication([])

        profiles = [
            {
                "wall_type": 0,
                "short_label": "AS1 Wall",
                "description": "AS1 test wall",
            },
            {
                "wall_type": 3,
                "short_label": "AS2 Wall",
                "description": "AS2 test wall",
            },
        ]

        with patch("app.ui.panels.theme_shelf.get_default_wall_type", return_value=0):
            with patch.object(ThemeShelfPanel, "_load_sorted_profiles", return_value=profiles):
                with patch.object(ThemeShelfPanel, "_add_wall_set_legend"):
                    panel = LeftShelfPanel()
                    panel.set_project_version(PROJECT_VERSION_AS2)

                    self.assertTrue(panel.as2_button.isChecked())
                    self.assertEqual(panel.as2_format_combo.currentText(), PROJECT_VERSION_AS2)
                    self.assertEqual(panel.get_project_version(), PROJECT_VERSION_AS2)

        panel.close()
        application.processEvents()

    def test_as2r_door_option_looks_unavailable_but_warns_when_clicked(self) -> None:
        """AS2R should use a gray clickable state instead of disabling Qt input."""
        application = QApplication.instance()
        if application is None:
            application = QApplication([])

        harness = DoorOptionHarness(PROJECT_VERSION_AS2R)
        MainWindow._sync_global_door_option(harness, PROJECT_VERSION_AS2R, True)

        self.assertTrue(harness.is_door_open_check.isEnabled())
        self.assertTrue(harness.is_door_open_check.property("unavailable"))
        self.assertFalse(harness.is_door_open_check.isChecked())

        with patch.object(QMessageBox, "warning") as warning:
            harness.is_door_open_check.click()

        warning.assert_called_once()
        self.assertFalse(harness.is_door_open_check.isChecked())
        harness.viewport.set_is_door_open.assert_called_with(False)

        harness.is_door_open_check.close()
        application.processEvents()

    def test_as1_door_option_restores_normal_checkbox_behavior(self) -> None:
        """Switching back to AS1 should restore styling and normal clicks."""
        application = QApplication.instance()
        if application is None:
            application = QApplication([])

        harness = DoorOptionHarness(PROJECT_VERSION_AS1)
        MainWindow._sync_global_door_option(harness, PROJECT_VERSION_AS2R, False)
        MainWindow._sync_global_door_option(harness, PROJECT_VERSION_AS1, True)

        self.assertTrue(harness.is_door_open_check.isEnabled())
        self.assertFalse(harness.is_door_open_check.property("unavailable"))
        self.assertTrue(harness.is_door_open_check.isChecked())

        with patch.object(QMessageBox, "warning") as warning:
            harness.is_door_open_check.click()

        warning.assert_not_called()
        self.assertFalse(harness.is_door_open_check.isChecked())
        harness.viewport.set_is_door_open.assert_called_with(False)

        harness.is_door_open_check.close()
        application.processEvents()

    def test_as1_and_all_as2_formats_support_ceiling_generation(self) -> None:
        """Every selectable project format should allow ceiling generation."""
        self.assertTrue(supports_ceiling_generation(PROJECT_VERSION_AS1))
        self.assertTrue(supports_ceiling_generation(PROJECT_VERSION_AS2))
        self.assertTrue(supports_ceiling_generation(PROJECT_VERSION_AS2R))
        self.assertTrue(supports_ceiling_generation(PROJECT_VERSION_AS2OE))

    def test_as2r_ceiling_option_is_available(self) -> None:
        """AS2R should preserve the requested ceiling state and accept clicks."""
        application = QApplication.instance()
        if application is None:
            application = QApplication([])

        harness = CeilingOptionHarness(PROJECT_VERSION_AS2R)
        MainWindow._sync_ceiling_option(harness, PROJECT_VERSION_AS2R, True)

        self.assertTrue(harness.ceiling_check.isEnabled())
        self.assertFalse(harness.ceiling_check.property("unavailable"))
        self.assertTrue(harness.ceiling_check.isChecked())
        harness.inspector.set_ceiling_project_state.assert_called_with(
            PROJECT_VERSION_AS2R,
            True,
        )

        with patch.object(QMessageBox, "warning") as warning:
            harness.ceiling_check.click()

        warning.assert_not_called()
        self.assertFalse(harness.ceiling_check.isChecked())

        harness.ceiling_check.close()
        application.processEvents()

    def test_as1_ceiling_option_is_available(self) -> None:
        """AS1 should enable generation and its Inspector controls."""
        application = QApplication.instance()
        if application is None:
            application = QApplication([])

        harness = CeilingOptionHarness(PROJECT_VERSION_AS1)
        MainWindow._sync_ceiling_option(harness, PROJECT_VERSION_AS1, True)

        self.assertTrue(harness.ceiling_check.isEnabled())
        self.assertFalse(harness.ceiling_check.property("unavailable"))
        self.assertTrue(harness.ceiling_check.isChecked())
        harness.inspector.set_ceiling_project_state.assert_called_with(
            PROJECT_VERSION_AS1,
            True,
        )

        with patch.object(QMessageBox, "warning") as warning:
            harness.ceiling_check.click()

        warning.assert_not_called()
        self.assertFalse(harness.ceiling_check.isChecked())
        harness.inspector.set_ceiling_project_state.assert_called_with(
            PROJECT_VERSION_AS1,
            False,
        )

        harness.ceiling_check.close()
        application.processEvents()


if __name__ == "__main__":
    unittest.main()
