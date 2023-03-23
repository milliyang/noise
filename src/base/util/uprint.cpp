#define LOG_TAG "uprint"

#include "noise/def.h"
#include "noise/strategy.h"
#include "noise/broker.h"
#include "noise/util.h"
#include "spdlog/fmt/fmt.h"
#include "noise/log.h"


namespace noise {

#if 0
#define DEBUG_OFF   return;
#else
#define DEBUG_OFF
#endif

static std::string format_trade(const struct trade &trade)
{
    float ratio = 0.0f;
    int exit_date = 0;

    if (trade.is_closed()) {
        exit_date = utime::get_date(trade.exit_time);
        ratio = trade.profit();
    }

    auto result = fmt::format("{} size:{:6d} entry({:0.2f} {:d}) exit({:0.2f} {:d}) PNL:{:+7.2f} profit:{:+7.2f}%",
        trade.code,
        trade.size,
        trade.entry_price,
        utime::get_date(trade.entry_time),
        trade.exit_price,
        exit_date,
        trade.PNL,
        ratio*100);

    return result;
}

void uprint::print(const char *tag, const struct trade &trade)
{
    DEBUG_OFF
    LOGD("[{}] {}", tag, format_trade(trade));
}

void uprint::print(const char*tag, const std::list<struct trade> &trades)
{
    char subtag[10];
    int idx = 0;
    LOGD("{}:", tag);
    for (auto it = trades.begin(); it != trades.end(); ++it) {
        snprintf(subtag, 10, "%3d", idx++);
        print(subtag, *it);
    }
}

void uprint::print(const char*tag, const std::vector<struct trade> &trades)
{
    char subtag[10];
    int idx = 0;
    LOGD("{}:", tag);
    for (auto it = trades.begin(); it != trades.end(); ++it) {
        snprintf(subtag, 10, "%3d", idx++);
        print(subtag, *it);
    }
}

void uprint::print(const char*tag, const VecF &values)
{
    for (auto it = values.begin(); it != values.end(); ++it) {
        LOGD("[{}] {}", tag, *it);
    }
}

void uprint::print(const char*tag, const std::vector<std::string> &strings)
{
    for (auto it = strings.begin(); it != strings.end(); ++it) {
        LOGD("[{}] {}", tag, *it);
    }
}

void uprint::print(const char *tag, const struct order &order)
{
    DEBUG_OFF

    LOGD("[{}] {} size:{:6d} price:{:0.2f} type:{}",
        tag,
        order.code,
        order.size,
        order.price,
        order.type);
}

void uprint::print(const char *tag, const struct code_info &info)
{
    DEBUG_OFF
    LOGD("[{}] {} {} {} {} {} {} {} {}",
        tag,
        info.seq      ,
        info.code     ,
        info.symbol   ,
        info.name     ,
        info.area     ,
        info.industry ,
        info.market   ,
        info.list_date);
}

void uprint::print(const char *tag, const struct bar &bar)
{
    DEBUG_OFF

#if 0
    LOGD("[%s] date:%d, %s, ohlc:%7.3f,%7.3f,%7.3f,%7.3f, %0.2f,%0.2f,%0.2f,%0.2f, amount:%0.2f",
        tag,
        bar.date,
        bar.code.c_str(),
        bar.open       ,
        bar.high       ,
        bar.low        ,
        bar.close      ,
        bar.pre_close  ,
        bar.change     ,
        bar.pct_chg    ,
        bar.volumn     ,
        bar.amount);
#else
    LOGD("[{}] {} {} ohlc:{:7.3f},{:7.3f},{:7.3f},{:7.3f}, prev_close:{:7.3f}",
        tag,
        bar.date,
        bar.code,
        bar.open       ,
        bar.high       ,
        bar.low        ,
        bar.close      ,
        bar.pre_close);
#endif
}

static const char *get_sqn_desc(float SQN)
{
    // 0.00 ~ 1.5:     Probably Very Hard To Trade.
    // 1.51 ~ 2.0:     Average System (Should Be At Least 1.7).
    // 2.01 ~ 3.00:    Good System.
    // 3.01 ~ 5.00:    Excellent System.
    // 5.01 ~ 7.00:    Super System (Few Exist).
    // 7.01 Or Higher: Holy Grail System.
    if (SQN <= 1.5 || isnan(SQN)) {
        return "Hard To Trade   (---)";
    } else if (SQN <= 1.7) {
        return "Average System  (-- )";
    } else if (SQN <= 2.0) {
        return "Average System  (+  )";
    } else if (SQN <= 3.0) {
        return "Good System     (++ )";
    } else if (SQN <= 5.0) {
        return "Excellent       (+++)";
    } else if (SQN <= 7.0) {
        return "Superb          (+++)";
    } else {
        return "Holy Grail      (+++)";
    }
}

void uprint::print(const char *tag, const struct stat &stat)
{
    #define PERCENT (100.0f)

    LOGI("[{}] summary:", tag);
    LOGI("  Start                      [YYYMMDD]    {:6d}",          utime::get_date(stat.start_time));
    LOGI("  End                        [YYYMMDD]    {:6d}",          utime::get_date(stat.end_time));
    LOGI("  Duration                      [days]      {:6d}",        stat.duration);
    LOGI("  Exposure Time                    [%]      {:6.2f}",      stat.exposure_time * PERCENT);
    LOGI("  Equity Final                     [$] {:+11.2f}",         stat.equity_final);
    LOGI("  Equity Peak                      [$] {:+11.2f}",         stat.equity_peak);
    LOGI("  PNL                              [$] {:+11.2f}",         stat.PNL);
    LOGI("  Return                           [%]     {:7.2f}",       stat.return_final * PERCENT);
    LOGI("  Buy & Hold Return                [%]     {:7.2f}",       stat.return_buy_and_hold * PERCENT);
    LOGI("  Return     (Ann.)                [%]     {:7.2f} TBD",   stat.return_annual * PERCENT);
    LOGI("  Volatility (Ann.)                [%]     {:7.2f} TBD",   stat.volatility_annual * PERCENT);
    LOGI("  Sharpe Ratio                             {:7.2f} TBD",   stat.sharpe_ratio);
    //printf(" Sortino Ratio                             {:7.2f}", stat.sortino_ratip);
    //printf(" Calmar Ratio                              {:7.2f}", stat.calmar_ratip);
    LOGI("  Max. Drawdown                    [%]     {:7.2f} TBD",   stat.max_drawdown * PERCENT);
    LOGI("  Avg. Drawdown                    [%]     {:7.2f} TBD",   stat.avg_drawdown * PERCENT);
    LOGI("  Max. Drawdown Duration        [days]     {:7.2f} TBD",   stat.max_drawdown_duration);
    LOGI("  Avg. Drawdown Duration        [days]     {:7.2f} TBD",   stat.avg_drawdown_duration);
    LOGI("  ******");
    LOGI("  Trades                                    {:6d}",        stat.trade_cnt);
    LOGI("  Win Rate                         [%]     {:7.2f}",       stat.win_ratio   * PERCENT);
    LOGI("  Avg.  Trade                      [%]     {:7.2f}",       stat.avg_trade   * PERCENT);
    LOGI("  Best. Trade                      [%]     {:7.2f}",       stat.best_trade  * PERCENT);
    LOGI("  Wors. Trade                      [%]     {:7.2f}",       stat.worst_trade * PERCENT);
    LOGI("  Best. Trade on PNL               [$]  {:+10.2f}",        stat.best_trade_on_PNL);
    LOGI("  Wors. Trade on PNL               [$]  {:+10.2f}",        stat.worst_trade_on_PNL);
    LOGI("  Max.  Trade Duration          [days]    {:8.2f}",        stat.max_trade_duration);
    LOGI("  Avg.  Trade Duration          [days]    {:8.2f}",        stat.avg_trade_duration);
    //printf(" Profit Factor                       [%] {:7.2f}", stat.profit_factor);
    //printf(" Expectancy                            {:7.2f}", stat.expectancy);
    LOGI("  SQN     {}            {:7.2f}",                   get_sqn_desc(stat.SQN), stat.SQN);


    //For grep
    LOGD("[grep] Return:{:7.2f}% B&H Return:{:7.2f}% SQN:{:7.2f} Trades:{:4d} Win Rate:{:7.2f}% PNL:{:+11.2f}$",
        stat.return_final * PERCENT,
        stat.return_buy_and_hold * PERCENT,
        stat.SQN,
        stat.trade_cnt,
        stat.win_ratio * PERCENT,
        stat.PNL);
}

void uprint::print(const char *tag, time_t time)
{
    //YYYY-MM-DD HH:mm:ss
    std::string ss = utime::fmt_time_long(time);
    printf("[%s] time:%s", tag, ss.c_str());
}

}