#include "editor/ui/canvas/viewport.h"
#include "auto_mapper/core/decoration_builder/placeholder_vids.h"
#include "auto_mapper/core/door_builder/door_profiles_as1.h"
#include "auto_mapper/core/indoor_decorations/array_layout.h"
#include "editor/i18n/text_keys.h"
#include "editor/ui/colors.h"
#include "editor/ui/tools/eraser/eraser_geometry.h"
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>
#include <limits>

namespace editor {
namespace {
constexpr double MIN_ZOOM = 0.002;
constexpr double MAX_ZOOM = 30.0;
constexpr double WHEEL_ZOOM_FACTOR = 1.15;
// Fitted maps leave 10% of the widget as margin.
constexpr double FIT_MARGIN = 0.90;
// Screen pixels within which an array edge counts as grabbed.
constexpr double EDGE_GRAB_PIXELS = 8;
constexpr double STAMP_PREVIEW_OPACITY = 1.0;
constexpr double STAMP_GHOST_PREVIEW_OPACITY = 0.55;
// Half the arm length of a stamp member placeholder cross, in Map Coordinate units (scales with zoom).
constexpr double STAMP_PLACEHOLDER_HALF_SIZE = 12.0;
// Door states shared with the core door builder.
constexpr int DOOR_STATE_OPEN = auto_mapper::core::DOOR_STATE_OPEN;
constexpr int LIGHT_STATE_BROKEN = auto_mapper::core::LIGHT_STATE_BROKEN;

double length(const QPointF &v) {
    return std::hypot(v.x(), v.y());
}

double segmentDistance(QPointF point, QPointF first, QPointF last) {
    QPointF delta = last - first;
    double squared = QPointF::dotProduct(delta, delta);
    if (squared == 0)
        return length(point - first);
    double t = std::clamp(QPointF::dotProduct(point - first, delta) / squared, 0.0, 1.0);
    return length(point - first - t * delta);
}

QPen cosmeticPen(const QColor &color, double width = 2, Qt::PenStyle style = Qt::SolidLine) {
    QPen pen(color, width, style);
    pen.setCosmetic(true);
    return pen;
}

struct DoorStyle {
    const char *line;
    const char *dot;
    bool hollow;
};

// Door marker colors, ported from the legacy MapViewport._get_door_colors().
DoorStyle doorStyle(const DoorInstance &door, bool globalDoorsOpen) {
    using namespace colors;
    bool isOpen = door.door_state == DOOR_STATE_OPEN;

    // 1. Lab Wall Set: laser doors ignore the global override.
    if (door.wall_type == 1) {
        if (door.light_state == LIGHT_STATE_BROKEN)
            return {DOOR_LAB_DECO_LINE, DOOR_LAB_DECO_DOT, false};
        if (!isOpen)
            return {DOOR_LAB_LASER_WITH_LINE, DOOR_LAB_LASER_WITH_DOT, false};
        return {DOOR_LAB_LASER_WITHOUT_LINE, DOOR_LAB_LASER_WITHOUT_DOT, true};
    }

    // 2. AS2 Wall Sets (wall_type >= 3).
    if (door.wall_type >= 3)
        return {DOOR_AS2_LINE, DOOR_AS2_DOT, isOpen || globalDoorsOpen};

    // 3. Standard Wall Sets: active doors follow door_state or the global override.
    if (door.light_state != LIGHT_STATE_BROKEN) {
        if (isOpen || globalDoorsOpen)
            return {DOOR_STD_ACTIVE_LINE, DOOR_STD_ACTIVE_DOT_OPEN, true};
        return {DOOR_STD_ACTIVE_LINE, DOOR_STD_ACTIVE_DOT_CLOSED, false};
    }
    // Dead doors: z offset 0 is closed; a raised panel is jammed while closed, else open.
    if (door.z_offset == 0.0f)
        return {DOOR_STD_DEAD_CLOSED_LINE, DOOR_STD_DEAD_CLOSED_DOT, false};
    if (!isOpen)
        return {DOOR_STD_DEAD_JAMMED_LINE, DOOR_STD_DEAD_JAMMED_DOT, false};
    return {DOOR_STD_DEAD_OPEN_LINE, DOOR_STD_DEAD_OPEN_DOT, true};
}
} // namespace

Viewport::Viewport(Project &project, const Catalog &catalog, QWidget *parent)
    : QWidget(parent), project_(project), catalog_(catalog) {
    setObjectName("mapViewport");
    setMinimumSize(720, 540);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::CrossCursor);
}

bool Viewport::isPlacing() const {
    return !activeStamp.isEmpty() || !activeArray.isEmpty();
}

bool Viewport::isEditing() const {
    return mode == DrawingMode::Select && !isPlacing();
}

void Viewport::resetInteraction() {
    start_.reset();
    selectedGridPoint_.reset();
    arrayStart_.reset();
    activeStamp.clear();
    activeArray.clear();
    // Leaving a decoration tool must restore the wall body, or wall tools draw nothing.
    part = PART_WALL_BODY;
    selectedDecoration = -1;
    dragMode_.clear();
    erasing_ = false;
    emit selectionChanged();
    update();
}

void Viewport::setMode(DrawingMode value) {
    resetInteraction();
    mode = value;
    emit modeChanged();
}

void Viewport::setWall(int type, const QString &partId) {
    resetInteraction();
    wallType = type;
    part = partId;
    if (mode == DrawingMode::Select)
        mode = DrawingMode::Polyline;
    emit modeChanged();
}

