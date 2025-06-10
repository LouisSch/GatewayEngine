#include "SymbolOrderBookManager.hpp"

void SymbolOrderBookManager::addOrder(const Order& order) {
    IOrderBook* book;

    {
        std::lock_guard<std::mutex> lock(mapMutex);
        if (!books.contains(order.symbol)) {
            books[order.symbol] = std::make_unique<RealisticOrderBook>();
            bookMutexes[order.symbol];
        }
        book = books[order.symbol].get();
    }

    std::lock_guard<std::mutex> lock(bookMutexes[order.symbol]);
    book->addOrder(order);
}