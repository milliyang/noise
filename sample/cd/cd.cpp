#define LOG_TAG "cd"

#include "noise/noise.h"
#include "noise/indicator/ta.h"
#include "noise/util.h"
#include "noise/log.h"
#include "noise/args.h"
#include "cd.h"

#ifdef CONFIG_QPLOT
#include "qplot.h"
#endif

using namespace noise;

#define MAX_CODE_NUM   (20)

StrategyCD::StrategyCD(void)
{
    ma5_ = nullptr;
    ma10_ = nullptr;
    ma60_ = nullptr;
    ma120_ = nullptr;
    stddev10_ = nullptr;
    boll_ = nullptr;
}

StrategyCD::~StrategyCD(void)
{

}

void StrategyCD::override_config(struct noise::bt_config &cfg)
{
    PtrArgs args = uargs::args0_get();
    if (args == nullptr) {
        return;
    }

    if (args->has(ARGS_PLOT)) {
        plot_on_finish_ = (args->geti(ARGS_PLOT) > 0) ? true : false;
    }
    if (args->has(ARGS_INIT_CASH)) {
        cfg.broker.init_cash = args->getf(ARGS_INIT_CASH);
    }

    if (args->has(ARGS_CODE_FILE) && !args->has(ARGS_CODE) ) {
        auto codefile = args->get(ARGS_CODE_FILE);
        std::vector<struct code_info> infos;
        // util::parse_code_file(infos, ".config.ini");
        // util::parse_code_file(infos, ".code.csv");
        util::parse_code_file(infos, codefile);
        cfg.codes.clear();
        for (auto iter = infos.begin(); iter != infos.end(); iter++) {
            cfg.codes.push_back(iter->code);
            if (cfg.codes.size() >= MAX_CODE_NUM) {
                break;
            }
        }
    }

    if (args->has(ARGS_CODE)) {
        cfg.codes.clear();
        auto code = args->get(ARGS_CODE);
        cfg.codes.push_back(code);
    }

    if (args->has(ARGS_BEGIN_DATE)) {
        cfg.feed.begin_date = args->get(ARGS_BEGIN_DATE); //"20250310";
    }

    uprint::print("codes", cfg.codes);
}

void StrategyCD::on_create(struct noise::bt_config &cfg)
{
    cfg.mode = noise::BT_MODE_ONE2ONE;
    cfg.codes = {"300015.SZ"};

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
    //cfg.feed.begin_date = "20220610";

    plot_on_finish_ = true;

#ifdef CONFIG_QPLOT
    cfg.func.plot = [](){ return std::make_shared<noise::QPlot>();};
#elif defined(CONFIG_GNUPLOT)
    cfg.func.plot = [](){ return std::make_shared<noise::GnuPlot>();};
#endif
    override_config(cfg);
}

void StrategyCD::on_destroy(void)
{

}

void StrategyCD::on_start(void)
{
    index_ = -1;
    ma5_        = new ta::Ma(this,      "ma5",   5);
    ma10_       = new ta::Ma(this,      "ma10",  10);
    ma60_       = new ta::Ma(this,      "ma60",  60);
    ma120_      = new ta::Ma(this,      "ma120", 120);
    stddev10_   = new ta::Stddev(this,  "stddev10", 15);
    boll_       = new ta::Boll(this,    "BOLL");
    LOGI("on_start id:{} code:{}", id_, context_.feed->config_.code);
}

void StrategyCD::on_bar_start(void)
{
    series_close_ = context_.data->find_series(INDICATOR_CLOSE);
}

void StrategyCD::prepare_data(const struct noise::bar &bar)
{
    index_++;
}

void StrategyCD::on_bar_recv(const struct noise::bar &bar)
{
    TRACE_TAG(bar.code.c_str());
    prepare_data(bar);

    if (!stddev10_->is_ready() || !ma60_->is_ready()) {
        return;
    }

    if (broker_->is_in_position()) {
        in_pos_cnt_++;
        float profit = get_profit();
        if (profit > max_profit_) {
            max_profit_ = profit;
        }
        float profit_max_drawdown = max_profit_ - profit;
        LOGD("profit:{}", profit);
        LOGD("profit_max_drawdown:{}", profit_max_drawdown);
        LOGD("in_pos_cnt_:{}", in_pos_cnt_);

        if (in_pos_cnt_ >= 40 ||
            profit <= -0.10f  ||
            (max_profit_ > 0 && profit_max_drawdown > 0.05f)) {
            sell(0);
        }
        return;
    }

#if 0
    if (/*stddev10_->get(0) <= 0.02f &&*/ ta::above(ma10_, ma60_)) {
        buy(1.0f);
        in_pos_cnt_ = 0;
        max_profit = 0;
    }
#endif

#if 1
    if (!boll_->is_ready()) {
        return;
    }
    time_t tt = series_close_->get_time(index_);

    //if (ta::crossover(series_close_, boll_->series_low_, index_, false)) {
    if (ta::crossover(series_close_, boll_->series_low_, index_, true)) {
        LOGD("date:{} boll buy close:{:.2f} boll_low:{:.2f}",
            utime::fmt_time(tt), series_close_->get(index_), boll_->series_low_->get(index_));
        buy(1.0f);
        in_pos_cnt_ = 0;
        max_profit_ = -1;
    }
#endif

#if 0
    if (!ma10_->is_ready()) {
        return;
    }
    if (ta::crossover(ma5_, ma10_, true)) {
        time_t tt = series_close_->get_time(index_);
        LOGD("date:{} gold ma10:{:.2f} ma5:{:.2f}"
            utime::fmt_time(tt), ma10_->get(0), ma5_->get(0));
    } else if (ta::crossover(ma5_, ma10_, false)) {
        time_t tt = series_close_->get_time(index_);
        LOGD("date:{} dead ma10:{:.2f} ma5:{:.2f}",
            utime::fmt_time(tt), ma10_->get(0), ma5_->get(0));
    }
#endif

}

void StrategyCD::on_bar_stop(void)
{

}

void StrategyCD::on_finish(void)
{
    stddev10_->series_->figure = "Stddev";

    // noise::uprint::print("m_close",         m_close);
    // noise::uprint::print("m_close_ma5",     m_ma5->data);
    // noise::uprint::print("m_close_ma10",    m_ma10->data);

    if (plot_on_finish_) {
        plot();
    }
}

int32_t main(int32_t argc, char** argv)
{
    noise::init();
    if (uargs::args0_parse(__FILE__, argc, argv) != 0) {
        return 0;
    }

    LOGI("Backtest run...");
    noise::Timelapsed time_performance(__FUNCTION__);
    noise::Backtest bt([](){
        return std::make_shared<StrategyCD>();
    });

    bt.run();
    return 0;
}

/**

MSBuild.exe noise.sln -m

.\Debug\bt.exe -c 300014.SZ -m 100000 -p0
.\Debug\bt.exe -f .code.csv -m 100000 -p1

.\Debug\show.exe -c D:\quant\noise\build\Debug\000026.SZ.csv

grep.exe -rin "sqn\|code\|Win Rate" logs\sink.i.log
grep.exe -rin "sqn\|code\|Win Rate" logs\sink.d.log

*/