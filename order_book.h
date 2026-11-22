#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <string>
#include <queue>
#include <vector>
#include "order.h"

class OrderBook {
private:
    std::string ticker; 

    std::priority_queue<Order, std::vector<Order>, Order::ForBids> bids;
    std::priority_queue<Order, std::vector<Order>, Order::ForAsks> asks;

public: 
    OrderBook(const std::string& ticker);

    void add(const Order& order);

    void cancel(int orderID);

    std::string toString() const;

};

#endif