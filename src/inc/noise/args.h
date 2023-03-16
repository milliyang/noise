#pragma once

#include <string>
#include <stdint.h>
#include <memory>
#include <map>

#define ARGS_HELP        "help"             //                        -h
#define ARGS_PLOT        "plot"             // gui.plot()             -p
#define ARGS_CODE        "code"             // backtest codes files   -c      //conflict with -f
#define ARGS_CODE_FILE   "code_file"        // backtest one code      -f
#define ARGS_INIT_CASH   "init_cash"        // init cash money        -m
#define ARGS_BEGIN_DATE  "begin_date"       // begin date             -d
#define ARGS_NUM         "num"              // backtest code num      -n
#define ARGS_SKIP        "skip"             // skip code num          -k

namespace noise {

struct args0 {
    const char* brief;
    const char* usage;
    //
    std::map<std::string, std::string> args_map;

    bool        has(std::string key)  { auto iter = args_map.find(key); return (iter != args_map.end()) ? true :false; };
    std::string get(std::string key)  { auto iter = args_map.find(key); return iter->second; };
    float       getf(std::string key) { auto iter = args_map.find(key); return std::stof(iter->second); };
    int         geti(std::string key) { auto iter = args_map.find(key); return std::stoi(iter->second); };
};

}