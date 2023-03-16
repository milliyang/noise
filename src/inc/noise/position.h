#pragma once

#include "noise/def.h"

namespace noise
{

class Broker;

struct code_position {
    int share;
    float price;
    float cost_price;       //init and cost average price
};

class Position {

public:
    Position(void);
    ~Position(void);

    void  set_cash(float cash = 100000.0f);
    float get_cash(void);
    float equity(void);    // cash + stock
    int   get_size(std::string &code);
    float get_profit(std::string &code);

protected:
    void withdraw(float money);
    void deposit(float money);

    void update(std::string code, float cur_price);
    void update(std::string code, int share, float cur_price);

private:
    float cash_;
    std::map<std::string, struct code_position> position_map_;

    friend class Broker;
};


} // namespace name

