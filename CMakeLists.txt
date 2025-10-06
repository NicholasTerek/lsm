cmake_minimum_required(VERSION 3.16)
project(lsm_project LANGUAGES CXX)

# ---- C++ standard ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Build type (default Debug if not set) ----
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ---- Options ----
option(ENABLE_ASAN "Enable AddressSanitizer for tests" ON)

# ---- GTest ----
find_package(GTest REQUIRED)

# ---- Include paths ----
# Prefer pointing to your public headers folder (adjust if your header path differs)
# e.g., your header is include/skiplist.hpp
include_directories(${CMAKE_SOURCE_DIR})

# ---- Library (your implementation) ----
add_library(skiplist src/skiplist.cpp)
target_include_directories(skiplist PUBLIC ${CMAKE_SOURCE_DIR}/include)
# Nice debug defaults
target_compile_options(skiplist PRIVATE
  $<$<CONFIG:Debug>:-O0 -g3 -fno-omit-frame-pointer>
)

# ---- Test binary ----
add_executable(skiplist_test test/skiplist_test.cpp)
target_link_libraries(skiplist_test PRIVATE skiplist GTest::gtest GTest::gtest_main pthread)
target_compile_options(skiplist_test PRIVATE
  $<$<CONFIG:Debug>:-O0 -g3 -fno-omit-frame-pointer>
)

# Optional ASan on tests only
if(ENABLE_ASAN)
  target_compile_options(skiplist_test PRIVATE -fsanitize=address)
  target_link_options(skiplist_test PRIVATE -fsanitize=address)
endif()

# ---- CTest integration ----
enable_testing()
add_test(NAME SkipListTest COMMAND skiplist_test)
