#include "include/lsm_storage.hpp"


LsmStorageState::LsmStorageState() {
    // memtable is initialized automatically by its constructor
}

LsmStorageState LsmStorageState::create() {
    return LsmStorageState();
}

LsmStorageInner::LsmStorageInner() {
    // state_ is initialized automatically by its constructor
}

LsmStorageInner::~LsmStorageInner() {
    // Destructor handles cleanup automatically
}

std::optional<std::string> LsmStorageInner::get(const std::string& key) {
    std::optional<std::string> result = state_.memtable.get(key);
    
    if (result.has_value()) {
        if (result.value().empty()) {
            return std::nullopt; // Found tombstone
        }
        return result.value();
    }
    
    for (MemTable* memtable : state_.imm_memtables) {
        std::optional<std::string> result = memtable->get(key);
        if (result.has_value()) {
            if (result.value().empty()) {
                return std::nullopt; // Found tombstone
            }
            return result.value();
        }
    }
    
    return std::nullopt;
}

void LsmStorageInner::put(const std::string& key, const std::string& value) {
    state_.memtable.put(key, value);
}

void LsmStorageInner::delete_key(const std::string& key) {
    state_.memtable.put(key, "");
}

Lsm::Lsm() {
    inner_ = new LsmStorageInner();
}

Lsm::~Lsm() {
    delete inner_;
}

std::optional<std::string> Lsm::get(const std::string& key) {
    return inner_->get(key);
}

void Lsm::put(const std::string& key, const std::string& value) {
    inner_->put(key, value);
}

void Lsm::delete_key(const std::string& key) {
    inner_->delete_key(key);
}