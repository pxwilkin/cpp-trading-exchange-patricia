#include "order.h"
#include "order_book.h"
#include <queue>
#include <sstream>

OrderBook::OrderBook(const std::string& ticker, MemPool<Order>& memPool)
    : ticker(ticker), memPool(memPool) {}

void OrderBook::add(const Order& order) {
    if (order.getTicker() != ticker) {
        return;
    }
    
    Order* newOrder = memPool.allocate(
        order.getTicker(),
        order.getSize(),
        order.getSide(),
        order.getType(),
        order.getPrice(),
        order.getClientID(),
        order.getOrderID()
    );

    if (newOrder == nullptr) {
        return;
    }

    if (order.getSide() == Side::BUY) {
        bids.push(newOrder);
    }
    else {
        asks.push(newOrder);
    }
}

void OrderBook::cancel(int orderID) {
    std::priority_queue<Order*, std::vector<Order*>, Order::ForBids> tempBids;
    std::priority_queue<Order*, std::vector<Order*>, Order::ForAsks> tempAsks;

    Order* canceledOrder = nullptr;

    // Search through buy orders
    while (!bids.empty()) {
        Order* order = bids.top();
        bids.pop();

        if (order->getOrderID() != orderID) {
            tempBids.push(order);
        }
        else {
            canceledOrder = order;
        }
    }

    // Search through sell orders
    while (!asks.empty()) {
        Order* order = asks.top();
        asks.pop();

        if (order->getOrderID() != orderID) {
            tempAsks.push(order);
        }
        else {
            canceledOrder = order;
        }
    }

    bids = tempBids;
    asks = tempAsks;
    
    if (canceledOrder != nullptr) {
        memPool.deallocate(canceledOrder);
    }

}

std::string OrderBook::toString() const {
    std::ostringstream oss;

    oss << "Order Book: " << ticker << "\n";

    // Buy orders
    oss << "\nBUY ORDERS:\n";

    auto tempBids = bids;
    while (!tempBids.empty()) {
        oss << "  " << tempBids.top()->toString() << "\n";
        tempBids.pop();
    }

    // sell orders
    oss << "\nSELL ORDERS:\n";

    auto tempAsks = asks;
    while (!tempAsks.empty()) {
        oss << "  " << tempAsks.top()->toString() << "\n";
        tempAsks.pop();
    }

    return oss.str();

}
