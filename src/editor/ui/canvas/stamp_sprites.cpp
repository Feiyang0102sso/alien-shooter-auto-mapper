#include "editor/ui/canvas/stamp_sprites.h"
#include "auto_mapper/core/decoration_builder/placeholder_vids.h"
#include <QDebug>
#include <QPainter>
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace editor {

std::vector<const auto_mapper::core::decoration_builder::DecorationMember *>
stampDrawOrder(const auto_mapper::core::decoration_builder::DecorationStamp &stamp) {
    using namespace auto_mapper::core::decoration_builder;
    std::vector<const DecorationMember *> members;
    for (const auto &member : stamp.members)
        members.push_back(&member);
    std::stable_sort(members.begin(), members.end(), [](const DecorationMember *left, const DecorationMember *right) {
        bool leftOnGround = lies_on_ground(left->vid);
        bool rightOnGround = lies_on_ground(right->vid);
        if (leftOnGround != rightOnGround)
            return leftOnGround;
        return left->offset_y < right->offset_y;
    });
    return members;
}

bool StampSprites::open(const QString &gameRoot) {
    cache_.clear();
    std::filesystem::path root(gameRoot.toStdU16String());
    // Decorations are AS2 Series only, so the folder is always read with the AS2 OBJ layout.
    bool opened = library_.open(root, auto_mapper::game_assets::GameFamily::AS2);
    qInfo() << "Stamp sprite preview game folder:" << gameRoot << "opened:" << opened;
    return opened;
}

bool StampSprites::isOpen() const {
    return library_.is_open();
}

const StampSprite *StampSprites::sprite(const auto_mapper::core::decoration_builder::DecorationMember &member) {
    if (!library_.is_open())
        return nullptr;
    Key key(member.vid, member.direction, member.gamma.r, member.gamma.g, member.gamma.b);
    auto cached = cache_.find(key);
    if (cached == cache_.end()) {
        std::optional<StampSprite> result;
        const auto_mapper::game_assets::VidImage *frame = library_.frame(member.vid, member.direction);
        if (frame != nullptr && frame->width > 0 && frame->height > 0) {
            // The library already applied the OBJ gamma; the map sprite gamma comes on top of it.
            auto_mapper::game_assets::VidImage tinted = *frame;
            auto_mapper::game_assets::ColorGamma spriteGamma;
            spriteGamma.red = member.gamma.r;
            spriteGamma.green = member.gamma.g;
            spriteGamma.blue = member.gamma.b;
            auto_mapper::game_assets::apply_gamma(tinted, spriteGamma);

            QImage image(tinted.rgba.data(), tinted.width, tinted.height, tinted.width * 4,
                         QImage::Format_RGBA8888);
            StampSprite sprite;
            // copy(): the QImage above only wraps the temporary pixel buffer.
            sprite.image = image.copy();
            sprite.topLeft = QPoint(tinted.left, tinted.top);
            result = sprite;
        }
        cached = cache_.emplace(key, result).first;
    }
    if (!cached->second)
        return nullptr;
    return &*cached->second;
}

QImage StampSprites::render(const auto_mapper::core::decoration_builder::DecorationStamp &stamp) {
    if (!library_.is_open())
        return {};
    // Same placement as the canvas: anchor lifted by posZ, frame scaled around the anchor.
    std::vector<std::pair<const StampSprite *, QRectF>> placed;
    QRectF bounds;
    for (const auto *member : stampDrawOrder(stamp)) {
        const StampSprite *frame = sprite(*member);
        if (frame == nullptr)
            continue;
        QPointF anchor(member->offset_x, member->offset_y - member->pos_z);
        QPointF topLeft = anchor + QPointF(frame->topLeft.x() * member->scale.x, frame->topLeft.y() * member->scale.y);
        QSizeF size(frame->image.width() * member->scale.x, frame->image.height() * member->scale.y);
        QRectF target(topLeft, size);
        placed.push_back({frame, target});
        bounds = bounds.united(target);
    }
    if (placed.empty())
        return {};

    QImage image(int(std::ceil(bounds.width())), int(std::ceil(bounds.height())), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.translate(-bounds.topLeft());
    for (const auto &[frame, target] : placed)
        painter.drawImage(target, frame->image);
    painter.end();
    qDebug() << "Rendered stamp preview" << QString::fromStdString(stamp.id) << image.size() << "from" << placed.size()
             << "sprites";
    return image;
}

} // namespace editor
