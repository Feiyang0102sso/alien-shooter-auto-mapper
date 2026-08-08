"""
Wall set shelf panel for choosing a C++ supported wall profile.
"""
from pathlib import Path

from PySide6.QtCore import QPointF, Signal, Qt
from PySide6.QtGui import QBrush, QColor, QPainter, QPen, QPixmap
from PySide6.QtWidgets import (
    QButtonGroup,
    QFrame,
    QHBoxLayout,
    QLabel,
    QLayout,
    QPushButton,
    QScrollArea,
    QSizePolicy,
    QVBoxLayout,
    QWidget,
)

from app.config import APP_RESOURCE_DIR
from app.editor.drawable_parts import AS2_SET1_WALL_TYPES, PART_WALL_BODY, get_wall_set_drawable_parts
from app.editor.wall_profiles import get_default_wall_type, get_wall_profile, get_wall_profiles
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey
from app.ui.colors import (
    DOOR_AS2_DOT,
    DOOR_AS2_LINE,
    DOOR_LAB_DECO_DOT,
    DOOR_LAB_DECO_LINE,
    DOOR_LAB_LASER_WITH_DOT,
    DOOR_LAB_LASER_WITH_LINE,
    DOOR_LAB_LASER_WITHOUT_DOT,
    DOOR_LAB_LASER_WITHOUT_LINE,
    DOOR_STD_ACTIVE_LINE,
    DOOR_STD_DEAD_CLOSED_DOT,
    DOOR_STD_DEAD_CLOSED_LINE,
    DOOR_STD_DEAD_JAMMED_DOT,
    DOOR_STD_DEAD_JAMMED_LINE,
    DOOR_STD_DEAD_OPEN_DOT,
    DOOR_STD_DEAD_OPEN_LINE,
)


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
GAME_FAMILY_AS1 = "AS1"
GAME_FAMILY_AS2 = "AS2"
LEGEND_SYMBOL_WIDTH = 52
LEGEND_SYMBOL_HEIGHT = 18
LEGEND_LINE_START_X = 3
LEGEND_LINE_END_X = 49
LEGEND_DOT_RADIUS = 5

LEGEND_DOOR_STYLES = {
    "active_door": (DOOR_STD_ACTIVE_LINE, DOOR_STD_ACTIVE_LINE, True),
    "dead_door_closed": (DOOR_STD_DEAD_CLOSED_LINE, DOOR_STD_DEAD_CLOSED_DOT, False),
    "dead_door_jammed": (DOOR_STD_DEAD_JAMMED_LINE, DOOR_STD_DEAD_JAMMED_DOT, False),
    "dead_door_open": (DOOR_STD_DEAD_OPEN_LINE, DOOR_STD_DEAD_OPEN_DOT, True),
    "lab_laser_closed": (DOOR_LAB_LASER_WITH_LINE, DOOR_LAB_LASER_WITH_DOT, False),
    "lab_laser_open": (DOOR_LAB_LASER_WITHOUT_LINE, DOOR_LAB_LASER_WITHOUT_DOT, True),
    "lab_decoration_door": (DOOR_LAB_DECO_LINE, DOOR_LAB_DECO_DOT, False),
    "as2_door_closed": (DOOR_AS2_LINE, DOOR_AS2_DOT, False),
    "as2_door_open": (DOOR_AS2_LINE, DOOR_AS2_DOT, True),
}


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
        self.content_layout.setSizeConstraint(QLayout.SetMinAndMaxSize)
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
        card.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Maximum)
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

        self._add_wall_set_legend(card_layout, wall_type)

        button = QPushButton(tr(TextKey.BUTTON_SELECT))
        button.setProperty("wall_type", wall_type)
        button.setProperty("wall_name", title_text)
        button.clicked.connect(self._on_select_button_clicked)
        card_layout.addWidget(button)

        self.content_layout.addWidget(card, 0, Qt.AlignTop)

    def _add_wall_set_legend(self, card_layout: QVBoxLayout, wall_type: int) -> None:
        """
        Add a non-interactive list of every drawable part in the wall set.
        """
        legend = QWidget()
        legend.setObjectName("wallSetLegend")
        legend_layout = QVBoxLayout(legend)
        legend_layout.setContentsMargins(0, 0, 0, 0)
        legend_layout.setSpacing(2)

        for drawable_wall_type, part_id, label_text in get_wall_set_drawable_parts(wall_type):
            line_color = get_wall_profile(drawable_wall_type)["color"]
            dot_color = None
            hollow_dot = False

            if part_id != PART_WALL_BODY:
                line_color, dot_color, hollow_dot = LEGEND_DOOR_STYLES[part_id]

            row_layout = QHBoxLayout()
            row_layout.setContentsMargins(0, 0, 0, 0)
            row_layout.setSpacing(6)

            symbol = QLabel()
            symbol.setFixedSize(LEGEND_SYMBOL_WIDTH, LEGEND_SYMBOL_HEIGHT)
            symbol.setPixmap(self._draw_legend_symbol(line_color, dot_color, hollow_dot))
            row_layout.addWidget(symbol)

            label = QLabel(label_text)
            label.setObjectName("wallSetLegendLabel")
            label.setWordWrap(True)
            row_layout.addWidget(label, 1)

            legend_layout.addLayout(row_layout)

        card_layout.addWidget(legend)

    def _draw_legend_symbol(self, line_color: str, dot_color: str | None, hollow_dot: bool) -> QPixmap:
        """
        Draw one wall or door legend symbol as a line with an optional center dot.
        """
        pixmap = QPixmap(LEGEND_SYMBOL_WIDTH, LEGEND_SYMBOL_HEIGHT)
        pixmap.fill(Qt.transparent)

        painter = QPainter(pixmap)
        painter.setRenderHint(QPainter.Antialiasing)

        center_y = LEGEND_SYMBOL_HEIGHT / 2.0
        line_width = 4
        if dot_color is not None:
            line_width = 5

        line_pen = QPen(QColor(line_color))
        line_pen.setWidth(line_width)
        painter.setPen(line_pen)
        painter.drawLine(
            QPointF(LEGEND_LINE_START_X, center_y),
            QPointF(LEGEND_LINE_END_X, center_y),
        )

        if dot_color is not None:
            center_x = LEGEND_SYMBOL_WIDTH / 2.0

            # Clear the line below a hollow marker so it matches the canvas door symbol.
            if hollow_dot:
                painter.setCompositionMode(QPainter.CompositionMode_Clear)
                painter.setPen(Qt.NoPen)
                painter.setBrush(QBrush(QColor("#000000")))
                painter.drawEllipse(QPointF(center_x, center_y), LEGEND_DOT_RADIUS, LEGEND_DOT_RADIUS)
                painter.setCompositionMode(QPainter.CompositionMode_SourceOver)

            dot_pen = QPen(QColor(line_color))
            dot_pen.setWidth(2)
            painter.setPen(dot_pen)

            if hollow_dot:
                painter.setBrush(Qt.NoBrush)
            else:
                painter.setBrush(QBrush(QColor(dot_color)))

            painter.drawEllipse(QPointF(center_x, center_y), LEGEND_DOT_RADIUS, LEGEND_DOT_RADIUS)

        painter.end()
        return pixmap

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
