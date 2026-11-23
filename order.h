#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <sstream>

enum class Side {
    BUY,
    SELL
};

enum class OrderType {
    LIMIT,
    MARKET
};

class Order {
    private:
    std::string ticker;
    int size;
    Side side;
    OrderType type;
    double price;
    int client_id;
    int order_id;

    public:
    Order() = default;
    
    Order(const std::string& ticker, int size, Side side, 
        OrderType type, double price, int client_id, int order_id) 
        : ticker(ticker), size(size), side(side), type(type), 
        price(price), client_id(client_id), order_id(order_id) {} 

    // getters
    const std::string& getTicker() const { return ticker; }
    int getSize() const { return size; }
    Side getSide() const { return side; }
    OrderType getType() const { return type; }
    double getPrice() const { return price; }
    int getClientID() const { return client_id; }
    int getOrderID() const { return order_id; }

    // setter
    void setSize(int newSize) {
        size = newSize;
    }

    struct ForBids {
        bool operator()(const Order* a, const Order* b) const {
            return a->getPrice() < b->getPrice();
        }
    };

    struct ForAsks {
        bool operator()(const Order* a, const Order* b) const {
            return a->getPrice() > b->getPrice();
        }
    };


    std::string toString() const {
        std::ostringstream oss;

        oss << "Ticker: " << ticker 
            << ", Size: " << size 
            << ", Side: " << (side == Side::BUY ? "BUY" : "SELL") 
            << ", Type: " << (type == OrderType::LIMIT ? "LIMIT" : "MARKET");

            if (type == OrderType::LIMIT) {
                oss << ", Price: " << price; 

            }

            oss << ", Client ID: " << client_id 
                << ", Order ID: " << order_id;
        
        return oss.str();
    }



};


#endif