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
    bool  is_in_position(void);
    float get_equity(void);
    float get_cur_price(void);
    int   get_position(std::string &code);
    float get_profit(std::string &code);

protected:
    void init(const struct broker_config &cfg);
    void process(const struct bar &bar);
    void on_finish(void);

    //for stat
    float get_init_equity(void);
    void  get_first_and_last_bars(struct bar &first, struct bar &last);

    const std::vector<struct trade>& get_trades(void);
    const std::vector<struct order>& get_orders(void);

private:
    void  process_order(void);
    float adjust_price_on_fee(int size, float price);
    int   adjust_size_on_position(int size);

    void open_a_trade(float price, int size, const struct order& order, float cur_price);
    void close_a_trade(struct trade& trade, float price, time_t time);
    void reduce_a_trade(struct trade& trade, float price, int size, time_t time);
    void close_position(void);

    void print_status(void);

private:
    std::list<struct order>     active_orders_;
    std::list<struct trade>     active_trades_;
    std::vector<struct trade>   closed_trades_;
    std::vector<struct order>   closed_orders_;

    Position    position_;

    struct broker_config  config_;
    struct bar  cur_bar_;
    struct bar  first_bar_;

    friend class Backtest;
    friend class Stat;
};

} // namespace name

