"""Register configured editor hotkeys and open their help dialog."""

from functools import partial

from PySide6.QtCore import Qt
from PySide6.QtGui import QAction, QKeySequence
from PySide6.QtWidgets import (
    QAbstractButton,
    QAbstractSpinBox,
    QComboBox,
    QLineEdit,
    QPlainTextEdit,
    QPushButton,
    QTextEdit,
    QWidget,
)

from app.hotkeys.config import (
    COMMAND_HOTKEY_HELP,
    HOTKEY_BINDINGS,
    get_hotkey_binding,
    get_hotkey_text,
)
from app.hotkeys.dialog import HotkeyHelpDialog


HOTKEY_BUTTON_WIDTH = 112
HOTKEY_BUTTON_HEIGHT = 52


class HotkeyManager:
    """Own window shortcuts, focus rules, text hints and the help dialog."""

    def __init__(self, window: QWidget) -> None:
        self.window = window
        self._shortcut_actions = []
        self._registered_commands = set()
        self._help_dialog = None

        self._validate_config()
        self.register_callback(COMMAND_HOTKEY_HELP, self.toggle_help_dialog)

    def create_help_button(self) -> QPushButton:
        """Create the localized top-toolbar button that opens shortcut help."""
        button = QPushButton(self.window)
        button.setObjectName("hotkeyHelpButton")
        button.setText(get_hotkey_text("button"))
        button.setFixedSize(HOTKEY_BUTTON_WIDTH, HOTKEY_BUTTON_HEIGHT)
        button.setToolTip(get_hotkey_text("tooltip"))
        button.clicked.connect(self.toggle_help_dialog)
        self.add_text_hint(button, COMMAND_HOTKEY_HELP, True)
        return button

    def bind_action(
        self,
        command_id: str,
        action: QAction,
        hint_on_new_line: bool = False,
    ) -> None:
        """Bind a configured shortcut to an existing toolbar action."""
        self.register_callback(command_id, action.trigger)
        self.add_text_hint(action, command_id, hint_on_new_line)

    def bind_button(
        self,
        command_id: str,
        button: QAbstractButton,
        callback=None,
        hint_on_new_line: bool = False,
    ) -> None:
        """Bind a configured shortcut to a button or checkbox."""
        if callback is None:
            callback = button.click

        guarded_callback = partial(self._activate_button, button, callback)
        self.register_callback(command_id, guarded_callback)
        self.add_text_hint(button, command_id, hint_on_new_line)

    def register_callback(self, command_id: str, callback) -> None:
        """Create window-scoped QAction shortcuts for one command callback."""
        if command_id in self._registered_commands:
            raise ValueError(f"Hotkey command registered twice: {command_id}")

        binding = get_hotkey_binding(command_id)
        if not binding.enabled:
            raise ValueError(f"Reserved hotkey command cannot be registered: {command_id}")

        for key_sequence in binding.key_sequences:
            shortcut_action = QAction(self.window)
            shortcut_action.setShortcut(QKeySequence(key_sequence))
            shortcut_action.setShortcutContext(Qt.WindowShortcut)
            shortcut_action.triggered.connect(partial(self._trigger_callback, callback))
            self.window.addAction(shortcut_action)
            self._shortcut_actions.append(shortcut_action)

        self._registered_commands.add(command_id)

    def add_text_hint(
        self,
        target,
        command_id: str,
        hint_on_new_line: bool = False,
    ) -> None:
        """Append one configured key label to an action or button text."""
        binding = get_hotkey_binding(command_id)
        hint = f"[{binding.display_key}]"
        current_text = target.text()
        if hint in current_text:
            return

        separator = " "
        if hint_on_new_line:
            separator = "\n"

        target.setText(f"{current_text}{separator}{hint}")

    def toggle_help_dialog(self) -> None:
        """Open shortcut help, or close the currently open help dialog."""
        if self._help_dialog is not None:
            self._help_dialog.reject()
            return

        self._help_dialog = HotkeyHelpDialog(self.window)
        self._help_dialog.exec()
        self._help_dialog.deleteLater()
        self._help_dialog = None

    def _trigger_callback(self, callback, checked: bool = False) -> None:
        """Run a shortcut unless an editor input currently owns the keyboard."""
        del checked
        focused_widget = self.window.focusWidget()
        if self._is_editing_widget(focused_widget):
            return

        callback()

    def _activate_button(self, button: QAbstractButton, callback) -> None:
        """Do not let hotkeys force disabled or unavailable toolbar options."""
        if not button.isEnabled():
            return
        if button.property("unavailable"):
            return

        callback()

    def _is_editing_widget(self, widget: QWidget | None) -> bool:
        """Return whether a focused widget must retain every keyboard input."""
        current_widget = widget
        while current_widget is not None:
            if isinstance(current_widget, QLineEdit):
                return True
            if isinstance(current_widget, QTextEdit):
                return True
            if isinstance(current_widget, QPlainTextEdit):
                return True
            if isinstance(current_widget, QAbstractSpinBox):
                return True
            if isinstance(current_widget, QComboBox):
                return True

            current_widget = current_widget.parentWidget()

        return False

    def _validate_config(self) -> None:
        """Fail fast when command ids or key sequences conflict."""
        command_ids = set()
        key_sequences = set()

        for binding in HOTKEY_BINDINGS:
            if binding.command_id in command_ids:
                raise ValueError(f"Duplicate hotkey command: {binding.command_id}")
            command_ids.add(binding.command_id)

            for configured_sequence in binding.key_sequences:
                sequence = QKeySequence(configured_sequence)
                normalized_sequence = sequence.toString(QKeySequence.PortableText)
                if not normalized_sequence:
                    raise ValueError(
                        f"Invalid hotkey sequence for {binding.command_id}: {configured_sequence}"
                    )
                if normalized_sequence in key_sequences:
                    raise ValueError(f"Duplicate hotkey sequence: {normalized_sequence}")

                key_sequences.add(normalized_sequence)
