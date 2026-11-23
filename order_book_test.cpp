#include "order_book.h"
#include <gtest/gtest.h>
#include "mem_pool.h"


class OrderBookTest : public ::testing::Test {
protected:
    MemPool<Order> pool{10};
    OrderBook book{"AAPL", pool};
};

// Test adding a buy order
TEST_F(OrderBookTest, AddBuyOrder) {
    Order order("AAPL", 100, Side::BUY, OrderType::LIMIT, 150.0, 1, 101);

    book.add(order);

    std::string result = book.toString();

    EXPECT_NE(result.find("BUY ORDERS"), std::string::npos);
    EXPECT_NE(result.find("Order ID: 101"), std::string::npos);
}

// Test adding a sell order
TEST_F(OrderBookTest, AddSellOrder) {
    Order order("AAPL", 100, Side::SELL, OrderType::LIMIT, 155.0, 1, 102);

    book.add(order);

    std::string result = book.toString();

    EXPECT_NE(result.find("SELL ORDERS"), std::string::npos);
    EXPECT_NE(result.find("Order ID: 102"), std::string::npos);
}

// Test that orders with a different ticker are rejected
TEST_F(OrderBookTest, AddWrongTicker) {
    Order order("MSFT", 100, Side::BUY, OrderType::LIMIT, 150.0, 1, 103);

    book.add(order);

    std::string result = book.toString();

    EXPECT_EQ(result.find("Order ID: 103"), std::string::npos);
}

// Test that buy orders are prioritized by highest price
TEST_F(OrderBookTest, BuyPriority) {
    Order order1("AAPL", 100, Side::BUY, OrderType::LIMIT, 100.0, 1, 101);
    Order order2("AAPL", 100, Side::BUY, OrderType::LIMIT, 150.0, 1, 102);
    Order order3("AAPL", 100, Side::BUY, OrderType::LIMIT, 125.0, 1, 103);

    book.add(order1);
    book.add(order2);
    book.add(order3);

    std::string result = book.toString();

    size_t pos102 = result.find("Order ID: 102");
    size_t pos103 = result.find("Order ID: 103");
    size_t pos101 = result.find("Order ID: 101");

    ASSERT_NE(pos102, std::string::npos);
    ASSERT_NE(pos103, std::string::npos);
    ASSERT_NE(pos101, std::string::npos);

    EXPECT_LT(pos102, pos103);
    EXPECT_LT(pos103, pos101);
}

// Test that sell orders are prioritized by lowest price
TEST_F(OrderBookTest, SellPriority) {
    Order order1("AAPL", 100, Side::SELL, OrderType::LIMIT, 150.0, 1, 101);
    Order order2("AAPL", 100, Side::SELL, OrderType::LIMIT, 100.0, 1, 102);
    Order order3("AAPL", 100, Side::SELL, OrderType::LIMIT, 125.0, 1, 103);

    book.add(order1);
    book.add(order2);
    book.add(order3);

    std::string result = book.toString();

    size_t pos102 = result.find("Order ID: 102");
    size_t pos103 = result.find("Order ID: 103");
    size_t pos101 = result.find("Order ID: 101");

    ASSERT_NE(pos102, std::string::npos);
    ASSERT_NE(pos103, std::string::npos);
    ASSERT_NE(pos101, std::string::npos);

    EXPECT_LT(pos102, pos103);
    EXPECT_LT(pos103, pos101);
}

// Test canceling a buy order
TEST_F(OrderBookTest, CancelBuyOrder) {
    Order order1("AAPL", 100, Side::BUY, OrderType::LIMIT, 150.0, 1, 101);
    Order order2("AAPL", 100, Side::BUY, OrderType::LIMIT, 140.0, 1, 102);

    book.add(order1);
    book.add(order2);

    book.cancel(101);

    std::string result = book.toString();

    EXPECT_EQ(result.find("Order ID: 101"), std::string::npos);
    EXPECT_NE(result.find("Order ID: 102"), std::string::npos);
}

