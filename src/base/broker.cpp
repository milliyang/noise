#include "noise/noise.h"
#include "noise/broker.h"

namespace noise {

Broker::Broker(void)
{

    m_orders_active.clear();
    m_orders_closed.clear();
    m_trades_active.clear();
    m_trades_closed.clear();

    config_.commission = 0;
    config_.hedging = 0;
    config_.trade_on_close = 0;

    m_first_bar.time = 0;
    m_cur_bar.time = 0;
    //config_.commission = 0.005f; //0.5%
}

Broker::~Broker(void)
{
}

/**
 * @brief process order, fee, update status
 * 
 */
void Broker::process(const struct bar &bar)
{
    uprint::print("new   bar", bar);

    if (m_first_bar.time == 0) { m_first_bar = bar; };

    m_cur_bar = bar;
    process_order();

    //TODO:
    // - update posotion
    // - update stat, Enti, PNL
    // -
    m_position.update(bar.code, bar.close);
}

/**
 * @brief process order
 * 
 */
void Broker::process_order(void)
{

    //TODO:
    // support order executed on condition

    for (auto it = m_orders_active.begin(); it != m_orders_active.end(); ++it) {
        //const struct order &order = *it;
        struct order &order = *it;

        // force market order [now]
        if (order.type != ORDER_T_MARKET) { order.type = ORDER_T_MARKET; }

        //TODO: one broker handle: multiple code together
        if (0 && order.code != m_cur_bar.code) {
            struct bar code_bar;// xxx_get_bar(m_order.code);
            // 1. get barinfo of the code
            // 2.
        } else {
            order.code = m_cur_bar.code;
        }

        uprint::print("new order", order);

        float cur_price;
        if (config_.trade_on_close) {
            cur_price = m_cur_bar.pre_close;
        } else {
            cur_price = m_cur_bar.open;
        }

        int size = adjust_size_on_position(order.size);
        int need_size = size;

        if (!config_.hedging) {
            int preprocess = 1; //preprocess the order & trade with the same date
            do {
                if (need_size == 0) { break; } //bugfix

                for (auto it_trade = m_trades_active.begin(); it_trade != m_trades_active.end(); ++it_trade) {
                    if (it_trade->is_closed()) { continue; }
                    if (preprocess) {
                        if (it_trade->entry_time != m_cur_bar.time) {
                            continue;
                        }
                    }

                    //the same direction, use a new order
                    if (it_trade->is_long() && order.is_long()) {
                        continue;
                    }
                    if (it_trade->is_short() && order.is_short()) {
                        continue;
                    }

                    // Order size greater than this opposite-directed existing trade,
                    // so it will be closed completely
                    if (std::abs(need_size) >= std::abs(it_trade->size)) {
                        need_size += it_trade->size;
                        close_a_trade(*it_trade, cur_price, m_cur_bar.time);
                        it_trade->hedging = true;
                    } else {
                        // The existing trade is larger than the new order,
                        // so it will only be closed partially
                        reduce_a_trade(*it_trade, cur_price, need_size, m_cur_bar.time);
                        need_size = 0;
                    }
                    if (need_size == 0) { break; }
                }
            } while (preprocess--);

            //remove closed trades
            for (auto it_trade = m_trades_active.begin(); it_trade != m_trades_active.end(); /*++it_trade*/) {
                if (it_trade->is_closed()) {
                    it_trade = m_trades_active.erase(it_trade);
                } else {
                    it_trade++;
                }
            }
        }

        if (need_size != 0) {
            float adjusted_price = adjust_price_on_fee(need_size, cur_price);
            open_a_trade(adjusted_price, need_size, order, cur_price);
            m_orders_closed.push_back(order);
        }
    }

    //cleanup
    m_orders_active.clear();
}

void Broker::init(const struct broker_config &cfg)
{
    config_ = cfg;
    m_position.set_cash(cfg.init_cash);
}

void Broker::on_finish(void)
{
    //ignore pendding order
    m_orders_active.clear();

    close_position();
}

void Broker::close_position(void)
{

    for (auto it_trade = m_trades_active.begin(); it_trade != m_trades_active.end(); ++it_trade) {
        if (it_trade->is_closed()) { continue; }

        struct bar bar;
        bar.code  = it_trade->code;
        bar.time  = m_cur_bar.time;
        bar.close = m_cur_bar.close;
        //TODO: xxx_get_bar()
        close_a_trade(*it_trade, bar.close, bar.time);
    }
    m_trades_active.clear();

    print_status();
}

bool Broker::is_in_position(void)
{
    for (auto it_trade = m_trades_active.begin(); it_trade != m_trades_active.end(); ++it_trade) {
        if (it_trade->is_closed()) { continue; }

        return true;
    }

    return false;
}

const std::vector<struct trade>& Broker::get_trades(void)
{
    return m_trades_closed;
}

const std::vector<struct order>& Broker::get_orders(void)
{
    return m_orders_closed;
}

void Broker::get_first_and_last_bars(struct bar &first, struct bar &last)
{
    first = m_first_bar;
    last = m_cur_bar;
}

/**
 * @brief Adjust price to include commission (or bid-ask spread).
 */
float Broker::adjust_price_on_fee(int size, float price)
{
    if (size > 0) {
        price += (price * config_.commission);  // 0.3% * 19.0 
    } else if (size < 0) {
        price -= (price * config_.commission);
    }
    return price;
}

int Broker::adjust_size_on_position(int size)
{
    return size;
}

void Broker::place_a_order(struct order& order)
{

    //check valid
    if (order.size == 0) return;

    //todo:
    // must provide code namw with broker processing multiple code bar
    int code_num = 1;
    int bt_mode = BT_MODE_ONE2ONE;
    if (order.code.length()) {
        if (bt_mode == bt_mode && code_num > 1) {
            assert("must provide code name\n");
            return;
        } else {
            //override if not provide
            order.code = m_cur_bar.code;
        }
    }
    m_orders_active.push_back(order);
}

void Broker::open_a_trade(float price, int size, const struct order& order, float cur_price)
{
    struct trade trade;
    trade.code = order.code;
    trade.entry_price = price;
    trade.size = size;

    float money = price * std::abs(size);
    m_position.withdraw(money);
    m_position.update(order.code, size, cur_price);

    //fill in system info
    trade.entry_time = m_cur_bar.time;
    //init
    trade.exit_time = 0;        //must
    trade.exit_price = 0;       //must
    trade.PNL = 0;

    uprint::print("new trade", trade);
    m_trades_active.push_back(trade);
}

void Broker::close_a_trade(struct trade& trade, float price, time_t time)
{
    trade.set_closed(time);

    struct trade one_trade = trade;
    one_trade.exit_time = time;
    one_trade.exit_price = price;
    one_trade.PNL = one_trade.size * (one_trade.exit_price - one_trade.entry_price);

    float money = price * std::abs(one_trade.size);
    m_position.deposit(money);
    m_position.update(trade.code, (-1)*one_trade.size, price);

    uprint::print("close trade", one_trade);
    m_trades_closed.push_back(one_trade);
}

void Broker::reduce_a_trade(struct trade& trade, float price, int size, time_t time)
{
    assert(trade.size * size < 0);

    //eg. 100 (size=-30)  or  -100  (size=30)

    struct trade one_trade = trade;
    one_trade.size = -size;
    one_trade.hedging = true;
    close_a_trade(one_trade, price, time);

    //
    trade.size += size;
}

float Broker::get_equity(void)
{
    return m_position.equity();
}

void Broker::print_status(void)
{
    LOGI("[broker.stat] m_orders_active size:{}",      (int)m_orders_active.size());
    LOGI("[broker.stat] m_trades_active size:{}",      (int)m_trades_active.size());
    LOGI("[broker.stat] m_trades_closed size:{}",      (int)m_trades_closed.size());
    LOGI("[broker.stat] m_orders_closed size:{}",      (int)m_orders_closed.size());

    uprint::print("[broker.stat] m_trades_active ", m_trades_active);
    uprint::print("[broker.stat] m_trades_closed ", m_trades_closed);
}



}