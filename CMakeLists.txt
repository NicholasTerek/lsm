cmake_minimum_required(VERSION 3.14)
project(lsm_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Google Test
find_package(GTest REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Create the skiplist library
add_library(skiplist src/skiplist.cpp)
target_include_directories(skiplist PUBLIC ${CMAKE_SOURCE_DIR})

# Create the test executable
add_executable(skiplist_test test/skiplist_test.cpp)
target_link_libraries(skiplist_test skiplist GTest::gtest GTest::gtest_main)

# Enable testing
enable_testing()
add_test(NAME SkipListTest COMMAND skiplist_test)
