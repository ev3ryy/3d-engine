#ifndef LOGS_H
#define LOGS_H

#include "logger.h"

#define LOG_INFO(...) logger::log_info(__VA_ARGS__)
#define LOG_ERROR(...) logger::log_error(__VA_ARGS__)
#define LOG_WARN(...) logger::log_warn(__VA_ARGS__)
#define LOG_CRITICAL(...) logger::log_critical(__VA_ARGS__)

#endif // LOGS_H