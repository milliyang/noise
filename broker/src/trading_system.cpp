#include "trading_system.h"
#include <cmath>
#include <optional>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <iostream>

#include "utils.h"

TradingSystem::TradingSystem() : record_keeper_(std::make_unique<TradingRecordKeeper>()), cash_(0.0) {}
TradingSystem::~TradingSystem() = default;

TradingSystem::TradingReport TradingSystem::generate_report() const {
    TradingReport report;
    report.positions = positions_;
    
    // 计算未实现盈亏
    for (const auto& [symbol, pos] : positions_) {
        report.unrealized_pnl += pos.unrealized_pnl_;
        //std::cout << "symbol: " << symbol << " pos.unrealized_pnl_: " << pos.unrealized_pnl_ << "\n";
    }
    
    // 从交易记录中计算已实现盈亏
    const auto& trades = record_keeper_->get_trades();
    for (const auto& trade : trades) {
        if (trade.type == OrderType::SELL) {
            auto it = positions_.find(trade.symbol);
            if (it != positions_.end()) {
                report.realized_pnl += 
                    (trade.price - it->second.avg_price_) * trade.quantity;
            }
        }
        report.total_commission += trade.commission;
    }
    
    report.total_pnl = report.realized_pnl + report.unrealized_pnl;
    report.total_trades = trades.size();
    
    return report;
}

void TradingSystem::export_trading_records(const std::string& directory) const {
    record_keeper_->export_to_csv(directory);
}

std::string TradingSystem::generate_order_id() {
    // Generate timestamp-based unique ID
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    // Convert to hex string and add random suffix
    std::stringstream ss;
    ss << std::hex << timestamp << "-" 
        << std::setfill('0') << std::setw(4) 
        << (std::rand() % 65536);
    return ss.str();
}

std::string TradingSystem::generate_trade_id() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::stringstream ss;
    ss << "T" << std::hex << timestamp << "-" 
       << std::setfill('0') << std::setw(4) 
       << (std::rand() % 65536);
    return ss.str();
}

bool TradingSystem::execute_order(const Order& order)
{
    // ... 现有的订单验证代码 ...

    if (order.type_ == OrderType::BUY) {
        if (execute_buy_order(order)) {
            // 记录交易
            record_trade_events(order, positions_[order.symbol_], generate_trade_id());

            return true;
        }
    }
    else if (order.type_ == OrderType::SELL) {
        if (execute_sell_order(order)) {
            // 记录交易
            record_trade_events(order, positions_[order.symbol_], generate_trade_id(), (order.price_ - positions_[order.symbol_].avg_price_) * order.quantity_);

            return true;
        }
    }
    return false;
}

bool TradingSystem::execute_sell_order(const Order& order) {
    auto it = positions_.find(order.symbol_);
    if (it == positions_.end() || it->second.quantity_ < order.quantity_) {
        return false;  // 持仓不足
    }
    
    double commission = calculate_commission(order);
    double total_amount = order.quantity_ * order.price_;
    
    cash_ += (total_amount - commission);
    auto& position = positions_[order.symbol_];
    position.quantity_ -= order.quantity_;
    
    // 如果持仓变为0，可以重置均价
    if (position.quantity_ == 0) {
        position.avg_price_ = 0;
    }
    
    return true;
}

void TradingSystem::update_unrealized_pnl(const std::map<std::string, double>& current_prices) {
    for (const auto& [symbol, price] : current_prices) {
        auto it = positions_.find(symbol);
        if (it != positions_.end()) {
            it->second.last_price_ = price;  // 更新最新价格
            it->second.unrealized_pnl_ = 
                (price - it->second.avg_price_) * it->second.quantity_;
        }
    }
}

std::string TradingSystem::format_date(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d");
    return ss.str();
}

bool TradingSystem::is_same_day(const std::chrono::system_clock::time_point& t1,
                               const std::chrono::system_clock::time_point& t2) {
    return format_date(t1) == format_date(t2);
}

double TradingSystem::calculate_drawdown(const std::vector<double>& equity_curve) const {
    double max_drawdown = 0.0;
    double peak = equity_curve[0];
    
    for (double equity : equity_curve) {
        if (equity > peak) {
            peak = equity;
        }
        double drawdown = (peak - equity) / peak;
        max_drawdown = std::max(max_drawdown, drawdown);
    }
    
    return max_drawdown;
}

double TradingSystem::calculate_sharpe_ratio(const std::vector<double>& daily_returns) const {
    if (daily_returns.empty()) return 0.0;
    
    // 计算平均收益率
    double mean = std::accumulate(daily_returns.begin(), daily_returns.end(), 0.0) 
                 / daily_returns.size();
    
    // 计算标准差
    double sq_sum = std::inner_product(daily_returns.begin(), daily_returns.end(),
                                     daily_returns.begin(), 0.0);
    double std_dev = std::sqrt(sq_sum / daily_returns.size() - mean * mean);
    
    // 假设无风险利率为0，年化夏普比率
    return std_dev == 0 ? 0 : (mean * 252) / (std_dev * std::sqrt(252));
}

