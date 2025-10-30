#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "common_types.h"

// 账户变动记录
struct AccountTransaction {
    TimePoint timestamp;
    AccountChangeType type;
    double amount;
    double balance_after;
    std::string description;
};

// 成交记录
struct TradeRecord {
    TimePoint timestamp;
    std::string symbol;
    OrderType type;
    double quantity;
    double price;
    double commission;
    double total_amount;
    std::string order_id;
    std::string trade_id;
};

// 持仓变动记录
struct PositionChange {
    TimePoint timestamp;
    std::string symbol;
    double quantity_before;
    double quantity_after;
    double avg_price_before;
    double avg_price_after;
    double realized_pnl;
    std::string trade_id;
};

class TradingRecordKeeper {
public:
    void add_trade(const TradeRecord& trade);
    void add_position_change(const PositionChange& change);
    void add_account_transaction(const AccountTransaction& transaction);
    const std::vector<TradeRecord>& get_trades() const { return trades_; }
    void export_to_csv(const std::string& directory) const;
    const std::vector<PositionChange>& get_position_changes() const {
        return position_changes_;
    }

private:
    std::vector<TradeRecord> trades_;
    std::vector<PositionChange> position_changes_;
    std::vector<AccountTransaction> account_transactions_;
};

