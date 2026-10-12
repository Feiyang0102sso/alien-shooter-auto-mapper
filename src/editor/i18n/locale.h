// JSON-backed UI text. Every visible string goes through Locale::tr().
#pragma once
#include <QJsonObject>
#include <QList>
#include <QPair>
#include <QString>

namespace editor {

// Supported UI locales; each one has resources/i18n/<locale>.json.
inline constexpr const char *LOCALE_EN_US = "en_US";
inline constexpr const char *LOCALE_ZH_CN = "zh_CN";

// Placeholder values for one text template, e.g. {{"size", "3"}} fills "{size}".
using TextArgs = QList<QPair<QString, QString>>;

bool isSupportedLocale(const QString &name);

class Locale {
  public:
    explicit Locale(const QString &name = LOCALE_EN_US);

    QString name() const;
    bool isChinese() const;

    // Missing keys are logged and fall back to the key itself, like the Python version.
    QString tr(const QString &key) const;
    // Fills "{name}" and Python-style "{name:.1f}" placeholders; callers format numbers.
    QString tr(const QString &key, const TextArgs &values) const;
    bool contains(const QString &key) const;
    QStringList keys() const;

  private:
    QString name_;
    QJsonObject texts_;
};

} // namespace editor
