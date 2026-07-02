"""
Main window assembly for the Auto Mapper editor.
"""
from json import JSONDecodeError
from pathlib import Path

from PySide6.QtCore import Qt
from PySide6.QtGui import QAction
from PySide6.QtWidgets import QCheckBox, QFileDialog, QDockWidget, QMainWindow, QMessageBox, QToolBar

from app.binding.client import AutoMapperLibClient
from app.config import ROOT_DIR
from app.logger import logger
from app.project.data import DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y, ProjectData
from app.project.io import load_project_json, save_project_json
from app.ui.canvas.viewport import MapViewport
from app.ui.panels.inspector import InspectorPanel
from app.ui.panels.theme_shelf import ThemeShelfPanel
from app.ui.tools.drawing_modes import DRAWING_MODE_LABELS, DrawingMode
from app.ui.tools.drawing_toolbar import DrawingToolbar
from app.binding.dll_registry import register_all_from_dll
from app.editor.wall_profiles import get_wall_profile


class MainWindow(QMainWindow):
    """
    Top-level window that wires the first PySide6 UI skeleton together.
    """

    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("Auto Mapper - Map Editor")
        self.resize(1440, 900)

        self.auto_mapper_client = AutoMapperLibClient()
        self._register_dll_metadata()

        self.viewport = MapViewport()
        self.drawing_toolbar = DrawingToolbar(self)
        self.theme_shelf = ThemeShelfPanel()
        self.inspector = InspectorPanel()

        self.setCentralWidget(self.viewport)
        self._build_drawing_toolbar()
        self._build_toolbar()
        self._build_docks()
        self._connect_signals()

        self.statusBar().showMessage("Ready")
        logger.debug("Main window initialized")

    def _build_drawing_toolbar(self) -> None:
        """
        Build the left-side drawing tool selector.
        """
        self.addToolBar(Qt.LeftToolBarArea, self.drawing_toolbar)

    def _register_dll_metadata(self) -> None:
        """
        Register DLL metadata before UI widgets are built.
        """
        registered = register_all_from_dll(self.auto_mapper_client)
        if registered:
            return

        message = "Failed to register DLL metadata. Build the C++ DLL before starting the UI."
        logger.error(message)
        QMessageBox.critical(self, "DLL Error", message)
        raise RuntimeError(message)

    def _build_toolbar(self) -> None:
        """
        Build the first-pass command toolbar.
        """
        toolbar = QToolBar("Main Tools")
        toolbar.setObjectName("mainToolbar")
        toolbar.setMovable(False)
        toolbar.setToolButtonStyle(Qt.ToolButtonTextBesideIcon)
        self.addToolBar(Qt.TopToolBarArea, toolbar)

        new_action = QAction("New", self)
        import_action = QAction("Import JSON", self)
        export_action = QAction("Export JSON", self)
        generate_action = QAction("Generate .MAP", self)

        self.floor_check = QCheckBox("Floor")
        self.floor_check.setObjectName("floorCheck")
        self.floor_check.setChecked(False)

        self.ceiling_check = QCheckBox("Ceiling")
        self.ceiling_check.setObjectName("disabledCeilingCheck")
        self.ceiling_check.setChecked(False)
        self.ceiling_check.clicked.connect(self._show_ceiling_warning)

        new_action.triggered.connect(self._clear_canvas)
        import_action.triggered.connect(self._import_json)
        export_action.triggered.connect(self._export_json)
        generate_action.triggered.connect(self._generate_map)

        toolbar.addAction(new_action)
        toolbar.addSeparator()
        toolbar.addAction(import_action)
        toolbar.addAction(export_action)
        toolbar.addSeparator()
        toolbar.addAction(generate_action)
        toolbar.addSeparator()
        toolbar.addWidget(self.floor_check)
        toolbar.addWidget(self.ceiling_check)

    def _build_docks(self) -> None:
        """
        Build left and right editor panels.
        """
        left_dock = QDockWidget("Wall Sets", self)
        left_dock.setObjectName("themeShelfDock")
        left_dock.setWidget(self.theme_shelf)
        left_dock.setAllowedAreas(Qt.LeftDockWidgetArea)
        left_dock.setFeatures(QDockWidget.NoDockWidgetFeatures)
        self.addDockWidget(Qt.LeftDockWidgetArea, left_dock)

        right_dock = QDockWidget("Inspector", self)
        right_dock.setObjectName("inspectorDock")
        right_dock.setWidget(self.inspector)
        right_dock.setAllowedAreas(Qt.RightDockWidgetArea)
        right_dock.setFeatures(QDockWidget.NoDockWidgetFeatures)
        self.addDockWidget(Qt.RightDockWidgetArea, right_dock)

    def _connect_signals(self) -> None:
        """
        Connect first-pass panel interactions.
        """
        self.theme_shelf.wall_set_selected.connect(self._on_wall_set_selected)
        self.drawing_toolbar.drawing_mode_changed.connect(self._on_drawing_mode_changed)
        self.viewport.grid_point_selected.connect(self._on_grid_point_selected)
        self.viewport.cursor_grid_changed.connect(self._on_cursor_grid_changed)
        self.viewport.view_changed.connect(self._on_view_changed)
        self.viewport.segment_started.connect(self._on_segment_started)
        self.viewport.segment_created.connect(self._on_segment_created)
        self.viewport.door_created.connect(self._on_door_created)
        self.viewport.drawing_cancelled.connect(self._on_drawing_cancelled)
        self.inspector.map_size_applied.connect(self._on_map_size_applied)
        self.inspector.drawable_part_changed.connect(self._on_drawable_part_changed)
        self.inspector.eraser_size_changed.connect(self._on_eraser_size_changed)

    def _on_wall_set_selected(self, wall_type: int, wall_name: str) -> None:
        """
        React to wall set card selection.
        """
        self.viewport.set_wall_type(wall_type)
        self.inspector.set_wall_set(wall_type, wall_name)
        self.statusBar().showMessage(f"Wall set selected: {wall_name}")
        logger.info(f"Wall set selected: {wall_type}")

    def _on_drawing_mode_changed(self, drawing_mode: DrawingMode) -> None:
        """
        Apply the selected drawing tool to the viewport.
        """
        self.viewport.set_drawing_mode(drawing_mode)
        mode_label = DRAWING_MODE_LABELS[drawing_mode]
        self.statusBar().showMessage(f"Drawing tool selected: {mode_label}")
        logger.info(f"Drawing tool selected: {drawing_mode.value}")
        self.inspector.set_tool_properties_for_mode(drawing_mode)

    def _on_grid_point_selected(self, grid_x: int, grid_y: int) -> None:
        """
        Show the selected logical grid point.
        """
        self.statusBar().showMessage(f"Selected grid point: X={grid_x}, Y={grid_y}")
        logger.info(f"Grid point selected: x={grid_x}, y={grid_y}")

    def _on_cursor_grid_changed(self, grid_x: int, grid_y: int) -> None:
        """
        Show hover coordinates without changing editor state.
        """
        self.statusBar().showMessage(f"Hover grid point: X={grid_x}, Y={grid_y}")

    def _on_view_changed(self, zoom_factor: float) -> None:
        """
        Show current canvas zoom.
        """
        zoom_percent = int(zoom_factor * 100)
        self.statusBar().showMessage(f"Canvas zoom: {zoom_percent}%")

    def _on_segment_started(self, grid_x: int, grid_y: int) -> None:
        """
        Show that wall drawing is waiting for an end point.
        """
        self.statusBar().showMessage(f"Wall start: X={grid_x}, Y={grid_y}. Click end point.")

    def _on_segment_created(self, x1: int, y1: int, x2: int, y2: int, count: int) -> None:
        """
        Show the committed wall segment.
        """
        message = f"Wall segment #{count}: ({x1}, {y1}) -> ({x2}, {y2})"
        self.statusBar().showMessage(message)
        logger.info(message)

    def _on_door_created(self, grid_x: int, grid_y: int, count: int) -> None:
        """
        Show the committed door marker.
        """
        message = f"Door #{count}: ({grid_x}, {grid_y})"
        self.statusBar().showMessage(message)
        logger.info(message)

    def _on_drawing_cancelled(self) -> None:
        """
        Show cancellation feedback.
        """
        self.statusBar().showMessage("Wall drawing cancelled")

    def _on_map_size_applied(self, map_size_x: float, map_size_y: float) -> None:
        """
        Apply Inspector map size to the canvas grid.
        """
        self.viewport.set_map_size(map_size_x, map_size_y)
        message = f"Map size applied: {map_size_x:.1f} x {map_size_y:.1f}"
        self.statusBar().showMessage(message)
        logger.info(message)

    def _show_ceiling_warning(self) -> None:
        """
        Explain why ceiling generation is disabled.
        """
        self.ceiling_check.setChecked(False)
        QMessageBox.warning(self, "Ceiling", "Ceiling generation is disabled because the algorithm is currently incorrect.")
        self.statusBar().showMessage("Ceiling generation is disabled")

    def _on_drawable_part_changed(self, part_id: str) -> None:
        """
        Apply selected drawable part to the canvas.
        """
        self.viewport.set_drawable_part(part_id)
        message = f"Drawable part selected: {part_id}"
        self.statusBar().showMessage(message)
        logger.info(message)

    def _clear_canvas(self) -> None:
        """
        Clear the current in-memory drawing.
        """
        self.viewport.clear_segments()
        self.inspector.set_map_size(DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y)
        self.viewport.set_map_size(DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y)
        profile = get_wall_profile(self.viewport.active_wall_type)
        self.inspector.set_wall_set(self.viewport.active_wall_type, profile["short_label"])
        self.statusBar().showMessage("Canvas cleared")
        logger.info("Canvas cleared")

    def _export_json(self) -> None:
        """
        Export current wall segments as old UI compatible JSON.
        """
        project_data = self._collect_project_data()
        if not project_data.segments and not project_data.doors:
            QMessageBox.warning(self, "Export JSON", "Draw wall segments before exporting.")
            return

        selected_path = QFileDialog.getSaveFileName(
            self,
            "Export JSON",
            "auto_mapper_project.json",
            "JSON files (*.json);;All files (*.*)",
        )
        file_name = selected_path[0]
        if not file_name:
            return

        file_path = Path(file_name)
        try:
            save_project_json(file_path, project_data)
        except OSError as error:
            QMessageBox.critical(self, "Export JSON", f"Failed to export JSON:\n{error}")
            logger.error(f"Failed to export JSON: {error}")
            return

        message = f"Exported {len(project_data.segments)} wall segments and {len(project_data.doors)} doors: {file_path}"
        self.statusBar().showMessage(message)
        logger.info(message)

    def _import_json(self) -> None:
        """
        Import wall segments from old UI compatible JSON.
        """
        selected_path = QFileDialog.getOpenFileName(
            self,
            "Import JSON",
            "",
            "JSON files (*.json);;All files (*.*)",
        )
        file_name = selected_path[0]
        if not file_name:
            return

        file_path = Path(file_name)
        try:
            project_data = load_project_json(file_path)
        except OSError as error:
            QMessageBox.critical(self, "Import JSON", f"Failed to read JSON:\n{error}")
            logger.error(f"Failed to read JSON: {error}")
            return
        except JSONDecodeError as error:
            QMessageBox.critical(self, "Import JSON", f"Invalid JSON file:\n{error}")
            logger.error(f"Invalid JSON file: {error}")
            return
        except ValueError as error:
            QMessageBox.critical(self, "Import JSON", f"Invalid project data:\n{error}")
            logger.error(f"Invalid project data: {error}")
            return

        self.viewport.set_segments(project_data.segments)
        self.viewport.set_doors(project_data.doors)
        self.inspector.set_map_size(project_data.map_size_x, project_data.map_size_y)
        self.viewport.set_map_size(project_data.map_size_x, project_data.map_size_y)

        message = f"Imported {len(project_data.segments)} wall segments and {len(project_data.doors)} doors: {file_path}"
        self.statusBar().showMessage(message)
        logger.info(message)

    def _generate_map(self) -> None:
        """
        Generate a .map file through the C++ DLL.
        """
        project_data = self._collect_project_data()
        if not project_data.segments and not project_data.doors:
            QMessageBox.warning(self, "Generate .MAP", "Draw wall segments or doors before generating a map.")
            return

        selected_path = QFileDialog.getSaveFileName(
            self,
            "Generate .MAP",
            str(ROOT_DIR / "ui_output.map"),
            "MAP files (*.map);;All files (*.*)",
        )
        file_name = selected_path[0]
        if not file_name:
            return

        output_path = Path(file_name)
        if output_path.suffix == "":
            output_path = output_path.with_suffix(".map")

        try:
            success = self.auto_mapper_client.generate_map(
                output_path,
                project_data,
                generate_floor=self.floor_check.isChecked(),
                generate_ceiling=False,
            )
        except FileNotFoundError as error:
            QMessageBox.critical(self, "Generate .MAP", f"DLL not found:\n{error}")
            logger.error(f"DLL not found during map generation: {error}")
            return
        except OSError as error:
            QMessageBox.critical(self, "Generate .MAP", f"Failed to call DLL:\n{error}")
            logger.error(f"Failed to call DLL: {error}")
            return
        except RuntimeError as error:
            QMessageBox.critical(self, "Generate .MAP", f"C++ engine error:\n{error}")
            logger.error(f"C++ engine error: {error}")
            return

        if not success:
            QMessageBox.critical(self, "Generate .MAP", "C++ engine failed to generate the map.")
            logger.error("C++ engine returned false during map generation.")
            return

        message = f"Generated map: {output_path}"
        self.statusBar().showMessage(message)
        QMessageBox.information(self, "Generate .MAP", message)
        logger.info(message)

    def _collect_project_data(self) -> ProjectData:
        """
        Collect current UI state into project data.
        """
        map_size = self.inspector.get_map_size()
        project_data = ProjectData(
            map_size_x=map_size[0],
            map_size_y=map_size[1],
            segments=self.viewport.get_segments(),
            doors=self.viewport.get_doors(),
        )
        return project_data

    def _show_not_ready(self) -> None:
        """
        Keep unfinished commands visible without pretending they work.
        """
        self.statusBar().showMessage("Command placeholder: implementation pending")
        logger.info("Toolbar command clicked before implementation")

    def _on_eraser_size_changed(self, size: int) -> None:
        """Sync eraser size from the inspector to the viewport."""
        self.viewport.set_eraser_size(size)
        self.statusBar().showMessage(f"Eraser size: {size}")
        logger.info(f"Eraser size changed: {size}")
