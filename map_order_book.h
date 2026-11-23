#ifndef MAP_ORDER_BOOK_H
#define MAP_ORDER_BOOK_H
#include "order.h"
#include "mem_pool.h"
#include <map>
#include <list>


struct MapPriceLevel {
    int totalQuantity = 0;
    std::list<Order*> orders;
};

class MapOrderBook {
private:
    std::string ticker; 
    MemPool<Order>& memPool;
    
    std::map<double, MapPriceLevel> buyOrders;
    std::map<double, MapPriceLevel> sellOrders;

public:
    MapOrderBook(const std::string& ticker, MemPool<Order>& memPool);

    void add(const Order& order);

    void cancel(int orderID);

    std::string toString() const;

};


#endif