// Inspector group for the eraser size (slider and spin box kept in sync).
#pragma once
#include "editor/i18n/locale.h"
#include <QGroupBox>

class QSlider;
class QSpinBox;

namespace editor {

class EraserPropertiesWidget : public QGroupBox {
    Q_OBJECT
  public:
    explicit EraserPropertiesWidget(const Locale &locale, QWidget *parent = nullptr);
    int size() const;

  signals:
    void sizeChanged(int size);

  private:
    QSlider *slider_ = nullptr;
    QSpinBox *spin_ = nullptr;
};

} // namespace editor
