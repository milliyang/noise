#pragma once

#include <functional>
#include "noise/noise.h"

namespace noise
{

enum BT_MODE {
    BT_MODE_DEFAULT = 0,        // 1:1; one code one instance
    BT_MODE_FUSION,             // n:1; multi code on one instance
};

struct bt_func {
    FuncPlot plot;
};

struct bt_config {
    enum BT_MODE mode;
    std::vector<std::string> codes;     //subscribe code
    struct broker_config broker;
    struct feed_config feed;
    struct plot_config plot;
    struct bt_func func;
};

class Backtest {

public:
    Backtest(void);
    Backtest(FuncStrategy func);
    ~Backtest(void);

    void run(void);

private:
    void init();
    void deinit();

    void load_config(std::string filepath);

    void make_strategy(int id, struct BtContext &context, const std::shared_ptr<Strategy> &strategy);
    void run_strategy(const struct BtContext &context);

    //
private:
    FuncStrategy m_func_get_stragegy;
    struct BtContext m_context;
    std::vector<struct BtContext> m_v_context;

    //others
private:
    const std::string m_build_date;
    struct bt_config m_config;
};


} // namespace name

