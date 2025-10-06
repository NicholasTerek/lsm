#include "src/include/skiplist.hpp"
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <thread>
#include <vector>

static inline std::string K(int x) { return "k" + std::to_string(x); }
static inline std::string V(int x) { return "v" + std::to_string(x); }

TEST(SkipListTest, Instantiation) {
    SkipList sl;
    EXPECT_TRUE(sl.isEmpty());
    EXPECT_EQ(sl.Size(), 0);
}

TEST(SkipListTest, InsertAndGet) {
    SkipList sl;

    sl.Insert("apple",  "red");
    sl.Insert("banana", "yellow");
    sl.Insert("cherry", "dark");

    EXPECT_EQ(sl.Size(), 3);
    EXPECT_FALSE(sl.isEmpty());

    EXPECT_EQ(sl.Contains("apple").value(),  "red");
    EXPECT_EQ(sl.Contains("banana").value(), "yellow");
    EXPECT_EQ(sl.Contains("cherry").value(), "dark");
    EXPECT_FALSE(sl.Contains("durian").has_value());
}

TEST(SkipListTest, OverwriteDoesNotGrowSize) {
    SkipList sl;
    sl.Insert("a", "1");
    sl.Insert("a", "2"); 
    sl.Insert("b", "3");
    sl.Insert("b", "4"); 
    sl.Insert("c", "5");

    EXPECT_TRUE(sl.Contains("a").has_value());
    EXPECT_TRUE(sl.Contains("b").has_value());
    EXPECT_TRUE(sl.Contains("c").has_value());
    EXPECT_EQ(sl.Size(), 3); 
    EXPECT_EQ(sl.Contains("a").value(), "2");
    EXPECT_EQ(sl.Contains("b").value(), "4");
    EXPECT_EQ(sl.Contains("c").value(), "5");
}

TEST(SkipListTest, ClearResetsStructure) {
    SkipList sl;
    sl.Insert("k1", "v1");
    sl.Insert("k2", "v2");
    sl.Insert("k3", "v3");
    EXPECT_EQ(sl.Size(), 3);

    sl.Clear();
    EXPECT_TRUE(sl.isEmpty());
    EXPECT_EQ(sl.Size(), 0);
    EXPECT_FALSE(sl.Contains("k1").has_value());
    EXPECT_FALSE(sl.Contains("k2").has_value());
    EXPECT_FALSE(sl.Contains("k3").has_value());

    // Ensure list still works after Clear
    sl.Insert("k4", "v4");
    EXPECT_TRUE(sl.Contains("k4").has_value());
    EXPECT_EQ(sl.Contains("k4").value(), "v4");
    EXPECT_EQ(sl.Size(), 1);
}

TEST(SkipListTest, EraseExistingMiddleHeadTail) {
    SkipList sl;
    // Lexicographic order: "", "a", "b", "c", "zz"
    sl.Insert("b", "vb");
    sl.Insert("a", "va");
    sl.Insert("c", "vc");
    sl.Insert("zz", "vzz");

    EXPECT_EQ(sl.Size(), 4);

    // Erase middle
    sl.Erase("b");
    EXPECT_EQ(sl.Size(), 3);
    EXPECT_FALSE(sl.Contains("b").has_value());
    EXPECT_EQ(sl.Contains("a").value(),  "va");
    EXPECT_EQ(sl.Contains("c").value(),  "vc");
    EXPECT_EQ(sl.Contains("zz").value(), "vzz");

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
    EXPECT_TRUE(sl.isEmpty());
    EXPECT_EQ(sl.Size(), 0);
}

TEST(SkipListTest, EraseMissingNoCrash) {
    SkipList sl;
    sl.Insert("a", "va");
    sl.Insert("c", "vc");

    sl.Erase("b"); // not present
    EXPECT_EQ(sl.Size(), 2);
    EXPECT_TRUE(sl.Contains("a").has_value());
    EXPECT_TRUE(sl.Contains("c").has_value());
}

