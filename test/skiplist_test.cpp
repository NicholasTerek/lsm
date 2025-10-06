#include "src/include/skiplist.hpp"
#include <gtest/gtest.h>

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>



static inline std::string K(int x) { return "k" + std::to_string(x); }

// Tests

TEST(SkipListTest, Instantiation) {
    SkipList sl;
    EXPECT_TRUE(sl.Empty());
    EXPECT_EQ(sl.Size(), 0);
}

TEST(SkipListTest, InsertAndContains) {
    SkipList sl;

    sl.Insert("apple");
    sl.Insert("banana");
    sl.Insert("cherry");

    EXPECT_EQ(sl.Size(), 3);
    EXPECT_FALSE(sl.Empty());

    EXPECT_TRUE(sl.Contains("apple").has_value());
    EXPECT_TRUE(sl.Contains("banana").has_value());
    EXPECT_TRUE(sl.Contains("cherry").has_value());
    EXPECT_FALSE(sl.Contains("durian").has_value());
}

TEST(SkipListTest, DuplicateInsert) {
    SkipList sl;
    sl.Insert("a");
    sl.Insert("a");
    sl.Insert("b");
    sl.Insert("b");
    sl.Insert("b");
    sl.Insert("c");

    EXPECT_TRUE(sl.Contains("a").has_value());
    EXPECT_TRUE(sl.Contains("b").has_value());
    EXPECT_TRUE(sl.Contains("c").has_value());
    EXPECT_EQ(sl.Size(), 3);
}

TEST(SkipListTest, ClearResetsStructure) {
    SkipList sl;
    sl.Insert("k1");
    sl.Insert("k2");
    sl.Insert("k3");
    EXPECT_EQ(sl.Size(), 3);

    sl.Clear();
    EXPECT_TRUE(sl.Empty());
    EXPECT_EQ(sl.Size(), 0);
    EXPECT_FALSE(sl.Contains("k1").has_value());
    EXPECT_FALSE(sl.Contains("k2").has_value());
    EXPECT_FALSE(sl.Contains("k3").has_value());

    // Ensure list still works after Clear
    sl.Insert("k4");
    EXPECT_TRUE(sl.Contains("k4").has_value());
    EXPECT_EQ(sl.Size(), 1);
}

TEST(SkipListTest, EraseExistingMiddleHeadTail) {
    SkipList sl;
    // Lexicographic order: "", "a", "b", "c", "zz"
    sl.Insert("b");
    sl.Insert("a");
    sl.Insert("c");
    sl.Insert("zz");

    EXPECT_EQ(sl.Size(), 4);

    // Erase middle
    sl.Erase("b");
    EXPECT_EQ(sl.Size(), 3);
    EXPECT_FALSE(sl.Contains("b").has_value());
    EXPECT_TRUE(sl.Contains("a").has_value());
    EXPECT_TRUE(sl.Contains("c").has_value());
    EXPECT_TRUE(sl.Contains("zz").has_value());

    // Erase head-equivalent (smallest)
    sl.Erase("a");
    EXPECT_EQ(sl.Size(), 2);
    EXPECT_FALSE(sl.Contains("a").has_value());

    // Erase tail (largest)
    sl.Erase("zz");
    EXPECT_EQ(sl.Size(), 1);
    EXPECT_TRUE(sl.Contains("c").has_value());

    // Erase last
    sl.Erase("c");
    EXPECT_TRUE(sl.Empty());
    EXPECT_EQ(sl.Size(), 0);
}

TEST(SkipListTest, EraseMissingNoCrash) {
    SkipList sl;
    sl.Insert("a");
    sl.Insert("c");

    sl.Erase("b"); // not present
    EXPECT_EQ(sl.Size(), 2);
    EXPECT_TRUE(sl.Contains("a").has_value());
    EXPECT_TRUE(sl.Contains("c").has_value());
}

TEST(SkipListTest, AllowsEmptyStringKey) {
    SkipList sl;
    sl.Insert("");
    sl.Insert("x");
    EXPECT_EQ(sl.Size(), 2);
    EXPECT_TRUE(sl.Contains("").has_value());
    EXPECT_TRUE(sl.Contains("x").has_value());

    sl.Erase("");
    EXPECT_EQ(sl.Size(), 1);
    EXPECT_FALSE(sl.Contains("").has_value());
    EXPECT_TRUE(sl.Contains("x").has_value());
}

