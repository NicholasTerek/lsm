#include "src/include/lsm_storage.hpp"
#include <gtest/gtest.h>
#include <iostream>

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

TEST(LsmStorageTest, StorageIntegrationFreeze) {
    LsmStorageInner storage;
    
    storage.put("1", "233");
    storage.put("2", "2333");
    storage.put("3", "23333");
    
    // Force freeze current memtable
    storage.force_freeze_memtable();
    
    // Should have 1 immutable memtable
    EXPECT_EQ(storage.get_imm_memtables_count(), 1);
    
    // Get the size of the first frozen memtable
    int previous_approximate_size = storage.get_imm_memtable_size(0);
    EXPECT_GE(previous_approximate_size, 15);
    
    // Add more data to new memtable
    storage.put("1", "2333");
    storage.put("2", "23333");
    storage.put("3", "233333");
    
    // Force freeze again
    storage.force_freeze_memtable();
    
    // Should have 2 immutable memtables
    EXPECT_EQ(storage.get_imm_memtables_count(), 2);
    
    // Check order - latest frozen should be at index 0 (newest first)
    EXPECT_EQ(storage.get_imm_memtable_size(1), previous_approximate_size);
    EXPECT_GT(storage.get_imm_memtable_size(0), previous_approximate_size);
}

TEST(LsmStorageTest, FreezeOnCapacity) {
    LsmStorageInner storage;
    
    // Set very small target size to trigger freezing
    storage.set_target_sst_size(50);
    
    // Add data with unique keys until it should trigger automatic freezing
    for (int i = 0; i < 20; i++) {
        std::string key = "key" + std::to_string(i);
        storage.put(key, "2333");
        int current_size = storage.get_current_memtable_size();
    }
    
    int final_size = storage.get_current_memtable_size();
    
    // Should have at least 1 immutable memtable from automatic freezing
    int num_imm_memtables = storage.get_imm_memtables_count();
    EXPECT_GE(num_imm_memtables, 1) << "No memtable frozen?";
    
    // Add more unique keys to trigger more freezing
    for (int i = 20; i < 40; i++) {
        std::string key = "key" + std::to_string(i);
        storage.put(key, "2333");
    }
    
    // Should have more frozen memtables
    EXPECT_GT(storage.get_imm_memtables_count(), num_imm_memtables) 
        << "No more memtable frozen?";
}
