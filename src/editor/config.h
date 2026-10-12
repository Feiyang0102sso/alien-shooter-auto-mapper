// AutoMapper.cfg access. The file keeps the legacy "key = value" format so old configs still work.
#pragma once
#include <QString>

namespace editor {

// Register the embedded styles, texts and catalog (the UI library is static). Safe to call repeatedly.
void initEditorResources();
// Path of AutoMapper.cfg: the working directory copy wins during development, else beside the exe.
QString configPath();
// Saved UI locale, or an empty string when the user has not chosen one yet (ask on startup).
QString loadLanguage();
// Persist the UI locale; throws std::runtime_error when the file cannot be written.
void saveLanguage(const QString &locale);

} // namespace editor