void Viewport::armStamp(const QString &id) {
    resetInteraction();
    activeStamp = id;
    part.clear();
    mode = DrawingMode::Select;
    emit modeChanged();
}

void Viewport::armArray(const QString &type) {
    resetInteraction();
    activeArray = type;
    part.clear();
    mode = DrawingMode::Rectangle;
    emit modeChanged();
}

void Viewport::cancel() {
    if (arrayStart_) {
        arrayStart_.reset();
    } else if (isPlacing()) {
        setMode(DrawingMode::Select);
        qInfo() << "Decoration placing ended, select tool active";
        emit placingEnded();
    } else if (start_) {
        start_.reset();
        emit drawingCancelled();
    }
    update();
}

void Viewport::fitMap() {
    double zoomX = width() * FIT_MARGIN / project_.width;
    double zoomY = height() * FIT_MARGIN / project_.height;
    zoom = std::clamp(std::min(zoomX, zoomY), MIN_ZOOM, MAX_ZOOM);
    pan = {(width() - project_.width * zoom) / 2, (height() - project_.height * zoom) / 2};
    emit viewChanged(zoom);
    update();
}

QPointF Viewport::gridToMap(const QPointF &grid, int type) const {
    const auto profile = catalog_.wallProfile(type);
    double stepX = profile.step_x / profile.grid_divisor;
    // nearbyint preserves the old Python round-to-even rule at half-grid centers.
    double shiftX = std::nearbyint((project_.width / 2 - stepX / 2) / stepX) * stepX + stepX / 2;
    double shiftY = profile.step_y / (2.0 * profile.grid_divisor) + profile.step_y;
    return {(grid.x() - grid.y()) * profile.step_x + shiftX, (grid.x() + grid.y()) * profile.step_y + shiftY};
}

QPointF Viewport::mapToGrid(const QPointF &point, int type) const {
    auto origin = gridToMap({}, type);
    const auto profile = catalog_.wallProfile(type);
    double a = (point.x() - origin.x()) / profile.step_x;
    double b = (point.y() - origin.y()) / profile.step_y;
    return {(a + b) / 2, (b - a) / 2};
}

QPointF Viewport::mapToScreen(const QPointF &point) const {
    return point * zoom + pan;
}

QPointF Viewport::screenToMap(const QPointF &point) const {
    return (point - pan) / zoom;
}

QPolygonF Viewport::toScreen(const QPolygonF &mapPolygon) const {
    QPolygonF result;
    for (const QPointF &point : mapPolygon)
        result << mapToScreen(point);
    return result;
}

QRectF Viewport::bounds(int type) const {
    const auto profile = catalog_.wallProfile(type);
    double x = profile.step_x / 2;
    double y = profile.step_y / 2;
    double right = std::max(x, std::trunc((project_.width - x) / profile.step_x) * profile.step_x + x);
    double bottom = std::max(y, std::trunc((project_.height - y) / profile.step_y) * profile.step_y + y);
    return QRectF(QPointF(x, y), QPointF(right, bottom));
}

bool Viewport::nearBounds(const QPointF &point) const {
    const auto p = catalog_.wallProfile(wallType);
    return bounds(wallType).adjusted(-p.step_x, -p.step_y, p.step_x, p.step_y).contains(point);
}

QPointF Viewport::clampMap(const QPointF &point) const {
    auto area = bounds(wallType);
    return {std::clamp(point.x(), area.left(), area.right()), std::clamp(point.y(), area.top(), area.bottom())};
}

QPoint Viewport::snapToGrid(const QPointF &screen) const {
    QPointF physical = screenToMap(screen);
    QPointF logical = mapToGrid(clampMap(physical), wallType);
    QPoint base(static_cast<int>(std::nearbyint(logical.x())), static_cast<int>(std::nearbyint(logical.y())));
    // Pick the nearest in-bounds vertex around the rounded guess.
    QPoint best = base;
    double bestDistance = std::numeric_limits<double>::infinity();
    for (int x = -4; x <= 4; ++x) {
        for (int y = -4; y <= 4; ++y) {
            QPoint candidate = base + QPoint(x, y);
            auto map = gridToMap(candidate, wallType);
            if (!nearBounds(map))
                continue;
            double distance = length(map - physical);
            if (distance < bestDistance) {
                best = candidate;
                bestDistance = distance;
            }
        }
    }
    return best;
}

QPoint Viewport::constrainedEnd(QPoint point) const {
    if (!start_)
        return point;
    // Walls follow one grid axis: keep the dominant direction.
    if (std::abs(point.x() - start_->x()) > std::abs(point.y() - start_->y()))
        point.setY(start_->y());
    else
        point.setX(start_->x());
    // Pull the end back inside the map boundary one vertex at a time.
    while (point != *start_ && !nearBounds(gridToMap(point, wallType))) {
        if (point.x() > start_->x())
            point.rx()--;
        else if (point.x() < start_->x())
            point.rx()++;
        else if (point.y() > start_->y())
            point.ry()--;
        else
            point.ry()++;
    }
    return point;
}