TEST(SkipListTest, AllowsEmptyStringKey) {
    SkipList sl;
    sl.Insert("", "vz");
    sl.Insert("x", "vx");
    EXPECT_EQ(sl.Size(), 2);
    EXPECT_EQ(sl.Contains("").value(), "vz");
    EXPECT_EQ(sl.Contains("x").value(), "vx");

    sl.Erase("");
    EXPECT_EQ(sl.Size(), 1);
    EXPECT_FALSE(sl.Contains("").has_value());
    EXPECT_TRUE(sl.Contains("x").has_value());
}

TEST(SkipListTest, EraseNonExistingElement) {
    SkipList list;

    for (int i = 0; i < 5; ++i) {
        list.Insert(K(i), V(i));
    }

    list.Erase(K(10)); // not present

    EXPECT_EQ(list.Size(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(list.Contains(K(i)).has_value());
        EXPECT_EQ(list.Contains(K(i)).value(), V(i));
    }
    EXPECT_FALSE(list.Contains(K(10)).has_value());
}

TEST(SkipListTest, ConcurrentInsertTest) {
    SkipList list;
    const int num_threads = 10;
    const int num_insertions_per_thread = 100;

    auto insert_task = [&](int start) {
        for (int i = start; i < start + num_insertions_per_thread; ++i) {
            list.Insert(K(i), V(i));
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(insert_task, t * num_insertions_per_thread);
    }
    for (auto &th : threads) th.join();

    for (int i = 0; i < num_threads * num_insertions_per_thread; ++i) {
        auto got = list.Contains(K(i));
        ASSERT_TRUE(got.has_value());
        EXPECT_EQ(*got, V(i));
    }
    EXPECT_EQ(list.Size(), static_cast<size_t>(num_threads * num_insertions_per_thread));
}

TEST(SkipListTest, ConcurrentEraseTest) {
    SkipList list;

    for (int i = 0; i < 100; ++i) {
        list.Insert(K(i), V(i));
    }

    const int num_threads = 10;
    const int num_erasures_per_thread = 10;

    auto erase_task = [&](int start) {
        for (int i = start; i < start + num_erasures_per_thread; ++i) {
            list.Erase(K(i));
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(erase_task, t * num_erasures_per_thread);
    }
    for (auto &th : threads) th.join();

    for (int i = 0; i < 100; ++i) {
        if (i < num_threads * num_erasures_per_thread) {
            EXPECT_FALSE(list.Contains(K(i)).has_value());
        } else {
            auto got = list.Contains(K(i));
            ASSERT_TRUE(got.has_value());
            EXPECT_EQ(*got, V(i));
        }
    }
}

TEST(SkipListTest, ConcurrentInsertAndEraseTest) {
    SkipList list;

    for (int i = 0; i < 100; ++i) {
        list.Insert(K(i), V(i));
    }

    const int num_threads = 10;
    const int num_operations_per_thread = 10;

    auto task = [&](int start) {
        for (int i = start; i < start + num_operations_per_thread; ++i) {
            if (!list.Contains(K(i)).has_value()) {
                list.Insert(K(i), V(i));
            }
            list.Insert(K(i + 100), V(i + 100)); // disjoint range
            list.Erase(K(i));                    // remove original
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(task, t * num_operations_per_thread);
    }
    for (auto &th : threads) th.join();

    for (int i = 100; i < 100 + num_threads * num_operations_per_thread; ++i) {
        auto got = list.Contains(K(i));
        ASSERT_TRUE(got.has_value());
        EXPECT_EQ(*got, V(i));
    }
    for (int i = 0; i < num_threads * num_operations_per_thread; ++i) {
        EXPECT_FALSE(list.Contains(K(i)).has_value());
    }
}

TEST(SkipListTest, ConcurrentReadTest) {
    const int num_threads = 8;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    const int total_num_elements = num_threads * 10000;

    auto list = std::make_unique<SkipList>();
    for (int i = 0; i < total_num_elements; ++i) {
        list->Insert(K(i), V(i));
    }

    auto read_task = [&list](int start, int end) {
        for (int i = start; i < end; ++i) {
            (void)list->Contains(K(i));
        }
    };

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back(read_task, 0, total_num_elements);
    }

    for (auto &th : threads) th.join();

    EXPECT_TRUE(list->Contains(K(0)).has_value());
    EXPECT_TRUE(list->Contains(K(total_num_elements - 1)).has_value());
}
