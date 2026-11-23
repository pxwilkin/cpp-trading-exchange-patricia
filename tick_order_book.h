#ifndef TICK_ORDER_BOOK_H
#define TICK_ORDER_BOOK_H

#include "order.h"
#include "mem_pool.h"
#include <string>
#include <list>
#include <vector>


struct PriceLevel {
    int totalQuantity = 0;
    std::list<Order*> orders;
};

class TickOrderBook {
private:
    std::string ticker;
    MemPool<Order>& memPool;

    int numTicks;
    double tickSize = 0.01;
    double basePrice;

    std::vector<PriceLevel> buyLevels;
    std::vector<PriceLevel> sellLevels;

    int bestBidIndex;
    int bestAskIndex;

public:
    TickOrderBook(const std::string& ticker, MemPool<Order>& memPool, int numTicks, double basePrice);

    void add(const Order& order);

    void cancel(int orderID);

    std::string toString() const;

};

#endif