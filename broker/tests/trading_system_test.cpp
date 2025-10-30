#include <gtest/gtest.h>
#include "trading_system.h"
#include <iostream>
#include <iomanip>

class TradingSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        system.deposit(1000000.0);  // 初始资金100万
    }

    // Helper function to print position details
    void PrintPosition(const std::string& symbol) {
        auto position = system.get_position(symbol);
        if (position) {
            std::cout << "Position for " << symbol << ": "
                      << " Quantity: " << position->quantity_ << " "
                      << " Average Price: " << position->avg_price_ << " "
                      << " Last Price: " << position->last_price_ << " "
                      << " Unrealized PnL: " << position->unrealized_pnl_ << " ";
            
            // 计算已实现盈亏
            double realized_pnl = 0.0;
            const auto& trades = system.get_trades();
            for (const auto& trade : trades) {
                if (trade.symbol == symbol && trade.type == OrderType::SELL) {
                    realized_pnl += 
                        (trade.price - position->avg_price_) * trade.quantity;
                }
            }
            std::cout << "Realized PnL: " << realized_pnl << "\n";
        } else {
            std::cout << "No position found for " << symbol << "\n";
        }
    }

    TradingSystem system;
};

// Test multiple buy and sell operations for the same stock
TEST_F(TradingSystemTest, MultipleBuySellOperations) {
    const std::string symbol = "AAPL";
    
    // First buy: 100 shares at $150
    Order buy1(symbol, OrderType::BUY, 100, 150.0);
    ASSERT_TRUE(system.place_order(buy1));
    PrintPosition(symbol);

    // Update price to $155
    system.update_unrealized_pnl({{symbol, 155.0}});
    PrintPosition(symbol);
    
    // Second buy: 50 shares at $155
    Order buy2(symbol, OrderType::BUY, 50, 155.0);
    ASSERT_TRUE(system.place_order(buy2));
    PrintPosition(symbol);

    // Partial sell: 75 shares at $160
    Order sell1(symbol, OrderType::SELL, 75, 160.0);
    ASSERT_TRUE(system.place_order(sell1));
    PrintPosition(symbol);

    // Update price to $145 (price drop)
    system.update_unrealized_pnl({{symbol, 145.0}});
    PrintPosition(symbol);

    // Buy more at lower price: 100 shares at $145
    Order buy3(symbol, OrderType::BUY, 100, 145.0);
    ASSERT_TRUE(system.place_order(buy3));
    PrintPosition(symbol);

    // Update price to $165 (price spike)
    system.update_unrealized_pnl({{symbol, 165.0}});
    PrintPosition(symbol);

    // Sell half of position at high price
    auto report = system.generate_report();
    auto pos = report.positions.find(symbol);
    ASSERT_NE(pos, report.positions.end());
    Order sell2(symbol, OrderType::SELL, pos->second.quantity_ / 2, 165.0);
    ASSERT_TRUE(system.place_order(sell2));
    PrintPosition(symbol);

    // Final position check
    report = system.generate_report();
    pos = report.positions.find(symbol);
    ASSERT_NE(pos, report.positions.end());
    
    // Print final trading statistics
    auto stats = system.get_trade_statistics(
        std::chrono::system_clock::now() - std::chrono::hours(24),
        std::chrono::system_clock::now()
    );
    
    std::cout << "\nFinal Trading Statistics:\n"
              << "Total Trades: " << stats.total_trades << "\n"
              << "Win Rate: " << (stats.win_rate * 100) << "%\n"
              << "Profit Factor: " << stats.profit_factor << "\n"
              << "Max Drawdown: " << (stats.max_drawdown * 100) << "%\n"
              << "Sharpe Ratio: " << stats.sharpe_ratio << "\n";
}

// Test edge cases
TEST_F(TradingSystemTest, EdgeCases) {
    const std::string symbol = "AAPL";
    
    // Test buying with insufficient funds
    Order big_buy(symbol, OrderType::BUY, 1000000, 150.0);
    ASSERT_FALSE(system.place_order(big_buy));

    // Test selling without position
    Order invalid_sell(symbol, OrderType::SELL, 100, 150.0);
    ASSERT_FALSE(system.place_order(invalid_sell));

    // Test zero quantity order
    Order zero_qty(symbol, OrderType::BUY, 0, 150.0);
    ASSERT_FALSE(system.place_order(zero_qty));

    // Test negative price order
    Order neg_price(symbol, OrderType::BUY, 100, -150.0);
    ASSERT_FALSE(system.place_order(neg_price));
}

