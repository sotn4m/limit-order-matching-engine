# Limit Order Matching Engine

Fintech-flavored C++ practice project: a simplified exchange matching engine with price–time priority.

**Language:** C++23  
**Runtime:** standard library only  
**Tests:** [Google Test](https://github.com/google/googletest) v1.17.0 (fetched by CMake)

## Build & run tests

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Run the test binary directly:

```bash
./build/order_book_tests
```

Run a single test:

```bash
./build/order_book_tests --gtest_filter=OrderBookTest.PartialFill
```

List all tests:

```bash
./build/order_book_tests --gtest_list_tests
```

## Clean

Remove all build artifacts, downloaded GTest, and CTest output:

```bash
rm -rf build Testing
```

Always run `ctest` with `--test-dir build`. Running `ctest` from the project root creates a stray `Testing/` folder in the source tree.

## Neovim / clangd

- `compile_commands.json` is generated in `build/`
- `.clangd` points clangd at that database

---

## Matching rules

1. **Price–time priority** for resting limit orders:
   - Best price first (highest bid, lowest ask).
   - At the same price, earlier `timestamp` wins.

2. **Limit orders** (`OrderType::Limit`) that do not fully match are **rested** on the book.

3. **Market orders** (`OrderType::Market`):
   - Match immediately against the best available opposite side.
   - Unfilled quantity is **discarded** (no resting).

4. **Trade price** = the **resting order's price** (standard price–time matching).

5. **Partial fills** are allowed; update remaining quantity on resting orders.

6. **Symbol isolation**: `AAPL` only matches `AAPL`.

7. **No self-trade**: an incoming order must not match against resting orders with the **same `order_id`**.

8. Assume valid input (positive quantity, valid side, non-empty symbol). Input validation is optional.

### Example walkthrough

```text
submit({1, "AAPL", Sell, Limit, 100, 10, 1})  -> []
submit({2, "AAPL", Buy,  Limit, 105,  5, 2})  -> [Trade{2,1,"AAPL",100,5}]
  // Order 1 rests with qty 5 @ 100

submit({3, "AAPL", Buy,  Limit, 100,  5, 3})  -> [Trade{3,1,"AAPL",100,5}]
  // Order 1 fully filled

submit({4, "AAPL", Sell, Limit,  99,  5, 4})  -> []
submit({5, "AAPL", Buy,  Market,  0, 20, 5}) -> [Trade{5,4,"AAPL",99,5}]
  // Market buy: 5 filled @ 99, remaining 15 discarded
```

---

## API

```cpp
class OrderBook {
 public:
  std::vector<Trade> submit(const Order& order);
  std::optional<int64_t> getBestAsk(const std::string& symbol) const;
  std::optional<int64_t> getBestBid(const std::string& symbol) const;
  std::optional<int64_t> cancelOrder(uint64_t orderId);
};
```

`cancelOrder` removes a resting order by ID and returns its remaining quantity, or `std::nullopt` if the ID is unknown.

---

## Project layout

```text
CMakeLists.txt
src/
  types.hpp         # Side, OrderType, Order, Trade
  symbol_book.hpp   # Per-symbol bid/ask book
  order_book.hpp    # OrderBook declaration
  order_book.cpp    # Matching logic
tests/
  test_helpers.hpp  # Builders and expect_* helpers
  order_book_test.cpp
```

---

## Test coverage

21 Google Test cases cover:

| Area | Scenarios |
|------|-----------|
| Matching | Partial fill, price–time priority, multi-fill, resting price |
| Market orders | Unfilled quantity discarded |
| Edge cases | No self-trade, symbol isolation, non-crossing rests |
| BBO | `getBestAsk`, `getBestBid`, promotion after fill |
| Cancel | Unknown ID, resting removal, BBO update |
| Integration | README example walkthrough |

---

## Follow-up topics

Possible next steps for interview-style depth:

- Per-symbol locking or sharded router for concurrent `submit` / `cancel`
- Property-based / fuzz testing against a naive reference book
- Decimal precision (fixed-point or integer ticks only)
- Persistence and recovery
- Feed-handler integration
