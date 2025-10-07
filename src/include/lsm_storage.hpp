#pragma once
#include "mem_table.hpp"
#include <optional>
#include <string>
#include <vector>
#include <mutex>

// Represents the state of the storage engine
class LsmStorageState {
public:
    LsmStorageState();
    ~LsmStorageState();
    
    MemTable* memtable;
    
    std::vector<MemTable*> imm_memtables;
    
    static LsmStorageState create();
};

// The storage interface of the LSM tree
class LsmStorageInner {
public:
    LsmStorageInner();
    ~LsmStorageInner();
    
    std::optional<std::string> get(const std::string& key);
    void put(const std::string& key, const std::string& value);
    void delete_key(const std::string& key);
    
    // Force freeze the current memtable to an immutable memtable
    void force_freeze_memtable();
    
    // Test accessors
    int get_imm_memtables_count() const;
    int get_imm_memtable_size(int index) const;
    void set_target_sst_size(int size);
    int get_current_memtable_size() const;

private:
    LsmStorageState state_;
    
    // State lock for synchronizing state modifications
    std::mutex state_lock_;
    
    // Configuration
    int target_sst_size_;
    int next_sst_id_;
    
    // Helper to get next SST ID
    int next_sst_id();
    
    // Helper to check if memtable should be frozen
    bool try_freeze(int estimated_size);
};

// Thin wrapper for LsmStorageInner and the user interface
class Lsm {
public:
    Lsm();
    ~Lsm();
    
    std::optional<std::string> get(const std::string& key);
    void put(const std::string& key, const std::string& value);
    void delete_key(const std::string& key);

private:
    LsmStorageInner* inner_;
};