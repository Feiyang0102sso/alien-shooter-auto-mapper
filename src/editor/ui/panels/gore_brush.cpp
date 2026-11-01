#include "editor/ui/panels/gore_brush.h"
#include "editor/i18n/text_keys.h"
#include "editor/ui/widget_helpers.h"
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QVBoxLayout>

namespace editor {
namespace {
// Distances are Map Coordinates; the blood count is sprites per corpse (per dab on the pool card).
constexpr double DISTANCE_MIN = 10;
constexpr double DISTANCE_MAX = 3000;
constexpr double DISTANCE_STEP = 5;
constexpr int BLOOD_COUNT_MAX = 200;

QDoubleSpinBox *distanceInput(const QString &name) {
    QDoubleSpinBox *input = decimalInput(name, DISTANCE_MIN, DISTANCE_MAX, 0);
    input->setSingleStep(DISTANCE_STEP);
    return input;
}
} // namespace

GoreBrushPropertiesWidget::GoreBrushPropertiesWidget(const Locale &locale, QWidget *parent)
    : QGroupBox(locale.tr(TextKey::GROUP_GORE_BRUSH), parent) {
    setObjectName("goreBrushPropertiesGroup");
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 12, 10, 10);
    layout->setSpacing(8);

    form_ = new QFormLayout;
    form_->setSpacing(8);
    radius_ = distanceInput("goreRadius");
    spacing_ = distanceInput("goreSpacing");
    bloodCount_ = new QSpinBox;
    bloodCount_->setObjectName("goreBloodCount");
    bloodCount_->setRange(0, BLOOD_COUNT_MAX);
    bloodCount_->setKeyboardTracking(false);
    form_->addRow(locale.tr(TextKey::LABEL_GORE_RADIUS), radius_);
    form_->addRow(locale.tr(TextKey::LABEL_GORE_SPACING), spacing_);
    form_->addRow(locale.tr(TextKey::LABEL_GORE_BLOOD_COUNT), bloodCount_);
    layout->addLayout(form_);

    connect(radius_, &QDoubleSpinBox::valueChanged, this, &GoreBrushPropertiesWidget::emitSettings);
    connect(spacing_, &QDoubleSpinBox::valueChanged, this, &GoreBrushPropertiesWidget::emitSettings);
    connect(bloodCount_, &QSpinBox::valueChanged, this, &GoreBrushPropertiesWidget::emitSettings);
    setSettings(auto_mapper::core::default_gore_brush(kind_));
}

void GoreBrushPropertiesWidget::setSettings(const auto_mapper::core::GoreBrushSettings &settings) {
    kind_ = settings.kind;
    // Filling the inputs is not a user edit: keep settingsChanged quiet meanwhile.
    radius_->blockSignals(true);
    spacing_->blockSignals(true);
    bloodCount_->blockSignals(true);
    radius_->setValue(settings.radius);
    spacing_->setValue(settings.spacing);
    bloodCount_->setValue(settings.blood_count);
    radius_->blockSignals(false);
    spacing_->blockSignals(false);
    bloodCount_->blockSignals(false);

    // A blood pool has no corpses to keep apart.
    bool pool = kind_ == auto_mapper::core::GoreKind::BloodPool;
    form_->setRowVisible(spacing_, !pool);
}

auto_mapper::core::GoreBrushSettings GoreBrushPropertiesWidget::settings() const {
    auto_mapper::core::GoreBrushSettings settings;
    settings.kind = kind_;
    settings.radius = static_cast<float>(radius_->value());
    settings.spacing = static_cast<float>(spacing_->value());
    settings.blood_count = bloodCount_->value();
    return settings;
}

void GoreBrushPropertiesWidget::emitSettings() {
    emit settingsChanged(settings());
}

} // namespace editor
