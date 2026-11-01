/**
 * @file scatter_fill.cpp
 * @brief Center-outwards dart throwing shared by the Loot Box and Gore brushes.
 */

#include "auto_mapper/core/scatter/scatter_fill.h"

#include "auto_mapper/core/randomizer.h"

#include <algorithm>
#include <cmath>

namespace auto_mapper::core {
namespace {

// Dart throws per spacing-sized cell of the brush circle, and a hard cap per dab.
constexpr double ATTEMPTS_PER_CELL = 6.0;
constexpr int ATTEMPTS_MAX = 4000;
constexpr double PI = 3.14159265358979323846;

float distance_between(float ax, float ay, float bx, float by) {
    return std::hypot(ax - bx, ay - by);
}

bool far_from_all(const std::vector<ScatterPoint>& points, const ScatterPoint& candidate, float min_distance) {
    for (const ScatterPoint& point : points) {
        if (distance_between(point.x, point.y, candidate.x, candidate.y) < min_distance) {
            return false;
        }
    }
    return true;
}

struct Candidate {
    float distance = 0.0f;
    ScatterPoint point;
};

bool closer_to_center(const Candidate& first, const Candidate& second) {
    return first.distance < second.distance;
}

} // namespace

ScatterPoint random_point_in_circle(float center_x, float center_y, float radius) {
    double r = radius * std::sqrt(Random::get_real(0.0, 1.0));
    double angle = Random::get_real(0.0, 2.0 * PI);
    ScatterPoint point;
    point.x = center_x + static_cast<float>(r * std::cos(angle));
    point.y = center_y + static_cast<float>(r * std::sin(angle));
    return point;
}

std::vector<ScatterPoint> fill_from_center(float center_x, float center_y, float radius, float spacing,
                                           std::vector<ScatterPoint>& occupied) {
    double cells = PI * radius * radius / (spacing * spacing);
    int attempts = std::clamp(static_cast<int>(cells * ATTEMPTS_PER_CELL), 1, ATTEMPTS_MAX);

    std::vector<Candidate> candidates;
    candidates.reserve(attempts + 1);
    candidates.push_back({0.0f, {center_x, center_y}});
    for (int attempt = 0; attempt < attempts; ++attempt) {
        ScatterPoint point = random_point_in_circle(center_x, center_y, radius);
        float distance = distance_between(point.x, point.y, center_x, center_y);
        candidates.push_back({distance, point});
    }
    std::sort(candidates.begin(), candidates.end(), closer_to_center);

    std::vector<ScatterPoint> spots;
    for (const Candidate& candidate : candidates) {
        if (!far_from_all(occupied, candidate.point, spacing)) {
            continue;
        }
        spots.push_back(candidate.point);
        occupied.push_back(candidate.point);
    }
    return spots;
}

} // namespace auto_mapper::core
