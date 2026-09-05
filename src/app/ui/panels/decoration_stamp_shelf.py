"""
Decoration stamp shelf panel for choosing an authored room stamp series.
"""

from functools import partial

from PySide6.QtCore import Qt, Signal
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import (
    QFrame,
    QLabel,
    QLayout,
    QPushButton,
    QScrollArea,
    QVBoxLayout,
    QWidget,
)

from app.editor.decoration_stamps import get_decoration_stamp_series_items
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey
from app.ui.previews.decoration_assets import get_decoration_stamp_series_thumbnail_path


STAMP_PREVIEW_HEIGHT = 118
STAMP_PREVIEW_WIDTH = 260


class DecorationStampShelfPanel(QWidget):
    """
    Large-card selector for authored decoration stamp series.

    One card stands for a whole series; the member stamps are picked from the
    inspector dropdown, the same way AS2 Wall Set 1 exposes its wall variants.
    """

    stamp_selected = Signal(str, str)

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("decorationStampShelfPanel")
        self.series_items = get_decoration_stamp_series_items()
        self.selected_series_id = None
        self._cards_by_series_id = {}

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(12)

        self.scroll_area = QScrollArea()
        self.scroll_area.setObjectName("themeShelfScrollArea")
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)

        self.content = QWidget()
        self.content.setObjectName("themeShelfScrollContent")
        self.content_layout = QVBoxLayout(self.content)
        self.content_layout.setContentsMargins(0, 0, 0, 0)
        self.content_layout.setSpacing(12)
        self.content_layout.setSizeConstraint(QLayout.SetMinAndMaxSize)
        self.scroll_area.setWidget(self.content)
        layout.addWidget(self.scroll_area)

        self._populate_series_cards()

    def _populate_series_cards(self) -> None:
        """
        Fill the shelf with one card per authored stamp series.
        """
        for series in self.series_items:
            self._add_card(series)

        self.content_layout.addStretch(1)

    def _add_card(self, series: dict) -> None:
        """
        Add one decoration stamp series selector card.
        """
        card = QFrame()
        card.setObjectName("themeCard")
        card.setProperty("selected", False)
        card_layout = QVBoxLayout(card)
        card_layout.setContentsMargins(10, 10, 10, 10)
        card_layout.setSpacing(8)

        preview = QLabel()
        preview.setObjectName("themePreview")
        preview.setAlignment(Qt.AlignCenter)
        preview.setFixedHeight(STAMP_PREVIEW_HEIGHT)
        thumbnail_path = get_decoration_stamp_series_thumbnail_path(series["series_id"])
        if thumbnail_path.is_file():
            thumbnail = QPixmap(str(thumbnail_path))
            preview.setPixmap(
                thumbnail.scaled(
                    STAMP_PREVIEW_WIDTH,
                    STAMP_PREVIEW_HEIGHT,
                    Qt.KeepAspectRatio,
                    Qt.SmoothTransformation,
                )
            )
        card_layout.addWidget(preview)

        title = QLabel(series["label"])
        title.setObjectName("cardTitle")
        title.setWordWrap(True)
        card_layout.addWidget(title)

        detail = QLabel(series["description"])
        detail.setObjectName("cardDetail")
        detail.setWordWrap(True)
        card_layout.addWidget(detail)

        variant_text = tr(
            TextKey.LABEL_STAMP_VARIANT_COUNT,
            variant_count=series["variant_count"],
        )
        variant_label = QLabel(variant_text)
        variant_label.setObjectName("cardDetail")
        variant_label.setWordWrap(True)
        card_layout.addWidget(variant_label)

        # Selecting the card arms the first stamp of the series.
        default_profile_id = series["default_profile_id"]
        series_label = series["label"]
        button = QPushButton(tr(TextKey.BUTTON_SELECT))
        button.clicked.connect(
            partial(self._select_series, series["series_id"], default_profile_id, series_label)
        )
        card_layout.addWidget(button)

        self.content_layout.addWidget(card)
        self._cards_by_series_id[series["series_id"]] = card

    def ensure_selection(self) -> None:
        """Select the first series the first time this shelf becomes active."""
        if self.selected_series_id is not None:
            self.activate_selected_series()
            return
        if not self.series_items:
            return

        first_series = self.series_items[0]
        self._select_series(
            first_series["series_id"],
            first_series["default_profile_id"],
            first_series["label"],
        )

    def activate_selected_series(self) -> None:
        """Re-emit the remembered series when its shelf becomes active."""
        for series in self.series_items:
            if series["series_id"] != self.selected_series_id:
                continue

            self._select_series(
                series["series_id"],
                series["default_profile_id"],
                series["label"],
            )
            return

        self.selected_series_id = None
        self.ensure_selection()

    def select_relative_series(self, step: int) -> None:
        """Select an adjacent decoration series without wrapping."""
        if self.selected_series_id is None:
            self.ensure_selection()
            return

        current_index = -1
        index = 0
        for series in self.series_items:
            if series["series_id"] == self.selected_series_id:
                current_index = index
                break
            index += 1

        if current_index < 0:
            self.selected_series_id = None
            self.ensure_selection()
            return

        next_index = current_index + step
        if next_index < 0:
            return
        if next_index >= len(self.series_items):
            return

        next_series = self.series_items[next_index]
        self._select_series(
            next_series["series_id"],
            next_series["default_profile_id"],
            next_series["label"],
        )

    def _select_series(
        self,
        series_id: str,
        default_profile_id: str,
        series_label: str,
        checked: bool = False,
    ) -> None:
        """Select, highlight and emit one decoration stamp series."""
        del checked
        self.selected_series_id = series_id
        self._update_card_selection()
        self.stamp_selected.emit(default_profile_id, series_label)

    def _update_card_selection(self) -> None:
        """Highlight the selected card and keep it visible."""
        selected_card = None
        for series_id, card in self._cards_by_series_id.items():
            is_selected = series_id == self.selected_series_id
            card.setProperty("selected", is_selected)
            card.style().unpolish(card)
            card.style().polish(card)
            card.update()
            if is_selected:
                selected_card = card

        if selected_card is not None:
            self.scroll_area.ensureWidgetVisible(selected_card)
