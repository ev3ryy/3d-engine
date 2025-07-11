#ifndef PHYSICS_LOGS_H
#define PHYSICS_LOGS_H

#include <spdlog/spdlog.h>

namespace logger {
    inline void log_info(const char* message) { spdlog::info(message); }

    template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
        inline void log_info(const char* fmt_str, Args... args) {
        spdlog::info(my_sprintf(fmt_str, args...));
    }

    inline void log_error(const char* message) { spdlog::error(message); }

    template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
        inline void log_error(const char* fmt_str, Args... args) {
        spdlog::error(my_sprintf(fmt_str, args...));
    }

    inline void log_warn(const char* message) { spdlog::warn(message); }

    template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
        inline void log_warn(const char* fmt_str, Args... args) {
        spdlog::warn(my_sprintf(fmt_str, args...));
    }

    inline void log_critical(const char* message) {
        spdlog::critical(message);
        throw std::runtime_error(message);
    }

    template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
        inline void log_critical(const char* fmt_str, Args... args) {
        std::string message = my_sprintf(fmt_str, args...);
        spdlog::critical(message);
        throw std::runtime_error(message);
    }
}

#define LOG_INFO(...) logger::log_info(__VA_ARGS__)
#define LOG_ERROR(...) logger::log_error(__VA_ARGS__)
#define LOG_WARN(...) logger::log_warn(__VA_ARGS__)
#define LOG_CRITICAL(...) logger::log_critical(__VA_ARGS__)

#endif // PHYSICS_LOGS_H