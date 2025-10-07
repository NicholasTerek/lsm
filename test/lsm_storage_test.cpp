#include "src/include/lsm_storage.hpp"
#include <gtest/gtest.h>

TEST(LsmStorageTest, StorageIntegration) {
    LsmStorageInner storage;
    
    // Test getting non-existent key returns None
    EXPECT_FALSE(storage.get("0").has_value());
    
    // Put some key-value pairs
    storage.put("1", "233");
    storage.put("2", "2333");
    storage.put("3", "23333");
    
    // Verify gets return correct values
    EXPECT_EQ(storage.get("1").value(), "233");
    EXPECT_EQ(storage.get("2").value(), "2333");
    EXPECT_EQ(storage.get("3").value(), "23333");
    
    // Delete a key and verify it returns None
    storage.delete_key("2");
    EXPECT_FALSE(storage.get("2").has_value());
    
    // Delete non-existent key - should NOT report any error
    storage.delete_key("0");
}

TEST(LsmStorageTest, BasicPutAndGet) {
    Lsm lsm;
    
    lsm.put("key1", "value1");
    lsm.put("key2", "value2");
    
    EXPECT_EQ(lsm.get("key1").value(), "value1");
    EXPECT_EQ(lsm.get("key2").value(), "value2");
    EXPECT_FALSE(lsm.get("key3").has_value());
}

TEST(LsmStorageTest, TombstoneDelete) {
    Lsm lsm;
    
    lsm.put("key1", "value1");
    EXPECT_EQ(lsm.get("key1").value(), "value1");
    
    lsm.delete_key("key1");
    EXPECT_FALSE(lsm.get("key1").has_value());
}

TEST(LsmStorageTest, OverwriteValue) {
    Lsm lsm;
    
    lsm.put("key1", "value1");
    lsm.put("key1", "value2");
    
    EXPECT_EQ(lsm.get("key1").value(), "value2");
}
