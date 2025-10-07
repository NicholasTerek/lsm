#pragma once
#include "mem_table.hpp"
#include <optional>
#include <string>
#include <vector>

// Represents the state of the storage engine
class LsmStorageState {
public:
    LsmStorageState();
    
    MemTable memtable;
    
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

private:
    LsmStorageState state_;
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