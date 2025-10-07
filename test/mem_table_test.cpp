#include "src/include/mem_table.hpp"
#include <gtest/gtest.h>

TEST(MemTableTest, PutAndGet) {
    MemTable mem;

    mem.put("key1", "value1");
    mem.put("key2", "value2");
    mem.put("key3", "value3");

    EXPECT_EQ(mem.get("key1").value(), "value1");
    EXPECT_EQ(mem.get("key2").value(), "value2");
    EXPECT_EQ(mem.get("key3").value(), "value3");
}

TEST(MemTableTest, OverwriteValue) {
    MemTable mem;

    mem.put("key1", "value1");
    mem.put("key2", "value2");
    mem.put("key3", "value3");

    mem.put("key1", "value11");
    mem.put("key2", "value22");
    mem.put("key3", "value33");

    EXPECT_EQ(mem.get("key1").value(), "value11");
    EXPECT_EQ(mem.get("key2").value(), "value22");
    EXPECT_EQ(mem.get("key3").value(), "value33");
}

TEST(MemTableTest, GetMissingKey) {
    MemTable mem;
    mem.put("key1", "value1");
    EXPECT_FALSE(mem.get("key2").has_value());
}
