#include "editor/ui/canvas/stamp_sprites.h"
#include "auto_mapper/core/decoration_builder/placeholder_vids.h"
#include <QDebug>
#include <QPainter>
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace editor {
namespace {
using auto_mapper::core::decoration_builder::DecorationMember;

// A sprite at least this high rests on another one: stacked cabinets (z 27 / 54), a computer on a desk (z 32),
// blood on a corpse.
constexpr float RESTING_MIN_Z = 5.0f;
// A resting sprite's support is the nearest lower standing member within this Manhattan anchor distance.
// Objects on furniture sit up to ~25 map units behind their support's anchor in the library maps.
constexpr float SUPPORT_SEARCH_DISTANCE = 60.0f;

// Paint layer: 0 = flat on the ground, 1 = standing, 2 = flat but raised onto another sprite.
int drawLayer(const DecorationMember &member, auto_mapper::io::MapFormat format) {
    if (!auto_mapper::core::decoration_builder::lies_on_ground(format, member.vid))
        return 1;
    if (member.pos_z >= RESTING_MIN_Z)
        return 2;
    return 0;
}

/**
 * Painter depth of every member. Usually posY; a resting standing sprite takes its support's depth instead,
 * so it covers the support (even with its anchor a little behind it) without jumping in front of sprites
 * that stand between the support and the viewer. Neither posY nor posY + posZ alone gets both right.
 */
std::vector<float> paintDepths(const std::vector<const DecorationMember *> &members, auto_mapper::io::MapFormat format) {
    // Lower members first, so a support's depth is final before anything resting on it looks it up.
    std::vector<std::size_t> lowestFirst;
    for (std::size_t index = 0; index < members.size(); ++index)
        lowestFirst.push_back(index);
    std::stable_sort(lowestFirst.begin(), lowestFirst.end(), [&members](std::size_t left, std::size_t right) {
        return members[left]->pos_z < members[right]->pos_z;
    });

    std::vector<float> depths(members.size(), 0.0f);
    for (std::size_t index : lowestFirst) {
        const DecorationMember *member = members[index];
        depths[index] = member->offset_y;
        if (member->pos_z < RESTING_MIN_Z || drawLayer(*member, format) != 1)
            continue;

        int support = -1;
        float nearest = SUPPORT_SEARCH_DISTANCE;
        for (std::size_t other = 0; other < members.size(); ++other) {
            const DecorationMember *candidate = members[other];
            if (other == index || candidate->pos_z >= member->pos_z || drawLayer(*candidate, format) != 1)
                continue;
            float distance = std::abs(candidate->offset_x - member->offset_x) + std::abs(candidate->offset_y - member->offset_y);
            if (distance <= nearest) {
                nearest = distance;
                support = static_cast<int>(other);
            }
        }
        if (support >= 0)
            depths[index] = depths[support];
    }
    return depths;
}
} // namespace

std::vector<const auto_mapper::core::decoration_builder::DecorationMember *>
stampDrawOrder(const auto_mapper::core::decoration_builder::DecorationStamp &stamp, auto_mapper::io::MapFormat format) {
    using namespace auto_mapper::core::decoration_builder;
    std::vector<const DecorationMember *> members;
    for (const auto &member : stamp.members) {
        // Air walls are invisible in game: no sprite and no placeholder in any preview.
        if (is_invisible_block(format, member.vid))
            continue;
        members.push_back(&member);
    }

    std::vector<float> depths = paintDepths(members, format);
    std::vector<std::size_t> order;
    for (std::size_t index = 0; index < members.size(); ++index)
        order.push_back(index);
    std::stable_sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right) {
        int leftLayer = drawLayer(*members[left], format);
        int rightLayer = drawLayer(*members[right], format);
        if (leftLayer != rightLayer)
            return leftLayer < rightLayer;
        if (depths[left] != depths[right])
            return depths[left] < depths[right];
        // Same depth: a stack or a resting sprite and its support, drawn bottom-up.
        return members[left]->pos_z < members[right]->pos_z;
    });

    std::vector<const DecorationMember *> result;
    for (std::size_t index : order)
        result.push_back(members[index]);
    return result;
}

bool StampSprites::open(const QString &gameRoot, auto_mapper::game_assets::GameFamily family) {
    cache_.clear();
    std::filesystem::path root(gameRoot.toStdU16String());
    bool opened = library_.open(root, family);
    qInfo() << "Stamp sprite preview game folder:" << gameRoot << "AS1:"
             << (family == auto_mapper::game_assets::GameFamily::AS1) << "opened:" << opened;
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

QImage StampSprites::render(const auto_mapper::core::decoration_builder::DecorationStamp &stamp,
                            auto_mapper::io::MapFormat format) {
    if (!library_.is_open())
        return {};
    // Same placement as the canvas: anchor lifted by posZ, frame scaled around the anchor.
    std::vector<std::pair<const StampSprite *, QRectF>> placed;
    QRectF bounds;
    for (const auto *member : stampDrawOrder(stamp, format)) {
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
