#include "editor/ui/panels/inspector.h"
#include "editor/hotkeys/hotkey_config.h"
#include "editor/i18n/text_keys.h"
#include "editor/ui/widget_helpers.h"
#include <algorithm>

namespace editor {
namespace {
constexpr int PREVIEW_WIDTH = 260;
constexpr int PREVIEW_HEIGHT = 190;
// Map size inputs step by 50 map units; spacing scales by 0.1.
constexpr double MAP_SIZE_STEP = 50;
constexpr double SPACING_MIN = 0.1;
constexpr double SPACING_MAX = 10;
constexpr double SPACING_STEP = 0.1;

QGroupBox *propertyGroup(const QString &title, const QString &name) {
    auto *group = new QGroupBox(title);
    group->setObjectName(name);
    auto *layout = new QVBoxLayout(group);
    layout->setContentsMargins(10, 12, 10, 10);
    layout->setSpacing(8);
    return group;
}

QVBoxLayout *groupLayout(QGroupBox *group) {
    return qobject_cast<QVBoxLayout *>(group->layout());
}
} // namespace

InspectorPanel::InspectorPanel(const Locale &locale, QWidget *parent) : QWidget(parent), locale_(locale) {
    setObjectName("inspectorPanel");
    setMaximumWidth(320);
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 8, 14, 14);
    layout->setSpacing(12);
    auto *title = new QLabel(locale.tr(TextKey::PANEL_INSPECTOR));
    title->setObjectName("panelTitle");
    layout->addWidget(title);

    // Map size.
    QGroupBox *mapGroup = propertyGroup(locale.tr(TextKey::GROUP_MAP_SIZE), "mapSizeGroup");
    auto *form = new QFormLayout;
    form->setSpacing(8);
    width_ = decimalInput("mapWidth", 1, MAX_MAP_SIZE);
    height_ = decimalInput("mapHeight", 1, MAX_MAP_SIZE);
    width_->setSingleStep(MAP_SIZE_STEP);
    height_->setSingleStep(MAP_SIZE_STEP);
    form->addRow(locale.tr(TextKey::LABEL_MAP_SIZE_X), width_);
    form->addRow(locale.tr(TextKey::LABEL_MAP_SIZE_Y), height_);
    groupLayout(mapGroup)->addLayout(form);
    auto *apply = new QPushButton(locale.tr(TextKey::BUTTON_APPLY_MAP_SIZE));
    apply->setObjectName("applyMapSize");
    groupLayout(mapGroup)->addWidget(apply);
    connect(apply, &QPushButton::clicked, this, [this] {
        emit mapSizeApplied(width_->value(), height_->value());
    });
    layout->addWidget(mapGroup);

    // AS1 ceiling layers.
    ceiling_ = new AS1CeilingPropertiesWidget(locale);
    layout->addWidget(ceiling_);

    // Component preview.
    QGroupBox *componentGroup = propertyGroup(locale.tr(TextKey::LABEL_COMPONENT_PREVIEW), "componentGroup");
    QVBoxLayout *componentLayout = groupLayout(componentGroup);
    theme_ = new QLabel;
    theme_->setObjectName("activeThemeLabel");
    componentLayout->addWidget(theme_);
    components_ = new QComboBox;
    components_->setObjectName("componentCombo");
    componentLayout->addWidget(components_);
    connect(components_, &QComboBox::currentIndexChanged, this, &InspectorPanel::componentChosen);
    stampDetails_ = wrappingLabel({});
    stampDetails_->setObjectName("stampDetailLabel");
    preview_ = new QLabel;
    preview_->setObjectName("componentPreview");
    preview_->setFixedSize(PREVIEW_WIDTH, PREVIEW_HEIGHT);
    preview_->setAlignment(Qt::AlignCenter);
    componentLayout->addWidget(preview_);
    componentLayout->addWidget(stampDetails_);
    notice_ = wrappingLabel({});
    notice_->setObjectName("stampPreviewNotice");
    componentLayout->addWidget(notice_);
    setSpritePreviewReady(false);
    nvid_ = wrappingLabel({});
    nvid_->setObjectName("nvidLabel");
    componentLayout->addWidget(nvid_);
    layout->addWidget(componentGroup);

    // Selected decoration properties.
    selectionGroup_ = propertyGroup(locale.tr(TextKey::GROUP_DECORATION_PROPERTY), "decorationPropertiesGroup");
    spacingWidget_ = new QWidget;
    auto *spacingForm = new QFormLayout(spacingWidget_);
    spacingForm->setContentsMargins(0, 0, 0, 0);
    itemSpacing_ = decimalInput("itemSpacing", SPACING_MIN, SPACING_MAX, 2);
    rowSpacing_ = decimalInput("rowSpacing", SPACING_MIN, SPACING_MAX, 2);
    itemSpacing_->setSingleStep(SPACING_STEP);
    rowSpacing_->setSingleStep(SPACING_STEP);
    spacingForm->addRow(locale.tr(TextKey::LABEL_ITEM_SPACING), itemSpacing_);
    spacingForm->addRow(locale.tr(TextKey::LABEL_COLUMN_SPACING), rowSpacing_);
    groupLayout(selectionGroup_)->addWidget(spacingWidget_);
    auto emitSpacing = [this] {
        emit spacingChanged(itemSpacing_->value(), rowSpacing_->value());
    };
    connect(itemSpacing_, &QDoubleSpinBox::valueChanged, this, emitSpacing);
    connect(rowSpacing_, &QDoubleSpinBox::valueChanged, this, emitSpacing);
    QString deleteText = locale.tr(TextKey::BUTTON_DELETE_DECORATION) + " " +
                         hotkeys::hotkeyHint(hotkeys::COMMAND_DELETE_DECORATION);
    delete_ = new QPushButton(deleteText);
    delete_->setObjectName("deleteDecoration");
    connect(delete_, &QPushButton::clicked, this, &InspectorPanel::deleteDecorationRequested);
    groupLayout(selectionGroup_)->addWidget(delete_);
    layout->addWidget(selectionGroup_);

