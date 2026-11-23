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
    
    Order* newOrder = memPool.allocate(order);

    if (newOrder == nullptr) {
        throw std::runtime_error("Memory pool is full");
    }

    ordersByID[newOrder->getOrderID()] = newOrder;
    
    if (order.getSide() == Side::BUY) {
        bids.push(newOrder);
    }
    else {
        asks.push(newOrder);
    }
}

void OrderBook::cancel(int orderID) {
    auto it = ordersByID.find(orderID);

    if (it == ordersByID.end()) {
        throw std::invalid_argument("Order ID not found");
    }

    it->second->cancel();
    ordersByID.erase(it);
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

void OrderBook::cleanBids() {
    while (!bids.empty() && bids.top()->isCancelled()) {
        Order* cancelledOrder = bids.top();
        bids.pop();
        memPool.deallocate(cancelledOrder);
    }
}

void OrderBook::cleanAsks() {
    while (!asks.empty() && asks.top()->isCancelled()) {
        Order* cancelledOrder = asks.top();
        asks.pop();
        memPool.deallocate(cancelledOrder);
    }
}