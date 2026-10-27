// Right dock content: map size, AS1 ceiling layers, component preview, decoration and tool properties.
#pragma once
#include "editor/i18n/locale.h"
#include "editor/project/project.h"
#include "editor/ui/panels/as1_ceiling.h"
#include "editor/ui/tools/eraser/eraser_properties.h"
#include <QWidget>

class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLabel;
class QPushButton;

namespace editor {

class InspectorPanel : public QWidget {
    Q_OBJECT
  public:
    explicit InspectorPanel(const Locale &locale, QWidget *parent = nullptr);

    void setMapSize(double width, double height);
    AS1CeilingPropertiesWidget *ceiling() const;
    EraserPropertiesWidget *eraser() const;

    // Component preview: the Editor Set title and its Editor Set Items.
    void setComponentSet(const QString &title, const QStringList &itemLabels, int selectedIndex);
    int componentIndex() const;
    int componentCount() const;
    // "+" / "-" hotkeys: move the selected Editor Set Item by `step`, clamped.
    void selectRelativeComponent(int step);
    void showWallPart(const QString &imagePath, const QString &nvidText);
    // `spritesReady`: an AS2 game folder is loaded; `preview` is the composed stamp (may be null).
    void showStamp(int memberCount, bool spritesReady, const QImage &preview);
    // Stamp notice: whether an AS2 game folder is loaded, so sprites render instead of crosses.
    void setSpritePreviewReady(bool ready);

    // Decoration properties group; hidden when nothing is selected in edit mode.
    void setDecorationSelection(const Decoration *decoration);

  signals:
    void mapSizeApplied(double width, double height);
    void componentChosen(int index);
    void spacingChanged(double itemSpacing, double rowSpacing);
    void deleteDecorationRequested();

  private:
    void showPreview(const QString &imagePath);
    void setPreviewNotConfigured(bool notConfigured);

    const Locale &locale_;
    QDoubleSpinBox *width_ = nullptr;
    QDoubleSpinBox *height_ = nullptr;
    AS1CeilingPropertiesWidget *ceiling_ = nullptr;
    QLabel *theme_ = nullptr;
    QComboBox *components_ = nullptr;
    QLabel *stampDetails_ = nullptr;
    QLabel *preview_ = nullptr;
    QLabel *notice_ = nullptr;
    QLabel *nvid_ = nullptr;
    QGroupBox *selectionGroup_ = nullptr;
    QWidget *spacingWidget_ = nullptr;
    QDoubleSpinBox *itemSpacing_ = nullptr;
    QDoubleSpinBox *rowSpacing_ = nullptr;
    QPushButton *delete_ = nullptr;
    EraserPropertiesWidget *eraser_ = nullptr;
};

} // namespace editor