TradingSystem::TradeStatistics TradingSystem::get_trade_statistics(
    const std::chrono::system_clock::time_point& start_time,
    const std::chrono::system_clock::time_point& end_time) const {
    
    TradeStatistics stats{};
    std::vector<double> profits;
    std::vector<double> equity_curve;
    double current_equity = 0.0;
    
    for (const auto& trade : record_keeper_->get_trades()) {
        if (trade.timestamp < start_time || trade.timestamp > end_time) {
            continue;
        }
        
        stats.total_trades++;
        stats.trades_by_symbol[trade.symbol]++;
        
        // 计算交易盈亏
        double profit = trade.total_amount - trade.commission;
        profits.push_back(profit);
        current_equity += profit;
        equity_curve.push_back(current_equity);
        
        if (profit > 0) {
            stats.winning_trades++;
            stats.avg_win += profit;
        } else {
            stats.losing_trades++;
            stats.avg_loss += profit;
        }
    }
    
    // 计算统计指标
    if (stats.total_trades > 0) {
        stats.win_rate = static_cast<double>(stats.winning_trades) / stats.total_trades;
        if (stats.winning_trades > 0) {
            stats.avg_win /= stats.winning_trades;
        }
        if (stats.losing_trades > 0) {
            stats.avg_loss /= stats.losing_trades;
        }
        stats.profit_factor = stats.avg_loss != 0 ? 
            (stats.avg_win * stats.winning_trades) / 
            (-stats.avg_loss * stats.losing_trades) : 0;
    }
    
    // 计算最大回撤和夏普比率
    if (!equity_curve.empty()) {
        stats.max_drawdown = calculate_drawdown(equity_curve);
        
        // 计算日收益率用于夏普比率
        std::vector<double> daily_returns;
        for (size_t i = 1; i < equity_curve.size(); ++i) {
            daily_returns.push_back((equity_curve[i] - equity_curve[i-1]) / 
                                   equity_curve[i-1]);
        }
        stats.sharpe_ratio = calculate_sharpe_ratio(daily_returns);
    }
    
    return stats;
}

