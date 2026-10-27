#include "editor/ui/panels/sprite_preview_dialog.h"
#include "editor/i18n/text_keys.h"
#include <QCheckBox>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

namespace editor {
namespace {
constexpr int DIALOG_WIDTH = 400;
// One slider step per percent; Page Up / Down and clicks on the groove move ten.
constexpr int OPACITY_PAGE_STEP = 10;
// Wide enough for "100%" so the slider does not jump while the text changes.
constexpr int OPACITY_LABEL_WIDTH = 52;
} // namespace

SpritePreviewDialog::SpritePreviewDialog(const Locale &locale, QWidget *parent) : QDialog(parent), locale_(locale) {
    setObjectName("spritePreviewDialog");
    setWindowTitle(locale.tr(TextKey::DIALOG_SPRITE_PREVIEW));
    setModal(false);
    setFixedWidth(DIALOG_WIDTH);
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(10);

    as2FolderLabel_ = addGameFolderRow(GameFamily::AS2, TextKey::LABEL_AS2_GAME_FOLDER);
    as1FolderLabel_ = addGameFolderRow(GameFamily::AS1, TextKey::LABEL_AS1_GAME_FOLDER);

    // Option cards: the whole row toggles, styled by QSS (#spritePreviewDialog QCheckBox).
    crossesWithSprites_ = new QCheckBox(locale.tr(TextKey::CHECK_STAMP_CROSSES_WITH_SPRITES));
    crossesWithSprites_->setObjectName("stampCrossesWithSpritesCheck");
    layout->addWidget(crossesWithSprites_);
    connect(crossesWithSprites_, &QCheckBox::toggled, this, &SpritePreviewDialog::crossesWithSpritesChanged);
    alwaysShowFrame_ = new QCheckBox(locale.tr(TextKey::CHECK_STAMP_ALWAYS_SHOW_FRAME));
    alwaysShowFrame_->setObjectName("stampAlwaysShowFrameCheck");
    layout->addWidget(alwaysShowFrame_);
    connect(alwaysShowFrame_, &QCheckBox::toggled, this, &SpritePreviewDialog::alwaysShowFrameChanged);

    layout->addWidget(new QLabel(locale.tr(TextKey::LABEL_STAMP_SPRITE_OPACITY)));
    auto *opacityRow = new QHBoxLayout;
    spriteOpacity_ = new QSlider(Qt::Horizontal);
    spriteOpacity_->setObjectName("stampSpriteOpacitySlider");
    // The slider cannot leave the limits, the same ones the cfg reader clamps to.
    spriteOpacity_->setRange(STAMP_SPRITE_OPACITY_MIN, STAMP_SPRITE_OPACITY_MAX);
    spriteOpacity_->setPageStep(OPACITY_PAGE_STEP);
    opacityRow->addWidget(spriteOpacity_, 1);
    spriteOpacityValue_ = new QLabel;
    spriteOpacityValue_->setObjectName("stampSpriteOpacityValue");
    spriteOpacityValue_->setFixedWidth(OPACITY_LABEL_WIDTH);
    spriteOpacityValue_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    opacityRow->addWidget(spriteOpacityValue_);
    layout->addLayout(opacityRow);
    connect(spriteOpacity_, &QSlider::valueChanged, this, [this](int value) {
        syncOpacityLabel();
        emit spriteOpacityChanged(value);
        // Clicks and keys change the value without a drag; a drag commits once on release.
        if (!spriteOpacity_->isSliderDown())
            emit spriteOpacityCommitted(value);
    });
    connect(spriteOpacity_, &QSlider::sliderReleased, this, [this] {
        emit spriteOpacityCommitted(spriteOpacity_->value());
    });
    syncOpacityLabel();
}

QLabel *SpritePreviewDialog::addGameFolderRow(GameFamily family, const char *titleKey) {
    auto *layout = static_cast<QVBoxLayout *>(this->layout());
    auto *title = new QLabel(locale_.tr(titleKey));
    title->setObjectName("spritePreviewSectionLabel");
    title->setWordWrap(true);
    layout->addWidget(title);
    auto *row = new QHBoxLayout;
    auto *path = new QLabel(locale_.tr(TextKey::LABEL_GAME_FOLDER_UNSET));
    path->setObjectName("gameFolderPathLabel");
    path->setWordWrap(true);
    path->setTextInteractionFlags(Qt::TextSelectableByMouse);
    row->addWidget(path, 1);
    auto *choose = new QPushButton(locale_.tr(TextKey::BUTTON_CHOOSE_GAME_FOLDER));
    if (family == GameFamily::AS1)
        choose->setObjectName("chooseAs1GameFolderButton");
    else
        choose->setObjectName("chooseAs2GameFolderButton");
    row->addWidget(choose);
    layout->addLayout(row);
    connect(choose, &QPushButton::clicked, this, [this, family] {
        emit chooseGameFolderRequested(family);
    });
    return path;
}

void SpritePreviewDialog::setGameFolder(GameFamily family, const QString &path) {
    QLabel *label = as2FolderLabel_;
    if (family == GameFamily::AS1)
        label = as1FolderLabel_;
    if (path.isEmpty()) {
        label->setText(locale_.tr(TextKey::LABEL_GAME_FOLDER_UNSET));
        return;
    }
    label->setText(QDir::toNativeSeparators(path));
}

void SpritePreviewDialog::setOptions(bool crossesWithSprites, bool alwaysShowFrame, int spriteOpacity) {
    QSignalBlocker crossesBlocker(crossesWithSprites_);
    QSignalBlocker frameBlocker(alwaysShowFrame_);
    QSignalBlocker opacityBlocker(spriteOpacity_);
    crossesWithSprites_->setChecked(crossesWithSprites);
    alwaysShowFrame_->setChecked(alwaysShowFrame);
    spriteOpacity_->setValue(spriteOpacity);
    syncOpacityLabel();
}

void SpritePreviewDialog::syncOpacityLabel() {
    spriteOpacityValue_->setText(QString::number(spriteOpacity_->value()) + "%");
}

} // namespace editor
