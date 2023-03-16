#include "noise/noise.h"
#include "noise/broker.h"

namespace noise {

Broker::Broker(void)
{
    active_orders_.clear();
    closed_orders_.clear();
    active_trades_.clear();
    closed_trades_.clear();

    config_.commission = 0;
    config_.hedging = 0;
    config_.trade_on_close = 0;

    first_bar_.time = 0;
    cur_bar_.time = 0;
    config_.commission = 0.005f; //0.5%
}

Broker::~Broker(void)
{
}

void Broker::init(const struct broker_config &cfg)
{
    config_ = cfg;
    position_.set_cash(cfg.init_cash);
}

/**
 * @brief process order, fee, update position
 */
void Broker::process(const struct bar &bar)
{
    uprint::print("new   bar", bar);

    if (first_bar_.time == 0) {
        first_bar_ = bar;
    };

    cur_bar_ = bar;
    process_order();

    position_.update(bar.code, bar.close);
}

/**
 * @brief process order, generate trade
 */
void Broker::process_order(void)
{
    //TODO:
    // support order executed on condition
    //  - cash not enouth
    //  - fixed order

    for (auto it = active_orders_.begin(); it != active_orders_.end(); ++it) {
        //const struct order &order = *it;
        struct order &order = *it;

        // force market order [now]
        if (order.type != ORDER_T_MARKET) { order.type = ORDER_T_MARKET; }

        //TODO: one broker handle: multiple code together
        if (0 && order.code != cur_bar_.code) {
            struct bar code_bar;// xxx_get_bar(m_order.code);
            // 1. get barinfo of the code
            // 2.
        } else {
            order.code = cur_bar_.code;
        }

        uprint::print("new order", order);

        float cur_price;
        if (config_.trade_on_close) {
            cur_price = cur_bar_.pre_close;
        } else {
            cur_price = cur_bar_.open;
        }

        int size = adjust_size_on_position(order.size);
        int need_size = size;

        if (!config_.hedging) {
            int preprocess = 1; //preprocess the order & trade with the same date
            do {
                if (need_size == 0) { break; } //bugfix

                for (auto it_trade = active_trades_.begin(); it_trade != active_trades_.end(); ++it_trade) {
                    if (it_trade->is_closed()) { continue; }
                    if (preprocess) {
                        if (it_trade->entry_time != cur_bar_.time) {
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
                        close_a_trade(*it_trade, cur_price, cur_bar_.time);
                        it_trade->hedging = true;
                    } else {
                        // The existing trade is larger than the new order,
                        // so it will only be closed partially
                        reduce_a_trade(*it_trade, cur_price, need_size, cur_bar_.time);
                        need_size = 0;
                    }
                    if (need_size == 0) { break; }
                }
            } while (preprocess--);

            //remove closed trades
            for (auto it_trade = active_trades_.begin(); it_trade != active_trades_.end(); /*++it_trade*/) {
                if (it_trade->is_closed()) {
                    it_trade = active_trades_.erase(it_trade);
                } else {
                    it_trade++;
                }
            }
        }

        if (need_size != 0) {
            float adjusted_price = adjust_price_on_fee(need_size, cur_price);
            open_a_trade(adjusted_price, need_size, order, cur_price);
            closed_orders_.push_back(order);
        }
    }

    //cleanup, moved to trade
    active_orders_.clear();
}

void Broker::on_finish(void)
{
    //ignore pendding order
    active_orders_.clear();

    close_position();
}

void Broker::close_position(void)
{
    for (auto it_trade = active_trades_.begin(); it_trade != active_trades_.end(); ++it_trade) {
        if (it_trade->is_closed()) { continue; }

        struct bar bar;
        bar.code  = it_trade->code;
        bar.time  = cur_bar_.time;
        bar.close = cur_bar_.close;
        //TODO: xxx_get_bar()
        close_a_trade(*it_trade, bar.close, bar.time);
    }

    //clean up, all closed
    active_trades_.clear();

    print_status();
}

bool Broker::is_in_position(void)
{
    for (auto it_trade = active_trades_.begin(); it_trade != active_trades_.end(); ++it_trade) {
        if (it_trade->is_closed()) { continue; }

        return true;
    }

    return false;
}

const std::vector<struct trade>& Broker::get_trades(void)
{
    return closed_trades_;
}

const std::vector<struct order>& Broker::get_orders(void)
{
    return closed_orders_;
}

void Broker::get_first_and_last_bars(struct bar &first, struct bar &last)
{
    first = first_bar_;
    last = cur_bar_;
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
    if (order.size == 0) {
        return;
    }
    
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
            order.code = cur_bar_.code;
        }
    }
    active_orders_.push_back(order);
}

void Broker::open_a_trade(float price, int size, const struct order& order, float cur_price)
{
    struct trade trade;
    trade.code = order.code;
    trade.entry_price = price;
    trade.size = size;

    float money = price * std::abs(size);
    position_.withdraw(money);
    position_.update(order.code, size, price);

    //fill in system info
    trade.entry_time = cur_bar_.time;
    //init
    trade.exit_time = 0;        //must
    trade.exit_price = 0;       //must
    trade.PNL = 0;

    uprint::print("new trade", trade);
    active_trades_.push_back(trade);
}

void Broker::close_a_trade(struct trade& trade, float price, time_t time)
{
    trade.set_closed(time);

    struct trade one_trade = trade;
    one_trade.exit_time = time;
    one_trade.exit_price = price;
    one_trade.PNL = one_trade.size * (one_trade.exit_price - one_trade.entry_price);

    float money = price * std::abs(one_trade.size);
    position_.deposit(money);
    position_.update(trade.code, (-1)*one_trade.size, price);

    uprint::print("close trade", one_trade);
    closed_trades_.push_back(one_trade);
}

void Broker::reduce_a_trade(struct trade& trade, float price, int size, time_t time)
{
    assert(trade.size * size < 0);

    //eg. 100 (size=-30)  or  -100  (size=30)

    struct trade one_trade = trade;
    one_trade.size = -size;
    one_trade.hedging = true;
    close_a_trade(one_trade, price, time);

    trade.size += size;
}

int Broker::get_position(std::string &code)
{
    return position_.get_size(code);
}

float Broker::get_profit(std::string &code)
{
    return position_.get_profit(code);
}

float Broker::get_cur_price(void)
{
    return cur_bar_.pre_close;
}

float Broker::get_equity(void)
{
    return position_.equity();
}

float Broker::get_init_equity(void)
{
    return config_.init_cash;
}

void Broker::print_status(void)
{
    LOGD("[broker.stat] active_orders_ size:{}",      (int)active_orders_.size());
    LOGD("[broker.stat] active_trades_ size:{}",      (int)active_trades_.size());
    LOGD("[broker.stat] closed_trades_ size:{}",      (int)closed_trades_.size());
    LOGD("[broker.stat] closed_orders_ size:{}",      (int)closed_orders_.size());

    uprint::print("[broker.stat] active_trades_ ", active_trades_);
    uprint::print("[broker.stat] closed_trades_ ", closed_trades_);
}



}