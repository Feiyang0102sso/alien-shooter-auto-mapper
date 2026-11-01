// Loot Table presets in loot_presets.ini beside the exe, read and written with SimpleIni.
//
// Presets are shared by every Loot Box card and told apart by name only. One section per preset;
// itemN holds a whole loot text and itemN_p its percent ("%" optional when read, always written):
//
//   [drop_egg]
//   item1 = Ammo_shotgun Ammo_minigun Ammo_rocket Bonus_health10 EMPTY EMPTY
//   item1_p = 100%
//
// Rows are ordered by N; numbers may skip. A row without a valid percent is skipped with a warning.
//
// The file ships beside AutoMapper.exe (template: loot_presets.ini in the repository root, early / mid / late box drops from
// the official maps plus the egg line). Without it there are no presets; the first save creates it.
#pragma once
#include "auto_mapper/core/loot/loot_table.h"
#include <QList>
#include <QString>
#include <vector>

namespace editor {

// One named Loot Table preset.
struct LootPreset {
    QString name;
    std::vector<auto_mapper::core::LootLine> lines;
};

// Path of loot_presets.ini: a working-directory copy wins during development (like AutoMapper.cfg),
// else the file beside the exe. It need not exist yet.
QString lootPresetsPath();
// Every preset in file order; a missing file gives none.
QList<LootPreset> loadLootPresets();
// True when `name` can be a section name: not blank, no '[' or ']'.
bool isValidLootPresetName(const QString &name);
// Replace (or append) one preset, keeping every other section and comment.
// Throws std::runtime_error for an invalid name or when the file cannot be read or written.
void saveLootPreset(const LootPreset &preset);
void deleteLootPreset(const QString &name);

} // namespace editor
