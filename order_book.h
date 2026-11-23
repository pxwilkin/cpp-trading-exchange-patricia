#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <string>
#include <queue>
#include <vector>
#include <unordered_map>
#include "order.h"
#include "mem_pool.h"

class OrderBook {
private:
    std::string ticker; 
    MemPool<Order>& memPool;
    
    std::priority_queue<Order*, std::vector<Order*>, Order::ForBids> bids;
    std::priority_queue<Order*, std::vector<Order*>, Order::ForAsks> asks;

    // unordered map for ease of locating orders
    std::unordered_map<int, Order*> ordersByID;

public: 
    OrderBook(const std::string& ticker, MemPool<Order>& memPool);

    // getters
    Order* getBestBid();
    
    Order* getBestAsk();

    // other functions
    void add(const Order& order);

    void cancel(int orderID);

    void update(int orderID, int quantityFilled);

    std::string toString() const;


    // helper functions to remove lazy deleted orders 
    void cleanBids();

    void cleanAsks();

    
};

#endif