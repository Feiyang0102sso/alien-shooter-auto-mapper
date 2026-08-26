/**
 * @file decoration_builder.cpp
 * @brief Implements authored decoration stamp expansion.
 */

#include "auto_mapper/core/decoration_builder/decoration_builder.h"
#include "auto_mapper/core/vid_properties/vid_armies_as2.h"

#include <cmath>

namespace auto_mapper::core::decoration_builder {

namespace {

void append_members(
    std::vector<io::Sprite>& sprites,
    const std::vector<DecorationMemberProfile>& members,
    const DecorationPlacement& placement,
    const DecorationPoint& shift
) {
    for (const DecorationMemberProfile& member : members) {
        io::Sprite sprite;
        sprite.vid = member.vid;
        sprite.posX = placement.center_x + member.offset_x + shift.x;
        sprite.posY = placement.center_y + member.offset_y + shift.y;
        sprite.posZ = member.pos_z;
        sprite.direction = member.direction;
        sprite.army = get_as2_vid_army(member.vid);
        sprite.gamma = member.gamma;
        sprite.scale = member.scale;
        sprites.push_back(sprite);
    }
}

void append_boundary_edge(
    std::vector<io::Sprite>& sprites,
    const DecorationPoint& start,
    const DecorationPoint& end,
    const WallPartAsset& wall_asset,
    const WallProfile& wall_profile,
    const DecorationPlacement& placement
) {
    float delta_x = end.x - start.x;
    float delta_y = end.y - start.y;
    float edge_length = std::hypot(delta_x, delta_y);
    float wall_length = std::hypot(wall_profile.step_x, wall_profile.step_y);
    if (edge_length <= 0.0f || wall_length <= 0.0f) {
        return;
    }

    // A fractional final slot is shifted inward so no wall crosses the corner.
    float slot_count = edge_length / wall_length;
    int wall_count = static_cast<int>(std::ceil(slot_count - 0.001f));

    for (int wall_index = 0; wall_index < wall_count; ++wall_index) {
        float start_distance = static_cast<float>(wall_index) * wall_length;
        if (wall_index == wall_count - 1) {
            start_distance = edge_length - wall_length;
        }

        float edge_ratio = start_distance / edge_length;
        float canonical_x = start.x + delta_x * edge_ratio;
        float canonical_y = start.y + delta_y * edge_ratio;

        io::Sprite sprite;
        sprite.vid = wall_asset.vid;
        sprite.posX = placement.center_x + canonical_x + wall_asset.offset_x;
        sprite.posY = placement.center_y + canonical_y + wall_asset.offset_y;
        sprite.direction = wall_asset.direction;
        sprite.army = get_as2_vid_army(wall_asset.vid);
        sprite.gamma = wall_asset.gamma;
        sprite.scale = wall_asset.scale;
        sprites.push_back(sprite);
    }
}

void append_calculated_boundary(
    std::vector<io::Sprite>& sprites,
    const DecorationProfile& profile,
    const DecorationPlacement& placement
) {
    const WallProfile* wall_profile = profile.test_boundary_wall_profile;
    if (wall_profile == nullptr || wall_profile->variant_count == 0) {
        return;
    }

    const WallVariant& wall_variant = wall_profile->variants[0];
    const std::array<DecorationPoint, 4>& corners = profile.frame.corner_offsets;

    append_boundary_edge(
        sprites,
        corners[1],
        corners[0],
        wall_variant.dir_a,
        *wall_profile,
        placement
    );
    append_boundary_edge(
        sprites,
        corners[2],
        corners[3],
        wall_variant.dir_a,
        *wall_profile,
        placement
    );
    append_boundary_edge(
        sprites,
        corners[0],
        corners[3],
        wall_variant.dir_b,
        *wall_profile,
        placement
    );
    append_boundary_edge(
        sprites,
        corners[1],
        corners[2],
        wall_variant.dir_b,
        *wall_profile,
        placement
    );
}

} // namespace

DecorationPoint resolve_member_shift(const DecorationMemberNudge& nudge) {
    // dir_a runs (90, -64) and dir_b runs (90, 64), both measured in wall steps.
    DecorationPoint shift;
    shift.x = (nudge.along_dir_a + nudge.along_dir_b) * DECORATION_WALL_STEP_X;
    shift.y = (-nudge.along_dir_a + nudge.along_dir_b) * DECORATION_WALL_STEP_Y;
    return shift;
}

std::vector<io::Sprite> DecorationBuilder::build(
    const DecorationProfile& profile,
    const DecorationPlacement& placement,
    DecorationBuildOptions options
) const {
    std::vector<io::Sprite> sprites;
    sprites.reserve(profile.members.size());

    DecorationPoint shift = resolve_member_shift(profile.member_nudge);
    append_members(sprites, profile.members, placement, shift);
    if (options.keep_boundary) {
        append_calculated_boundary(sprites, profile, placement);
    }

    return sprites;
}

} // namespace auto_mapper::core::decoration_builder
