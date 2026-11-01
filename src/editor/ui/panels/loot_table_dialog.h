// Loot Table editor of one Loot Box card: whole loot text lines with their percents, plus cfg presets.
#pragma once
#include "editor/loot_presets.h"
#include "editor/i18n/locale.h"
#include <QDialog>
#include <vector>

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

namespace editor {

class LootTableDialog : public QDialog {
    Q_OBJECT
  public:
    // Presets come from loot_presets.ini and are shared by every card; `kindLabel` only titles the dialog.
    LootTableDialog(const Locale &locale, const QString &kindLabel,
                    const std::vector<auto_mapper::core::LootLine> &lines, QWidget *parent = nullptr);

    // Rows as edited; rows whose text is blank are dropped.
    std::vector<auto_mapper::core::LootLine> lines() const;
    // Sum of the percents of all rows, blank rows included.
    double totalPercent() const;

  protected:
    // Refits a loot text box whenever its width changes (dialog resize), since the wrap depends on it.
    bool eventFilter(QObject *watched, QEvent *event) override;

  private:
    struct Row {
        QWidget *widget = nullptr;
        QPlainTextEdit *text = nullptr;
        QDoubleSpinBox *percent = nullptr;
    };

    void addRow(const auto_mapper::core::LootLine &line);
    void removeRow(QWidget *rowWidget);
    void setLines(const std::vector<auto_mapper::core::LootLine> &lines);
    // Remainder text, red when overfull; OK only while the total stays within 100%.
    void updateTotal();
    // A loot text wraps inside the wide text column; its box grows downwards to show every line.
    void fitTextHeight(QPlainTextEdit *text);
    void refreshPresets(const QString &select);
    void loadPreset();
    void savePreset();
    void deletePreset();

    const Locale &locale_;
    QList<LootPreset> presets_;
    QList<Row> rows_;
    QComboBox *presetCombo_ = nullptr;
    QScrollArea *scroll_ = nullptr;
    QWidget *rowsWidget_ = nullptr;
    QVBoxLayout *rowsLayout_ = nullptr;
    QLabel *total_ = nullptr;
    QPushButton *ok_ = nullptr;
};

} // namespace editor
