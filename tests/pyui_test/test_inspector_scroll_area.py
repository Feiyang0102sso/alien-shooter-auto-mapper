"""Tests for the scrollable Inspector container."""

import unittest

from PySide6.QtGui import QPalette
from PySide6.QtWidgets import QApplication, QMainWindow, QScrollArea, QTabWidget, QWidget

from app.main import get_style_path
from app.ui.main_window import MainWindow


class InspectorScrollAreaTest(unittest.TestCase):
    """Verify that the native scroll viewport keeps the Inspector theme."""

    def setUp(self) -> None:
        """Build only the dock widgets needed by the regression test."""
        self.application = QApplication.instance()
        if self.application is None:
            self.application = QApplication([])

        self.original_style_sheet = self.application.styleSheet()
        style_text = get_style_path().read_text(encoding="utf-8")
        self.application.setStyleSheet(style_text)

        self.window = QMainWindow.__new__(MainWindow)
        QMainWindow.__init__(self.window)
        self.window.left_shelf_tabs = QTabWidget()
        self.window.theme_shelf = QWidget()
        self.window.decoration_shelf = QWidget()
        self.window.inspector = QWidget()
        self.window.inspector.setObjectName("inspectorPanel")
        self.window._build_docks()
        self.window.show()
        self.application.processEvents()

    def tearDown(self) -> None:
        """Restore the application stylesheet after the test."""
        self.window.close()
        self.application.setStyleSheet(self.original_style_sheet)
        self.application.processEvents()

    def test_scroll_viewport_uses_inspector_background(self) -> None:
        """The platform viewport must never expose its default light palette."""
        scroll_area = self.window.findChild(QScrollArea, "inspectorScrollArea")
        self.assertIsNotNone(scroll_area)

        viewport_color = scroll_area.viewport().palette().color(QPalette.Window)
        self.assertEqual(viewport_color.name(), "#121b20")


if __name__ == "__main__":
    unittest.main()
