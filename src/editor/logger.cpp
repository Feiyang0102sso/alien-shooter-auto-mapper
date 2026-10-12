#include "editor/logger.h"
#include "auto_mapper/common/config.h"
#include "auto_mapper/common/logger.h"
#include <QString>
#include <QtGlobal>

namespace editor {
namespace {
void forwardQtMessage(QtMsgType type, const QMessageLogContext &context, const QString &message) {
    using Level = auto_mapper::Logger::Level;
    Level level = Level::INFO;
    if (type == QtDebugMsg)
        level = Level::DEBUG;
    if (type == QtWarningMsg)
        level = Level::WARNING;
    if (type == QtCriticalMsg || type == QtFatalMsg)
        level = Level::ERROR;

    // QT_MESSAGELOGCONTEXT keeps file/line in Release builds; fall back to a fixed tag otherwise.
    const char *file = context.file;
    if (file == nullptr)
        file = "qt";
    auto_mapper::Logger::write(level, file, context.line, message.toStdString());
}
} // namespace

void initLogging() {
    // Opens <exe dir>/AutoMapper.log (overwritten each run) plus a console sink.
    auto_mapper::Config::init_env();
    qInstallMessageHandler(forwardQtMessage);
}

} // namespace editor
