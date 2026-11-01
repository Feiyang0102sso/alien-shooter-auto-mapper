// Inspector controls for the Loot Box brush of the active card, plus the Loot Table button.
#pragma once
#include "auto_mapper/core/loot/loot_box_builder.h"
#include "editor/i18n/locale.h"
#include <QGroupBox>

class QDoubleSpinBox;
class QFormLayout;

namespace editor {

class LootBrushPropertiesWidget : public QGroupBox {
    Q_OBJECT
  public:
    explicit LootBrushPropertiesWidget(const Locale &locale, QWidget *parent = nullptr);

    // Show one card's settings without emitting settingsChanged; the stack row shows on box cards,
    // the broken egg row on the egg card.
    void setSettings(const auto_mapper::core::LootBrushSettings &settings);
    auto_mapper::core::LootBrushSettings settings() const;

  signals:
    void settingsChanged(const auto_mapper::core::LootBrushSettings &settings);
    void lootTableRequested();

  private:
    void emitSettings();

    auto_mapper::core::LootBoxKind kind_ = auto_mapper::core::LootBoxKind::Wood;
    QFormLayout *form_ = nullptr;
    QDoubleSpinBox *radius_ = nullptr;
    QDoubleSpinBox *spacing_ = nullptr;
    QDoubleSpinBox *stackChance_ = nullptr;
    QDoubleSpinBox *brokenChance_ = nullptr;
};

} // namespace editor
