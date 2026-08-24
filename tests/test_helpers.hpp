#pragma once

#include "order_book.hpp"
#include "types.hpp"

#include <gtest/gtest.h>

#include <optional>
#include <vector>

inline Order make_limit_order (uint64_t id,
                               const char* symbol,
                               Side side,
                               int64_t price,
                               int64_t qty,
                               uint64_t ts) {
  return Order {id, symbol, side, OrderType::Limit, price, qty, ts};
}

inline Order make_market_order (uint64_t id,
                                const char* symbol,
                                Side side,
                                int64_t qty,
                                uint64_t ts) {
  return Order {id, symbol, side, OrderType::Market, 0, qty, ts};
}

inline Trade make_trade (uint64_t buy_id,
                         uint64_t sell_id,
                         const char* symbol,
                         int64_t price,
                         int64_t qty) {
  return Trade {buy_id, sell_id, symbol, price, qty};
}

inline void expect_trades (OrderBook& book,
                           const Order& order,
                           const std::vector<Trade>& expected) {
  EXPECT_EQ (book.submit (order), expected);
}

inline void expect_no_trades (OrderBook& book, const Order& order) {
  expect_trades (book, order, {});
}

inline void expect_price (const std::optional<int64_t>& actual,
                          const std::optional<int64_t>& expected) {
  EXPECT_EQ (actual, expected);
}

inline void expect_cancel (OrderBook& book,
                           uint64_t order_id,
                           const std::optional<int64_t>& expected_qty) {
  EXPECT_EQ (book.cancelOrder (order_id), expected_qty);
}

class OrderBookTest : public ::testing::Test {
 protected:
  OrderBook book;
};
