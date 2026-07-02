"""
Wall set shelf panel for choosing a C++ supported wall profile.
"""
from pathlib import Path

from PySide6.QtCore import Signal, Qt
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QFrame, QLabel, QPushButton, QVBoxLayout, QWidget

from app.editor.wall_profiles import get_default_wall_type, get_wall_profiles
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey


PROJECT_ROOT = Path(__file__).resolve().parents[4]
DEMO_IMAGE_ROOT = PROJECT_ROOT / "_ui_html_demo"

PREVIEW_IMAGES = {
    "base": DEMO_IMAGE_ROOT / "base_preview.png",
    "lab": DEMO_IMAGE_ROOT / "wall_preview.png",
}


class ThemeShelfPanel(QWidget):
    """
    Large-card wall set selector.
    """

    wall_set_selected = Signal(int, str)

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("themeShelfPanel")
        self.selected_wall_type = get_default_wall_type()

        layout = QVBoxLayout(self)
        layout.setContentsMargins(14, 14, 14, 14)
        layout.setSpacing(12)

        title = QLabel(tr(TextKey.PANEL_WALL_SETS))
        title.setObjectName("panelTitle")
        layout.addWidget(title)

        for profile in get_wall_profiles():
            preview_key = profile["preview_key"]
            image_path = PREVIEW_IMAGES.get(preview_key)
            self._add_card(
                layout,
                profile["wall_type"],
                profile["short_label"],
                profile["description"],
                image_path,
            )

        layout.addStretch(1)

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
        self._load_preview(image, image_path)
        card_layout.addWidget(image)

        title = QLabel(title_text)
        title.setObjectName("cardTitle")
        card_layout.addWidget(title)

        detail = QLabel(detail_text)
        detail.setObjectName("cardDetail")
        detail.setWordWrap(True)
        card_layout.addWidget(detail)

        button = QPushButton(tr(TextKey.BUTTON_SELECT))
        button.clicked.connect(lambda checked=False: self._select_wall_set(wall_type, title_text))
        card_layout.addWidget(button)

        layout.addWidget(card)

    def _load_preview(self, image: QLabel, image_path: Path | None) -> None:
        if image_path is not None and image_path.exists():
            pixmap = QPixmap(str(image_path))
            image.setPixmap(pixmap.scaled(260, 118, Qt.KeepAspectRatioByExpanding, Qt.SmoothTransformation))
            return

        image.setText(tr(TextKey.LABEL_PREVIEW_MISSING))

    def _select_wall_set(self, wall_type: int, wall_name: str) -> None:
        self.selected_wall_type = wall_type
        self.wall_set_selected.emit(wall_type, wall_name)
