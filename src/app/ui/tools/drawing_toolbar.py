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
        self._actions = {}

        self._add_mode_action(DrawingMode.POLYLINE, tr(TextKey.DRAWING_POLYLINE))
        self._add_mode_action(DrawingMode.STRAIGHT_LINE, tr(TextKey.DRAWING_LINE))
        self._add_mode_action(DrawingMode.RECTANGLE, tr(TextKey.DRAWING_RECT))
        self.addSeparator()
        self._add_mode_action(DrawingMode.ERASER, tr(TextKey.DRAWING_ERASER))

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
