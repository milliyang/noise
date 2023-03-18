#define LOG_TAG "show"

#include "noise/noise.h"
#include "noise/indicator/ta.h"
#include "noise/util.h"
#include "noise/log.h"
#include "noise/args.h"
#include "show.h"

#ifdef CONFIG_QPLOT
#include "qplot.h"
#endif

using namespace noise;

#define MAX_CODE_NUM   (20)

StrategyShow::StrategyShow(void)
{
    ma5_ = nullptr;
    ma10_ = nullptr;
    ma60_ = nullptr;
    ma120_ = nullptr;
    stddev10_ = nullptr;
    boll_ = nullptr;
}

StrategyShow::~StrategyShow(void)
{

}

void StrategyShow::override_config(struct noise::bt_config &cfg)
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
        auto filep = std::fopen(code.c_str(), "r");
        cfg.codes.push_back(code);

        if(filep) {
            std::fclose(filep);
            cfg.feed.root_path = "";
            cfg.feed.filename = code;  //it's a filepath
            LOGD("use:{}", cfg.feed.filename);
        }
    }

    if (args->has(ARGS_BEGIN_DATE)) {
        cfg.feed.begin_date = args->get(ARGS_BEGIN_DATE); //"20250310";
    }

    uprint::print("codes", cfg.codes);
}

void StrategyShow::on_create(struct noise::bt_config &cfg)
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
#endif
    override_config(cfg);
}

void StrategyShow::on_destroy(void)
{

}

void StrategyShow::on_start(void)
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

void StrategyShow::on_bar_start(void)
{
    series_close_ = context_.data->find_series(INDICATOR_CLOSE);
}

void StrategyShow::prepare_data(const struct noise::bar &bar)
{
    index_++;
}

void StrategyShow::on_bar_recv(const struct noise::bar &bar)
{
    TRACE_TAG(bar.code.c_str());
    prepare_data(bar);
}

void StrategyShow::on_bar_stop(void)
{

}

void StrategyShow::on_finish(void)
{
    stddev10_->series_->figure = "Stddev10";
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
        return std::make_shared<StrategyShow>();
    });

    bt.run();
}

/**

MSBuild.exe noise.sln -m

.\Debug\bt.exe -c 300014.SZ -m 100000 -p0
.\Debug\bt.exe -f .code.csv -m 100000 -p1

.\Debug\show.exe -c D:\quant\noise\build\Debug\000026.SZ.csv

grep.exe -rin "sqn\|code\|Win Rate" logs\sink.i.log
grep.exe -rin "sqn\|code\|Win Rate" logs\sink.d.log

*/