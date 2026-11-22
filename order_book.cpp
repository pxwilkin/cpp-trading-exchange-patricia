#include "order.h"
#include "order_book.h"
#include <queue>
#include <sstream>

OrderBook::OrderBook(const std::string& ticker) 
    : ticker(ticker) {}


void OrderBook::add(const Order& order) {
    if (order.getTicker() != ticker) {
        return;
    }

    if (order.getSide() == Side::BUY) {
        bids.push(order);
    }
    else {
        asks.push(order);
    }
}

void OrderBook::cancel(int orderID) {
    std::priority_queue<Order, std::vector<Order>, Order::ForBids> tempBids;
    std::priority_queue<Order, std::vector<Order>, Order::ForAsks> tempAsks;

    // Search through buy orders
    while (!bids.empty()) {
        Order order = bids.top();
        bids.pop();

        if (order.getOrderID() != orderID) {
            tempBids.push(order);
        }
    }

    // Search through sell orders
    while (!asks.empty()) {
        Order order = asks.top();
        asks.pop();

        if (order.getOrderID() != orderID) {
            tempAsks.push(order);
        }
    }

    bids = tempBids;
    asks = tempAsks;
}

std::string OrderBook::toString() const {
    std::ostringstream oss;

    oss << "Order Book: " << ticker << "\n";

    // Buy orders
    oss << "\nBUY ORDERS:\n";

    auto tempBids = bids;
    while (!tempBids.empty()) {
        oss << tempBids.top().toString() << "\n";
        tempBids.pop();
    }

    // sell orders
    oss << "\nSELL ORDERS:\n";

    auto tempAsks = asks;
    while (!tempAsks.empty()) {
        oss << tempAsks.top().toString() << "\n";
        tempAsks.pop();
    }

    return oss.str();
    
}
