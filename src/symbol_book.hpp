#pragma once
#include <list>
#include <set>
#include "types.hpp"

struct SymbolBook {
  using Iterator = std::list<Order>::iterator;

  struct CompareBid {
    bool operator() (Iterator a, Iterator b) const {
      if (a->price != b->price) {
        return a->price > b->price;
      }
      return a->timestamp < b->timestamp;
    }
  };

  struct CompareAsk {
    bool operator() (Iterator a, Iterator b) const {
      if (a->price != b->price) {
        return a->price < b->price;
      }
      return a->timestamp < b->timestamp;
    }
  };

  std::list<Order> asks;  // sell orders
  std::list<Order> bids;  // buy orderes
  std::set<Iterator, CompareAsk> asks_by_price;
  std::set<Iterator, CompareBid> bids_by_price;
};
