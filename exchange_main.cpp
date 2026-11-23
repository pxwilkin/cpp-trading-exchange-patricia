
#include "order.h"
#include "order_book.h"
#include "mem_pool.h"
#include "exchange_main.h"

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <csignal>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <functional>

int main() {
    MemPool<Order> pool(10000);
    OrderBook book("AAPL", pool);
    MatchingEngine engine(book);

    // Client 1: submits sell orders
    std::thread client1([&engine]() {
        for (int i = 0; i < 5; i++) {
            Order order("AAPL", 10, Side::SELL, OrderType::LIMIT,
                        150.00 + i, 1, 100 + i);

            engine.placeOrder(order);
        }
    });

    // Client 2: submits buy orders
    std::thread client2([&engine]() {
        for (int i = 0; i < 5; i++) {
            Order order("AAPL", 10, Side::BUY, OrderType::LIMIT,
                        150.00, 2, 200 + i);

            engine.placeOrder(order);
        }
    });

    // Client 3: submits additional sell orders
    std::thread client3([&engine]() {
        for (int i = 0; i < 5; i++) {
            Order order("AAPL", 5, Side::SELL, OrderType::LIMIT,
                        149.00, 3, 300 + i);

            engine.placeOrder(order);
        }
    });

    // Client 4: submits additional buy orders
    std::thread client4([&engine]() {
        for (int i = 0; i < 5; i++) {
            Order order("AAPL", 5, Side::BUY, OrderType::LIMIT,
                        151.00, 4, 400 + i);

            engine.placeOrder(order);
        }
    });

    // Wait for all clients to finish
    client1.join();
    client2.join();
    client3.join();
    client4.join();

    // Print remaining orders
    std::cout << book.toString() << std::endl;

    return 0;
}