std::vector<Segment> Viewport::pendingSegments(QPoint point) const {
    std::vector<Segment> result;
    if (!start_)
        return result;
    auto first = *start_;
    if (mode == DrawingMode::Rectangle && part == PART_WALL_BODY) {
        if (first.x() == point.x() || first.y() == point.y())
            return result;
        std::vector<QPoint> corners{first, {point.x(), first.y()}, point, {first.x(), point.y()}, first};
        for (int i = 0; i < 4; ++i) {
            bool firstInside = nearBounds(gridToMap(corners[i], wallType));
            bool nextInside = nearBounds(gridToMap(corners[i + 1], wallType));
            if (firstInside && nextInside)
                result.push_back({{corners[i].x(), corners[i].y()}, {corners[i + 1].x(), corners[i + 1].y()}, wallType});
        }
    } else {
        point = constrainedEnd(point);
        if (first != point)
            result.push_back({{first.x(), first.y()}, {point.x(), point.y()}, wallType});
    }
    return result;
}

void Viewport::wallClick(const QPoint &point) {
    selectedGridPoint_ = point;
    emit gridPointSelected(point);
    if (!start_) {
        start_ = point;
        emit segmentStarted(point);
        return;
    }
    auto segments = pendingSegments(point);
    if (segments.empty())
        return;
    QPoint end = constrainedEnd(point);
    if (part == PART_WALL_BODY) {
        for (const auto &segment : segments) {
            project_.segments.push_back(segment);
            int count = static_cast<int>(project_.segments.size());
            qInfo().nospace() << "Wall segment #" << count << ": (" << segment.start.x << ", " << segment.start.y
                              << ") -> (" << segment.end.x << ", " << segment.end.y << ")";
            emit segmentCreated(segment.start.x, segment.start.y, segment.end.x, segment.end.y, count);
        }
        // Polyline keeps chaining from the last end point; Line and Rect start over.
        if (mode == DrawingMode::Polyline)
            start_ = end;
        else
            start_.reset();
    } else {
        DoorInstance door = makeDoor(*start_, end);
        project_.doors.push_back(door);
        start_.reset();
        int count = static_cast<int>(project_.doors.size());
        qInfo().nospace() << "Door #" << count << ": (" << door.pos.x << ", " << door.pos.y << ")";
        emit doorCreated(QPoint(door.pos.x, door.pos.y), count);
    }
    emit projectChanged();
}

DoorInstance Viewport::makeDoor(QPoint first, QPoint last) const {
    DoorInstance door{};
    door.wall_type = wallType;
    door.light_state = LIGHT_STATE_BROKEN;
    int size = 1;
    // Door API positions address edges, whereas drawing clicks address vertices.
    if (first.x() == last.x()) {
        door.pos.x = first.x();
        door.pos.y = std::min(first.y(), last.y()) + 1;
        size = std::abs(last.y() - first.y());
    } else {
        door.direction_type = 1;
        door.pos.x = std::min(first.x(), last.x()) + 1;
        door.pos.y = first.y();
        size = std::abs(last.x() - first.x());
    }
    // Standard doors come in a small and a large span.
    int maximum = auto_mapper::core::DOOR_STANDARD.large.span_steps;
    door.size = std::clamp(size, auto_mapper::core::DOOR_STANDARD.small.span_steps, maximum);
    // AS2 Wall Sets 2..9 only ship the full-size door; lab laser doors are single-width.
    if (wallType >= 6)
        door.size = maximum;
    if (part.startsWith("lab_"))
        door.size = 1;
    if (part == "active_door") {
        door.door_state = DOOR_STATE_OPEN;
        door.light_state = 0;
    }
    if (part == "dead_door_jammed")
        door.z_offset = auto_mapper::core::get_random_standard_jam_z_offset(door.size);
    if (part == "dead_door_open") {
        door.z_offset = auto_mapper::core::get_standard_door_variant(door.size).dead_open_z_offset.min_z;
        door.door_state = DOOR_STATE_OPEN;
    }
    if (part == "lab_laser_closed" || part == "as2_door_closed")
        door.light_state = 1;
    if (part == "lab_laser_open" || part == "as2_door_open") {
        door.light_state = 1;
        door.door_state = DOOR_STATE_OPEN;
    }
    return door;
}

std::pair<QPointF, QPointF> Viewport::arrayAxisSteps(const QString &type) const {
    using namespace auto_mapper::core::indoor_decorations;
    if (type == DECORATION_TYPE_DESK_ARRAY) {
        QPointF row(DESK_DEFAULT_ROW_SPACING_X, DESK_DEFAULT_ROW_SPACING_Y);
        QPointF column(DESK_DEFAULT_COLUMN_SPACING_X, DESK_DEFAULT_COLUMN_SPACING_Y);
        return {row, column};
    }
    QPointF row(INCUBATOR_DEFAULT_ITEM_STEP_X, INCUBATOR_DEFAULT_ITEM_STEP_Y);
    QPointF column(INCUBATOR_DEFAULT_ROW_STEP_X, INCUBATOR_DEFAULT_ROW_STEP_Y);
    return {row, column};
}

std::pair<QPointF, QPointF> Viewport::arrayAxes(const QString &type) const {
    // Unit vectors along the builder's item and row steps.
    auto [row, column] = arrayAxisSteps(type);
    return {row / length(row), column / length(column)};
}

QPointF Viewport::footprint(const QString &type) const {
    using namespace auto_mapper::core::indoor_decorations;
    if (type == DECORATION_TYPE_DESK_ARRAY)
        return {DESK_FOOTPRINT_ROW_LENGTH, DESK_FOOTPRINT_COLUMN_LENGTH};
    return {INCUBATOR_FOOTPRINT_ITEM_LENGTH, INCUBATOR_FOOTPRINT_ROW_LENGTH};
}

