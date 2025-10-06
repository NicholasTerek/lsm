#include "src/include/skiplist.hpp"
#include <gtest/gtest.h>


TEST(SkipListTest, Instantiation) {
    SkipList sl;
    EXPECT_TRUE(sl.Empty());
    EXPECT_EQ(sl.Size(), 0);
}

TEST(SkipListTest, InsertAndContains) {
    SkipList sl;
    EXPECT_TRUE(sl.Empty());

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
    EXPECT_EQ(sl.Size(), 1);
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

    sl.Insert("k4");
    EXPECT_TRUE(sl.Contains("k4").has_value());
    EXPECT_EQ(sl.Size(), 1);
}

TEST(SkipListTest, EraseExisting) {
    SkipList sl;
    sl.Insert("x");
    sl.Insert("y");
    sl.Insert("z");
    EXPECT_EQ(sl.Size(), 3);

    sl.Erase("y");
    EXPECT_EQ(sl.Size(), 2);
    EXPECT_TRUE(sl.Contains("x").has_value());
    EXPECT_FALSE(sl.Contains("y").has_value());
    EXPECT_TRUE(sl.Contains("z").has_value());
}

TEST(SkipListTest, EraseMissingNoCrash) {
    SkipList sl;
    sl.Insert("a");
    sl.Insert("c");
    sl.Erase("b"); 
    EXPECT_EQ(sl.Size(), 2);
    EXPECT_TRUE(sl.Contains("a").has_value());
    EXPECT_TRUE(sl.Contains("c").has_value());
}