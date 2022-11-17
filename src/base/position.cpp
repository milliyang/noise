#include "noise/def.h"

#include "noise/def.h"
#include "noise/position.h"

namespace noise {

Position::Position(void)
{
    TRACE_LINE
}

Position::~Position(void)
{
    TRACE_LINE
}

void Position::update(std::string code, int share, float cur_price)
{
    TRACE_LINE

    auto it = m_position.find(code);
    if (it != m_position.end()) {
        it->second.share += share;
        it->second.price = cur_price;
    } else {
        struct code_position pos;
        pos.share = share;
        pos.price = cur_price;
        m_position.emplace(code, pos);
    }
}

void Position::set_cash(float cash)
{
    m_cash = cash;
}

float Position::get_cash(void)
{
    return m_cash;
}

}