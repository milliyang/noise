#pragma once

#include <chrono>
#include <string>

// 订单类型
enum class OrderType {
    BUY,
    SELL
};

// 订单状态
enum class OrderStatus {
    PENDING,
    FILLED,
    REJECTED
};

// 账户变动类型
enum class AccountChangeType {
    DEPOSIT,
    WITHDRAW,
    TRADE,
    FEE,
    INTEREST,
    DIVIDEND
}; 

using TimePoint = std::chrono::system_clock::time_point;