/**
 * @file logger.h
 * @author 张一帆
 * @brief  日志输出宏
 * @version 0.1
 * @date 2022-12-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef LOGGER_H_
#define LOGGER_H_
#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "common_global.h"
#include <spdlog/spdlog.h>

namespace Common {
    COMMON_EXPORT void InitLogger(const std::string &path = "", spdlog::level::level_enum defaultLevel = spdlog::level::debug);
}

#define LOG_TRACE             SPDLOG_TRACE
#define LOG_DEBUG             SPDLOG_DEBUG
#define LOG_INFO              SPDLOG_INFO
#define LOG_WARN              SPDLOG_WARN
#define LOG_ERROR             SPDLOG_ERROR

#endif // LOGGER_H_