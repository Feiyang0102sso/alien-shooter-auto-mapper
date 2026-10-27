#include "editor/ui/panels/theme_shelf.h"
#include <QDebug>
#include "editor/i18n/text_keys.h"
#include "editor/ui/colors.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

namespace editor {
namespace {
constexpr int CARD_PREVIEW_WIDTH = 260;
constexpr int CARD_PREVIEW_HEIGHT = 118;
constexpr int LEGEND_SYMBOL_WIDTH = 52;
constexpr int LEGEND_SYMBOL_HEIGHT = 18;

// Door legend style per drawable part, as in the legacy LEGEND_DOOR_STYLES table.
struct DoorLegendStyle {
    const char *line;
    const char *dot;
    bool hollow;
};

bool doorLegendStyle(const QString &partId, DoorLegendStyle &style) {
    using namespace colors;
    if (partId == "active_door")
        style = {DOOR_STD_ACTIVE_LINE, DOOR_STD_ACTIVE_LINE, true};
    else if (partId == "dead_door_closed")
        style = {DOOR_STD_DEAD_CLOSED_LINE, DOOR_STD_DEAD_CLOSED_DOT, false};
    else if (partId == "dead_door_jammed")
        style = {DOOR_STD_DEAD_JAMMED_LINE, DOOR_STD_DEAD_JAMMED_DOT, false};
    else if (partId == "dead_door_open")
        style = {DOOR_STD_DEAD_OPEN_LINE, DOOR_STD_DEAD_OPEN_DOT, true};
    else if (partId == "lab_laser_closed")
        style = {DOOR_LAB_LASER_WITH_LINE, DOOR_LAB_LASER_WITH_DOT, false};
    else if (partId == "lab_laser_open")
        style = {DOOR_LAB_LASER_WITHOUT_LINE, DOOR_LAB_LASER_WITHOUT_DOT, true};
    else if (partId == "lab_decoration_door")
        style = {DOOR_LAB_DECO_LINE, DOOR_LAB_DECO_DOT, false};
    else if (partId == "as2_door_closed")
        style = {DOOR_AS2_LINE, DOOR_AS2_DOT, false};
    else if (partId == "as2_door_open")
        style = {DOOR_AS2_LINE, DOOR_AS2_DOT, true};
    else
        return false;
    return true;
}

QLabel *textLabel(const QString &text, const QString &name) {
    auto *label = new QLabel(text);
    label->setObjectName(name);
    label->setWordWrap(true);
    return label;
}
} // namespace

QPixmap legendSymbol(int wallType, const QString &partId) {
    QPixmap pixmap(LEGEND_SYMBOL_WIDTH, LEGEND_SYMBOL_HEIGHT);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QPointF lineStart(3, 9);
    QPointF lineEnd(49, 9);
    QPointF dotCenter(26, 9);
    double dotRadius = 5;

    DoorLegendStyle door{};
    if (!doorLegendStyle(partId, door)) {
        // Wall body: a plain line in the wall accent color.
        painter.setPen(QPen(colors::wallColor(wallType), 4));
        painter.drawLine(lineStart, lineEnd);
        return pixmap;
    }

    QColor line(door.line);
    painter.setPen(QPen(line, 5));
    painter.drawLine(lineStart, lineEnd);
    if (door.hollow) {
        // Punch a transparent hole so the hollow dot shows the card background.
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.drawEllipse(dotCenter, dotRadius, dotRadius);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setBrush(Qt::NoBrush);
    } else {
        painter.setBrush(QColor(door.dot));
    }
    painter.setPen(QPen(line, 2));
    painter.drawEllipse(dotCenter, dotRadius, dotRadius);
    return pixmap;
}

ThemeShelfPanel::ThemeShelfPanel(const Locale &locale, QWidget *parent) : QWidget(parent), locale_(locale) {
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(12);
    scroll_ = new QScrollArea;
    scroll_->setObjectName("themeShelfScrollArea");
    scroll_->setWidgetResizable(true);
    scroll_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    content_ = new QWidget;
    content_->setObjectName("themeShelfScrollContent");
    layout_ = new QVBoxLayout(content_);
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(12);
    layout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
    scroll_->setWidget(content_);
    outer->addWidget(scroll_);
}

void ThemeShelfPanel::setCards(const QList<ShelfCard> &cards, bool compact) {
    while (QLayoutItem *item = layout_->takeAt(0)) {
        delete item->widget();
        delete item;
    }
    cards_.clear();
    current_ = -1;

    for (const auto &data : cards) {
        int row = cards_.size();
        auto *card = new QFrame;
        card->setObjectName("themeCard");
        card->setProperty("selected", false);
        if (compact)
            card->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(10, 10, 10, 10);
        cardLayout->setSpacing(8);

        // Cards without a preview path (Decoration Stamp Series) show text only.
        if (!data.previewPath.isEmpty()) {
            auto *preview = new QLabel;
            preview->setObjectName("themePreview");
            preview->setAlignment(Qt::AlignCenter);
            preview->setFixedHeight(CARD_PREVIEW_HEIGHT);
            QPixmap pixmap(data.previewPath);
            if (pixmap.isNull()) {
                qWarning() << "Missing shelf preview:" << data.previewPath;
                preview->setText(locale_.tr(TextKey::LABEL_PREVIEW_MISSING));
            } else {
                QPixmap scaled = pixmap.scaled(CARD_PREVIEW_WIDTH, CARD_PREVIEW_HEIGHT, Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation);
                preview->setPixmap(scaled);
            }
            cardLayout->addWidget(preview);
        }
        cardLayout->addWidget(textLabel(data.title, "cardTitle"));
        if (!data.description.isEmpty())
            cardLayout->addWidget(textLabel(data.description, "cardDetail"));
        if (!data.detail.isEmpty())
            cardLayout->addWidget(textLabel(data.detail, "cardDetail"));

        if (!data.legend.isEmpty()) {
            auto *legend = new QWidget;
            legend->setObjectName("wallSetLegend");
            auto *legendLayout = new QVBoxLayout(legend);
            legendLayout->setContentsMargins(0, 0, 0, 0);
            legendLayout->setSpacing(2);
            for (const auto &legendRow : data.legend) {
                auto *rowLayout = new QHBoxLayout;
                rowLayout->setContentsMargins(0, 0, 0, 0);
                rowLayout->setSpacing(6);
                auto *symbol = new QLabel;
                symbol->setFixedSize(LEGEND_SYMBOL_WIDTH, LEGEND_SYMBOL_HEIGHT);
                symbol->setPixmap(legendRow.symbol);
                rowLayout->addWidget(symbol);
                rowLayout->addWidget(textLabel(legendRow.label, "wallSetLegendLabel"), 1);
                legendLayout->addLayout(rowLayout);
            }
            cardLayout->addWidget(legend);
        }

        auto *button = new QPushButton(locale_.tr(TextKey::BUTTON_SELECT));
        connect(button, &QPushButton::clicked, this, [this, row] {
            setCurrentRow(row);
        });
        cardLayout->addWidget(button);
        if (compact) {
            layout_->addWidget(card, 0, Qt::AlignTop);
        } else {
            layout_->addWidget(card);
            // The original decoration shelf is laid out before its first selection.
            // Polish the unselected frame first so selecting it does not shift its contents.
            card->ensurePolished();
        }
        cards_.append(card);
    }
    layout_->addStretch(1);
}

int ThemeShelfPanel::currentRow() const {
    return current_;
}

int ThemeShelfPanel::count() const {
    return cards_.size();
}

void ThemeShelfPanel::setCurrentRow(int row, bool reveal) {
    if (row < 0 || row >= cards_.size())
        return;
    current_ = row;
    for (int index = 0; index < cards_.size(); ++index) {
        QFrame *card = cards_[index];
        // The "selected" property drives the highlighted card border in QSS.
        card->setProperty("selected", index == current_);
        card->style()->unpolish(card);
        card->style()->polish(card);
        card->update();
    }
    if (reveal)
        scroll_->ensureWidgetVisible(cards_[row]);
    emit currentRowChanged(row);
}

} // namespace editor
