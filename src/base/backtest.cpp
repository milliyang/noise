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
#include "noise/math/position_tracker.h"

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

        if (position_tracker_enable_ && ctx.broker->is_in_position()) {
            position_tracker_.set(bar.date);
        }
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
 * 1. total summary
 * 2. histogram
 * 3. summary ploting
 */
void Backtest::post_process(void)
{
    if (context_vector_.size() <= 1) {
        return;
    }
    math::Histogram histo(64, true);

    //show total summary of all backtests
    
    struct stat status;
    memset(&status, 0, sizeof(struct stat));

    VecF vec_pnl;
    VecF avg_dura;
    float max_duration = 0;

    double equity_final = 0;

    for (int i = 0; i < context_vector_.size(); i++) {
        auto st = context_vector_[i].stat->get_stat();
        if (i == 0) {
            status.start_time   = st.start_time;
            status.end_time     = st.end_time;
        }
        status.PNL                  += st.PNL;
        status.trade_cnt            += st.trade_cnt;
        status.extra.win_trade_cnt  += st.extra.win_trade_cnt;
        equity_final                += st.equity_final;
        //
        status.best_trade_on_PNL  = std::max(status.best_trade_on_PNL,  st.best_trade_on_PNL);
        status.worst_trade_on_PNL = std::min(status.worst_trade_on_PNL, st.worst_trade_on_PNL);
        status.max_trade_duration = std::max(status.max_trade_duration, st.max_trade_duration);
        status.equity_peak        = std::max(status.equity_peak,        st.equity_peak);

        status.extra.code_cnt++;

        if (st.avg_trade_duration > 0) {
            avg_dura.push_back(st.avg_trade_duration);
        }

        const auto broker = context_vector_[i].broker;

        const auto trades = broker->get_trades();
        for (auto it = trades.begin(); it != trades.end(); ++it) {
            const struct trade& trade = *it;
            vec_pnl.push_back(trade.PNL);

            histo.push(trade.profit()*100);

            //
            position_tracker_.set_pnl(utime::get_date(trade.exit_time), trade.PNL);
        }
    }
    status.avg_trade_duration = (float) umath::mean(avg_dura);

    status.equity_final  = (float) (equity_final / context_vector_.size());
    status.win_ratio     = (float) status.extra.win_trade_cnt / (float) status.trade_cnt;

    status.SQN = sqrtf((float)vec_pnl.size()) * umath::mean(vec_pnl) / umath::stddev(vec_pnl);
    uprint::print("All Code", status);

    histo.print();

    //Global ploting
    {
        struct feed_config feed_cfg = config_.feed;
        if (config_.codes.size() > 0) {
            feed_cfg.code = config_.codes.at(0);    //SZZS where always active on ALL trade date
        }
        auto feed = mcache::get_feed(feed_cfg);

        std::shared_ptr<Data> data   = std::make_shared<Data>();
        data->setup(nullptr, feed, config_.plot);
        data->setup_plot(config_.func.plot);
        data->process_preload();

        // Total trade (N) at the same time.  base = N * 5W
        //    equity_peak   (at date)
        //    equity        (at date)
        //    PNL           (at date)
        //
        auto a_series = data->create_series(INDICATOR_TRADE);
        auto ptrades = std::make_shared<struct tradeseries>();
        a_series->extra.ptrades = ptrades;
        a_series->figure = "Gain & Loss";
        a_series->name = "Cash";
        a_series->sign = "triangle";

        int   max_in_pos = position_tracker_.max();
        float avg_in_pos = position_tracker_.average();
        float curr_cash = max_in_pos * config_.broker.init_cash;
        LOGI("MaxOnPositionTheSameDay:{} Avg:{}", max_in_pos, avg_in_pos);

        auto dddate = data->find_series(INDICATOR_DATE);

        for (int i = 0; i < dddate->data.size(); i++) {
            int date = utime::get_date((time_t)dddate->data[i]);
            float pnl = position_tracker_.get_pnl(date);
            curr_cash += pnl;
            a_series->data.push_back(curr_cash);
        }

        data->plot();
    }

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
        position_tracker_enable_ = context_vector_.size() > 1 ? true : false;
#ifdef __SUPPORT_OPENMP_TASK__
// #pragma omp parallel for num_threads(OPENMP_CPU_CORE)
#pragma omp parallel for
#endif
        for (int i = 0; i < context_vector_.size(); i++) {
            run_strategy(context_vector_.at(i));
        }
    }
    LOGI("finish");

    post_process();
    deinit();
}

}