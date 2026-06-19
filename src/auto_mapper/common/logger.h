/**
 * @file logger.h
 * @brief log system
 */

#pragma once
#include <string>
#include <format>
#include <iostream>

#include <source_location>

namespace auto_mapper {

    template <typename... Args>
    struct LogFormat {
        std::format_string<Args...> fmt;
        std::source_location loc;

        template <typename Str>
        consteval LogFormat(const Str& s, const std::source_location l = std::source_location::current())
            : fmt(s), loc(l) {}
    };

    /**
    * @brief Universal logging facility.
    */
    class Logger {
    public:
        static void init();
        template<typename... Args>
        static void debug(LogFormat<std::type_identity_t<Args>...> format, Args&&... args) {
            log(Level::DEBUG, format.loc, std::format(format.fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void info(LogFormat<std::type_identity_t<Args>...> format, Args&&... args) {
            log(Level::INFO, format.loc, std::format(format.fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void warning(LogFormat<std::type_identity_t<Args>...> format, Args&&... args) {
            log(Level::WARNING, format.loc, std::format(format.fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void error(LogFormat<std::type_identity_t<Args>...> format, Args&&... args) {
            log(Level::ERROR, format.loc, std::format(format.fmt, std::forward<Args>(args)...));
        }

    private:
        enum class Level { DEBUG, INFO, WARNING, ERROR };
        static void log(Level level, const std::source_location& loc, const std::string& msg);
    };

}
