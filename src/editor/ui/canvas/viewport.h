// Isometric editor canvas. Screen, Map and Grid conversions are explicit.
//
// Decoration interaction has three mutually exclusive states (see AGENTS.md):
//   placing  - a decoration tool is armed (isPlacing()); left click always drops a new one.
//   editing  - Select tool without an armed decoration (isEditing()); left click picks and drags.
//   off      - wall and eraser tools; decorations are only background.
#pragma once
#include "editor/catalog/catalog.h"
#include "editor/project/project.h"
#include "editor/ui/tools/drawing_modes.h"
#include <QHash>
#include <QPixmap>
#include <QWidget>
#include <optional>

namespace editor {

class Viewport : public QWidget {
    Q_OBJECT
  public:
    Viewport(Project &project, const Catalog &catalog, QWidget *parent = nullptr);
    DrawingMode mode = DrawingMode::Polyline;
    int wallType = 0;
    QString part = PART_WALL_BODY;
    QString activeStamp;
    QString activeArray;
    int selectedDecoration = -1;
    int eraserSize = 1;
    double zoom = 1;
    QPointF pan;

    bool isPlacing() const;
    bool isEditing() const;
    void setMode(DrawingMode value);
    void setWall(int type, const QString &partId = PART_WALL_BODY);
    void armStamp(const QString &id);
    void armArray(const QString &type);
    void resetInteraction();
    // Right click / Esc: drop the pending wall start, or leave placing mode.
    void cancel();
    void fitMap();
    bool deleteSelection();
    void setSpacing(double item, double row);
    QPointF gridToMap(const QPointF &grid, int type) const;
    QPointF mapToGrid(const QPointF &point, int type) const;
    QPointF mapToScreen(const QPointF &point) const;
    QPointF screenToMap(const QPointF &point) const;
    QPoint snapToGrid(const QPointF &screen) const;
    QPolygonF decorationFrame(const Decoration &decoration) const;
    bool outsideMap(const Decoration &decoration) const;

  signals:
    void projectChanged();
    void selectionChanged();
    void modeChanged();
    // Status-bar feedback, mirroring the legacy Python viewport signals.
    void cursorGridChanged(QPoint grid);
    void viewChanged(double zoom);
    void gridPointSelected(QPoint grid);
    void segmentStarted(QPoint grid);
    void segmentCreated(int x1, int y1, int x2, int y2, int count);
    void doorCreated(QPoint grid, int count);
    void decorationCreated(int count);
    void stampPlaced(const QString &stampId, bool outside);
    void decorationSelected();
    void decorationDeselected();
    void placingEnded();
    void drawingCancelled();

  protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void leaveEvent(QEvent *) override;
    void keyPressEvent(QKeyEvent *) override;

  private:
    Project &project_;
    const Catalog &catalog_;
    std::optional<QPoint> start_;
    std::optional<QPoint> selectedGridPoint_;
    std::optional<QPointF> arrayStart_;
    std::optional<QPointF> cursor_;
    bool panning_ = false;
    bool erasing_ = false;
    QPointF dragStart_;
    QPointF panStart_;
    // "move" or one of the array edges "left" / "right" / "top" / "bottom"; empty when idle.
    QString dragMode_;
    Decoration dragOriginal_;
    // Stamp canvas images by path; misses are cached as null pixmaps.
    QHash<QString, QPixmap> images_;

    QRectF bounds(int type) const;
    bool nearBounds(const QPointF &point) const;
    QPointF clampMap(const QPointF &point) const;
    QPoint constrainedEnd(QPoint point) const;
    std::vector<Segment> pendingSegments(QPoint point) const;
    void wallClick(const QPoint &point);
    DoorInstance makeDoor(QPoint first, QPoint last) const;
    std::pair<int, QString> hitDecoration(const QPointF &map) const;
    void dragDecoration(const QPointF &point);
    // Builder step vectors for an array's item and row axes, and their unit vectors.
    std::pair<QPointF, QPointF> arrayAxisSteps(const QString &type) const;
    std::pair<QPointF, QPointF> arrayAxes(const QString &type) const;
    QPointF axisDistances(const QPointF &delta, const QString &type) const;
    QPointF footprint(const QString &type) const;
    Decoration arrayFromPoints(const QPointF &first, const QPointF &last) const;
    QPolygonF toScreen(const QPolygonF &mapPolygon) const;
    void drawGrid(QPainter &painter);
    void drawDecoration(QPainter &painter, const Decoration &decoration, bool selected);
    void drawStampImage(QPainter &painter, const QString &stampId, const QPolygonF &screenPolygon, double opacity);
    void drawIncubatorUnits(QPainter &painter, const Decoration &decoration);
    void drawStampGhost(QPainter &painter, const QPointF &center);
    void drawArrayPreview(QPainter &painter, const Decoration &preview);
    void drawDoors(QPainter &painter);
    void drawLabels(QPainter &painter);
};

} // namespace editor
