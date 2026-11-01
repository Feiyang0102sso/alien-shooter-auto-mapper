// Isometric editor canvas. Screen, Map and Grid conversions are explicit.
//
// Decoration interaction has three mutually exclusive states (see AGENTS.md):
//   placing  - a decoration tool is armed (isPlacing()); left click always drops a new one.
//   editing  - Select tool without an armed decoration (isEditing()); left click picks and drags.
//   off      - wall and eraser tools; decorations are only background.
//
// Curve Walls: while the Curve Wall card is active (isCurveWallCard()), Freehand draws a stroke in Map
// Coordinates and Eraser removes whole curves; the grid tools are hidden.
//
// Loot Boxes: while a Loot Box card is active (isLootCard()), Freehand is the brush (each dab calls the core
// scatter) and Eraser removes that card's boxes; Select and the grid tools are hidden.
// Gore works the same way (isGoreCard()): corpses with blood around them, or bare blood pools.
// Both brushes show the next dab as a ghost under the cursor: every dab runs from scatterSeed_, so the ghost is
// exactly what a click places. Right click rolls a new seed; every placed dab rolls one too.
#pragma once
#include "editor/catalog/catalog.h"
#include "editor/config.h"
#include "editor/project/project.h"
#include "editor/ui/canvas/stamp_sprites.h"
#include "editor/ui/tools/drawing_modes.h"
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
    // Curve Wall type the Freehand tool draws, or -1 while a grid wall card is active.
    // Unlike `part`, tool switches keep it, so Eraser and Freehand can alternate on the same card.
    int curveWallType = -1;
    // Loot Box card the brush and eraser act on, empty off the Loot shelf. Like curveWallType, tool switches keep it.
    std::optional<LootBoxKind> lootKind;
    // Brush parameters of the active Loot Box card; MainWindow keeps one copy per card.
    auto_mapper::core::LootBrushSettings lootBrush;
    // Gore card and its brush, kept like lootKind / lootBrush; at most one of lootKind and goreKind is set.
    std::optional<GoreKind> goreKind;
    auto_mapper::core::GoreBrushSettings goreBrush;
    int selectedDecoration = -1;
    int eraserSize = 1;
    double zoom = 1;
    QPointF pan;
    // Sprite preview options from AutoMapper.cfg (see config.h).
    bool stampCrossesWithSprites = false;
    bool stampAlwaysShowFrame = false;
    // Percent, already clamped to the config.h limits; the placing ghost multiplies its own opacity on top.
    int stampSpriteOpacity = 100;

    bool isPlacing() const;
    bool isEditing() const;
    bool isCurveWallCard() const;
    bool isLootCard() const;
    bool isGoreCard() const;
    // A Loot Box or Gore card: Freehand is a scatter brush and Eraser removes that card's sprites.
    bool isBrushCard() const;
    void setMode(DrawingMode value);
    void setWall(int type, const QString &partId = PART_WALL_BODY);
    void armStamp(const QString &id);
    void armArray(const QString &type);
    void armCurveWall(int type);
    void armLoot(LootBoxKind kind);
    void armGore(GoreKind kind);
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
    // Game folder of one family for the stamp sprite preview; false keeps the green crosses.
    bool openGameFolder(GameFamily family, const QString &gameRoot);
    // True when the current project's family has a game folder open.
    bool hasGameFolder() const;
    // Whole stamp drawn from game sprites for the inspector; null without a game folder.
    QImage renderStampPreview(const QString &stampId);
    // A small sample pile (or egg patch) of one Loot Box card for its shelf card and the inspector; null without
    // a game folder.
    QImage renderLootPreview(LootBoxKind kind);
    // A corpse with a little blood (or a small pool) of one Gore card; null without a game folder.
    QImage renderGorePreview(GoreKind kind);

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
    void curveWallCreated(int count, int pieceCount);
    // A brush dab or an eraser stroke changed the loot boxes; `changed` is how many were added or removed.
    void lootBoxesPainted(int changed, int total);
    void lootBoxesErased(int changed, int total);
    // The same for corpses and blood; `changed` counts sprites.
    void gorePainted(int changed, int total);
    void goreErased(int changed, int total);
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
    // Freehand stroke in Map Coordinates while the left button is held; empty otherwise.
    std::vector<QPointF> stroke_;
    bool panning_ = false;
    bool erasing_ = false;
    // Loot brush: held left button, and the Map Coordinate of the last dab.
    bool brushing_ = false;
    QPointF lastDab_;
    // Seed of the next brush dab (see the header comment).
    uint32_t scatterSeed_ = 0;
    QPointF dragStart_;
    QPointF panStart_;
    // "move" or one of the array edges "left" / "right" / "top" / "bottom"; empty when idle.
    QString dragMode_;
    Decoration dragOriginal_;
    // One sprite cache per game family: AS1 and AS2 stamps read different game folders.
    StampSprites stampSpritesAS1_;
    StampSprites stampSpritesAS2_;
    // Sprite cache of the current project's family.
    StampSprites &stampSprites();

    QRectF bounds(int type) const;
    bool nearBounds(const QPointF &point) const;
    QPointF clampMap(const QPointF &point) const;
    // Eraser reach in Map Coordinates; the dashed preview circle has the same radius.
    double eraserRadius() const;
    // Stroke length in screen pixels at the current zoom.
    double strokeScreenLength(const std::vector<QPointF> &stroke) const;
    // True when releasing now would close the stroke into a loop (its end is back near its start).
    bool strokeCloses(const std::vector<QPointF> &stroke) const;
    void finishStroke();
    // One loot brush dab at a Map Coordinate; boxes landing outside the map are dropped.
    void lootDab(const QPointF &center);
    void eraseLoot(const QPointF &center);
    // One gore brush dab, and the gore eraser; same rules as the loot ones.
    void goreDab(const QPointF &center);
    void eraseGore(const QPointF &center);
    // Radius of the active brush card's circle (Map Coordinates).
    double brushRadius() const;
    // The dab a click at `center` would place now: same seed, so preview and placement agree.
    // Items landing outside the map are already dropped.
    std::vector<LootBox> rollLoot(const QPointF &center) const;
    std::vector<GoreItem> rollGore(const QPointF &center) const;
    // Right click on a brush card, and after every placed dab: the next dab gets a different layout.
    void rerollScatter();
    // Ghost of the next dab plus the green dashed brush circle.
    void drawScatterGhost(QPainter &painter, const QPointF &cursor);
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
    // Stamp members placed around `center` (Map Coordinates): game sprites when a game folder is open,
    // green crosses for members without a decoded frame (or for every member with stampCrossesWithSprites).
    void drawStampMembers(QPainter &painter, const QString &stampId, const QPointF &center, double opacity);
    // Shared by stamps and loot boxes: members around `center`, sprites in stampDrawOrder() then crosses.
    void drawMembers(QPainter &painter, const auto_mapper::core::decoration_builder::DecorationStamp &stamp,
                     const QPointF &center, double opacity);
    // Loot boxes as members of one stamp at the origin, so stacking and opacity follow the stamp rules.
    void drawLootItems(QPainter &painter, const std::vector<LootBox> &boxes, double opacity);
    // Corpse and blood sprites as members of one stamp; without a decoded frame blood gets a dark red dot and
    // bone piles a cross, since the placeholder rules (placeholder_vids) hide both.
    void drawGoreItems(QPainter &painter, const std::vector<GoreItem> &items, double opacity);
    void drawIncubatorUnits(QPainter &painter, const Decoration &decoration);
    void drawStampGhost(QPainter &painter, const QPointF &center);
    void drawArrayPreview(QPainter &painter, const Decoration &preview);
    void drawDoors(QPainter &painter);
    // Curve Wall lines and piece dots; like grid walls, never game sprites.
    void drawCurveWalls(QPainter &painter);
    void drawStroke(QPainter &painter);
    void drawLabels(QPainter &painter);
};

} // namespace editor
