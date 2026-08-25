/**
 * @file decoration_builder.h
 * @brief Expand authored decoration profiles at a target map position.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/io/map_writer.h"

#include <vector>

namespace auto_mapper::core::decoration_builder {

class DecorationBuilder {
public:
    /**
     * Build one complete stamp. Placement applies translation only.
     */
    std::vector<io::Sprite> build(
        const DecorationProfile& profile,
        const DecorationPlacement& placement,
        DecorationBuildOptions options = {}
    ) const;
};

} // namespace auto_mapper::core::decoration_builder
