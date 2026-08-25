"""Inspector controls for AS1 ceiling-layer counts."""

from PySide6.QtCore import Qt, Signal
from PySide6.QtWidgets import (
    QGroupBox,
    QHBoxLayout,
    QLabel,
    QSlider,
    QSpinBox,
    QVBoxLayout,
    QWidget,
)

from app.i18n.locale import tr
from app.i18n.text_keys import TextKey
from app.project.data import PROJECT_VERSION_AS1


class CeilingLayerControl(QWidget):
    """Keep one ceiling-layer slider and spinbox synchronized."""

    value_changed = Signal(int)

    def __init__(self, label_text: str, minimum: int, maximum: int, value: int) -> None:
        super().__init__()

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(4)

        label = QLabel(label_text)
        layout.addWidget(label)

        row = QHBoxLayout()
        row.setSpacing(8)

        self.slider = QSlider(Qt.Horizontal)
        self.slider.setRange(minimum, maximum)
        self.slider.setValue(value)

        self.spinbox = QSpinBox()
        self.spinbox.setRange(minimum, maximum)
        self.spinbox.setValue(value)

        self.slider.valueChanged.connect(self._on_slider_changed)
        self.spinbox.valueChanged.connect(self._on_spinbox_changed)

        row.addWidget(self.slider, stretch=1)
        row.addWidget(self.spinbox)
        layout.addLayout(row)

    def value(self) -> int:
        """Return the selected total layer count."""
        return self.slider.value()

    def set_value(self, value: int) -> None:
        """Update both controls without emitting a user-change signal."""
        self.slider.blockSignals(True)
        self.spinbox.blockSignals(True)
        self.slider.setValue(value)
        self.spinbox.setValue(value)
        self.slider.blockSignals(False)
        self.spinbox.blockSignals(False)

    def _on_slider_changed(self, value: int) -> None:
        """Sync the spinbox after slider movement."""
        self.spinbox.blockSignals(True)
        self.spinbox.setValue(value)
        self.spinbox.blockSignals(False)
        self.value_changed.emit(value)

    def _on_spinbox_changed(self, value: int) -> None:
        """Sync the slider after direct numeric input."""
        self.slider.blockSignals(True)
        self.slider.setValue(value)
        self.slider.blockSignals(False)
        self.value_changed.emit(value)


class AS1CeilingPropertiesWidget(QGroupBox):
    """Show project-wide Standard and Lab ceiling-layer settings for AS1."""

    layers_changed = Signal(int, int)

    def __init__(self, config: dict, parent=None) -> None:
        super().__init__(tr(TextKey.GROUP_AS1_CEILING), parent)
        self.setObjectName("as1CeilingPropertiesGroup")
        self.config = dict(config)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 12, 10, 10)
        layout.setSpacing(8)

        minimum = self.config["min_layer_count"]
        maximum = self.config["max_layer_count"]

        self.standard_control = CeilingLayerControl(
            tr(TextKey.LABEL_AS1_STANDARD_CEILING_LAYERS),
            minimum,
            maximum,
            self.config["default_standard_layer_count"],
        )
        self.standard_control.setObjectName("as1StandardCeilingLayerControl")
        self.standard_control.value_changed.connect(self._emit_layers_changed)
        layout.addWidget(self.standard_control)

        self.lab_control = CeilingLayerControl(
            tr(TextKey.LABEL_AS1_LAB_CEILING_LAYERS),
            minimum,
            maximum,
            self.config["default_lab_layer_count"],
        )
        self.lab_control.setObjectName("as1LabCeilingLayerControl")
        self.lab_control.value_changed.connect(self._emit_layers_changed)
        layout.addWidget(self.lab_control)

        self.setToolTip(tr(TextKey.TOOLTIP_AS1_CEILING_LAYERS))

    def get_layer_counts(self) -> tuple:
        """Return Standard and Lab total layer counts."""
        return self.standard_control.value(), self.lab_control.value()

    def set_layer_counts(self, standard_layer_count: int, lab_layer_count: int) -> None:
        """Replace both layer counts from imported project data."""
        self.standard_control.set_value(standard_layer_count)
        self.lab_control.set_value(lab_layer_count)

    def reset_layer_counts(self) -> None:
        """Restore both layer counts from DLL-provided constants."""
        self.set_layer_counts(
            self.config["default_standard_layer_count"],
            self.config["default_lab_layer_count"],
        )

    def set_project_state(self, project_version: str, generate_ceiling: bool) -> None:
        """Apply AS1-only visibility and the ceiling master switch state."""
        is_as1_project = project_version == PROJECT_VERSION_AS1
        self.setVisible(is_as1_project)
        self.setEnabled(is_as1_project and generate_ceiling)

    def _emit_layers_changed(self) -> None:
        """Publish both values after either child control changes."""
        layer_counts = self.get_layer_counts()
        self.layers_changed.emit(layer_counts[0], layer_counts[1])
