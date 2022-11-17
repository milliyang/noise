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

Backtest::Backtest(void)
{   
    m_func_get_stragegy = [](){ return std::make_shared<StrategyABC>(); };
}

Backtest::Backtest(FuncStrategy func)
{
    m_func_get_stragegy = func;
}

Backtest::~Backtest(void)
{
    //std::cout << __FUNCTION__ << std::endl;
}

void Backtest::make_strategy(int id, struct BtContext &ctx, const std::shared_ptr<Strategy> &strategy)
{
    ctx.broker = std::make_shared<Broker>();
    ctx.stat   = std::make_shared<Stat>();
    ctx.feed   = std::make_shared<FeedCSV>();
    //ctx.plot   = std::make_shared<Plot>();
    ctx.data   = std::make_shared<Data>();
    ctx.stat->init(ctx.broker);


    if (strategy.get() == nullptr) {
        ctx.strategy = m_func_get_stragegy();
    } else {
        ctx.strategy = strategy;
    }
    if (ctx.strategy) {
        ctx.strategy->set_id(id);
        ctx.strategy->init(ctx.broker);
        ctx.strategy->set_context(ctx);
    }

    //todo:
    struct feed_config cfg = m_config.feed;
    cfg.code = m_config.codes.at(id);
    ctx.feed->init(cfg);

    //after feed init
    ctx.data->init(ctx.broker, ctx.feed, m_config.plot);
    ctx.data->setup_plot(m_config.func.plot);

    TRACE_LINE
}

void Backtest::run_strategy(const struct BtContext &ctx)
{
    struct bar bar;
    bool update_data = ctx.feed->is_support_preload() ? false : true;

    ctx.strategy->on_start();

    ctx.strategy->on_bar_start();
    while(ctx.feed->next(bar)) {

        if (update_data) {
            //TODO:
            //ctx.data->update_default_indicator(bar);
        }
        ctx.broker->process(bar);
        ctx.strategy->next(bar);
        ctx.strategy->on_bar_recv(bar);
    }
    ctx.strategy->on_bar_stop();
    ctx.broker->close();
    ctx.stat->process();

    ctx.strategy->on_finish();
    ctx.stat->plot();
}

void Backtest::init(void)
{
    m_build_date == __DATE__;

    m_config.mode == BT_MODE_DEFAULT;

    std::shared_ptr<Strategy> first_strategy = m_func_get_stragegy();
    first_strategy->on_create(m_config);
    assert(m_config.codes.size() >= 1);

    if (m_config.codes.size() == 1) {
        m_config.mode = BT_MODE_DEFAULT;
    }

    int strategy_context_num;
    if (m_config.mode == BT_MODE_DEFAULT) {
        strategy_context_num = 1;
    } else {
        strategy_context_num = m_config.codes.size();
    }
    uprint::print("subscribe code", m_config.codes);

    for (int i = 0; i < strategy_context_num; i++) {
        struct BtContext ctx;
        if (i == 0) {
            make_strategy(i, ctx, first_strategy);
            m_context = ctx;
        } else {
            make_strategy(i, ctx, nullptr);
        }
        m_v_context.push_back(ctx);
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
     * 2. calc indicator (MACD,STDDEV)
     * 3. for bars:
     *      next()
     * 4. calc stat, PNL
     * 5. plot
     */

    init();

    if (m_config.mode == BT_MODE_DEFAULT) {
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