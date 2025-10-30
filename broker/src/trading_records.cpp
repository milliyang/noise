#include "trading_records.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include "utils.h"
#include <iostream>

// Helper function to create directory
void create_directory(const std::string& path) {
#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0777);
#endif
}

void TradingRecordKeeper::add_trade(const TradeRecord& trade)
{
    std::cout << "add_trade: " << format_timepoint(trade.timestamp) 
              << " sym:" << trade.symbol 
              << " type:" << (trade.type == OrderType::BUY ? "BUY" : "SELL") 
              << " qty:" << trade.quantity 
              << " price:" << trade.price 
              << " com:" << trade.commission 
              << " total:" << trade.total_amount 
              << " oid:" << trade.order_id 
              << " tid:" << trade.trade_id 
              << std::endl;
    trades_.push_back(trade);
}

void TradingRecordKeeper::add_position_change(const PositionChange& change)
{
    position_changes_.push_back(change);
}

void TradingRecordKeeper::add_account_transaction(
    const AccountTransaction& transaction)
{
    account_transactions_.push_back(transaction);
}

void TradingRecordKeeper::export_to_csv(const std::string& directory) const
{
    // Create directory if it doesn't exist
    create_directory(directory);

    // Export trades
    {
        std::ofstream trade_file(directory + "/trades.csv");
        trade_file << "Timestamp,Symbol,Type,Quantity,Price,Commission,TotalAmount,OrderID,TradeID\n";

        for (const auto& trade : trades_) {
            auto time = std::chrono::system_clock::to_time_t(trade.timestamp);
            trade_file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
                       << "," << trade.symbol
                       << "," << (trade.type == OrderType::BUY ? "BUY" : "SELL")
                       << "," << trade.quantity
                       << "," << trade.price
                       << "," << trade.commission
                       << "," << trade.total_amount
                       << "," << trade.order_id
                       << "," << trade.trade_id
                       << "\n";
        }
    }

    // Export position changes
    {
        std::ofstream position_file(directory + "/position_changes.csv");
        position_file << "Timestamp,Symbol,QuantityBefore,QuantityAfter,AvgPriceBefore,AvgPriceAfter,RealizedPnL,TradeID\n";

        for (const auto& change : position_changes_) {
            auto time = std::chrono::system_clock::to_time_t(change.timestamp);
            position_file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
                         << "," << change.symbol
                         << "," << change.quantity_before
                         << "," << change.quantity_after
                         << "," << change.avg_price_before
                         << "," << change.avg_price_after
                         << "," << change.realized_pnl
                         << "," << change.trade_id
                         << "\n";
        }
    }

    // Export account transactions
    {
        std::ofstream account_file(directory + "/account_transactions.csv");
        account_file << "Timestamp,Type,Amount,BalanceAfter,Description\n";

        for (const auto& trans : account_transactions_) {
            auto time = std::chrono::system_clock::to_time_t(trans.timestamp);
            account_file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
                        << "," << static_cast<int>(trans.type)
                        << "," << trans.amount
                        << "," << trans.balance_after
                        << "," << trans.description
                        << "\n";
        }
    }
}