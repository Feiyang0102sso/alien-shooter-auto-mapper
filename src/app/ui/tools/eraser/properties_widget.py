"""Inspector widget for eraser tool properties."""
from PySide6.QtCore import Qt, Signal
from PySide6.QtWidgets import QGroupBox, QHBoxLayout, QLabel, QSlider, QSpinBox, QVBoxLayout

from app.i18n.locale import tr
from app.i18n.text_keys import TextKey
from app.ui.tools.eraser.config import DEFAULT_ERASER_SIZE, ERASER_SIZE_MAX, ERASER_SIZE_MIN


class EraserPropertiesWidget(QGroupBox):
    """Tool properties panel for the eraser."""

    size_changed = Signal(int)

    def __init__(self, parent=None) -> None:
        super().__init__(tr(TextKey.GROUP_TOOL_PROPERTIES), parent)
        self.setObjectName("toolPropertiesGroup")

        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 12, 10, 10)
        layout.setSpacing(8)

        label = QLabel(tr(TextKey.LABEL_ERASER_SIZE))
        layout.addWidget(label)

        row = QHBoxLayout()
        row.setSpacing(8)

        self.size_slider = QSlider(Qt.Horizontal)
        self.size_slider.setRange(ERASER_SIZE_MIN, ERASER_SIZE_MAX)
        self.size_slider.setValue(DEFAULT_ERASER_SIZE)

        self.size_spinbox = QSpinBox()
        self.size_spinbox.setRange(ERASER_SIZE_MIN, ERASER_SIZE_MAX)
        self.size_spinbox.setValue(DEFAULT_ERASER_SIZE)

        self.size_slider.valueChanged.connect(self._on_slider_changed)
        self.size_spinbox.valueChanged.connect(self._on_spinbox_changed)

        row.addWidget(self.size_slider, stretch=1)
        row.addWidget(self.size_spinbox)
        layout.addLayout(row)

        self.setVisible(False)

    def get_size(self) -> int:
        """Return the current eraser size."""
        return self.size_slider.value()

    def _on_slider_changed(self, value: int) -> None:
        """Sync spinbox when the slider changes."""
        self.size_spinbox.blockSignals(True)
        self.size_spinbox.setValue(value)
        self.size_spinbox.blockSignals(False)
        self.size_changed.emit(value)

    def _on_spinbox_changed(self, value: int) -> None:
        """Sync slider when the spinbox changes."""
        self.size_slider.blockSignals(True)
        self.size_slider.setValue(value)
        self.size_slider.blockSignals(False)
        self.size_changed.emit(value)
