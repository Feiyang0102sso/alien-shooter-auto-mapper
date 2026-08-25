"""Tests for AS1 ceiling-layer Inspector controls."""

import unittest

from PySide6.QtWidgets import QApplication

from app.project.data import PROJECT_VERSION_AS1, PROJECT_VERSION_AS2R
from app.ui.panels.as1_ceiling import AS1CeilingPropertiesWidget


class AS1CeilingUiTest(unittest.TestCase):
    """Verify AS1-only visibility, enabled state, and synchronized inputs."""

    config = {
        "min_layer_count": 1,
        "max_layer_count": 50,
        "default_standard_layer_count": 13,
        "default_lab_layer_count": 6,
    }

    def setUp(self) -> None:
        """Create the Qt application and one ceiling widget."""
        self.application = QApplication.instance()
        if self.application is None:
            self.application = QApplication([])

        self.widget = AS1CeilingPropertiesWidget(self.config)

    def tearDown(self) -> None:
        """Close controls after each test."""
        self.widget.close()
        self.application.processEvents()

    def test_as1_group_is_disabled_until_generation_is_enabled(self) -> None:
        """The whole AS1 group should gray out under the master switch."""
        self.widget.set_project_state(PROJECT_VERSION_AS1, False)

        self.assertFalse(self.widget.isHidden())
        self.assertFalse(self.widget.isEnabled())

        self.widget.set_project_state(PROJECT_VERSION_AS1, True)

        self.assertTrue(self.widget.isEnabled())

    def test_as2r_hides_the_entire_group(self) -> None:
        """AS2R should leave no Inspector space for AS1-only settings."""
        self.widget.set_project_state(PROJECT_VERSION_AS2R, True)

        self.assertTrue(self.widget.isHidden())

    def test_slider_and_spinbox_stay_synchronized(self) -> None:
        """Both input methods should represent the same integer value."""
        self.widget.standard_control.slider.setValue(21)
        self.assertEqual(self.widget.standard_control.spinbox.value(), 21)

        self.widget.lab_control.spinbox.setValue(9)
        self.assertEqual(self.widget.lab_control.slider.value(), 9)
        self.assertEqual(self.widget.get_layer_counts(), (21, 9))

    def test_reset_uses_dll_provided_defaults(self) -> None:
        """New and legacy projects should restore DLL-provided defaults."""
        self.widget.set_layer_counts(20, 10)
        self.widget.reset_layer_counts()

        self.assertEqual(self.widget.get_layer_counts(), (13, 6))


if __name__ == "__main__":
    unittest.main()
