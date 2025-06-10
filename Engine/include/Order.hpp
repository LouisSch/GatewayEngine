#pragma once

#include <string>
#include <chrono>

enum class Side { BUY, SELL };

struct Order {
    std::string id;
    std::string symbol;
    int quantity;
    double price;
    Side side;
    std::chrono::steady_clock::time_point timestamp;
};