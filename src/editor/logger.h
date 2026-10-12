// Application logging. Qt messages (qDebug/qInfo/qWarning/qCritical) are forwarded to the core
// spdlog logger, so editor and generator messages share one AutoMapper.log beside the exe.
#pragma once

namespace editor {

// Call once after QApplication is created. Tests skip it and keep Qt's default console output.
void initLogging();

} // namespace editor
