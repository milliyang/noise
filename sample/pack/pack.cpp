
#define LOG_TAG "pack"

#include "noise/filesystem/fs.h"

#include "noise/noise.h"
#include "noise/util2.h"
#include "noise/log.h"
#include "spdlog/fmt/fmt.h"
#include "3rd/args.hxx"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>

using namespace noise;

#define CODE_BASIC_CSV   "basic.csv"

void pack_bars_csv_to_h5(const std::string &dir, const std::string &filename, int num)
{
    fs::H5 h5_writer = fs::H5(filename, false);

    std::vector<struct code_info> infos;

    std::string basic_path = fmt::format("{}/{}", dir, CODE_BASIC_CSV);
    util::parse_code_file(infos, basic_path);

    struct feed_config cfg;
    cfg.root_path = dir;

    VecS codes;
    int i = 0;
    for(auto it = infos.begin(); it != infos.end(); it++) {
        codes.push_back(it->code);
        {
            FeedCSV feed;
            cfg.code = it->code;
            feed.init(cfg);
            auto bars = feed.get_bars();

            struct bars bbb;
            bbb.code = it->code;
            bbb.data = bars;

            auto h5_bars = fs::fs_get_h5_bars(bbb);
            h5_writer.write_bars(h5_bars);
            //LOGI("pack:{}", h5_bars.code);
        }

        //
        if (num > 0) {
            if (i++ > num) {
                break;
            }
        }
    }
    h5_writer.write_codes(codes);

    auto basic = fs::fs_get_basic(infos);
    h5_writer.write_basic(basic);
    LOGI("generate: {}", filename);
}

void unpack_bars_h5_csv(const std::string& dir, const std::string& filename)
{
    std::string bar_dir = fmt::format("{}/bars", dir);
    std::filesystem::create_directory(dir);
    std::filesystem::create_directory(bar_dir);

    fs::H5 h5_reader = fs::H5(filename);
    VecS codes = h5_reader.read_codes();

    for (int i = 0; i < codes.size(); i++) {
        auto h5_bars = h5_reader.read_bars(codes[i]);
        auto bars = fs::fs_get_bars(h5_bars);

        std::string bar_file = fmt::format("{}/{}.csv", bar_dir, codes[i]);
        fs::csv_write_bars_file(bars.data, bar_file);
    }

    auto basic = h5_reader.read_basic();
    std::string basic_file = fmt::format("{}/{}", dir, CODE_BASIC_CSV);
    fs::csv_write_basic_file(basic, basic_file);
    LOGI("unpack done");
}

const static char* USAGE = "\n"                   \
"Example:\n"                                      \
"    >./pack -h\n"                                \
"    >./pack -d dir_root -f filename         \n"  \
"    >./pack -d dir_root -f filename -n 100  \n"  \
"    >./pack -d dir_root -f filename -x      \n"  \
"Note:\n"  \
"     \n"  \
;

const static char* ROOT =   "\n" \
" ROOT should be:            \n" \
"                            \n" \
"   +-- dir_root             \n" \
"   |   +-- basic.csv        \n" \
"   |   +-- bars             \n" \
"   |   |   +-- xxx001.csv   \n" \
"   |   |   +-- xxx002.csv   \n" \
;

const static char* BRIEF = "pack or unpack h5 data";

struct cmd_param {
    std::string path;       //path to root
    std::string filename;   //h5 file
    int num;
    bool pack;              //pack/unpack
};

int cmd_parse(struct cmd_param &param, int32_t argc, char** argv)
{
    args::ArgumentParser         parser(BRIEF, USAGE);
    args::HelpFlag               ahelp(parser,         "help ",     "Display help",   {'h',  "help"});
    args::ValueFlag<std::string> dir(parser,          " int ",      "root_dir",       {'d',  "dir"});
    args::ValueFlag<std::string> file(parser,          "code ",     "h5 file",        {'f',  "file"});
    args::ValueFlag<std::string> num(parser,           " num ",     "number",         {'n',  "num"});
    args::Flag                   unpack(parser,        "null ",     "unpack",         {'x'} );

    try {
        parser.ParseCLI(argc, argv);
        if (dir) {
            param.path = args::get(dir);
        }
        if (file) {
            param.filename = args::get(file);
        }
        if (unpack) {
            param.pack = false;
        } else {
            param.pack = true;
        }
        if (num) {
            param.num = std::stoi(args::get(num));
        } else {
            param.num = -1;
        }
        if (ahelp) {
            std::cerr << parser;
            std::cerr << ROOT;
            return -1;
        }
    } catch (args::Help const&help) {
        (void) help;
        std::cerr << parser;
        std::cerr << ROOT;
        return -2;
    } catch (args::ParseError const&e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        std::cerr << ROOT;
        return -1;
    } catch (args::ValidationError const&e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        std::cerr << ROOT;
        return -1;
    }

    if (param.filename.length() < 1 || param.path.length() < 1) {
        std::cerr << parser;
        std::cerr << ROOT;
        return -1;
    }
    return 0;
}

int32_t main(int32_t argc, char** argv)
{
    struct cmd_param param;
    log_init();
    if (cmd_parse(param, argc, argv) != 0) {
        return 0;
    };

    LOGI("path : {}", param.path);
    LOGI("file : {}", param.filename);
    LOGI("pack : {}", param.pack ? "true" : "false");
    LOGI("num  : {}", param.num);
    LOGI("");

    if (param.pack) {
        pack_bars_csv_to_h5(param.path, param.filename, param.num);
    } else {
        unpack_bars_h5_csv(param.path, param.filename);
    }

    LOGI(" ->@{} [done]", __FUNCTION__);
    return 0;
}

/** usage:

.\Debug\pack.exe -d P:\tmpfs\root -f .chs.codeset.h5
.\Debug\pack.exe -d P:\tmpfs\root -f .chs.codeset.h5 -n 100

.\Debug\pack.exe -d D:\test -f .chs.codeset.h5 -x


*/