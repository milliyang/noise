#pragma once

#include "noise/def.h"

namespace noise
{

class Broker;

struct code_position {
    int share;
    float price;
};

class Position {

public:
    Position(void);
    ~Position(void);

    void  set_cash(float cash = 100000.0f);
    float get_cash(void);
    float equity(void);    // cash + stock

protected:
    void withdraw(float money);
    void deposit(float money);

    void update(std::string code, float cur_price);
    void update(std::string code, int share, float cur_price);

private:
    float m_cash;
    std::map<std::string, struct code_position> m_position;

    friend class Broker;
};


} // namespace name

