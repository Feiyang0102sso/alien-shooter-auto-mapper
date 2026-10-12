#include "editor/ui/tools/eraser/eraser_properties.h"
#include "editor/i18n/text_keys.h"
#include "editor/ui/tools/eraser/eraser_geometry.h"
#include "editor/ui/widget_helpers.h"

namespace editor {

EraserPropertiesWidget::EraserPropertiesWidget(const Locale &locale, QWidget *parent)
    : QGroupBox(locale.tr(TextKey::GROUP_TOOL_PROPERTIES), parent) {
    setObjectName("toolPropertiesGroup");
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 12, 10, 10);
    layout->setSpacing(8);
    layout->addWidget(wrappingLabel(locale.tr(TextKey::LABEL_ERASER_SIZE)));

    spin_ = new QSpinBox;
    spin_->setObjectName("eraserSize");
    spin_->setRange(ERASER_MIN_SIZE, ERASER_MAX_SIZE);
    slider_ = new QSlider(Qt::Horizontal);
    slider_->setObjectName("eraserSizeSlider");
    slider_->setRange(ERASER_MIN_SIZE, ERASER_MAX_SIZE);
    auto *row = new QHBoxLayout;
    row->setSpacing(8);
    row->addWidget(slider_, 1);
    row->addWidget(spin_);
    layout->addLayout(row);

    // Slider and spin box mirror each other; the spin box is the single source of the signal.
    connect(slider_, &QSlider::valueChanged, spin_, &QSpinBox::setValue);
    connect(spin_, &QSpinBox::valueChanged, slider_, &QSlider::setValue);
    connect(spin_, &QSpinBox::valueChanged, this, &EraserPropertiesWidget::sizeChanged);
}

int EraserPropertiesWidget::size() const {
    return spin_->value();
}

} // namespace editor
