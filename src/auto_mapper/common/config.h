/**
 * @file config.h
 * @brief universal path and configuration
 *
 * auto identify the program's runtime environment and
 * determine the critical system absolute path.
 */
#pragma once
#include <filesystem>

namespace auto_mapper {
    namespace fs = std::filesystem;

    class Config {
    public:
        static fs::path root_dir; // root path for exe or the script
        static fs::path log_file_path;

        // must be first function to be called when the system starts up
        static void init_env();
        
    private:
        static fs::path get_app_root();
    };
}
