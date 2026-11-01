#include "editor/ui/panels/loot_brush.h"
#include "editor/i18n/text_keys.h"
#include "editor/ui/widget_helpers.h"
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>

namespace editor {
namespace {
// Distances are Map Coordinates; chances are shown in percent and stored as 0..1.
constexpr double DISTANCE_MIN = 10;
constexpr double DISTANCE_MAX = 3000;
constexpr double DISTANCE_STEP = 5;
constexpr double PERCENT_MAX = 100;
constexpr double PERCENT_PER_CHANCE = 100;

QDoubleSpinBox *distanceInput(const QString &name) {
    QDoubleSpinBox *input = decimalInput(name, DISTANCE_MIN, DISTANCE_MAX, 0);
    input->setSingleStep(DISTANCE_STEP);
    return input;
}

QDoubleSpinBox *percentInput(const QString &name) {
    return decimalInput(name, 0, PERCENT_MAX, 1);
}
} // namespace

LootBrushPropertiesWidget::LootBrushPropertiesWidget(const Locale &locale, QWidget *parent)
    : QGroupBox(locale.tr(TextKey::GROUP_LOOT_BRUSH), parent) {
    setObjectName("lootBrushPropertiesGroup");
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 12, 10, 10);
    layout->setSpacing(8);

    form_ = new QFormLayout;
    form_->setSpacing(8);
    radius_ = distanceInput("lootRadius");
    spacing_ = distanceInput("lootSpacing");
    stackChance_ = percentInput("lootStackChance");
    brokenChance_ = percentInput("lootBrokenChance");
    form_->addRow(locale.tr(TextKey::LABEL_LOOT_RADIUS), radius_);
    form_->addRow(locale.tr(TextKey::LABEL_LOOT_SPACING), spacing_);
    form_->addRow(locale.tr(TextKey::LABEL_LOOT_STACK_CHANCE), stackChance_);
    form_->addRow(locale.tr(TextKey::LABEL_LOOT_EGG_BROKEN), brokenChance_);
    layout->addLayout(form_);

    auto *table = new QPushButton(locale.tr(TextKey::BUTTON_LOOT_TABLE));
    table->setObjectName("lootTableButton");
    layout->addWidget(table);
    connect(table, &QPushButton::clicked, this, &LootBrushPropertiesWidget::lootTableRequested);

    for (QDoubleSpinBox *input : {radius_, spacing_, stackChance_, brokenChance_})
        connect(input, &QDoubleSpinBox::valueChanged, this, &LootBrushPropertiesWidget::emitSettings);
    setSettings(auto_mapper::core::default_loot_brush(kind_));
}

void LootBrushPropertiesWidget::setSettings(const auto_mapper::core::LootBrushSettings &settings) {
    kind_ = settings.kind;
    const QList<QDoubleSpinBox *> inputs = {radius_, spacing_, stackChance_, brokenChance_};
    // Filling the inputs is not a user edit: keep settingsChanged quiet meanwhile.
    for (QDoubleSpinBox *input : inputs)
        input->blockSignals(true);
    radius_->setValue(settings.radius);
    spacing_->setValue(settings.spacing);
    stackChance_->setValue(settings.stack_chance * PERCENT_PER_CHANCE);
    brokenChance_->setValue(settings.broken_chance * PERCENT_PER_CHANCE);
    for (QDoubleSpinBox *input : inputs)
        input->blockSignals(false);

    // Boxes stack, eggs break.
    bool eggs = kind_ == auto_mapper::core::LootBoxKind::Egg;
    form_->setRowVisible(stackChance_, !eggs);
    form_->setRowVisible(brokenChance_, eggs);
}

auto_mapper::core::LootBrushSettings LootBrushPropertiesWidget::settings() const {
    auto_mapper::core::LootBrushSettings settings;
    settings.kind = kind_;
    settings.radius = static_cast<float>(radius_->value());
    settings.spacing = static_cast<float>(spacing_->value());
    settings.stack_chance = stackChance_->value() / PERCENT_PER_CHANCE;
    settings.broken_chance = brokenChance_->value() / PERCENT_PER_CHANCE;
    return settings;
}

void LootBrushPropertiesWidget::emitSettings() {
    emit settingsChanged(settings());
}

} // namespace editor
