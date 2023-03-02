
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
    //[21:46:54.511] [info] [uprint.cpp:74] [%s] %s

    //
    bool truncate = true;
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.log", truncate);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    //file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
    //[2023-01-11 21:46:54.511] [loggername] [info] [uprint.cpp:74] [%s] %s

    std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
    auto logger = std::make_shared<spdlog::logger>("loggers", sinks.begin(), sinks.end());

    spdlog::set_default_logger(logger);

    console_sink->set_level(spdlog::level::info);
    file_sink->set_level(spdlog::level::debug);
    logger->set_level(spdlog::level::debug);

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