#pragma once

#include <string>
#include <stdint.h>
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <assert.h>
#include <chrono>
#include <fstream>
// #include <ifstream>
#include <sstream>
#include <string.h>
#include <math.h> 
#include <ctime>
#include <functional>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>


#if 0
#define TRACE_LINE    printf("; %s:%d func:%s()\n", __FILE__, __LINE__, __FUNCTION__);
#define TRACE_NEWLINE printf("; \n");
#define TRACE_TAG(ss) printf("; %s:%d func:%s() msg:%s\n", __FILE__, __LINE__, __FUNCTION__, ss);
#else
#define TRACE_LINE
#define TRACE_NEWLINE
#define TRACE_TAG(ss)
#endif


#if ((_WIN32) || (_WIN64))

#define isnanf _isnanf
#define localtime_r(a,b) localtime_s(b,a)

#endif


#define LOGD printf

#define INDICATOR_DATE      "_date"
#define INDICATOR_OPEN      "_open"
#define INDICATOR_HIGH      "_high"
#define INDICATOR_LOW       "_low"
#define INDICATOR_CLOSE     "_close"
#define INDICATOR_VOLUME    "_volume"
#define INDICATOR_CHANGES   "_changes"
#define INDICATOR_PNL       "_PNL"

#define FIGURE_DEFAULT      "default"
#define AUTO                "auto"

#define MAX(a,b)            std::max((a),(b))
#define MIN(a,b)            std::min((a),(b))
#define CLAMP(a,l,h)        MIN(MAX(a,l), h)

//#define CLAMP(a,b,c)        std::clamp((a),(b),(c))   //fxxk c++17

namespace noise {

struct code_info {
    std::string code;           //"000001.SZ", unique | security
    std::string symbol;         //"000001"
    std::string name;           //"平安银行"
    std::string area;           //"深圳"
    std::string industry;       //"银行"
    std::string market;         //"主板"
    int list_date;              //[YYYYMMDD] 19910403
};

struct bar {
    int    date;                //[YYYYMMDD] 20190901
    std::string code;
    float open;
    float high;
    float low;
    float close;
    float pre_close;
    float change;
    float pct_chg;
    float volumn;
    float amount;
    //
    time_t time;         //seconds from "00:00:00 GMT, Jan 1, 1970"; now, most PC: sizeof(time_t) == 8
};

enum order_type {
    ORDER_T_MARKET = 0,
    ORDER_T_LIMITED,
    ORDER_T_STOP,
    ORDER_T_STOP_LIMITED,
    ORDER_T_TAKE_PROFIT,
};

struct order {
    int         type;
    std::string code;
    int         size;
    float       price;

    //update by broker
    uint32_t    id;
    int         date;
    time_t      time;
    float       trade_price;

    bool is_long(void)  const { return size > 0 ? true:false; }
    bool is_short(void) const { return size < 0 ? true:false; }
};

struct trade {
    std::string code;
    int         size;
    float       entry_price;
    time_t      entry_time;
    float       exit_price;
    time_t      exit_time;
    float       PNL; //Profit-And-Loss when closed
    bool        hedging;

    bool is_long(void)  const { return size > 0 ? true:false; }
    bool is_short(void) const { return size < 0 ? true:false; }
    bool is_closed(void) const { return exit_time > 0 ? true:false; }
    void set_closed(time_t bar_time) { exit_time = bar_time; }
    float profit (void) const {
        float ratio = (exit_price - entry_price) / entry_price;
        if (size < 0) { ratio *=-1.0f; }
        return ratio;
    }
};

struct stat {
    time_t  start_time;
    time_t  end_time;
    int     duration;
    //
    float exposure_time;            //[%]
    float equity_final;
    float equity_peak;
    float PNL;
    float return_final;             //[%]
    float return_buy_and_hold;      //[%]
    float return_annual;            //[%]
    float volatility_annual;        //[%]
    float sharpe_ratio;
    //float sortino_ratip;
    //float calmar_ratip;
    float max_drawdown;             //[%]
    float avg_drawdown;             //[%]
    float max_drawdown_duration;
    float avg_drawdown_duration;
    //Trades
    int   trade_cnt;
    float win_ratio;                //[%]
    float best_trade;               //[%] (on profit ratio)
    float worst_trade;              //[%] (on profit ratio)
    float avg_trade;                //[%] (on profit ratio)
    float best_trade_on_PNL;        //[$]
    float worst_trade_on_PNL;       //[$]
    float max_trade_duration;
    float avg_trade_duration;
    //float profit_factor;
    //float expectancy;
    float SQN;
};

struct timeseries {
    std::vector<time_t> time;
};

//system default indicator name: _open,_high_,_low,_close,_volume,_changes,_date
struct indicator {
    std::string name;               //ma5,ma10,stddev;   don't use prefix: "_" 
    std::string figure;             //plotting figure name
    std::string color;              //r,g,b,red,yellow
    std::string gamma;              //linear,log,exp
    int shown_on_mouse;
    std::string sign;               //arrow,star,circle,rect, "-", "|" "--",
    std::vector<float> data;        //

    struct {
        std::shared_ptr<struct timeseries> ptime;
        std::string short_name;     //default is name[0:4];
    } extra;
};

using PtrIndicator  = std::shared_ptr<struct indicator>;

}