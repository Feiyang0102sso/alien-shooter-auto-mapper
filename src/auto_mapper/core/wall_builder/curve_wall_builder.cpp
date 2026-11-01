/**
 * @file curve_wall_builder.cpp
 * @brief Curve Wall smoothing, freehand simplification, eraser cuts and piece placement.
 */

#include "auto_mapper/core/wall_builder/curve_wall_builder.h"
#include "auto_mapper/common/logger.h"
#include "auto_mapper/core/randomizer.h"
#include "auto_mapper/core/vid_properties/vid_armies_as2.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace auto_mapper::core {
namespace {

// Spline samples per control-point gap; dense enough that chord length matches arc length.
constexpr int SMOOTH_SAMPLES_PER_SEGMENT = 16;
// Map y is squashed by the isometric view: 28 / 40, the Standard wall step ratio.
// Fitting official maps gives the same value (0.6 .. 0.8 all fit within noise).
constexpr float ISO_Y_SCALE = 0.7f;
// Arc length on each side of a piece used to measure its tangent.
constexpr float TANGENT_PROBE_LENGTH = 8.0f;
// Engine direction units per full turn.
constexpr int DIRECTION_UNITS = 256;
// Fewest pieces a closed curve can have and still enclose an area.
constexpr int MIN_CLOSED_PIECE_COUNT = 3;
// Eraser cuts: a remaining run shorter than this share of the piece spacing is erased too.
constexpr float MIN_RUN_SPACING_SHARE = 0.5f;
// Points closer than this (Map Coordinates) are merged when a cut run is assembled.
constexpr float DUPLICATE_POINT_DISTANCE = 0.5f;

// 2670: its broad face shows at frame 0, so the tangent maps straight to the direction.
// End reach: solid pixels span -132 .. +119 along a horizontal run, and reach 83 .. 96 below the
// anchor on a vertical run (/ 0.7 -> 119 .. 137 on the ground).
constexpr CurveWallProfile ROCK_PROFILE = {2670, AS2_VID_2670_ARMY, 110.0f, 0.0f, 4, 125.0f, false,
                                           FLOOR_TYPE_CURVE_ROCK};
// 2700: its broad faces sit about 3 frames later than 2670's; official maps put it 27 degrees back.
// End reach: -170 .. +136 / -137 .. +172 horizontally, 94 .. 121 below the anchor vertically (134 .. 173).
constexpr CurveWallProfile ALIEN_NEST_PROFILE = {2700, AS2_VID_2700_ARMY, 130.0f, -27.0f, 4, 155.0f, false,
                                                 FLOOR_TYPE_CURVE_ALIEN_NEST};
// 3300 (16 directions): every official chain keeps one face, at +40 units (+56 degrees) from its tangent
// in walking order, so the drawing direction picks the side and directions are not folded by half a turn.
// Gaps cluster at 170 .. 200; solid pixels span about -200 .. +200 along a horizontal run.
constexpr CurveWallProfile CLIFF_PROFILE = {3300, AS2_VID_3300_ARMY, 185.0f, 56.0f, 4, 200.0f, true,
                                            FLOOR_TYPE_CURVE_CLIFF};

float distance(const MapPoint& a, const MapPoint& b) {
    return std::hypot(b.x - a.x, b.y - a.y);
}

MapPoint catmull_rom(const MapPoint& p0, const MapPoint& p1, const MapPoint& p2, const MapPoint& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    MapPoint result{};
    result.x = 0.5f * (2.0f * p1.x + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                       (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
    result.y = 0.5f * (2.0f * p1.y + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
                       (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
    return result;
}

// Control point by index: a closed curve wraps around, an open one repeats its end points.
const MapPoint& control_point(const CurveWall& wall, int index) {
    int count = static_cast<int>(wall.points.size());
    if (wall.closed) {
        int wrapped = ((index % count) + count) % count;
        return wall.points[wrapped];
    }
    if (index < 0) {
        return wall.points.front();
    }
    if (index >= count) {
        return wall.points.back();
    }
    return wall.points[index];
}

/**
 * Point at arc length `length` along `path`; `cumulative[i]` is the arc length up to path[i].
 * A closed path wraps lengths outside [0, total], an open one clamps them.
 */
MapPoint point_at_length(const std::vector<MapPoint>& path, const std::vector<float>& cumulative, float length,
                         bool closed) {
    float total = cumulative.back();
    if (closed) {
        length = std::fmod(length, total);
        if (length < 0.0f) {
            length += total;
        }
    } else {
        length = std::clamp(length, 0.0f, total);
    }

    size_t index = 1;
    while (index < path.size() - 1 && cumulative[index] < length) {
        ++index;
    }
    float span = cumulative[index] - cumulative[index - 1];
    float t = 0.0f;
    if (span > 0.0f) {
        t = (length - cumulative[index - 1]) / span;
    }
    MapPoint result{};
    result.x = path[index - 1].x + (path[index].x - path[index - 1].x) * t;
    result.y = path[index - 1].y + (path[index].y - path[index - 1].y) * t;
    return result;
}

/**
 * Where segment a -> b crosses the circle. `entering` picks the first crossing (a outside, b inside),
 * otherwise the last one (a inside, b outside).
 */
MapPoint circle_crossing(const MapPoint& a, const MapPoint& b, const MapPoint& center, float radius, bool entering) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float fx = a.x - center.x;
    float fy = a.y - center.y;
    // |a + t * (b - a) - center| = radius  ->  qa * t^2 + qb * t + qc = 0
    float qa = dx * dx + dy * dy;
    float qb = 2.0f * (fx * dx + fy * dy);
    float qc = fx * fx + fy * fy - radius * radius;
    if (qa <= 0.0f) {
        return a;
    }
    float discriminant = std::max(0.0f, qb * qb - 4.0f * qa * qc);
    float root = std::sqrt(discriminant);
    float t = (-qb + root) / (2.0f * qa);
    if (entering) {
        t = (-qb - root) / (2.0f * qa);
    }
    t = std::clamp(t, 0.0f, 1.0f);
    MapPoint result{};
    result.x = a.x + dx * t;
    result.y = a.y + dy * t;
    return result;
}

float polyline_length(const std::vector<MapPoint>& points) {
    float total = 0.0f;
    for (size_t index = 1; index < points.size(); ++index) {
        total += distance(points[index - 1], points[index]);
    }
    return total;
}

/**
 * Turn one run left by an eraser cut into an open curve, or nothing when it is too short.
 * `points` are the run ends and the original control points between them, in order; `middle` is a
 * smoothed sample from the middle of the run, used when no control point lies between the two ends.
 */
void keep_run(const CurveWall& source, const std::vector<MapPoint>& points, const MapPoint& middle,
              std::vector<CurveWall>& result) {
    CurveWall run;
    run.curve_wall_type = source.curve_wall_type;
    run.closed = false;
    for (const MapPoint& point : points) {
        if (!run.points.empty() && distance(run.points.back(), point) < DUPLICATE_POINT_DISTANCE) {
            continue;
        }
        run.points.push_back(point);
    }
    if (run.points.size() == 2) {
        // Keep the bend of a short run that fell between two control points.
        run.points.insert(run.points.begin() + 1, middle);
    }
    if (run.points.size() < 2) {
        return;
    }
    float minimum = get_curve_wall_profile(source.curve_wall_type).spacing * MIN_RUN_SPACING_SHARE;
    if (polyline_length(smooth_curve_wall(run)) < minimum) {
        return;
    }
    result.push_back(run);
}

/**
 * Arc length (Map Coordinates) that covers `ground_reach` on the ground plane, measured along the curve
 * from `end` toward `inward`. A run down the screen is squashed by 0.7, so it needs less Map length.
 */
float map_length_for_ground(const MapPoint& end, const MapPoint& inward, float ground_reach) {
    float dx = inward.x - end.x;
    float dy = inward.y - end.y;
    float map_length = std::hypot(dx, dy);
    if (map_length <= 0.0f) {
        return ground_reach;
    }
    float ground_length = std::hypot(dx, dy / ISO_Y_SCALE);
    return ground_reach * map_length / ground_length;
}

// Engine direction (0 = up, clockwise) for a piece whose curve runs from `before` to `after`.
int tangent_direction(const MapPoint& before, const MapPoint& after, float angle_offset_degrees) {
    // Un-squash Map y so the angle is measured on the ground plane.
    float dx = after.x - before.x;
    float dy = (after.y - before.y) / ISO_Y_SCALE;
    // The piece faces its curve normal, and direction 0 faces up, so the tangent angle measured
    // from +x (clockwise, screen y down) is already the facing direction.
    float degrees = std::atan2(dy, dx) * 180.0f / std::numbers::pi_v<float> + angle_offset_degrees;
    int units = static_cast<int>(std::lround(degrees * DIRECTION_UNITS / 360.0f));
    return ((units % DIRECTION_UNITS) + DIRECTION_UNITS) % DIRECTION_UNITS;
}

} // namespace

bool is_supported_curve_wall_type(int curve_wall_type) {
    if (curve_wall_type == CURVE_WALL_TYPE_ROCK || curve_wall_type == CURVE_WALL_TYPE_ALIEN_NEST) {
        return true;
    }
    return curve_wall_type == CURVE_WALL_TYPE_CLIFF;
}

const CurveWallProfile& get_curve_wall_profile(int curve_wall_type) {
    if (curve_wall_type == CURVE_WALL_TYPE_ROCK) {
        return ROCK_PROFILE;
    }
    if (curve_wall_type == CURVE_WALL_TYPE_ALIEN_NEST) {
        return ALIEN_NEST_PROFILE;
    }
    if (curve_wall_type == CURVE_WALL_TYPE_CLIFF) {
        return CLIFF_PROFILE;
    }
    throw std::out_of_range("Unknown curve wall type: " + std::to_string(curve_wall_type));
}

std::vector<MapPoint> smooth_curve_wall(const CurveWall& wall) {
    std::vector<MapPoint> path;
    int count = static_cast<int>(wall.points.size());
    if (count < 2) {
        return wall.points;
    }

    int segment_count = count - 1;
    if (wall.closed) {
        segment_count = count;
    }
    for (int segment = 0; segment < segment_count; ++segment) {
        const MapPoint& p0 = control_point(wall, segment - 1);
        const MapPoint& p1 = control_point(wall, segment);
        const MapPoint& p2 = control_point(wall, segment + 1);
        const MapPoint& p3 = control_point(wall, segment + 2);
        for (int sample = 0; sample < SMOOTH_SAMPLES_PER_SEGMENT; ++sample) {
            float t = static_cast<float>(sample) / SMOOTH_SAMPLES_PER_SEGMENT;
            path.push_back(catmull_rom(p0, p1, p2, p3, t));
        }
    }
    // Close the loop on its first point, or end exactly on the last control point.
    if (wall.closed) {
        path.push_back(wall.points.front());
    } else {
        path.push_back(wall.points.back());
    }
    return path;
}

std::vector<MapPoint> simplify_stroke(const std::vector<MapPoint>& stroke, float tolerance) {
    if (stroke.size() < 3) {
        return stroke;
    }

    // Iterative Ramer-Douglas-Peucker: keep the farthest point of a span while it is off by more than tolerance.
    std::vector<bool> keep(stroke.size(), false);
    keep.front() = true;
    keep.back() = true;
    std::vector<std::pair<size_t, size_t>> spans;
    spans.push_back({0, stroke.size() - 1});
    while (!spans.empty()) {
        auto [first, last] = spans.back();
        spans.pop_back();
        const MapPoint& a = stroke[first];
        const MapPoint& b = stroke[last];
        float chord = distance(a, b);

        float farthest_distance = 0.0f;
        size_t farthest = first;
        for (size_t index = first + 1; index < last; ++index) {
            const MapPoint& p = stroke[index];
            float offset = 0.0f;
            if (chord > 0.0f) {
                offset = std::abs((b.x - a.x) * (a.y - p.y) - (a.x - p.x) * (b.y - a.y)) / chord;
            } else {
                offset = distance(a, p);
            }
            if (offset > farthest_distance) {
                farthest_distance = offset;
                farthest = index;
            }
        }
        if (farthest_distance > tolerance) {
            keep[farthest] = true;
            spans.push_back({first, farthest});
            spans.push_back({farthest, last});
        }
    }

    std::vector<MapPoint> result;
    for (size_t index = 0; index < stroke.size(); ++index) {
        if (keep[index]) {
            result.push_back(stroke[index]);
        }
    }
    return result;
}

std::vector<CurveWall> erase_curve_wall_circle(const CurveWall& wall, const MapPoint& center, float radius) {
    std::vector<CurveWall> result;
    std::vector<MapPoint> path = smooth_curve_wall(wall);
    if (!is_supported_curve_wall_type(wall.curve_wall_type) || path.size() < 2) {
        result.push_back(wall);
        return result;
    }

    // A closed path repeats its first sample at the end; walk it as a ring without that copy.
    int sample_count = static_cast<int>(path.size());
    if (wall.closed) {
        sample_count -= 1;
    }
    std::vector<bool> erased(sample_count, false);
    int first_erased = -1;
    bool any_kept = false;
    for (int sample = 0; sample < sample_count; ++sample) {
        erased[sample] = distance(path[sample], center) <= radius;
        if (erased[sample] && first_erased < 0) {
            first_erased = sample;
        }
        if (!erased[sample]) {
            any_kept = true;
        }
    }
    if (first_erased < 0) {
        result.push_back(wall);
        return result;
    }
    if (!any_kept) {
        return result;
    }

    // An open curve is walked from its start. A loop is walked from just after an erased sample,
    // so every kept run is one contiguous stretch of the walk.
    int start = 0;
    if (wall.closed) {
        start = first_erased + 1;
    }
    std::vector<MapPoint> run;
    std::vector<int> run_samples;
    for (int step = 0; step < sample_count; ++step) {
        int sample = (start + step) % sample_count;
        int previous = (sample - 1 + sample_count) % sample_count;
        if (erased[sample]) {
            if (!run.empty()) {
                // The run ends where the path enters the circle.
                run.push_back(circle_crossing(path[previous], path[sample], center, radius, true));
                keep_run(wall, run, path[run_samples[run_samples.size() / 2]], result);
                run.clear();
                run_samples.clear();
            }
            continue;
        }
        if (run.empty()) {
            // A run starts where the path leaves the circle, or at the start of an open curve.
            bool curve_start = !wall.closed && sample == 0;
            if (!curve_start) {
                run.push_back(circle_crossing(path[previous], path[sample], center, radius, false));
            }
        }
        run_samples.push_back(sample);
        // A sample on a control point keeps that control point (the open curve end is its last point).
        if (sample % SMOOTH_SAMPLES_PER_SEGMENT == 0) {
            int control = (sample / SMOOTH_SAMPLES_PER_SEGMENT) % static_cast<int>(wall.points.size());
            run.push_back(wall.points[control]);
        }
    }
    if (!run.empty()) {
        // Only an open curve can still have a run going here: it reached the curve end.
        keep_run(wall, run, path[run_samples[run_samples.size() / 2]], result);
    }
    return result;
}

std::vector<io::Sprite> build_curve_wall(const CurveWall& wall, bool randomize_directions) {
    std::vector<io::Sprite> sprites;
    if (!is_supported_curve_wall_type(wall.curve_wall_type)) {
        Logger::error("Skip curve wall with unknown type {}", wall.curve_wall_type);
        return sprites;
    }
    if (wall.points.size() < 2) {
        Logger::warning("Skip curve wall with {} control point(s)", wall.points.size());
        return sprites;
    }

    const CurveWallProfile& profile = get_curve_wall_profile(wall.curve_wall_type);
    std::vector<MapPoint> path = smooth_curve_wall(wall);
    std::vector<float> cumulative;
    cumulative.push_back(0.0f);
    for (size_t index = 1; index < path.size(); ++index) {
        cumulative.push_back(cumulative.back() + distance(path[index - 1], path[index]));
    }
    float total = cumulative.back();
    if (total <= 0.0f) {
        Logger::warning("Skip curve wall with zero length");
        return sprites;
    }

    // Arc length of every piece anchor, spread evenly.
    std::vector<float> piece_lengths;
    if (wall.closed) {
        // A loop has no ends.
        int piece_count = std::max(MIN_CLOSED_PIECE_COUNT, static_cast<int>(std::lround(total / profile.spacing)));
        float step = total / piece_count;
        for (int piece = 0; piece < piece_count; ++piece) {
            piece_lengths.push_back(piece * step);
        }
    } else {
        // End pieces sit inside the end points by the art's reach, so the art itself ends on them.
        MapPoint first = point_at_length(path, cumulative, 0.0f, false);
        MapPoint after_first = point_at_length(path, cumulative, TANGENT_PROBE_LENGTH, false);
        MapPoint last = point_at_length(path, cumulative, total, false);
        MapPoint before_last = point_at_length(path, cumulative, total - TANGENT_PROBE_LENGTH, false);
        float start = map_length_for_ground(first, after_first, profile.end_reach);
        float end = total - map_length_for_ground(last, before_last, profile.end_reach);
        float usable = end - start;
        int piece_count = 1;
        if (usable > 0.0f) {
            piece_count = static_cast<int>(std::lround(usable / profile.spacing)) + 1;
        }
        if (piece_count < 2) {
            // Too short for two pieces: one piece in the middle.
            piece_lengths.push_back(total / 2.0f);
        } else {
            float step = usable / (piece_count - 1);
            for (int piece = 0; piece < piece_count; ++piece) {
                piece_lengths.push_back(start + piece * step);
            }
        }
    }

    DirectionRandomizer randomizer(randomize_directions);
    for (float length : piece_lengths) {
        MapPoint anchor = point_at_length(path, cumulative, length, wall.closed);
        MapPoint before = point_at_length(path, cumulative, length - TANGENT_PROBE_LENGTH, wall.closed);
        MapPoint after = point_at_length(path, cumulative, length + TANGENT_PROBE_LENGTH, wall.closed);
        int direction = tangent_direction(before, after, profile.angle_offset_degrees);
        // Official pieces deviate a few units from the tangent; mimic that only when randomization is on.
        int jittered = randomizer.select(direction, direction - profile.direction_jitter,
                                         direction + profile.direction_jitter);
        jittered = ((jittered % DIRECTION_UNITS) + DIRECTION_UNITS) % DIRECTION_UNITS;

        io::Sprite sprite(profile.vid, std::round(anchor.x), std::round(anchor.y), 0.0f,
                          static_cast<uint32_t>(jittered));
        sprite.army = profile.army;
        sprites.push_back(sprite);
    }
    return sprites;
}

} // namespace auto_mapper::core
