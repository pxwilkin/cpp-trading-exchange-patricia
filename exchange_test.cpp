#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "order.h"
#include "order_book.h"
#include "mem_pool.h"
#include "exchange_main.h"

class MatchingEngineTest : public ::testing::Test {
protected:
    MemPool<Order> pool{1000};
    OrderBook book{"AAPL", pool};
    MatchingEngine engine{book};
};

// Test that a buy order matches an existing sell order
TEST_F(MatchingEngineTest, BuyMatchesSell) {
    Order sell("AAPL", 10, Side::SELL, OrderType::LIMIT,
               150.00, 1, 101);

    Order buy("AAPL", 10, Side::BUY, OrderType::LIMIT,
              150.00, 2, 102);

    engine.placeOrder(sell);
    engine.placeOrder(buy);

    EXPECT_EQ(book.getBestAsk(), nullptr);
    EXPECT_EQ(book.getBestBid(), nullptr);
}

// Test that a sell order matches an existing buy order
TEST_F(MatchingEngineTest, SellMatchesBuy) {
    Order buy("AAPL", 10, Side::BUY, OrderType::LIMIT,
              150.00, 1, 101);

    Order sell("AAPL", 10, Side::SELL, OrderType::LIMIT,
               149.00, 2, 102);

    engine.placeOrder(buy);
    engine.placeOrder(sell);

    EXPECT_EQ(book.getBestBid(), nullptr);
    EXPECT_EQ(book.getBestAsk(), nullptr);
}

// Test partial fill
TEST_F(MatchingEngineTest, PartialFill) {
    Order sell("AAPL", 10, Side::SELL, OrderType::LIMIT,
               150.00, 1, 101);

    Order buy("AAPL", 4, Side::BUY, OrderType::LIMIT,
              150.00, 2, 102);

    engine.placeOrder(sell);
    engine.placeOrder(buy);

    ASSERT_NE(book.getBestAsk(), nullptr);
    EXPECT_EQ(book.getBestAsk()->getSize(), 6);
}

// Test that an unmatched limit order enters the book
TEST_F(MatchingEngineTest, UnmatchedLimitOrder) {
    Order buy("AAPL", 10, Side::BUY, OrderType::LIMIT,
              140.00, 1, 101);

    engine.placeOrder(buy);

    ASSERT_NE(book.getBestBid(), nullptr);
    EXPECT_EQ(book.getBestBid()->getSize(), 10);
}

// Test that a buy limit order does not cross the best ask
TEST_F(MatchingEngineTest, BuyDoesNotMatchAboveItsLimit) {
    Order sell("AAPL", 10, Side::SELL, OrderType::LIMIT,
               150.00, 1, 101);

    Order buy("AAPL", 10, Side::BUY, OrderType::LIMIT,
              149.00, 2, 102);

    engine.placeOrder(sell);
    engine.placeOrder(buy);

    ASSERT_NE(book.getBestAsk(), nullptr);
    ASSERT_NE(book.getBestBid(), nullptr);

    EXPECT_EQ(book.getBestAsk()->getPrice(), 150.00);
    EXPECT_EQ(book.getBestBid()->getPrice(), 149.00);
}

// Test that one incoming order matches multiple resting orders
TEST_F(MatchingEngineTest, MultipleMatches) {
    Order sell1("AAPL", 4, Side::SELL, OrderType::LIMIT,
                150.00, 1, 101);

    Order sell2("AAPL", 6, Side::SELL, OrderType::LIMIT,
                151.00, 2, 102);

    Order buy("AAPL", 10, Side::BUY, OrderType::LIMIT,
              151.00, 3, 103);

    engine.placeOrder(sell1);
    engine.placeOrder(sell2);
    engine.placeOrder(buy);

    EXPECT_EQ(book.getBestAsk(), nullptr);
    EXPECT_EQ(book.getBestBid(), nullptr);
}

// Test concurrent submissions
TEST_F(MatchingEngineTest, ConcurrentOrderSubmission) {
    std::vector<std::thread> clients;

    for (int i = 0; i < 4; i++) {
        clients.emplace_back([this, i]() {
            for (int j = 0; j < 10; j++) {
                Order order(
                    "AAPL",
                    5,
                    i % 2 == 0 ? Side::BUY : Side::SELL,
                    OrderType::LIMIT,
                    150.00,
                    i + 1,
                    (i + 1) * 100 + j
                );

                engine.placeOrder(order);
            }
        });
    }

    for (auto& client : clients) {
        client.join();
    }

    // Verify the order book remains accessible after concurrent submissions
    book.toString();
}