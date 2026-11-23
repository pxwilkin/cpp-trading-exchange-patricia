#include "map_order_book.h"
#include <sstream>

MapOrderBook::MapOrderBook(const std::string& ticker, MemPool<Order>& memPool) 
    : ticker(ticker), memPool(memPool) {}

void MapOrderBook::add(const Order& order) {
    if (order.getTicker() != ticker) {
        throw std::invalid_argument("Order ticker does not match order book");
    }

    double price = order.getPrice();

    // Allocate memory for the new order
    Order* newOrder = memPool.allocate(order);

    if (newOrder == nullptr) {
        throw std::runtime_error("Memory pool is full");
    }

    // Add to buy
    if (order.getSide() == Side::BUY) {
        buyOrders[price].orders.push_back(newOrder);
        buyOrders[price].totalQuantity += order.getSize();
    } 
    // Add to sell
    else {
        sellOrders[price].orders.push_back(newOrder);
        sellOrders[price].totalQuantity += order.getSize();
    }

}

void MapOrderBook::cancel(int orderID) {

    // Search buy orders
    for (auto priceIt = buyOrders.begin(); priceIt != buyOrders.end(); ++priceIt) {
        for (auto orderIt = priceIt->second.orders.begin(); orderIt != priceIt->second.orders.end(); ++orderIt) {
            if ((*orderIt)->getOrderID() == orderID) {
                Order* canceledOrder = *orderIt;

                priceIt->second.totalQuantity -= canceledOrder->getSize();
                priceIt->second.orders.erase(orderIt);

                memPool.deallocate(canceledOrder);

                // If no orders left at price point, delete key
                if (priceIt->second.orders.empty()) {
                    buyOrders.erase(priceIt);
                }

                return;
            }
        }
    }

    // Search sell orders
    for (auto priceIt = sellOrders.begin(); priceIt != sellOrders.end(); ++priceIt) {
        for (auto orderIt = priceIt->second.orders.begin();
             orderIt != priceIt->second.orders.end(); ++orderIt) {

            if ((*orderIt)->getOrderID() == orderID) {
                Order* canceledOrder = *orderIt;

                priceIt->second.totalQuantity -= canceledOrder->getSize();
                priceIt->second.orders.erase(orderIt);

                memPool.deallocate(canceledOrder);

                // If no orders left at price point, delete key
                if (priceIt->second.orders.empty()) {
                    sellOrders.erase(priceIt);
                }

                return;
            }
        }
    }
}

std::string MapOrderBook::toString() const {
    std::ostringstream oss;

    oss << "Order Book: " << ticker << "\n";

    // Buy orders (highest price first)
    oss << "\nBUY ORDERS:\n";

    for (auto priceIt = buyOrders.rbegin(); priceIt != buyOrders.rend(); ++priceIt) {
        for (auto order : priceIt->second.orders) {
            oss << "  " << order->toString() << "\n";
        }
    }

    // Sell orders (lowest price first)
    oss << "\nSELL ORDERS:\n";

    for (auto priceIt = sellOrders.begin(); priceIt != sellOrders.end(); ++priceIt) {
        for (auto order : priceIt->second.orders) {
            oss << "  " << order->toString() << "\n";
        }
    }

    return oss.str();
}