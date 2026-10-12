#include "editor/ui/colors.h"

namespace editor::colors {

QColor wallColor(int wallType) {
    // Wall type -> accent color, as in the legacy WALL_PROFILE_UI_METADATA table.
    switch (wallType) {
    case 0:
        return QColor(WALL_PROFILE_STANDARD);
    case 1:
        return QColor(WALL_PROFILE_LAB);
    case 2:
        return QColor(WALL_PROFILE_STANDARD_DARK);
    case 3:
        return QColor(WALL_PROFILE_AS2_SET1_V1);
    case 4:
        return QColor(WALL_PROFILE_AS2_SET1_V2);
    case 5:
        return QColor(WALL_PROFILE_AS2_SET1_V3);
    case 6:
        return QColor(WALL_PROFILE_AS2_SET2);
    case 7:
        return QColor(WALL_PROFILE_AS2_SET3);
    case 8:
        return QColor(WALL_PROFILE_AS2_SET4);
    case 9:
        return QColor(WALL_PROFILE_AS2_SET5);
    case 10:
        return QColor(WALL_PROFILE_AS2_SET6);
    case 11:
        return QColor(WALL_PROFILE_AS2_SET7);
    case 12:
        return QColor(WALL_PROFILE_AS2_SET8);
    case 13:
        return QColor(WALL_PROFILE_AS2_SET9);
    default:
        return QColor(WALL_PROFILE_FALLBACK);
    }
}

} // namespace editor::colors
