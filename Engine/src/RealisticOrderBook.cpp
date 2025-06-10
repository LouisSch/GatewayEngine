#include "OrderBook.hpp"

void RealisticOrderBook::matchBuy(Order& order) {
    for (auto it = sellSide.begin(); it != sellSide.end() && order.quantity > 0; ) {
        if (it->first > order.price) break;

        auto& queue = it->second;
        while (!queue.empty() && order.quantity > 0) {
            Order& sellOrder = queue.front();

            if (sellOrder.symbol != order.symbol) break;
            
            int tradedAmount = std::min(sellOrder.quantity, order.quantity);
            std::cout << "MATCH: " << order.id << " x " << sellOrder.id << " @ " << it->first << " for " << tradedAmount << "units\n";

            order.quantity -= tradedAmount;
            sellOrder.quantity -= tradedAmount;

            if (sellOrder.quantity == 0) queue.pop_front();
        }

        if (queue.empty())
            it = sellSide.erase(it);
        else
            it++;
    }

    if (order.quantity > 0)
        buySide[order.price].push_back(std::move(order));
}

void RealisticOrderBook::matchSell(Order& order) {
    for (auto it = buySide.begin(); it != buySide.end() && order.quantity > 0; ) {
        if (it->first < order.price) break;

        auto& queue = it->second;
        while (!queue.empty() && order.quantity > 0) {
            Order& buyOrder = queue.front();

            if (buyOrder.symbol != order.symbol) break;
            
            int tradedAmount = std::min(buyOrder.quantity, order.quantity);
            std::cout << "MATCH: " << order.id << " x " << buyOrder.id << " @ " << it->first << " for " << tradedAmount << "units\n";

            order.quantity -= tradedAmount;
            buyOrder.quantity -= tradedAmount;

            if (buyOrder.quantity == 0) queue.pop_front();
        }

        if (queue.empty())
            it = buySide.erase(it);
        else
            it++;
    }

    if (order.quantity > 0)
        sellSide[order.price].push_back(std::move(order));
}

void RealisticOrderBook::addOrder(const Order& order) {
    if (order.quantity <= 0) return;

    Order workingCopy = order;

    if (order.side == Side::BUY)
        matchBuy(workingCopy);
    else
        matchSell(workingCopy);
}

size_t RealisticOrderBook::totalOrders() const {
    size_t total = 0;

    for (const auto& [_, q] : buySide) total += q.size();
    for (const auto& [_, q] : sellSide) total += q.size();

    return total;
}

void RealisticOrderBook::printBook() const {
    std::cout << "---- SELL ORDERS ----\n";
    for (const auto& [price, queue] : sellSide) {
        for (const auto& order : queue) {
            std::cout << order.symbol << " SELL " << order.quantity << " @ " << price << std::endl;
        }
    }

    std::cout << "---- BUY ORDERS ----\n";
    for (const auto& [price, queue] : buySide) {
        for (const auto& order : queue) {
            std::cout << order.symbol << " BUY " << order.quantity << " @ " << price << std::endl;
        }
    }
}