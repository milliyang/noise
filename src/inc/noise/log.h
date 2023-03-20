
#pragma once

#include "spdlog/fmt/fmt.h"

#ifndef LOG_TAG
  #define LOG_TAG "none"
#endif

#ifdef CONFIG_SPDLOG

  #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
  #include "spdlog/spdlog.h"

  #define LOGT(...)    SPDLOG_TRACE("[" LOG_TAG "] " __VA_ARGS__)
  #define LOGD(...)    SPDLOG_DEBUG("[" LOG_TAG "] " __VA_ARGS__)
  #define LOGI(...)    SPDLOG_INFO("[" LOG_TAG "] " __VA_ARGS__)
  #define LOGE(...)    SPDLOG_ERROR("[" LOG_TAG "] " __VA_ARGS__)
  #define LOGW(...)    SPDLOG_WARN("[" LOG_TAG "] " __VA_ARGS__)
  #define LOGC(...)    SPDLOG_CRITICAL("[" LOG_TAG "] " __VA_ARGS__)
#else
  #if 0
    #define LOGT    printf
    #define LOGD    printf
    #define LOGI    printf
    #define LOGE    printf
    #define LOGW    printf
    #define LOGC    printf
  #else
    #define LOGV(...) log_printf(fmt::format(__VA_ARGS__ ))
    #define LOGD(...) log_printf(fmt::format(__VA_ARGS__ ))
    #define LOGI(...) log_printf(fmt::format(__VA_ARGS__ ))
    #define LOGW(...) log_printf(fmt::format(__VA_ARGS__ ))
    #define LOGE(...) log_printf(fmt::format(__VA_ARGS__ ))
  #endif
#endif

void log_init(void);
void log_deinit(void);

void log_backtrace(void);
void log_printf(const std::string &data);