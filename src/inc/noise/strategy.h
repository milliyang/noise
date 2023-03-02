#pragma once

#include "noise/noise.h"
#include "noise/data.h"

namespace noise
{

class Broker;

struct BtContext;

/**
 * Strategy Live Cycle:
 *
 * - on_create()
 *    - on_start()
 *         - on_bar_start()
 *         - on_bar_recv()
 *         - on_bar_stop()
 *    - on_finish()
 * - on_destroy()
 */
class Strategy {

public:
    Strategy(void);
    ~Strategy(void);

public:
    //common system livecycle callback:
    virtual void on_create(struct bt_config &cfg) = 0;
    virtual void on_start() = 0;
    virtual void on_bar_start() = 0;
    virtual void on_bar_recv(const struct bar &bar) = 0;
    virtual void on_bar_stop() = 0;
    virtual void on_finish() = 0;
    virtual void on_destroy() = 0;

public:
    //common user API:
    void buy(int size, int type = noise::ORDER_T_MARKET);
    void buy(float precent, int type = noise::ORDER_T_MARKET);
    void sell(int size, int type = noise::ORDER_T_MARKET);
    void sell(float precent, int type = noise::ORDER_T_MARKET);

    void send_order(struct order& one);         //broker.send_order(x)
    void close_position(void);                  //broker.close_position()

    void plot(void);

    void setup_plot(FuncPlot func_plot);

protected:
    //Framework API:
    void next(const struct bar &bar);
    void init(std::shared_ptr<Broker> broker);

    void set_id(uint32_t id) { m_id = id; };
    void set_context(struct BtContext &ctx) { m_ctx = ctx; };
    void register_indicator(PtrIndicator ptr);

protected:
    void init();

    uint32_t m_id;
    uint32_t m_bar_idx;
    std::shared_ptr<Broker> broker_;
    struct BtContext m_ctx;
    std::vector<PtrIndicator> indicators_;

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

