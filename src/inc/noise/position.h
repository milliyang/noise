#pragma once

#include "noise/def.h"

namespace noise
{

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
    
    void update(std::string code, int share, float cur_price);

private:
    float m_cash;
    std::map<std::string, struct code_position> m_position;

};


} // namespace name

