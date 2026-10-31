#include "editor/hotkeys/hotkey_dialog.h"
#include "editor/hotkeys/hotkey_config.h"
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace editor::hotkeys {
namespace {
constexpr int DIALOG_WIDTH = 1080;
constexpr int DIALOG_HEIGHT = 430;
constexpr const char *DIALOG_STYLE_RESOURCE = ":/styles/hotkeys.qss";

// Cards per row: every function key fits one row; the other pages stay readable.
int pageColumns(const QString &pageId) {
    if (pageId == PAGE_FUNCTIONS)
        return 12;
    if (pageId == PAGE_TOOLS)
        return 10;
    return 4;
}

QFrame *keyCard(const HotkeyBinding &binding, const Locale &locale) {
    auto *card = new QFrame;
    card->setObjectName("hotkeyCard");
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(6, 8, 6, 8);
    layout->setSpacing(7);
    auto *key = new QLabel(binding.displayKey);
    key->setObjectName("hotkeyKeyLabel");
    key->setAlignment(Qt::AlignCenter);
    layout->addWidget(key);
    auto *command = new QLabel(locale.tr(hotkeyTextKey(binding.commandId)));
    command->setObjectName("hotkeyCommandLabel");
    command->setAlignment(Qt::AlignCenter);
    command->setWordWrap(true);
    layout->addWidget(command);
    return card;
}

QWidget *helpPage(const QString &pageId, const Locale &locale) {
    auto *widget = new QWidget;
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(14);
    auto *title = new QLabel(locale.tr(hotkeyTextKey("page." + pageId)));
    title->setObjectName("hotkeyPageTitle");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);
    auto *cards = new QGridLayout;
    cards->setSpacing(8);
    int columns = pageColumns(pageId);
    int index = 0;
    for (const auto &binding : hotkeyBindings()) {
        if (binding.helpPage != pageId)
            continue;
        cards->addWidget(keyCard(binding, locale), index / columns, index % columns);
        ++index;
    }
    layout->addLayout(cards);
    layout->addStretch(1);
    return widget;
}
} // namespace

HotkeyHelpDialog::HotkeyHelpDialog(const Locale &locale, QWidget *parent) : QDialog(parent) {
    setObjectName("hotkeyHelpDialog");
    setWindowTitle(locale.tr(hotkeyTextKey("dialog_title")));
    setModal(true);
    setFixedSize(DIALOG_WIDTH, DIALOG_HEIGHT);
    QFile style(DIALOG_STYLE_RESOURCE);
    if (style.open(QIODevice::ReadOnly))
        setStyleSheet(QString::fromUtf8(style.readAll()));

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 18, 20, 16);
    layout->setSpacing(14);
    auto *stack = new QStackedWidget;
    for (const QString &pageId : helpPageOrder())
        stack->addWidget(helpPage(pageId, locale));
    layout->addWidget(stack, 1);

    auto *navigation = new QHBoxLayout;
    auto *previous = new QPushButton("←");
    previous->setToolTip(locale.tr(hotkeyTextKey("previous_page")));
    auto *next = new QPushButton("→");
    next->setToolTip(locale.tr(hotkeyTextKey("next_page")));
    auto *indicator = new QLabel;
    indicator->setObjectName("hotkeyPageIndicator");
    indicator->setAlignment(Qt::AlignCenter);
    navigation->addWidget(previous);
    navigation->addStretch(1);
    navigation->addWidget(indicator);
    navigation->addStretch(1);
    navigation->addWidget(next);
    layout->addLayout(navigation);

    auto updateNavigation = [stack, previous, next, indicator] {
        previous->setEnabled(stack->currentIndex() > 0);
        next->setEnabled(stack->currentIndex() < stack->count() - 1);
        indicator->setText(QString("%1 / %2").arg(stack->currentIndex() + 1).arg(stack->count()));
    };
    connect(previous, &QPushButton::clicked, stack, [stack] {
        stack->setCurrentIndex(stack->currentIndex() - 1);
    });
    connect(next, &QPushButton::clicked, stack, [stack] {
        stack->setCurrentIndex(stack->currentIndex() + 1);
    });
    connect(stack, &QStackedWidget::currentChanged, this, updateNavigation);

    // Arrow keys flip pages; Esc closes the dialog (QDialog default).
    auto *left = new QShortcut(QKeySequence(Qt::Key_Left), this);
    auto *right = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(left, &QShortcut::activated, previous, &QPushButton::click);
    connect(right, &QShortcut::activated, next, &QPushButton::click);
    updateNavigation();
}

} // namespace editor::hotkeys
