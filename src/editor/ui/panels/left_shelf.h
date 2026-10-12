// Left dock content: game family buttons, AS2 format selector, shelf mode tabs and the card shelf.
#pragma once
#include "editor/catalog/catalog.h"
#include "editor/ui/panels/theme_shelf.h"
#include <QWidget>

class QComboBox;
class QPushButton;

namespace editor {

class LeftShelfPanel : public QWidget {
    Q_OBJECT
  public:
    explicit LeftShelfPanel(const Catalog &catalog, QWidget *parent = nullptr);

    // Reflect the Project Version without emitting request signals.
    void setProjectVersion(const QString &version);
    void setDecorationShelf(bool decorations);
    // Fill the shelf with Wall Set cards, or with Decoration Stamp Series cards.
    void showWallSets(const QList<int> &setIds);
    void showStampSeries(const QStringList &seriesIds);
    ThemeShelfPanel *shelf() const;
    // Q / E hotkeys: move the selected card by `step`, clamped to the shelf.
    void selectRelativeSet(int step);

  signals:
    void familyRequested(bool as1);
    void as2FormatRequested(const QString &version);
    void shelfModeRequested(bool decorations);
    void setChosen(int row);

  private:
    const Catalog &catalog_;
    QPushButton *as1Button_ = nullptr;
    QPushButton *as2Button_ = nullptr;
    QComboBox *as2Format_ = nullptr;
    QPushButton *wallsButton_ = nullptr;
    QPushButton *decorationsButton_ = nullptr;
    ThemeShelfPanel *shelf_ = nullptr;
};

} // namespace editor
