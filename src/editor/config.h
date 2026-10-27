// AutoMapper.cfg access. The file keeps the legacy "key = value" format so old configs still work.
#pragma once
#include "game_assets/game_family.h"
#include <QString>

namespace editor {

using GameFamily = auto_mapper::game_assets::GameFamily;

// Decoration sprite opacity limits in percent; cfg values outside them are clamped.
inline constexpr int STAMP_SPRITE_OPACITY_MIN = 10;
inline constexpr int STAMP_SPRITE_OPACITY_MAX = 100;
inline constexpr int STAMP_SPRITE_OPACITY_DEFAULT = 100;

// Register the embedded styles, texts and catalog (the UI library is static). Safe to call repeatedly.
void initEditorResources();
// Path of AutoMapper.cfg: the working directory copy wins during development, else beside the exe.
QString configPath();
// Saved UI locale, or an empty string when the user has not chosen one yet (ask on startup).
QString loadLanguage();
// Persist the UI locale; throws std::runtime_error when the file cannot be written.
void saveLanguage(const QString &locale);
// Game folder of one family (cfg as1_path / as2_path), or an empty string when none is set.
// AS2 drives the decoration sprite preview; AS1 is only stored for now.
QString loadGamePath(GameFamily family);
// Persist one family's game folder; throws std::runtime_error when the file cannot be written.
void saveGamePath(GameFamily family, const QString &path);
// Green crosses over decoration sprites too (cfg 0/1, default 0). Savers throw like saveLanguage().
bool loadStampCrossesWithSprites();
void saveStampCrossesWithSprites(bool enabled);
// Decoration stamp frames on every stamp (cfg 0/1, default 0), not only while placing or selected.
bool loadStampAlwaysShowFrame();
void saveStampAlwaysShowFrame(bool enabled);
// Decoration sprite opacity in percent (cfg integer, default 100, clamped to the limits above).
int loadStampSpriteOpacity();
void saveStampSpriteOpacity(int percent);

} // namespace editor
