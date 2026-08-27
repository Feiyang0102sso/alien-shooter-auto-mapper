"""
Decoration stamp shelf panel for choosing an authored room stamp series.
"""

from PySide6.QtCore import Qt, Signal
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


# Previews are not authored yet, so cards reserve the space and stay empty.
STAMP_PREVIEW_HEIGHT = 118


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

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(12)

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

        self._populate_series_cards()

    def _populate_series_cards(self) -> None:
        """
        Fill the shelf with one card per authored stamp series.
        """
        for series in get_decoration_stamp_series_items():
            self._add_card(series)

        self.content_layout.addStretch(1)

    def _add_card(self, series: dict) -> None:
        """
        Add one decoration stamp series selector card.
        """
        card = QFrame()
        card.setObjectName("themeCard")
        card_layout = QVBoxLayout(card)
        card_layout.setContentsMargins(10, 10, 10, 10)
        card_layout.setSpacing(8)

        # Preview art is not authored yet; the placeholder keeps card heights even.
        preview = QLabel()
        preview.setObjectName("themePreview")
        preview.setAlignment(Qt.AlignCenter)
        preview.setFixedHeight(STAMP_PREVIEW_HEIGHT)
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
            lambda checked=False: self.stamp_selected.emit(default_profile_id, series_label)
        )
        card_layout.addWidget(button)

        self.content_layout.addWidget(card)
