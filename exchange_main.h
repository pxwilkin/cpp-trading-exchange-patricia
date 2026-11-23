
#ifndef MATCHING_ENGINE_H
#define MATCHING_ENGINE_H

#include <vector>
#include <mutex>
#include <algorithm>
#include "order.h"
#include "order_book.h"

struct Trade {
    int buyOrderID;
    int sellOrderID;
    double price;
    int quantity;
};

class MatchingEngine {
private:
    OrderBook& book;
    std::mutex orderMutex;
    std::vector<Trade> trades;

public:
    MatchingEngine(OrderBook& book) : book(book) {}

    void placeOrder(const Order& order) {
        std::lock_guard<std::mutex> lock(orderMutex);

        Order incoming = order;

        if (incoming.getSide() == Side::BUY) {
            while (incoming.getSize() > 0) {
                Order* bestAsk = book.getBestAsk();

                if (bestAsk == nullptr) {
                    break;
                }

                if (incoming.getType() == OrderType::LIMIT &&
                    incoming.getPrice() < bestAsk->getPrice()) {
                    break;
                }

                int quantity = std::min(
                    incoming.getSize(), bestAsk->getSize()
                );

                trades.push_back({
                    incoming.getOrderID(),
                    bestAsk->getOrderID(),
                    bestAsk->getPrice(),
                    quantity
                });

                incoming.setSize(incoming.getSize() - quantity);
                book.update(bestAsk->getOrderID(), quantity);
            }
        }
        else {
            while (incoming.getSize() > 0) {
                Order* bestBid = book.getBestBid();

                if (bestBid == nullptr) {
                    break;
                }

                if (incoming.getType() == OrderType::LIMIT &&
                    incoming.getPrice() > bestBid->getPrice()) {
                    break;
                }

                int quantity = std::min(
                    incoming.getSize(), bestBid->getSize()
                );

                trades.push_back({
                    bestBid->getOrderID(),
                    incoming.getOrderID(),
                    bestBid->getPrice(),
                    quantity
                });

                incoming.setSize(incoming.getSize() - quantity);
                book.update(bestBid->getOrderID(), quantity);
            }
        }

        if (incoming.getSize() > 0 &&
            incoming.getType() == OrderType::LIMIT) {
            book.add(incoming);
        }
    }

    const std::vector<Trade>& getTrades() const {
        return trades;
    }
};

#endif
