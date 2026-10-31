#include "editor/catalog/catalog.h"
#include "editor/i18n/text_keys.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <filesystem>
#include <stdexcept>

namespace editor {
namespace {
// Shelf card data, embedded as a Qt resource.
constexpr const char *CATALOG_RESOURCE = ":/catalog.json";
// Folder beside AutoMapper.exe that holds preview images.
constexpr const char *ASSET_FOLDER = "assets";
constexpr const char *PREVIEW_FOLDER = "preview";
// Library maps that become Decoration Stamps, relative to the asset folder: one folder per game family.
constexpr const char *DECORATION_MAP_FOLDER_AS1 = "decoration_maps/AS1";
constexpr const char *DECORATION_MAP_FOLDER_AS2 = "decoration_maps/AS2";
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

    QString as1Maps = assetDir_.filePath(DECORATION_MAP_FOLDER_AS1);
    QString as2Maps = assetDir_.filePath(DECORATION_MAP_FOLDER_AS2);
    decorationLibraryAS1_.load_directory(std::filesystem::path(as1Maps.toStdU16String()));
    decorationLibraryAS2_.load_directory(std::filesystem::path(as2Maps.toStdU16String()));
    qDebug() << "Catalog loaded:" << wallSets_.size() << "wall sets," << auto_mapper::core::SUPPORTED_WALL_TYPE_COUNT
             << "wall profiles, AS1" << decorationLibraryAS1_.series().size() << "stamp series /"
             << decorationLibraryAS1_.stamps().size() << "stamps, AS2" << decorationLibraryAS2_.series().size()
             << "stamp series /" << decorationLibraryAS2_.stamps().size() << "stamps";
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

QStringList Catalog::stampSeries(bool as1) const {
    QStringList result;
    for (const std::string &seriesId : decorationLibrary(as1).series())
        result.append(QString::fromStdString(seriesId));
    return result;
}

QStringList Catalog::seriesVariants(bool as1, const QString &seriesId) const {
    QStringList result;
    std::string series = seriesId.toStdString();
    for (const auto &stamp : decorationLibrary(as1).stamps()) {
        if (stamp.series_id == series)
            result.append(QString::fromStdString(stamp.id));
    }
    return result;
}

QString Catalog::seriesLabel(const QString &seriesId) const {
    return seriesId;
}

QString Catalog::seriesDescription(const QString &seriesId) const {
    QString key = QString("decoration.%1.description").arg(seriesId);
    // Checked first: a series without its own text is normal, not a missing-key warning.
    if (!locale.contains(key))
        return locale.tr(TextKey::LABEL_STAMP_SERIES_DESCRIPTION_DEFAULT);
    return locale.tr(key);
}

QString Catalog::stampLabel(const QString &stampId) const {
    // Stamp ids are "<series>:<number>" (AS2 Series region number, AS1 grid cell number).
    QString series = stampId.section(':', 0, 0);
    QString number = stampId.section(':', 1, 1);
    return locale.tr(TextKey::LABEL_STAMP_REGION, {{"series", series}, {"number", number}});
}

const auto_mapper::core::decoration_builder::DecorationStamp *Catalog::stamp(bool as1, const QString &stampId) const {
    return decorationLibrary(as1).find(stampId.toStdString());
}

const auto_mapper::core::decoration_builder::DecorationLibrary &Catalog::decorationLibrary(bool as1) const {
    if (as1)
        return decorationLibraryAS1_;
    return decorationLibraryAS2_;
}

QString Catalog::imagePath(const QString &relativePath) const {
    return assetDir_.filePath(relativePath);
}

QPolygonF Catalog::stampFrame(bool as1, const QString &stampId, const QPointF &origin) const {
    QPolygonF result;
    const auto *found = stamp(as1, stampId);
    if (found == nullptr)
        return result;
    // The frame is an axis-aligned Map Coordinate rectangle around its center.
    QRectF rect(origin.x() - found->width / 2.0, origin.y() - found->height / 2.0, found->width, found->height);
    result << rect.topLeft() << rect.topRight() << rect.bottomRight() << rect.bottomLeft();
    return result;
}

QSizeF Catalog::stampFrameSize(bool as1, const QString &stampId) const {
    return stampFrame(as1, stampId, {}).boundingRect().size();
}

} // namespace editor
