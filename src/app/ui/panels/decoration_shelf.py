"""
Decoration shelf panel for choosing indoor decoration tools.
"""

from PySide6.QtCore import Signal, Qt
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QFrame, QLabel, QPushButton, QScrollArea, QVBoxLayout, QWidget

from app.config import APP_RESOURCE_DIR
from app.editor.decorations import get_decoration_items
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey


IMAGE_ROOT = APP_RESOURCE_DIR / "images" / "preview" / "AS1"


class DecorationShelfPanel(QWidget):
    """
    Large-card decoration selector.
    """

    decoration_selected = Signal(str, str)

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("decorationShelfPanel")

        layout = QVBoxLayout(self)
        layout.setContentsMargins(14, 8, 14, 14)
        layout.setSpacing(12)

        title = QLabel(tr(TextKey.PANEL_DECORATIONS))
        title.setObjectName("panelTitle")
        layout.addWidget(title)

        scroll_area = QScrollArea()
        scroll_area.setObjectName("themeShelfScrollArea")
        scroll_area.setWidgetResizable(True)
        scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)

        content = QWidget()
        content.setObjectName("themeShelfScrollContent")
        content_layout = QVBoxLayout(content)
        content_layout.setContentsMargins(0, 0, 0, 0)
        content_layout.setSpacing(12)

        for decoration in get_decoration_items():
            self._add_decoration_card(
                content_layout,
                decoration["decoration_type"],
                decoration["label"],
                decoration["description"],
                decoration.get("preview_path"),
            )

        content_layout.addStretch(1)
        scroll_area.setWidget(content)
        layout.addWidget(scroll_area)

    def _add_decoration_card(
        self,
        layout: QVBoxLayout,
        decoration_type: str,
        title_text: str,
        detail_text: str,
        preview_path: str | None,
    ) -> None:
        """
        Add one decoration selector card.
        """
        card = QFrame()
        card.setObjectName("themeCard")
        card_layout = QVBoxLayout(card)
        card_layout.setContentsMargins(10, 10, 10, 10)
        card_layout.setSpacing(8)

        preview = QLabel()
        preview.setObjectName("themePreview")
        preview.setAlignment(Qt.AlignCenter)
        preview.setFixedHeight(118)
        self._load_preview(preview, preview_path)
        card_layout.addWidget(preview)

        title = QLabel(title_text)
        title.setObjectName("cardTitle")
        title.setWordWrap(True)
        card_layout.addWidget(title)

        detail = QLabel(detail_text)
        detail.setObjectName("cardDetail")
        detail.setWordWrap(True)
        card_layout.addWidget(detail)

        button = QPushButton(tr(TextKey.BUTTON_SELECT))
        button.clicked.connect(lambda checked=False: self._select_decoration(decoration_type, title_text))
        card_layout.addWidget(button)

        layout.addWidget(card)

    def _load_preview(self, preview: QLabel, preview_path: str | None) -> None:
        """
        Load the decoration preview image when the asset is available.
        """
        image_path = IMAGE_ROOT / preview_path if preview_path else None
        if image_path is not None and image_path.exists():
            pixmap = QPixmap(str(image_path))
            preview.setPixmap(pixmap.scaled(260, 118, Qt.KeepAspectRatio, Qt.SmoothTransformation))
            return

        preview.clear()

    def _select_decoration(self, decoration_type: str, decoration_name: str) -> None:
        """
        Notify listeners that a decoration tool is active.
        """
        self.decoration_selected.emit(decoration_type, decoration_name)
