#include "include/mem_table.hpp"


MemTable::MemTable(){

}
MemTable::~MemTable(){

}

int MemTable::Id() {
    return id_;
}

int MemTable::Size() {
    return approximatesize_;
}
bool MemTable::isEmpty(){
    return map_.isEmpty();
}

void MemTable::Clear() {
    return; //todo
}

std::optional<std::string> MemTable::get(std::string key){
    std::optional<std::string> found = map_.Contains(key);
    return found;
}

bool MemTable::put(std::string key, std::string value){
    auto old = map_.Contains(key);
    if (old.has_value()) {
        int delta = value.size() - old->size();
        approximatesize_.fetch_add(delta);
    } else {
        approximatesize_.fetch_add(key.size() + value.size());
    }

    map_.Insert(key, value);
    return true;
}