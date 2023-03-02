
#pragma once

#ifdef CONFIG_SPDLOG
  #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
  #include "spdlog/spdlog.h"
  #include "spdlog/fmt/fmt.h"

  //file:
  #define LOGT    SPDLOG_TRACE
  #define LOGD    SPDLOG_DEBUG
  //terminal:
  #define LOGI    SPDLOG_INFO
  #define LOGE    SPDLOG_ERROR
  #define LOGW    SPDLOG_WARN
  #define LOGC    SPDLOG_CRITICAL
#else
  #include "spdlog/fmt/fmt.h"
  #if 1
    #define LOGT    printf
    #define LOGD    printf
    #define LOGI    printf
    #define LOGE    printf
    #define LOGW    printf
    #define LOGC    printf
  #else
    
    #define LOGV(...) printf(fmt::format(##__VA_ARGS__ ).c_str())
    #define LOGD(...) printf(fmt::format(##__VA_ARGS__ ).c_str())
    #define LOGI(...) printf(fmt::format(##__VA_ARGS__ ).c_str())
    #define LOGW(...) printf(fmt::format(##__VA_ARGS__ ).c_str())
    #define LOGE(...) printf(fmt::format(##__VA_ARGS__ ).c_str())
  #endif
#endif

void log_init(void);
void log_deinit(void);

