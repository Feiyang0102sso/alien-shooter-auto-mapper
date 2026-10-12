// Editor catalog: wall sets, drawable parts and decoration stamps shown on the shelves.
// Geometry comes from the core profiles; resources/catalog.json only holds language-neutral
// editor metadata, and every visible label is looked up through Locale by id.
#pragma once
#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_builder.h"
#include "editor/i18n/locale.h"
#include <QDir>
#include <QHash>
#include <QList>
#include <QPolygonF>
#include <QString>

namespace editor {

inline constexpr const char *PART_WALL_BODY = "wall_body";

// One selectable item inside a Wall Set card (a wall variant or a door choice).
struct WallPart {
    int wallType = 0;
    QString id;
    QString labelKey;
    QString preview;
    QString nvidText;
};

// One Wall Set card. `id` is the wall type the card represents on the shelf.
struct WallSet {
    int id = 0;
    QString preview;
    QList<WallPart> parts;
};

class Catalog {
  public:
    explicit Catalog(const Locale &locale);

    const Locale &locale;
    // Core decoration stamp profiles by stamp id (owned by the core registry).
    QHash<QString, const auto_mapper::core::decoration_builder::DecorationProfile *> stamps;

    // Every wall type the core supports, and its geometry profile (step, offsets, grid divisor).
    QList<int> wallTypes() const;
    const auto_mapper::core::WallProfile &wallProfile(int wallType) const;

    // Wall Set ids in shelf order for the AS1 or AS2 family.
    QList<int> wallSetOrder(bool as1) const;
    WallSet wallSet(int id) const;
    // Wall Set card that owns a wall type (AS2 Set 1 variants 3..5 share one card).
    int wallSetOf(int wallType) const;
    QString wallLabel(int id) const;
    QString wallShortLabel(int id) const;
    QString wallDescription(int id) const;
    QString partLabel(const WallPart &part) const;

    // Decoration Stamp Series ids in shelf order.
    QStringList stampSeries() const;
    // Stamp ids belonging to one series, in authored order; the first one is the default.
    QStringList seriesVariants(const QString &seriesId) const;
    QString seriesLabel(const QString &seriesId) const;
    QString seriesDescription(const QString &seriesId) const;
    QString stampLabel(const QString &stampId) const;
    QString stampSeriesOf(const QString &stampId) const;

    // Preview images live beside the exe in assets/, not inside Qt resources.
    QString imagePath(const QString &relativePath) const;
    QString seriesImage(const QString &seriesId) const;
    QString stampImage(const QString &stampId, bool thumbnail = false) const;
    // Decoration Stamp Frame in Map Coordinates for one stamp centered at `origin`.
    QPolygonF stampFrame(const QString &stampId, const QPointF &origin) const;
    QSizeF stampFrameSize(const QString &stampId) const;

  private:
    QDir assetDir_;
    QHash<bool, QList<int>> wallSetOrder_;
    QList<WallSet> wallSets_;
    QStringList series_;
    // Stamp id -> series id, plus the authored stamp order.
    QHash<QString, QString> stampSeries_;
    QStringList stampOrder_;
};

} // namespace editor
