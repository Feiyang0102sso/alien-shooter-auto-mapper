// Central UI text keys. Every fixed UI string is looked up through one of these constants.
// Data-driven keys (walls, parts, stamps, hotkeys) are built by Catalog / hotkeys from their ids.
#pragma once

namespace editor::TextKey {

inline constexpr const char *APP_NAME = "app.name";
inline constexpr const char *WINDOW_TITLE = "window.title";

inline constexpr const char *TOOLBAR_MAIN_TOOLS = "toolbar.main_tools";
inline constexpr const char *TOOLBAR_DRAWING_TOOLS = "toolbar.drawing_tools";

inline constexpr const char *ACTION_NEW = "action.new";
inline constexpr const char *ACTION_IMPORT_JSON = "action.import_json";
inline constexpr const char *ACTION_EXPORT_JSON = "action.export_json";
inline constexpr const char *ACTION_GENERATE_MAP = "action.generate_map";
inline constexpr const char *ACTION_SPRITE_PREVIEW = "action.sprite_preview";

inline constexpr const char *CHECK_FLOOR = "check.floor";
inline constexpr const char *CHECK_CEILING = "check.ceiling";
inline constexpr const char *CHECK_IS_DOOR_OPEN = "check.is_door_open";
inline constexpr const char *CHECK_RANDOM_DIRECTION = "check.random_direction";

inline constexpr const char *TOOLTIP_FLOOR = "tooltip.floor";
inline constexpr const char *TOOLTIP_CEILING = "tooltip.ceiling";
inline constexpr const char *TOOLTIP_AS1_CEILING_LAYERS = "tooltip.as1_ceiling_layers";
inline constexpr const char *TOOLTIP_IS_DOOR_OPEN = "tooltip.is_door_open";
inline constexpr const char *TOOLTIP_RANDOM_DIRECTION = "tooltip.random_direction";
inline constexpr const char *TOOLTIP_LANGUAGE = "tooltip.language";
inline constexpr const char *TOOLTIP_SPRITE_PREVIEW = "tooltip.sprite_preview";

inline constexpr const char *DIALOG_SPRITE_PREVIEW = "dialog.sprite_preview";
inline constexpr const char *LABEL_AS1_GAME_FOLDER = "label.as1_game_folder";
inline constexpr const char *LABEL_AS2_GAME_FOLDER = "label.as2_game_folder";
inline constexpr const char *LABEL_GAME_FOLDER_UNSET = "label.game_folder_unset";
inline constexpr const char *BUTTON_CHOOSE_GAME_FOLDER = "button.choose_game_folder";
inline constexpr const char *CHECK_STAMP_CROSSES_WITH_SPRITES = "check.stamp_crosses_with_sprites";
inline constexpr const char *CHECK_STAMP_ALWAYS_SHOW_FRAME = "check.stamp_always_show_frame";
inline constexpr const char *LABEL_STAMP_SPRITE_OPACITY = "label.stamp_sprite_opacity";
inline constexpr const char *TOOLTIP_DECORATION_SETS = "tooltip.decoration_sets";

inline constexpr const char *PANEL_WALL_SETS = "panel.wall_sets";
inline constexpr const char *PANEL_INSPECTOR = "panel.inspector";
inline constexpr const char *PANEL_TOOLS = "panel.tools";
inline constexpr const char *SHELF_MODE_WALL_SETS = "shelf.mode.wall_sets";
inline constexpr const char *SHELF_MODE_DECORATION_SETS = "shelf.mode.decoration_sets";

inline constexpr const char *BUTTON_SELECT = "button.select";
inline constexpr const char *BUTTON_APPLY_MAP_SIZE = "button.apply_map_size";
inline constexpr const char *BUTTON_DELETE_DECORATION = "button.delete_decoration";

inline constexpr const char *GROUP_MAP_SIZE = "group.map_size";
inline constexpr const char *GROUP_AS1_CEILING = "group.as1_ceiling";
inline constexpr const char *GROUP_TOOL_PROPERTIES = "group.tool_properties";
inline constexpr const char *GROUP_DECORATION_PROPERTY = "group.decoration_property";

inline constexpr const char *LABEL_MAP_SIZE_X = "label.map_size_x";
inline constexpr const char *LABEL_MAP_SIZE_Y = "label.map_size_y";
inline constexpr const char *LABEL_COMPONENT_PREVIEW = "label.component_preview";
inline constexpr const char *LABEL_ERASER_SIZE = "label.eraser_size";
inline constexpr const char *LABEL_AS1_STANDARD_CEILING_LAYERS = "label.as1_standard_ceiling_layers";
inline constexpr const char *LABEL_AS1_LAB_CEILING_LAYERS = "label.as1_lab_ceiling_layers";
inline constexpr const char *LABEL_PREVIEW_MISSING = "label.preview_missing";
inline constexpr const char *LABEL_ITEM_SPACING = "label.item_spacing";
inline constexpr const char *LABEL_COLUMN_SPACING = "label.column_spacing";
inline constexpr const char *LABEL_STAMP_VARIANT_COUNT = "label.stamp_variant_count";
// Series card description when "decoration.<series>.description" is absent.
inline constexpr const char *LABEL_STAMP_SERIES_DESCRIPTION_DEFAULT = "label.stamp_series_description_default";
inline constexpr const char *LABEL_STAMP_MEMBER_COUNT = "label.stamp_member_count";
// "{series} #{number}": one region of a library map.
inline constexpr const char *LABEL_STAMP_REGION = "label.stamp_region";
inline constexpr const char *LABEL_STAMP_PREVIEW_READY = "label.stamp_preview_ready";
inline constexpr const char *LABEL_STAMP_PREVIEW_UNSET = "label.stamp_preview_unset";
inline constexpr const char *LABEL_PREVIEW_NOT_CONFIGURED = "label.preview_not_configured";

inline constexpr const char *CANVAS_TITLE = "canvas.title";
inline constexpr const char *CANVAS_INFO = "canvas.info";
inline constexpr const char *CANVAS_HELP = "canvas.help";
inline constexpr const char *CANVAS_HELP_PLACING = "canvas.help_placing";
inline constexpr const char *CANVAS_HELP_SELECTING = "canvas.help_selecting";

inline constexpr const char *DRAWING_SELECT = "drawing.select";
inline constexpr const char *DRAWING_POLYLINE = "drawing.polyline";
inline constexpr const char *DRAWING_LINE = "drawing.line";
inline constexpr const char *DRAWING_RECT = "drawing.rect";
inline constexpr const char *DRAWING_ERASER = "drawing.eraser";

inline constexpr const char *DIALOG_ERROR = "dialog.error";
inline constexpr const char *DIALOG_UNSAVED_PROJECT = "dialog.unsaved_project";
inline constexpr const char *DIALOG_RESTART_REQUIRED = "dialog.restart_required";
inline constexpr const char *DIALOG_DOOR_OPTION = "dialog.door_option";
inline constexpr const char *DIALOG_EXPORT_JSON = "dialog.export_json";
inline constexpr const char *DIALOG_IMPORT_JSON = "dialog.import_json";
inline constexpr const char *DIALOG_GENERATE_MAP = "dialog.generate_map";
inline constexpr const char *DIALOG_GAME_FOLDER = "dialog.game_folder";

inline constexpr const char *FILE_FILTER_JSON = "file_filter.json";
inline constexpr const char *FILE_FILTER_MAP = "file_filter.map";

inline constexpr const char *ERROR_EXPORT_EMPTY = "error.export_empty";
inline constexpr const char *ERROR_EXPORT_JSON_FAILED = "error.export_json_failed";
inline constexpr const char *ERROR_INVALID_PROJECT_DATA = "error.invalid_project_data";
inline constexpr const char *ERROR_GENERATE_EMPTY = "error.generate_empty";
inline constexpr const char *ERROR_CPP_ENGINE = "error.cpp_engine";
inline constexpr const char *ERROR_CPP_ENGINE_GENERATE_FAILED = "error.cpp_engine_generate_failed";
inline constexpr const char *ERROR_AS2_DOOR_OPTION_UNAVAILABLE = "error.as2_door_option_unavailable";

inline constexpr const char *MESSAGE_SAVE_BEFORE_CONTINUE = "message.save_before_continue";
inline constexpr const char *MESSAGE_LANGUAGE_CHANGED = "message.language_changed";
inline constexpr const char *MESSAGE_GAME_FOLDER_INVALID = "message.game_folder_invalid";

inline constexpr const char *STATUS_READY = "status.ready";
inline constexpr const char *STATUS_WALL_SET_SELECTED = "status.wall_set_selected";
inline constexpr const char *STATUS_DRAWING_TOOL_SELECTED = "status.drawing_tool_selected";
inline constexpr const char *STATUS_SELECTED_GRID_POINT = "status.selected_grid_point";
inline constexpr const char *STATUS_HOVER_GRID_POINT = "status.hover_grid_point";
inline constexpr const char *STATUS_CANVAS_ZOOM = "status.canvas_zoom";
inline constexpr const char *STATUS_WALL_START = "status.wall_start";
inline constexpr const char *STATUS_WALL_SEGMENT = "status.wall_segment";
inline constexpr const char *STATUS_DOOR = "status.door";
inline constexpr const char *STATUS_DECORATION_TOOL_SELECTED = "status.decoration_tool_selected";
inline constexpr const char *STATUS_DECORATION_CREATED = "status.decoration_created";
inline constexpr const char *STATUS_DECORATION_DELETED = "status.decoration_deleted";
inline constexpr const char *STATUS_DECORATION_SELECTED = "status.decoration_selected";
inline constexpr const char *STATUS_DECORATION_DESELECTED = "status.decoration_deselected";
inline constexpr const char *STATUS_DECORATION_PLACING_ENDED = "status.decoration_placing_ended";
inline constexpr const char *STATUS_DECORATION_SPACING = "status.decoration_spacing";
inline constexpr const char *STATUS_WALL_DRAWING_CANCELLED = "status.wall_drawing_cancelled";
inline constexpr const char *STATUS_MAP_SIZE_APPLIED = "status.map_size_applied";
inline constexpr const char *STATUS_GAME_FOLDER_OPENED = "status.game_folder_opened";
inline constexpr const char *STATUS_AS2_DOOR_OPTION_UNAVAILABLE = "status.as2_door_option_unavailable";
inline constexpr const char *STATUS_DRAWABLE_PART_SELECTED = "status.drawable_part_selected";
inline constexpr const char *STATUS_CANVAS_CLEARED = "status.canvas_cleared";
inline constexpr const char *STATUS_EXPORTED_PROJECT = "status.exported_project";
inline constexpr const char *STATUS_IMPORTED_PROJECT = "status.imported_project";
inline constexpr const char *STATUS_GENERATING_MAP = "status.generating_map";
inline constexpr const char *STATUS_GENERATED_MAP = "status.generated_map";
inline constexpr const char *STATUS_ERASER_SIZE = "status.eraser_size";
inline constexpr const char *STATUS_LANGUAGE_SAVED = "status.language_saved";
inline constexpr const char *STATUS_STAMP_PLACED = "status.stamp_placed";
inline constexpr const char *STATUS_MAP_TOO_SMALL_FOR_STAMP = "status.map_too_small_for_stamp";

inline constexpr const char *LANGUAGE_NAME_ENGLISH = "language_name.english";
inline constexpr const char *LANGUAGE_NAME_CHINESE = "language_name.chinese";

} // namespace editor::TextKey
