/**
 * @file config.cpp
 * @brief universal path and configuration
 */
#include "config.h"
#include "logger.h"

#ifdef _WIN32
#include <windows.h>
#else
#error "Currently only Windows OS is supported due to get_app_root implementation"
#endif

namespace auto_mapper {

    fs::path Config::root_dir;
    fs::path Config::log_file_path;

    fs::path Config::get_app_root() {
        wchar_t path[MAX_PATH];
        // get cur EXE abs path
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const fs::path exe_path(path);
        return exe_path.parent_path();
    }

    void Config::init_env() {
        root_dir = get_app_root();
        log_file_path = root_dir / "AutoMapper_dll.log";

        // Initialize logger file stream now that log_file_path is configured
        Logger::init();

        Logger::debug("App Root Dir: {}", root_dir.string());
        Logger::debug("Log File Path: {}", log_file_path.string());
    }

}
