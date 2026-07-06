"""
Wall set shelf panel for choosing a C++ supported wall profile.
"""
from pathlib import Path

from PySide6.QtCore import Signal, Qt
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QFrame, QLabel, QPushButton, QScrollArea, QVBoxLayout, QWidget

from app.config import APP_RESOURCE_DIR
from app.editor.decorations import get_decoration_items
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
}


class ThemeShelfPanel(QWidget):
    """
    Large-card wall set selector.
    """

    wall_set_selected = Signal(int, str)
    decoration_selected = Signal(str, str)

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("themeShelfPanel")
        self.selected_wall_type = get_default_wall_type()

        layout = QVBoxLayout(self)
        layout.setContentsMargins(14, 8, 14, 14)
        layout.setSpacing(12)

        title = QLabel(tr(TextKey.PANEL_WALL_SETS))
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

        profiles = sorted(
            get_wall_profiles(),
            key=lambda profile: WALL_SET_ORDER.get(profile["wall_type"], profile["wall_type"]),
        )

        for profile in profiles:
            preview_key = profile["preview_key"]
            image_path = PREVIEW_IMAGES.get(preview_key)
            self._add_card(
                content_layout,
                profile["wall_type"],
                profile["short_label"],
                profile["description"],
                image_path,
            )

        decoration_title = QLabel("Decorations")
        decoration_title.setObjectName("panelTitle")
        content_layout.addWidget(decoration_title)

        for decoration in get_decoration_items():
            self._add_decoration_card(
                content_layout,
                decoration["decoration_type"],
                decoration["label"],
                decoration["description"],
            )

        content_layout.addStretch(1)
        scroll_area.setWidget(content)
        layout.addWidget(scroll_area)

    def _add_card(
        self,
        layout: QVBoxLayout,
        wall_type: int,
        title_text: str,
        detail_text: str,
        image_path: Path | None,
    ) -> None:
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
        button.clicked.connect(lambda checked=False: self._select_wall_set(wall_type, title_text))
        card_layout.addWidget(button)

        layout.addWidget(card)

    def _add_decoration_card(
        self,
        layout: QVBoxLayout,
        decoration_type: str,
        title_text: str,
        detail_text: str,
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
        preview.setObjectName("decorationPreview")
        preview.setAlignment(Qt.AlignCenter)
        preview.setFixedHeight(118)
        preview.setText("80 x 42\nplaceholder")
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

    def _load_preview(self, image: QLabel, image_path: Path | None) -> None:
        if image_path is not None and image_path.exists():
            pixmap = QPixmap(str(image_path))
            image.setPixmap(pixmap.scaled(260, 118, Qt.KeepAspectRatio, Qt.SmoothTransformation))
            return

        image.setText(tr(TextKey.LABEL_PREVIEW_MISSING))

    def _select_wall_set(self, wall_type: int, wall_name: str) -> None:
        self.selected_wall_type = wall_type
        self.wall_set_selected.emit(wall_type, wall_name)

    def _select_decoration(self, decoration_type: str, decoration_name: str) -> None:
        """
        Notify listeners that a decoration tool is active.
        """
        self.decoration_selected.emit(decoration_type, decoration_name)
