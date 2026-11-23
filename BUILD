load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_library(
    name = "order_book",
    srcs = ["order_book.cpp",
            "tick_order_book.cpp",
            "map_order_book.cpp",
    ],
    hdrs = [
        "order_book.h",
        "tick_order_book.h",
        "map_order_book.h",
        "order.h",
        "mem_pool.h",
    ],
)

cc_test(
    name = "order_book_test",
    srcs = ["order_book_test.cpp"],
    deps = [
        ":order_book",
        "@googletest//:gtest_main",
    ],
    size = "small",
)

cc_binary(
    name = "containers_benchmark",
    srcs = ["containers_benchmark.cpp"],
    deps = [
        ":order_book",
    ],
    linkopts = [
        "-lbenchmark",
        "-lpthread",
    ],
)