"""
Main window assembly for the Auto Mapper editor.
"""
from json import JSONDecodeError
import math
from pathlib import Path

from PySide6.QtCore import QPointF, QRectF, QSize, Qt
from PySide6.QtGui import QAction, QBrush, QColor, QIcon, QPainter, QPen, QPixmap, QPolygonF
from PySide6.QtWidgets import (
    QCheckBox,
    QFileDialog,
    QDockWidget,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QSizePolicy,
    QToolBar,
    QToolButton,
    QWidget,
)

from app.binding.client import AutoMapperLibClient
from app.config import ROOT_DIR
from app.i18n.locale import LOCALE_EN_US, LOCALE_ZH_CN, get_locale, save_locale_preference, tr
from app.i18n.text_keys import TextKey
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
        self.setWindowTitle(tr(TextKey.WINDOW_TITLE))
        self.resize(1440, 900)

        self.auto_mapper_client = AutoMapperLibClient()
        self.pending_locale = get_locale()
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

        self.statusBar().showMessage(tr(TextKey.STATUS_READY))
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

        message = tr(TextKey.ERROR_DLL_REGISTER_FAILED)
        logger.error("Failed to register DLL metadata. Build the C++ DLL before starting the UI.")
        QMessageBox.critical(self, tr(TextKey.DIALOG_DLL_ERROR), message)
        raise RuntimeError(message)

    def _build_toolbar(self) -> None:
        """
        Build the first-pass command toolbar.
        """
        toolbar = QToolBar(tr(TextKey.TOOLBAR_MAIN_TOOLS))
        toolbar.setObjectName("mainToolbar")
        toolbar.setMovable(False)
        toolbar.setToolButtonStyle(Qt.ToolButtonTextBesideIcon)
        self.addToolBar(Qt.TopToolBarArea, toolbar)

        new_action = QAction(tr(TextKey.ACTION_NEW), self)
        import_action = QAction(tr(TextKey.ACTION_IMPORT_JSON), self)
        export_action = QAction(tr(TextKey.ACTION_EXPORT_JSON), self)
        generate_action = QAction(tr(TextKey.ACTION_GENERATE_MAP), self)

        self.floor_check = QCheckBox(tr(TextKey.CHECK_FLOOR))
        self.floor_check.setObjectName("floorCheck")
        self.floor_check.setChecked(False)
        self.floor_check.setToolTip(tr(TextKey.TOOLTIP_FLOOR))

        self.ceiling_check = QCheckBox(tr(TextKey.CHECK_CEILING))
        self.ceiling_check.setObjectName("disabledCeilingCheck")
        self.ceiling_check.setChecked(False)
        self.ceiling_check.setToolTip(tr(TextKey.TOOLTIP_CEILING))
        self.ceiling_check.clicked.connect(self._show_ceiling_warning)

        self.is_door_open_check = QCheckBox(tr(TextKey.CHECK_IS_DOOR_OPEN))
        self.is_door_open_check.setObjectName("isDoorOpenCheck")
        self.is_door_open_check.setChecked(False)
        self.is_door_open_check.setToolTip(tr(TextKey.TOOLTIP_IS_DOOR_OPEN))
        self.is_door_open_check.clicked.connect(self._on_is_door_open_changed)

        self.random_direction_check = QCheckBox(tr(TextKey.CHECK_RANDOM_DIRECTION))
        self.random_direction_check.setObjectName("randomDirectionCheck")
        self.random_direction_check.setChecked(True)
        self.random_direction_check.setToolTip(tr(TextKey.TOOLTIP_RANDOM_DIRECTION))

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
        toolbar.addWidget(self.is_door_open_check)
        toolbar.addWidget(self.random_direction_check)

        toolbar_spacer = QWidget(self)
        toolbar_spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        toolbar.addWidget(toolbar_spacer)

        self.language_button = QPushButton(self)
        self.language_button.setObjectName("languageToggleButton")
        self.language_button.setIcon(self._create_language_icon())
        self.language_button.setIconSize(QSize(88, 34))
        self.language_button.setFixedSize(112, 52)
        self.language_button.setToolTip("Switch language")
        self.language_button.clicked.connect(self._on_language_toggle_clicked)
        toolbar.addWidget(self.language_button)

    def _build_docks(self) -> None:
        """
        Build left and right editor panels.
        """
        left_dock = QDockWidget(self)
        left_dock.setObjectName("themeShelfDock")
        left_dock.setTitleBarWidget(QWidget())  # hide docker title bar
        left_dock.setWidget(self.theme_shelf)
        left_dock.setAllowedAreas(Qt.LeftDockWidgetArea)
        left_dock.setFeatures(QDockWidget.NoDockWidgetFeatures)
        self.addDockWidget(Qt.LeftDockWidgetArea, left_dock)

        right_dock = QDockWidget(self)
        right_dock.setObjectName("inspectorDock")
        right_dock.setTitleBarWidget(QWidget())  # hide docker title bar
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
        self.statusBar().showMessage(tr(TextKey.STATUS_WALL_SET_SELECTED, wall_name=wall_name))
        logger.info(f"Wall set selected: {wall_type}")

    def _on_drawing_mode_changed(self, drawing_mode: DrawingMode) -> None:
        """
        Apply the selected drawing tool to the viewport.
        """
        self.viewport.set_drawing_mode(drawing_mode)
        mode_label = DRAWING_MODE_LABELS[drawing_mode]
        self.statusBar().showMessage(tr(TextKey.STATUS_DRAWING_TOOL_SELECTED, mode_label=mode_label))
        logger.info(f"Drawing tool selected: {drawing_mode.value}")
        self.inspector.set_tool_properties_for_mode(drawing_mode)

    def _on_grid_point_selected(self, grid_x: int, grid_y: int) -> None:
        """
        Show the selected logical grid point.
        """
        self.statusBar().showMessage(tr(TextKey.STATUS_SELECTED_GRID_POINT, grid_x=grid_x, grid_y=grid_y))
        logger.info(f"Grid point selected: x={grid_x}, y={grid_y}")

    def _on_cursor_grid_changed(self, grid_x: int, grid_y: int) -> None:
        """
        Show hover coordinates without changing editor state.
        """
        self.statusBar().showMessage(tr(TextKey.STATUS_HOVER_GRID_POINT, grid_x=grid_x, grid_y=grid_y))

    def _on_view_changed(self, zoom_factor: float) -> None:
        """
        Show current canvas zoom.
        """
        zoom_percent = int(zoom_factor * 100)
        self.statusBar().showMessage(tr(TextKey.STATUS_CANVAS_ZOOM, zoom_percent=zoom_percent))

    def _on_segment_started(self, grid_x: int, grid_y: int) -> None:
        """
        Show that wall drawing is waiting for an end point.
        """
        self.statusBar().showMessage(tr(TextKey.STATUS_WALL_START, grid_x=grid_x, grid_y=grid_y))

    def _on_segment_created(self, x1: int, y1: int, x2: int, y2: int, count: int) -> None:
        """
        Show the committed wall segment.
        """
        message = tr(TextKey.STATUS_WALL_SEGMENT, count=count, x1=x1, y1=y1, x2=x2, y2=y2)
        self.statusBar().showMessage(message)
        logger.info(f"Wall segment #{count}: ({x1}, {y1}) -> ({x2}, {y2})")

    def _on_door_created(self, grid_x: int, grid_y: int, count: int) -> None:
        """
        Show the committed door marker.
        """
        message = tr(TextKey.STATUS_DOOR, count=count, grid_x=grid_x, grid_y=grid_y)
        self.statusBar().showMessage(message)
        logger.info(f"Door #{count}: ({grid_x}, {grid_y})")

    def _on_drawing_cancelled(self) -> None:
        """
        Show cancellation feedback.
        """
        self.statusBar().showMessage(tr(TextKey.STATUS_WALL_DRAWING_CANCELLED))

    def _on_map_size_applied(self, map_size_x: float, map_size_y: float) -> None:
        """
        Apply Inspector map size to the canvas grid.
        """
        self.viewport.set_map_size(map_size_x, map_size_y)
        message = tr(TextKey.STATUS_MAP_SIZE_APPLIED, map_size_x=map_size_x, map_size_y=map_size_y)
        self.statusBar().showMessage(message)
        logger.info(f"Map size applied: {map_size_x:.1f} x {map_size_y:.1f}")

    def _show_ceiling_warning(self) -> None:
        """
        Explain why ceiling generation is disabled.
        """
        self.ceiling_check.setChecked(False)
        QMessageBox.warning(self, tr(TextKey.DIALOG_CEILING), tr(TextKey.ERROR_CEILING_DISABLED))
        self.statusBar().showMessage(tr(TextKey.STATUS_CEILING_DISABLED))

    def _on_language_toggle_clicked(self) -> None:
        """
        Save the next UI language and ask the user to restart.
        """
        next_locale = self._get_next_pending_locale()
        self.pending_locale = next_locale
        save_locale_preference(next_locale)

        language_name = self._get_locale_display_name(next_locale)
        message = f"Language saved: {language_name}. Restart to apply it fully."
        self.statusBar().showMessage(message)
        QMessageBox.information(
            self,
            "Restart Required",
            f"Language has been changed to {language_name}. Restart Auto Mapper to apply it fully.",
        )

    def _get_next_pending_locale(self) -> str:
        """
        Return the language selected by the next toggle click.
        """
        if self.pending_locale == LOCALE_EN_US:
            return LOCALE_ZH_CN

        return LOCALE_EN_US

    def _get_locale_display_name(self, locale_name: str) -> str:
        """
        Return a readable language name for restart prompts.
        """
        if locale_name == LOCALE_ZH_CN:
            return "中文"

        return "English"

    def _create_language_icon(self) -> QIcon:
        """
        Create a two-flag language icon without relying on emoji font support.
        Optimized for high-DPI scaling.
        """
        logical_width = 88
        logical_height = 34

        ratio = self.devicePixelRatio()
        if ratio <= 0:
            ratio = 1.0

        pixmap = QPixmap(int(logical_width * ratio), int(logical_height * ratio))
        pixmap.fill(Qt.transparent)
        pixmap.setDevicePixelRatio(ratio)

        painter = QPainter(pixmap)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Draw both flags with original sizes (38x27) but with better spacing in the 88x34 logical area
        # Left margin: 4, spacing: 4, right margin: 4 (4 + 38 + 4 + 38 + 4 = 88)
        self._draw_us_flag(painter, QRectF(4, 3.5, 38, 27))
        self._draw_china_flag(painter, QRectF(46, 3.5, 38, 27))
        painter.end()

        return QIcon(pixmap)

    def _draw_star(self, painter: QPainter, cx: float, cy: float, radius: float, color: QColor, angle_offset: float = 0.0) -> None:
        """
        Draw a standard 5-point star (pentagram) centered at (cx, cy).
        """
        points = []
        r = radius * 0.382
        for i in range(10):
            # Start angle is -pi/2 (top)
            angle = -math.pi / 2 + i * math.pi / 5 + angle_offset
            current_r = radius if i % 2 == 0 else r
            x = cx + current_r * math.cos(angle)
            y = cy + current_r * math.sin(angle)
            points.append(QPointF(x, y))
            
        polygon = QPolygonF(points)
        painter.setBrush(QBrush(color))
        painter.setPen(Qt.NoPen)
        painter.drawPolygon(polygon)

    def _draw_us_flag(self, painter: QPainter, flag_rect: QRectF) -> None:
        """
        Draw a compact US flag for the language toggle with actual stars.
        """
        painter.setPen(QPen(QColor("#d8dee9"), 0.8))
        painter.setBrush(QBrush(QColor("#f8f8f2")))
        painter.drawRoundedRect(flag_rect, 2, 2)

        stripe_height = flag_rect.height() / 7.0
        painter.setPen(Qt.NoPen)
        painter.setBrush(QBrush(QColor("#bf3131")))

        for index in range(0, 7, 2):
            y = flag_rect.y() + index * stripe_height
            stripe_rect = QRectF(flag_rect.x(), y, flag_rect.width(), stripe_height)
            painter.drawRect(stripe_rect)

        canton_rect = QRectF(flag_rect.x(), flag_rect.y(), flag_rect.width() * 0.46, stripe_height * 4)
        painter.setBrush(QBrush(QColor("#21468b")))
        painter.drawRect(canton_rect)

        # Draw real stars instead of circle placeholders
        star_points = [
            QPointF(canton_rect.x() + 4.5, canton_rect.y() + 4.5),
            QPointF(canton_rect.x() + 12.5, canton_rect.y() + 4.5),
            QPointF(canton_rect.x() + 4.5, canton_rect.y() + 11.5),
            QPointF(canton_rect.x() + 12.5, canton_rect.y() + 11.5),
        ]

        for point in star_points:
            self._draw_star(painter, point.x(), point.y(), 1.8, QColor("#ffffff"))

    def _draw_china_flag(self, painter: QPainter, flag_rect: QRectF) -> None:
        """
        Draw a standard, geometrically accurate China flag for the language toggle.
        """
        # Draw background
        painter.setPen(QPen(QColor("#d8dee9"), 0.8))
        painter.setBrush(QBrush(QColor("#de2910")))
        painter.drawRoundedRect(flag_rect, 2, 2)

        # Scale parameters based on standard 30x20 grid
        w = flag_rect.width()
        h = flag_rect.height()
        x0 = flag_rect.x()
        y0 = flag_rect.y()
        
        # Grid unit size
        dx = w / 30.0
        dy = h / 20.0

        # Big star center and radius
        bx = x0 + 5.0 * dx
        by = y0 + 5.0 * dy
        br = 3.0 * dy
        
        self._draw_star(painter, bx, by, br, QColor("#ffde00"))

        # Small stars grid positions and rotation angles so they point to the big star
        small_stars = [
            (10.0, 2.0),
            (12.0, 4.0),
            (12.0, 7.0),
            (10.0, 9.0)
        ]
        
        sr = 1.0 * dy
        for sx_grid, sy_grid in small_stars:
            sx = x0 + sx_grid * dx
            sy = y0 + sy_grid * dy
            # Calculate angle from small star to big star and rotate to align one tip
            angle = math.atan2(by - sy, bx - sx) + math.pi / 2
            self._draw_star(painter, sx, sy, sr, QColor("#ffde00"), angle)

    def _on_drawable_part_changed(self, part_id: str) -> None:
        """
        Apply selected drawable part to the canvas.
        """
        self.viewport.set_drawable_part(part_id)
        message = tr(TextKey.STATUS_DRAWABLE_PART_SELECTED, part_id=part_id)
        self.statusBar().showMessage(message)
        logger.info(f"Drawable part selected: {part_id}")

    def _clear_canvas(self) -> None:
        """
        Clear the current in-memory drawing.
        """
        self.viewport.clear_segments()
        self.is_door_open_check.setChecked(False)
        self.viewport.set_is_door_open(False)
        self.inspector.set_map_size(DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y)
        self.viewport.set_map_size(DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y)
        profile = get_wall_profile(self.viewport.active_wall_type)
        self.inspector.set_wall_set(self.viewport.active_wall_type, profile["short_label"])
        self.statusBar().showMessage(tr(TextKey.STATUS_CANVAS_CLEARED))
        logger.info("Canvas cleared")

    def _export_json(self) -> None:
        """
        Export current wall segments as old UI compatible JSON.
        """
        project_data = self._collect_project_data()
        if not project_data.segments and not project_data.doors:
            QMessageBox.warning(self, tr(TextKey.DIALOG_EXPORT_JSON), tr(TextKey.ERROR_EXPORT_EMPTY))
            return

        selected_path = QFileDialog.getSaveFileName(
            self,
            tr(TextKey.DIALOG_EXPORT_JSON),
            "auto_mapper_project.json",
            tr(TextKey.FILE_FILTER_JSON),
        )
        file_name = selected_path[0]
        if not file_name:
            return

        file_path = Path(file_name)
        try:
            save_project_json(file_path, project_data)
        except OSError as error:
            QMessageBox.critical(
                self,
                tr(TextKey.DIALOG_EXPORT_JSON),
                tr(TextKey.ERROR_EXPORT_JSON_FAILED, error=error),
            )
            logger.error(f"Failed to export JSON: {error}")
            return

        message = tr(
            TextKey.STATUS_EXPORTED_PROJECT,
            segment_count=len(project_data.segments),
            door_count=len(project_data.doors),
            file_path=file_path,
        )
        self.statusBar().showMessage(message)
        logger.info(
            f"Exported {len(project_data.segments)} wall segments and "
            f"{len(project_data.doors)} doors: {file_path}"
        )

    def _import_json(self) -> None:
        """
        Import wall segments from old UI compatible JSON.
        """
        selected_path = QFileDialog.getOpenFileName(
            self,
            tr(TextKey.DIALOG_IMPORT_JSON),
            "",
            tr(TextKey.FILE_FILTER_JSON),
        )
        file_name = selected_path[0]
        if not file_name:
            return

        file_path = Path(file_name)
        try:
            project_data = load_project_json(file_path)
        except OSError as error:
            QMessageBox.critical(
                self,
                tr(TextKey.DIALOG_IMPORT_JSON),
                tr(TextKey.ERROR_READ_JSON_FAILED, error=error),
            )
            logger.error(f"Failed to read JSON: {error}")
            return
        except JSONDecodeError as error:
            QMessageBox.critical(
                self,
                tr(TextKey.DIALOG_IMPORT_JSON),
                tr(TextKey.ERROR_INVALID_JSON, error=error),
            )
            logger.error(f"Invalid JSON file: {error}")
            return
        except ValueError as error:
            QMessageBox.critical(
                self,
                tr(TextKey.DIALOG_IMPORT_JSON),
                tr(TextKey.ERROR_INVALID_PROJECT_DATA, error=error),
            )
            logger.error(f"Invalid project data: {error}")
            return

        self.viewport.set_segments(project_data.segments)
        self.viewport.set_doors(project_data.doors)
        self.is_door_open_check.setChecked(project_data.is_door_open)
        self.viewport.set_is_door_open(project_data.is_door_open)
        self.inspector.set_map_size(project_data.map_size_x, project_data.map_size_y)
        self.viewport.set_map_size(project_data.map_size_x, project_data.map_size_y)

        message = tr(
            TextKey.STATUS_IMPORTED_PROJECT,
            segment_count=len(project_data.segments),
            door_count=len(project_data.doors),
            file_path=file_path,
        )
        self.statusBar().showMessage(message)
        logger.info(
            f"Imported {len(project_data.segments)} wall segments and "
            f"{len(project_data.doors)} doors: {file_path}"
        )

    def _generate_map(self) -> None:
        """
        Generate a .map file through the C++ DLL.
        """
        project_data = self._collect_project_data()
        if not project_data.segments and not project_data.doors:
            QMessageBox.warning(self, tr(TextKey.DIALOG_GENERATE_MAP), tr(TextKey.ERROR_GENERATE_EMPTY))
            return

        selected_path = QFileDialog.getSaveFileName(
            self,
            tr(TextKey.DIALOG_GENERATE_MAP),
            str(ROOT_DIR / "ui_output.map"),
            tr(TextKey.FILE_FILTER_MAP),
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
                random_direction=self.random_direction_check.isChecked(),
            )
        except FileNotFoundError as error:
            QMessageBox.critical(
                self,
                tr(TextKey.DIALOG_GENERATE_MAP),
                tr(TextKey.ERROR_DLL_NOT_FOUND, error=error),
            )
            logger.error(f"DLL not found during map generation: {error}")
            return
        except OSError as error:
            QMessageBox.critical(
                self,
                tr(TextKey.DIALOG_GENERATE_MAP),
                tr(TextKey.ERROR_DLL_CALL_FAILED, error=error),
            )
            logger.error(f"Failed to call DLL: {error}")
            return
        except RuntimeError as error:
            QMessageBox.critical(
                self,
                tr(TextKey.DIALOG_GENERATE_MAP),
                tr(TextKey.ERROR_CPP_ENGINE, error=error),
            )
            logger.error(f"C++ engine error: {error}")
            return

        if not success:
            QMessageBox.critical(self, tr(TextKey.DIALOG_GENERATE_MAP), tr(TextKey.ERROR_CPP_ENGINE_GENERATE_FAILED))
            logger.error("C++ engine returned false during map generation.")
            return

        message = tr(TextKey.STATUS_GENERATED_MAP, output_path=output_path)
        self.statusBar().showMessage(message)
        QMessageBox.information(self, tr(TextKey.DIALOG_GENERATE_MAP), message)
        logger.info(f"Generated map: {output_path}")

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
            is_door_open=self.is_door_open_check.isChecked(),
        )
        return project_data

    def _show_not_ready(self) -> None:
        """
        Keep unfinished commands visible without pretending they work.
        """
        self.statusBar().showMessage(tr(TextKey.STATUS_COMMAND_PLACEHOLDER))
        logger.info("Toolbar command clicked before implementation")

    def _on_eraser_size_changed(self, size: int) -> None:
        """Sync eraser size from the inspector to the viewport."""
        self.viewport.set_eraser_size(size)
        self.statusBar().showMessage(tr(TextKey.STATUS_ERASER_SIZE, size=size))
        logger.info(f"Eraser size changed: {size}")

    def _on_is_door_open_changed(self, checked: bool) -> None:
        """Update the viewport open door state and redraw."""
        self.viewport.set_is_door_open(checked)
        logger.info(f"Global is_door_open changed: {checked}")