QPointF Viewport::axisDistances(const QPointF &delta, const QString &type) const {
    // Solve delta = rowDistance * row + columnDistance * column.
    auto [row, column] = arrayAxes(type);
    double determinant = row.x() * column.y() - row.y() * column.x();
    return {(delta.x() * column.y() - delta.y() * column.x()) / determinant,
            (row.x() * delta.y() - row.y() * delta.x()) / determinant};
}

QPolygonF Viewport::decorationFrame(const Decoration &d) const {
    if (d.type == DECORATION_TYPE_ROOM_STAMP)
        return catalog_.stampFrame(project_.isAS1(), d.profileId, d.origin);
    auto [row, column] = arrayAxes(d.type);
    QPolygonF polygon;
    polygon << d.origin << d.origin + row * d.rowLength << d.origin + row * d.rowLength + column * d.columnLength
            << d.origin + column * d.columnLength;
    return polygon;
}

bool Viewport::outsideMap(const Decoration &decoration) const {
    QRectF area(0, 0, project_.width, project_.height);
    for (auto point : decorationFrame(decoration)) {
        if (!area.contains(point))
            return true;
    }
    return false;
}

Decoration Viewport::arrayFromPoints(const QPointF &first, const QPointF &last) const {
    Decoration result;
    result.type = activeArray;
    result.origin = first;
    auto delta = axisDistances(last - first, activeArray);
    auto [row, column] = arrayAxes(activeArray);
    // Dragging "backwards" moves the origin so lengths stay positive.
    if (delta.x() < 0)
        result.origin += row * delta.x();
    if (delta.y() < 0)
        result.origin += column * delta.y();
    auto minimum = footprint(activeArray);
    result.rowLength = std::max(std::abs(delta.x()), minimum.x());
    result.columnLength = std::max(std::abs(delta.y()), minimum.y());
    return result;
}

std::pair<int, QString> Viewport::hitDecoration(const QPointF &map) const {
    // Walk from the top-most (last drawn) decoration down.
    double grab = EDGE_GRAB_PIXELS / zoom;
    for (int i = static_cast<int>(project_.decorations.size()) - 1; i >= 0; --i) {
        const auto &decoration = project_.decorations[i];
        auto polygon = decorationFrame(decoration);
        if (polygon.size() != 4)
            continue;
        // Stamps only move; arrays can also be resized by their edges.
        if (decoration.type != DECORATION_TYPE_ROOM_STAMP) {
            if (segmentDistance(map, polygon[0], polygon[3]) <= grab)
                return {i, "left"};
            if (segmentDistance(map, polygon[1], polygon[2]) <= grab)
                return {i, "right"};
            if (segmentDistance(map, polygon[0], polygon[1]) <= grab)
                return {i, "top"};
            if (segmentDistance(map, polygon[3], polygon[2]) <= grab)
                return {i, "bottom"};
        }
        if (polygon.containsPoint(map, Qt::OddEvenFill))
            return {i, "move"};
    }
    return {-1, {}};
}

void Viewport::dragDecoration(const QPointF &point) {
    if (selectedDecoration < 0)
        return;
    auto &d = project_.decorations[selectedDecoration];
    d = dragOriginal_;
    QPointF delta = point - dragStart_;
    if (dragMode_ == "move") {
        d.origin += delta;
    } else {
        auto distances = axisDistances(delta, d.type);
        auto [row, column] = arrayAxes(d.type);
        auto minimum = footprint(d.type);
        if (dragMode_ == "left" && d.rowLength - distances.x() >= minimum.x()) {
            d.origin += row * distances.x();
            d.rowLength -= distances.x();
        }
        if (dragMode_ == "right")
            d.rowLength = std::max(minimum.x(), d.rowLength + distances.x());
        if (dragMode_ == "top" && d.columnLength - distances.y() >= minimum.y()) {
            d.origin += column * distances.y();
            d.columnLength -= distances.y();
        }
        if (dragMode_ == "bottom")
            d.columnLength = std::max(minimum.y(), d.columnLength + distances.y());
    }
    emit projectChanged();
    emit selectionChanged();
}

bool Viewport::deleteSelection() {
    if (!isEditing() || selectedDecoration < 0)
        return false;
    project_.decorations.erase(project_.decorations.begin() + selectedDecoration);
    selectedDecoration = -1;
    dragMode_.clear();
    qInfo() << "Decoration deleted";
    emit projectChanged();
    emit selectionChanged();
    update();
    return true;
}

void Viewport::setSpacing(double item, double row) {
    if (!isEditing() || selectedDecoration < 0)
        return;
    auto &decoration = project_.decorations[selectedDecoration];
    if (decoration.type == DECORATION_TYPE_ROOM_STAMP)
        return;
    decoration.itemSpacing = item;
    decoration.rowSpacing = row;
    emit projectChanged();
    update();
}

