#include "src/include/StorageIterator.hpp"
#include "src/include/data_structures/skiplist.hpp"
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

    class MemTableIterator : public StorageIterator {
    public:
        MemTableIterator();
        MemTableIterator(SkipList::Node* current);

        std::string key() override;
        std::string value() override;
        bool is_valid() override;
        void next() override;

    private:
        SkipList::Node* current_node_;
    };

    MemTableIterator begin() const;
    MemTableIterator scan(const std::string& lower_bound, const std::string& upper_bound) const;
private:
    SkipList map_;
    int id_;
    std::atomic<int> approximatesize_;
//  WriteAheadLog log;

};
