// JSON compatibility and the native C++ generator boundary.
#include "editor/project/project.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSaveFile>
#include <cmath>
#include <stdexcept>

namespace editor {
namespace {
void fail(const QString &message) {
    throw std::runtime_error(message.toStdString());
}

QJsonObject objectValue(const QJsonValue &value) {
    if (value.isUndefined())
        return {};
    if (!value.isObject())
        fail("Expected a JSON object.");
    return value.toObject();
}

QJsonArray arrayValue(const QJsonValue &value) {
    if (value.isUndefined() || value.isNull())
        return {};
    if (!value.isArray())
        fail("Expected a JSON array.");
    return value.toArray();
}

double number(const QJsonObject &obj, const QString &key, double fallback = 0) {
    if (!obj.contains(key))
        return fallback;
    auto value = obj.value(key);
    if (!value.isDouble() || !std::isfinite(value.toDouble()))
        fail("Invalid number: " + key);
    return value.toDouble();
}

int integer(const QJsonObject &obj, const QString &key, int fallback = 0) {
    double value = number(obj, key, fallback);
    if (std::trunc(value) != value || std::abs(value) > 100000000)
        fail("Invalid integer: " + key);
    return static_cast<int>(value);
}

QJsonObject point(double x, double y) {
    return {{"x", x}, {"y", y}};
}

void validateLayers(int count) {
    if (count < auto_mapper::core::AS1_CEILING_LAYER_COUNT_MIN || count > auto_mapper::core::AS1_CEILING_LAYER_COUNT_MAX)
        fail("Invalid ceiling layer count.");
}
} // namespace

double defaultMapSize(const QString &version) {
    if (version == PROJECT_VERSION_AS1)
        return AS1_DEFAULT_MAP_SIZE;
    return AS2_DEFAULT_MAP_SIZE;
}

QString lootKindId(LootBoxKind kind) {
    if (kind == LootBoxKind::Metal)
        return LOOT_KIND_METAL;
    if (kind == LootBoxKind::Egg)
        return LOOT_KIND_EGG;
    return LOOT_KIND_WOOD;
}

LootBoxKind lootKindFromId(const QString &id) {
    for (LootBoxKind kind : LOOT_KINDS) {
        if (lootKindId(kind) == id)
            return kind;
    }
    fail("Unknown loot box kind: " + id);
    return LootBoxKind::Wood;
}

bool supportsGlobalDoorState(const QString &version) {
    return version == PROJECT_VERSION_AS1;
}

std::pair<QPoint, QPoint> doorGridEndpoints(const DoorInstance &door) {
    // direction_type 0 runs along grid y, 1 along grid x; pos is the first edge after the start vertex.
    int x = door.pos.x;
    int y = door.pos.y;
    if (door.direction_type == 0)
        return {{x, y - 1}, {x, y + door.size - 1}};
    return {{x - 1, y}, {x + door.size - 1, y}};
}

bool Project::isAS1() const {
    return version == PROJECT_VERSION_AS1;
}
bool Project::empty() const {
    return segments.empty() && doors.empty() && curveWalls.empty() && decorations.empty() && lootBoxes.empty();
}

auto_mapper::io::MapFormat Project::mapFormat() const {
    using auto_mapper::io::MapFormat;
    if (version == PROJECT_VERSION_AS1)
        return MapFormat::AS1;
    if (version == PROJECT_VERSION_AS2)
        return MapFormat::AS2;
    if (version == PROJECT_VERSION_AS2R)
        return MapFormat::AS2R;
    if (version == PROJECT_VERSION_AS2OE)
        return MapFormat::AS2OE;
    fail("Unsupported project version: " + version);
    return MapFormat::AS1;
}

QJsonObject Project::toJson() const {
    QJsonArray walls, doorItems, curveWallItems, decorationItems;
    for (const auto &segment : segments) {
        walls.append(QJsonObject{{"start", point(segment.start.x, segment.start.y)},
                                 {"end", point(segment.end.x, segment.end.y)},
                                 {"wall_type", segment.wall_type},
                                 {"floor_type", 0}});
    }
    for (const auto &door : doors) {
        doorItems.append(QJsonObject{{"pos", point(door.pos.x, door.pos.y)},
                                     {"wall_type", door.wall_type},
                                     {"direction_type", door.direction_type},
                                     {"size", door.size},
                                     {"door_state", door.door_state},
                                     {"light_state", door.light_state},
                                     {"z_offset", door.z_offset}});
    }
    for (const auto &curveWall : curveWalls) {
        QJsonArray points;
        for (const auto &controlPoint : curveWall.points)
            points.append(point(controlPoint.x, controlPoint.y));
        curveWallItems.append(QJsonObject{{"curve_wall_type", curveWall.curve_wall_type},
                                          {"closed", curveWall.closed},
                                          {"points", points}});
    }
    for (const auto &decoration : decorations) {
        QJsonObject item{{"type", decoration.type}};
        if (decoration.type == DECORATION_TYPE_ROOM_STAMP) {
            item["profile_id"] = decoration.profileId;
            item["center"] = point(decoration.origin.x(), decoration.origin.y());
        } else {
            item["start"] = point(decoration.origin.x(), decoration.origin.y());
            item["row_length"] = decoration.rowLength;
            item["column_length"] = decoration.columnLength;
            item["item_spacing_scale"] = decoration.itemSpacing;
            item["row_spacing_scale"] = decoration.rowSpacing;
        }
        decorationItems.append(item);
    }
    QJsonObject result{{"version", version}, {"map_size_x", width}, {"map_size_y", height},
                       {"segments", walls},  {"doors", doorItems},  {"decorations", decorationItems}};
    // Older readers ignore unknown keys, so the key is only written when there is something to keep.
    if (!curveWalls.empty())
        result["curve_walls"] = curveWallItems;
    if (!lootBoxes.empty()) {
        QJsonArray lootBoxItems;
        for (const auto &box : lootBoxes) {
            lootBoxItems.append(QJsonObject{{"vid", box.vid}, {"x", box.x}, {"y", box.y}, {"z", box.z},
                                            {"direction", static_cast<int>(box.direction)}});
        }
        result["loot_boxes"] = lootBoxItems;
    }
    QJsonObject lootTableItems;
    for (const auto &[kind, lines] : lootTables) {
        if (lines.empty())
            continue;
        QJsonArray lineItems;
        for (const auto &line : lines)
            lineItems.append(QJsonObject{{"text", QString::fromStdString(line.text)}, {"percent", line.percent}});
        lootTableItems[lootKindId(kind)] = lineItems;
    }
    if (!lootTableItems.isEmpty())
        result["loot_tables"] = lootTableItems;
    if (isAS1()) {
        result["is_door_open"] = doorsOpen;
        if (standardLayers > 0)
            result["as1_standard_ceiling_layer_count"] = standardLayers;
        if (labLayers > 0)
            result["as1_lab_ceiling_layer_count"] = labLayers;
    }
    return result;
}

Project Project::fromJson(const QJsonObject &obj) {
    Project result;
    // Legacy projects without "version" are AS1.
    result.version = obj.value("version").toString(PROJECT_VERSION_AS1);
    if (obj.contains("version") && !obj.value("version").isString())
        fail("Invalid project version.");
    result.mapFormat();
    double defaultSize = defaultMapSize(result.version);
    result.width = number(obj, "map_size_x", defaultSize);
    result.height = number(obj, "map_size_y", defaultSize);
    if (result.width <= 0 || result.height <= 0 || result.width > MAX_MAP_SIZE || result.height > MAX_MAP_SIZE)
        fail("Map dimensions must be between 1 and 100000.");
    if (result.isAS1()) {
        result.doorsOpen = obj.value("is_door_open").toBool(false);
        if (obj.contains("as1_standard_ceiling_layer_count") &&
            !obj.value("as1_standard_ceiling_layer_count").isNull()) {
            result.standardLayers = integer(obj, "as1_standard_ceiling_layer_count");
            validateLayers(result.standardLayers);
        }
        if (obj.contains("as1_lab_ceiling_layer_count") &&
            !obj.value("as1_lab_ceiling_layer_count").isNull()) {
            result.labLayers = integer(obj, "as1_lab_ceiling_layer_count");
            validateLayers(result.labLayers);
        }
    }
    for (const auto &value : arrayValue(obj.value("segments"))) {
        auto item = objectValue(value);
        auto start = objectValue(item.value("start"));
        auto end = objectValue(item.value("end"));
        Segment segment;
        segment.start = {integer(start, "x"), integer(start, "y")};
        segment.end = {integer(end, "x"), integer(end, "y")};
        segment.wall_type = integer(item, "wall_type");
        if (!auto_mapper::core::is_supported_wall_type(segment.wall_type))
            fail("Unsupported wall type.");
        if (segment.start.x != segment.end.x && segment.start.y != segment.end.y)
            fail("Walls must follow a grid axis.");
        result.segments.push_back(segment);
    }
    for (const auto &value : arrayValue(obj.value("doors"))) {
        auto item = objectValue(value);
        auto pos = objectValue(item.value("pos"));
        DoorInstance door{};
        door.pos = {integer(pos, "x"), integer(pos, "y")};
        door.wall_type = integer(item, "wall_type");
        door.direction_type = integer(item, "direction_type");
        door.size = integer(item, "size", 1);
        door.door_state = integer(item, "door_state");
        door.light_state = integer(item, "light_state");
        door.z_offset = static_cast<float>(number(item, "z_offset"));
        bool validWall = auto_mapper::core::is_supported_wall_type(door.wall_type);
        bool validSize = door.size >= 1 && door.size <= 2;
        bool validDirection = door.direction_type == 0 || door.direction_type == 1;
        if (!validWall || !validSize || !validDirection)
            fail("Invalid door.");
        result.doors.push_back(door);
    }
    for (const auto &value : arrayValue(obj.value("curve_walls"))) {
        auto item = objectValue(value);
        CurveWall curveWall;
        curveWall.curve_wall_type = integer(item, "curve_wall_type");
        curveWall.closed = item.value("closed").toBool(false);
        for (const auto &pointValue : arrayValue(item.value("points"))) {
            auto controlPoint = objectValue(pointValue);
            curveWall.points.push_back({static_cast<float>(number(controlPoint, "x")),
                                        static_cast<float>(number(controlPoint, "y"))});
        }
        if (!auto_mapper::core::is_supported_curve_wall_type(curveWall.curve_wall_type))
            fail("Unsupported curve wall type.");
        if (curveWall.points.size() < 2)
            fail("A curve wall needs at least two points.");
        // AS1 has no 2670 / 2700 assets.
        if (result.isAS1())
            fail("Curve walls need an AS2 Series project.");
        result.curveWalls.push_back(curveWall);
    }
    for (const auto &value : arrayValue(obj.value("decorations"))) {
        auto item = objectValue(value);
        Decoration decoration;
        decoration.type = item.value("type").toString();
        bool isStamp = decoration.type == DECORATION_TYPE_ROOM_STAMP;
        bool isArray = decoration.type == DECORATION_TYPE_INCUBATOR_ARRAY || decoration.type == DECORATION_TYPE_DESK_ARRAY;
        if (!isStamp && !isArray) {
            qWarning() << "Skipped unsupported decoration type:" << decoration.type;
            continue;
        }
        // Stamps store their center; arrays store their start corner.
        QString positionKey = "start";
        if (isStamp)
            positionKey = "center";
        auto pos = objectValue(item.value(positionKey));
        decoration.origin = {number(pos, "x"), number(pos, "y")};
        if (isStamp) {
            // Stamp ids are checked against the decoration library by the caller (MainWindow::openProject).
            decoration.profileId = item.value("profile_id").toString();
        } else {
            decoration.rowLength = number(item, "row_length");
            decoration.columnLength = number(item, "column_length");
            decoration.itemSpacing = number(item, "item_spacing_scale", 1);
            decoration.rowSpacing = number(item, "row_spacing_scale", 1);
            if (decoration.rowLength < 0 || decoration.columnLength < 0 || decoration.itemSpacing <= 0 ||
                decoration.rowSpacing <= 0)
                fail("Invalid decoration array dimensions or spacing.");
        }
        result.decorations.push_back(decoration);
    }
    for (const auto &value : arrayValue(obj.value("loot_boxes"))) {
        auto item = objectValue(value);
        LootBox box;
        box.vid = integer(item, "vid");
        box.x = static_cast<float>(number(item, "x"));
        box.y = static_cast<float>(number(item, "y"));
        box.z = static_cast<float>(number(item, "z"));
        box.direction = static_cast<uint32_t>(integer(item, "direction"));
        bool known = false;
        for (LootBoxKind kind : LOOT_KINDS) {
            if (auto_mapper::core::loot_box_belongs_to(box.vid, kind))
                known = true;
        }
        if (!known)
            fail("Unsupported loot box vid.");
        // The Loot Box brush and nameText exist only in the AS2 Series.
        if (result.isAS1())
            fail("Loot boxes need an AS2 Series project.");
        result.lootBoxes.push_back(box);
    }
    QJsonObject lootTableItems = objectValue(obj.value("loot_tables"));
    for (auto entry = lootTableItems.begin(); entry != lootTableItems.end(); ++entry) {
        LootBoxKind kind = lootKindFromId(entry.key());
        auto_mapper::core::LootTable table;
        for (const auto &lineValue : arrayValue(entry.value())) {
            auto lineItem = objectValue(lineValue);
            LootLine line;
            line.text = lineItem.value("text").toString().toStdString();
            line.percent = number(lineItem, "percent");
            table.lines.push_back(line);
        }
        if (!auto_mapper::core::is_valid_loot_table(table))
            fail("Loot table percents must be 0..100 and add up to at most 100.");
        result.lootTables[kind] = table.lines;
    }
    return result;
}

Project Project::load(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        fail(file.errorString());
    QJsonParseError error;
    auto document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError)
        fail(error.errorString());
    if (!document.isObject())
        fail("Project JSON root must be an object.");
    Project project = fromJson(document.object());
    qInfo() << "Project loaded:" << path << "segments" << project.segments.size() << "doors" << project.doors.size()
            << "curve walls" << project.curveWalls.size();
    return project;
}

