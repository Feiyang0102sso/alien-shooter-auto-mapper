// Decoration stamp sprite preview: real game frames for stamp members, read from an AS2 game folder.
// Walls, doors and floors never use it; without a game folder the canvas keeps its green crosses.
#pragma once
#include "auto_mapper/core/decoration_builder/decoration_stamp.h"
#include "auto_mapper/io/map_writer.h"
#include "game_assets/vid/vid_library.h"
#include <QImage>
#include <QPoint>
#include <QString>
#include <map>
#include <optional>
#include <tuple>
#include <vector>

namespace editor {

// One decoded member frame. `topLeft` is relative to the sprite anchor (posX, posY - posZ), in map units.
struct StampSprite {
    QImage image;
    QPoint topLeft;
};

// Coarse painter order (the game uses a z-buffer): flat ground sprites first, then standing ones by posY, where a
// sprite resting on another (stacked cabinet, computer on a desk) takes its support's posY and draws right after it,
// then flat sprites raised onto others (blood on a corpse).
// `format` picks that game's ground / air-wall nvid lists.
std::vector<const auto_mapper::core::decoration_builder::DecorationMember *>
stampDrawOrder(const auto_mapper::core::decoration_builder::DecorationStamp &stamp, auto_mapper::io::MapFormat format);

class StampSprites {
  public:
    // Open <gameRoot>/objects.res and Vid/ with that family's OBJ layout; false keeps the placeholder preview.
    bool open(const QString &gameRoot, auto_mapper::game_assets::GameFamily family);
    bool isOpen() const;
    // Frame with the OBJ and map sprite gamma applied, or nullptr when it cannot be decoded.
    const StampSprite *sprite(const auto_mapper::core::decoration_builder::DecorationMember &member);
    // Every decoded member drawn into one transparent image at 1 pixel per map unit, cropped to the sprites.
    // Null when no game folder is open or no member could be decoded.
    QImage render(const auto_mapper::core::decoration_builder::DecorationStamp &stamp, auto_mapper::io::MapFormat format);

  private:
    // (nvid, direction, map gamma r, g, b): the same vid looks different per direction and gamma.
    using Key = std::tuple<int, std::uint32_t, int, int, int>;

    auto_mapper::game_assets::VidLibrary library_;
    // std::nullopt remembers members whose frame cannot be decoded.
    std::map<Key, std::optional<StampSprite>> cache_;
};

} // namespace editor
