#include "include/lsm_storage.hpp"


LsmStorageState::LsmStorageState() {
    // Initialize with memtable of id 0
    memtable = new MemTable();
}

LsmStorageState::~LsmStorageState() {
    delete memtable;
    for (MemTable* mt : imm_memtables) {
        delete mt;
    }
}

LsmStorageState LsmStorageState::create() {
    return LsmStorageState();
}


LsmStorageInner::LsmStorageInner() {
    // state_ is initialized automatically by its constructor
    target_sst_size_ = 2 * 1024 * 1024; // 2MB default (like Rust)
    next_sst_id_ = 1;
}

LsmStorageInner::~LsmStorageInner() {
    // Destructor handles cleanup automatically
}

std::optional<std::string> LsmStorageInner::get(const std::string& key) {
    // Search on the current memtable (mirrors Rust implementation)
    std::optional<std::string> result = state_.memtable->get(key);
    
    if (result.has_value()) {
        // Check for tombstone (empty value means deleted)
        if (result.value().empty()) {
            return std::nullopt; // Found tombstone
        }
        return result.value();
    }
    
    // Search on immutable memtables (for later implementation)
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
    // Put a key-value pair into the storage by writing into the current memtable
    state_.memtable->put(key, value);
    
    // Check if memtable should be frozen after put
    int estimated_size = state_.memtable->Size();
    try_freeze(estimated_size);
}

void LsmStorageInner::delete_key(const std::string& key) {
    // Remove a key from the storage by writing an empty value (tombstone)
    state_.memtable->put(key, "");
    
    // Check if memtable should be frozen after delete (tombstone still takes space)
    int estimated_size = state_.memtable->Size();
    try_freeze(estimated_size);
}

void LsmStorageInner::force_freeze_memtable() {
    std::lock_guard<std::mutex> lock(state_lock_);
    
    // Force freeze regardless of size (as the name suggests)
    // Move current memtable to immutable list and create new one
    MemTable* old_memtable = state_.memtable;
    
    // Add to immutable memtables (latest first)
    state_.imm_memtables.insert(state_.imm_memtables.begin(), old_memtable);
    
    // Create new current memtable
    state_.memtable = new MemTable();
}

int LsmStorageInner::next_sst_id() {
    return next_sst_id_++;
}

bool LsmStorageInner::try_freeze(int estimated_size) {
    if (estimated_size >= target_sst_size_) {
        std::lock_guard<std::mutex> lock(state_lock_);
        
        // Double-check after acquiring lock (race condition prevention)
        if (state_.memtable->Size() >= target_sst_size_) {
            // Inline the freeze logic to avoid deadlock
            MemTable* old_memtable = state_.memtable;
            
            // Add to immutable memtables (latest first)
            state_.imm_memtables.insert(state_.imm_memtables.begin(), old_memtable);
            
            // Create new current memtable
            state_.memtable = new MemTable();
            return true;
        }
    }
    return false;
}

// Test accessors
int LsmStorageInner::get_imm_memtables_count() const {
    return state_.imm_memtables.size();
}

int LsmStorageInner::get_imm_memtable_size(int index) const {
    if (index >= 0 && index < static_cast<int>(state_.imm_memtables.size())) {
        return state_.imm_memtables[index]->Size();
    }
    return 0;
}

void LsmStorageInner::set_target_sst_size(int size) {
    target_sst_size_ = size;
}

int LsmStorageInner::get_current_memtable_size() const {
    return state_.memtable->Size();
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