void Viewport::mousePressEvent(QMouseEvent *event) {
    setFocus();
    cursor_ = event->position();
    if (event->button() == Qt::MiddleButton) {
        panning_ = true;
        dragStart_ = event->position();
        panStart_ = pan;
        setCursor(Qt::ClosedHandCursor);
    } else if (event->button() == Qt::RightButton) {
        cancel();
    } else if (event->button() == Qt::LeftButton) {
        auto physical = screenToMap(event->position());
        if (isPlacing()) {
            if (!activeStamp.isEmpty()) {
                // Placing stays armed for repeat drops, so the fresh stamp is not selected:
                // the orange frame must never share the screen with the ghost frame.
                Decoration decoration;
                decoration.profileId = activeStamp;
                decoration.origin = physical;
                project_.decorations.push_back(decoration);
                bool outside = outsideMap(decoration);
                qInfo() << "Stamp placed:" << activeStamp << "outside map:" << outside;
                emit projectChanged();
                emit decorationCreated(static_cast<int>(project_.decorations.size()));
                emit stampPlaced(activeStamp, outside);
            } else if (!arrayStart_) {
                arrayStart_ = clampMap(physical);
            } else {
                project_.decorations.push_back(arrayFromPoints(*arrayStart_, clampMap(physical)));
                arrayStart_.reset();
                emit projectChanged();
                emit decorationCreated(static_cast<int>(project_.decorations.size()));
            }
        } else if (isEditing()) {
            auto hit = hitDecoration(physical);
            bool hadSelection = selectedDecoration >= 0;
            selectedDecoration = hit.first;
            dragMode_ = hit.second;
            if (selectedDecoration >= 0) {
                dragStart_ = physical;
                dragOriginal_ = project_.decorations[selectedDecoration];
                emit decorationSelected();
            } else if (hadSelection) {
                emit decorationDeselected();
            }
            emit selectionChanged();
        } else if (mode == DrawingMode::Eraser) {
            erasing_ = true;
            if (eraseAround(project_.segments, project_.doors, snapToGrid(event->position()), eraserSize, wallType))
                emit projectChanged();
        } else {
            wallClick(snapToGrid(event->position()));
        }
    }
    update();
    event->accept();
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
    cursor_ = event->position();
    if (panning_) {
        pan = panStart_ + event->position() - dragStart_;
    } else if (!dragMode_.isEmpty()) {
        dragDecoration(screenToMap(event->position()));
    } else if (erasing_) {
        if (eraseAround(project_.segments, project_.doors, snapToGrid(event->position()), eraserSize, wallType))
            emit projectChanged();
    } else if (isEditing()) {
        // Hint what a press would do: move the body or resize an array edge.
        auto hit = hitDecoration(screenToMap(event->position()));
        if (hit.second == "move")
            setCursor(Qt::SizeAllCursor);
        else if (!hit.second.isEmpty())
            setCursor(Qt::SizeFDiagCursor);
        else
            setCursor(Qt::CrossCursor);
    }
    emit cursorGridChanged(snapToGrid(event->position()));
    update();
    event->accept();
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        panning_ = false;
        setCursor(Qt::CrossCursor);
    }
    if (event->button() == Qt::LeftButton) {
        erasing_ = false;
        dragMode_.clear();
    }
    update();
    event->accept();
}

void Viewport::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() == 0)
        return;
    // Zoom around the cursor: the Map Coordinate under it stays fixed.
    auto before = screenToMap(event->position());
    double factor = WHEEL_ZOOM_FACTOR;
    if (event->angleDelta().y() < 0)
        factor = 1 / factor;
    zoom = std::clamp(zoom * factor, MIN_ZOOM, MAX_ZOOM);
    pan = event->position() - before * zoom;
    emit viewChanged(zoom);
    update();
    event->accept();
}

void Viewport::leaveEvent(QEvent *event) {
    cursor_.reset();
    update();
    QWidget::leaveEvent(event);
}

void Viewport::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        cancel();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void Viewport::drawGrid(QPainter &painter) {
    const auto profile = catalog_.wallProfile(wallType);
    // Level of detail: skip grid lines as adjacent lines get closer on screen.
    double pixels = zoom * std::hypot(profile.step_x, profile.step_y);
    int stride = 0;
    if (pixels >= 12)
        stride = 1;
    else if (pixels >= 6)
        stride = 2;
    else if (pixels >= 3)
        stride = 5;
    else if (pixels >= 1.5)
        stride = 10;
    else if (pixels >= 0.5)
        stride = 50;
    auto area = bounds(wallType);
    if (stride > 0) {
        // Grid range covered by the map ...
        QList<QPointF> mapCorners{area.topLeft(), area.topRight(), area.bottomLeft(), area.bottomRight()};
        double low = std::numeric_limits<double>::infinity();
        double high = -low;
        for (auto corner : mapCorners) {
            auto grid = mapToGrid(corner, wallType);
            low = std::min({low, grid.x(), grid.y()});
            high = std::max({high, grid.x(), grid.y()});
        }
        int first = std::floor(low) - 1;
        int last = std::ceil(high) + 1;
        // ... intersected with the range visible on screen (one extra line avoids edge gaps).
        double minX = std::numeric_limits<double>::infinity();
        double minY = minX;
        double maxX = -minX;
        double maxY = -minX;
        QList<QPointF> screenCorners{{0, 0}, {double(width()), 0}, {0, double(height())}, {double(width()), double(height())}};
        for (auto corner : screenCorners) {
            auto grid = mapToGrid(screenToMap(corner), wallType);
            minX = std::min(minX, grid.x());
            maxX = std::max(maxX, grid.x());
            minY = std::min(minY, grid.y());
            maxY = std::max(maxY, grid.y());
        }
        int x1 = std::max(first, int(std::floor(minX)) - 1);
        int x2 = std::min(last, int(std::ceil(maxX)) + 1);
        int y1 = std::max(first, int(std::floor(minY)) - 1);
        int y2 = std::min(last, int(std::ceil(maxY)) + 1);
        // The legacy viewport keeps the initial standard-wall grid color.
        painter.setPen(cosmeticPen(colors::wallColor(0), 1));
        int row = int(std::ceil(double(y1) / stride)) * stride;
        for (; row <= y2; row += stride)
            painter.drawLine(mapToScreen(gridToMap({double(x1), double(row)}, wallType)),
                             mapToScreen(gridToMap({double(x2), double(row)}, wallType)));
        int column = int(std::ceil(double(x1) / stride)) * stride;
        for (; column <= x2; column += stride)
            painter.drawLine(mapToScreen(gridToMap({double(column), double(y1)}, wallType)),
                             mapToScreen(gridToMap({double(column), double(y2)}, wallType)));
    }
    painter.setPen(cosmeticPen(QColor(colors::CANVAS_BOUNDARY), 2));
    painter.setBrush(Qt::NoBrush);
    auto topLeft = mapToScreen(area.topLeft());
    auto bottomRight = mapToScreen(area.bottomRight());
    // PySide's four-argument drawRect selects the integer overload.
    painter.drawRect(int(topLeft.x()), int(topLeft.y()), int(bottomRight.x() - topLeft.x()),
                     int(bottomRight.y() - topLeft.y()));
}

