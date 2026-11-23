#include "tick_order_book.h"
#include "mem_pool.h"
#include "order.h"
#include <sstream>
#include <cmath>

TickOrderBook::TickOrderBook(const std::string& ticker, MemPool<Order>& memPool, 
    int numTicks, double basePrice)
    : ticker(ticker), memPool(memPool), numTicks(numTicks), basePrice(basePrice), 
    buyLevels(numTicks), sellLevels(numTicks), bestBidIndex(-1), bestAskIndex(numTicks) {}

void TickOrderBook::add(const Order& order) {
    // Check ticker
    if (order.getTicker() != ticker) {
        throw std::invalid_argument("Order ticker does not match order book");
    }

    // Calculate array index
    double price = order.getPrice();

    int index = static_cast<int>(
        std::round((price - basePrice) / tickSize)
    );

     // Check that price is within the supported range
    if (index < 0 || index >= numTicks) {
        throw std::out_of_range("Order price is outside the supported range");
    }

    // Check that price is aligned with the tick size
    double expectedPrice = basePrice + index * tickSize;

    if (std::abs(price - expectedPrice) > 1e-8) {
        throw std::invalid_argument("Order price does not match tick size");
    }


    // Allocate order from memory pool
    Order* newOrder = memPool.allocate(order);

    if (newOrder == nullptr) {
        throw std::runtime_error("Memory pool is full");
    }

    // Add order to appropriate side
    if (order.getSide() == Side::BUY) {

        buyLevels[index].orders.push_back(newOrder);

        buyLevels[index].totalQuantity += order.getSize();

        // Update best bid
        if (bestBidIndex == -1 || index > bestBidIndex) {
            bestBidIndex = index;
        }

    } 
    else {

        sellLevels[index].orders.push_back(newOrder);

        sellLevels[index].totalQuantity += order.getSize();

        // Update best ask
        if (bestAskIndex == -1 || index < bestAskIndex) {
            bestAskIndex = index;
        }
    }
}


void TickOrderBook::cancel(int orderID) {
    // Search through buy orders
    for (int i = 0; i < numTicks; i++) 
    {
        for (auto it = buyLevels[i].orders.begin(); it != buyLevels[i].orders.end(); it++) 
        {
            if ((*it)->getOrderID() == orderID) 
            {
                buyLevels[i].totalQuantity -= (*it)->getSize();
                memPool.deallocate(*it);
                buyLevels[i].orders.erase(it);

                //update best bid 
                if (buyLevels[i].orders.empty() && i == bestBidIndex) 
                {
                    bestBidIndex = -1;
                    for (int j = numTicks - 1; j >= 0; j--) 
                    {
                        if (!buyLevels[j].orders.empty()) 
                        {
                            bestBidIndex = j;
                            break;
                        }
                    }
                }

                return;
            }
        }
    }

    // Search through sell orders
for (int i = 0; i < numTicks; i++) {
        for (auto it = sellLevels[i].orders.begin();
             it != sellLevels[i].orders.end(); ++it) {

            if ((*it)->getOrderID() == orderID) {

                sellLevels[i].totalQuantity -= (*it)->getSize();

                memPool.deallocate(*it);

                sellLevels[i].orders.erase(it);

                // Update best ask if  level is now empty
                if (sellLevels[i].orders.empty() && i == bestAskIndex) {
                    bestAskIndex = -1;

                    for (int j = 0; j < numTicks; j++) {
                        if (!sellLevels[j].orders.empty()) {
                            bestAskIndex = j;
                            break;
                        }
                    }
                }

                return;
            }
        }
    }

    throw std::invalid_argument("Order ID not found");

}

std::string TickOrderBook::toString() const {
    std::ostringstream oss;

    oss << "Tick-Based Array Order Book: " << ticker << "\n";

    // Buy orders
    oss << "\nBUY ORDERS:\n";
    
    for (int i = numTicks - 1; i >= 0; i--) {
        if (!buyLevels[i].orders.empty()) {
            for (auto it = buyLevels[i].orders.begin(); it != buyLevels[i].orders.end(); ++it) {
                oss << "  " << (*it)->toString() << "\n";
            }
        }
    }

 

    // sell orders
    oss << "\nSELL ORDERS:\n";
    
    for (int i = 0; i < numTicks; i++) {
        if (!sellLevels[i].orders.empty()) {
            for (auto it = sellLevels[i].orders.begin(); it != buyLevels[i].orders.end(); ++it) {
                oss << "  " << (*it)->toString() << "\n";
            }
        }
    }

    return oss.str();

}