// Test position averaging and PnL calculations
TEST_F(TradingSystemTest, PositionAveraging) {
    const std::string symbol = "AAPL";
    
    // Buy 100 shares at $150
    ASSERT_TRUE(system.place_order(Order(symbol, OrderType::BUY, 100, 150.0)));
    
    // Buy 100 shares at $160
    ASSERT_TRUE(system.place_order(Order(symbol, OrderType::BUY, 100, 160.0)));
    
    auto report = system.generate_report();
    auto pos = report.positions.find(symbol);
    ASSERT_NE(pos, report.positions.end());
    
    // Average price should be $155
    EXPECT_NEAR(pos->second.avg_price_, 155.0, 0.01);
    
    // Update market price to $165
    system.update_unrealized_pnl({{symbol, 165.0}});
    
    report = system.generate_report();
    pos = report.positions.find(symbol);
    
    // Unrealized PnL should be (165 - 155) * 200 = 2000
    EXPECT_NEAR(pos->second.unrealized_pnl_, 2000.0, 0.01);
}

// Test rapid trading scenario
TEST_F(TradingSystemTest, RapidTrading) {
    const std::string symbol = "AAPL";
    std::vector<double> prices = {150.0, 152.0, 151.0, 153.0, 152.5, 154.0, 153.5, 155.0};
    
    for (size_t i = 0; i < prices.size(); ++i) {
        // Alternate between buying and selling
        if (i % 2 == 0) {
            ASSERT_TRUE(system.place_order(Order(symbol, OrderType::BUY, 100, prices[i])));
        } else {
            ASSERT_TRUE(system.place_order(Order(symbol, OrderType::SELL, 50, prices[i])));
        }
        
        // Update market price after each trade
        system.update_unrealized_pnl({{symbol, prices[i]}});
        PrintPosition(symbol);
    }
}

TEST_F(TradingSystemTest, HistoricalTrading) {
    auto now = std::chrono::system_clock::now();
    auto day_ago = now - std::chrono::hours(24);
    
    std::vector<Order> orders = {
        {"AAPL", OrderType::BUY, 100, 150.0, day_ago},  // 买入成本 15000
    };
    
    std::map<TimePoint, std::map<std::string, double>> prices = {
        {day_ago + std::chrono::hours(4), {{"AAPL", 155.0}}},  // 未实现盈利 500
        {now, {{"AAPL", 160.0}}}  // 未实现盈利 1000
    };
    
    system.process_historical_data(orders, prices);
    
    auto report = system.generate_report();
    EXPECT_NEAR(report.unrealized_pnl, 1000.0, 0.01);
    EXPECT_NEAR(report.realized_pnl, 0.0, 0.01);
}

TEST_F(TradingSystemTest, BuyAndSellTest) {
    system.deposit(10000.0);
    
    // 买入
    Order buy_order("AAPL", OrderType::BUY, 100, 150.0);
    EXPECT_TRUE(system.place_order(buy_order));
    
    auto position = system.get_position("AAPL");
    ASSERT_TRUE(position.has_value());
    EXPECT_EQ(position->quantity_, 100);
    EXPECT_EQ(position->avg_price_, 150.0);
    EXPECT_EQ(position->unrealized_pnl_, 0.0);
    
    // 更新价格，检查未实现盈亏
    std::map<std::string, double> prices{{"AAPL", 160.0}};
    system.update_unrealized_pnl(prices);
    
    position = system.get_position("AAPL");
    ASSERT_TRUE(position.has_value());
    EXPECT_EQ(position->unrealized_pnl_, 1000.0);  // (160-150)*100
    
    // 卖出
    Order sell_order("AAPL", OrderType::SELL, 50, 160.0);
    EXPECT_TRUE(system.place_order(sell_order));
    system.update_unrealized_pnl(prices);              //bugfix: force update unrealized pnl

    // 检查持仓和盈亏
    position = system.get_position("AAPL");
    ASSERT_TRUE(position.has_value());
    EXPECT_EQ(position->quantity_, 50);
    EXPECT_EQ(position->avg_price_, 150.0);
    EXPECT_EQ(position->unrealized_pnl_, 500.0);    //(160-150)*50
    EXPECT_EQ(position->last_price_, 160.0);        // 160.0

    // 生成报告检查已实现盈亏
    auto report = system.generate_report();

    EXPECT_EQ(report.realized_pnl, 500.0);  // (160-150)*50
    EXPECT_EQ(report.unrealized_pnl, 500.0);  // (160-150)*50
    EXPECT_EQ(report.total_pnl, 1000.0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 