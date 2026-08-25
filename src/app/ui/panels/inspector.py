"""
Inspector panel for the selected theme and component placeholder data.
"""
from PySide6.QtCore import Signal, Qt
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import (
    QComboBox,
    QDoubleSpinBox,
    QFormLayout,
    QGroupBox,
    QLabel,
    QPushButton,
    QVBoxLayout,
    QWidget,
)

from app.editor.drawable_parts import AS2_SET1_WALL_TYPES, get_wall_set_drawable_parts
from app.editor.wall_profiles import get_default_wall_type, get_wall_profile
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey
from app.project.data import DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y
from app.ui.panels.as1_ceiling import AS1CeilingPropertiesWidget
from app.ui.previews.wall_assets import get_wall_component_preview_path
from app.ui.tools.drawing_modes import DrawingMode
from app.ui.tools.eraser import EraserPropertiesWidget

INSPECTOR_MAX_WIDTH = 320
COMPONENT_PREVIEW_WIDTH = 260
COMPONENT_PREVIEW_HEIGHT = 190
class InspectorPanel(QWidget):
    """
    First-pass right panel for component selection and VID display.
    """

    map_size_applied = Signal(float, float)
    drawable_part_changed = Signal(str)
    wall_set_variant_changed = Signal(int, str)
    eraser_size_changed = Signal(int)
    decoration_spacing_changed = Signal(float, float)
    decoration_delete_requested = Signal()

    def __init__(self, as1_ceiling_layer_config: dict) -> None:
        super().__init__()
        self.setObjectName("inspectorPanel")
        self.setMaximumWidth(INSPECTOR_MAX_WIDTH)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(14, 8, 14, 14)
        layout.setSpacing(12)

        title = QLabel(tr(TextKey.PANEL_INSPECTOR))
        title.setObjectName("panelTitle")
        layout.addWidget(title)

        # ── Map Size ──
        map_size_group = QGroupBox(tr(TextKey.GROUP_MAP_SIZE))
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

        map_size_form.addRow(tr(TextKey.LABEL_MAP_SIZE_X), self.map_size_x_input)
        map_size_form.addRow(tr(TextKey.LABEL_MAP_SIZE_Y), self.map_size_y_input)
        map_size_layout.addLayout(map_size_form)

        self.apply_map_size_button = QPushButton(tr(TextKey.BUTTON_APPLY_MAP_SIZE))
        self.apply_map_size_button.clicked.connect(self._emit_map_size_applied)
        map_size_layout.addWidget(self.apply_map_size_button)

        layout.addWidget(map_size_group)

        self.as1_ceiling_properties = AS1CeilingPropertiesWidget(
            as1_ceiling_layer_config
        )
        layout.addWidget(self.as1_ceiling_properties)

        # ── Component Preview (Component Group) ──
        component_group = QGroupBox(tr(TextKey.LABEL_COMPONENT_PREVIEW))
        component_group.setObjectName("componentGroup")
        component_layout = QVBoxLayout(component_group)
        component_layout.setContentsMargins(10, 12, 10, 10)
        component_layout.setSpacing(8)

        default_wall_type = get_default_wall_type()
        default_profile = get_wall_profile(default_wall_type)
        self.current_wall_type = default_wall_type

        self.theme_label = QLabel(default_profile["short_label"])
        self.theme_label.setObjectName("activeThemeLabel")
        component_layout.addWidget(self.theme_label)

        self.component_combo = QComboBox()
        self._drawable_part_ids = []
        self._drawable_wall_types = []
        self.component_combo.currentIndexChanged.connect(self._emit_drawable_part_changed)
        component_layout.addWidget(self.component_combo)

        self.preview = QLabel()
        self.preview.setObjectName("componentPreview")
        self.preview.setFixedSize(COMPONENT_PREVIEW_WIDTH, COMPONENT_PREVIEW_HEIGHT)
        self.preview.setAlignment(Qt.AlignCenter)
        component_layout.addWidget(self.preview)

        self.nvid_label = QLabel()
        self.nvid_label.setObjectName("nvidLabel")
        self.nvid_label.setStyleSheet("font-family: monospace; font-size: 12px; color: #69f0ae; padding-top: 4px;")
        self.nvid_label.setWordWrap(True)
        component_layout.addWidget(self.nvid_label)

        layout.addWidget(component_group)

        decoration_group = QGroupBox(tr(TextKey.GROUP_DECORATION_PROPERTY))
        decoration_group.setObjectName("decorationPropertiesGroup")
        decoration_layout = QVBoxLayout(decoration_group)
        decoration_layout.setContentsMargins(10, 12, 10, 10)
        decoration_layout.setSpacing(8)

        decoration_form = QFormLayout()
        decoration_form.setSpacing(8)

        self.item_spacing_input = QDoubleSpinBox()
        self.item_spacing_input.setRange(0.1, 10.0)
        self.item_spacing_input.setDecimals(2)
        self.item_spacing_input.setSingleStep(0.1)
        self.item_spacing_input.setValue(1.0)
        self.item_spacing_input.valueChanged.connect(self._emit_decoration_spacing_changed)

        self.row_spacing_input = QDoubleSpinBox()
        self.row_spacing_input.setRange(0.1, 10.0)
        self.row_spacing_input.setDecimals(2)
        self.row_spacing_input.setSingleStep(0.1)
        self.row_spacing_input.setValue(1.0)
        self.row_spacing_input.valueChanged.connect(self._emit_decoration_spacing_changed)

        decoration_form.addRow(tr(TextKey.LABEL_ITEM_SPACING), self.item_spacing_input)
        decoration_form.addRow(tr(TextKey.LABEL_COLUMN_SPACING), self.row_spacing_input)
        decoration_layout.addLayout(decoration_form)

        self.delete_decoration_button = QPushButton(tr(TextKey.BUTTON_DELETE_DECORATION))
        self.delete_decoration_button.clicked.connect(self.decoration_delete_requested)
        decoration_layout.addWidget(self.delete_decoration_button)

        self.decoration_properties_group = decoration_group
        layout.addWidget(self.decoration_properties_group)

        self.eraser_properties = EraserPropertiesWidget()
        self.eraser_properties.size_changed.connect(self.eraser_size_changed)
        layout.addWidget(self.eraser_properties)

        layout.addStretch(1)

        # Set initial wall choices after widgets are initialized
        self.set_wall_set(default_wall_type, default_profile["short_label"])
        self.clear_decoration_selection()

    def set_theme(self, theme_name: str) -> None:
        """
        Update the active theme label.
        """
        self.theme_label.setText(theme_name)

    def set_wall_set(self, wall_type: int, wall_name: str) -> None:
        """
        Update active wall set and its drawable part choices.
        """
        self.current_wall_type = wall_type
        self.theme_label.setText(wall_name)
        self.component_combo.setVisible(True)
        self.nvid_label.setVisible(True)

        self._reload_drawable_part_choices(wall_type)

    def set_decoration_tool(self, decoration_type: str, decoration_name: str) -> None:
        """
        Show the active decoration tool in the component preview area.
        """
        self.theme_label.setText(decoration_name)
        self.component_combo.setVisible(False)
        self.nvid_label.setVisible(False)
        self._update_decoration_preview(decoration_type)

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

    def get_eraser_size(self) -> int:
        """
        Return the current eraser size.
        """
        return self.eraser_properties.get_size()

    def get_as1_ceiling_layer_counts(self) -> tuple:
        """Return the project-wide Standard and Lab ceiling layer counts."""
        return self.as1_ceiling_properties.get_layer_counts()

    def set_as1_ceiling_layer_counts(
        self,
        standard_layer_count: int,
        lab_layer_count: int,
    ) -> None:
        """Update layer controls from imported project data."""
        self.as1_ceiling_properties.set_layer_counts(
            standard_layer_count,
            lab_layer_count,
        )

    def reset_as1_ceiling_layer_counts(self) -> None:
        """Restore DLL-provided AS1 ceiling defaults."""
        self.as1_ceiling_properties.reset_layer_counts()

    def set_ceiling_project_state(
        self,
        project_version: str,
        generate_ceiling: bool,
    ) -> None:
        """Apply project version and master-switch state to ceiling controls."""
        self.as1_ceiling_properties.set_project_state(
            project_version,
            generate_ceiling,
        )

    def set_tool_properties_for_mode(self, drawing_mode: DrawingMode) -> None:
        """
        Show tool properties for the active drawing mode.
        """
        self.eraser_properties.setVisible(drawing_mode == DrawingMode.ERASER)

    def set_decoration_selection(self, decoration) -> None:
        """
        Show properties for the selected decoration.
        """
        self.item_spacing_input.blockSignals(True)
        self.row_spacing_input.blockSignals(True)
        self.item_spacing_input.setValue(decoration.item_spacing_scale)
        self.row_spacing_input.setValue(decoration.row_spacing_scale)
        self.item_spacing_input.blockSignals(False)
        self.row_spacing_input.blockSignals(False)
        self.decoration_properties_group.setVisible(True)

    def clear_decoration_selection(self) -> None:
        """
        Hide decoration properties when no decoration is selected.
        """
        self.decoration_properties_group.setVisible(False)

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

        if part_id == "wall_body":
            wall_type = self._drawable_wall_types[index]
            if wall_type != self.current_wall_type:
                self.current_wall_type = wall_type
                variant_label = self.component_combo.currentText()
                self.wall_set_variant_changed.emit(wall_type, variant_label)
        else:
            wall_type = self.current_wall_type

        self._update_preview(wall_type, part_id)
        self.drawable_part_changed.emit(part_id)

    def _emit_decoration_spacing_changed(self) -> None:
        """
        Notify listeners that selected decoration spacing changed.
        """
        item_spacing = self.item_spacing_input.value()
        row_spacing = self.row_spacing_input.value()
        self.decoration_spacing_changed.emit(item_spacing, row_spacing)

    def _update_decoration_preview(self, decoration_type: str) -> None:
        """
        Update component preview for the active decoration tool.
        """
        self.preview.clear()

    def _reload_drawable_part_choices(self, wall_type: int) -> None:
        """
        Reload drawable part choices after a wall set or wall variant changes.
        """
        self.component_combo.blockSignals(True)
        self.component_combo.clear()
        self._drawable_part_ids = []
        self._drawable_wall_types = []

        for drawable_wall_type, part_id, label in get_wall_set_drawable_parts(wall_type):
            self._drawable_part_ids.append(part_id)
            self._drawable_wall_types.append(drawable_wall_type)
            self.component_combo.addItem(label)

        if not self._drawable_part_ids:
            self._drawable_part_ids.append("wall_body")
            self._drawable_wall_types.append(wall_type)
            self.component_combo.addItem(tr(TextKey.DRAWABLE_WALL_BODY))

        selected_index = self._find_drawable_wall_type_index(wall_type)
        self.component_combo.setCurrentIndex(selected_index)
        self.component_combo.blockSignals(False)

        if self._drawable_part_ids:
            part_id = self._drawable_part_ids[selected_index]
            selected_wall_type = self._drawable_wall_types[selected_index]
            self._update_preview(selected_wall_type, part_id)
            self.drawable_part_changed.emit(part_id)

    def _find_drawable_wall_type_index(self, wall_type: int) -> int:
        """
        Return the combo index matching the current wall type.
        """
        index = 0
        while index < len(self._drawable_wall_types):
            if self._drawable_wall_types[index] == wall_type:
                return index
            index += 1

        return 0

    def _update_preview(self, wall_type: int, part_id: str) -> None:
        """
        Update component preview image and nvid labels based on wall type and part ID.
        """
        image_path = get_wall_component_preview_path(wall_type, part_id)

        if image_path and image_path.exists():
            pixmap = QPixmap(str(image_path))
            self.preview.setPixmap(
                pixmap.scaled(
                    COMPONENT_PREVIEW_WIDTH,
                    COMPONENT_PREVIEW_HEIGHT,
                    Qt.KeepAspectRatio,
                    Qt.SmoothTransformation
                )
            )
        else:
            self.preview.clear()
            self.preview.setText(tr(TextKey.LABEL_PREVIEW_MISSING) if hasattr(TextKey, 'LABEL_PREVIEW_MISSING') else "Preview Missing")

        nvids = []
        if part_id == "wall_body":
            try:
                profile = get_wall_profile(wall_type)
                nvids = [str(profile["dir_a_vid"]), str(profile["dir_b_vid"]), str(profile["pillar_vid"])]
            except Exception:
                pass
        else:
            if wall_type == 0:
                if part_id == "active_door":
                    nvids = ["423", "424", "605", "606", "607", "608"]
                elif part_id in ("dead_door_closed", "dead_door_jammed", "dead_door_open"):
                    nvids = ["425",  "606", "608", "611", "617" ]
            elif wall_type == 1:
                if part_id == "lab_laser_closed":
                    nvids = ["164", "653"]
                elif part_id == "lab_laser_open":
                    nvids = ["653"]
                elif part_id == "lab_decoration_door":
                    nvids = ["654"]
            elif wall_type == 2:
                if part_id == "active_door":
                    nvids = ["423", "424", "605", "607", "623", "624"]
                elif part_id in ("dead_door_closed", "dead_door_jammed", "dead_door_open"):
                    nvids = ["425", "611", "617", "623", "624"]
            elif part_id in ("as2_door_closed", "as2_door_open"):
                nvids = self._get_as2_door_nvids(wall_type)

        if nvids:
            self.nvid_label.setText("nvid=" + "; ".join(nvids) + ";")
        else:
            self.nvid_label.clear()

    # Key door VIDs for each AS2 wall set, sourced from door_profiles_as2.h.
    # Format: [frame_vid(s), panel_closed_vid, panel_open_vid]
    # Set1 variants (3/4/5) share the same door VIDs.
    AS2_DOOR_NVID_MAP = {
        3:  ["1779", "1788", "1777", "1778", "1785", "1786"],
        4:  ["1779", "1788", "1777", "1778", "1785", "1786"],
        5:  ["1779", "1788", "1777", "1778", "1785", "1786"],
        6:  ["1703", "1785", "1786"],
        7:  ["1103", "1785", "1786"],
        8:  ["1103", "1785", "1786"],
        9:  ["2504", "2506", "2505"],
        10: ["2604", "2605", "2606"],
        11: ["2624", "2645"],
        12: ["2644", "2645"],
        13: ["1731", "1732"],
    }

    def _get_as2_door_nvids(self, wall_type: int) -> list:
        """
        Return key door VIDs for the given AS2 wall type.
        """
        return list(self.AS2_DOOR_NVID_MAP.get(wall_type, []))
