/**
 * @file decoration_registry.cpp
 * @brief The single translation unit that expands the AS2 decoration profile table.
 */

#include "auto_mapper/core/decoration_builder/decoration_registry.h"
#include "auto_mapper/core/decoration_builder/decoration_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

std::span<const DecorationProfile* const> as2_decoration_profiles() {
    return std::span<const DecorationProfile* const>(AS2_DECORATION_PROFILES, AS2_DECORATION_PROFILE_COUNT);
}

const DecorationProfile* find_as2_decoration_profile(std::string_view profile_id) {
    for (const DecorationProfile* profile : as2_decoration_profiles()) {
        if (profile->id == profile_id) {
            return profile;
        }
    }

    return nullptr;
}

} // namespace auto_mapper::core::decoration_builder
