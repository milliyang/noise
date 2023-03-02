#include "noise/def.h"

#include "noise/def.h"
#include "noise/strategy.h"
#include "noise/broker.h"

namespace noise {

Strategy::Strategy(void)
{
    TRACE_LINE
    m_bar_idx = 0;
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
    TRACE_LINE;
    m_bar_idx++;
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
    //TODO:
}

void Strategy::sell(int size, int type)
{
    struct noise::order order;
    order.type = type;
    order.size = std::abs(size) * -1;
    send_order(order);
}

void Strategy::sell(float precent, int type)
{
    //TODO:
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

void Strategy::plot(void)
{
    if (m_ctx.data.get() != nullptr) {
        m_ctx.data->plot();
    }
    return;
}

void Strategy::register_indicator(PtrIndicator ptr)
{
    assert(ptr->name().length() > 0);

    ptr->setup(m_ctx.data);
    m_ctx.data->push_indicator(ptr->name(), ptr);
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