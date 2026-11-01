/**
 * @file loot_table.h
 * @brief Loot Tables: which whole loot text a loot container gets in its SPRD nameText.
 *
 * A loot text is a space separated item list ("EMPTY Ammo_shotgun Obj_money3"); the game picks one
 * entry when the container breaks, so repeated names weigh more and EMPTY means "nothing".
 * The tool never parses it: each container rolls one whole line of its table and writes it verbatim.
 */

#pragma once

#include "auto_mapper/io/map_writer.h"

#include <cstdint>
#include <string>
#include <vector>

namespace auto_mapper::core {

inline constexpr double LOOT_PERCENT_TOTAL = 100.0;

/**
 * One table row: a whole loot text and the chance a container receives it.
 */
struct LootLine {
    std::string text;
    double percent = 0.0;
};

/**
 * Lines shared by every sprite whose VID is listed, stamp members included.
 * Percents add up to at most 100; the rest leaves the nameText empty.
 */
struct LootTable {
    std::vector<int32_t> vids;
    std::vector<LootLine> lines;
};

/**
 * True when every percent is within 0..100 and they add up to at most 100.
 */
bool is_valid_loot_table(const LootTable& table);

/**
 * Roll one line per sprite whose VID a table lists and write it into name_text.
 * Sprites that roll the remainder, or that no table lists, keep their name_text.
 */
void assign_loot_text(std::vector<io::Sprite>& sprites, const std::vector<LootTable>& tables);

} // namespace auto_mapper::core
