"""
Main window assembly for the Auto Mapper editor.
"""
from json import JSONDecodeError
from pathlib import Path

from PySide6.QtCore import Qt
from PySide6.QtGui import QAction, QActionGroup
from PySide6.QtWidgets import (
    QCheckBox,
    QFileDialog,
    QDockWidget,
    QMainWindow,
    QMenu,
    QMessageBox,
    QPushButton,
    QScrollArea,
    QSizePolicy,
    QTabWidget,
    QToolBar,
    QWidget,
)

from app.binding.client import AutoMapperLibClient
from app.config import ROOT_DIR
from app.i18n.locale import LOCALE_EN_US, LOCALE_ZH_CN, get_locale, save_locale_preference, tr
from app.i18n.text_keys import TextKey
from app.logger import logger
from app.project.data import (
    DECORATION_TYPE_ROOM_STAMP,
    get_default_map_size,
    ProjectData,
    resolve_as1_ceiling_layer_counts,
    supports_ceiling_generation,
    supports_global_door_state,
)
from app.project.io import load_project_json, save_project_json
from app.ui.canvas.viewport import MapViewport
from app.ui.panels.decoration_shelf import DecorationShelfPanel
from app.ui.panels.inspector import InspectorPanel
from app.ui.panels.left_shelf import LeftShelfPanel
from app.ui.tools.drawing_modes import DRAWING_MODE_LABELS, DrawingMode
from app.ui.tools.drawing_toolbar import DrawingToolbar
from app.binding.dll_registry import register_all_from_dll
from app.editor.decoration_stamps import get_stamp_frame_size
from app.editor.wall_profiles import get_wall_profile


LANGUAGE_BUTTON_TEXT = "语言\nlanguage"