void Viewport::drawStampMembers(QPainter &painter, const QString &stampId, const QPointF &center, double opacity) {
    using namespace auto_mapper::core::decoration_builder;
    const auto *stamp = catalog_.stamp(project_.isAS1(), stampId);
    if (stamp == nullptr)
        return;

    auto_mapper::io::MapFormat format = project_.mapFormat();
    std::vector<const DecorationMember *> members = stampDrawOrder(*stamp, format);

    // Screen rectangles of the decoded sprites, in paint order.
    std::vector<std::pair<QRectF, const QImage *>> spriteTargets;
    QRectF spriteBounds;
    // Members that still need a cross; crosses go on top of every sprite so none is hidden.
    std::vector<QPointF> crossAnchors;
    for (const DecorationMember *member : members) {
        // (posX, posY) is the coordinate written to the map.
        QPointF anchor = center + QPointF(member->offset_x, member->offset_y);
        const StampSprite *sprite = stampSprites().sprite(*member);
        if (sprite != nullptr) {
            // The engine lifts a sprite by posZ and scales its frame around the anchor.
            QPointF lifted = anchor - QPointF(0, member->pos_z);
            QPointF topLeft = lifted + QPointF(sprite->topLeft.x() * member->scale.x, sprite->topLeft.y() * member->scale.y);
            QSizeF size(sprite->image.width() * member->scale.x, sprite->image.height() * member->scale.y);
            QRectF target(mapToScreen(topLeft), size * zoom);
            if (target.intersects(rect())) {
                spriteTargets.push_back({target, &sprite->image});
                spriteBounds = spriteBounds.united(target);
            }
        }
        if (!draws_placeholder(format, member->vid))
            continue;
        if (sprite == nullptr || stampCrossesWithSprites)
            crossAnchors.push_back(anchor);
    }

    painter.save();
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    double spriteOpacity = opacity * stampSpriteOpacity / 100.0;
    if (spriteOpacity >= 1.0) {
        for (const auto &[target, image] : spriteTargets)
            painter.drawImage(target, *image);
    } else if (!spriteTargets.empty()) {
        // Translucent stamps (placing ghost, opacity slider) are composed opaque first and faded as one layer;
        // fading each sprite alone lets lower stacked sprites show through the ones in front of them.
        QRect layerRect = spriteBounds.toAlignedRect().intersected(rect());
        qreal pixelRatio = devicePixelRatioF();
        QImage layer(layerRect.size() * pixelRatio, QImage::Format_ARGB32_Premultiplied);
        layer.setDevicePixelRatio(pixelRatio);
        layer.fill(Qt::transparent);
        QPainter layerPainter(&layer);
        layerPainter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        layerPainter.translate(-layerRect.topLeft());
        for (const auto &[target, image] : spriteTargets)
            layerPainter.drawImage(target, *image);
        layerPainter.end();
        painter.setOpacity(spriteOpacity);
        painter.drawImage(layerRect.topLeft(), layer);
    }

    painter.setOpacity(opacity);
    painter.setPen(cosmeticPen(QColor(colors::DECORATION_STAMP_PLACEHOLDER), 1.5));
    QPointF arm(STAMP_PLACEHOLDER_HALF_SIZE, STAMP_PLACEHOLDER_HALF_SIZE);
    QPointF mirroredArm(STAMP_PLACEHOLDER_HALF_SIZE, -STAMP_PLACEHOLDER_HALF_SIZE);
    for (const QPointF &anchor : crossAnchors) {
        painter.drawLine(mapToScreen(anchor - arm), mapToScreen(anchor + arm));
        painter.drawLine(mapToScreen(anchor - mirroredArm), mapToScreen(anchor + mirroredArm));
    }
    painter.restore();
}

QImage Viewport::renderStampPreview(const QString &stampId) {
    const auto *stamp = catalog_.stamp(project_.isAS1(), stampId);
    if (stamp == nullptr)
        return {};
    return stampSprites().render(*stamp, project_.mapFormat());
}

StampSprites &Viewport::stampSprites() {
    if (project_.isAS1())
        return stampSpritesAS1_;
    return stampSpritesAS2_;
}

