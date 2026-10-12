// Scrollable card shelf for Editor Sets: Wall Set cards or Decoration Stamp Series cards.
#pragma once
#include "editor/i18n/locale.h"
#include <QList>
#include <QPixmap>
#include <QWidget>

class QFrame;
class QScrollArea;
class QVBoxLayout;

namespace editor {

// One row of the Wall Set Legend: a line/door symbol and the part name.
struct LegendRow {
    QPixmap symbol;
    QString label;
};

// Everything one card displays; built by LeftShelfPanel from the catalog.
struct ShelfCard {
    QString previewPath;
    QString title;
    QString description;
    // Extra detail line (stamp series variant count); empty for wall sets.
    QString detail;
    QList<LegendRow> legend;
};

// Legend symbol for one drawable part: a plain wall line, or a door line with its status dot.
QPixmap legendSymbol(int wallType, const QString &partId);

class ThemeShelfPanel : public QWidget {
    Q_OBJECT
  public:
    explicit ThemeShelfPanel(const Locale &locale, QWidget *parent = nullptr);

    // `compact` wall cards shrink to their content; stamp cards stretch like the legacy stamp shelf.
    void setCards(const QList<ShelfCard> &cards, bool compact);
    int currentRow() const;
    int count() const;
    void setCurrentRow(int row, bool reveal = true);

  signals:
    void currentRowChanged(int row);

  private:
    const Locale &locale_;
    QScrollArea *scroll_ = nullptr;
    QWidget *content_ = nullptr;
    QVBoxLayout *layout_ = nullptr;
    QList<QFrame *> cards_;
    int current_ = -1;
};

} // namespace editor
