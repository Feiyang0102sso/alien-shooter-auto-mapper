// AutoMapper editor entry point: logging, language choice, style, then the main window.
#include "editor/config.h"
#include "editor/i18n/locale.h"
#include "editor/i18n/text_keys.h"
#include "editor/logger.h"
#include "editor/ui/main_window.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDialog>
#include <QDialogButtonBox>
#include <QElapsedTimer>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QTimer>
#include <QVBoxLayout>

namespace {
constexpr const char *APP_STYLE = "windows11";
constexpr const char *APP_FONT = "Microsoft YaHei UI";
constexpr int APP_FONT_SIZE = 9;
constexpr const char *STYLE_RESOURCE = ":/styles/dark_theme.qss";
// Wait for the first paint before grabbing a --screenshot.
constexpr int SCREENSHOT_DELAY_MS = 800;

// First-start language picker. Shown before any locale exists, so its text is bilingual on purpose.
QString chooseInitialLanguage() {
    QDialog dialog;
    dialog.setWindowTitle("Language / 语言");
    dialog.setModal(true);
    auto *layout = new QVBoxLayout(&dialog);
    layout->setSpacing(10);
    layout->addWidget(new QLabel("Choose language / 选择语言"));
    auto *english = new QRadioButton("English");
    auto *chinese = new QRadioButton("中文");
    english->setChecked(true);
    layout->addWidget(english);
    layout->addWidget(chinese);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout->addWidget(buttons);
    dialog.exec();
    if (chinese->isChecked())
        return editor::LOCALE_ZH_CN;
    return editor::LOCALE_EN_US;
}

void loadStyle(QApplication &app) {
    QFile style(STYLE_RESOURCE);
    if (!style.open(QIODevice::ReadOnly)) {
        qWarning() << "Style file not found:" << STYLE_RESOURCE;
        return;
    }
    app.setStyleSheet(QString::fromUtf8(style.readAll()));
    qDebug() << "Loaded QSS style:" << STYLE_RESOURCE;
}
} // namespace

int main(int argc, char **argv) {
    QElapsedTimer startup;
    startup.start();
    QApplication app(argc, argv);
    editor::initLogging();
    editor::initEditorResources();
    app.setStyle(APP_STYLE);
    app.setFont(QFont(APP_FONT, APP_FONT_SIZE));
    app.setApplicationName("AutoMapper");
    app.setOrganizationName("AutoMapper");

    // Qt reads PRODUCTVERSION from editor.rc as four numbers ("1.0.1.0");
    // AutoMapper versions have three parts, so drop the unused fourth one.
    QStringList versionParts = QCoreApplication::applicationVersion().split('.');
    if (versionParts.size() == 4)
        versionParts.removeLast();
    app.setApplicationVersion(versionParts.join('.'));
    qInfo() << "AutoMapper" << QCoreApplication::applicationVersion() << "starting";

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption({"language", "UI locale: en_US or zh_CN", "locale"});
    parser.addOption({"screenshot", "Save a rendered window and exit", "path"});
    parser.addPositionalArgument("project", "Optional project JSON to open", "[project]");
    parser.process(app);

    QString language = parser.value("language");
    if (language.isEmpty())
        language = editor::loadLanguage();
    if (language.isEmpty()) {
        language = chooseInitialLanguage();
        try {
            editor::saveLanguage(language);
        } catch (const std::exception &error) {
            // Not fatal: the picker simply shows again on the next start.
            qWarning() << "Failed to save language:" << error.what();
        }
    }
    if (!editor::isSupportedLocale(language)) {
        qCritical() << "Unsupported language:" << language;
        return 2;
    }
    loadStyle(app);

    try {
        editor::MainWindow window(language);
        window.show();
        if (!parser.positionalArguments().isEmpty())
            window.openProject(parser.positionalArguments().first());
        if (parser.isSet("screenshot")) {
            QTimer::singleShot(SCREENSHOT_DELAY_MS, &window, [&] {
                bool saved = window.grab().save(parser.value("screenshot"));
                if (saved)
                    app.exit(0);
                else
                    app.exit(3);
            });
        }
        qInfo() << "Startup to main window:" << startup.elapsed() << "ms";
        return app.exec();
    } catch (const std::exception &error) {
        // Missing assets or broken resources: tell the user instead of closing silently.
        qCritical() << "Startup failed:" << error.what();
        QMessageBox::critical(nullptr, "AutoMapper", QString::fromUtf8(error.what()));
        return 1;
    }
}
