// Scrollable card shelf for Editor Sets: Wall Set cards or Decoration Stamp Series cards.
#pragma once
#include "editor/i18n/locale.h"
#include <QList>
#include <QPixmap>
#include <QWidget>

class QFrame;
class QImage;
class QLabel;
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
    // Decoration Stamp Series: keep a preview box for a rendered stamp, filled by setCardPreview().
    bool spritePreview = false;
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
    // Fill a `spritePreview` card's box: the rendered stamp, or a "not configured" box without a game folder.
    void setCardPreview(int row, bool spritesReady, const QImage &image);

  signals:
    void currentRowChanged(int row);

  private:
    const Locale &locale_;
    QScrollArea *scroll_ = nullptr;
    QWidget *content_ = nullptr;
    QVBoxLayout *layout_ = nullptr;
    QList<QFrame *> cards_;
    // Preview box of each card, nullptr for cards without one.
    QList<QLabel *> previews_;
    int current_ = -1;
};

} // namespace editor
