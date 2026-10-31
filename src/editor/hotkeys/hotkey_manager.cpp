#include "editor/hotkeys/hotkey_manager.h"
#include "editor/hotkeys/hotkey_config.h"
#include <QAbstractButton>
#include <QAbstractSpinBox>
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <stdexcept>

namespace editor::hotkeys {

HotkeyManager::HotkeyManager(QWidget *window) : window_(window) {}

void HotkeyManager::registerCallback(const QString &commandId, std::function<void()> callback) {
    // Throws for a command id the table does not know.
    hotkeyBinding(commandId);
    if (callbacks_.contains(commandId))
        throw std::runtime_error(("Hotkey command registered twice: " + commandId).toStdString());
    callbacks_[commandId] = std::move(callback);
}

void HotkeyManager::bindButton(const QString &commandId, QAbstractButton *button, std::function<void()> callback) {
    registerCallback(commandId, [button, callback] {
        // Hotkeys must not force disabled or unavailable toolbar options.
        if (!button->isEnabled())
            return;
        if (button->property("unavailable").toBool())
            return;
        callback();
    });
}

bool HotkeyManager::acceptsShortcut(QKeyEvent *event) const {
    if (!window_->isActiveWindow() || !window_->isEnabled())
        return false;
    if (QApplication::activeModalWidget() != nullptr)
        return false;
    if (event->isAutoRepeat())
        return false;
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if (modifiers.testFlag(Qt::ControlModifier) || modifiers.testFlag(Qt::AltModifier) ||
        modifiers.testFlag(Qt::MetaModifier))
        return false;
    if (isEditingWidget(QApplication::focusWidget()))
        return false;
    return true;
}

bool HotkeyManager::dispatch(QKeyEvent *event) {
    Qt::KeyboardModifiers modifiers = event->modifiers();
    int key = event->key();
    // Some layouts report Shift+= as Key_Equal with Shift held; treat it as '+'.
    if (key == Qt::Key_Equal && modifiers.testFlag(Qt::ShiftModifier))
        key = Qt::Key_Plus;

    for (const auto &binding : hotkeyBindings()) {
        if (!binding.keys.contains(key))
            continue;
        if (!callbacks_.contains(binding.commandId))
            return false;
        qDebug() << "Hotkey triggered:" << binding.commandId;
        callbacks_[binding.commandId]();
        return true;
    }
    return false;
}

bool HotkeyManager::isEditingWidget(QWidget *widget) const {
    QWidget *current = widget;
    while (current != nullptr) {
        if (qobject_cast<QLineEdit *>(current) != nullptr)
            return true;
        if (qobject_cast<QTextEdit *>(current) != nullptr)
            return true;
        if (qobject_cast<QPlainTextEdit *>(current) != nullptr)
            return true;
        if (qobject_cast<QAbstractSpinBox *>(current) != nullptr)
            return true;
        if (qobject_cast<QComboBox *>(current) != nullptr)
            return true;
        current = current->parentWidget();
    }
    return false;
}

} // namespace editor::hotkeys
