#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <filesystem>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <memory>
#include <string>
#include <cstdarg>

namespace logger {
    inline std::string my_sprintf(const char* fmt_str, ...) {
        int final_n, n = ((int)std::strlen(fmt_str)) * 2;
        std::string str;
        std::vector<char> buf(n);
        va_list ap;

        while (true) {
            va_start(ap, fmt_str);
            final_n = vsnprintf(buf.data(), n, fmt_str, ap);
            va_end(ap);
            if (final_n < 0 || final_n >= n) {
                n += std::abs(final_n - n + 1);
                buf.resize(n);
            }
            else {
                str = buf.data();
                break;
            }
        }
        return str;
    }

	inline std::string getLogFileName() {
#ifdef _DEBUG
        return "logs/engine-debug.log";
#else
        return "logs/engine-release.log";
#endif
	}

	inline void init() {
        std::filesystem::create_directories("logs");

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        std::string file_path = getLogFileName();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_path, true);

        std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
        auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());

        logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

        logger->set_level(spdlog::level::trace);
        spdlog::set_default_logger(logger);

        spdlog::flush_on(spdlog::level::info);
	}

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

} // namespace logging

#endif // LOGGER_H