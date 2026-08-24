#include "order_book.hpp"
#include <utility>

#include "symbol_book.hpp"
#include "types.hpp"

bool OrderBook::crosses (const Order& incoming, const Order& resting) const {
  if (incoming.side == Side::Buy) {
    return incoming.type == OrderType::Market ||
           incoming.price >= resting.price;
  }
  return incoming.type == OrderType::Market || incoming.price <= resting.price;
}

SymbolBook& OrderBook::getSymbolBook (const Order& order) {
  return orders_.try_emplace (order.symbol, SymbolBook {}).first->second;
}

std::vector<Trade> OrderBook::fulfill (const Order& order) {
  std::vector<Trade> trades {};

  auto& symbolBook = getSymbolBook (order);

  std::list<Order>& opposite =
      (order.side == Side::Sell) ? symbolBook.bids : symbolBook.asks;

  const auto rest_on_same_side = [&] (int64_t quantity) {
    if (quantity <= 0 || order.type == OrderType::Market) {
      return;
    }

    const Order resting {
        .order_id = order.order_id,
        .symbol = order.symbol,
        .side = order.side,
        .type = OrderType::Limit,
        .price = order.price,
        .quantity = quantity,
        .timestamp = order.timestamp,
    };

    if (order.side == Side::Sell) {
      const auto it = symbolBook.asks.emplace (symbolBook.asks.end (), resting);
      symbolBook.asks_by_price.insert (it);
      pendingOrders_[order.order_id] = {order.symbol, order.side, it};
    } else {
      const auto it = symbolBook.bids.emplace (symbolBook.bids.end (), resting);
      symbolBook.bids_by_price.insert (it);
      pendingOrders_[order.order_id] = {order.symbol, order.side, it};
    }
  };

  int64_t quantity_to_fulfill = order.quantity;
  while (quantity_to_fulfill > 0 && !opposite.empty ()) {
    std::optional<SymbolBook::Iterator> matchingOrderItOpt;
    if (order.side == Side::Buy) {
      matchingOrderItOpt =
          findMatchingOrderIn (symbolBook.asks_by_price, order);
    } else {
      matchingOrderItOpt =
          findMatchingOrderIn (symbolBook.bids_by_price, order);
    }

    if (!matchingOrderItOpt) {
      break;
    }

    const auto restingOrderIt = *matchingOrderItOpt;
    if (!crosses (order, *restingOrderIt)) {
      break;
    }

    const int64_t quantity = quantity_to_fulfill >= restingOrderIt->quantity
                                 ? restingOrderIt->quantity
                                 : quantity_to_fulfill;

    trades.emplace_back (Trade {
        .buy_order_id = (order.side == Side::Buy) ? order.order_id
                                                  : restingOrderIt->order_id,
        .sell_order_id = (order.side == Side::Sell) ? order.order_id
                                                    : restingOrderIt->order_id,
        .symbol = order.symbol,
        .price = restingOrderIt->price,
        .quantity = quantity,
    });

    restingOrderIt->quantity -= quantity;
    if (restingOrderIt->quantity == 0) {
      opposite.erase (restingOrderIt);
      pendingOrders_.erase (restingOrderIt->order_id);
      if (order.side == Side::Sell) {
        symbolBook.bids_by_price.erase (restingOrderIt);
      } else {
        symbolBook.asks_by_price.erase (restingOrderIt);
      }
    }
    quantity_to_fulfill -= quantity;
  }

  rest_on_same_side (quantity_to_fulfill);
  return trades;
}

std::optional<int64_t> OrderBook::getBestAsk (const std::string& symbol) const {
  auto it = orders_.find (symbol);
  if (it == orders_.end ()) {
    return std::nullopt;
  }

  auto setIt = it->second.asks_by_price.begin ();
  if (setIt == it->second.asks_by_price.end ())
    return std::nullopt;

  return (*setIt)->price;
}

std::optional<int64_t> OrderBook::getBestBid (const std::string& symbol) const {
  auto it = orders_.find (symbol);
  if (it == orders_.end ()) {
    return std::nullopt;
  }

  auto setIt = it->second.bids_by_price.begin ();
  if (setIt == it->second.bids_by_price.end ())
    return std::nullopt;

  return (*setIt)->price;
}

std::optional<int64_t> OrderBook::cancelOrder (const uint64_t orderId) {
  auto it = pendingOrders_.find (orderId);
  if (it == pendingOrders_.end ())
    return std::nullopt;

  auto orderTuple = it->second;
  auto symbol = std::get<std::string> (orderTuple);

  auto symbolBookIt = orders_.find (symbol);
  if (symbolBookIt == orders_.end ())
    return std::nullopt;

  auto side = std::get<Side> (orderTuple);
  auto order = std::get<SymbolBook::Iterator> (orderTuple);
  auto cancelledQuantity {order->quantity};
  if (side == Side::Sell) {
    symbolBookIt->second.asks_by_price.erase (order);
    symbolBookIt->second.asks.erase (order);
  } else {
    symbolBookIt->second.bids_by_price.erase (order);
    symbolBookIt->second.bids.erase (order);
  }

  pendingOrders_.erase (orderId);
  return cancelledQuantity;
}

std::vector<Trade> OrderBook::submit (const Order& order) {
  // O(k log n) time, k = trades produced, n = resting orders; O(k) output
  // space.
  return fulfill (order);
}