bool Viewport::hasGameFolder() const {
    if (project_.isAS1())
        return stampSpritesAS1_.isOpen();
    return stampSpritesAS2_.isOpen();
}

bool Viewport::openGameFolder(GameFamily family, const QString &gameRoot) {
    bool opened = false;
    if (family == GameFamily::AS1)
        opened = stampSpritesAS1_.open(gameRoot, family);
    else
        opened = stampSpritesAS2_.open(gameRoot, family);
    update();
    return opened;
}

void Viewport::drawIncubatorUnits(QPainter &painter, const Decoration &decoration) {
    using namespace auto_mapper::core::indoor_decorations;
    auto [rowStep, columnStep] = arrayAxisSteps(decoration.type);
    auto [row, column] = arrayAxes(decoration.type);
    QPointF size = footprint(decoration.type);
    ArrayLayoutAxis rowAxis{static_cast<float>(rowStep.x()), static_cast<float>(rowStep.y()),
                            static_cast<float>(decoration.itemSpacing), static_cast<float>(size.x())};
    ArrayLayoutAxis columnAxis{static_cast<float>(columnStep.x()), static_cast<float>(columnStep.y()),
                               static_cast<float>(decoration.rowSpacing), static_cast<float>(size.y())};
    ArrayLayout layout{static_cast<float>(decoration.origin.x()),
                       static_cast<float>(decoration.origin.y()),
                       static_cast<float>(decoration.rowLength),
                       static_cast<float>(decoration.columnLength),
                       rowAxis,
                       columnAxis};
    painter.setPen(cosmeticPen(QColor(colors::INCUBATOR_PREVIEW_BORDER), 1));
    painter.setBrush(colors::withAlpha(colors::INCUBATOR_PREVIEW_FILL, colors::INCUBATOR_PREVIEW_FILL_ALPHA));
    for (auto anchor : calculate_array_item_anchors(layout)) {
        // The builder returns centers; footprint origins are half an axis back.
        QPointF origin = QPointF(anchor.pos_x, anchor.pos_y) - row * size.x() / 2 - column * size.y() / 2;
        QPolygonF unit;
        unit << origin << origin + row * size.x() << origin + row * size.x() + column * size.y()
             << origin + column * size.y();
        QPolygonF screenUnit = toScreen(unit);
        if (screenUnit.boundingRect().intersects(rect()))
            painter.drawPolygon(screenUnit);
    }
}

void Viewport::drawDecoration(QPainter &painter, const Decoration &decoration, bool selected) {
    auto frame = decorationFrame(decoration);
    if (frame.size() != 4)
        return;
    QPolygonF polygon = toScreen(frame);
    if (!polygon.boundingRect().intersects(rect()))
        return;
    bool outside = false;
    if (decoration.type == DECORATION_TYPE_ROOM_STAMP)
        outside = outsideMap(decoration);
    QColor border(colors::DECORATION_AREA_BORDER);
    if (selected)
        border = QColor(colors::DECORATION_AREA_SELECTED_BORDER);
    // The map bound warning wins over the selection highlight.
    if (outside)
        border = QColor(colors::DECORATION_STAMP_OUT_OF_BOUNDS);
    if (decoration.type != DECORATION_TYPE_ROOM_STAMP) {
        painter.setPen(cosmeticPen(border));
        painter.setBrush(colors::withAlpha(colors::DECORATION_AREA_FILL, colors::DECORATION_AREA_FILL_ALPHA));
        painter.drawPolygon(polygon);
    } else {
        // Stamp frames clutter the sprites, so by default they only appear while they carry information:
        // selected (orange, also while dragging) or outside the map (red).
        bool showFrame = stampAlwaysShowFrame || selected || outside;
        if (showFrame) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(colors::withAlpha(colors::DECORATION_AREA_FILL, colors::DECORATION_AREA_FILL_ALPHA));
            painter.drawPolygon(polygon);
        }
        drawStampMembers(painter, decoration.profileId, decoration.origin, STAMP_PREVIEW_OPACITY);
        // The border goes over the sprites so they cannot hide the frame the select tool acts on.
        if (showFrame) {
            painter.setPen(cosmeticPen(border));
            painter.setBrush(Qt::NoBrush);
            painter.drawPolygon(polygon);
        }
    }
    if (decoration.type == DECORATION_TYPE_INCUBATOR_ARRAY)
        drawIncubatorUnits(painter, decoration);
}

