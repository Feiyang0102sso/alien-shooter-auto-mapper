#include "editor/hotkeys/hotkey_config.h"
#include <QSet>
#include <qnamespace.h>
#include <stdexcept>

namespace editor::hotkeys {
namespace {
QList<HotkeyBinding> buildBindings() {
    // Keep this list in display order: the help dialog renders each page by walking it.
    QList<HotkeyBinding> bindings{
        {COMMAND_NEW, "F1", {Qt::Key_F1}, PAGE_FUNCTIONS},
        {COMMAND_IMPORT_JSON, "F2", {Qt::Key_F2}, PAGE_FUNCTIONS},
        {COMMAND_EXPORT_JSON, "F3", {Qt::Key_F3}, PAGE_FUNCTIONS},
        {COMMAND_GENERATE_MAP, "F4", {Qt::Key_F4}, PAGE_FUNCTIONS},
        {COMMAND_FLOOR, "F5", {Qt::Key_F5}, PAGE_FUNCTIONS},
        {COMMAND_CEILING, "F6", {Qt::Key_F6}, PAGE_FUNCTIONS},
        {COMMAND_DOOR_STATE, "F7", {Qt::Key_F7}, PAGE_FUNCTIONS},
        {COMMAND_RANDOM_DIRECTION, "F8", {Qt::Key_F8}, PAGE_FUNCTIONS},
        // The shortcuts and language buttons have no key of their own.
        {COMMAND_SPRITE_PREVIEW, "F9", {Qt::Key_F9}, PAGE_FUNCTIONS},
        {COMMAND_PREVIOUS_SET, "Q", {Qt::Key_Q}, PAGE_NAVIGATION},
        {COMMAND_NEXT_SET, "E", {Qt::Key_E}, PAGE_NAVIGATION},
        // Main-row and keypad minus both report Key_Minus.
        {COMMAND_PREVIOUS_ITEM, "-", {Qt::Key_Minus}, PAGE_NAVIGATION},
        // Shift+= reports Key_Plus on most layouts; the dispatcher also maps Shift+Key_Equal here.
        {COMMAND_NEXT_ITEM, "+", {Qt::Key_Plus}, PAGE_NAVIGATION},
        {COMMAND_SELECT, "0", {Qt::Key_0}, PAGE_TOOLS},
        {COMMAND_POLYLINE, "1", {Qt::Key_1}, PAGE_TOOLS},
        {COMMAND_STRAIGHT_LINE, "2", {Qt::Key_2}, PAGE_TOOLS},
        {COMMAND_RECTANGLE, "3", {Qt::Key_3}, PAGE_TOOLS},
        {COMMAND_ERASER, "4", {Qt::Key_4}, PAGE_TOOLS},
        {COMMAND_DELETE_DECORATION, "Del", {Qt::Key_Delete}, PAGE_TOOLS},
    };

    // Fail fast when command ids or keys conflict, like HotkeyManager._validate_config().
    QSet<QString> commandIds;
    QSet<int> keys;
    for (const auto &binding : bindings) {
        if (commandIds.contains(binding.commandId))
            throw std::runtime_error(("Duplicate hotkey command: " + binding.commandId).toStdString());
        commandIds.insert(binding.commandId);
        for (int key : binding.keys) {
            if (keys.contains(key))
                throw std::runtime_error(("Duplicate hotkey key: " + binding.displayKey).toStdString());
            keys.insert(key);
        }
    }
    return bindings;
}
} // namespace

const QList<HotkeyBinding> &hotkeyBindings() {
    static const QList<HotkeyBinding> bindings = buildBindings();
    return bindings;
}

QStringList helpPageOrder() {
    return {PAGE_FUNCTIONS, PAGE_NAVIGATION, PAGE_TOOLS};
}

const HotkeyBinding &hotkeyBinding(const QString &commandId) {
    for (const auto &binding : hotkeyBindings()) {
        if (binding.commandId == commandId)
            return binding;
    }
    throw std::runtime_error(("Unknown hotkey command: " + commandId).toStdString());
}

QString hotkeyHint(const QString &commandId) {
    return "[" + hotkeyBinding(commandId).displayKey + "]";
}

QString hotkeyTextKey(const QString &name) {
    return "hotkey." + name;
}

} // namespace editor::hotkeys
