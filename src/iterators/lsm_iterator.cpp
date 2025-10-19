#include "src/include/iterators/lsm_iterator.hpp"
#include "src/include/iterators/merge_iterator.hpp"
#include <memory>

LsmIterator::LsmIterator(std::unique_ptr<MergeIterator> inner) : LsmIteratorInner_(std::move(inner)){
    skip_deleted_keys();
}

std::unique_ptr<LsmIterator> LsmIterator::create(std::unique_ptr<MergeIterator> merge_iter) {
    return std::unique_ptr<LsmIterator>(new LsmIterator(std::move(merge_iter)));
}

void LsmIterator::skip_deleted_keys(){
    while(LsmIteratorInner_->is_valid() && LsmIteratorInner_->value().empty()){
        LsmIteratorInner_->next();
    }
}
std::string LsmIterator::key() {
    if (!LsmIteratorInner_->is_valid()) {
        return "";
    }
    return LsmIteratorInner_->key();
}

std::string LsmIterator::value() {
    if (!LsmIteratorInner_->is_valid()) {
        return "";
    }
    return LsmIteratorInner_->value();
}

bool LsmIterator::is_valid() {
    return LsmIteratorInner_->is_valid();
}

void LsmIterator::next() {
    if (!LsmIteratorInner_->is_valid()) {
        return;
    }
    LsmIteratorInner_->next();

    skip_deleted_keys();
}





FusedIterator::FusedIterator(std::unique_ptr<StorageIterator> inner)
    : inner_(std::move(inner)), has_errored_(false) {}

std::unique_ptr<FusedIterator> FusedIterator::create(std::unique_ptr<StorageIterator> inner) {
    return std::unique_ptr<FusedIterator>(new FusedIterator(std::move(inner)));
}

std::string FusedIterator::key() {
    if (has_errored_ || !inner_->is_valid()) {
        return "";
    }
    return inner_->key();
}

std::string FusedIterator::value() {
    if (has_errored_ || !inner_->is_valid()) {
        return "";
    }
    return inner_->value();
}

bool FusedIterator::is_valid() {
    if (has_errored_) {
        return false;
    }
    return inner_->is_valid();
}

void FusedIterator::next() {
    if (has_errored_ || !inner_->is_valid()) {
        return;
    }
    

    inner_->next();
}
