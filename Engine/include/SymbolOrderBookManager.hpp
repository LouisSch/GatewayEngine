#pragma once

#include <mutex>
#include <unordered_map>
#include "OrderBook.hpp"

class SymbolOrderBookManager {
private:
    std::unordered_map<std::string, std::unique_ptr<IOrderBook>> books;
    std::unordered_map<std::string, std::mutex> bookMutexes;
    std::mutex mapMutex;
public:
    void addOrder(const Order& order);
};