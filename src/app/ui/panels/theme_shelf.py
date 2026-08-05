"""
Wall set shelf panel for choosing a C++ supported wall profile.
"""
from pathlib import Path

from PySide6.QtCore import Signal, Qt
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QButtonGroup, QFrame, QHBoxLayout, QLabel, QPushButton, QScrollArea, QVBoxLayout, QWidget

from app.config import APP_RESOURCE_DIR
from app.editor.wall_profiles import get_default_wall_type, get_wall_profiles
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey


IMAGE_ROOT = APP_RESOURCE_DIR / "images" / "preview" / "AS1"

PREVIEW_IMAGES = {
    "base": IMAGE_ROOT / "standard" / "standard_wall.webp",
    "lab": IMAGE_ROOT / "lab" / "lab_wall.webp",
    "standard_dark": IMAGE_ROOT / "standard_dark" / "standard_wall_dark.webp",
}

WALL_SET_ORDER = {
    0: 0,
    2: 1,
    1: 2,
    3: 10,
    6: 11,
    7: 12,
    8: 13,
    9: 14,
    10: 15,
    11: 16,
    12: 17,
    13: 18,
}

AS1_WALL_TYPES = {0, 1, 2}
AS2_WALL_TYPES = {3, 6, 7, 8, 9, 10, 11, 12, 13}
AS2_SET1_WALL_TYPES = {3, 4, 5}
GAME_FAMILY_AS1 = "AS1"
GAME_FAMILY_AS2 = "AS2"