# Breathing room added to the suggested map size when a stamp does not fit.
STAMP_MAP_SIZE_MARGIN = 200.0


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
        self.as1_ceiling_layer_config = {}
        self._register_dll_metadata()

        self.viewport = MapViewport()
        self.drawing_toolbar = DrawingToolbar(self)
        self.shelf_panel = LeftShelfPanel()
        self.decoration_shelf = DecorationShelfPanel()
        self.left_shelf_tabs = QTabWidget()
        self.inspector = InspectorPanel(self.as1_ceiling_layer_config)

        self.setCentralWidget(self.viewport)
        self._build_drawing_toolbar()
        self._build_toolbar()
        self._build_docks()
        self._connect_signals()
        project_version = self.shelf_panel.get_project_version()
        self._sync_ceiling_option(project_version, False)
        self._sync_global_door_option(project_version, False)

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
        ceiling_config = self.auto_mapper_client.load_as1_ceiling_layer_config()
        if registered and ceiling_config:
            self.as1_ceiling_layer_config = ceiling_config
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
        self.ceiling_check.setObjectName("ceilingCheck")
        self.ceiling_check.setChecked(False)
        self.ceiling_check.setProperty("unavailable", False)
        self.ceiling_check.setToolTip(tr(TextKey.TOOLTIP_CEILING))
        self.ceiling_check.clicked.connect(self._on_ceiling_changed)

        self.is_door_open_check = QCheckBox(tr(TextKey.CHECK_IS_DOOR_OPEN))
        self.is_door_open_check.setObjectName("isDoorOpenCheck")
        self.is_door_open_check.setChecked(False)
        self.is_door_open_check.setProperty("unavailable", False)
        self.is_door_open_check.setToolTip(tr(TextKey.TOOLTIP_IS_DOOR_OPEN))
        self.is_door_open_check.clicked.connect(self._on_is_door_open_changed)

        self.random_direction_check = QCheckBox(tr(TextKey.CHECK_RANDOM_DIRECTION))
        self.random_direction_check.setObjectName("randomDirectionCheck")
        self.random_direction_check.setChecked(True)
        self.random_direction_check.setToolTip(tr(TextKey.TOOLTIP_RANDOM_DIRECTION))

        new_action.triggered.connect(self._new_project)
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
        self.language_button.setText(LANGUAGE_BUTTON_TEXT)
        self.language_button.setFixedSize(112, 52)
        self.language_button.setToolTip(tr(TextKey.TOOLTIP_LANGUAGE))
        self.language_button.setMenu(self._create_language_menu())
        toolbar.addWidget(self.language_button)

    def _build_docks(self) -> None:
        """
        Build left and right editor panels.
        """
        left_dock = QDockWidget(self)
        left_dock.setObjectName("themeShelfDock")
        left_dock.setTitleBarWidget(QWidget())  # hide docker title bar
        self.left_shelf_tabs.setObjectName("leftShelfTabs")
        self.left_shelf_tabs.addTab(self.shelf_panel, tr(TextKey.PANEL_WALL_SETS))
        self.left_shelf_tabs.addTab(self.decoration_shelf, tr(TextKey.PANEL_DECORATIONS))

        # Decoration tools are kept for a future redesign, but are not ready for users.
        self.left_shelf_tabs.setTabVisible(1, False)
        self.left_shelf_tabs.tabBar().setVisible(False)

        left_dock.setWidget(self.left_shelf_tabs)
        left_dock.setAllowedAreas(Qt.LeftDockWidgetArea)
        left_dock.setFeatures(QDockWidget.NoDockWidgetFeatures)
        self.addDockWidget(Qt.LeftDockWidgetArea, left_dock)

        right_dock = QDockWidget(self)
        right_dock.setObjectName("inspectorDock")
        right_dock.setTitleBarWidget(QWidget())  # hide docker title bar
        inspector_scroll = QScrollArea()
        inspector_scroll.setObjectName("inspectorScrollArea")
        inspector_scroll.viewport().setObjectName("inspectorScrollViewport")
        inspector_scroll.setWidgetResizable(True)
        inspector_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        inspector_scroll.setWidget(self.inspector)
        right_dock.setWidget(inspector_scroll)
        right_dock.setAllowedAreas(Qt.RightDockWidgetArea)
        right_dock.setFeatures(QDockWidget.NoDockWidgetFeatures)
        self.addDockWidget(Qt.RightDockWidgetArea, right_dock)

    def _connect_signals(self) -> None:
        """
        Connect first-pass panel interactions.
        """
        self.shelf_panel.project_version_changed.connect(self._on_project_version_changed)
        self.shelf_panel.wall_set_selected.connect(self._on_wall_set_selected)
        self.shelf_panel.stamp_selected.connect(self._on_stamp_tool_selected)
        self.decoration_shelf.decoration_selected.connect(self._on_decoration_tool_selected)
        self.drawing_toolbar.drawing_mode_changed.connect(self._on_drawing_mode_changed)
        self.viewport.grid_point_selected.connect(self._on_grid_point_selected)
        self.viewport.cursor_grid_changed.connect(self._on_cursor_grid_changed)
        self.viewport.view_changed.connect(self._on_view_changed)
        self.viewport.segment_started.connect(self._on_segment_started)
        self.viewport.segment_created.connect(self._on_segment_created)
        self.viewport.door_created.connect(self._on_door_created)
        self.viewport.decoration_created.connect(self._on_decoration_created)
        self.viewport.stamp_placed.connect(self._on_stamp_placed)
        self.viewport.decoration_selected.connect(self._on_decoration_selected)
        self.viewport.decoration_changed.connect(self._on_decoration_changed)
        self.viewport.drawing_cancelled.connect(self._on_drawing_cancelled)
        self.inspector.map_size_applied.connect(self._on_map_size_applied)
        self.inspector.drawable_part_changed.connect(self._on_drawable_part_changed)
        self.inspector.wall_set_variant_changed.connect(self._on_wall_set_variant_changed)
        self.inspector.eraser_size_changed.connect(self._on_eraser_size_changed)
        self.inspector.decoration_spacing_changed.connect(self._on_decoration_spacing_changed)
        self.inspector.decoration_delete_requested.connect(self._on_decoration_delete_requested)

    def _on_wall_set_selected(self, wall_type: int, wall_name: str) -> None:
        """
        React to wall set card selection.
        """
        self.viewport.set_wall_type(wall_type)
        self.viewport.set_drawing_mode(DrawingMode.POLYLINE)
        self.drawing_toolbar.set_mode(DrawingMode.POLYLINE)
        self.inspector.set_wall_set(wall_type, wall_name)
        self.inspector.clear_decoration_selection()
        self.statusBar().showMessage(tr(TextKey.STATUS_WALL_SET_SELECTED, wall_name=wall_name))
        logger.info(f"Wall set selected: {wall_type}")

    def _on_wall_set_variant_changed(self, wall_type: int, variant_name: str) -> None:
        """
        Apply a right-panel wall set variant selection.
        """
        self.viewport.set_wall_type(wall_type)
        self.statusBar().showMessage(tr(TextKey.STATUS_WALL_SET_SELECTED, wall_name=variant_name))
        logger.info(f"Wall set variant selected: {wall_type}")

    def _on_project_version_changed(self, project_version: str) -> None:
        """
        Create an empty project after the user changes the project version.
        """
        self._reset_empty_project()
        logger.info(f"Project version switched: {project_version}")

    def _on_decoration_tool_selected(self, decoration_type: str, decoration_name: str) -> None:
        """
        React to decoration card selection.
        """
        self.viewport.set_active_decoration(decoration_type)
        self.viewport.set_drawing_mode(DrawingMode.RECTANGLE)
        self.drawing_toolbar.set_mode(DrawingMode.RECTANGLE)
        self.inspector.set_decoration_tool(decoration_type, decoration_name)
        self.statusBar().showMessage(tr(TextKey.STATUS_DECORATION_TOOL_SELECTED, decoration_name=decoration_name))
        logger.info(f"Decoration selected: {decoration_type}")

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

    def _on_stamp_tool_selected(self, profile_id: str, stamp_name: str) -> None:
        """
        Arm click-to-place for one authored room stamp.
        """
        self.viewport.set_active_decoration_stamp(profile_id)
        self.inspector.set_decoration_tool(DECORATION_TYPE_ROOM_STAMP, stamp_name)
        self.statusBar().showMessage(tr(TextKey.STATUS_DECORATION_TOOL_SELECTED, decoration_name=stamp_name))
        logger.info(f"Stamp tool selected: {profile_id}")

    def _on_stamp_placed(self, profile_id: str, out_of_bounds: bool) -> None:
        """
        Report a placed stamp and warn when the map cannot hold it.
        """
        if not out_of_bounds:
            self.statusBar().showMessage(tr(TextKey.STATUS_STAMP_PLACED, stamp_name=profile_id))
            logger.info(f"Stamp placed: {profile_id}")
            return

        frame_size = get_stamp_frame_size(profile_id)
        message = tr(
            TextKey.STATUS_MAP_TOO_SMALL_FOR_STAMP,
            suggested_x=f"{frame_size[0] + STAMP_MAP_SIZE_MARGIN:.0f}",
            suggested_y=f"{frame_size[1] + STAMP_MAP_SIZE_MARGIN:.0f}",
        )
        self.statusBar().showMessage(message)
        logger.info(f"Stamp placed out of bounds: {profile_id}")

    def _on_decoration_created(self, count: int) -> None:
        """
        Show the committed decoration.
        """
        self.statusBar().showMessage(tr(TextKey.STATUS_DECORATION_CREATED, count=count))
        logger.info(f"Decoration #{count} created")

    def _on_decoration_selected(self, decoration) -> None:
        """
        Show selected decoration properties.
        """
        self.inspector.set_decoration_selection(decoration)
        self.statusBar().showMessage(tr(TextKey.STATUS_DECORATION_SELECTED))

    def _on_decoration_changed(self, decoration) -> None:
        """
        Keep Inspector synchronized after drag edits.
        """
        self.inspector.set_decoration_selection(decoration)
        # logger.info("Decoration changed")

    def _on_decoration_delete_requested(self) -> None:
        """
        Delete the currently selected decoration frame.
        """
        deleted = self.viewport.delete_selected_decoration()
        if not deleted:
            return

        self.inspector.clear_decoration_selection()
        self.statusBar().showMessage(tr(TextKey.STATUS_DECORATION_DELETED))
        logger.info("Decoration deleted")

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

    def _on_ceiling_changed(self, checked: bool) -> None:
        """Apply the ceiling master switch to version-specific controls."""
        project_version = self.shelf_panel.get_project_version()
        self.inspector.set_ceiling_project_state(project_version, checked)
        logger.info(f"Ceiling generation changed: {checked}")

    def _create_language_menu(self) -> QMenu:
        """
        Build the language selector menu.
        """
        language_menu = QMenu(self.language_button)
        self.language_action_group = QActionGroup(self)
        self.language_action_group.setExclusive(True)

        english_action = QAction("English", self)
        english_action.setCheckable(True)
        english_action.setChecked(self.pending_locale == LOCALE_EN_US)
        self.language_action_group.addAction(english_action)
        english_action.triggered.connect(lambda: self._on_language_selected(LOCALE_EN_US))
        language_menu.addAction(english_action)

        chinese_action = QAction("中文", self)
        chinese_action.setCheckable(True)
        chinese_action.setChecked(self.pending_locale == LOCALE_ZH_CN)
        self.language_action_group.addAction(chinese_action)
        chinese_action.triggered.connect(lambda: self._on_language_selected(LOCALE_ZH_CN))
        language_menu.addAction(chinese_action)

        return language_menu

    def _on_language_selected(self, locale_name: str) -> None:
        """
        Save the selected UI language and ask the user to restart.
        """
        self.pending_locale = locale_name
        save_locale_preference(locale_name)

        language_name = self._get_locale_display_name(locale_name)
        message = tr(TextKey.STATUS_LANGUAGE_SAVED, language_name=language_name)
        self.statusBar().showMessage(message)
        QMessageBox.information(
            self,
            tr(TextKey.DIALOG_RESTART_REQUIRED),
            tr(TextKey.MESSAGE_LANGUAGE_CHANGED, language_name=language_name),
        )

    def _get_locale_display_name(self, locale_name: str) -> str:
        """
        Return a readable language name for restart prompts.
        """
        if locale_name == LOCALE_ZH_CN:
            return tr(TextKey.LANGUAGE_NAME_CHINESE)

        return tr(TextKey.LANGUAGE_NAME_ENGLISH)

    def _on_drawable_part_changed(self, part_id: str) -> None:
        """
        Apply selected drawable part to the canvas.
        """
        self.viewport.set_drawable_part(part_id)
        message = tr(TextKey.STATUS_DRAWABLE_PART_SELECTED, part_id=part_id)
        self.statusBar().showMessage(message)
        logger.info(f"Drawable part selected: {part_id}")

    def _new_project(self) -> None:
        """
        Create an empty project using the currently selected version.
        """
        self._reset_empty_project()
        logger.info(f"New project created: {self.shelf_panel.get_project_version()}")

    def _reset_empty_project(self) -> None:
        """Reset editor content while preserving the selected project version."""
        self.viewport.clear_segments()
        project_version = self.shelf_panel.get_project_version()
        self.inspector.reset_as1_ceiling_layer_counts()
        self._sync_ceiling_option(project_version, False)
        self._sync_global_door_option(project_version, False)
        default_map_size = get_default_map_size(project_version)
        self.inspector.set_map_size(default_map_size[0], default_map_size[1])
        self.viewport.set_map_size(default_map_size[0], default_map_size[1])
        profile = get_wall_profile(self.viewport.active_wall_type)
        self.inspector.set_wall_set(self.viewport.active_wall_type, profile["short_label"])
        self.inspector.clear_decoration_selection()
        self.statusBar().showMessage(tr(TextKey.STATUS_CANVAS_CLEARED))
        logger.info("Canvas cleared")

    def _export_json(self) -> None:
        """
        Export the current editor project as JSON.
        """
        project_data = self._collect_project_data()
        if not project_data.segments and not project_data.doors and not project_data.decorations:
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
        Replace the current editor project from JSON.
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
            project_data = load_project_json(
                file_path,
                self.as1_ceiling_layer_config,
            )
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

        self._replace_project(project_data)

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
        if not project_data.segments and not project_data.doors and not project_data.decorations:
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
                generate_ceiling=self.ceiling_check.isChecked(),
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
        ceiling_layer_counts = self.inspector.get_as1_ceiling_layer_counts()
        project_data = ProjectData(
            version=self.shelf_panel.get_project_version(),
            map_size_x=map_size[0],
            map_size_y=map_size[1],
            segments=self.viewport.get_segments(),
            doors=self.viewport.get_doors(),
            decorations=self.viewport.get_decorations(),
            is_door_open=self.is_door_open_check.isChecked(),
            as1_standard_ceiling_layer_count=ceiling_layer_counts[0],
            as1_lab_ceiling_layer_count=ceiling_layer_counts[1],
        )
        return project_data

    def _replace_project(self, project_data: ProjectData) -> None:
        """Replace all editor state with one imported project."""
        self.shelf_panel.set_project_version(project_data.version)
        self.viewport.set_segments(project_data.segments)
        self.viewport.set_doors(project_data.doors)
        self.viewport.set_decorations(project_data.decorations)
        ceiling_layer_counts = resolve_as1_ceiling_layer_counts(
            project_data,
            self.as1_ceiling_layer_config,
        )
        self.inspector.set_as1_ceiling_layer_counts(
            ceiling_layer_counts[0],
            ceiling_layer_counts[1],
        )
        self._sync_ceiling_option(project_data.version, False)
        self._sync_global_door_option(project_data.version, project_data.is_door_open)
        self.inspector.set_map_size(project_data.map_size_x, project_data.map_size_y)
        self.viewport.set_map_size(project_data.map_size_x, project_data.map_size_y)
        self.inspector.clear_decoration_selection()

    def _sync_ceiling_option(self, project_version: str, generate_ceiling: bool) -> None:
        """Apply project-version support rules to the ceiling checkbox."""
        option_supported = supports_ceiling_generation(project_version)
        effective_generate_ceiling = False
        if option_supported:
            effective_generate_ceiling = generate_ceiling

        self.ceiling_check.setChecked(effective_generate_ceiling)
        self.ceiling_check.setEnabled(True)
        self._set_checkbox_unavailable(self.ceiling_check, not option_supported)
        self.inspector.set_ceiling_project_state(
            project_version,
            effective_generate_ceiling,
        )

    def _sync_global_door_option(self, project_version: str, is_door_open: bool) -> None:
        """Apply version support rules to the global door option and viewport."""
        option_supported = supports_global_door_state(project_version)
        effective_is_open = False
        if option_supported:
            effective_is_open = is_door_open

        self.is_door_open_check.setChecked(effective_is_open)
        self.is_door_open_check.setEnabled(True)
        self._set_checkbox_unavailable(self.is_door_open_check, not option_supported)
        self.viewport.set_is_door_open(effective_is_open)

    def _set_checkbox_unavailable(self, checkbox: QCheckBox, unavailable: bool) -> None:
        """Apply a gray but clickable unavailable state to one checkbox."""
        checkbox.setProperty("unavailable", unavailable)
        checkbox.style().unpolish(checkbox)
        checkbox.style().polish(checkbox)
        checkbox.update()

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
        project_version = self.shelf_panel.get_project_version()
        if not supports_global_door_state(project_version):
            self.is_door_open_check.setChecked(False)
            self.viewport.set_is_door_open(False)
            QMessageBox.warning(
                self,
                tr(TextKey.DIALOG_DOOR_OPTION),
                tr(TextKey.ERROR_AS2_DOOR_OPTION_UNAVAILABLE),
            )
            self.statusBar().showMessage(tr(TextKey.STATUS_AS2_DOOR_OPTION_UNAVAILABLE))
            logger.info(f"Global door option unavailable: {project_version}")
            return

        self.viewport.set_is_door_open(checked)
        logger.info(f"Global is_door_open changed: {checked}")

    def _on_decoration_spacing_changed(self, item_spacing_scale: float, row_spacing_scale: float) -> None:
        """Sync decoration spacing from the inspector to the viewport."""
        self.viewport.update_selected_decoration_spacing(item_spacing_scale, row_spacing_scale)
        self.statusBar().showMessage(
            tr(
                TextKey.STATUS_DECORATION_SPACING,
                item_spacing_scale=item_spacing_scale,
                row_spacing_scale=row_spacing_scale,
            )
        )
        logger.info(
            f"Decoration spacing changed: item={item_spacing_scale:.2f}, column={row_spacing_scale:.2f}"
        )
