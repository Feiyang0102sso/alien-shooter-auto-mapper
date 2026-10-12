#include "editor/ui/panels/left_shelf.h"
#include "editor/i18n/text_keys.h"
#include "editor/project/project.h"
#include <QButtonGroup>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <algorithm>

namespace editor {

LeftShelfPanel::LeftShelfPanel(const Catalog &catalog, QWidget *parent) : QWidget(parent), catalog_(catalog) {
    const Locale &locale = catalog.locale;
    setObjectName("leftShelfPanel");
    setMinimumWidth(292);
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 8, 14, 14);
    layout->setSpacing(12);

    // Game family row: AS1 / AS2 plus the exact AS2 Series format.
    auto *family = new QHBoxLayout;
    family->setSpacing(8);
    auto *familyGroup = new QButtonGroup(this);
    as1Button_ = new QPushButton("AS1");
    as2Button_ = new QPushButton("AS2");
    for (QPushButton *button : {as1Button_, as2Button_}) {
        button->setObjectName("gameFamilyButton");
        button->setCheckable(true);
        familyGroup->addButton(button);
        family->addWidget(button);
    }
    connect(as1Button_, &QPushButton::clicked, this, [this] {
        emit familyRequested(true);
    });
    connect(as2Button_, &QPushButton::clicked, this, [this] {
        emit familyRequested(false);
    });
    as2Format_ = new QComboBox;
    as2Format_->setObjectName("as2FormatCombo");
    as2Format_->addItems({PROJECT_VERSION_AS2, PROJECT_VERSION_AS2R, PROJECT_VERSION_AS2OE});
    // AS2R is the default target when the user first switches to the AS2 family.
    as2Format_->setCurrentText(PROJECT_VERSION_AS2R);
    family->addWidget(as2Format_);
    connect(as2Format_, &QComboBox::currentTextChanged, this, &LeftShelfPanel::as2FormatRequested);
    layout->addLayout(family);

    // Shelf mode row: Wall Sets / Decoration Sets.
    auto *modes = new QHBoxLayout;
    modes->setSpacing(8);
    auto *modeGroup = new QButtonGroup(this);
    wallsButton_ = new QPushButton(locale.tr(TextKey::SHELF_MODE_WALL_SETS));
    decorationsButton_ = new QPushButton(locale.tr(TextKey::SHELF_MODE_DECORATION_SETS));
    decorationsButton_->setToolTip(locale.tr(TextKey::TOOLTIP_DECORATION_SETS_AS2_ONLY));
    for (QPushButton *button : {wallsButton_, decorationsButton_}) {
        button->setObjectName("shelfModeButton");
        button->setCheckable(true);
        modes->addWidget(button);
        modeGroup->addButton(button);
    }
    layout->addLayout(modes);
    connect(wallsButton_, &QPushButton::clicked, this, [this] {
        emit shelfModeRequested(false);
    });
    connect(decorationsButton_, &QPushButton::clicked, this, [this] {
        emit shelfModeRequested(true);
    });

    shelf_ = new ThemeShelfPanel(locale);
    shelf_->setObjectName("setShelf");
    layout->addWidget(shelf_);
    connect(shelf_, &ThemeShelfPanel::currentRowChanged, this, &LeftShelfPanel::setChosen);
}

void LeftShelfPanel::setProjectVersion(const QString &version) {
    bool isAS1 = version == PROJECT_VERSION_AS1;
    QSignalBlocker blocker(as2Format_);
    if (!isAS1)
        as2Format_->setCurrentText(version);
    as2Format_->setVisible(!isAS1);
    as1Button_->setChecked(isAS1);
    as2Button_->setChecked(!isAS1);
    // Decoration Stamps are AS2 Series content.
    decorationsButton_->setEnabled(!isAS1);
}

void LeftShelfPanel::setDecorationShelf(bool decorations) {
    wallsButton_->setChecked(!decorations);
    decorationsButton_->setChecked(decorations);
}

void LeftShelfPanel::showWallSets(const QList<int> &setIds) {
    QList<ShelfCard> cards;
    for (int setId : setIds) {
        WallSet wall = catalog_.wallSet(setId);
        ShelfCard card;
        card.previewPath = catalog_.imagePath(wall.preview);
        card.title = catalog_.wallShortLabel(setId);
        card.description = catalog_.wallDescription(setId);
        for (const auto &part : wall.parts)
            card.legend.append({legendSymbol(part.wallType, part.id), catalog_.partLabel(part)});
        cards.append(card);
    }
    QSignalBlocker blocker(shelf_);
    shelf_->setCards(cards, true);
}

void LeftShelfPanel::showStampSeries(const QStringList &seriesIds) {
    QList<ShelfCard> cards;
    for (const QString &seriesId : seriesIds) {
        ShelfCard card;
        card.previewPath = catalog_.seriesImage(seriesId);
        card.title = catalog_.seriesLabel(seriesId);
        card.description = catalog_.seriesDescription(seriesId);
        int variantCount = catalog_.seriesVariants(seriesId).size();
        card.detail = catalog_.locale.tr(TextKey::LABEL_STAMP_VARIANT_COUNT,
                                         {{"variant_count", QString::number(variantCount)}});
        cards.append(card);
    }
    QSignalBlocker blocker(shelf_);
    shelf_->setCards(cards, false);
}

ThemeShelfPanel *LeftShelfPanel::shelf() const {
    return shelf_;
}

void LeftShelfPanel::selectRelativeSet(int step) {
    if (shelf_->count() == 0)
        return;
    int next = std::clamp(shelf_->currentRow() + step, 0, shelf_->count() - 1);
    shelf_->setCurrentRow(next);
}

} // namespace editor
