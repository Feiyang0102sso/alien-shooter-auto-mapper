/**
 * @file logger.cpp
 * @brief log system + coloring + stop_on_error setting
 */
#include "logger.h"
#include "config.h"
#include <fstream>
#include <chrono>
#include <mutex>
#include <filesystem>

// add to fix CLion console not colored strange ...
#ifdef _WIN32
#include <windows.h>
#undef ERROR
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif
// ...

namespace auto_mapper {

    bool Logger::stop_on_error = false;
    static std::ofstream log_file;
    static std::mutex log_mutex;

    void Logger::set_stop_on_error(const bool should_stop) {
        stop_on_error = should_stop;
    }

    void Logger::init() {

// add to fix CLion console not colored
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
        HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
        if (hErr != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hErr, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hErr, dwMode);
            }
        }
#endif
// ...

        std::lock_guard<std::mutex> lock(log_mutex);
        if (log_file.is_open()) {
            log_file.close();
        }
        // log into .log which is defined in config
        log_file.open(Config::log_file_path, std::ios::out | std::ios::trunc);
    }

    void Logger::log(const Level level, const std::source_location& loc, const std::string& msg) {
        // get timestamp
        const auto now = std::chrono::system_clock::now();
        const std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        struct tm parts = {};
        localtime_s(&parts, &now_c); // Windows safe local time

        char time_buf[64];
        std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &parts);

        std::string time_str = std::format("{}.{:03d}", time_buf, ms.count());

        std::string level_str;
        std::string color_code;
        const auto reset_code = "\033[0m";

        switch (level) {
            case Level::DEBUG:   level_str = "DEBUG";   color_code = "\033[36m"; break; // Cyan
            case Level::INFO:    level_str = "INFO";    color_code = "\033[32m"; break; // Green
            case Level::WARNING: level_str = "WARNING"; color_code = "\033[33m"; break; // Yellow
            case Level::ERROR:   level_str = "ERROR";   color_code = "\033[31m"; break; // Red
        }

        const std::string console_line = std::format("[{}] [{}] {}", time_str, level_str, msg);
        
        // Format for file: add file name and line number
        // We use std::filesystem::path to extract just the filename from the full path
        const std::filesystem::path file_path(loc.file_name());
        const std::string file_line = std::format("[{}] [{}] [{}:{}] {}", 
            time_str, level_str, file_path.filename().string(), loc.line(), msg);

        // write into file and console with lock
        std::lock_guard<std::mutex> lock(log_mutex);

        // removed
        // if (level == Level::ERROR) {
        //     std::cerr << color_code << log_line << reset_code << "\n";
        // } else {
        //     std::cout << color_code << log_line << reset_code << "\n";
        // }
        
        // Output all levels to std::cout to prevent out-of-order printing in IDEs (like CLion) 
        // which read stdout and stderr via separate asynchronous pipes.
        std::cout << color_code << console_line << reset_code << "\n";

        if (log_file.is_open()) {
            // log file no need for coloring, but includes source location
            log_file << file_line << "\n";
        }

        // Optimize flush: Only flush immediately on WARNING or ERROR to improve I/O performance
        if (level == Level::WARNING || level == Level::ERROR) {
            std::cout.flush();
            if (log_file.is_open()) {
                log_file.flush();
            }
        }
    }

}
