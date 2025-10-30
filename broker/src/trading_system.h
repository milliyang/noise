#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <optional>
#include "common_types.h"
#include "trading_records.h"

// Forward declarations
class TradingRecordKeeper;

// 订单类
class Order
{
public:
    Order(const std::string& symbol, OrderType type, double quantity,
          double price, TimePoint timestamp =
              std::chrono::system_clock::now())
        : symbol_(symbol), type_(type), quantity_(quantity), price_(price),
          timestamp_(timestamp), status_(OrderStatus::PENDING) {}

    std::string symbol_;
    OrderType type_;
    double quantity_;
    double price_;
    TimePoint timestamp_;
    OrderStatus status_;
    mutable std::string order_id_;
};

// 持仓类
struct Position {
    double quantity_ = 0;
    double avg_price_ = 0;
    double unrealized_pnl_ = 0;
    double last_price_ = 0;

    Position() = default;
    Position(double qty, double price) 
        : quantity_(qty), avg_price_(price), last_price_(price) {}
};

// 交易系统类
class TradingSystem
{
public:
    TradingSystem();
    ~TradingSystem();

    // 报告结构体
    struct TradingReport {
        double total_pnl = 0;
        double realized_pnl = 0;
        double unrealized_pnl = 0;
        double total_commission = 0;
        int total_trades = 0;
        std::map<std::string, Position> positions;
    };

    // 交易统计结构体
    struct TradeStatistics {
        int total_trades = 0;
        int winning_trades = 0;
        int losing_trades = 0;
        double win_rate = 0;
        double avg_win = 0;
        double avg_loss = 0;
        double profit_factor = 0;
        double max_drawdown = 0;
        double sharpe_ratio = 0;
        std::map<std::string, int> trades_by_symbol;
    };

    // 每日盈亏结构体
    struct DailyPnL {
        std::string date;
        double realized_pnl = 0;
        double unrealized_pnl = 0;
        double total_pnl = 0;
    };

    void deposit(double amount);
    bool place_order(const Order& order);
    void update_unrealized_pnl(const std::map<std::string, double>& market_prices);
    TradingReport generate_report() const;
    void export_trading_records(const std::string& directory) const;

    // 新增方法声明
    TradeStatistics get_trade_statistics(
        const TimePoint& start_time,
        const TimePoint& end_time) const;

    std::vector<DailyPnL> get_daily_pnl(const TimePoint& start, const TimePoint& end) const;

    void process_historical_data(
        const std::vector<Order>& orders,
        const std::map<TimePoint, std::map<std::string, double>>& historical_prices
    );

    // 获取指定股票的持仓信息
    std::optional<Position> get_position(const std::string& symbol) const {
        auto it = positions_.find(symbol);
        if (it != positions_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    // 获取所有持仓信息
    const std::map<std::string, Position>& get_positions() const {
        return positions_;
    }

    // 获取交易记录
    std::vector<TradeRecord> get_trades() const {
        return record_keeper_->get_trades();
    }

private:
    std::unique_ptr<TradingRecordKeeper> record_keeper_;
    std::map<std::string, Position> positions_;
    double cash_;

    bool execute_order(const Order& order);
    bool execute_buy_order(const Order& order);
    bool execute_sell_order(const Order& order);
    std::string generate_trade_id();
    std::string generate_order_id();
    double calculate_commission(const Order& order);

    // 辅助方法
    static std::string format_date(const TimePoint& tp);
    static bool is_same_day(const TimePoint& t1,
                          const TimePoint& t2);
    double calculate_drawdown(const std::vector<double>& equity_curve) const;
    double calculate_sharpe_ratio(const std::vector<double>& daily_returns) const;

    void record_trade_events(
        const Order& order,
        const Position& position,
        const std::string& trade_id,
        double realized_pnl = 0.0
    );
};