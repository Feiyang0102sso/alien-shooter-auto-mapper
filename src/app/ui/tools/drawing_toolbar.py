"""Left-side toolbar for choosing drawing tools."""
from PySide6.QtCore import Qt, Signal
from PySide6.QtGui import QAction, QActionGroup
from PySide6.QtWidgets import QLabel, QToolBar

from app.i18n.locale import tr
from app.i18n.text_keys import TextKey
from app.ui.tools.drawing_modes import DRAWING_MODE_LABELS, DrawingMode


class DrawingToolbar(QToolBar):
    """Toolbar that exposes the available drawing modes."""

    drawing_mode_changed = Signal(object)

    def __init__(self, parent=None) -> None:
        super().__init__(tr(TextKey.TOOLBAR_DRAWING_TOOLS), parent)
        self.setObjectName("drawingToolbar")
        self.setMovable(False)
        self.setOrientation(Qt.Vertical)
        self.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)

        # Title label matching panelTitle style
        title = QLabel(tr(TextKey.PANEL_TOOLS))
        title.setObjectName("panelTitle")
        title.setAlignment(Qt.AlignCenter)
        self.addWidget(title)

        self.action_group = QActionGroup(self)
        self.action_group.setExclusive(True)
        # Placing a decoration stamp owns the left button, so no drawing tool is
        # active then. The group must be able to show nothing checked.
        self.action_group.setExclusionPolicy(QActionGroup.ExclusionPolicy.ExclusiveOptional)
        self._actions = {}
        self._separators = {}
        self._suppress_emit = False

        # Select comes first: it is the only tool that edits already placed things.
        self._add_mode_action(DrawingMode.SELECT, tr(TextKey.DRAWING_SELECT))
        # Each hideable tool owns the separator next to it, so hiding the tool
        # does not leave a dangling divider behind.
        self._separators[DrawingMode.SELECT] = self.addSeparator()
        self._add_mode_action(DrawingMode.POLYLINE, tr(TextKey.DRAWING_POLYLINE))
        self._add_mode_action(DrawingMode.STRAIGHT_LINE, tr(TextKey.DRAWING_LINE))
        self._add_mode_action(DrawingMode.RECTANGLE, tr(TextKey.DRAWING_RECT))
        self._separators[DrawingMode.ERASER] = self.addSeparator()
        self._add_mode_action(DrawingMode.ERASER, tr(TextKey.DRAWING_ERASER))

        default_action = self._actions[DrawingMode.POLYLINE]
        default_action.setChecked(True)

    def set_mode(self, mode: DrawingMode) -> None:
        """
        Check the matching toolbar action without emitting a duplicate signal.
        """
        action = self._actions.get(mode)
        if action is None:
            return

        self._suppress_emit = True
        action.setChecked(True)
        self._suppress_emit = False

    def get_checked_mode(self):
        """
        Return the active tool mode, or None when no tool is checked.
        """
        checked_action = self.action_group.checkedAction()
        if checked_action is None:
            return None

        for mode, action in self._actions.items():
            if action is checked_action:
                return mode

        return None

    def get_mode_action(self, mode: DrawingMode):
        """Return the QAction backing one drawing mode."""
        return self._actions.get(mode)

    def set_mode_visible(self, mode: DrawingMode, visible: bool) -> None:
        """
        Show or hide one tool together with its separator.

        A tool that cannot act on the current shelf is only a dead button.
        """
        action = self._actions.get(mode)
        if action is None:
            return

        action.setVisible(visible)

        separator = self._separators.get(mode)
        if separator is not None:
            separator.setVisible(visible)

    def clear_mode(self) -> None:
        """
        Uncheck every tool, for when something outside the toolbar takes over.
        """
        checked_action = self.action_group.checkedAction()
        if checked_action is None:
            return

        self._suppress_emit = True
        checked_action.setChecked(False)
        self._suppress_emit = False

    def _add_mode_action(self, mode: DrawingMode, text: str) -> None:
        """Create a checkable action for one drawing mode."""
        action = QAction(text, self)
        action.setCheckable(True)
        action.setToolTip(DRAWING_MODE_LABELS[mode])
        action.triggered.connect(lambda checked, selected_mode=mode: self._emit_mode(selected_mode))

        self.action_group.addAction(action)
        self.addAction(action)
        self._actions[mode] = action

    def _emit_mode(self, mode: DrawingMode) -> None:
        """Emit the selected mode when its action becomes active."""
        if self._suppress_emit:
            return

        self.drawing_mode_changed.emit(mode)
