"""
Decoration stamp shelf panel for choosing an authored room stamp.
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

from app.editor.decoration_stamps import get_decoration_stamp_items
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey


# Previews are not authored yet, so cards reserve the space and stay empty.
STAMP_PREVIEW_HEIGHT = 118


class DecorationStampShelfPanel(QWidget):
    """
    Large-card selector for authored decoration stamps.
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

        self._populate_stamp_cards()

    def _populate_stamp_cards(self) -> None:
        """
        Fill the shelf with one card per authored stamp.
        """
        for stamp in get_decoration_stamp_items():
            self._add_card(stamp)

        self.content_layout.addStretch(1)

    def _add_card(self, stamp: dict) -> None:
        """
        Add one decoration stamp selector card.
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

        title = QLabel(stamp["label"])
        title.setObjectName("cardTitle")
        title.setWordWrap(True)
        card_layout.addWidget(title)

        detail = QLabel(stamp["description"])
        detail.setObjectName("cardDetail")
        detail.setWordWrap(True)
        card_layout.addWidget(detail)

        member_text = tr(TextKey.LABEL_STAMP_MEMBER_COUNT, member_count=stamp["member_count"])
        member_label = QLabel(member_text)
        member_label.setObjectName("cardDetail")
        member_label.setWordWrap(True)
        card_layout.addWidget(member_label)

        profile_id = stamp["profile_id"]
        stamp_label = stamp["label"]
        button = QPushButton(tr(TextKey.BUTTON_SELECT))
        button.clicked.connect(
            lambda checked=False: self.stamp_selected.emit(profile_id, stamp_label)
        )
        card_layout.addWidget(button)

        self.content_layout.addWidget(card)
