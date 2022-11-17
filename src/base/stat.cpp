#include "noise/def.h"
#include "noise/stat.h"
#include "noise/util.h"
#include "noise/broker.h"

namespace noise {

Stat::Stat(void)
{
    TRACE_LINE
}

Stat::~Stat(void)
{
    TRACE_LINE
}

/*
Start                     2004-08-19 00:00:00
End                       2013-03-01 00:00:00
Duration                   3116 days 00:00:00
Exposure Time [%]                       94.27
Equity Final [$]                     68935.12
Equity Peak [$]                      68991.22
Return [%]                             589.35
Buy & Hold Return [%]                  703.46
Return (Ann.) [%]                       25.42
Volatility (Ann.) [%]                   38.43
Sharpe Ratio                             0.66
Sortino Ratio                            1.30
Calmar Ratio                             0.77
Max. Drawdown [%]                      -33.08
Avg. Drawdown [%]                       -5.58
Max. Drawdown Duration      688 days 00:00:00
Avg. Drawdown Duration       41 days 00:00:00
# Trades                                   93
Win Rate [%]                            53.76
Best Trade [%]                          57.12
Worst Trade [%]                        -16.63
Avg. Trade [%]                           1.96
Max. Trade Duration         121 days 00:00:00
Avg. Trade Duration          32 days 00:00:00
Profit Factor                            2.13
Expectancy [%]                           6.91
SQN                                      1.78
_strategy              SmaCross(n1=10, n2=20)
_equity_curve                          Equ...
_trades                       Size  EntryB...
dtype: object
 */
void Stat::plot(void)
{
    TRACE_LINE

    uprint::print("Stat", m_stat);
}

void Stat::process(void)
{
    TRACE_LINE

    const auto trades = m_broker->get_trades();
    if (trades.size() <= 0) { return; }

    struct bar first_bar, last_bar;
    m_broker->get_first_and_last_bars(first_bar,last_bar);

    float enquity_base = 10000;
    float enquity_cur  = enquity_base;
    int win_cnt = 0;

    float best_trade_on_pnl,   worst_trade_on_pnl;
    float best_trade_on_ratio, worst_trade_on_ratio;
    float avg_profit_ratio = 0;
    std::vector<float> vec_pnl;
    std::vector<float> vec_duration;
    std::vector<int> vec_exposure;
    float max_duration = 0;

    const auto &the_1st_trade = *trades.begin();
    best_trade_on_pnl = worst_trade_on_pnl = the_1st_trade.PNL;
    best_trade_on_ratio = worst_trade_on_ratio = (the_1st_trade.exit_price - the_1st_trade.entry_price) / the_1st_trade.entry_price;

    m_stat.start_time = first_bar.time;
    m_stat.end_time = last_bar.time;

    //printf("first_bar.time :%d\n", first_bar.time);
    //printf("last_bar.time  :%d\n", last_bar.time);

    //m_stat.duration = utime::get_duration(first_bar.time, last_bar.time);
    m_stat.duration = utime::get_duration(last_bar.time - first_bar.time);
    vec_exposure.resize(m_stat.duration);

    for (auto it = trades.begin(); it != trades.end(); ++it) {
        const struct trade& trade = *it;

        // if (m_stat.start_time > trade.entry_time) {
        //     m_stat.start_time = trade.entry_time;
        // }
        // if (m_stat.end_time < trade.exit_time) {
        //     m_stat.end_time = trade.exit_time;
        // }
        m_stat.PNL += trade.PNL;
        enquity_cur += trade.PNL;

        if (m_stat.equity_peak < enquity_cur) {
            m_stat.equity_peak = enquity_cur;
        }
        if (trade.PNL > 0) {
            win_cnt++;
        }
        if (best_trade_on_pnl < trade.PNL) {
            best_trade_on_pnl = trade.PNL;
        }
        if (worst_trade_on_pnl > trade.PNL) {
            worst_trade_on_pnl = trade.PNL;
        }

        float profit_ratio = (trade.exit_price - trade.entry_price) / trade.entry_price;
        if (best_trade_on_ratio < profit_ratio) {
            best_trade_on_ratio = profit_ratio;
        }
        if (worst_trade_on_ratio > profit_ratio) {
            worst_trade_on_ratio = profit_ratio;
        }
        avg_profit_ratio += profit_ratio;

        float dura = trade.exit_time - trade.entry_time;
        if (max_duration < dura) {
            max_duration = dura;
        }

        vec_pnl.push_back(trade.PNL);
        vec_duration.push_back(dura);

#if 0
        int dura_offset = trade.entry_time - first_bar.time;
        int dura_days = trade.exit_time - trade.entry_time;
        for (int i = 0; i <= dura_days; i++) {
            if (dura_offset+i >= vec_exposure.size()) {
                printf("[err] dura_offset+i:%d %d\n", dura_offset, i);
                break;
            }
            vec_exposure.at(dura_offset+i) = 1;
        }
#endif
    }
    int exposion_days = umath::sum(vec_exposure);
    m_stat.exposure_time = (float) exposion_days / (float) m_stat.duration;

    //
    m_stat.trade_cnt = trades.size();
    m_stat.equity_final = enquity_base+m_stat.PNL;
    m_stat.return_final = m_stat.PNL / enquity_base;
    m_stat.return_buy_and_hold = (last_bar.close - first_bar.open) / first_bar.open;
    m_stat.win_ratio = (float) win_cnt / (float)trades.size();
    m_stat.best_trade_on_PNL = best_trade_on_pnl;
    m_stat.worst_trade_on_PNL = worst_trade_on_pnl;
    m_stat.best_trade = best_trade_on_ratio;
    m_stat.worst_trade = worst_trade_on_ratio;
    m_stat.avg_trade = avg_profit_ratio / trades.size();

    //convert second to duration
    m_stat.avg_trade_duration = umath::mean(vec_duration);
    m_stat.max_trade_duration = max_duration;

    m_stat.avg_trade_duration = utime::get_duration(m_stat.avg_trade_duration);
    m_stat.max_trade_duration = utime::get_duration(m_stat.max_trade_duration);

    //
    m_stat.SQN = sqrtf(trades.size()) * umath::mean(vec_pnl) / umath::stddev(vec_pnl);
    //s.loc['SQN'] = np.sqrt(n_trades) * pl.mean() / (pl.std() or np.nan)


    //update PNL indicator

}

void Stat::init(std::shared_ptr<Broker> broker)
{
    m_broker = broker;
    memset(&m_stat, 0, sizeof(m_stat));
}


}