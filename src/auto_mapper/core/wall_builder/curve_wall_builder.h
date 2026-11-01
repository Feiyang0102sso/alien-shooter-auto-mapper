/**
 * @file curve_wall_builder.h
 * @brief Curve Walls: AS2 rock / alien nest / cliff pieces laid along a freehand curve.
 *
 * A Curve Wall is a list of control points in Map Coordinates. The builder smooths it with a
 * Catmull-Rom spline, places one piece every `spacing` units of arc length and turns each piece
 * to the local tangent. The engine draws the piece frame for its direction (32 frames over 0..255)
 * and links the matching air wall by itself, so only the visible nvid is written.
 *
 * Calibration comes from the official maps (AS2R Level_13 uses 2670, Level_14 uses 2700,
 * Level_04 / AS2R Level_08 use 3300): direction = tangent angle with Map y un-squashed by 0.7,
 * plus a per-asset offset.
 */

#pragma once

#include "auto_mapper/core/geometry.h"
#include "auto_mapper/core/vid_properties/vid_property_types.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>

namespace auto_mapper::core {

inline constexpr int CURVE_WALL_TYPE_ROCK = 0;
inline constexpr int CURVE_WALL_TYPE_ALIEN_NEST = 1;
inline constexpr int CURVE_WALL_TYPE_CLIFF = 2;

struct CurveWallProfile {
    int vid;
    VidArmy army;
    // Arc length between neighbouring pieces, in Map Coordinates.
    float spacing;
    // Added to the tangent angle: the asset's broad face is not always drawn at direction 0.
    float angle_offset_degrees;
    // Largest random direction change (0..255 units) when Direction Randomization is enabled.
    int direction_jitter;
    // How far one piece's art reaches past its anchor along the curve, measured on the ground plane
    // (Map y un-squashed). Open curves keep their end pieces this far inside, so the art ends on the end points.
    float end_reach;
    // One-sided assets show a different face on each side. For the 3300 cliff the layered face is on the
    // left of the drawing direction and the cracked face on the right. Two-sided assets look alike from both sides.
    bool one_sided;
};

/**
 * One curve drawn by the user. `points` are control points in Map Coordinates;
 * a closed curve joins the last point back to the first.
 */
struct CurveWall {
    int curve_wall_type = CURVE_WALL_TYPE_ROCK;
    std::vector<MapPoint> points;
    bool closed = false;
};

bool is_supported_curve_wall_type(int curve_wall_type);
// Throws std::out_of_range for an unknown type.
const CurveWallProfile& get_curve_wall_profile(int curve_wall_type);

/**
 * Dense polyline through every control point (uniform Catmull-Rom).
 * A closed curve repeats its first point at the end. The editor draws and hit-tests this path.
 */
std::vector<MapPoint> smooth_curve_wall(const CurveWall& wall);

/**
 * Reduce a raw freehand stroke to control points (Ramer-Douglas-Peucker).
 * `tolerance` is the largest allowed distance, in Map Coordinates, between the stroke and the result.
 */
std::vector<MapPoint> simplify_stroke(const std::vector<MapPoint>& stroke, float tolerance);

/**
 * Erase the part of `wall` inside a circle (Map Coordinates), the way an eraser brush cuts a curve.
 * Returns what is left: the unchanged wall when the circle misses it, nothing when it covers it all,
 * otherwise one open curve per remaining run. A closed loop opens at the cut.
 * Each run keeps the original control points it contains and ends exactly on the circle, so repeated
 * cuts never reshape the untouched parts. Runs shorter than half a piece spacing are dropped.
 */
std::vector<CurveWall> erase_curve_wall_circle(const CurveWall& wall, const MapPoint& center, float radius);

/**
 * Pieces of one Curve Wall, ordered along the curve.
 * An open curve keeps its end pieces `end_reach` inside its end points; one too short for two pieces gets
 * a single piece in its middle. Without randomization every piece faces its exact tangent, so the output
 * is deterministic.
 */
std::vector<io::Sprite> build_curve_wall(const CurveWall& wall, bool randomize_directions);

} // namespace auto_mapper::core
