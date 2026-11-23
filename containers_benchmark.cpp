#include <benchmark/benchmark.h>
#include <vector>
#include <random>

#include "order.h"
#include "mem_pool.h"
#include "order_book.h"
#include "tick_order_book.h"
#include "map_order_book.h"

// Generate a set of orders to use in the benchmarks
std::vector<Order> generateOrders(int n) {
    std::vector<Order> orders;

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> priceDist(10000, 14999);

    for (int i = 0; i < n; i++) {
        Side side = (i % 2 == 0) ? Side::BUY : Side::SELL;

        double price = priceDist(rng) / 100.0;

        orders.emplace_back(
            "AAPL",
            10,
            side,
            OrderType::LIMIT,
            price,
            i + 1,
            i + 1
        );
    }

    return orders;
}


// ADD BENCHMARKS

static void BM_PriorityQueue_Add(benchmark::State& state) {
    int n = state.range(0);
    std::vector<Order> orders = generateOrders(n);

    for (auto _ : state) {
        MemPool<Order> pool(n);
        OrderBook book("AAPL", pool);

        for (const Order& order : orders) {
            book.add(order);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}


static void BM_Tick_Add(benchmark::State& state) {
    int n = state.range(0);
    std::vector<Order> orders = generateOrders(n);

    for (auto _ : state) {
        MemPool<Order> pool(n);
        TickOrderBook book("AAPL", pool, 10000, 100.0);

        for (const Order& order : orders) {
            book.add(order);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}


static void BM_Map_Add(benchmark::State& state) {
    int n = state.range(0);
    std::vector<Order> orders = generateOrders(n);

    for (auto _ : state) {
        MemPool<Order> pool(n);
        MapOrderBook book("AAPL", pool);

        for (const Order& order : orders) {
            book.add(order);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}


// CANCEL BENCHMARKS

static void BM_PriorityQueue_Cancel(benchmark::State& state) {
    int n = state.range(0);
    std::vector<Order> orders = generateOrders(n);

    for (auto _ : state) {
        MemPool<Order> pool(n);
        OrderBook book("AAPL", pool);

        // Add all orders before timing cancellation
        for (const Order& order : orders) {
            book.add(order);
        }

        state.PauseTiming();

        state.ResumeTiming();

        for (int i = 1; i <= n; i++) {
            book.cancel(i);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}


static void BM_Tick_Cancel(benchmark::State& state) {
    int n = state.range(0);
    std::vector<Order> orders = generateOrders(n);

    for (auto _ : state) {
        MemPool<Order> pool(n);
        TickOrderBook book("AAPL", pool, 10000, 100.0);

        for (const Order& order : orders) {
            book.add(order);
        }

        state.PauseTiming();
        state.ResumeTiming();

        for (int i = 1; i <= n; i++) {
            book.cancel(i);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}


static void BM_Map_Cancel(benchmark::State& state) {
    int n = state.range(0);
    std::vector<Order> orders = generateOrders(n);

    for (auto _ : state) {
        MemPool<Order> pool(n);
        MapOrderBook book("AAPL", pool);

        for (const Order& order : orders) {
            book.add(order);
        }

        state.PauseTiming();
        state.ResumeTiming();

        for (int i = 1; i <= n; i++) {
            book.cancel(i);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Priority Queue: Best Bid
static void BM_PriorityQueue_GetBestBid(benchmark::State& state) {
    int n = state.range(0);

    MemPool<Order> pool(n);
    OrderBook book("AAPL", pool);

    std::vector<Order> orders = generateOrders(n);

    for (const Order& order : orders) {
        book.add(order);
    }

    for (auto _ : state) {
        Order* bestBid = book.getBestBid();
        benchmark::DoNotOptimize(bestBid);
    }

    state.SetItemsProcessed(state.iterations());
}

// Priority Queue: Best Ask
static void BM_PriorityQueue_GetBestAsk(benchmark::State& state) {
    int n = state.range(0);

    MemPool<Order> pool(n);
    OrderBook book("AAPL", pool);

    std::vector<Order> orders = generateOrders(n);

    for (const Order& order : orders) {
        book.add(order);
    }

    for (auto _ : state) {
        Order* bestAsk = book.getBestAsk();
        benchmark::DoNotOptimize(bestAsk);
    }

    state.SetItemsProcessed(state.iterations());
}

// Tick Order Book: Best Bid
static void BM_Tick_GetBestBid(benchmark::State& state) {
    int n = state.range(0);

    MemPool<Order> pool(n);
    TickOrderBook book("AAPL", pool, 10000, 100.0);

    std::vector<Order> orders = generateOrders(n);

    for (const Order& order : orders) {
        book.add(order);
    }

    for (auto _ : state) {
        Order* bestBid = book.getBestBid();
        benchmark::DoNotOptimize(bestBid);
    }

    state.SetItemsProcessed(state.iterations());
}

// Tick Order Book: Best Ask
static void BM_Tick_GetBestAsk(benchmark::State& state) {
    int n = state.range(0);

    MemPool<Order> pool(n);
    TickOrderBook book("AAPL", pool, 10000, 100.0);

    std::vector<Order> orders = generateOrders(n);

    for (const Order& order : orders) {
        book.add(order);
    }

    for (auto _ : state) {
        Order* bestAsk = book.getBestAsk();
        benchmark::DoNotOptimize(bestAsk);
    }

    state.SetItemsProcessed(state.iterations());
}

// Map Order Book: Best Bid
static void BM_Map_GetBestBid(benchmark::State& state) {
    int n = state.range(0);

    MemPool<Order> pool(n);
    MapOrderBook book("AAPL", pool);

    std::vector<Order> orders = generateOrders(n);

    for (const Order& order : orders) {
        book.add(order);
    }

    for (auto _ : state) {
        Order* bestBid = book.getBestBid();
        benchmark::DoNotOptimize(bestBid);
    }

    state.SetItemsProcessed(state.iterations());
}

// Map Order Book: Best Ask
static void BM_Map_GetBestAsk(benchmark::State& state) {
    int n = state.range(0);

    MemPool<Order> pool(n);
    MapOrderBook book("AAPL", pool);

    std::vector<Order> orders = generateOrders(n);

    for (const Order& order : orders) {
        book.add(order);
    }

    for (auto _ : state) {
        Order* bestAsk = book.getBestAsk();
        benchmark::DoNotOptimize(bestAsk);
    }

    state.SetItemsProcessed(state.iterations());
}

// REGISTER BENCHMARKS

BENCHMARK(BM_PriorityQueue_Add)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_Tick_Add)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_Map_Add)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK(BM_PriorityQueue_Cancel)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_Tick_Cancel)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_Map_Cancel)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK(BM_PriorityQueue_GetBestBid)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_PriorityQueue_GetBestAsk)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK(BM_Tick_GetBestBid)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_Tick_GetBestAsk)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK(BM_Map_GetBestBid)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_Map_GetBestAsk)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();