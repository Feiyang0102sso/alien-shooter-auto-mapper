#include "editor/ui/panels/as1_ceiling.h"
#include "auto_mapper/core/wall_builder/ceiling_profiles_as1.h"
#include "editor/i18n/text_keys.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

namespace editor {

CeilingLayerControl::CeilingLayerControl(const QString &labelText, const QString &spinName, int minimum,
                                         int maximum, QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    layout->addWidget(new QLabel(labelText));
    auto *row = new QHBoxLayout;
    row->setSpacing(8);
    slider_ = new QSlider(Qt::Horizontal);
    slider_->setRange(minimum, maximum);
    spin_ = new QSpinBox;
    spin_->setObjectName(spinName);
    spin_->setRange(minimum, maximum);
    spin_->setKeyboardTracking(false);
    row->addWidget(slider_, 1);
    row->addWidget(spin_);
    layout->addLayout(row);

    connect(slider_, &QSlider::valueChanged, this, [this](int value) {
        QSignalBlocker blocker(spin_);
        spin_->setValue(value);
        emit valueChanged(value);
    });
    connect(spin_, &QSpinBox::valueChanged, this, [this](int value) {
        QSignalBlocker blocker(slider_);
        slider_->setValue(value);
        emit valueChanged(value);
    });
}

int CeilingLayerControl::value() const {
    return spin_->value();
}

void CeilingLayerControl::setValue(int value) {
    QSignalBlocker sliderBlocker(slider_);
    QSignalBlocker spinBlocker(spin_);
    slider_->setValue(value);
    spin_->setValue(value);
}

AS1CeilingPropertiesWidget::AS1CeilingPropertiesWidget(const Locale &locale, QWidget *parent)
    : QGroupBox(locale.tr(TextKey::GROUP_AS1_CEILING), parent) {
    setObjectName("as1CeilingPropertiesGroup");
    setToolTip(locale.tr(TextKey::TOOLTIP_AS1_CEILING_LAYERS));

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 12, 10, 10);
    layout->setSpacing(8);
    using namespace auto_mapper::core;
    standard_ = new CeilingLayerControl(locale.tr(TextKey::LABEL_AS1_STANDARD_CEILING_LAYERS), "standardLayers",
                                        AS1_CEILING_LAYER_COUNT_MIN, AS1_CEILING_LAYER_COUNT_MAX);
    standard_->setObjectName("as1StandardCeilingLayerControl");
    layout->addWidget(standard_);
    lab_ = new CeilingLayerControl(locale.tr(TextKey::LABEL_AS1_LAB_CEILING_LAYERS), "labLayers",
                                   AS1_CEILING_LAYER_COUNT_MIN, AS1_CEILING_LAYER_COUNT_MAX);
    lab_->setObjectName("as1LabCeilingLayerControl");
    layout->addWidget(lab_);

    auto emitLayers = [this] {
        emit layersChanged(standard_->value(), lab_->value());
    };
    connect(standard_, &CeilingLayerControl::valueChanged, this, emitLayers);
    connect(lab_, &CeilingLayerControl::valueChanged, this, emitLayers);
    setLayerCounts(0, 0);
}

void AS1CeilingPropertiesWidget::setLayerCounts(int standardLayerCount, int labLayerCount) {
    // 0 means the project keeps the core default.
    if (standardLayerCount == 0)
        standardLayerCount = auto_mapper::core::DEFAULT_AS1_CEILING_LAYER_SETTINGS.standard_total_layer_count;
    if (labLayerCount == 0)
        labLayerCount = auto_mapper::core::DEFAULT_AS1_CEILING_LAYER_SETTINGS.lab_total_layer_count;
    standard_->setValue(standardLayerCount);
    lab_->setValue(labLayerCount);
}

void AS1CeilingPropertiesWidget::setProjectState(bool isAS1, bool generateCeiling) {
    setVisible(isAS1);
    setEnabled(isAS1 && generateCeiling);
}

} // namespace editor
