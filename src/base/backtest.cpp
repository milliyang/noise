#define LOG_TAG "backtest"

#include "noise/def.h"
#include "noise/backtest.h"
#include "noise/broker.h"
#include "noise/stat.h"
#include "noise/strategy.h"
#include "noise/feed/feed.h"
#include "noise/util.h"
#include "noise/args.h"
#include "noise/mcache.h"

#include "noise/math/histogram.h"

#ifdef __SUPPORT_OPENMP_TASK__
#include <omp.h>
#endif

#define OPENMP_CPU_CORE                  (6)

namespace noise {

Backtest::Backtest(void):
    build_date_(__DATE__)
{   
    func_get_strategy_ = [](){ return std::make_shared<StrategyABC>(); };
}

Backtest::Backtest(FuncStrategy func)
    : build_date_(__DATE__)
{
    func_get_strategy_ = func;
}

Backtest::~Backtest(void)
{
}

void Backtest::make_strategy(uint32_t id)
{
    struct BtContext ctx;
    ctx.broker = std::make_shared<Broker>();
    ctx.stat   = std::make_shared<Stat>();
    ctx.data   = std::make_shared<Data>();
    ctx.stat->init(ctx.broker, ctx.data);

    ctx.strategy = func_get_strategy_();
    ctx.strategy->set_id(id);
    ctx.strategy->init(ctx.broker);
    //ctx.strategy->set_context(ctx);

    if (id == 0) {
        ctx.strategy->on_create(config_);
    }
    ctx.broker->init(config_.broker);

    struct feed_config feed_cfg = config_.feed;
    if (config_.codes.size() > 0) {
        feed_cfg.code = config_.codes.at(id);
    }
    ctx.feed   = mcache::get_feed(feed_cfg);

    //after feed init
    ctx.data->setup(ctx.broker, ctx.feed, config_.plot);
    ctx.data->setup_plot(config_.func.plot);

    ctx.strategy->set_code(config_.codes.at(id));
    ctx.strategy->set_context(ctx);
    
    context_vector_.push_back(ctx);
    if (id == 0) {
        context_ = ctx;
    }
}

void Backtest::run_strategy(const struct BtContext &ctx)
{
    struct bar bar;

    ctx.strategy->on_start();
    ctx.data->process_preload();

    ctx.strategy->on_bar_start();
    while(ctx.feed->next(bar)) {
        ctx.data->update_data_series(bar);
        ctx.broker->process(bar);
        ctx.strategy->next(bar);
        ctx.strategy->on_bar_recv(bar);
        ctx.stat->on_trade_day();
    }
    ctx.strategy->on_bar_stop();
    ctx.broker->on_finish();
    ctx.stat->on_finish();
    if (Strategy::stat_on_finish_) {
        ctx.stat->plot();
    }
    ctx.strategy->on_finish();
}

/**
 * @brief create strategy
 */
void Backtest::init(void)
{
    config_.mode = BT_MODE_ONE2ONE;

    make_strategy(0);
    assert(context_vector_.size() == 1);

    int strategy_context_num;
    if (config_.mode == BT_MODE_M2ONE) {
        strategy_context_num = 1;
    } else {
        strategy_context_num = (int) config_.codes.size();
    }
    uprint::print("subscribe code", config_.codes);

    for (int i = 1; i < strategy_context_num; i++) {
        make_strategy(i);
    }
}

void Backtest::deinit(void)
{
    context_.strategy->on_destroy();
}

/**
 * @brief run backtest
 */
void Backtest::run()
{
    init();

    if (config_.mode == BT_MODE_M2ONE) {
        run_strategy(context_);
    } else {
#ifdef __SUPPORT_OPENMP_TASK__
// #pragma omp parallel for num_threads(OPENMP_CPU_CORE)
#pragma omp parallel for
#endif
        for (int i = 0; i < context_vector_.size(); i++) {
            run_strategy(context_vector_.at(i));
        }
    }
    LOGI("finish");

    math::Histogram histo(64);

    //show total summary of all backtests
    if (context_vector_.size() > 1) {
        struct stat status;
        memset(&status, 0, sizeof(struct stat));

        VecF vec_pnl;

        for (int i = 0; i < context_vector_.size(); i++) {
            auto st = context_vector_[i].stat->get_stat();
            if (i == 0) {
                status.start_time   = st.start_time;
                status.end_time     = st.end_time;
            }
            status.PNL                  += st.PNL;
            status.trade_cnt            += st.trade_cnt;
            status.extra.win_trade_cnt  += st.extra.win_trade_cnt;
            status.equity_final         += st.equity_final;
            //
            status.best_trade_on_PNL  = std::max(status.best_trade_on_PNL, st.best_trade_on_PNL);
            status.worst_trade_on_PNL = std::min(status.worst_trade_on_PNL, st.worst_trade_on_PNL);
            status.equity_peak        = std::max(status.equity_peak, st.equity_peak);
            status.extra.code_cnt++;

            const auto broker = context_vector_[i].broker;

            const auto trades = broker->get_trades();
            for (auto it = trades.begin(); it != trades.end(); ++it) {
                const struct trade& trade = *it;
                vec_pnl.push_back(trade.PNL);

                histo.push(trade.profit()*100);
            }
        }

        status.equity_final /= (float) context_vector_.size();
        status.win_ratio     = (float) status.extra.win_trade_cnt / (float) status.trade_cnt;

        status.SQN = sqrtf((float)vec_pnl.size()) * umath::mean(vec_pnl) / umath::stddev(vec_pnl);
        uprint::print("All Code", status);

        histo.print();
    }

    deinit();
}


}
