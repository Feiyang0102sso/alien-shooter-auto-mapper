/**
 * @file loot_table.cpp
 * @brief Roll whole loot texts onto loot containers.
 */

#include "auto_mapper/core/scatter/loot/loot_table.h"

#include "auto_mapper/common/logger.h"
#include "auto_mapper/core/randomizer.h"

#include <algorithm>

namespace auto_mapper::core {
namespace {

// Floating point slack when checking that the percents add up to at most 100.
constexpr double PERCENT_EPSILON = 1e-6;

const LootTable* find_table(const std::vector<LootTable>& tables, int32_t vid) {
    for (const LootTable& table : tables) {
        if (std::find(table.vids.begin(), table.vids.end(), vid) != table.vids.end()) {
            return &table;
        }
    }
    return nullptr;
}

/**
 * One roll over the table: the chosen line, or nullptr for the "write nothing" remainder.
 */
const LootLine* roll_line(const LootTable& table) {
    double roll = Random::get_real(0.0, LOOT_PERCENT_TOTAL);
    double cumulative = 0.0;
    for (const LootLine& line : table.lines) {
        cumulative += line.percent;
        if (roll < cumulative) {
            return &line;
        }
    }
    return nullptr;
}

} // namespace

bool is_valid_loot_table(const LootTable& table) {
    double total = 0.0;
    for (const LootLine& line : table.lines) {
        if (line.percent < 0.0 || line.percent > LOOT_PERCENT_TOTAL) {
            return false;
        }
        total += line.percent;
    }
    return total <= LOOT_PERCENT_TOTAL + PERCENT_EPSILON;
}

void assign_loot_text(std::vector<io::Sprite>& sprites, const std::vector<LootTable>& tables) {
    int written = 0;
    int skipped = 0;
    for (io::Sprite& sprite : sprites) {
        const LootTable* table = find_table(tables, sprite.vid);
        if (table == nullptr) {
            continue;
        }
        const LootLine* line = roll_line(*table);
        if (line == nullptr) {
            skipped += 1;
            continue;
        }
        sprite.name_text = line->text;
        written += 1;
    }
    Logger::info("Loot text: {} container(s) filled, {} left empty", written, skipped);
}

} // namespace auto_mapper::core
