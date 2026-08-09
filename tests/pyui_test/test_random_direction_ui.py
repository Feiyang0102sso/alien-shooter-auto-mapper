"""Tests for the UI random-direction generation option."""

import unittest
from unittest.mock import Mock, patch

from PySide6.QtWidgets import QApplication, QCheckBox, QMessageBox

from app.project.data import ProjectData
from app.ui.main_window import MainWindow


class RandomDirectionHarness:
    """Minimal Qt harness for the map generation action."""

    def __init__(self, random_direction: bool) -> None:
        self.auto_mapper_client = Mock()
        self.auto_mapper_client.generate_map.return_value = True
        self.floor_check = QCheckBox()
        self.floor_check.setChecked(True)
        self.random_direction_check = QCheckBox()
        self.random_direction_check.setChecked(random_direction)
        self.status_bar = Mock()

    def _collect_project_data(self) -> ProjectData:
        """Return a non-empty project so generation reaches the DLL client."""
        return ProjectData(segments=[object()])

    def statusBar(self):
        """Return a status bar mock matching QMainWindow's public seam."""
        return self.status_bar


class RandomDirectionUiTest(unittest.TestCase):
    """Verify the checkbox value reaches the Python DLL client."""

    def test_generate_map_passes_random_direction_checkbox_value(self) -> None:
        """Both checkbox states should be forwarded without reinterpretation."""
        application = QApplication.instance()
        if application is None:
            application = QApplication([])

        for random_direction in (False, True):
            with self.subTest(random_direction=random_direction):
                harness = RandomDirectionHarness(random_direction)

                with patch(
                    "app.ui.main_window.QFileDialog.getSaveFileName",
                    return_value=("random_direction_test.map", ""),
                ):
                    with patch.object(QMessageBox, "information"):
                        MainWindow._generate_map(harness)

                call_arguments = harness.auto_mapper_client.generate_map.call_args
                self.assertEqual(
                    call_arguments.kwargs["random_direction"],
                    random_direction,
                )

                harness.floor_check.close()
                harness.random_direction_check.close()

        application.processEvents()


if __name__ == "__main__":
    unittest.main()
