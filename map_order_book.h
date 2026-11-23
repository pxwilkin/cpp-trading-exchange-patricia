#ifndef MAP_ORDER_BOOK_H
#define MAP_ORDER_BOOK_H
#include "order.h"
#include "mem_pool.h"
#include <map>
#include <list>


struct PriceLevel {
    int totalQuantity = 0;
    std::list<Order*> orders;
};

class MapOrderBook {
private:
    std::string ticker; 
    MemPool<Order>& memPool;
    
    std::map<double, PriceLevel> buyOrders;
    std::map<double, PriceLevel> sellOrders;

public:
    MapOrderBook(const std::string& ticker, MemPool<Order>& memPool);

    void add(const Order& order);

    void cancel(int orderID);

    std::string toString() const;

};


#endif