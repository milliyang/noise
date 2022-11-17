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
}

void Strategy::next(const struct bar &bar)
{
    TRACE_LINE;
    m_bar_idx++;
}

void Strategy::init(std::shared_ptr<Broker> broker)
{
    m_broker = broker;
}

void Strategy::send_order(struct order& order)
{
    TRACE_LINE
    if (m_broker.get() != nullptr) {
        m_broker->place_a_order(order);
    }
}

void Strategy::plot(void)
{
    if (m_ctx.data.get() != nullptr) {
        m_ctx.data->plot();
    }
    return;
}

PtrIndicator Strategy::create_indicator(const std::string &name)
{
    if (m_ctx.data.get() != nullptr) {
        return m_ctx.data->create_indicator(name);
    }
    return nullptr;
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

    cfg.mode = noise::BT_MODE_DEFAULT;
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