class ThemeShelfPanel(QWidget):
    """
    Wall set selector grouped by game family.
    """

    wall_set_selected = Signal(int, str)
    game_family_changed = Signal(str)

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("themeShelfPanel")
        self.selected_wall_type = get_default_wall_type()
        self.current_game_family = self._get_game_family_for_wall_type(self.selected_wall_type)
        self.profiles = self._load_sorted_profiles()

        layout = QVBoxLayout(self)
        layout.setContentsMargins(14, 8, 14, 14)
        layout.setSpacing(12)

        title = QLabel(tr(TextKey.PANEL_WALL_SETS))
        title.setObjectName("panelTitle")
        layout.addWidget(title)

        family_layout = QHBoxLayout()
        family_layout.setContentsMargins(0, 0, 0, 0)
        family_layout.setSpacing(8)

        self.family_button_group = QButtonGroup(self)
        self.family_button_group.setExclusive(True)
        self.as1_button = self._create_family_button(GAME_FAMILY_AS1)
        self.as2_button = self._create_family_button(GAME_FAMILY_AS2)
        family_layout.addWidget(self.as1_button)
        family_layout.addWidget(self.as2_button)
        layout.addLayout(family_layout)

        scroll_area = QScrollArea()
        scroll_area.setObjectName("themeShelfScrollArea")
        scroll_area.setWidgetResizable(True)
        scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)

        self.content = QWidget()
        self.content.setObjectName("themeShelfScrollContent")
        self.content_layout = QVBoxLayout(self.content)
        self.content_layout.setContentsMargins(0, 0, 0, 0)
        self.content_layout.setSpacing(12)
        scroll_area.setWidget(self.content)
        layout.addWidget(scroll_area)

        self._sync_family_buttons()
        self._populate_wall_cards()

    def _create_family_button(self, game_family: str) -> QPushButton:
        """
        Create one AS family toggle button.
        """
        button = QPushButton(game_family)
        button.setObjectName("gameFamilyButton")
        button.setCheckable(True)
        button.setProperty("game_family", game_family)
        button.clicked.connect(self._on_family_button_clicked)
        self.family_button_group.addButton(button)
        return button

    def _load_sorted_profiles(self) -> list:
        """
        Load DLL-backed profiles in UI display order.
        """
        display_profiles = []

        for profile in get_wall_profiles():
            wall_type = profile["wall_type"]
            if wall_type in AS2_SET1_WALL_TYPES and wall_type != 3:
                continue

            display_profile = dict(profile)
            if wall_type == 3:
                display_profile["short_label"] = "AS2 Wall Set 1"
                display_profile["description"] = "AS2 wall set 1. Choose its variant in the right property panel."

            display_profiles.append(display_profile)

        profiles = sorted(
            display_profiles,
            key=lambda profile: WALL_SET_ORDER.get(profile["wall_type"], profile["wall_type"]),
        )
        return profiles

    def _populate_wall_cards(self) -> None:
        """
        Rebuild wall set cards for the selected game family.
        """
        self._clear_layout(self.content_layout)

        for profile in self.profiles:
            wall_type = profile["wall_type"]
            if not self._profile_belongs_to_current_family(wall_type):
                continue

            preview_key = profile["preview_key"]
            image_path = PREVIEW_IMAGES.get(preview_key)
            self._add_card(
                profile["wall_type"],
                profile["short_label"],
                profile["description"],
                image_path,
            )

        self.content_layout.addStretch(1)

    def _add_card(self, wall_type: int, title_text: str, detail_text: str, image_path: Path | None) -> None:
        card = QFrame()
        card.setObjectName("themeCard")
        card_layout = QVBoxLayout(card)
        card_layout.setContentsMargins(10, 10, 10, 10)
        card_layout.setSpacing(8)

        image = QLabel()
        image.setObjectName("themePreview")
        image.setAlignment(Qt.AlignCenter)
        image.setFixedHeight(118)
        self._load_preview(image, image_path)
        card_layout.addWidget(image)

        title = QLabel(title_text)
        title.setObjectName("cardTitle")
        title.setWordWrap(True)
        card_layout.addWidget(title)

        detail = QLabel(detail_text)
        detail.setObjectName("cardDetail")
        detail.setWordWrap(True)
        card_layout.addWidget(detail)

        button = QPushButton(tr(TextKey.BUTTON_SELECT))
        button.setProperty("wall_type", wall_type)
        button.setProperty("wall_name", title_text)
        button.clicked.connect(self._on_select_button_clicked)
        card_layout.addWidget(button)

        self.content_layout.addWidget(card)

    def _load_preview(self, image: QLabel, image_path: Path | None) -> None:
        image.clear()
        if image_path is not None and image_path.exists():
            pixmap = QPixmap(str(image_path))
            image.setPixmap(pixmap.scaled(260, 118, Qt.KeepAspectRatio, Qt.SmoothTransformation))
            return

        image.setText(tr(TextKey.LABEL_PREVIEW_MISSING))

    def _on_family_button_clicked(self) -> None:
        """
        Switch between AS1 and AS2 wall entries.
        """
        sender = self.sender()
        if sender is None:
            return

        game_family = sender.property("game_family")
        if not game_family:
            return

        if game_family == self.current_game_family:
            return

        self.current_game_family = game_family
        self._sync_family_buttons()
        self.game_family_changed.emit(game_family)
        self._populate_wall_cards()
        self._select_first_wall_set()

    def _on_select_button_clicked(self) -> None:
        """
        Select one wall set card.
        """
        sender = self.sender()
        if sender is None:
            return

        wall_type = sender.property("wall_type")
        wall_name = sender.property("wall_name")
        if wall_type is None or wall_name is None:
            return

        self._select_wall_set(int(wall_type), str(wall_name))

    def _select_first_wall_set(self) -> None:
        """
        Select the first visible wall set after switching game family.
        """
        for profile in self.profiles:
            wall_type = profile["wall_type"]
            if self._profile_belongs_to_current_family(wall_type):
                self._select_wall_set(wall_type, profile["short_label"])
                return

    def _select_wall_set(self, wall_type: int, wall_name: str) -> None:
        self.selected_wall_type = wall_type
        self.wall_set_selected.emit(wall_type, wall_name)

    def _sync_family_buttons(self) -> None:
        """
        Keep the AS family toggle buttons checked without triggering selection.
        """
        self.as1_button.blockSignals(True)
        self.as2_button.blockSignals(True)
        self.as1_button.setChecked(self.current_game_family == GAME_FAMILY_AS1)
        self.as2_button.setChecked(self.current_game_family == GAME_FAMILY_AS2)
        self.as1_button.blockSignals(False)
        self.as2_button.blockSignals(False)

    def _profile_belongs_to_current_family(self, wall_type: int) -> bool:
        if self.current_game_family == GAME_FAMILY_AS1:
            return wall_type in AS1_WALL_TYPES

        return wall_type in AS2_WALL_TYPES

    def _get_game_family_for_wall_type(self, wall_type: int) -> str:
        if wall_type in AS2_WALL_TYPES or wall_type in AS2_SET1_WALL_TYPES:
            return GAME_FAMILY_AS2

        return GAME_FAMILY_AS1

    def _clear_layout(self, layout: QVBoxLayout) -> None:
        """
        Remove old wall cards before rebuilding the shelf.
        """
        while layout.count() > 0:
            item = layout.takeAt(0)
            widget = item.widget()
            if widget is not None:
                widget.setParent(None)
                widget.deleteLater()
