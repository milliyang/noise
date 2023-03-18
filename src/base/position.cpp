#define LOG_TAG "position"

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
    auto it = position_map_.find(code);
    if (it != position_map_.end()) {
        it->second.price = cur_price;
    }
}

void Position::update(std::string code, int share, float cur_price)
{
    auto it = position_map_.find(code);
    if (it != position_map_.end()) {
        if (it->second.share == 0) {
            it->second.share = share;
            it->second.cost_price = cur_price;
        } else {
            //TODO:
            // bug:  if (share * it->second.share) < 0, cost_price is incorrect
            //
            //  100*1.0f +  100*1.3f => OK
            // -100*1.0f + -100*1.3f => OK
            //  200*1.0f + -100*1.3f => NG...
            //
            int total_share = it->second.share + share;
            it->second.cost_price = (it->second.cost_price * it->second.share + share * cur_price) / total_share;
            it->second.share = total_share;
        }
        it->second.price = cur_price;
    } else {
        struct code_position pos;
        pos.share = share;
        pos.price = cur_price;
        pos.cost_price = cur_price;
        position_map_.emplace(code, pos);
    }
}

int Position::get_size(std::string &code)
{
    auto it = position_map_.find(code);
    if (it != position_map_.end()) {
        return it->second.share;
    }
    return 0;
}

float Position::get_profit(std::string &code)
{
    auto it = position_map_.find(code);
    if (it != position_map_.end()) {
        //float ratio = (exit_price - entry_price) / entry_price;
        float ratio = (it->second.price - it->second.cost_price) / it->second.cost_price;
        if (it->second.share < 0) { ratio *=-1.0f; }
        return ratio;
    }
    return 0;
}

void Position::withdraw(float money)
{
    cash_ -= money;
}

void Position::deposit(float money)
{
    cash_ += money;
}

void Position::set_cash(float cash)
{
    cash_ = cash;
}

float Position::get_cash(void)
{
    return cash_;
}

float Position::equity(void)
{
    float stock_cash = 0;
    for (auto iter = position_map_.begin(); iter != position_map_.end(); iter++) {
        if (iter->second.share != 0) {
            stock_cash += std::abs(iter->second.price * iter->second.share);
            //LOGI("money Share:{}, price:{}, worth:{}", iter->second.price, iter->second.share, std::abs(iter->second.price * iter->second.share));
        }
    }
    // if (stock_cash > 0) {
    //     LOGD("money cash:{}, stock:{} total:{}", cash_, stock_cash, cash_ + stock_cash);
    // }
    return cash_ + stock_cash;
}

}