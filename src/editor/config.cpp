#include "editor/config.h"
#include "editor/i18n/locale.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStringList>
#include <algorithm>
#include <stdexcept>

// Q_INIT_RESOURCE must be expanded outside any namespace.
static void registerEditorResources() {
    Q_INIT_RESOURCE(editor_assets);
}

namespace editor {
namespace {
constexpr const char *CONFIG_FILE_NAME = "AutoMapper.cfg";
constexpr const char *LANGUAGE_CONFIG_KEY = "language";
// Game folders (objects.res + Vid/). AS2 drives the decoration sprite preview; AS1 is stored for later use.
constexpr const char *AS1_GAME_PATH_CONFIG_KEY = "as1_path";
constexpr const char *AS2_GAME_PATH_CONFIG_KEY = "as2_path";
// 1 = keep drawing green crosses over decoration sprites; 0 (default) = crosses only without a sprite.
constexpr const char *STAMP_CROSSES_WITH_SPRITES_CONFIG_KEY = "stamp_crosses_with_sprites";
// 1 = always draw decoration stamp frames; 0 (default) = only while placing, selected or out of the map.
constexpr const char *STAMP_ALWAYS_SHOW_FRAME_CONFIG_KEY = "stamp_always_show_frame";
// Decoration sprite opacity in percent; values outside the limits in config.h are clamped.
constexpr const char *STAMP_SPRITE_OPACITY_CONFIG_KEY = "stamp_sprite_opacity";
// Stored language values; 0 (or a missing line) means "ask on startup".
constexpr int LANGUAGE_EN_US = 1;
constexpr int LANGUAGE_ZH_CN = 2;

// Header written when the file is created from scratch, same as the Python version.
constexpr const char *CONFIG_HEADER = "# Auto Mapper configuration\n"
                                      "# language: 0 = ask on startup, 1 = English, 2 = Chinese\n";

QString lineKey(const QString &line) {
    return line.section('=', 0, 0).trimmed();
}

// Raw value of one "key = value" line, or an empty string when the key or the file is missing.
QString loadConfigValue(const char *key) {
    QFile file(configPath());
    if (!file.open(QIODevice::ReadOnly))
        return {};
    const QStringList lines = QString::fromUtf8(file.readAll()).split('\n');
    for (const QString &line : lines) {
        if (line.trimmed().startsWith('#'))
            continue;
        if (lineKey(line) == key)
            return line.section('=', 1).trimmed();
    }
    return {};
}

// Replace or append one "key = value" line, keeping every other line and comment.
void saveConfigValue(const char *key, const QString &value) {
    QString valueLine = QString("%1 = %2").arg(key, value);

    QString path = configPath();
    QStringList lines;
    QFile source(path);
    if (source.open(QIODevice::ReadOnly)) {
        lines = QString::fromUtf8(source.readAll()).split('\n');
        source.close();
    } else {
        lines = QString(CONFIG_HEADER).split('\n');
    }
    // Drop trailing blank lines so repeated saves do not grow the file.
    while (!lines.isEmpty() && lines.last().trimmed().isEmpty())
        lines.removeLast();

    bool replaced = false;
    for (QString &line : lines) {
        if (lineKey(line) == key) {
            line = valueLine;
            replaced = true;
        }
    }
    if (!replaced)
        lines.append(valueLine);

    QSaveFile target(path);
    if (!target.open(QIODevice::WriteOnly))
        throw std::runtime_error(target.errorString().toStdString());
    QByteArray bytes = (lines.join('\n') + '\n').toUtf8();
    if (target.write(bytes) != bytes.size() || !target.commit())
        throw std::runtime_error(target.errorString().toStdString());
}
} // namespace

void initEditorResources() {
    registerEditorResources();
}

QString configPath() {
    // Prefer the original working-directory config during development.
    QString local = QDir::current().filePath(CONFIG_FILE_NAME);
    if (QFileInfo::exists(local))
        return local;
    return QDir(QCoreApplication::applicationDirPath()).filePath(CONFIG_FILE_NAME);
}

QString loadLanguage() {
    int value = loadConfigValue(LANGUAGE_CONFIG_KEY).toInt();
    if (value == LANGUAGE_EN_US)
        return LOCALE_EN_US;
    if (value == LANGUAGE_ZH_CN)
        return LOCALE_ZH_CN;
    return {};
}

void saveLanguage(const QString &locale) {
    int value = LANGUAGE_EN_US;
    if (locale == LOCALE_ZH_CN)
        value = LANGUAGE_ZH_CN;
    saveConfigValue(LANGUAGE_CONFIG_KEY, QString::number(value));
    qInfo() << "Saved UI locale preference:" << locale << "to" << configPath();
}

// cfg key of one game family's folder.
const char *gamePathKey(GameFamily family) {
    if (family == GameFamily::AS1)
        return AS1_GAME_PATH_CONFIG_KEY;
    return AS2_GAME_PATH_CONFIG_KEY;
}

QString loadGamePath(GameFamily family) {
    return loadConfigValue(gamePathKey(family));
}

bool loadStampCrossesWithSprites() {
    return loadConfigValue(STAMP_CROSSES_WITH_SPRITES_CONFIG_KEY).toInt() == 1;
}

void saveStampCrossesWithSprites(bool enabled) {
    saveConfigValue(STAMP_CROSSES_WITH_SPRITES_CONFIG_KEY, QString::number(int(enabled)));
    qInfo() << "Saved stamp crosses with sprites:" << enabled;
}

bool loadStampAlwaysShowFrame() {
    return loadConfigValue(STAMP_ALWAYS_SHOW_FRAME_CONFIG_KEY).toInt() == 1;
}

void saveStampAlwaysShowFrame(bool enabled) {
    saveConfigValue(STAMP_ALWAYS_SHOW_FRAME_CONFIG_KEY, QString::number(int(enabled)));
    qInfo() << "Saved stamp always show frame:" << enabled;
}

int loadStampSpriteOpacity() {
    bool ok = false;
    int value = loadConfigValue(STAMP_SPRITE_OPACITY_CONFIG_KEY).toInt(&ok);
    if (!ok)
        return STAMP_SPRITE_OPACITY_DEFAULT;
    return std::clamp(value, STAMP_SPRITE_OPACITY_MIN, STAMP_SPRITE_OPACITY_MAX);
}

void saveStampSpriteOpacity(int percent) {
    int value = std::clamp(percent, STAMP_SPRITE_OPACITY_MIN, STAMP_SPRITE_OPACITY_MAX);
    saveConfigValue(STAMP_SPRITE_OPACITY_CONFIG_KEY, QString::number(value));
    qInfo() << "Saved stamp sprite opacity:" << value;
}

void saveGamePath(GameFamily family, const QString &path) {
    saveConfigValue(gamePathKey(family), QDir::fromNativeSeparators(path));
    qInfo() << "Saved game folder" << gamePathKey(family) << path << "to" << configPath();
}

} // namespace editor
