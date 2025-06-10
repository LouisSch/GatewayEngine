#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <deque>

#include "Order.hpp"

class IOrderBook {
public:
    virtual ~IOrderBook() = default;
    virtual void addOrder(const Order& order) = 0;
    virtual size_t totalOrders() const = 0;
    virtual void printBook() const = 0;
};

class RealisticOrderBook : public IOrderBook {
private:
    std::map<double, std::deque<Order>, std::greater<>> buySide;
    std::map<double, std::deque<Order>> sellSide;

    void matchBuy(Order& order);
    void matchSell(Order& order);
public:
    void addOrder(const Order& order) override;
    size_t totalOrders() const override;
    void printBook() const override;
};

class NaiveOrderBook : public IOrderBook {
private:
    std::vector<Order> buyOrders;
    std::vector<Order> sellOrders;

    void match();
public:
    void addOrder(const Order& order) override;
    size_t totalOrders() const override;
    void printBook() const override;
};