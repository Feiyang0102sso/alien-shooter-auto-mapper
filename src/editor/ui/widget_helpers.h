// Small widget constructors shared by panels and dialogs.
#pragma once
#include <QtWidgets>
namespace editor {
inline QDoubleSpinBox *decimalInput(const QString &name, double minimum, double maximum, int decimals = 1) {
    auto *input = new QDoubleSpinBox;
    input->setObjectName(name);
    input->setRange(minimum, maximum);
    input->setDecimals(decimals);
    input->setKeyboardTracking(false);
    return input;
}
inline QLabel *wrappingLabel(const QString &text) {
    auto *label = new QLabel(text);
    label->setWordWrap(true);
    label->setTextFormat(Qt::PlainText);
    return label;
}
inline QScrollArea *scrolled(QWidget *widget, const QString &name) {
    auto *area = new QScrollArea;
    area->setObjectName(name);
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setWidget(widget);
    return area;
}
} // namespace editor
