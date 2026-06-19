/**
 * @file logger.cpp
 * @brief log system (powered by spdlog)
 */
#include "logger.h"
#include "config.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>
#include <iostream>

#ifdef _WIN32
#undef ERROR
#endif

namespace auto_mapper {

    void Logger::init() {
        try {
            // Setup console sink
            const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            // Setup file sink, overwrite existing file
            const auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(Config::log_file_path.string(), true);

            // Create multi-sink logger
            std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
            const auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
            
            // Set pattern to include filename and line number
            // [%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v
            logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
            
            // Set default logger
            spdlog::set_default_logger(logger);
            spdlog::set_level(spdlog::level::debug); // Set global log level to debug
            spdlog::flush_on(spdlog::level::warn); // Optimize flush
        } catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Log initialization failed: " << ex.what() << "\n";
        }
    }

    void Logger::log(const Level level, const std::source_location& loc, const std::string& msg) {
        spdlog::level::level_enum mapped_level;
        switch (level) {
            case Level::DEBUG:   mapped_level = spdlog::level::debug; break;
            case Level::INFO:    mapped_level = spdlog::level::info; break;
            case Level::WARNING: mapped_level = spdlog::level::warn; break;
            case Level::ERROR:   mapped_level = spdlog::level::err; break;
            default:             mapped_level = spdlog::level::info; break;
        }

        // Forward to spdlog, preserving source location
        const spdlog::source_loc source{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()};
        if (const auto logger = spdlog::default_logger()) {
            logger->log(source, mapped_level, msg);
        }
    }

}
