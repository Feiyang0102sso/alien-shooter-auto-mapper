"""
Left shelf container holding the project version, the shelf mode and both shelves.
"""

from PySide6.QtCore import Signal
from PySide6.QtWidgets import (
    QButtonGroup,
    QHBoxLayout,
    QPushButton,
    QStackedWidget,
    QVBoxLayout,
    QWidget,
)

from app.i18n.locale import tr
from app.i18n.text_keys import TextKey
from app.project.data import (
    PROJECT_VERSION_AS1,
    PROJECT_VERSION_AS2R,
    is_as2_series_project_version,
    validate_project_version,
)
from app.ui.panels.decoration_stamp_shelf import DecorationStampShelfPanel
from app.ui.panels.theme_shelf import ThemeShelfPanel


SHELF_MODE_WALLS = 0
SHELF_MODE_DECORATIONS = 1


class LeftShelfPanel(QWidget):
    """
    Three-level shelf: project version, shelf mode, then the card list.

    The version buttons change the whole project. The mode buttons only swap
    which shelf is visible, so the two levels must never be mixed up.
    """

    wall_set_selected = Signal(int, str)
    stamp_selected = Signal(str, str)
    project_version_changed = Signal(str)

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("leftShelfPanel")

        self.theme_shelf = ThemeShelfPanel()
        self.decoration_stamp_shelf = DecorationStampShelfPanel()
        self.current_project_version = self.theme_shelf.get_project_version()

        layout = QVBoxLayout(self)
        layout.setContentsMargins(14, 8, 14, 14)
        layout.setSpacing(12)

        layout.addLayout(self._build_version_row())
        layout.addLayout(self._build_mode_row())

        self.shelf_stack = QStackedWidget()
        self.shelf_stack.addWidget(self.theme_shelf)
        self.shelf_stack.addWidget(self.decoration_stamp_shelf)
        layout.addWidget(self.shelf_stack)

        self.theme_shelf.wall_set_selected.connect(self.wall_set_selected)
        self.decoration_stamp_shelf.stamp_selected.connect(self.stamp_selected)

        self._sync_version_buttons()
        self._sync_mode_availability()

    def get_project_version(self) -> str:
        """Return the currently selected project version."""
        return self.current_project_version

    def set_project_version(self, project_version: str) -> None:
        """Select a project version without emitting a user-change signal."""
        validated_version = validate_project_version(project_version)
        self.current_project_version = validated_version
        self.theme_shelf.set_project_version(validated_version)
        self._sync_version_buttons()
        self._sync_mode_availability()

    def _build_version_row(self) -> QHBoxLayout:
        """
        Build the project version toggle row.
        """
        row = QHBoxLayout()
        row.setContentsMargins(0, 0, 0, 0)
        row.setSpacing(8)

        self.version_button_group = QButtonGroup(self)
        self.version_button_group.setExclusive(True)
        self.as1_button = self._create_version_button(PROJECT_VERSION_AS1)
        self.as2r_button = self._create_version_button(PROJECT_VERSION_AS2R)
        row.addWidget(self.as1_button)
        row.addWidget(self.as2r_button)
        return row

    def _build_mode_row(self) -> QHBoxLayout:
        """
        Build the wall / decoration shelf toggle row.
        """
        row = QHBoxLayout()
        row.setContentsMargins(0, 0, 0, 0)
        row.setSpacing(8)

        self.mode_button_group = QButtonGroup(self)
        self.mode_button_group.setExclusive(True)

        self.wall_mode_button = self._create_mode_button(
            tr(TextKey.SHELF_MODE_WALL_SETS),
            SHELF_MODE_WALLS,
        )
        self.decoration_mode_button = self._create_mode_button(
            tr(TextKey.SHELF_MODE_DECORATION_SETS),
            SHELF_MODE_DECORATIONS,
        )
        self.wall_mode_button.setChecked(True)

        row.addWidget(self.wall_mode_button)
        row.addWidget(self.decoration_mode_button)
        return row

    def _create_version_button(self, project_version: str) -> QPushButton:
        """
        Create one project version toggle button.
        """
        button = QPushButton(project_version)
        button.setObjectName("gameFamilyButton")
        button.setCheckable(True)
        button.setProperty("project_version", project_version)
        button.clicked.connect(self._on_version_button_clicked)
        self.version_button_group.addButton(button)
        return button

    def _create_mode_button(self, label: str, shelf_mode: int) -> QPushButton:
        """
        Create one shelf mode toggle button.
        """
        button = QPushButton(label)
        button.setObjectName("shelfModeButton")
        button.setCheckable(True)
        button.setProperty("shelf_mode", shelf_mode)
        button.clicked.connect(self._on_mode_button_clicked)
        self.mode_button_group.addButton(button)
        return button

    def _on_version_button_clicked(self) -> None:
        """
        Switch the whole project to another game version.
        """
        sender = self.sender()
        if sender is None:
            return

        project_version = sender.property("project_version")
        if not project_version:
            return

        if project_version == self.current_project_version:
            return

        self.set_project_version(str(project_version))
        self.project_version_changed.emit(str(project_version))

    def _on_mode_button_clicked(self) -> None:
        """
        Swap the visible shelf without touching the project version.
        """
        sender = self.sender()
        if sender is None:
            return

        shelf_mode = sender.property("shelf_mode")
        if shelf_mode is None:
            return

        self.shelf_stack.setCurrentIndex(int(shelf_mode))

    def _sync_version_buttons(self) -> None:
        """
        Keep the version buttons checked without triggering selection.
        """
        self.as1_button.blockSignals(True)
        self.as2r_button.blockSignals(True)
        self.as1_button.setChecked(self.current_project_version == PROJECT_VERSION_AS1)
        self.as2r_button.setChecked(self.current_project_version == PROJECT_VERSION_AS2R)
        self.as1_button.blockSignals(False)
        self.as2r_button.blockSignals(False)

    def _sync_mode_availability(self) -> None:
        """
        Decoration stamps are AS2 only, so AS1 projects get a disabled button.
        """
        decorations_supported = is_as2_series_project_version(self.current_project_version)
        self.decoration_mode_button.setEnabled(decorations_supported)

        if decorations_supported:
            self.decoration_mode_button.setToolTip("")
            return

        self.decoration_mode_button.setToolTip(tr(TextKey.TOOLTIP_DECORATION_SETS_AS2_ONLY))
        self.wall_mode_button.setChecked(True)
        self.shelf_stack.setCurrentIndex(SHELF_MODE_WALLS)
