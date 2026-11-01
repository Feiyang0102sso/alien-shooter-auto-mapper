// Left dock content: game family buttons, AS2 format selector, shelf mode tabs and the card shelf.
#pragma once
#include "editor/catalog/catalog.h"
#include "editor/ui/panels/theme_shelf.h"
#include <QWidget>

class QComboBox;
class QPushButton;

namespace editor {

// Which shelf the left dock shows. Loot exists only for AS2 Series projects.
enum class ShelfMode { Walls, Decorations, Loot };

class LeftShelfPanel : public QWidget {
    Q_OBJECT
  public:
    explicit LeftShelfPanel(const Catalog &catalog, QWidget *parent = nullptr);

    // Reflect the Project Version without emitting request signals.
    void setProjectVersion(const QString &version);
    void setShelfMode(ShelfMode mode);
    // Fill the shelf with Wall Set cards, Decoration Stamp Series cards of the AS1 or AS2 family,
    // or the three Loot Box cards (wood, metal, egg).
    void showWallSets(const QList<int> &setIds);
    void showStampSeries(bool as1, const QStringList &seriesIds);
    void showLootKinds();
    ThemeShelfPanel *shelf() const;
    // Q / E hotkeys: move the selected card by `step`, clamped to the shelf.
    void selectRelativeSet(int step);

  signals:
    void familyRequested(bool as1);
    void as2FormatRequested(const QString &version);
    void shelfModeRequested(ShelfMode mode);
    void setChosen(int row);

  private:
    const Catalog &catalog_;
    QPushButton *as1Button_ = nullptr;
    QPushButton *as2Button_ = nullptr;
    QComboBox *as2Format_ = nullptr;
    QPushButton *wallsButton_ = nullptr;
    QPushButton *decorationsButton_ = nullptr;
    QPushButton *lootButton_ = nullptr;
    ThemeShelfPanel *shelf_ = nullptr;
};

} // namespace editor
