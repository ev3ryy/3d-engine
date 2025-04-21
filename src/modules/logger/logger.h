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

namespace logger {
	inline std::string getLogFileName() {
        /*auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_now;
        
        localtime_s(&tm_now, &now_time);

        std::ostringstream oss;
        oss << "engine-" << std::put_time(&tm_now, "%Y-%m-%d-%H-%M-%S") << ".log";

        return "logs/" + oss.str();*/

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
}

#endif // LOGGER_H