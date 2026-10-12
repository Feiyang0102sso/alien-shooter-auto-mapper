// Native editor state. Grid walls and physical decoration coordinates stay distinct.
#pragma once
#include "auto_mapper/core/map_generator.h"
#include <QJsonObject>
#include <QPoint>
#include <QPointF>
#include <QString>
#include <utility>
#include <vector>

namespace editor {

// Walls and doors use the core types directly (Grid Coordinates).
using auto_mapper::core::DoorInstance;
using auto_mapper::core::Segment;

// Project Version names stored in project JSON.
inline constexpr const char *PROJECT_VERSION_AS1 = "AS1";
inline constexpr const char *PROJECT_VERSION_AS2 = "AS2";
inline constexpr const char *PROJECT_VERSION_AS2R = "AS2R";
inline constexpr const char *PROJECT_VERSION_AS2OE = "AS2OE";

// Default Map Coordinate size of a new project.
inline constexpr double AS1_DEFAULT_MAP_SIZE = 600.0;
inline constexpr double AS2_DEFAULT_MAP_SIZE = 3000.0;
inline constexpr double MAX_MAP_SIZE = 100000.0;

// Decoration types stored in project JSON.
inline constexpr const char *DECORATION_TYPE_ROOM_STAMP = "room_stamp";
inline constexpr const char *DECORATION_TYPE_INCUBATOR_ARRAY = "incubator_array";
inline constexpr const char *DECORATION_TYPE_DESK_ARRAY = "desk_array";

// One placed decoration. `origin` is a Map Coordinate: a stamp center, or an array start corner.
struct Decoration {
    QString type = DECORATION_TYPE_ROOM_STAMP;
    QString profileId;
    QPointF origin;
    double rowLength = 0;
    double columnLength = 0;
    double itemSpacing = 1;
    double rowSpacing = 1;
};

struct Project {
    QString version = PROJECT_VERSION_AS1;
    double width = AS1_DEFAULT_MAP_SIZE;
    double height = AS1_DEFAULT_MAP_SIZE;
    // Global Door State Override; only meaningful for AS1.
    bool doorsOpen = false;
    // 0 means "use the core default layer count".
    int standardLayers = 0;
    int labLayers = 0;
    std::vector<Segment> segments;
    std::vector<DoorInstance> doors;
    std::vector<Decoration> decorations;

    bool isAS1() const;
    bool empty() const;
    auto_mapper::io::MapFormat mapFormat() const;
    QJsonObject toJson() const;
    static Project fromJson(const QJsonObject &object);
    static Project load(const QString &path);
    // An empty project in the given version, sized with that version's default map size.
    static Project createEmpty(const QString &version);
    void save(const QString &path) const;
    bool generate(const QString &path, bool floor, bool ceiling, bool random) const;
};

double defaultMapSize(const QString &version);
// The Global Door State Override exists only in AS1.
bool supportsGlobalDoorState(const QString &version);
// Door positions address grid edges; drawing and erasing work on the two end vertices.
std::pair<QPoint, QPoint> doorGridEndpoints(const DoorInstance &door);

} // namespace editor
