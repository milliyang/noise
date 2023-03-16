#pragma once

#include "noise/def.h"

namespace noise
{

class Backtest;
class Broker;
class Data;

class Stat {

public:
    Stat(void);
    ~Stat(void);

    void plot(void);

protected:
    void init(std::shared_ptr<Broker> broker, std::shared_ptr<Data> data);
    void on_trade_day(void);
    void on_finish(void);
    const struct stat& get_stat(void);

private:
    void calc_stat(void);
    void generate_trade_series(void);
    void generate_equity_series(void);

private:
    std::shared_ptr<Broker> broker_;
    std::shared_ptr<Data> data_;
    PtrSeries equity_series_;
    struct stat status_;

    friend class Backtest;
};


} // namespace name

