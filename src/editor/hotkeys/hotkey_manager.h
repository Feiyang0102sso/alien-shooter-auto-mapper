// Dispatches configured hotkeys to editor commands and applies the keyboard focus rules.
#pragma once
#include <QHash>
#include <QString>
#include <functional>

class QAbstractButton;
class QKeyEvent;
class QWidget;

namespace editor::hotkeys {

class HotkeyManager {
  public:
    explicit HotkeyManager(QWidget *window);

    // Register one command; throws for unknown or duplicate command ids.
    void registerCallback(const QString &commandId, std::function<void()> callback);
    // Like registerCallback, but skipped while the button is disabled or marked "unavailable".
    void bindButton(const QString &commandId, QAbstractButton *button, std::function<void()> callback);
    // Focus and modifier rules shared by every shortcut, including the unlisted Esc cancel.
    bool acceptsShortcut(QKeyEvent *event) const;
    // Returns true when the key press ran a registered command. Call acceptsShortcut() first.
    bool dispatch(QKeyEvent *event);

  private:
    // Inputs that must keep every key press (typing a map size must not switch tools).
    bool isEditingWidget(QWidget *widget) const;

    QWidget *window_;
    QHash<QString, std::function<void()>> callbacks_;
};

} // namespace editor::hotkeys
