"""
Wall set shelf panel for choosing a C++ supported wall profile.
"""
from pathlib import Path

from PySide6.QtCore import Signal, Qt
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QFrame, QLabel, QPushButton, QVBoxLayout, QWidget

from app.editor.wall_profiles import WALL_TYPE_LAB, WALL_TYPE_STANDARD, get_wall_profile


PROJECT_ROOT = Path(__file__).resolve().parents[4]
DEMO_IMAGE_ROOT = PROJECT_ROOT / "_ui_html_demo"


class ThemeShelfPanel(QWidget):
    """
    Large-card wall set selector.
    """

    wall_set_selected = Signal(int, str)

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("themeShelfPanel")
        self.selected_wall_type = WALL_TYPE_STANDARD

        layout = QVBoxLayout(self)
        layout.setContentsMargins(14, 14, 14, 14)
        layout.setSpacing(12)

        title = QLabel("Wall Sets")
        title.setObjectName("panelTitle")
        layout.addWidget(title)

        standard_profile = get_wall_profile(WALL_TYPE_STANDARD)
        lab_profile = get_wall_profile(WALL_TYPE_LAB)

        self._add_card(
            layout,
            WALL_TYPE_STANDARD,
            standard_profile["short_label"],
            standard_profile["description"],
            DEMO_IMAGE_ROOT / "base_preview.png",
        )
        self._add_card(
            layout,
            WALL_TYPE_LAB,
            lab_profile["short_label"],
            lab_profile["description"],
            DEMO_IMAGE_ROOT / "wall_preview.png",
        )

        layout.addStretch(1)

    def _add_card(
        self,
        layout: QVBoxLayout,
        wall_type: int,
        title_text: str,
        detail_text: str,
        image_path: Path,
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

        button = QPushButton("Select")
        button.clicked.connect(lambda checked=False: self._select_wall_set(wall_type, title_text))
        card_layout.addWidget(button)

        layout.addWidget(card)

    def _load_preview(self, image: QLabel, image_path: Path) -> None:
        if image_path.exists():
            pixmap = QPixmap(str(image_path))
            image.setPixmap(pixmap.scaled(260, 118, Qt.KeepAspectRatioByExpanding, Qt.SmoothTransformation))
            return

        image.setText("Preview missing")

    def _select_wall_set(self, wall_type: int, wall_name: str) -> None:
        self.selected_wall_type = wall_type
        self.wall_set_selected.emit(wall_type, wall_name)
