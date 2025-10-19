#pragma once
#include "StorageIterator.hpp"
#include "merge_iterator.hpp"
#include <memory>
#include <string>

/**
 * LSMIterator wraps a MergeIterator over memtables and filters out deleted keys.
 */
class LsmIterator : StorageIterator {
public:
    explicit LsmIterator(std::unique_ptr<MergeIterator> inner);
    static std::unique_ptr<LsmIterator> create(std::unique_ptr<MergeIterator> merge_iter);

    std::string key() override;
    std::string value() override;
    bool is_valid() override;
    void next() override;

private:
    std::unique_ptr<MergeIterator> LsmIteratorInner_;
    void skip_deleted_keys();
};

class FusedIterator : StorageIterator {
public:
    explicit FusedIterator(std::unique_ptr<StorageIterator> inner);
    static std::unique_ptr<FusedIterator> create(std::unique_ptr<StorageIterator> inner);
    std::string key() override;
    std::string value() override;
    bool is_valid() override;
    void next() override;

private:
    bool has_errored_;
    std::unique_ptr<StorageIterator> inner_;

};