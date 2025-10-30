#pragma once

#include <functional>
#include "noise/noise.h"
#include "noise/args.h"
#include "noise/math/position_tracker.h"

namespace noise
{

enum BT_MODE {
    BT_MODE_ONE2ONE = 0,       // 1:1; one code one instance
    BT_MODE_M2ONE,             // n:1; multi code on one instance
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
    struct args0 args;
};

class Backtest {

public:
    Backtest(void);
    Backtest(FuncStrategy func);
    ~Backtest(void);

    void run(void);

private:
    void init(void);
    void post_process(void);
    void deinit(void);

    void make_strategy(uint32_t id);
    void run_strategy(const struct BtContext &context);

    void load_config(std::string filepath);

    

private:
    FuncStrategy func_get_strategy_;
    struct BtContext context_;
    std::vector<struct BtContext> context_vector_;

private:
    const std::string       build_date_;
    struct bt_config        config_;
    math::PositionTracker   position_tracker_;
    bool                    position_tracker_enable_;
};


} // namespace name

