#pragma once

#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

enum class Side { Buy, Sell };
enum class OrderType { Market = 0, Limit = 1 };

constexpr std::string_view to_string (Side side) {
  switch (side) {
    case Side::Buy:
      return "BUY";
    case Side::Sell:
      return "SELL";
  }
  return "Unknown";
}

struct Order {
  uint64_t order_id {};
  std::string symbol;
  Side side {};
  OrderType type {};
  int64_t price {};  // ticks (e.g. cents)
  int64_t quantity {};
  uint64_t timestamp {};  // lower = earlier priority at same price

  friend std::ostream& operator<< (std::ostream& os, const Order& obj);
};

inline std::ostream& operator<< (std::ostream& os, const Order& obj) {
  os << "Order [" << obj.order_id << "] Symbol [" << obj.symbol << "] Side ["
     << to_string (obj.side) << "] price [" << obj.price << "] quantity ["
     << obj.quantity << "] timestamp [" << obj.timestamp << "]";
  return os;
}

struct Trade {
  uint64_t buy_order_id {};
  uint64_t sell_order_id {};
  std::string symbol;
  int64_t price {};
  int64_t quantity {};

  friend bool operator== (const Trade&, const Trade&) = default;
  friend std::ostream& operator<< (std::ostream& os, const Trade& obj);
};

inline std::ostream& operator<< (std::ostream& os, const Trade& obj) {
  os << "New Trade buy_order_id [" << obj.buy_order_id << "] sell_order_id ["
     << obj.sell_order_id << "] symbol [" << obj.symbol << "] price ["
     << obj.price << "] quantity [" << obj.quantity << "] created\n";
  return os;
}
