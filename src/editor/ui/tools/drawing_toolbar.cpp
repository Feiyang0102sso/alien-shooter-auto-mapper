#include "editor/ui/tools/drawing_toolbar.h"
#include "editor/hotkeys/hotkey_config.h"
#include "editor/i18n/text_keys.h"
#include <QActionGroup>
#include <QLabel>

namespace editor {

DrawingToolbar::DrawingToolbar(const Locale &locale, QWidget *parent)
    : QToolBar(locale.tr(TextKey::TOOLBAR_DRAWING_TOOLS), parent) {
    setObjectName("drawingToolbar");
    // PySide's toolbar resolves the original QSS to 8 px for both metrics.
    // Specify the observed values because native Qt resolves the ID rule differently.
    setStyleSheet("QToolBar#drawingToolbar { padding: 8px; spacing: 8px; }");
    setMovable(false);
    setOrientation(Qt::Vertical);
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    auto *title = new QLabel(locale.tr(TextKey::PANEL_TOOLS));
    title->setObjectName("panelTitle");
    title->setAlignment(Qt::AlignCenter);
    addWidget(title);

    auto *group = new QActionGroup(this);
    // "Optional" lets placing mode leave every tool unchecked.
    group->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

    // Action order matches DrawingMode values and the 0..4 hotkeys.
    const QStringList textKeys{TextKey::DRAWING_SELECT, TextKey::DRAWING_POLYLINE, TextKey::DRAWING_LINE,
                               TextKey::DRAWING_RECT, TextKey::DRAWING_ERASER};
    const QStringList commands{hotkeys::COMMAND_SELECT, hotkeys::COMMAND_POLYLINE,
                               hotkeys::COMMAND_STRAIGHT_LINE, hotkeys::COMMAND_RECTANGLE,
                               hotkeys::COMMAND_ERASER};
    for (int index = 0; index < DRAWING_MODE_COUNT; ++index) {
        DrawingMode mode = static_cast<DrawingMode>(index);
        if (mode == DrawingMode::Eraser)
            separators_[index] = addSeparator();
        modeLabels_[index] = locale.tr(textKeys[index]);
        auto *action = addAction(modeLabels_[index] + " " + hotkeys::hotkeyHint(commands[index]));
        action->setObjectName("drawingMode" + QString::number(index));
        action->setCheckable(true);
        group->addAction(action);
        modeActions_[index] = action;
        connect(action, &QAction::triggered, this, [this, mode] {
            emit modeTriggered(mode);
        });
        if (mode == DrawingMode::Select)
            separators_[index] = addSeparator();
    }
}

QAction *DrawingToolbar::modeAction(DrawingMode mode) const {
    return modeActions_.value(static_cast<int>(mode));
}

void DrawingToolbar::setCheckedMode(DrawingMode mode, bool anyChecked) {
    for (int index = 0; index < DRAWING_MODE_COUNT; ++index) {
        bool checked = anyChecked && index == static_cast<int>(mode);
        modeActions_[index]->setChecked(checked);
    }
}

void DrawingToolbar::setModeVisible(DrawingMode mode, bool visible) {
    int index = static_cast<int>(mode);
    modeActions_[index]->setVisible(visible);
    if (separators_.contains(index))
        separators_[index]->setVisible(visible);
}

QString DrawingToolbar::modeLabel(DrawingMode mode) const {
    return modeLabels_.value(static_cast<int>(mode));
}

} // namespace editor
