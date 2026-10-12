// Single source of truth for editor hotkeys. The help dialog and the runtime dispatcher both walk
// this table, so documentation and bindings cannot drift apart.
#pragma once
#include <QList>
#include <QString>

namespace editor::hotkeys {

// Help dialog pages, in display order.
inline constexpr const char *PAGE_FUNCTIONS = "functions";
inline constexpr const char *PAGE_NAVIGATION = "navigation";
inline constexpr const char *PAGE_TOOLS = "tools";

// Command ids. Their help text lives in the locale files as "hotkey.<command id>".
inline constexpr const char *COMMAND_NEW = "new";
inline constexpr const char *COMMAND_IMPORT_JSON = "import_json";
inline constexpr const char *COMMAND_EXPORT_JSON = "export_json";
inline constexpr const char *COMMAND_GENERATE_MAP = "generate_map";
inline constexpr const char *COMMAND_FLOOR = "floor";
inline constexpr const char *COMMAND_CEILING = "ceiling";
inline constexpr const char *COMMAND_DOOR_STATE = "door_state";
inline constexpr const char *COMMAND_RANDOM_DIRECTION = "random_direction";
inline constexpr const char *COMMAND_HOTKEY_HELP = "hotkey_help";
inline constexpr const char *COMMAND_LANGUAGE = "language";
inline constexpr const char *COMMAND_PREVIOUS_SET = "previous_set";
inline constexpr const char *COMMAND_NEXT_SET = "next_set";
inline constexpr const char *COMMAND_PREVIOUS_ITEM = "previous_item";
inline constexpr const char *COMMAND_NEXT_ITEM = "next_item";
inline constexpr const char *COMMAND_SELECT = "select";
inline constexpr const char *COMMAND_POLYLINE = "polyline";
inline constexpr const char *COMMAND_STRAIGHT_LINE = "straight_line";
inline constexpr const char *COMMAND_RECTANGLE = "rectangle";
inline constexpr const char *COMMAND_ERASER = "eraser";
inline constexpr const char *COMMAND_DELETE_DECORATION = "delete_decoration";

// One configured command and every Qt key code that triggers it.
struct HotkeyBinding {
    QString commandId;
    QString displayKey;
    QList<int> keys;
    QString helpPage;
    // Reserved keys are shown in the help dialog but never dispatched.
    bool enabled = true;
};

const QList<HotkeyBinding> &hotkeyBindings();
QStringList helpPageOrder();
// Binding for one command id; throws std::runtime_error for an unknown id.
const HotkeyBinding &hotkeyBinding(const QString &commandId);
// "[F1]" style hint appended to buttons and actions.
QString hotkeyHint(const QString &commandId);
// Text key of a command label or of a dialog string such as "dialog_title".
QString hotkeyTextKey(const QString &name);

} // namespace editor::hotkeys
