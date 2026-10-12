#include "editor/i18n/locale.h"
#include "editor/config.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QRegularExpression>
#include <stdexcept>

namespace editor {

bool isSupportedLocale(const QString &name) {
    if (name == LOCALE_EN_US)
        return true;
    if (name == LOCALE_ZH_CN)
        return true;
    return false;
}

Locale::Locale(const QString &name) : name_(name) {
    initEditorResources();
    if (!isSupportedLocale(name))
        throw std::runtime_error(("Unsupported locale: " + name).toStdString());
    QString path = ":/i18n/" + name + ".json";
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        throw std::runtime_error(("Missing resource: " + path).toStdString());
    texts_ = QJsonDocument::fromJson(file.readAll()).object();
    qDebug() << "Loaded i18n locale:" << name;
}

QString Locale::name() const {
    return name_;
}

bool Locale::isChinese() const {
    return name_ == LOCALE_ZH_CN;
}

QString Locale::tr(const QString &key) const {
    if (!texts_.contains(key)) {
        qWarning() << "Missing i18n text key:" << key;
        return key;
    }
    return texts_.value(key).toString();
}

QString Locale::tr(const QString &key, const TextArgs &values) const {
    QString text = tr(key);
    for (const auto &value : values) {
        // "{size}" and "{map_size_x:.1f}" both name the placeholder "size" / "map_size_x".
        QRegularExpression placeholder("\\{" + QRegularExpression::escape(value.first) + "(:[^}]*)?\\}");
        // Splice by hand: QString::replace(regex, ...) would treat "\1" in Windows paths as backreferences.
        QRegularExpressionMatch match = placeholder.match(text);
        while (match.hasMatch()) {
            text.replace(match.capturedStart(), match.capturedLength(), value.second);
            int nextStart = match.capturedStart() + value.second.size();
            match = placeholder.match(text, nextStart);
        }
    }
    return text;
}

bool Locale::contains(const QString &key) const {
    return texts_.contains(key);
}

QStringList Locale::keys() const {
    return texts_.keys();
}

} // namespace editor
