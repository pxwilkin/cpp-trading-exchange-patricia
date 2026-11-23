load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_library(
    name = "order_book",
    srcs = ["order_book.cpp"],
    hdrs = [
        "order_book.h",
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