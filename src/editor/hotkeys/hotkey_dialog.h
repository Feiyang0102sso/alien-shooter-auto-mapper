// Paged keyboard-card help dialog, rendered from the hotkey binding table.
#pragma once
#include "editor/i18n/locale.h"
#include <QDialog>

namespace editor::hotkeys {

class HotkeyHelpDialog : public QDialog {
    Q_OBJECT
  public:
    HotkeyHelpDialog(const Locale &locale, QWidget *parent = nullptr);
};

} // namespace editor::hotkeys
