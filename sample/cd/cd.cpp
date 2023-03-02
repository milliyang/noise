#include "noise/noise.h"
#include "noise/indicator/ta.h"
#include "cd.h"

#ifdef CONFIG_QPLOT
#include "qplot.h"
#elif defined(CONFIG_GNUPLOT)
#include "gnuplot.h"
#endif

StrategyCD::StrategyCD(void)
{
}

StrategyCD::~StrategyCD(void)
{
}

void StrategyCD::on_create(struct noise::bt_config &cfg)
{
    /**
     * TODO:
     *   - init backtest framework
     *   - register user backtest code
     *   - init broker commission, style
     *   - register your ploting lib
     */
    cfg.mode = noise::BT_MODE_ONE2ONE;
    // cfg.codes = {"000004.SZ"};
    cfg.codes = {"300015.SZ"};
#if 0
    for (int i = 0; i < 10; i++) {
        cfg.codes.push_back("000001.SZ");
    }
#endif

    cfg.broker.init_cash = 100000.0f;
    cfg.broker.commission = 0.002f;
    cfg.broker.hedging = false;
    cfg.broker.trade_on_close = true;
    //
    //cfg.feed.root_path = "/home/leo/tmpfs/root";
    cfg.feed.root_path = "/home/leo/work/root";
    cfg.feed.root_path = "D:/root";
    cfg.feed.root_path = "P:/tmpfs/root";
    // cfg.feed.filename  = "/home/leo/tmpfs/root/bars/000001.SZ.csv";
    //cfg.feed.filename = "Z:\\work\\quant\\noise.cpp\\data\\300015.SZ.csv";
    // cfg.feed.filename  = "../data/300015.SZ.csv";
    cfg.feed.begin_date = "20220610";
    //cfg.feed.begin_date = "20250310";

    cfg.plot.show_on_finish = 1;

#ifdef CONFIG_QPLOT
    cfg.func.plot = [](){ return std::make_shared<noise::QPlot>();};
#elif defined(CONFIG_GNUPLOT)
    cfg.func.plot = [](){ return std::make_shared<noise::GnuPlot>();};
#endif
}

void StrategyCD::on_destroy(void)
{
}

void StrategyCD::on_start(void)
{
    index_ = -1;
    //ma5_        = new noise::ta::Ma(this, "ma5", 5);
    ma10_       = new noise::ta::Ma(this,       "ma10", 10);
    ma60_       = new noise::ta::Ma(this,       "ma60", 60);
    stddev10_   = new noise::ta::Stddev(this,   "stddev10", 10);
    boll_       = new noise::ta::Boll(this,     "BOLL");
    LOGI("on_start id:{}", m_id);
}

void StrategyCD::on_bar_start(void)
{
    series_close_ = m_ctx.data->find_series(INDICATOR_CLOSE);
}

void StrategyCD::prepare_data(const struct noise::bar &bar)
{
    index_++;
    if (index_ == 0) {
        code_ = bar.code;
    }
}

void StrategyCD::on_bar_recv(const struct noise::bar &bar)
{
    TRACE_TAG(bar.code.c_str());
    prepare_data(bar);

    if (broker_->is_in_position()) {
        in_pos_cnt_++;
        if (in_pos_cnt_ >= 5) {
            sell(1000);
        }
        return;
    }

#if 1
    if (!boll_->is_ready() || isnan(boll_->series_low_->get(index_))) {
        return;
    }
    time_t tt = series_close_->get_time(index_);

    if (noise::ta::crossover(series_close_, boll_->series_low_, index_, false)) {
        LOGI("date:{} boll buy close:{:.2f} boll_low:{:.2f}", noise::utime::fmt_time(tt), series_close_->get(index_), boll_->series_low_->get(index_));
        buy(1000);
        in_pos_cnt_ = 0;
    }

#endif

#if 0
    if (!ma10_->is_ready()) {
        return;
    }
    if (noise::ta::crossover(ma5_, ma10_, true)) {
        time_t tt = series_close_->get_time(index_);
        LOGI("date:{} gold ma10:{:.2f} ma5:{:.2f}", noise::utime::fmt_time(tt), ma10_->get(0), ma5_->get(0));
    } else if (noise::ta::crossover(ma5_, ma10_, false)) {
        time_t tt = series_close_->get_time(index_);
        LOGI("date:{} dead ma10:{:.2f} ma5:{:.2f}", noise::utime::fmt_time(tt), ma10_->get(0), ma5_->get(0));
    }
#endif

}

void StrategyCD::on_bar_stop(void)
{

}

void StrategyCD::on_finish(void)
{
    stddev10_->series_->figure = "figure2";

    // noise::uprint::print("m_close",         m_close);
    // noise::uprint::print("m_close_ma5",     m_ma5->data);
    // noise::uprint::print("m_close_ma10",    m_ma10->data);

    // test_plot0();
    plot();
}
