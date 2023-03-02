#pragma once

#include "noise/def.h"
#include "noise/position.h"

namespace noise
{
class Position;
class Backtest;
class Stat;

struct broker_config {
    float   init_cash;
    float   commission;
    bool    hedging;
    bool    trade_on_close;
};

class Broker {

public:
    Broker(void);
    ~Broker(void);

    void  place_a_order(struct order& order);
    void  close_position(void);
    bool  is_in_position(void);
    float get_equity(void);

    void  get_first_and_last_bars(struct bar &first, struct bar &last);

protected:
    void process(const struct bar &bar);
    void on_finish(void);
    void init(const struct broker_config &cfg);

    const std::vector<struct trade>& get_trades(void);
    const std::vector<struct order>& get_orders(void);

private:
    void  process_order(void);
    float adjust_price_on_fee(int size, float price);
    int   adjust_size_on_position(int size);

    void open_a_trade(float price, int size, const struct order& order, float cur_price);
    void close_a_trade(struct trade& trade, float price, time_t time);
    void reduce_a_trade(struct trade& trade, float price, int size, time_t time);

    void print_status(void);

    //
private:
    std::list<struct order>     m_orders_active;
    std::list<struct trade>     m_trades_active;

    std::vector<struct trade>   m_trades_closed;
    std::vector<struct order>   m_orders_closed;

    Position    m_position;

    struct broker_config  config_;

    struct bar  m_cur_bar;
    struct bar  m_first_bar;

    friend class Backtest;
    friend class Stat;
};

} // namespace name

