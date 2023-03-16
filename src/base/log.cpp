
#include "noise/log.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

static bool inited = false;

// create logger with 2 targets with different log levels and formats.
// the console will show only warnings or errors, while the file will log all.
void static multi_sink_log(void)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    //pattern refer: pattern_formatter-inl
    console_sink->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
    console_sink->set_level(spdlog::level::info);
    //[21:46:54.511] [info] [uprint.cpp:74] [%s] %s

    //
    bool truncate = true;
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/sink.d.log", truncate);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    file_sink->set_level(spdlog::level::debug);
    //file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
    //[2023-01-11 21:46:54.511] [loggername] [info] [uprint.cpp:74] [%s] %s

    auto file_sink_info = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/sink.i.log", truncate);
    file_sink_info->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    file_sink_info->set_level(spdlog::level::info);

    std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink, file_sink_info };
    auto logger = std::make_shared<spdlog::logger>("loggers", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::debug);
    spdlog::set_default_logger(logger);

    //LOGD("this should appear in both console and file");
    //LOGI("this message should not appear in the console, only in the file");
}

void log_init(void)
{
    if (!inited) {
        multi_sink_log();
    }
}

void log_deinit(void)
{

}