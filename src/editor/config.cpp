#include "editor/config.h"
#include "editor/i18n/locale.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStringList>
#include <stdexcept>

// Q_INIT_RESOURCE must be expanded outside any namespace.
static void registerEditorResources() {
    Q_INIT_RESOURCE(editor_assets);
}

namespace editor {
namespace {
constexpr const char *CONFIG_FILE_NAME = "AutoMapper.cfg";
constexpr const char *LANGUAGE_CONFIG_KEY = "language";
// Stored language values; 0 (or a missing line) means "ask on startup".
constexpr int LANGUAGE_EN_US = 1;
constexpr int LANGUAGE_ZH_CN = 2;

// Header written when the file is created from scratch, same as the Python version.
constexpr const char *CONFIG_HEADER = "# Auto Mapper configuration\n"
                                      "# language: 0 = ask on startup, 1 = English, 2 = Chinese\n";

QString lineKey(const QString &line) {
    return line.section('=', 0, 0).trimmed();
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
    QFile file(configPath());
    if (!file.open(QIODevice::ReadOnly))
        return {};
    const QStringList lines = QString::fromUtf8(file.readAll()).split('\n');
    for (const QString &line : lines) {
        if (line.trimmed().startsWith('#'))
            continue;
        if (lineKey(line) != LANGUAGE_CONFIG_KEY)
            continue;
        int value = line.section('=', 1).trimmed().toInt();
        if (value == LANGUAGE_EN_US)
            return LOCALE_EN_US;
        if (value == LANGUAGE_ZH_CN)
            return LOCALE_ZH_CN;
        return {};
    }
    return {};
}

void saveLanguage(const QString &locale) {
    int value = LANGUAGE_EN_US;
    if (locale == LOCALE_ZH_CN)
        value = LANGUAGE_ZH_CN;
    QString languageLine = QString("%1 = %2").arg(LANGUAGE_CONFIG_KEY).arg(value);

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
        if (lineKey(line) == LANGUAGE_CONFIG_KEY) {
            line = languageLine;
            replaced = true;
        }
    }
    if (!replaced)
        lines.append(languageLine);

    QSaveFile target(path);
    if (!target.open(QIODevice::WriteOnly))
        throw std::runtime_error(target.errorString().toStdString());
    QByteArray bytes = (lines.join('\n') + '\n').toUtf8();
    if (target.write(bytes) != bytes.size() || !target.commit())
        throw std::runtime_error(target.errorString().toStdString());
    qInfo() << "Saved UI locale preference:" << locale << "to" << path;
}

} // namespace editor