// Test canceling a sell order
TEST_F(OrderBookTest, CancelSellOrder) {
    Order order("AAPL", 100, Side::SELL, OrderType::LIMIT, 150.0, 1, 101);

    book.add(order);
    book.cancel(101);

    std::string result = book.toString();

    EXPECT_EQ(result.find("Order ID: 101"), std::string::npos);
}

// Test canceling an order that does not exist
TEST_F(OrderBookTest, CancelNonexistentOrder) {
    Order order("AAPL", 100, Side::BUY, OrderType::LIMIT, 150.0, 1, 101);

    book.add(order);
    book.cancel(999);

    std::string result = book.toString();

    EXPECT_NE(result.find("Order ID: 101"), std::string::npos);
}

// Test that toString prints both sides of the book
TEST_F(OrderBookTest, ToString) {
    Order buy("AAPL", 100, Side::BUY, OrderType::LIMIT, 150.0, 1, 101);
    Order sell("AAPL", 200, Side::SELL, OrderType::LIMIT, 155.0, 2, 102);

    book.add(buy);
    book.add(sell);

    std::string result = book.toString();

    EXPECT_NE(result.find("Order Book: AAPL"), std::string::npos);
    EXPECT_NE(result.find("BUY ORDERS"), std::string::npos);
    EXPECT_NE(result.find("SELL ORDERS"), std::string::npos);
    EXPECT_NE(result.find("Order ID: 101"), std::string::npos);
    EXPECT_NE(result.find("Order ID: 102"), std::string::npos);
}

TEST_F(OrderBookTest, CancelHighestBuyPromotesNext) {
    Order low{"AAPL", 100, Side::BUY, OrderType::LIMIT, 140, 1, 101};
    Order middle{"AAPL", 100, Side::BUY, OrderType::LIMIT, 150, 2, 102};
    Order high{"AAPL", 100, Side::BUY, OrderType::LIMIT, 160, 3, 103};

    book.add(low);
    book.add(middle);
    book.add(high);

    book.cancel(103);

    std::string result = book.toString();

    size_t middlePos = result.find(middle.toString());
    size_t lowPos = result.find(low.toString());

    ASSERT_NE(middlePos, std::string::npos);
    ASSERT_NE(lowPos, std::string::npos);

    EXPECT_LT(middlePos, lowPos);
    EXPECT_EQ(result.find(high.toString()), std::string::npos);
}

// Adding multiple orders at the same price
TEST_F(OrderBookTest, MultipleOrdersSamePrice) {
    Order order1("AAPL", 10, Side::BUY, OrderType::LIMIT, 150.0, 1, 101);
    Order order2("AAPL", 20, Side::BUY, OrderType::LIMIT, 150.0, 2, 102);
    Order order3("AAPL", 15, Side::BUY, OrderType::LIMIT, 150.0, 3, 103);

    book.add(order1);
    book.add(order2);
    book.add(order3);

    std::string result = book.toString();

    EXPECT_NE(result.find("Order ID: 101"), std::string::npos);
    EXPECT_NE(result.find("Order ID: 102"), std::string::npos);
    EXPECT_NE(result.find("Order ID: 103"), std::string::npos);
}

// Canceling all orders should leave an empty book
TEST_F(OrderBookTest, CancelAllOrders) {
    Order order1("AAPL", 10, Side::BUY, OrderType::LIMIT, 150.0, 1, 101);
    Order order2("AAPL", 20, Side::SELL, OrderType::LIMIT, 145.0, 2, 102);

    book.add(order1);
    book.add(order2);

    book.cancel(101);
    book.cancel(102);

    std::string result = book.toString();

    EXPECT_EQ(result.find("Order ID:"), std::string::npos);
}

// Adding another order after cancellation
TEST_F(OrderBookTest, AddOrderAfterCancellation) {
    Order order1("AAPL", 10, Side::BUY, OrderType::LIMIT, 150.0, 1, 101);
    Order order2("AAPL", 20, Side::BUY, OrderType::LIMIT, 155.0, 2, 102);

    book.add(order1);
    book.cancel(101);

    book.add(order2);

    std::string result = book.toString();

    EXPECT_EQ(result.find("Order ID: 101"), std::string::npos);
    EXPECT_NE(result.find("Order ID: 102"), std::string::npos);
}