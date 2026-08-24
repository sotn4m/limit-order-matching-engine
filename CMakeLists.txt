cmake_minimum_required(VERSION 3.24)
project(limit_order_matching_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(order_book STATIC src/order_book.cpp)
target_include_directories(order_book PUBLIC ${CMAKE_SOURCE_DIR}/src)

include(FetchContent)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(order_book_tests tests/order_book_test.cpp)
target_include_directories(order_book_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(order_book_tests PRIVATE order_book GTest::gtest_main)
add_test(NAME order_book_tests COMMAND order_book_tests)
