#define LOG_TAG "uargs"

#include "noise/def.h"
#include "noise/util.h"
#include "3rd/args.hxx"
#include "noise/args.h"
#include "noise/log.h"

namespace noise {

const static char *args0_usage = "\n"                                   \
"Example:\n"                                                            \
"    >./bt -h\n"                                                        \
"    >./bt -p 0                     //[0,1]                        \n"  \
"    >./bt -c 300015.SZ             //code                         \n"  \
"    >./bt -c xxx/000026.SZ.csv     //bar file                     \n"  \
"    >./bt -m 100000.0              //init equity                  \n"  \
"    >./bt -f .code.csv             //codes                        \n"  \
"    >./bt -f .code.csv -k10 -n200  //codes.skip(10).head(200)     \n"  \
"    >./bt -f .code.csv -s0         //skip stat info               \n"  \
";\n"                                                              \
;

/**
 * @brief parse commaon args
 * @eg.
 *   help                   -h
 *   gui.plot()             -p
 *   backtest one code      -c      //conflict with -f
 *   init cash money        -m
 *   backtest codes files   -f
 *   num of code            -n
 *   skip num of code       -k
 *
 */
int uargs::args0_parse(const char *title, int32_t argc, char** argv)
{
    PtrArgs param = args0_get();

    param->brief = title;
    param->usage = args0_usage;
    args::ArgumentParser            parser(param->brief, param->usage);
    args::HelpFlag                  ahelp(parser,         "help ",     "Display this help menu",        {'h',  ARGS_HELP});
    args::ValueFlag<std::string>    plot(parser,          " int ",     "GUI plot on finish",            {'p',  ARGS_PLOT});
    args::ValueFlag<std::string>    code(parser,          "code ",     "code",                          {'c',  ARGS_CODE});
    args::ValueFlag<std::string>    cash(parser,          "float",     "init cash money",               {'m',  ARGS_INIT_CASH});
    args::ValueFlag<std::string>    date(parser,          "date ",     "begin date 20230101",           {'d',  ARGS_BEGIN_DATE});
    args::ValueFlag<std::string>    codefile(parser,      "file ",     "codes in file",                 {'f',  ARGS_CODE_FILE});
    args::ValueFlag<std::string>    num(parser,           " num ",     "codes.head(num)",               {'n',  ARGS_NUM});
    args::ValueFlag<std::string>    skip(parser,          "skip ",     "codes.skip(num)",               {'k',  ARGS_SKIP});
    args::ValueFlag<std::string>    stat(parser,          " int ",     "print stat info",               {'s',  ARGS_STAT});
    args::Positional<std::string>   image_input(parser,   "image",     "input image");

    //TODO:
    // codes on position arguments
    //
    try {
        parser.ParseCLI(argc, argv);
        if (plot) {
            param->args_map.emplace(ARGS_PLOT, args::get(plot).c_str());
        }
        if (code) {
            param->args_map.emplace(ARGS_CODE, args::get(code).c_str());
        }
        if (codefile) {
            param->args_map.emplace(ARGS_CODE_FILE, args::get(codefile).c_str());
        }
        if (cash) {
            param->args_map.emplace(ARGS_INIT_CASH, args::get(cash).c_str());
        }
        if (date) {
            param->args_map.emplace(ARGS_BEGIN_DATE, args::get(date).c_str());
        }
        if (num) {
            param->args_map.emplace(ARGS_NUM, args::get(num).c_str());
        }
        if (skip) {
            param->args_map.emplace(ARGS_SKIP, args::get(skip).c_str());
        }
        if (stat) {
            param->args_map.emplace(ARGS_STAT, args::get(stat).c_str());
        }
        if (ahelp) {
            std::cerr << parser;
            return -1;
        }
        // if (image_list) { for (const auto one: args::get(image_list)) {std::cout << " one_image: " << one << std::endl;} }
    } catch (args::Help const&help) {
        (void) help;
        std::cerr << parser;
        return -2;
    } catch (args::ParseError const&e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return -1;
    } catch (args::ValidationError const&e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return -1;
    }

    LOGD("cmd args:");
    int i = 0;
    for (auto iter = param->args_map.begin(); iter != param->args_map.end(); iter++) {
        LOGD(" {} map[{}]={}", i, iter->first, iter->second);
        i++;
    }
    return 0;
}

PtrArgs uargs::args0_get(void)
{
    static PtrArgs s_args = nullptr;
    if (s_args == nullptr) {
        s_args = std::make_shared<struct args0>();
    }
    return s_args;
}

}