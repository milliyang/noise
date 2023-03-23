#define LOG_TAG "utils"

#include "noise/def.h"
#include "noise/strategy.h"
#include "noise/broker.h"
#include "noise/util.h"
#include "noise/log.h"

#include "3rd/INIReader.h"

#include <string.h>

namespace noise {

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

std::vector<std::string> split_without_string(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
       //remove ' '
       token.erase(0,token.find_first_not_of(" "));
       token.erase(token.find_last_not_of(" ") + 1);

       //remove '"'
       token.erase(0, token.find_first_not_of("\""));
       token.erase(token.find_last_not_of("\"") + 1);
       tokens.push_back(token);
   }
   return tokens;
}

std::string ext_to_lower_case(const std::string& s, char delimiter)
{
    std::string lower(s);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    std::vector<std::string> vstr = split(lower, '.');
    if (vstr.size() <= 1) {
        return lower;
    }
    std::string ext = vstr[vstr.size()-1];
    return ext;
}

bool is_file_ext(const std::string&s, const std::string&ext, char delimiter)
{
    std::string lower_ext = ext_to_lower_case(s, delimiter);
    if (lower_ext == ext) {
        return true;
    } else {
        return false;
    }
}

void util::parse_csv_bars_file(std::vector<struct bar> &bars, const std::string &filepath)
{
    std::ifstream input{filepath};

    //
    // trade_date,ts_code,open,high,low,close,pre_close,change,pct_chg,vol,amount
    // 20180102,000001.SZ,12.6805,13.2314,12.652,13.0129,12.633,0.379900000000001,3.0072,2081592.55,2856543.822
    // 20180103,000001.SZ,13.0414,13.1649,12.538,12.6615,13.0129,-0.35139999999999993,-2.7004,2962498.38,4006220.766

    if (!input.is_open()) {
        LOGE("open failed: {}", filepath);
        return;
    }

    int cnt = 0;
    int column = 0;

    int    cache_date = -1;
    time_t cache_time;

    for (std::string line; std::getline(input, line);) {
        std::istringstream ss(std::move(line));
        std::vector<std::string> one_row;

        for (std::string value; std::getline(ss, value, ',');) {
            one_row.push_back(std::move(value));
        }
        if (column == 0) {
            if (one_row.at(0) != "trade_date") {
                LOGE("parse failed: {}", line);
                return;
            }
            column = (int) one_row.size();
            continue;
            //printf("bars column:%d\n", (int)one_row.size());
        } else {
            if (column != one_row.size()) {
                LOGW("invalid line: {}", line);
                continue; //ignore
            }

            struct bar bar;
            int i = 0;
            bar.date        = std::stoi(one_row.at(i++));
            bar.code        = one_row.at(i++);
            bar.open        = std::stof(one_row.at(i++));
            bar.high        = std::stof(one_row.at(i++));
            bar.low         = std::stof(one_row.at(i++));
            bar.close       = std::stof(one_row.at(i++));
            bar.pre_close   = std::stof(one_row.at(i++));
            bar.change      = std::stof(one_row.at(i++));
            bar.pct_chg     = std::stof(one_row.at(i++));
            bar.volumn      = std::stof(one_row.at(i++));
            bar.amount      = std::stof(one_row.at(i++));

            if (cache_date != bar.date) {
                cache_date = bar.date;
                cache_time = utime::get_time(bar.date);
            }
            bar.time        = cache_time;

            //printf("bar_time date:%d  ", bar.date);
            //uprint::print("bar_time read_bar", bar);
            //uprint::print("bar_time", bar.time);
            bars.push_back(bar);
        }
        cnt++;
    }
}

void util::parse_ini_code_file(std::vector<struct code_info> &codes, const std::string &filepath)
{
    INIReader reader(filepath);

    if (reader.ParseError() < 0) {
        LOGE("Can't load {}", filepath);
        return;
    }
    auto codes_str = reader.Get("Backtest", "codes", "NULL");
    if (codes_str == "NULL") {
        return;
    }
    auto the_codes = split(codes_str, ',');

    for (auto iter = the_codes.begin(); iter != the_codes.end(); iter++) {
        struct code_info info;
        info.code = *iter;
        codes.push_back(info);
    }
    return;
}

void util::parse_csv_code_file(std::vector<struct code_info> &codes, const std::string &filepath)
{
    std::ifstream input{filepath};
    //
    // seq,ts_code,symbol,name,area,industry,market,list_date
    // 0,000001.SZ,000001,平安银行,深圳,银行,主板,19910403
    // 1,000002.SZ,000002,万科A,深圳,全国地产,主板,19910129
    if (!input.is_open()) {
        LOGE("open failed: {}", filepath);
        return;
    }

    int column = 0;
    for (std::string line; std::getline(input, line);) {
        std::istringstream ss(std::move(line));
        std::vector<std::string> one_row;

        for (std::string value; std::getline(ss, value, ',');) {
            one_row.push_back(std::move(value));
        }
        if (column == 0) {
            if (one_row.at(0) != "seq") {
                LOGE("parse failed: {}", line);
                return;
            }
            column = (int) one_row.size();
            continue;
            //printf("bars column:%d\n", (int)one_row.size());
        } else {
            if (column != one_row.size()) {
                LOGW("invalid line: {}", line);
                continue; //ignore
            }
            struct code_info info;
            int i = 0;
            info.seq         = one_row.at(i++);
            info.code        = one_row.at(i++);
            info.symbol      = one_row.at(i++);
            info.name        = one_row.at(i++);
            info.area        = one_row.at(i++);
            info.industry    = one_row.at(i++);
            info.market      = one_row.at(i++);
            info.list_date   = std::stoi(one_row.at(i++));
            codes.push_back(info);
            //uprint::print("", info);
        }
    }
    return;
}

void util::parse_code_file(std::vector<code_info> &codes, const std::string &filepath)
{
    if (is_file_ext(filepath, "csv", '.')) {
        parse_csv_code_file(codes, filepath);
    } else {
        parse_ini_code_file(codes, filepath);
    }
}

std::string util::get_filename_with_extension(const std::string& filepath)
{
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    size_t i = filepath.rfind(sep, filepath.length());
    if (i != std::string::npos) {
        std::string filename = filepath.substr(i+1, filepath.length() - i);
        std::string rawname = filename.substr(0, filepath.length());
        return(rawname);
    }
    return"";
}

std::string util::get_filename_without_extension(const std::string& filepath)
{
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    size_t i = filepath.rfind(sep, filepath.length());
    if (i != std::string::npos) {
        std::string filename = filepath.substr(i+1, filepath.length() - i);
        size_t lastindex = filename.find_last_of(".");
        std::string rawname = filename.substr(0, lastindex);
        return(rawname);
    }
    return"";
}

bool util::is_end_with(const std::string& filepath, const std::string& substr)
{
    if (substr.length() >= filepath.length()) {
        return false;
    }
    auto end_str = filepath.substr(filepath.length() - substr.length(), substr.length());
    if (end_str == substr) {
        return true;
    } else {
        return false;
    }
}

Timelapsed::Timelapsed(std::string tag) {start = std::chrono::high_resolution_clock::now(); a_tag=tag;}

Timelapsed::~Timelapsed(void) {
    auto nano = std::chrono::high_resolution_clock::now() - start;
    //auto us = std::chrono::duration_cast<std::chrono::microseconds>(nano).count();
    //auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(nano).count();
    //printf("time elapsed: %d(us)  %d(nano sec)\n", (int)us, (int)ns);
    LOGI("time elapsed: {:0.3f} ms   [{}] ", nano.count() / (1000.0f * 1000.0f), a_tag.c_str());
};


}