#include "noise/def.h"
#include "noise/position.h"
#include "noise/log.h"

namespace noise {

Position::Position(void)
{

}

Position::~Position(void)
{

}

void Position::update(std::string code, float cur_price)
{
    auto it = m_position.find(code);
    if (it != m_position.end()) {
        it->second.price = cur_price;
    }
}

void Position::update(std::string code, int share, float cur_price)
{
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

void Position::withdraw(float money)
{
    m_cash -= money;
}

void Position::deposit(float money)
{
    m_cash += money;
}

void Position::set_cash(float cash)
{
    m_cash = cash;
}

float Position::get_cash(void)
{
    return m_cash;
}

float Position::equity(void)
{
    float stock_cash = 0;
    for (auto iter = m_position.begin(); iter != m_position.end(); iter++) {
        if (iter->second.share != 0) {
            stock_cash += std::abs(iter->second.price * iter->second.share);
            //LOGI("money Share:{}, price:{}, worth:{}", iter->second.price, iter->second.share, std::abs(iter->second.price * iter->second.share));
        }
    }
    if (stock_cash > 0) {
        LOGI("money cash:{}, stock:{} total:{}", m_cash, stock_cash, m_cash + stock_cash);
    }
    return m_cash + stock_cash;
}

}