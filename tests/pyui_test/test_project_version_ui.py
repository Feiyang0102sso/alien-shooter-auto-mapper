"""Tests for project-version controls and dependent UI state."""

import unittest
from unittest.mock import Mock, patch

from PySide6.QtWidgets import QApplication, QCheckBox, QMessageBox

from app.project.data import (
    PROJECT_VERSION_AS1,
    PROJECT_VERSION_AS2R,
    supports_ceiling_generation,
)
from app.ui.main_window import MainWindow
from app.ui.panels.theme_shelf import ThemeShelfPanel


class DoorOptionHarness:
    """Minimal Qt harness for the MainWindow door-option methods."""

    def __init__(self, project_version: str) -> None:
        self.is_door_open_check = QCheckBox()
        self.theme_shelf = Mock()
        self.theme_shelf.get_project_version.return_value = project_version
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
        self.theme_shelf = Mock()
        self.theme_shelf.get_project_version.return_value = project_version
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

    def test_theme_shelf_labels_and_selects_as2r(self) -> None:
        """The visible version button should use the precise AS2R name."""
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
                    panel = ThemeShelfPanel()

                    panel.project_version_changed.connect(changed_versions.append)
                    panel.as2r_button.click()

                    self.assertEqual(panel.as2r_button.text(), PROJECT_VERSION_AS2R)
                    self.assertEqual(panel.get_project_version(), PROJECT_VERSION_AS2R)
                    self.assertEqual(changed_versions, [PROJECT_VERSION_AS2R])

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

    def test_only_as2_series_supports_ceiling_generation(self) -> None:
        """AS2R should support ceilings while AS1 remains unavailable."""
        self.assertFalse(supports_ceiling_generation(PROJECT_VERSION_AS1))
        self.assertTrue(supports_ceiling_generation(PROJECT_VERSION_AS2R))

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

        with patch.object(QMessageBox, "warning") as warning:
            harness.ceiling_check.click()

        warning.assert_not_called()
        self.assertFalse(harness.ceiling_check.isChecked())

        harness.ceiling_check.close()
        application.processEvents()

    def test_as1_ceiling_option_looks_unavailable_and_rejects_clicks(self) -> None:
        """AS1 should keep the ceiling control visible but prevent enabling it."""
        application = QApplication.instance()
        if application is None:
            application = QApplication([])

        harness = CeilingOptionHarness(PROJECT_VERSION_AS1)
        MainWindow._sync_ceiling_option(harness, PROJECT_VERSION_AS1, True)

        self.assertTrue(harness.ceiling_check.isEnabled())
        self.assertTrue(harness.ceiling_check.property("unavailable"))
        self.assertFalse(harness.ceiling_check.isChecked())

        with patch.object(QMessageBox, "warning") as warning:
            harness.ceiling_check.click()

        warning.assert_called_once()
        self.assertFalse(harness.ceiling_check.isChecked())

        harness.ceiling_check.close()
        application.processEvents()


if __name__ == "__main__":
    unittest.main()
