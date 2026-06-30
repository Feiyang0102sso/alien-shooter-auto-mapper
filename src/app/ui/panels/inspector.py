"""
Inspector panel for the selected theme and component placeholder data.
"""
from PySide6.QtCore import Signal
from PySide6.QtWidgets import QComboBox, QDoubleSpinBox, QFormLayout, QGroupBox, QLabel, QPushButton, QVBoxLayout, QWidget

from app.drawable_parts import get_drawable_parts
from app.project_data import DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y
from app.wall_profiles import WALL_TYPE_STANDARD


class InspectorPanel(QWidget):
    """
    First-pass right panel for component selection and VID display.
    """

    map_size_applied = Signal(float, float)
    drawable_part_changed = Signal(str)

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("inspectorPanel")

        layout = QVBoxLayout(self)
        layout.setContentsMargins(14, 14, 14, 14)
        layout.setSpacing(12)

        title = QLabel("Inspector")
        title.setObjectName("panelTitle")
        layout.addWidget(title)

        self.theme_label = QLabel("Standard Wall")
        self.theme_label.setObjectName("activeThemeLabel")
        layout.addWidget(self.theme_label)

        self.component_combo = QComboBox()
        self._drawable_part_ids = []
        self.component_combo.currentIndexChanged.connect(self._emit_drawable_part_changed)
        layout.addWidget(self.component_combo)
        self.set_wall_set(WALL_TYPE_STANDARD, "Standard Wall")

        map_size_group = QGroupBox("Map Size")
        map_size_group.setObjectName("mapSizeGroup")
        map_size_layout = QVBoxLayout(map_size_group)
        map_size_layout.setContentsMargins(10, 12, 10, 10)
        map_size_layout.setSpacing(8)

        map_size_form = QFormLayout()
        map_size_form.setSpacing(8)

        self.map_size_x_input = QDoubleSpinBox()
        self.map_size_x_input.setRange(1.0, 100000.0)
        self.map_size_x_input.setDecimals(1)
        self.map_size_x_input.setSingleStep(50.0)
        self.map_size_x_input.setValue(DEFAULT_MAP_SIZE_X)

        self.map_size_y_input = QDoubleSpinBox()
        self.map_size_y_input.setRange(1.0, 100000.0)
        self.map_size_y_input.setDecimals(1)
        self.map_size_y_input.setSingleStep(50.0)
        self.map_size_y_input.setValue(DEFAULT_MAP_SIZE_Y)

        map_size_form.addRow("Map Size X", self.map_size_x_input)
        map_size_form.addRow("Map Size Y", self.map_size_y_input)
        map_size_layout.addLayout(map_size_form)

        self.apply_map_size_button = QPushButton("Apply Map Size")
        self.apply_map_size_button.clicked.connect(self._emit_map_size_applied)
        map_size_layout.addWidget(self.apply_map_size_button)

        layout.addWidget(map_size_group)

        self.preview = QLabel("Component preview")
        self.preview.setObjectName("componentPreview")
        self.preview.setMinimumHeight(170)
        layout.addWidget(self.preview)

        form = QFormLayout()
        form.setSpacing(8)
        form.addRow("Main VID", QLabel("pending"))
        form.addRow("Side VID", QLabel("pending"))
        form.addRow("Corner VID", QLabel("pending"))
        form.addRow("Z Offset", QLabel("pending"))
        layout.addLayout(form)

        layout.addStretch(1)

    def set_theme(self, theme_name: str) -> None:
        """
        Update the active theme label.
        """
        self.theme_label.setText(theme_name)

    def set_wall_set(self, wall_type: int, wall_name: str) -> None:
        """
        Update active wall set and its drawable part choices.
        """
        self.theme_label.setText(wall_name)

        self.component_combo.blockSignals(True)
        self.component_combo.clear()
        self._drawable_part_ids = []

        for part_id, label in get_drawable_parts(wall_type):
            self._drawable_part_ids.append(part_id)
            self.component_combo.addItem(label)

        self.component_combo.setCurrentIndex(0)
        self.component_combo.blockSignals(False)

        if self._drawable_part_ids:
            self.drawable_part_changed.emit(self._drawable_part_ids[0])

    def set_map_size(self, map_size_x: float, map_size_y: float) -> None:
        """
        Update map size controls from imported project data.
        """
        self.map_size_x_input.setValue(map_size_x)
        self.map_size_y_input.setValue(map_size_y)

    def get_map_size(self) -> tuple:
        """
        Return the current map size from the controls.
        """
        map_size_x = self.map_size_x_input.value()
        map_size_y = self.map_size_y_input.value()
        return map_size_x, map_size_y

    def _emit_map_size_applied(self) -> None:
        """
        Notify the main window that map size should update the canvas.
        """
        map_size = self.get_map_size()
        self.map_size_applied.emit(map_size[0], map_size[1])

    def _emit_drawable_part_changed(self, index: int) -> None:
        """
        Notify the main window that the active drawable part changed.
        """
        if index < 0:
            return
        if index >= len(self._drawable_part_ids):
            return

        part_id = self._drawable_part_ids[index]
        self.drawable_part_changed.emit(part_id)
