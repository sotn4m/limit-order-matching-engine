#include "test_helpers.hpp"

TEST_F (OrderBookTest, PartialFill) {
  expect_no_trades (book, make_limit_order (1, "AAPL", Side::Sell, 100, 10, 1));
  expect_trades (book, make_limit_order (2, "AAPL", Side::Buy, 105, 5, 2),
                 {make_trade (2, 1, "AAPL", 100, 5)});
}

TEST_F (OrderBookTest, PriceTimePriority) {
  expect_no_trades (book, make_limit_order (1, "MSFT", Side::Sell, 100, 5, 10));
  expect_no_trades (book, make_limit_order (2, "MSFT", Side::Sell, 100, 5, 5));
  expect_trades (book, make_limit_order (3, "MSFT", Side::Buy, 100, 5, 20),
                 {make_trade (3, 2, "MSFT", 100, 5)});
}

TEST_F (OrderBookTest, MarketOrderDiscardsRemainder) {
  expect_no_trades (book, make_limit_order (4, "AAPL", Side::Sell, 99, 5, 4));
  expect_trades (book, make_market_order (5, "AAPL", Side::Buy, 20, 5),
                 {make_trade (5, 4, "AAPL", 99, 5)});
}

TEST_F (OrderBookTest, NoMatchResting) {
  expect_no_trades (book, make_limit_order (1, "GOOG", Side::Buy, 100, 10, 1));
  expect_no_trades (book, make_limit_order (2, "GOOG", Side::Sell, 110, 10, 2));
  expect_no_trades (book, make_limit_order (3, "GOOG", Side::Buy, 105, 5, 3));
}

TEST_F (OrderBookTest, NoSelfTrade) {
  expect_no_trades (book,
                    make_limit_order (42, "AAPL", Side::Sell, 100, 10, 1));
  expect_no_trades (book, make_limit_order (42, "AAPL", Side::Buy, 100, 10, 2));
}

TEST_F (OrderBookTest, SymbolIsolation) {
  expect_no_trades (book, make_limit_order (1, "AAPL", Side::Sell, 100, 10, 1));
  expect_no_trades (book, make_limit_order (2, "MSFT", Side::Buy, 200, 5, 2));
}

TEST_F (OrderBookTest, ExampleWalkthrough) {
  expect_no_trades (book, make_limit_order (1, "AAPL", Side::Sell, 100, 10, 1));
  expect_trades (book, make_limit_order (2, "AAPL", Side::Buy, 105, 5, 2),
                 {make_trade (2, 1, "AAPL", 100, 5)});
  expect_trades (book, make_limit_order (3, "AAPL", Side::Buy, 100, 5, 3),
                 {make_trade (3, 1, "AAPL", 100, 5)});
  expect_no_trades (book, make_limit_order (4, "AAPL", Side::Sell, 99, 5, 4));
  expect_trades (book, make_market_order (5, "AAPL", Side::Buy, 20, 5),
                 {make_trade (5, 4, "AAPL", 99, 5)});
}

TEST_F (OrderBookTest, MultiFillOneSubmit) {
  expect_no_trades (book, make_limit_order (1, "AAPL", Side::Sell, 99, 5, 1));
  expect_no_trades (book, make_limit_order (2, "AAPL", Side::Sell, 100, 10, 2));
  expect_trades (
      book, make_limit_order (3, "AAPL", Side::Buy, 105, 12, 3),
      {make_trade (3, 1, "AAPL", 99, 5), make_trade (3, 2, "AAPL", 100, 7)});
}

TEST_F (OrderBookTest, BidPricePriority) {
  expect_no_trades (book, make_limit_order (1, "MSFT", Side::Buy, 110, 5, 1));
  expect_no_trades (book, make_limit_order (2, "MSFT", Side::Buy, 105, 5, 2));
  expect_trades (book, make_limit_order (3, "MSFT", Side::Sell, 100, 5, 3),
                 {make_trade (1, 3, "MSFT", 110, 5)});
}

TEST_F (OrderBookTest, AskPricePriority) {
  expect_no_trades (book, make_limit_order (1, "GOOG", Side::Sell, 100, 5, 1));
  expect_no_trades (book, make_limit_order (2, "GOOG", Side::Sell, 99, 5, 2));
  expect_trades (book, make_limit_order (3, "GOOG", Side::Buy, 105, 5, 3),
                 {make_trade (3, 2, "GOOG", 99, 5)});
}

