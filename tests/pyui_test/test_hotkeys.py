"""Tests for centralized editor hotkeys and their help dialog."""

import unittest
from unittest.mock import Mock, patch

from PySide6.QtCore import Qt
from PySide6.QtGui import QAction
from PySide6.QtTest import QTest
from PySide6.QtWidgets import (
    QApplication,
    QCheckBox,
    QLineEdit,
    QMainWindow,
    QWidget,
)

from app.hotkeys.config import (
    COMMAND_CEILING,
    COMMAND_DELETE_DECORATION,
    COMMAND_HOTKEY_HELP,
    COMMAND_NEXT_ITEM,
    COMMAND_PREVIOUS_ITEM,
    HOTKEY_BINDINGS,
    HotkeyBinding,
    PAGE_FUNCTIONS,
)
from app.hotkeys.dialog import HotkeyHelpDialog
from app.hotkeys.manager import HotkeyManager


class HotkeyTest(unittest.TestCase):
    """Verify config, focus protection, text hints and help navigation."""

    @classmethod
    def setUpClass(cls) -> None:
        cls.application = QApplication.instance()
        if cls.application is None:
            cls.application = QApplication([])

    def test_config_contains_final_key_map_and_numpad_aliases(self) -> None:
        """The approved mappings should have one ordered source of truth."""
        display_keys = []
        binding_by_command = {}
        for binding in HOTKEY_BINDINGS:
            display_keys.append(binding.display_key)
            binding_by_command[binding.command_id] = binding

        expected_function_keys = []
        for index in range(1, 13):
            expected_function_keys.append(f"F{index}")

        self.assertEqual(display_keys[:12], expected_function_keys)

        expected_tool_keys = []
        for index in range(0, 10):
            expected_tool_keys.append(str(index))
        self.assertEqual(display_keys[16:26], expected_tool_keys)
        self.assertEqual(
            binding_by_command[COMMAND_PREVIOUS_ITEM].key_sequences,
            ("-", "Num+-"),
        )
        self.assertEqual(
            binding_by_command[COMMAND_NEXT_ITEM].key_sequences,
            ("Shift+=", "Num++"),
        )
        self.assertEqual(
            binding_by_command[COMMAND_DELETE_DECORATION].key_sequences,
            ("Delete",),
        )

    def test_manager_appends_hint_and_runs_bound_action(self) -> None:
        """A configured action should display and execute its shortcut."""
        window = QMainWindow()
        manager = HotkeyManager(window)
        action = QAction("Ceiling", window)
        callback = Mock()
        action.triggered.connect(callback)

        manager.bind_action(COMMAND_CEILING, action, True)
        self._trigger_sequence(manager, "F6")

        self.assertEqual(action.text(), "Ceiling\n[F6]")
        callback.assert_called_once()
        window.close()

    def test_input_focus_pauses_every_registered_callback(self) -> None:
        """Function keys must not bypass the common input-focus guard."""
        window = QMainWindow()
        input_widget = QLineEdit(window)
        window.setCentralWidget(input_widget)
        window.show()
        input_widget.setFocus()
        self.application.processEvents()

        manager = HotkeyManager(window)
        callback = Mock()
        manager.register_callback(COMMAND_CEILING, callback)
        self._trigger_sequence(manager, "F6")

        callback.assert_not_called()
        window.close()

    def test_main_and_numpad_plus_both_trigger_next_item(self) -> None:
        """Both physical plus-key locations should use the same command."""
        window = QMainWindow()
        canvas = QWidget(window)
        canvas.setFocusPolicy(Qt.StrongFocus)
        window.setCentralWidget(canvas)
        window.show()
        window.activateWindow()
        canvas.setFocus()
        QTest.qWait(20)

        manager = HotkeyManager(window)
        callback = Mock()
        manager.register_callback(COMMAND_NEXT_ITEM, callback)

        QTest.keyClick(canvas, Qt.Key_Equal, Qt.ShiftModifier)
        QTest.qWait(20)
        self.assertEqual(callback.call_count, 1)

        QTest.keyClick(canvas, Qt.Key_Plus, Qt.KeypadModifier)
        QTest.qWait(20)
        self.assertEqual(callback.call_count, 2)
        window.close()

    def test_unavailable_button_is_not_forced_by_hotkey(self) -> None:
        """Gray project-incompatible options should remain unchanged."""
        window = QMainWindow()
        manager = HotkeyManager(window)
        checkbox = QCheckBox("Ceiling", window)
        checkbox.setProperty("unavailable", True)

        manager.bind_button(COMMAND_CEILING, checkbox)
        self._trigger_sequence(manager, "F6")

        self.assertFalse(checkbox.isChecked())
        window.close()

    def test_duplicate_config_fails_fast(self) -> None:
        """A duplicate key must never silently replace another command."""
        duplicate_bindings = (
            HotkeyBinding("first", "F1", ("F1",), PAGE_FUNCTIONS),
            HotkeyBinding("second", "F1", ("F1",), PAGE_FUNCTIONS),
        )
        window = QMainWindow()

        with patch("app.hotkeys.manager.HOTKEY_BINDINGS", duplicate_bindings):
            with self.assertRaisesRegex(ValueError, "Duplicate hotkey sequence"):
                HotkeyManager(window)

        window.close()

    def test_help_dialog_has_three_non_wrapping_pages(self) -> None:
        """Help navigation should stop at both ends."""
        dialog = HotkeyHelpDialog()

        self.assertEqual(dialog.page_stack.count(), 3)
        self.assertEqual(dialog.page_indicator.text(), "1 / 3")
        self.assertFalse(dialog.previous_button.isEnabled())

        dialog.show_previous_page()
        self.assertEqual(dialog.page_stack.currentIndex(), 0)

        dialog.show_next_page()
        dialog.show_next_page()
        dialog.show_next_page()
        self.assertEqual(dialog.page_stack.currentIndex(), 2)
        self.assertEqual(dialog.page_indicator.text(), "3 / 3")
        self.assertFalse(dialog.next_button.isEnabled())
        dialog.close()

    def _trigger_sequence(self, manager: HotkeyManager, sequence: str) -> None:
        """Trigger the QAction created for one portable key sequence."""
        for action in manager._shortcut_actions:
            if action.shortcut().toString() == sequence:
                action.trigger()
                return

        self.fail(f"Hotkey action not registered: {sequence}")


if __name__ == "__main__":
    unittest.main()
