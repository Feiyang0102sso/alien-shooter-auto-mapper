// Inspector controls for AS1 ceiling-layer counts (Standard / Dark Standard and Lab).
#pragma once
#include "editor/i18n/locale.h"
#include <QGroupBox>

class QSlider;
class QSpinBox;

namespace editor {

// One synchronized slider + spin box pair.
class CeilingLayerControl : public QWidget {
    Q_OBJECT
  public:
    CeilingLayerControl(const QString &labelText, const QString &spinName, int minimum, int maximum,
                        QWidget *parent = nullptr);
    int value() const;
    // Update both controls without emitting valueChanged.
    void setValue(int value);

  signals:
    void valueChanged(int value);

  private:
    QSlider *slider_ = nullptr;
    QSpinBox *spin_ = nullptr;
};

class AS1CeilingPropertiesWidget : public QGroupBox {
    Q_OBJECT
  public:
    explicit AS1CeilingPropertiesWidget(const Locale &locale, QWidget *parent = nullptr);

    // Layer counts from project data; 0 means "use the core default".
    void setLayerCounts(int standardLayerCount, int labLayerCount);
    // AS1-only visibility; editable only while ceiling generation is on.
    void setProjectState(bool isAS1, bool generateCeiling);

  signals:
    void layersChanged(int standardLayerCount, int labLayerCount);

  private:
    CeilingLayerControl *standard_ = nullptr;
    CeilingLayerControl *lab_ = nullptr;
};

} // namespace editor