TEST_F (OrderBookTest, TradeAtRestingPrice) {
  expect_no_trades (book, make_limit_order (1, "AAPL", Side::Sell, 100, 10, 1));
  expect_trades (book, make_limit_order (2, "AAPL", Side::Buy, 110, 5, 2),
                 {make_trade (2, 1, "AAPL", 100, 5)});
}

TEST_F (OrderBookTest, SelfTradeSkipsToNext) {
  expect_no_trades (book, make_limit_order (1, "AAPL", Side::Sell, 100, 5, 1));
  expect_no_trades (book, make_limit_order (2, "AAPL", Side::Sell, 100, 5, 2));
  expect_trades (book, make_limit_order (1, "AAPL", Side::Buy, 100, 5, 3),
                 {make_trade (1, 2, "AAPL", 100, 5)});
}

TEST_F (OrderBookTest, IncomingSellMultiFill) {
  expect_no_trades (book, make_limit_order (1, "MSFT", Side::Buy, 110, 5, 1));
  expect_no_trades (book, make_limit_order (2, "MSFT", Side::Buy, 105, 10, 2));
  expect_trades (
      book, make_limit_order (3, "MSFT", Side::Sell, 100, 12, 3),
      {make_trade (1, 3, "MSFT", 110, 5), make_trade (2, 3, "MSFT", 105, 7)});
}

TEST_F (OrderBookTest, LimitBuyRestsAfterMultiFill) {
  expect_no_trades (book, make_limit_order (1, "AAPL", Side::Sell, 100, 5, 1));
  expect_trades (book, make_limit_order (2, "AAPL", Side::Buy, 105, 15, 2),
                 {make_trade (2, 1, "AAPL", 100, 5)});
  expect_no_trades (book, make_limit_order (3, "AAPL", Side::Buy, 102, 5, 3));
}

TEST_F (OrderBookTest, BboEmpty) {
  expect_price (book.getBestAsk ("AAPL"), std::nullopt);
  expect_price (book.getBestBid ("AAPL"), std::nullopt);
  expect_price (book.getBestAsk ("UNKNOWN"), std::nullopt);
}

TEST_F (OrderBookTest, BestAsk) {
  expect_no_trades (book, make_limit_order (1, "AAPL", Side::Sell, 100, 5, 1));
  expect_price (book.getBestAsk ("AAPL"), int64_t {100});
  expect_no_trades (book, make_limit_order (2, "AAPL", Side::Sell, 99, 5, 2));
  expect_price (book.getBestAsk ("AAPL"), int64_t {99});
}

TEST_F (OrderBookTest, BestBid) {
  expect_no_trades (book, make_limit_order (1, "MSFT", Side::Buy, 105, 5, 1));
  expect_price (book.getBestBid ("MSFT"), int64_t {105});
  expect_no_trades (book, make_limit_order (2, "MSFT", Side::Buy, 110, 5, 2));
  expect_price (book.getBestBid ("MSFT"), int64_t {110});
}

TEST_F (OrderBookTest, BboAfterTopRemoved) {
  expect_no_trades (book, make_limit_order (1, "GOOG", Side::Sell, 99, 5, 1));
  expect_no_trades (book, make_limit_order (2, "GOOG", Side::Sell, 100, 5, 2));
  expect_price (book.getBestAsk ("GOOG"), int64_t {99});
  expect_trades (book, make_limit_order (3, "GOOG", Side::Buy, 105, 5, 3),
                 {make_trade (3, 1, "GOOG", 99, 5)});
  expect_price (book.getBestAsk ("GOOG"), int64_t {100});
  expect_price (book.getBestBid ("GOOG"), std::nullopt);
}

TEST_F (OrderBookTest, CancelUnknownId) {
  expect_cancel (book, 999, std::nullopt);
}

TEST_F (OrderBookTest, CancelRestingOrder) {
  expect_no_trades (book, make_limit_order (1, "AAPL", Side::Sell, 100, 10, 1));
  expect_cancel (book, 1, int64_t {10});
  expect_price (book.getBestAsk ("AAPL"), std::nullopt);
  expect_no_trades (book, make_limit_order (2, "AAPL", Side::Buy, 105, 5, 2));
}

TEST_F (OrderBookTest, CancelUpdatesBestBid) {
  expect_no_trades (book, make_limit_order (1, "MSFT", Side::Buy, 110, 5, 1));
  expect_no_trades (book, make_limit_order (2, "MSFT", Side::Buy, 105, 5, 2));
  expect_price (book.getBestBid ("MSFT"), int64_t {110});
  expect_cancel (book, 1, int64_t {5});
  expect_price (book.getBestBid ("MSFT"), int64_t {105});
}