    // Tool properties (eraser size).
    eraser_ = new EraserPropertiesWidget(locale);
    layout->addWidget(eraser_);
    layout->addStretch();
}

void InspectorPanel::setMapSize(double width, double height) {
    width_->setValue(width);
    height_->setValue(height);
}

AS1CeilingPropertiesWidget *InspectorPanel::ceiling() const {
    return ceiling_;
}

EraserPropertiesWidget *InspectorPanel::eraser() const {
    return eraser_;
}

void InspectorPanel::setComponentSet(const QString &title, const QStringList &itemLabels, int selectedIndex) {
    QSignalBlocker blocker(components_);
    theme_->setText(title);
    components_->clear();
    components_->addItems(itemLabels);
    components_->setCurrentIndex(selectedIndex);
}

int InspectorPanel::componentIndex() const {
    return components_->currentIndex();
}

int InspectorPanel::componentCount() const {
    return components_->count();
}

void InspectorPanel::selectRelativeComponent(int step) {
    if (components_->count() == 0)
        return;
    int next = std::clamp(components_->currentIndex() + step, 0, components_->count() - 1);
    components_->setCurrentIndex(next);
}

void InspectorPanel::showWallPart(const QString &imagePath, const QString &nvidText) {
    nvid_->setText(nvidText);
    nvid_->setVisible(true);
    stampDetails_->setVisible(false);
    notice_->setVisible(false);
    preview_->setVisible(true);
    setPreviewNotConfigured(false);
    showPreview(imagePath);
}

void InspectorPanel::setSpritePreviewReady(bool ready) {
    if (ready)
        notice_->setText(locale_.tr(TextKey::LABEL_STAMP_PREVIEW_READY));
    else
        notice_->setText(locale_.tr(TextKey::LABEL_STAMP_PREVIEW_UNSET));
    // QSS colors the notice by this property: green when ready, red otherwise.
    notice_->setProperty("ready", ready);
    notice_->style()->unpolish(notice_);
    notice_->style()->polish(notice_);
}

void InspectorPanel::showStamp(int memberCount, bool spritesReady, const QImage &preview) {
    stampDetails_->setText(locale_.tr(TextKey::LABEL_STAMP_MEMBER_COUNT, {{"member_count", QString::number(memberCount)}}));
    stampDetails_->setVisible(true);
    notice_->setVisible(true);
    nvid_->setVisible(false);
    preview_->setVisible(true);
    preview_->clear();
    // QSS turns the box black with red text while no game folder is configured.
    setPreviewNotConfigured(!spritesReady);
    if (!spritesReady) {
        preview_->setText(locale_.tr(TextKey::LABEL_PREVIEW_NOT_CONFIGURED));
        return;
    }
    if (preview.isNull()) {
        preview_->setText(locale_.tr(TextKey::LABEL_PREVIEW_MISSING));
        return;
    }
    // Scaled like the wall part previews.
    QPixmap pixmap = QPixmap::fromImage(preview);
    preview_->setPixmap(pixmap.scaled(preview_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void InspectorPanel::setPreviewNotConfigured(bool notConfigured) {
    preview_->setProperty("notConfigured", notConfigured);
    preview_->style()->unpolish(preview_);
    preview_->style()->polish(preview_);
}

void InspectorPanel::showPreview(const QString &imagePath) {
    QPixmap pixmap(imagePath);
    preview_->clear();
    if (pixmap.isNull()) {
        qWarning() << "Missing component preview:" << imagePath;
        preview_->setText(locale_.tr(TextKey::LABEL_PREVIEW_MISSING));
        return;
    }
    preview_->setPixmap(pixmap.scaled(preview_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void InspectorPanel::setDecorationSelection(const Decoration *decoration) {
    bool visible = decoration != nullptr;
    selectionGroup_->setVisible(visible);
    delete_->setEnabled(visible);
    if (!visible)
        return;
    // Stamps keep their authored layout, so only arrays expose spacing.
    spacingWidget_->setVisible(decoration->type != DECORATION_TYPE_ROOM_STAMP);
    QSignalBlocker itemBlocker(itemSpacing_);
    QSignalBlocker rowBlocker(rowSpacing_);
    itemSpacing_->setValue(decoration->itemSpacing);
    rowSpacing_->setValue(decoration->rowSpacing);
}

} // namespace editor
