/**
 * @file decoration_builder.cpp
 * @brief Translates Decoration Stamp members to their placed Map Coordinates.
 */

#include "auto_mapper/core/decoration_builder/decoration_builder.h"

namespace auto_mapper::core::decoration_builder {

std::vector<io::Sprite> build_stamp(const DecorationStamp& stamp, float center_x, float center_y) {
    std::vector<io::Sprite> sprites;
    sprites.reserve(stamp.members.size());

    for (const DecorationMember& member : stamp.members) {
        io::Sprite sprite;
        sprite.vid = member.vid;
        sprite.posX = center_x + member.offset_x;
        sprite.posY = center_y + member.offset_y;
        sprite.posZ = member.pos_z;
        sprite.direction = member.direction;
        // Army comes from the library map: decals such as blood have no entry in vid_armies_as2.h.
        sprite.army = member.army;
        sprite.gamma = member.gamma;
        sprite.scale = member.scale;
        sprites.push_back(sprite);
    }

    return sprites;
}

} // namespace auto_mapper::core::decoration_builder
