#include "noise/def.h"
#include "noise/strategy.h"
#include "noise/broker.h"
#include "noise/util.h"

namespace noise {


void uprint::print(const char *tag, const struct trade &trade)
{
    float ratio = 0;
    int exit_date = 0;

    if (trade.is_closed() > 0) {
        exit_date = utime::get_date(trade.exit_time);
        ratio = (trade.exit_price - trade.entry_price) / trade.entry_price;
    }

    printf("[%s] code:%s size:%5d entry(%.2f %d) exit(%.2f %d) PNL:%+7.2f profit:%+7.2f%%\n",
        tag,
        trade.code.c_str(),
        trade.size,
        trade.entry_price,
        utime::get_date(trade.entry_time),
        trade.exit_price,
        exit_date,
        trade.PNL,
        ratio*100);
}

void uprint::print(const char*tag, const std::list<struct trade> &trades)
{
    for (auto it = trades.begin(); it != trades.end(); ++it) {
        print(tag, *it);
    }
}

void uprint::print(const char*tag, const std::vector<struct trade> &trades)
{
    for (auto it = trades.begin(); it != trades.end(); ++it) {
        print(tag, *it);
    }
}

void uprint::print(const char*tag, const std::vector<float> &values)
{
    for (auto it = values.begin(); it != values.end(); ++it) {
        printf("[%s] %f\n",
            tag,
            *it);
    }
}

void uprint::print(const char*tag, const std::vector<std::string> &strings)
{
    for (auto it = strings.begin(); it != strings.end(); ++it) {
        printf("[%s] %s\n",
            tag,
            it->c_str());
    }
}

void uprint::print(const char *tag, const struct order &order)
{
    printf("[%s] code:%s size:%5d price:%.2f type:%d\n",
        tag,
        order.code.c_str(),
        order.size,
        order.price,
        order.type);
}

void uprint::print(const char *tag, const struct bar &bar)
{
#if 0
    printf("[%s] date:%d, %s, ohlc:%7.3f,%7.3f,%7.3f,%7.3f, %0.2f,%0.2f,%0.2f,%0.2f, amount:%0.2f\n",
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
    printf("[%s] date:%d, %s, ohlc:%7.3f,%7.3f,%7.3f,%7.3f, prev_close:%7.3f\n",
        tag,
        bar.date,
        bar.code.c_str(),
        bar.open       ,
        bar.high       ,
        bar.low        ,
        bar.close      ,
        bar.pre_close);
#endif
}

void uprint::print(const char *tag, const struct stat &stat)
{
    #define PERCENT (100.0f)

    printf("[%s] summary:\n", tag);
    printf("  Start                      [YYYMMDD] %6d\n",      utime::get_date(stat.start_time));
    printf("  End                        [YYYMMDD] %6d\n",      utime::get_date(stat.end_time));
    printf("  Duration                       [days]  %6d\n",    stat.duration);
    printf("  Exposure Time                      [%%] %6.2f\n", stat.exposure_time * PERCENT);
    printf("  Equity Final                  [$]  %+10.2f\n",    stat.equity_final);
    printf("  Equity Peak                   [$]  %+10.2f\n",    stat.equity_peak);
    printf("  PNL                           [$]  %+10.2f\n",    stat.PNL);
    printf("  Return                             [%%] %6.2f\n", stat.return_final * PERCENT);
    printf("  Buy & Hold Return                  [%%] %6.2f\n", stat.return_buy_and_hold * PERCENT);
    printf("  Return (Ann.)                      [%%] %6.2f TBD\n", stat.return_annual * PERCENT);
    printf("  Volatility (Ann.)                  [%%] %6.2f TBD\n", stat.volatility_annual * PERCENT);
    printf("  Sharpe Ratio                           %6.2f TBD\n",  stat.sharpe_ratio);
    //printf(" Sortino Ratio                           %6.2f\n", stat.sortino_ratip);
    //printf(" Calmar Ratio                            %6.2f\n", stat.calmar_ratip);
    printf("  Max. Drawdown                      [%%] %6.2f TBD\n", stat.max_drawdown * PERCENT);
    printf("  Avg. Drawdown                      [%%] %6.2f TBD\n", stat.avg_drawdown * PERCENT);
    printf("  Max. Drawdown Duration         [days]  %6.2f TBD\n", stat.max_drawdown_duration);
    printf("  Avg. Drawdown Duration         [days]  %6.2f TBD\n", stat.avg_drawdown_duration);
    printf("  Trades                                 %6d\n",    stat.trade_cnt);
    printf("  Win Rate                           [%%] %6.2f\n", stat.win_ratio   * PERCENT);
    printf("  Avg. Trade                         [%%] %6.2f\n", stat.avg_trade   * PERCENT);
    printf("  Best. Trade                        [%%] %6.2f\n", stat.best_trade  * PERCENT);
    printf("  Wors. Trade                        [%%] %6.2f\n", stat.worst_trade * PERCENT);
    printf("  Best. Trade on PNL             [$] %+10.2f\n",     stat.best_trade_on_PNL);
    printf("  Wors. Trade on PNL             [$] %+10.2f\n",     stat.worst_trade_on_PNL);
    printf("  Max. Trade Duration           [days] %8.2f\n",  stat.max_trade_duration);
    printf("  Avg. Trade Duration           [days] %8.2f\n",  stat.avg_trade_duration);
    //printf(" Profit Factor                     [%%] %6.2f\n", stat.profit_factor);
    //printf(" Expectancy                          %6.2f\n", stat.expectancy);
    printf("  SQN                                    %6.2f\n",  stat.SQN);
    printf("\n");
}

void uprint::print(const char *tag, time_t time)
{
    //YYYY-MM-DD HH:mm:ss
    std::string ss = utime::fmt_time_long(time);
    printf("[%s] time:%s\n", tag, ss.c_str());
}

}