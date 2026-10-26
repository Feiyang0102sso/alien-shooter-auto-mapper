/**
 * @file game_family.h
 * @brief Types shared by every game asset reader: game family and the OBJ color gamma.
 */

#pragma once

namespace auto_mapper::game_assets {

/**
 * Engine generation of a game folder. AS1 covers Alien Shooter 1 (and Theseus);
 * AS2 covers AS2, AS2 Reloaded, Conscription and Zombie Shooter 2.
 */
enum class GameFamily {
    AS1,
    AS2
};

/**
 * Per-object color adjustment stored as GammaRGBA in objects.res.
 * A negative delta scales the channel down, a positive delta adds to it,
 * e.g. AS1 Standard Dark walls reuse 601.vid with (-80, -70, -60).
 */
struct ColorGamma {
    int red = 0;
    int green = 0;
    int blue = 0;
};

} // namespace auto_mapper::game_assets
