#pragma once

#include "noise/noise.h"
#include "noise/data.h"

namespace noise
{

class Broker;
struct BtContext;
struct bt_config;

class Strategy {

public:
    Strategy(void);
    ~Strategy(void);

public:
    //callback;                                            // Strategy lifecycle
    virtual void on_create(struct bt_config &cfg) = 0;     // - on_create()
    virtual void on_start() = 0;                           //    - on_start()
    virtual void on_bar_start() = 0;                       //         - on_bar_start()
    virtual void on_bar_recv(const struct bar &bar) = 0;   //         - on_bar_recv()
    virtual void on_bar_stop() = 0;                        //         - on_bar_stop()
    virtual void on_finish() = 0;                          //    - on_finish()
    virtual void on_destroy() = 0;                         // - on_destroy()

public:
    //common user API:
    void buy(int size,       int type = noise::ORDER_T_MARKET);
    void buy(float precent,  int type = noise::ORDER_T_MARKET);
    void sell(int size,      int type = noise::ORDER_T_MARKET);
    void sell(float precent, int type = noise::ORDER_T_MARKET);

    void send_order(struct order& one);         //broker.send_order(x)
    void close_position(void);                  //broker.close_position()
    float get_profit(void);                     //broker.close_position()

    void plot(void);
    void setup_plot(FuncPlot func_plot);

protected:
    //Framework API:
    void next(const struct bar &bar);
    void init(std::shared_ptr<Broker> broker);

    void set_id(uint32_t id) { id_ = id; };
    void set_code(std::string &code) { code_ = code; };
    void set_context(struct BtContext &ctx) { context_ = ctx; };
    void register_indicator(PtrIndicator ptr);

protected:
    void init();

    uint32_t    id_;
    int32_t     time_index_;

    std::string code_;
    struct BtContext context_;
    //struct bt_config config_;

    std::shared_ptr<Broker> broker_;
    std::vector<PtrIndicator> indicators_;

protected:
    static bool plot_on_finish_;            //shared between all strategy
    static bool stat_on_finish_;            //shared between all strategy


    friend class Backtest;
    friend class ta::Indicator;
};

class StrategyABC : public Strategy {

public:
    StrategyABC(void);
    ~StrategyABC(void);

    void on_create(struct bt_config &cfg) override;
    void on_start() override;
    void on_bar_start() override;
    void on_bar_recv(const struct bar &bar) override;
    void on_bar_stop() override;
    void on_finish() override;
    void on_destroy() override;

private:
    void init(void);
    void print_stat(void);
};

} // namespace name

