// Vertical drawing-tool toolbar on the left edge of the main window.
#pragma once
#include "editor/i18n/locale.h"
#include "editor/ui/tools/drawing_modes.h"
#include <QHash>
#include <QToolBar>

class QActionGroup;

namespace editor {

class DrawingToolbar : public QToolBar {
    Q_OBJECT
  public:
    explicit DrawingToolbar(const Locale &locale, QWidget *parent = nullptr);

    QAction *modeAction(DrawingMode mode) const;
    // Check one tool, or none while a decoration tool owns the left button.
    void setCheckedMode(DrawingMode mode, bool anyChecked);
    // Hide a tool together with the separator bound to it.
    void setModeVisible(DrawingMode mode, bool visible);
    // Tool label without its "[n]" hotkey hint, for status messages.
    QString modeLabel(DrawingMode mode) const;

  signals:
    void modeTriggered(DrawingMode mode);

  private:
    QHash<int, QAction *> modeActions_;
    // Select and Eraser each own one separator that hides with them.
    QHash<int, QAction *> separators_;
    QHash<int, QString> modeLabels_;
};

} // namespace editor
