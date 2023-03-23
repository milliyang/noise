#define LOG_TAG "strategy"

#include "noise/def.h"

#include "noise/def.h"
#include "noise/strategy.h"
#include "noise/broker.h"

namespace noise {

bool Strategy::plot_on_finish_ = true;            //shared between all strategy
bool Strategy::stat_on_finish_ = true;            //shared between all strategy

Strategy::Strategy(void)
{
    TRACE_LINE
    time_index_ = -1;
}

Strategy::~Strategy(void)
{
    TRACE_LINE

    //release all Indicator
    for(auto iter = indicators_.begin(); iter!=indicators_.end(); iter++) {
        delete *iter;
    }
}

void Strategy::next(const struct bar &bar)
{
    time_index_++;
}

void Strategy::init(std::shared_ptr<Broker> broker)
{
    broker_ = broker;
}

void Strategy::buy(int size, int type)
{
    struct noise::order order;
    order.type = type;
    order.size = std::abs(size);
    send_order(order);
}

void Strategy::buy(float precent, int type)
{
    int size = (int) (broker_->get_equity() * precent /  broker_->get_cur_price());
    buy(size, type);
}

void Strategy::sell(int size, int type)
{
    if (size <= 0) {
        size = broker_->get_position(code_);
    }
    struct noise::order order;
    order.type = type;
    order.size = std::abs(size) * -1;
    send_order(order);
}

void Strategy::sell(float precent, int type)
{
    //TODO:
    //int size = broker_->get_equity() /  broker_->get_cur_price() * precent;
}

void Strategy::send_order(struct order& order)
{
    if (order.type == noise::ORDER_T_MARKET) {
        order.price = NAN;
    }
    if (broker_.get() != nullptr) {
        broker_->place_a_order(order);
    }
}

float Strategy::get_profit(void)
{
    return broker_->get_profit(code_);
}

void Strategy::plot(void)
{
    if (context_.data.get() != nullptr) {
        context_.data->plot();
    }
    return;
}

void Strategy::register_indicator(PtrIndicator ptr)
{
    assert(ptr->name().length() > 0);

    ptr->setup(context_.data);
    context_.data->push_indicator(ptr->name(), ptr);
    indicators_.push_back(ptr);
}

StrategyABC::StrategyABC(void)
{
    TRACE_LINE
}

StrategyABC::~StrategyABC(void)
{
    TRACE_LINE
}

void StrategyABC::on_start(void)
{
    TRACE_LINE;
    TRACE_NEWLINE;
}

void StrategyABC::on_create(struct bt_config &cfg)
{
    TRACE_LINE;
    TRACE_NEWLINE;

    cfg.mode = noise::BT_MODE_ONE2ONE;
    cfg.codes = {"000004.SZ"};
}

void StrategyABC::on_destroy(void)
{
    TRACE_LINE;
    TRACE_NEWLINE;
}


void StrategyABC::on_bar_start(void)
{
    TRACE_LINE;
    TRACE_NEWLINE;
}

void StrategyABC::on_bar_recv(const struct bar &bar)
{
    TRACE_TAG(bar.code.c_str());

    static int cnt = 0;
    //if (cnt == 0) {
        {
            struct order order;
            order.code = "000001.SZ";
            order.size = 100;
            send_order(order);
        }

        {
            struct order order;
            order.code = "000001.SZ";
            order.size = -10;
            send_order(order);
        }
    //}

    cnt++;
    TRACE_NEWLINE;
}

void StrategyABC::on_bar_stop(void)
{
    TRACE_LINE;
    TRACE_NEWLINE;


}

void StrategyABC::on_finish(void)
{
    TRACE_LINE;
    TRACE_NEWLINE;


}

}