std::vector<TradingSystem::DailyPnL> TradingSystem::get_daily_pnl(
    const TimePoint& start0, const TimePoint& end0) const 
{
    std::map<std::string, DailyPnL> daily_pnls;
    
    TimePoint end = end0 + std::chrono::hours(1);
    TimePoint start = start0 - std::chrono::hours(1);

    // 获取所有交易记录
    const auto& trades = record_keeper_->get_trades();
    std::cout << "start: " << format_timepoint(start) << std::endl;
    std::cout << "end: " << format_timepoint(end) << std::endl;
    std::cout << "trades size: " << trades.size() << std::endl;

    // 处理每笔交易
    for (const auto& trade : trades) {
        if (trade.timestamp < start || trade.timestamp > end) {
            std::cout << " trade: " << trade.order_id << "[skip]" << std::endl;
            continue;
        }
        std::cout << "trade.timestamp: " << format_timepoint(trade.timestamp) << std::endl;
        
        // 转换为日期字符串
        auto time_t = std::chrono::system_clock::to_time_t(trade.timestamp);
        auto tm = *std::localtime(&time_t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d");
        std::string date = oss.str();
        
        // 确保日期记录存在
        if (daily_pnls.find(date) == daily_pnls.end()) {
            daily_pnls[date] = DailyPnL{date, 0.0, 0.0, 0.0};
        }
        
        // 计算交易的已实现盈亏
        if (trade.type == OrderType::SELL) {
            auto it = positions_.find(trade.symbol);
            if (it != positions_.end()) {
                double realized_pnl = 
                    (trade.price - it->second.avg_price_) * trade.quantity;
                daily_pnls[date].realized_pnl += realized_pnl;
            }
        }
    }
    
    // 为每个有交易的日期更新未实现盈亏
    for (auto& [date, pnl] : daily_pnls) {
        double unrealized_pnl = 0.0;
        for (const auto& [symbol, position] : positions_) {
            unrealized_pnl += position.unrealized_pnl_;
        }
        pnl.unrealized_pnl = unrealized_pnl;
        pnl.total_pnl = pnl.realized_pnl + pnl.unrealized_pnl;
    }
    
    // 转换为vector并排序
    std::vector<DailyPnL> result;
    for (const auto& [date, pnl] : daily_pnls) {
        result.push_back(pnl);
    }
    
    std::sort(result.begin(), result.end(),
              [](const DailyPnL& a, const DailyPnL& b) {
                  return a.date < b.date;
              });
    
    return result;
}

void TradingSystem::deposit(double amount) {
    cash_ += amount;
    
    // Record the deposit transaction
    AccountTransaction transaction{
        .timestamp = std::chrono::system_clock::now(),
        .type = AccountChangeType::DEPOSIT,
        .amount = amount,
        .balance_after = cash_,
        .description = "Deposit"
    };
    record_keeper_->add_account_transaction(transaction);
}

double TradingSystem::calculate_commission(const Order& order) {
    // Simple commission calculation (could be made more sophisticated)
    constexpr double commission_rate = 0.001; // 0.1%
    return order.quantity_ * order.price_ * commission_rate;
}

bool TradingSystem::place_order(const Order& order) {
    // Validate order
    if (order.quantity_ <= 0 || order.price_ <= 0) {
        return false;
    }

    // Generate order ID if not already set
    if (order.order_id_.empty()) {
        order.order_id_ = generate_order_id();
    }

    // Execute the order
    return execute_order(order);
}

bool TradingSystem::execute_buy_order(const Order& order) {
    double commission = calculate_commission(order);
    double total_cost = order.quantity_ * order.price_ + commission;
    
    // Check if we have enough cash
    if (cash_ < total_cost) {
        return false;
    }
    
    // Update cash balance
    cash_ -= total_cost;
    
    // Update position
    auto& position = positions_[order.symbol_];
    double old_quantity = position.quantity_;
    double old_avg_price = position.avg_price_;
    
    // Update average price and quantity
    if (old_quantity == 0) {
        position.avg_price_ = order.price_;
    } else {
        position.avg_price_ = (old_quantity * old_avg_price + 
                             order.quantity_ * order.price_) / 
                             (old_quantity + order.quantity_);
    }
    position.quantity_ += order.quantity_;
    
    return true;
}

void TradingSystem::record_trade_events(
    const Order& order,
    const Position& position,
    const std::string& trade_id,
    double realized_pnl)
{
    // 记录交易
    TradeRecord trade{
        .timestamp = order.timestamp_,
        .symbol = order.symbol_,
        .type = order.type_,
        .quantity = order.quantity_,
        .price = order.price_,
        .commission = calculate_commission(order),
        .total_amount = order.quantity_ * order.price_,
        .order_id = order.order_id_,
        .trade_id = trade_id
    };
    record_keeper_->add_trade(trade);

    // 记录持仓变动
    PositionChange change{
        .timestamp = order.timestamp_,
        .symbol = order.symbol_,
        .quantity_before = position.quantity_ - (order.type_ == OrderType::BUY ? order.quantity_ : -order.quantity_),
        .quantity_after = position.quantity_,
        .avg_price_before = position.avg_price_,
        .avg_price_after = position.avg_price_,
        .realized_pnl = realized_pnl,
        .trade_id = trade_id
    };
    record_keeper_->add_position_change(change);

    // 记录账户变动
    AccountTransaction transaction{
        .timestamp = order.timestamp_,
        .type = AccountChangeType::TRADE,
        .amount = order.type_ == OrderType::BUY ? -trade.total_amount : trade.total_amount,
        .balance_after = cash_,
        .description = (order.type_ == OrderType::BUY ? "Buy " : "Sell ") + 
                      std::to_string(order.quantity_) + " " + order.symbol_
    };
    record_keeper_->add_account_transaction(transaction);
}

void TradingSystem::process_historical_data(
    const std::vector<Order>& orders,
    const std::map<TimePoint, std::map<std::string, double>>& historical_prices
) {
    struct Event {
        TimePoint time;
        enum Type { ORDER, PRICE_UPDATE } type;
        std::optional<Order> order;
        std::map<std::string, double> prices;
    };
    
    std::vector<Event> events;
    
    // 添加订单事件
    for (const auto& order : orders) {
        Event event{
            .time = order.timestamp_,
            .type = Event::ORDER,
            .order = order,
            .prices = {}
        };
        events.push_back(event);
    }
    
    // 添加价格更新事件
    for (const auto& [time, price_map] : historical_prices) {
        Event event{
            .time = time,
            .type = Event::PRICE_UPDATE,
            .order = std::nullopt,
            .prices = price_map
        };
        events.push_back(event);
    }
    
    // 按时间排序所有事件
    std::sort(events.begin(), events.end(),
              [](const Event& a, const Event& b) { return a.time < b.time; });

    for (const auto& event : events) {
        std::cout << "event.time: " << format_timepoint(event.time) 
                  << " event.type: " << std::setw(15) << (event.type == Event::ORDER ? "ORDER" : "PRICE_UPDATE")
                  << " event.order: " << std::setw(10) << (event.order.has_value() ? event.order.value().order_id_ : "null")
                  << " event.prices: " << std::setw(10) << event.prices.size()
                  << std::endl;
    }
    
    // 按时间顺序处理事件
    for (const auto& event : events) {
        if (event.type == Event::ORDER && event.order.has_value()) {
            auto result = place_order(event.order.value());
            if (!result) {
                std::cerr << "Failed to place order: " << event.order.value().order_id_ << std::endl;
            } else {
                std::cout << "Placed order: " << event.order.value().order_id_ 
                          << " time: " << format_timepoint(event.time) 
                          << " type: " << (event.order.value().type_ == OrderType::BUY ? "BUY" : "SELL")
                          << " symbol: " << event.order.value().symbol_
                          << " quantity: " << event.order.value().quantity_
                          << " price: " << event.order.value().price_
                          << " result: " << (result ? "[success]" : "[failed]") 
                          << std::endl;
            }
        } else if (event.type == Event::PRICE_UPDATE) {
            update_unrealized_pnl(event.prices);
        }
    }
}
