#define LOG_TAG "fs.csv"

#include "noise/filesystem/fs.h"

#include "noise/util.h"
#include "noise/log.h"
#include "spdlog/fmt/fmt.h"

namespace noise {

namespace fs {


void csv_read_bars_file(std::vector<struct bar> &bars, const std::string &filepath)
{
    std::ifstream input{filepath};

    // trade_date,ts_code,open,high,low,close,pre_close,change,pct_chg,vol,amount
    // 20180102,000001.SZ,12.6805,13.2314,12.652,13.0129,12.633,0.3799,3.0072,2081592.55,2856543.822
    // 20180103,000001.SZ,13.0414,13.1649,12.538,12.6615,13.0129,-0.3514,-2.7004,2962498.38,4006220.766

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

void csv_write_bars_file(const std::vector<struct bar> &bars, const std::string &filepath)
{
    std::ofstream output{filepath};

    // trade_date,ts_code,open,high,low,close,pre_close,change,pct_chg,vol,amount
    // 20180102,000001.SZ,12.6805,13.2314,12.652,13.0129,12.633,0.3799,3.0072,2081592.55,2856543.822
    // 20180103,000001.SZ,13.0414,13.1649,12.538,12.6615,13.0129,-0.3514,-2.7004,2962498.38,4006220.766
    output << "trade_date,ts_code,open,high,low,close,pre_close,change,pct_chg,vol,amount\n";
    for (int i = 0; i < bars.size(); i++) {
        const auto &bar = bars[i];
        std::string ss = fmt::format("{},{},{},{},{},{},{},{},{},{:0.2f},{:0.3f}\n",
            bar.date     ,
            bar.code     ,
            bar.open     ,
            bar.high     ,
            bar.low      ,
            bar.close    ,
            bar.pre_close,
            bar.change   ,
            bar.pct_chg  ,
            bar.volumn   ,
            bar.amount);
        output << ss;
    }
    output << "\n";
    output.close();

    LOGI("Gen:{}", filepath);
}

void csv_write_basic_file(const h5_basic &basic, const std::string &filepath)
{
    std::ofstream output{filepath};
    //
    //seq,ts_code,symbol,name,area,industry,market,list_date
    //0,000001.SZ,000001,平安银行,深圳,银行,主板,19910403
    //1,000002.SZ,000002,万科A,深圳,全国地产,主板,19910129

    output << "seq,ts_code,symbol,name,area,industry,market,list_date\n";
    for (int i = 0; i < basic.data[0].size(); i++) {
        std::string ss = fmt::format("{},{},{},{},{},{},{},{}\n",
            basic.data[FS_B_SEQ].at(i),
            basic.data[FS_B_CODE].at(i),
            basic.data[FS_B_SYMBOL].at(i),
            basic.data[FS_B_NAME].at(i),
            basic.data[FS_B_AREA].at(i),
            basic.data[FS_B_INDUSTRY].at(i),
            basic.data[FS_B_MARKET].at(i),
            basic.data[FS_B_LIST_DATE].at(i));
        output << ss;
    }
    output << "\n";
    output.close();
    LOGI("Gen:{}", filepath);
}

void csv_read_basic_file(h5_basic &basic, const std::string &filepath)
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
            int i = 0;
            basic.data[FS_B_SEQ].push_back(       one_row.at(i++) );
            basic.data[FS_B_CODE].push_back(      one_row.at(i++) );
            basic.data[FS_B_SYMBOL].push_back(    one_row.at(i++) );
            basic.data[FS_B_NAME].push_back(      one_row.at(i++) );
            basic.data[FS_B_AREA].push_back(      one_row.at(i++) );
            basic.data[FS_B_INDUSTRY].push_back(  one_row.at(i++) );
            basic.data[FS_B_MARKET].push_back(    one_row.at(i++) );
            basic.data[FS_B_LIST_DATE].push_back( one_row.at(i++) );
            //uprint::print("", info);
        }
    }
    return;
}

} // namespace fs
} // namespace noise