#include "editor/loot_presets.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <SimpleIni.h>
#include <map>
#include <stdexcept>
#include <string>

namespace editor {
namespace {
constexpr const char *LOOT_PRESETS_FILE_NAME = "loot_presets.ini";
// itemN = whole loot text, itemN_p = its percent.
constexpr const char *ITEM_KEY_PREFIX = "item";
constexpr const char *PERCENT_KEY_SUFFIX = "_p";

// One itemN / itemN_p pair while reading a section; either half may be missing.
struct PresetRow {
    QString text;
    bool hasText = false;
    double percent = 0;
    bool hasPercent = false;
};

void fail(const QString &message) {
    throw std::runtime_error(message.toStdString());
}

// loot_presets.ini parsed (UTF-8); a missing file leaves it empty. Read through QFile so non-ASCII paths work;
// SimpleIni's own LoadFile() uses fopen, which cannot open them on Windows.
void loadIni(CSimpleIniA &ini) {
    ini.SetUnicode(true);
    QString path = lootPresetsPath();
    QFile file(path);
    if (!file.exists())
        return;
    if (!file.open(QIODevice::ReadOnly))
        fail(file.errorString() + ": " + path);
    QByteArray bytes = file.readAll();
    SI_Error result = ini.LoadData(bytes.constData(), static_cast<size_t>(bytes.size()));
    if (result < 0)
        fail("Cannot parse " + path);
}

void saveIni(const CSimpleIniA &ini) {
    std::string text;
    if (ini.Save(text) < 0)
        fail("Cannot serialize " + lootPresetsPath());
    QSaveFile file(lootPresetsPath());
    if (!file.open(QIODevice::WriteOnly))
        fail(file.errorString());
    QByteArray bytes(text.data(), static_cast<qsizetype>(text.size()));
    if (file.write(bytes) != bytes.size() || !file.commit())
        fail(file.errorString());
}

// "item12" -> 12, "item12_p" -> 12 with isPercent; 0 for keys of any other shape.
int itemNumber(const QString &key, bool &isPercent) {
    QString body = key;
    isPercent = body.endsWith(PERCENT_KEY_SUFFIX);
    if (isPercent)
        body.chop(QString(PERCENT_KEY_SUFFIX).size());
    if (!body.startsWith(ITEM_KEY_PREFIX))
        return 0;
    bool ok = false;
    int number = body.mid(QString(ITEM_KEY_PREFIX).size()).toInt(&ok);
    if (!ok || number <= 0)
        return 0;
    return number;
}

// "24.3%" or "24.3" -> 24.3; false when it is not a number.
bool parsePercent(QString value, double &percent) {
    value = value.trimmed();
    if (value.endsWith('%'))
        value.chop(1);
    bool ok = false;
    percent = value.trimmed().toDouble(&ok);
    return ok;
}

LootPreset readPreset(const CSimpleIniA &ini, const char *section) {
    LootPreset preset;
    preset.name = QString::fromUtf8(section);
    CSimpleIniA::TNamesDepend keys;
    ini.GetAllKeys(section, keys);
    // Rows by N, whatever the order in the file.
    std::map<int, PresetRow> rows;
    for (const CSimpleIniA::Entry &key : keys) {
        QString keyName = QString::fromUtf8(key.pItem);
        bool isPercent = false;
        int number = itemNumber(keyName, isPercent);
        if (number == 0) {
            qWarning() << "Loot preset" << preset.name << "ignores key" << keyName;
            continue;
        }
        QString value = QString::fromUtf8(ini.GetValue(section, key.pItem, ""));
        PresetRow &row = rows[number];
        if (!isPercent) {
            row.text = value.trimmed();
            row.hasText = true;
            continue;
        }
        row.hasPercent = parsePercent(value, row.percent);
        if (!row.hasPercent)
            qWarning() << "Loot preset" << preset.name << "has a bad percent:" << keyName << value;
    }
    for (const auto &[number, row] : rows) {
        if (!row.hasText || !row.hasPercent) {
            qWarning() << "Loot preset" << preset.name << "skips item" << number << "without text or percent";
            continue;
        }
        preset.lines.push_back({row.text.toStdString(), row.percent});
    }
    return preset;
}

// Every section's preset, in file order.
QList<LootPreset> readPresets(const CSimpleIniA &ini) {
    CSimpleIniA::TNamesDepend sections;
    ini.GetAllSections(sections);
    // SimpleIni hands sections back in its own order; the file order is what users expect.
    sections.sort(CSimpleIniA::Entry::LoadOrder());
    QList<LootPreset> presets;
    for (const CSimpleIniA::Entry &section : sections)
        presets.append(readPreset(ini, section.pItem));
    return presets;
}
} // namespace

QString lootPresetsPath() {
    QString local = QDir::current().filePath(LOOT_PRESETS_FILE_NAME);
    if (QFileInfo::exists(local))
        return local;
    return QDir(QCoreApplication::applicationDirPath()).filePath(LOOT_PRESETS_FILE_NAME);
}

QList<LootPreset> loadLootPresets() {
    CSimpleIniA ini;
    loadIni(ini);
    QList<LootPreset> presets = readPresets(ini);
    qInfo() << "Loaded" << presets.size() << "loot presets, file" << lootPresetsPath();
    return presets;
}

bool isValidLootPresetName(const QString &name) {
    if (name.trimmed().isEmpty())
        return false;
    return !name.contains('[') && !name.contains(']');
}

void saveLootPreset(const LootPreset &preset) {
    if (!isValidLootPresetName(preset.name))
        fail("Invalid loot preset name: " + preset.name);
    CSimpleIniA ini;
    loadIni(ini);
    std::string section = preset.name.trimmed().toStdString();
    // Empty the section key by key: the section keeps its place and its comment.
    CSimpleIniA::TNamesDepend keys;
    ini.GetAllKeys(section.c_str(), keys);
    for (const CSimpleIniA::Entry &key : keys) {
        std::string keyName = key.pItem;
        ini.Delete(section.c_str(), keyName.c_str());
    }
    ini.SetValue(section.c_str(), nullptr, nullptr);
    int number = 1;
    for (const auto &line : preset.lines) {
        std::string itemKey = ITEM_KEY_PREFIX + std::to_string(number);
        std::string percentKey = itemKey + PERCENT_KEY_SUFFIX;
        std::string percent = QString::number(line.percent).toStdString() + "%";
        ini.SetValue(section.c_str(), itemKey.c_str(), line.text.c_str());
        ini.SetValue(section.c_str(), percentKey.c_str(), percent.c_str());
        number += 1;
    }
    saveIni(ini);
    qInfo() << "Saved loot preset" << preset.name << "rows" << preset.lines.size() << "to" << lootPresetsPath();
}

void deleteLootPreset(const QString &name) {
    CSimpleIniA ini;
    loadIni(ini);
    std::string section = name.toStdString();
    ini.Delete(section.c_str(), nullptr);
    saveIni(ini);
    qInfo() << "Deleted loot preset" << name << "from" << lootPresetsPath();
}

} // namespace editor
