#pragma once

#include <optional>
#include <unordered_map>
#include <vector>
#include "symbol_book.hpp"
#include "types.hpp"

class OrderBook {
 public:
  std::vector<Trade> submit (const Order& order);
  std::optional<int64_t> getBestAsk (const std::string& symbol) const;
  std::optional<int64_t> getBestBid (const std::string& symbol) const;
  std::optional<int64_t> cancelOrder (const uint64_t orderId);

 private:
  std::vector<Trade> fulfill (const Order& order);
  bool crosses (const Order& incoming, const Order& resting) const;

  template <typename PriceSet>
  std::optional<SymbolBook::Iterator> findMatchingOrderIn (
      const PriceSet& opposite_by_price,
      const Order& order) const;

  SymbolBook& getSymbolBook (const Order& order);
  std::unordered_map<std::string, SymbolBook> orders_;
  std::unordered_map<uint64_t,
                     std::tuple<std::string, Side, SymbolBook::Iterator>>
      pendingOrders_;
};

template <typename PriceSet>
std::optional<SymbolBook::Iterator> OrderBook::findMatchingOrderIn (
    const PriceSet& opposite_by_price,
    const Order& order) const {
  for (const auto it : opposite_by_price) {
    if (it->order_id == order.order_id) {
      continue;
    }
    return it;
  }
  return std::nullopt;
};
