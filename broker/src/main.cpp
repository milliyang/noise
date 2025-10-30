#include "trading_system.h"
#include "trading_records.h"
//#include "position_change.h"
//#include "account_transaction.h"

#include <iostream>
#include <chrono>
#include <iomanip>
#include "utils.h"

int main()
{
    TradingSystem system;

    // 存入初始资金
    system.deposit(1000000.0);  // 存入100万初始资金

    // 创建时间点
    auto now = std::chrono::system_clock::now();
    auto two_days_ago = now - std::chrono::hours(48);
    auto yesterday = now - std::chrono::hours(24);

    // 准备历史订单
    std::vector<Order> historical_orders = {
        {"AAPL", OrderType::BUY, 100, 150.0, two_days_ago},                    // 成本 15000
        {"GOOGL", OrderType::BUY, 50, 2700.0, two_days_ago},                   // 成本 135000
        {"AAPL", OrderType::SELL, 50, 155.0, yesterday},                       // 收入 7750, 盈利 250
        {"AMZN", OrderType::BUY, 20, 135.0, yesterday}                         // 成本 2700
    };

    //add date order 2025-01-03 22:46:51
    TimePoint order_time = get_timepoint_from_string("2025-01-03 22:46:51");
    historical_orders.push_back({"AAPL", OrderType::BUY, 100, 150.0, order_time});

    // 准备历史价格数据
    std::map<TimePoint, std::map<std::string, double>> historical_prices;
    
    // 两天前的价格更新
    historical_prices[two_days_ago + std::chrono::hours(4)] = {
        {"AAPL", 152.0},    // +200 未实现
        {"GOOGL", 2720.0}   // +1000 未实现
    };

    //add date order 2025-01-03 22:46:51
    historical_prices[order_time] = {
        {"AAPL", 152.0},    // +200 未实现
        {"GOOGL", 2720.0}   // +1000 未实现
    };

    TimePoint order_time2 = get_timepoint_from_string("2025-01-04 22:46:51");
    historical_prices[order_time2] = {
        {"AAPL", 152.0},    // +200 未实现
        {"GOOGL", 2720.0}   // +1000 未实现
    };

    // 昨天的价格更新
    historical_prices[yesterday + std::chrono::hours(4)] = {
        {"AAPL", 155.0},    // +250 已实现, +150 未实现
        {"GOOGL", 2750.0},  // +2500 未实现
        {"AMZN", 137.0}     // +40 未实现
    };

    // 今天的价格更新
    historical_prices[now] = {
        {"AAPL", 157.0},    // +350 未实现
        {"GOOGL", 2770.0},  // +3500 未实现
        {"AMZN", 140.0}     // +100 未实现
    };

    // 处理历史数据
    system.process_historical_data(historical_orders, historical_prices);

    // 生成报告
    auto report = system.generate_report();
    std::cout << "\nTrading Report:\n"
              << std::fixed << std::setprecision(2)
              << "Total PnL: " << report.total_pnl << "\n"
              << "Realized PnL: " << report.realized_pnl << "\n"
              << "Unrealized PnL: " << report.unrealized_pnl << "\n\n";


    // 获取每日收益统计
    auto daily_pnl = system.get_daily_pnl(order_time, now);
    std::cout << "\nDaily PnL Report:\n";
    for (const auto& pnl : daily_pnl) {
        std::cout << "Date: " << pnl.date 
                  << ", Realized: " << std::fixed << std::setprecision(2) << pnl.realized_pnl
                  << ", Unrealized: " << pnl.unrealized_pnl
                  << ", Total: " << pnl.total_pnl << "\n";
    }
    std::cout << "\n";

    // 打印当前持仓
    std::cout << "\nCurrent Positions:\n";
    for (const auto& [symbol, pos] : report.positions) {
        std::cout << "Symbol: " << symbol 
                  << ", Quantity: " << pos.quantity_
                  << ", Avg Price: " << std::fixed << std::setprecision(2) << pos.avg_price_
                  << ", Current Price: " << pos.last_price_
                  << ", Unrealized PnL: " << pos.unrealized_pnl_ << "\n";
    }

    system.export_trading_records("./export");

    return 0;
}
