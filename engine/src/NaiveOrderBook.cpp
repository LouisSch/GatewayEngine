#include "OrderBook.hpp"

void NaiveOrderBook::match() {
    std::vector<Order> remainingBuys;
    for (auto& buy : buyOrders) {
        int remainingQty = buy.quantity;
        for (auto it = sellOrders.begin(); it != sellOrders.end() && remainingQty > 0; ) {
            if (it->symbol == buy.symbol && it->price <= buy.price) {
                int tradedQty = std::min(it->quantity, buy.quantity);
                std::cout << "MATCH: " << buy.id << " x " << it->id << " @ " << it->price << " for " << tradedQty << " units" << std::endl;

                remainingQty -= tradedQty;
                it->quantity -= tradedQty;

                if (it->quantity == 0) it = sellOrders.erase(it);
                else it++;

                break;
            } else it++;
        }
        
        if (remainingQty > 0) {
            Order leftover = buy;
            leftover.quantity = remainingQty;
            remainingBuys.push_back(leftover);
        }
    }
    buyOrders = std::move(remainingBuys);
}

void NaiveOrderBook::addOrder(const Order& order) {
    if (order.side == Side::BUY) buyOrders.push_back(order);
    else sellOrders.push_back(order);

    match();
}

size_t NaiveOrderBook::totalOrders() const {
    return buyOrders.size() + sellOrders.size();
}

void NaiveOrderBook::printBook() const {
    std::cout << "Buy Orders:\n";
    for (const auto& order : buyOrders) {
        std::cout << order.id << " " << order.symbol << " " << order.price << " " << order.quantity << std::endl;
    }

    std::cout << "Sell Orders:\n";
    for (const auto& order : sellOrders) {
        std::cout << order.id << " " << order.symbol << " " << order.price << " " << order.quantity << std::endl;
    }
}