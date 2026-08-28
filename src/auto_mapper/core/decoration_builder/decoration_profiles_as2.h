/**
 * @file decoration_profiles_as2.h
 * @brief Registry of every authored AS2 decoration stamp profile.
 *
 * The profiles themselves live one series per header under
 * decoration_profile_as2/. Adding a series means including its header here
 * and listing its profiles in the table below.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_as2/dorms_as2.h"
#include "auto_mapper/core/decoration_builder/decoration_profile_as2/office_as2.h"
#include "auto_mapper/core/decoration_builder/decoration_profile_as2/storage_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Registry of every authored AS2 decoration profile.
 * The C API enumerates this table, so new profiles only need to be listed here.
 */
static const DecorationProfile* const AS2_DECORATION_PROFILES[] = {
    &DORMS01_DECORATION_PROFILE,
    &DORMS02_DECORATION_PROFILE,
    &DORMS03_DECORATION_PROFILE,
    &DORMS04_DECORATION_PROFILE,
    &DORMS05_DECORATION_PROFILE,
    &DORMS06_DECORATION_PROFILE,
    &DORMS07_DECORATION_PROFILE,
    &DORMS08_DECORATION_PROFILE,
    &OFFICE01_DECORATION_PROFILE,
    &OFFICE02_DECORATION_PROFILE,
    &OFFICE03_DECORATION_PROFILE,
    &OFFICE04_DECORATION_PROFILE,
    &OFFICE05_DECORATION_PROFILE,
    &OFFICE06_DECORATION_PROFILE,
    &OFFICE07_DECORATION_PROFILE,
    &OFFICE08_DECORATION_PROFILE,
    &OFFICE09_DECORATION_PROFILE,
    &STORAGE01_DECORATION_PROFILE,
    &STORAGE02_DECORATION_PROFILE,
    &STORAGE03_DECORATION_PROFILE,
    &STORAGE04_DECORATION_PROFILE,
    &STORAGE05_DECORATION_PROFILE,
    &STORAGE06_DECORATION_PROFILE,
    &STORAGE07_DECORATION_PROFILE,
    &STORAGE08_DECORATION_PROFILE,
    &STORAGE09_DECORATION_PROFILE,
};

inline constexpr int AS2_DECORATION_PROFILE_COUNT =
    static_cast<int>(sizeof(AS2_DECORATION_PROFILES) / sizeof(AS2_DECORATION_PROFILES[0]));

} // namespace auto_mapper::core::decoration_builder
