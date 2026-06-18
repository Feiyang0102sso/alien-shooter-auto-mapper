/**
 * @file logger.h
 * @brief log system + coloring + stop_on_error setting
 */

#pragma once
#include <string>
#include <stdexcept>
#include <format>
#include <iostream>

#include <source_location>

namespace auto_mapper {

    // forcefully interrupt the pipeline and throw out error
    class FatalError : public std::runtime_error {
    public:
        explicit FatalError(const std::string& msg) : std::runtime_error(msg) {}
    };

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
    *
    * Provides colored console output and configurable
    * error_and_stop settings
    */
    class Logger {
    public:
        static void init();
        static void set_stop_on_error(bool should_stop);

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

        template<typename... Args>
        static void error_and_stop(LogFormat<std::type_identity_t<Args>...> format, Args&&... args) {
            const std::string msg = std::format(format.fmt, std::forward<Args>(args)...);
            log(Level::ERROR, format.loc, msg);
            if (stop_on_error) {
                throw FatalError(msg);
            }
        }

    private:
        enum class Level { DEBUG, INFO, WARNING, ERROR };
        static void log(Level level, const std::source_location& loc, const std::string& msg);
        static bool stop_on_error;
    };

}