void Viewport::drawStampGhost(QPainter &painter, const QPointF &center) {
    QPolygonF frame = catalog_.stampFrame(project_.isAS1(), activeStamp, center);
    if (frame.size() != 4)
        return;
    QPolygonF polygon = toScreen(frame);
    Decoration ghost;
    ghost.profileId = activeStamp;
    ghost.origin = center;
    QColor border(colors::DECORATION_STAMP_GHOST_BORDER);
    if (outsideMap(ghost))
        border = QColor(colors::DECORATION_STAMP_OUT_OF_BOUNDS);
    drawStampMembers(painter, activeStamp, center, STAMP_GHOST_PREVIEW_OPACITY);
    painter.setPen(cosmeticPen(border, 2, Qt::DashLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(polygon);
}

void Viewport::drawArrayPreview(QPainter &painter, const Decoration &preview) {
    painter.setPen(cosmeticPen(QColor(colors::DECORATION_AREA_PREVIEW_BORDER), 2, Qt::DashLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(toScreen(decorationFrame(preview)));
}

void Viewport::drawDoors(QPainter &painter) {
    // The Global Door State Override only exists in AS1 projects.
    bool globalDoorsOpen = project_.isAS1() && project_.doorsOpen;
    for (const auto &door : project_.doors) {
        auto ends = doorGridEndpoints(door);
        auto first = mapToScreen(gridToMap(ends.first, door.wall_type));
        auto last = mapToScreen(gridToMap(ends.second, door.wall_type));
        DoorStyle style = doorStyle(door, globalDoorsOpen);
        QColor line(style.line);
        painter.setPen(cosmeticPen(line, 5));
        painter.drawLine(first, last);
        // Status dot: filled while closed, hollow (canvas colored) while open.
        painter.setPen(cosmeticPen(line, 2));
        if (style.hollow)
            painter.setBrush(QColor(colors::CANVAS_BACKGROUND));
        else
            painter.setBrush(QColor(style.dot));
        painter.drawEllipse((first + last) / 2, 5, 5);
    }
}

void Viewport::drawLabels(QPainter &painter) {
    const Locale &locale = catalog_.locale;
    painter.setPen(QColor(colors::CANVAS_LABEL_TEXT));
    QString hint = locale.tr(TextKey::CANVAS_HELP);
    if (isPlacing())
        hint = locale.tr(TextKey::CANVAS_HELP_PLACING);
    if (isEditing())
        hint = locale.tr(TextKey::CANVAS_HELP_SELECTING);

    // Grid limits cover the map and anything drawn beyond it.
    auto profile = catalog_.wallProfile(wallType);
    int columns = std::max(1, int(std::ceil(project_.width / profile.step_x)));
    int rows = std::max(1, int(std::ceil(project_.height / profile.step_y)));
    for (const auto &segment : project_.segments) {
        columns = std::max({columns, segment.start.x, segment.end.x});
        rows = std::max({rows, segment.start.y, segment.end.y});
    }
    for (const auto &door : project_.doors) {
        auto ends = doorGridEndpoints(door);
        columns = std::max({columns, ends.first.x(), ends.second.x()});
        rows = std::max({rows, ends.first.y(), ends.second.y()});
    }
    QString info = locale.tr(TextKey::CANVAS_INFO, {{"map_size_x", QString::number(project_.width, 'f', 1)},
                                                    {"map_size_y", QString::number(project_.height, 'f', 1)},
                                                    {"wall_label", catalog_.wallLabel(wallType)},
                                                    {"grid_columns", QString::number(columns)},
                                                    {"grid_rows", QString::number(rows)}});
    painter.drawText(24, 32, locale.tr(TextKey::CANVAS_TITLE));
    painter.drawText(24, 54, info);
    painter.drawText(24, 76, hint);
}

void Viewport::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(colors::CANVAS_BACKGROUND));
    drawGrid(painter);
    for (int i = 0; i < static_cast<int>(project_.decorations.size()); ++i) {
        // Orange means "the select tool acts on this one", so only edit mode highlights.
        bool selected = isEditing() && selectedDecoration == i;
        drawDecoration(painter, project_.decorations[i], selected);
    }
    for (const auto &segment : project_.segments) {
        painter.setPen(cosmeticPen(colors::wallColor(segment.wall_type), 4));
        QPointF start(segment.start.x, segment.start.y);
        QPointF end(segment.end.x, segment.end.y);
        painter.drawLine(mapToScreen(gridToMap(start, segment.wall_type)), mapToScreen(gridToMap(end, segment.wall_type)));
    }
    drawDoors(painter);
    if (cursor_) {
        if (!activeStamp.isEmpty()) {
            drawStampGhost(painter, screenToMap(*cursor_));
        } else if (arrayStart_) {
            drawArrayPreview(painter, arrayFromPoints(*arrayStart_, clampMap(screenToMap(*cursor_))));
        } else if (mode == DrawingMode::Eraser) {
            auto p = catalog_.wallProfile(wallType);
            double radius = (eraserSize - 0.5) * std::hypot(p.step_x, p.step_y) * zoom;
            painter.setPen(cosmeticPen(QColor(colors::ERASER_PREVIEW_COLOR), 2, Qt::DashLine));
            painter.setBrush(colors::withAlpha(colors::ERASER_PREVIEW_COLOR, colors::ERASER_PREVIEW_ALPHA));
            painter.drawEllipse(*cursor_, radius, radius);
        } else if (!isEditing() && !isPlacing()) {
            painter.setPen(cosmeticPen(QColor(colors::CANVAS_PREVIEW_SEGMENT), 3, Qt::DashLine));
            painter.setBrush(Qt::NoBrush);
            for (const auto &segment : pendingSegments(snapToGrid(*cursor_))) {
                QPointF start(segment.start.x, segment.start.y);
                QPointF end(segment.end.x, segment.end.y);
                painter.drawLine(mapToScreen(gridToMap(start, wallType)), mapToScreen(gridToMap(end, wallType)));
            }
        }
    }
    if (selectedGridPoint_) {
        // The marker takes the active wall set's color, like the legacy theme_color.
        painter.setPen(cosmeticPen(QColor(colors::CANVAS_MARKER_OUTLINE), 2));
        painter.setBrush(colors::wallColor(wallType));
        painter.drawEllipse(mapToScreen(gridToMap(*selectedGridPoint_, wallType)), 6, 6);
    }
    drawLabels(painter);
}

} // namespace editor
