/**
 * @file decoration_registry.h
 * @brief Lookup of authored AS2 decoration stamp profiles.
 *
 * The profile headers define their data with internal linkage, so only
 * decoration_registry.cpp includes decoration_profiles_as2.h. Everyone else
 * (map generator, editor) reads the profiles through these functions.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include <span>
#include <string_view>

namespace auto_mapper::core::decoration_builder {

/**
 * Every authored AS2 decoration profile, in registry order.
 */
std::span<const DecorationProfile* const> as2_decoration_profiles();

/**
 * Look up an authored profile by its id. Returns nullptr when unknown.
 */
const DecorationProfile* find_as2_decoration_profile(std::string_view profile_id);

} // namespace auto_mapper::core::decoration_builder
