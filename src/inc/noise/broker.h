#pragma once

#include "noise/def.h"
#include "noise/position.h"

namespace noise
{

class Position;

struct broker_config {
    float   commission;
    int     hedging;
    int     trade_on_close;
};

class Broker {

public:
    Broker(void);
    ~Broker(void);

    void place_a_order(struct order& order);

public:
    void process(const struct bar &bar);
    void close(void);
    void init(void);

    void close_position(void);

    const std::vector<struct trade>& get_trades(void);
    const std::vector<struct order>& get_orders(void);
    void get_first_and_last_bars(struct bar &first, struct bar &last);

private:
    void    process_order(void);
    float   adjust_price_on_fee(int size, float price);
    int     adjust_size_on_position(int size);

    void open_a_trade(float price, int size, const struct order& order);
    void close_a_trade(struct trade& trade, float price, time_t time);
    void reduce_a_trade(struct trade& trade, float price, int size, time_t time);

    void print_stat(void);

    //
private:
    std::list<struct order>     m_orders_active;
    std::list<struct trade>     m_trades_active;

    std::vector<struct trade>   m_trades_closed;
    std::vector<struct order>   m_orders_closed;

    Position    m_position;

    struct broker_config m_config;

    struct bar  m_cur_bar;
    struct bar  m_first_bar;
};

} // namespace name

