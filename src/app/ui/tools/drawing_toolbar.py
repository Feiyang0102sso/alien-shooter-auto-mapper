"""Left-side toolbar for choosing drawing tools."""
from PySide6.QtCore import Qt, Signal
from PySide6.QtGui import QAction, QActionGroup
from PySide6.QtWidgets import QToolBar

from app.ui.tools.drawing_modes import DRAWING_MODE_LABELS, DrawingMode


class DrawingToolbar(QToolBar):
    """Toolbar that exposes the available drawing modes."""

    drawing_mode_changed = Signal(object)

    def __init__(self, parent=None) -> None:
        super().__init__("Drawing Tools", parent)
        self.setObjectName("drawingToolbar")
        self.setMovable(False)
        self.setOrientation(Qt.Vertical)
        self.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)

        self.action_group = QActionGroup(self)
        self.action_group.setExclusive(True)
        self._actions = {}

        self._add_mode_action(DrawingMode.POLYLINE, "Polyline")
        self._add_mode_action(DrawingMode.STRAIGHT_LINE, "Line")
        self._add_mode_action(DrawingMode.RECTANGLE, "Rect")
        self.addSeparator()
        self._add_mode_action(DrawingMode.ERASER, "Eraser")

        default_action = self._actions[DrawingMode.POLYLINE]
        default_action.setChecked(True)

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
        self.drawing_mode_changed.emit(mode)
