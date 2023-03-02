#include "noise/def.h"
#include "noise/backtest.h"
#include "noise/broker.h"
#include "noise/stat.h"
#include "noise/strategy.h"
#include "noise/feed.h"
#include "noise/util.h"

#ifdef __SUPPORT_OPENMP_TASK__
#include <omp.h>
#endif

#define OPENMP_CPU_CORE                  (6)

namespace noise {

Backtest::Backtest(void):
    m_build_date(__DATE__)
{   
    m_func_get_stragegy = [](){ return std::make_shared<StrategyABC>(); };
}

Backtest::Backtest(FuncStrategy func):
    m_build_date(__DATE__)
{
    m_func_get_stragegy = func;
}

Backtest::~Backtest(void)
{
    //std::cout << __FUNCTION__ << std::endl;
}

void Backtest::make_strategy(uint32_t id)
{
    struct BtContext ctx;
    ctx.broker = std::make_shared<Broker>();
    ctx.stat   = std::make_shared<Stat>();
    ctx.feed   = std::make_shared<FeedCSV>();
    //ctx.plot   = std::make_shared<Plot>();
    ctx.data   = std::make_shared<Data>();
    ctx.stat->init(ctx.broker, ctx.data);

    ctx.strategy = m_func_get_stragegy();
    ctx.strategy->set_id(id);
    ctx.strategy->init(ctx.broker);
    ctx.strategy->set_context(ctx);

    if (id == 0) {
        ctx.strategy->on_create(config_);
    }
    ctx.broker->init(config_.broker);

    //todo:
    struct feed_config cfg = config_.feed;
    cfg.code = config_.codes.at(id);
    ctx.feed->init(cfg);

    //after feed init
    ctx.data->setup(ctx.broker, ctx.feed, config_.plot);
    ctx.data->setup_plot(config_.func.plot);

    m_v_context.push_back(ctx);
    if (id == 0) { m_context = ctx; }

    TRACE_LINE
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
    ctx.stat->plot();
    LOGW("finish");
    ctx.strategy->on_finish();
}

void Backtest::init(void)
{
    config_.mode = BT_MODE_ONE2ONE;

    make_strategy(0);
    assert(m_v_context.size() == 1);


    int strategy_context_num;
    if (config_.mode == BT_MODE_ONE2ONE) {
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
    m_context.strategy->on_destroy();
}

/**
 * @brief run strategy
 */
void Backtest::run()
{
    struct bar bar;
    bar.time = 0;
    TRACE_LINE
    /**
     * @brief TODO:
     *
     * 0. load config
     * 1. load bars from feed
     * 2. calc series (MACD,STDDEV)
     * 3. for bars:
     *      next()
     * 4. calc stat, PNL
     * 5. plot
     */

    init();

    if (config_.mode == BT_MODE_ONE2ONE) {
        run_strategy(m_context);
    } else {
#ifdef __SUPPORT_OPENMP_TASK__
// #pragma omp parallel for num_threads(OPENMP_CPU_CORE)
#pragma omp parallel for
#endif
        for (int i = 0; i < m_v_context.size(); i++) {
            run_strategy(m_v_context.at(i));
        }
    }

    deinit();
}


}