#include "noise/def.h"
#include "noise/strategy.h"
#include "noise/broker.h"
#include "noise/util.h"


namespace noise {


void util::parse_csv_bars_file(std::vector<struct bar> &v_bars, const std::string &filepath)
{
    std::ifstream input{filepath};

    //
    // trade_date,ts_code,open,high,low,close,pre_close,change,pct_chg,vol,amount
    // 20180102,000001.SZ,12.6805,13.2314,12.652,13.0129,12.633,0.379900000000001,3.0072,2081592.55,2856543.822
    // 20180103,000001.SZ,13.0414,13.1649,12.538,12.6615,13.0129,-0.35139999999999993,-2.7004,2962498.38,4006220.766

    if (!input.is_open()) {
        std::cerr << "Couldn't read file: " << filepath << "\n";
        return;
    }

    std::vector<std::vector<std::string>> csvRows;

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
        if (cnt == 0) {
            if (one_row.at(0) != "trade_date") {
                std::cerr << "Couldn't parse line: " << line << "\n";
                return;
            }
            column = one_row.size();
            //printf("bars column:%d\n", (int)one_row.size());
        } else {
            if (column != one_row.size()) {
                std::cerr << "ignore bar volumn:" << one_row.size() << " " << line << "\n";
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
            v_bars.push_back(bar);
        }
        cnt++;

        // if (cnt >= 100) {
        //     break;
        // }
    }
}


}