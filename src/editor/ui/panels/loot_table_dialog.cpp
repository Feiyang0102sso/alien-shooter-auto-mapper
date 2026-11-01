#include "editor/ui/panels/loot_table_dialog.h"
#include "editor/i18n/text_keys.h"
#include "editor/ui/widget_helpers.h"
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTextDocument>
#include <QVBoxLayout>
#include <algorithm>
#include <stdexcept>

namespace editor {
namespace {
// The dialog opens wide so the text column holds a typical loot text on one line; longer texts wrap.
constexpr int DIALOG_WIDTH = 980;
constexpr int DIALOG_HEIGHT = 620;
constexpr int ROWS_MIN_HEIGHT = 220;
// Fixed side columns, shared by the header and every row so they line up.
constexpr int PERCENT_COLUMN_WIDTH = 110;
constexpr int REMOVE_COLUMN_WIDTH = 80;
// Height limit handed to the text measurement; far beyond any loot text.
constexpr int TEXT_MEASURE_HEIGHT = 100000;
// Room the QSS padding (2px top / bottom, 4px left / right) takes inside a loot text box, plus a little slack.
constexpr int TEXT_BOX_PADDING = 8;
constexpr double PERCENT_TOTAL = auto_mapper::core::LOOT_PERCENT_TOTAL;
// Slack for adding up one-decimal percents.
constexpr double PERCENT_EPSILON = 1e-6;
} // namespace

LootTableDialog::LootTableDialog(const Locale &locale, const QString &kindLabel,
                                 const std::vector<auto_mapper::core::LootLine> &lines, QWidget *parent)
    : QDialog(parent), locale_(locale) {
    setObjectName("lootTableDialog");
    setWindowTitle(locale.tr(TextKey::DIALOG_LOOT_TABLE, {{"loot_name", kindLabel}}));
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->addWidget(wrappingLabel(locale.tr(TextKey::LABEL_LOOT_TABLE_HINT)));

    // Presets from loot_presets.ini, shared by every card.
    auto *presetRow = new QHBoxLayout;
    presetRow->addWidget(new QLabel(locale.tr(TextKey::LABEL_LOOT_PRESET)));
    presetCombo_ = new QComboBox;
    presetCombo_->setObjectName("lootPresetCombo");
    presetRow->addWidget(presetCombo_, 1);
    auto *load = new QPushButton(locale.tr(TextKey::BUTTON_LOOT_PRESET_LOAD));
    auto *save = new QPushButton(locale.tr(TextKey::BUTTON_LOOT_PRESET_SAVE));
    auto *remove = new QPushButton(locale.tr(TextKey::BUTTON_LOOT_PRESET_DELETE));
    load->setObjectName("lootPresetLoad");
    save->setObjectName("lootPresetSave");
    remove->setObjectName("lootPresetDelete");
    presetRow->addWidget(load);
    presetRow->addWidget(save);
    presetRow->addWidget(remove);
    layout->addLayout(presetRow);
    connect(load, &QPushButton::clicked, this, &LootTableDialog::loadPreset);
    connect(save, &QPushButton::clicked, this, &LootTableDialog::savePreset);
    connect(remove, &QPushButton::clicked, this, &LootTableDialog::deletePreset);

    // Rows scroll vertically only: long texts wrap instead of widening the dialog.
    // The column titles sit in the same layout as the rows, so the columns line up.
    rowsWidget_ = new QWidget;
    rowsWidget_->setObjectName("lootRowsWidget");
    rowsLayout_ = new QVBoxLayout(rowsWidget_);
    rowsLayout_->setContentsMargins(0, 0, 0, 0);
    rowsLayout_->setSpacing(6);
    auto *header = new QHBoxLayout;
    header->setSpacing(6);
    header->addWidget(new QLabel(locale.tr(TextKey::LABEL_LOOT_TEXT)), 1);
    auto *percentTitle = new QLabel(locale.tr(TextKey::LABEL_LOOT_PERCENT));
    percentTitle->setFixedWidth(PERCENT_COLUMN_WIDTH);
    header->addWidget(percentTitle);
    header->addSpacing(REMOVE_COLUMN_WIDTH + header->spacing());
    rowsLayout_->addLayout(header);
    rowsLayout_->addStretch();
    scroll_ = new QScrollArea;
    scroll_->setObjectName("lootRowsScroll");
    scroll_->viewport()->setObjectName("lootRowsViewport");
    scroll_->setWidgetResizable(true);
    scroll_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_->setMinimumHeight(ROWS_MIN_HEIGHT);
    scroll_->setWidget(rowsWidget_);
    layout->addWidget(scroll_, 1);

    auto *add = new QPushButton(locale.tr(TextKey::BUTTON_LOOT_ADD_LINE));
    add->setObjectName("lootAddLine");
    connect(add, &QPushButton::clicked, this, [this] {
        addRow({});
        updateTotal();
    });
    layout->addWidget(add, 0, Qt::AlignLeft);

    total_ = new QLabel;
    total_->setObjectName("lootTotalLabel");
    layout->addWidget(total_);

    auto *buttons = new QHBoxLayout;
    buttons->addStretch();
    ok_ = new QPushButton(locale.tr(TextKey::BUTTON_OK));
    ok_->setObjectName("lootTableOk");
    ok_->setDefault(true);
    auto *cancel = new QPushButton(locale.tr(TextKey::BUTTON_CANCEL));
    buttons->addWidget(ok_);
    buttons->addWidget(cancel);
    layout->addLayout(buttons);
    connect(ok_, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);

    refreshPresets({});
    setLines(lines);
    resize(DIALOG_WIDTH, DIALOG_HEIGHT);
}

std::vector<auto_mapper::core::LootLine> LootTableDialog::lines() const {
    std::vector<auto_mapper::core::LootLine> result;
    for (const Row &row : rows_) {
        QString text = row.text->toPlainText().trimmed();
        if (text.isEmpty())
            continue;
        result.push_back({text.toStdString(), row.percent->value()});
    }
    return result;
}

double LootTableDialog::totalPercent() const {
    double total = 0;
    for (const Row &row : rows_)
        total += row.percent->value();
    return total;
}

void LootTableDialog::addRow(const auto_mapper::core::LootLine &line) {
    Row row;
    row.widget = new QWidget;
    // Rows keep their own height; the stretch below the last row takes the spare space.
    row.widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    auto *layout = new QHBoxLayout(row.widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    row.text = new QPlainTextEdit(QString::fromStdString(line.text));
    row.text->setObjectName("lootText");
    row.text->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    row.text->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    row.text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    row.text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // Tab moves on to the percent box instead of typing a tab into the loot text.
    row.text->setTabChangesFocus(true);
    row.percent = decimalInput("lootPercent", 0, PERCENT_TOTAL, 1);
    row.percent->setValue(line.percent);
    row.percent->setFixedWidth(PERCENT_COLUMN_WIDTH);
    auto *remove = new QPushButton(locale_.tr(TextKey::BUTTON_LOOT_REMOVE_LINE));
    remove->setObjectName("lootRemoveLine");
    remove->setFixedWidth(REMOVE_COLUMN_WIDTH);
    layout->addWidget(row.text, 1, Qt::AlignTop);
    layout->addWidget(row.percent, 0, Qt::AlignTop);
    layout->addWidget(remove, 0, Qt::AlignTop);
    // Keep the stretch below the last row.
    rowsLayout_->insertWidget(rowsLayout_->count() - 1, row.widget);
    rows_.append(row);

    QWidget *rowWidget = row.widget;
    connect(remove, &QPushButton::clicked, this, [this, rowWidget] {
        removeRow(rowWidget);
    });
    QPlainTextEdit *text = row.text;
    connect(text, &QPlainTextEdit::textChanged, this, [this, text] {
        // A loot text is one line in the map file: a pasted line break becomes a space.
        QString plain = text->toPlainText();
        if (plain.contains('\n')) {
            text->setPlainText(plain.replace('\n', ' '));
            text->moveCursor(QTextCursor::End);
        }
        fitTextHeight(text);
    });
    // Wrapping changes with the box width, so a resize refits the height too.
    text->installEventFilter(this);
    connect(row.percent, &QDoubleSpinBox::valueChanged, this, &LootTableDialog::updateTotal);
    fitTextHeight(text);
}

void LootTableDialog::removeRow(QWidget *rowWidget) {
    for (int i = 0; i < rows_.size(); ++i) {
        if (rows_[i].widget == rowWidget) {
            rows_.removeAt(i);
            break;
        }
    }
    rowWidget->deleteLater();
    updateTotal();
}

void LootTableDialog::setLines(const std::vector<auto_mapper::core::LootLine> &lines) {
    for (const Row &row : rows_)
        row.widget->deleteLater();
    rows_.clear();
    for (const auto &line : lines)
        addRow(line);
    // An empty table still offers one row to type into.
    if (rows_.isEmpty())
        addRow({});
    updateTotal();
}

void LootTableDialog::updateTotal() {
    double total = totalPercent();
    bool overfull = total > PERCENT_TOTAL + PERCENT_EPSILON;
    if (overfull) {
        total_->setText(locale_.tr(TextKey::LABEL_LOOT_OVERFULL, {{"percent", QString::number(total, 'f', 1)}}));
    } else {
        double remainder = std::max(0.0, PERCENT_TOTAL - total);
        total_->setText(locale_.tr(TextKey::LABEL_LOOT_REMAINDER, {{"percent", QString::number(remainder, 'f', 1)}}));
    }
    // QSS turns the label red by this property.
    total_->setProperty("overfull", overfull);
    total_->style()->unpolish(total_);
    total_->style()->polish(total_);
    ok_->setEnabled(!overfull);
}

bool LootTableDialog::eventFilter(QObject *watched, QEvent *event) {
    auto *text = qobject_cast<QPlainTextEdit *>(watched);
    if (text != nullptr && event->type() == QEvent::Resize)
        fitTextHeight(text);
    return QDialog::eventFilter(watched, event);
}

void LootTableDialog::fitTextHeight(QPlainTextEdit *text) {
    // Count the word-wrapped lines at the box's current width. QPlainTextEdit lays its document out lazily,
    // so its own line count is not ready before the box is painted.
    int margin = static_cast<int>(text->document()->documentMargin());
    int frame = text->frameWidth();
    int wrapWidth = std::max(1, text->width() - 2 * frame - 2 * margin - TEXT_BOX_PADDING);
    QString plain = text->toPlainText();
    if (plain.isEmpty())
        plain = " ";
    int lineSpacing = std::max(1, text->fontMetrics().lineSpacing());
    QRect bounds = text->fontMetrics().boundingRect(QRect(0, 0, wrapWidth, TEXT_MEASURE_HEIGHT), Qt::TextWordWrap, plain);
    int lineCount = std::max(1, (bounds.height() + lineSpacing - 1) / lineSpacing);
    int height = lineCount * lineSpacing + 2 * margin + 2 * frame + TEXT_BOX_PADDING;
    if (text->height() != height)
        text->setFixedHeight(height);
}

void LootTableDialog::refreshPresets(const QString &select) {
    try {
        presets_ = loadLootPresets();
    } catch (const std::exception &error) {
        presets_.clear();
        QMessageBox::warning(this, locale_.tr(TextKey::DIALOG_ERROR), QString::fromUtf8(error.what()));
    }
    presetCombo_->clear();
    for (const LootPreset &preset : presets_)
        presetCombo_->addItem(preset.name);
    int index = presetCombo_->findText(select);
    if (index >= 0)
        presetCombo_->setCurrentIndex(index);
}

void LootTableDialog::loadPreset() {
    int index = presetCombo_->currentIndex();
    if (index < 0 || index >= presets_.size())
        return;
    setLines(presets_[index].lines);
    qInfo() << "Loot preset loaded:" << presets_[index].name;
}

void LootTableDialog::savePreset() {
    bool accepted = false;
    QString name = QInputDialog::getText(this, locale_.tr(TextKey::DIALOG_LOOT_PRESET_NAME),
                                         locale_.tr(TextKey::DIALOG_LOOT_PRESET_NAME), QLineEdit::Normal,
                                         presetCombo_->currentText(), &accepted)
                       .trimmed();
    if (!accepted)
        return;
    // The name becomes an ini section name.
    if (!isValidLootPresetName(name)) {
        QMessageBox::warning(this, locale_.tr(TextKey::DIALOG_ERROR), locale_.tr(TextKey::MESSAGE_LOOT_PRESET_NAME_INVALID));
        return;
    }
    LootPreset preset;
    preset.name = name;
    preset.lines = lines();
    try {
        saveLootPreset(preset);
    } catch (const std::exception &error) {
        QMessageBox::warning(this, locale_.tr(TextKey::DIALOG_ERROR), QString::fromUtf8(error.what()));
        return;
    }
    refreshPresets(name);
}

void LootTableDialog::deletePreset() {
    QString name = presetCombo_->currentText();
    if (name.isEmpty())
        return;
    try {
        deleteLootPreset(name);
    } catch (const std::exception &error) {
        QMessageBox::warning(this, locale_.tr(TextKey::DIALOG_ERROR), QString::fromUtf8(error.what()));
        return;
    }
    refreshPresets({});
}

} // namespace editor