Project Project::createEmpty(const QString &version) {
    Project project;
    project.version = version;
    project.mapFormat();
    project.width = defaultMapSize(version);
    project.height = defaultMapSize(version);
    return project;
}

void Project::save(const QString &path) const {
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        fail(file.errorString());
    auto bytes = QJsonDocument(toJson()).toJson(QJsonDocument::Indented);
    if (file.write(bytes) != bytes.size() || !file.commit())
        fail(file.errorString());
    qInfo() << "Project saved:" << path;
}

bool Project::generate(const QString &path, bool floor, bool ceiling, bool random,
                       const auto_mapper::core::decoration_builder::DecorationLibrary *decorationLibrary) const {
    auto_mapper::core::MapRequest request;
    request.format = mapFormat();
    request.map_size_x = static_cast<float>(width);
    request.map_size_y = static_cast<float>(height);
    request.segments = segments;
    request.doors = doors;
    request.curve_walls = curveWalls;
    request.generate_floor = floor;
    request.generate_ceiling = ceiling;
    request.random_direction = random;
    request.decoration_library = decorationLibrary;
    request.loot_boxes = lootBoxes;
    for (const auto &[kind, lines] : lootTables) {
        auto_mapper::core::LootTable table;
        table.vids = auto_mapper::core::loot_table_vids(kind);
        table.lines = lines;
        request.loot_tables.push_back(table);
    }

    for (const auto &d : decorations) {
        if (d.type == DECORATION_TYPE_ROOM_STAMP) {
            auto_mapper::core::StampPlacement stamp;
            stamp.stamp_id = d.profileId.toStdString();
            stamp.center_x = static_cast<float>(d.origin.x());
            stamp.center_y = static_cast<float>(d.origin.y());
            request.stamps.push_back(stamp);
        } else if (d.type == DECORATION_TYPE_INCUBATOR_ARRAY) {
            auto_mapper::core::indoor_decorations::IncubatorArray array{};
            array.start_x = static_cast<float>(d.origin.x());
            array.start_y = static_cast<float>(d.origin.y());
            array.row_length = static_cast<float>(d.rowLength);
            array.column_length = static_cast<float>(d.columnLength);
            array.item_spacing_scale = static_cast<float>(d.itemSpacing);
            array.row_spacing_scale = static_cast<float>(d.rowSpacing);
            request.incubator_arrays.push_back(array);
        } else {
            auto_mapper::core::indoor_decorations::DeskArray array{};
            array.start_x = static_cast<float>(d.origin.x());
            array.start_y = static_cast<float>(d.origin.y());
            array.row_length = static_cast<float>(d.rowLength);
            array.column_length = static_cast<float>(d.columnLength);
            array.item_spacing_scale = static_cast<float>(d.itemSpacing);
            array.row_spacing_scale = static_cast<float>(d.rowSpacing);
            request.desk_arrays.push_back(array);
        }
    }

    // Preserve the legacy override: standard-family active doors, never lab lasers.
    if (isAS1()) {
        for (auto &door : request.doors) {
            if (door.light_state != 2 && (door.wall_type == 0 || door.wall_type == 2)) {
                door.door_state = 0;
                door.light_state = 1;
                if (doorsOpen) {
                    door.door_state = 1;
                    door.light_state = 0;
                }
            }
        }
    }

    // 0 layers means the project never overrode the core default.
    if (standardLayers != 0)
        request.as1_ceiling_layers.standard_total_layer_count = standardLayers;
    if (labLayers != 0)
        request.as1_ceiling_layers.lab_total_layer_count = labLayers;

    // write_map expects UTF-8 so non-ASCII (e.g. Chinese) paths survive.
    return auto_mapper::core::generate_map(request, path.toUtf8().toStdString());
}
} // namespace editor
