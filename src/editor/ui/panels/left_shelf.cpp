#include "editor/ui/panels/left_shelf.h"
#include "editor/i18n/text_keys.h"
#include "editor/project/project.h"
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
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

    // Shelf mode buttons in two rows: Wall Sets / Decoration Sets, then Loot Boxes / Gore (AS2 Series only).
    auto *modes = new QGridLayout;
    modes->setSpacing(8);
    auto *modeGroup = new QButtonGroup(this);
    wallsButton_ = new QPushButton(locale.tr(TextKey::SHELF_MODE_WALL_SETS));
    decorationsButton_ = new QPushButton(locale.tr(TextKey::SHELF_MODE_DECORATION_SETS));
    decorationsButton_->setToolTip(locale.tr(TextKey::TOOLTIP_DECORATION_SETS));
    lootButton_ = new QPushButton(locale.tr(TextKey::SHELF_MODE_LOOT));
    lootButton_->setToolTip(locale.tr(TextKey::TOOLTIP_LOOT));
    goreButton_ = new QPushButton(locale.tr(TextKey::SHELF_MODE_GORE));
    goreButton_->setToolTip(locale.tr(TextKey::TOOLTIP_GORE));
    for (QPushButton *button : {wallsButton_, decorationsButton_, lootButton_, goreButton_}) {
        button->setObjectName("shelfModeButton");
        button->setCheckable(true);
        modeGroup->addButton(button);
    }
    modes->addWidget(wallsButton_, 0, 0);
    modes->addWidget(decorationsButton_, 0, 1);
    modes->addWidget(lootButton_, 1, 0);
    modes->addWidget(goreButton_, 1, 1);
    layout->addLayout(modes);
    connect(wallsButton_, &QPushButton::clicked, this, [this] {
        emit shelfModeRequested(ShelfMode::Walls);
    });
    connect(decorationsButton_, &QPushButton::clicked, this, [this] {
        emit shelfModeRequested(ShelfMode::Decorations);
    });
    connect(lootButton_, &QPushButton::clicked, this, [this] {
        emit shelfModeRequested(ShelfMode::Loot);
    });
    connect(goreButton_, &QPushButton::clicked, this, [this] {
        emit shelfModeRequested(ShelfMode::Gore);
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
    // The Loot Box brush writes AS2 nameText only, and the Gore VIDs are AS2 objects: AS1 hides the second row.
    lootButton_->setVisible(!isAS1);
    goreButton_->setVisible(!isAS1);
}

void LeftShelfPanel::setShelfMode(ShelfMode mode) {
    wallsButton_->setChecked(mode == ShelfMode::Walls);
    decorationsButton_->setChecked(mode == ShelfMode::Decorations);
    lootButton_->setChecked(mode == ShelfMode::Loot);
    goreButton_->setChecked(mode == ShelfMode::Gore);
}

void LeftShelfPanel::showWallSets(const QList<int> &setIds) {
    QList<ShelfCard> cards;
    for (int setId : setIds) {
        WallSet wall = catalog_.wallSet(setId);
        ShelfCard card;
        card.previewPath = catalog_.imagePath(wall.preview);
        card.title = catalog_.wallShortLabel(setId);
        card.description = catalog_.wallDescription(setId);
        for (const auto &part : wall.parts) {
            if (part.isCurveWall())
                card.legend.append({curveWallLegendSymbol(part.curveWallType), catalog_.partLabel(part)});
            else
                card.legend.append({legendSymbol(part.wallType, part.id), catalog_.partLabel(part)});
        }
        cards.append(card);
    }
    QSignalBlocker blocker(shelf_);
    shelf_->setCards(cards, true);
}

void LeftShelfPanel::showStampSeries(bool as1, const QStringList &seriesIds) {
    QList<ShelfCard> cards;
    for (const QString &seriesId : seriesIds) {
        // The library maps carry no preview images: MainWindow renders each series' first stamp into the box.
        ShelfCard card;
        card.spritePreview = true;
        card.title = catalog_.seriesLabel(seriesId);
        card.description = catalog_.seriesDescription(seriesId);
        int variantCount = catalog_.seriesVariants(as1, seriesId).size();
        card.detail = catalog_.locale.tr(TextKey::LABEL_STAMP_VARIANT_COUNT,
                                         {{"variant_count", QString::number(variantCount)}});
        cards.append(card);
    }
    QSignalBlocker blocker(shelf_);
    shelf_->setCards(cards, true);
}

void LeftShelfPanel::showLootKinds() {
    const Locale &locale = catalog_.locale;
    QList<ShelfCard> cards;
    for (LootBoxKind kind : LOOT_KINDS) {
        // No preview images: MainWindow renders a sample pile from the game sprites into the box.
        QString id = lootKindId(kind);
        ShelfCard card;
        card.spritePreview = true;
        card.title = locale.tr(QString("loot.%1.label").arg(id));
        card.description = locale.tr(QString("loot.%1.description").arg(id));
        cards.append(card);
    }
    QSignalBlocker blocker(shelf_);
    shelf_->setCards(cards, true);
}

void LeftShelfPanel::showGoreKinds() {
    const Locale &locale = catalog_.locale;
    QList<ShelfCard> cards;
    for (GoreKind kind : GORE_KINDS) {
        // No preview images: MainWindow renders a sample corpse (or pool) from the game sprites into the box.
        QString id = goreKindId(kind);
        ShelfCard card;
        card.spritePreview = true;
        card.title = locale.tr(QString("gore.%1.label").arg(id));
        card.description = locale.tr(QString("gore.%1.description").arg(id));
        cards.append(card);
    }
    QSignalBlocker blocker(shelf_);
    shelf_->setCards(cards, true);
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
