// Inspector controls for the Gore brush of the active card (corpses with blood, or blood pools).
#pragma once
#include "auto_mapper/core/scatter/gore/gore_builder.h"
#include "editor/i18n/locale.h"
#include <QGroupBox>

class QDoubleSpinBox;
class QFormLayout;
class QSpinBox;

namespace editor {

class GoreBrushPropertiesWidget : public QGroupBox {
    Q_OBJECT
  public:
    explicit GoreBrushPropertiesWidget(const Locale &locale, QWidget *parent = nullptr);

    // Show one card's settings without emitting settingsChanged; the blood pool card hides the corpse spacing.
    void setSettings(const auto_mapper::core::GoreBrushSettings &settings);
    auto_mapper::core::GoreBrushSettings settings() const;

  signals:
    void settingsChanged(const auto_mapper::core::GoreBrushSettings &settings);

  private:
    void emitSettings();

    auto_mapper::core::GoreKind kind_ = auto_mapper::core::GoreKind::Monster;
    QFormLayout *form_ = nullptr;
    QDoubleSpinBox *radius_ = nullptr;
    QDoubleSpinBox *spacing_ = nullptr;
    QSpinBox *bloodCount_ = nullptr;
};

} // namespace editor
