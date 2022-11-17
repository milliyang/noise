#include "noise/noise.h"
#include "cd.h"

#ifdef CONFIG_QPLOT
#include "qplot.h"
#elif defined(CONFIG_GNUPLOT)
#include "gnuplot.h"
#endif

StrategyCD::StrategyCD(void)
{
    TRACE_LINE
}

StrategyCD::~StrategyCD(void)
{
    TRACE_LINE
}

void StrategyCD::on_create(struct noise::bt_config &cfg)
{
    TRACE_LINE;
    /**
     * TODO:
     *   - init backtest framework
     *   - register user backtest code
     *   - init broker commission, style
     *   - register your ploting lib
     */
    //cfg.mode = noise::BT_MODE_ISOLATION;
    cfg.mode = noise::BT_MODE_DEFAULT;
    cfg.codes = {"000004.SZ"};
#if 0
    for (int i = 0; i < 10; i++) {
        cfg.codes.push_back("000001.SZ");
    }
#endif

    cfg.broker.commission = 0.002;
    cfg.broker.hedging = 1;
    cfg.broker.trade_on_close = 1;
    //
    //cfg.feed.root_path = "/home/leo/tmpfs/root";
    cfg.feed.root_path = "/home/leo/work/root";
    cfg.feed.root_path = "D:/root";
    //cfg.feed.filename  = "/home/leo/tmpfs/root/bars/000001.SZ.csv";

    cfg.plot.show_on_finish = 1;

#ifdef CONFIG_QPLOT
    cfg.func.plot = [](){ return std::make_shared<noise::QPlot>();};
#elif defined(CONFIG_GNUPLOT)
    cfg.func.plot = [](){ return std::make_shared<noise::GnuPlot>();};
#endif
}

void StrategyCD::on_destroy(void)
{
    TRACE_LINE;
}

void StrategyCD::on_start(void)
{
    TRACE_LINE;
    /**
     * TODO:
     *   - init indicator before the first bar in
     *   - do preload stock code
     */

    m_cnt = 0;
    m_preloaded_once = false;

    printf("on_start id:%d\n", m_id);
}

void StrategyCD::on_bar_start(void)
{
    TRACE_LINE;
    /**
     * TODO:
     *   - init indicator before the first bar in
     *   - do preload stock code
     */

    //Your indicator:
    m_ma5       = create_indicator("ma5");
    m_ma10      = create_indicator("ma10");
    m_stddev5   = create_indicator("stddev5");
    m_boll_high = create_indicator("boll_high");
    m_boll_mid  = create_indicator("boll_mid ");
    m_boll_low  = create_indicator("boll_low ");

    //Plot on another figure
    m_stddev5->figure = "stddev";

    //Get default indicator
    m_close = m_ctx.data->find_indicator(INDICATOR_CLOSE);

    //Handle Bar at batch when preload supported
    if (m_ctx.data->is_support_preload()) {
        update_indicator();
        m_preloaded_once = true;
    }
}

void StrategyCD::update_indicator(void)
{
    /* preload data only need to update indicator once */
    if (m_preloaded_once) { return; }

    noise::utalib::ma(m_ma5->data,  m_close->data, 5);
    noise::utalib::ma(m_ma10->data, m_close->data, 10);
    noise::utalib::stddev(m_stddev5->data, m_close->data, 10);
    noise::utalib::boll(m_boll_high->data, m_boll_mid->data, m_boll_low->data, m_close->data, 5, 2.0f);

#if 1
    //test copy indicator
    auto xx  = create_indicator("boll_low2");
    xx->figure = "stddev2";
    xx->data = m_boll_low->data;
#endif
}

void StrategyCD::prepare_data(const struct noise::bar &bar)
{
    if (m_cnt == 0) {
        m_code = bar.code;
    }
    update_indicator();
}

void StrategyCD::on_bar_recv(const struct noise::bar &bar)
{
    TRACE_TAG(bar.code.c_str());
    prepare_data(bar);

    {
        struct noise::order order;
        order.size = 100;
        send_order(order);
    }

    {
        struct noise::order order;
        order.size = -10;
        send_order(order);
    }

    m_cnt++;
    TRACE_NEWLINE;
}

void StrategyCD::on_bar_stop(void)
{
    TRACE_LINE;
}

void StrategyCD::on_finish(void)
{
    TRACE_LINE;

    // noise::uprint::print("m_close",         m_close);
    // noise::uprint::print("m_close_ma5",     m_ma5->data);
    // noise::uprint::print("m_close_ma10",    m_ma10->data);
    // noise::uprint::print("m_close_stddev5", m_stddev5->data);

    // test_plot0();
    plot();
}
