#include "editor/catalog/catalog.h"
#include "auto_mapper/core/decoration_builder/decoration_registry.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <stdexcept>

namespace editor {
namespace {
// Shelf card data, embedded as a Qt resource.
constexpr const char *CATALOG_RESOURCE = ":/catalog.json";
// Folder beside AutoMapper.exe that holds preview images.
constexpr const char *ASSET_FOLDER = "assets";
constexpr const char *PREVIEW_FOLDER = "preview";
// Keys of catalog.json "shelf_order": one wall shelf per game family.
constexpr const char *SHELF_ORDER_AS1 = "AS1";
constexpr const char *SHELF_ORDER_AS2 = "AS2";

QJsonObject readObject(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        throw std::runtime_error(("Missing resource: " + path).toStdString());
    return QJsonDocument::fromJson(file.readAll()).object();
}

QList<int> readIntList(const QJsonArray &array) {
    QList<int> result;
    for (const auto &value : array)
        result.append(value.toInt());
    return result;
}
} // namespace

Catalog::Catalog(const Locale &locale) : locale(locale) {
    // The build and the package both copy assets/ next to AutoMapper.exe.
    assetDir_ = QDir(QCoreApplication::applicationDirPath() + "/" + ASSET_FOLDER);
    if (!assetDir_.exists(PREVIEW_FOLDER))
        throw std::runtime_error(("Missing asset folder: " + assetDir_.filePath(PREVIEW_FOLDER)).toStdString());

    QJsonObject data = readObject(CATALOG_RESOURCE);
    QJsonObject order = data.value("shelf_order").toObject();
    wallSetOrder_[true] = readIntList(order.value(SHELF_ORDER_AS1).toArray());
    wallSetOrder_[false] = readIntList(order.value(SHELF_ORDER_AS2).toArray());

    for (const auto &wallValue : data.value("walls").toArray()) {
        QJsonObject wallObject = wallValue.toObject();
        WallSet wall;
        wall.id = wallObject.value("id").toInt();
        wall.preview = wallObject.value("preview").toString();
        for (const auto &partValue : wallObject.value("parts").toArray()) {
            QJsonObject partObject = partValue.toObject();
            WallPart part;
            part.wallType = partObject.value("wall_type").toInt();
            part.id = partObject.value("id").toString();
            part.labelKey = partObject.value("label_key").toString();
            part.preview = partObject.value("preview").toString();
            part.nvidText = partObject.value("nvid_text").toString();
            wall.parts.append(part);
        }
        wallSets_.append(wall);
    }

    for (const auto &seriesValue : data.value("series").toArray())
        series_.append(seriesValue.toString());
    for (const auto &stampValue : data.value("stamps").toArray()) {
        QJsonObject stampObject = stampValue.toObject();
        QString id = stampObject.value("id").toString();
        stampSeries_[id] = stampObject.value("series").toString();
        stampOrder_.append(id);
    }

    for (const auto *profile : auto_mapper::core::decoration_builder::as2_decoration_profiles()) {
        QString id = QString::fromUtf8(profile->id.data(), static_cast<qsizetype>(profile->id.size()));
        stamps[id] = profile;
    }
    qDebug() << "Catalog loaded:" << wallSets_.size() << "wall sets," << series_.size() << "stamp series,"
             << auto_mapper::core::SUPPORTED_WALL_TYPE_COUNT << "wall profiles," << stamps.size() << "stamp profiles";
}

QList<int> Catalog::wallTypes() const {
    QList<int> result;
    for (int wallType : auto_mapper::core::SUPPORTED_WALL_TYPES)
        result.append(wallType);
    return result;
}

const auto_mapper::core::WallProfile &Catalog::wallProfile(int wallType) const {
    return auto_mapper::core::WallBuilder::get_wall_profile(wallType);
}

QList<int> Catalog::wallSetOrder(bool as1) const {
    return wallSetOrder_.value(as1);
}

WallSet Catalog::wallSet(int id) const {
    for (const auto &wall : wallSets_) {
        if (wall.id == id)
            return wall;
    }
    qWarning() << "Unknown wall set:" << id;
    return {};
}

int Catalog::wallSetOf(int wallType) const {
    // A card owns every wall type its wall-body parts switch to.
    for (bool as1 : {true, false}) {
        for (int setId : wallSetOrder_.value(as1)) {
            for (const auto &part : wallSet(setId).parts) {
                if (part.id == PART_WALL_BODY && part.wallType == wallType)
                    return setId;
            }
        }
    }
    return wallType;
}

QString Catalog::wallLabel(int id) const {
    return locale.tr(QString("wall.%1.label").arg(id));
}

QString Catalog::wallShortLabel(int id) const {
    return locale.tr(QString("wall.%1.short_label").arg(id));
}

QString Catalog::wallDescription(int id) const {
    return locale.tr(QString("wall.%1.description").arg(id));
}

QString Catalog::partLabel(const WallPart &part) const {
    return locale.tr(part.labelKey);
}

QStringList Catalog::stampSeries() const {
    return series_;
}

QStringList Catalog::seriesVariants(const QString &seriesId) const {
    QStringList result;
    for (const auto &id : stampOrder_) {
        if (stampSeries_.value(id) == seriesId)
            result.append(id);
    }
    return result;
}

QString Catalog::seriesLabel(const QString &seriesId) const {
    return locale.tr("decoration.stamp.series." + seriesId + ".label");
}

QString Catalog::seriesDescription(const QString &seriesId) const {
    return locale.tr("decoration.stamp.series." + seriesId + ".description");
}

QString Catalog::stampLabel(const QString &stampId) const {
    return locale.tr("decoration.stamp." + stampId + ".label");
}

QString Catalog::stampSeriesOf(const QString &stampId) const {
    return stampSeries_.value(stampId);
}

QString Catalog::imagePath(const QString &relativePath) const {
    return assetDir_.filePath(relativePath);
}

QString Catalog::seriesImage(const QString &seriesId) const {
    return imagePath(QString(PREVIEW_FOLDER) + "/AS2/decoration/" + seriesId + "/" + seriesId + "00_thumbnail.webp");
}

QString Catalog::stampImage(const QString &stampId, bool thumbnail) const {
    QString suffix = ".webp";
    if (thumbnail)
        suffix = "_thumbnail.webp";
    QString series = stampSeriesOf(stampId);
    return imagePath(QString(PREVIEW_FOLDER) + "/AS2/decoration/" + series + "/" + stampId + suffix);
}

QPolygonF Catalog::stampFrame(const QString &stampId, const QPointF &origin) const {
    QPolygonF result;
    if (!stamps.contains(stampId))
        return result;
    // Corner order: top, left, bottom, right, relative to the stamp center.
    for (const auto &corner : stamps.value(stampId)->frame.corner_offsets)
        result.append(origin + QPointF(corner.x, corner.y));
    return result;
}

QSizeF Catalog::stampFrameSize(const QString &stampId) const {
    return stampFrame(stampId, {}).boundingRect().size();
}

} // namespace editor
