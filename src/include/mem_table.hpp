#include "skiplist.hpp"
#include <atomic>
#include <optional>
#include <string>

class MemTable {
public:
    MemTable();
    ~MemTable();

    int Id();
    int Size();
    bool isEmpty();
    void Clear();

    std::optional<std::string> get(std::string key);
    bool put(std::string key, std::string value);
private:
    SkipList map_;
    int id_;
    std::atomic<int> approximatesize_;
//  WriteAheadLog log;

};