TEST(SkipListTest, EraseNonExistingElement) {
    SkipList list;

    // Insert some elements
    for (int i = 0; i < 5; ++i) {
        list.Insert(K(i));
    }

    // Try erasing a non-existing element (Erase is void in your API)
    list.Erase(K(10));

    // Size should remain the same, all originals still present
    EXPECT_EQ(list.Size(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(list.Contains(K(i)).has_value());
    }
    EXPECT_FALSE(list.Contains(K(10)).has_value());
}

TEST(SkipListTest, ConcurrentInsertTest) {
    SkipList list;
    const int num_threads = 10;
    const int num_insertions_per_thread = 100;

    auto insert_task = [&](int start) {
        for (int i = start; i < start + num_insertions_per_thread; ++i) {
            list.Insert(K(i)); // your Insert is void
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(insert_task, t * num_insertions_per_thread);
    }
    for (auto &th : threads) th.join();

    // Verify that all inserted keys are present
    for (int i = 0; i < num_threads * num_insertions_per_thread; ++i) {
        EXPECT_TRUE(list.Contains(K(i)).has_value());
    }

    // Optional: check size (target to work toward with thread-safety)
    EXPECT_EQ(list.Size(), num_threads * num_insertions_per_thread);
}

TEST(SkipListTest, ConcurrentEraseTest) {
    SkipList list;

    // Initially insert some elements into the list
    for (int i = 0; i < 100; ++i) {
        list.Insert(K(i));
    }

    const int num_threads = 10;
    const int num_erasures_per_thread = 10;

    auto erase_task = [&](int start) {
        for (int i = start; i < start + num_erasures_per_thread; ++i) {
            list.Erase(K(i)); // void Erase
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(erase_task, t * num_erasures_per_thread);
    }
    for (auto &th : threads) th.join();

    // Verify that erased elements are no longer in the list
    for (int i = 0; i < 100; ++i) {
        if (i < num_threads * num_erasures_per_thread) {
            EXPECT_FALSE(list.Contains(K(i)).has_value());
        } else {
            EXPECT_TRUE(list.Contains(K(i)).has_value());
        }
    }
}

TEST(SkipListTest, ConcurrentInsertAndEraseTest) {
    SkipList list;

    // Initially insert some elements into the list
    for (int i = 0; i < 100; ++i) {
        list.Insert(K(i));
    }

    const int num_threads = 10;
    const int num_operations_per_thread = 10;

    auto task = [&](int start) {
        for (int i = start; i < start + num_operations_per_thread; ++i) {
            if (!list.Contains(K(i)).has_value()) {
                list.Insert(K(i));
            }
            list.Insert(K(i + 100)); // insert a new disjoint range
            list.Erase(K(i));        // erase from the original range
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(task, t * num_operations_per_thread);
    }
    for (auto &th : threads) th.join();

    // Verify that elements that were inserted are in the list
    for (int i = 100; i < 100 + num_threads * num_operations_per_thread; ++i) {
        EXPECT_TRUE(list.Contains(K(i)).has_value());
    }

    // Verify that elements that were erased are no longer in the list
    for (int i = 0; i < num_threads * num_operations_per_thread; ++i) {
        EXPECT_FALSE(list.Contains(K(i)).has_value());
    }
}

TEST(SkipListTest, ConcurrentReadTest) {
    // Number of threads to use for concurrent reads
    const int num_threads = 8;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    // Use a smaller total locally if needed
    const int total_num_elements = num_threads * 10000;

    auto list = std::make_unique<SkipList>();
    // Insert some elements into the skip list
    for (int i = 0; i < total_num_elements; ++i) {
        list->Insert(K(i));
    }

    // Function to perform concurrent reads
    auto read_task = [&list](int start, int end) {
        for (int i = start; i < end; ++i) {
            (void)list->Contains(K(i)); // ignore result, just stress lookups
        }
    };

    // Launch threads to perform concurrent reads
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(read_task, 0, total_num_elements);
    }

    for (auto &th : threads) th.join();

    // Spot-check a couple values to still exist
    EXPECT_TRUE(list->Contains(K(0)).has_value());
    EXPECT_TRUE(list->Contains(K(total_num_elements - 1)).has_value());
}
