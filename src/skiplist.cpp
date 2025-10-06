#include "include/skiplist.hpp"
#include <optional>
#include <random>
#include <vector>

struct Node {
    std::string key;
    std::vector<Node*> next;
    Node(std::string k, int h) : key(std::move(k)), next(h, nullptr) {}
};

double random_() {
    static std::random_device rd;
    static std::mt19937 engine(rd());
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(engine);
}

SkipList::SkipList() {
    max_level_ = 16;
    level_ = 1;
    size_ = 0;
    prob_ = 0.5;
    head_ = new Node("", max_level_);
}

SkipList::~SkipList() {
    Clear();
    delete head_;
    head_ = nullptr;
}

bool SkipList::Empty() const {
    return size_ == 0;
}

int SkipList::Size() const {
    return size_;
}


void SkipList::Insert(const std::string& key) {
    std::vector<Node*> update(max_level_, nullptr);
    Node* x = FindGE_(key, update);
    
    if (x && x->key == key) return;

    int node_level = RandomHeight_();
    if (node_level > level_) {
        for (int i = level_; i < node_level; ++i) update[i] = head_;
        level_ = node_level;
    }

    Node* n = new Node(key, node_level);
    for (int i = 0; i < node_level; ++i) {
        n->next[i] = update[i]->next[i];
        update[i]->next[i] = n;
    }
    ++size_;
}

void SkipList::Erase(const std::string& searchKey) {
    std::vector<Node*> update(max_level_, nullptr);
    Node* x = FindGE_(searchKey, update);
    if (!x || x->key != searchKey) return;

    for (int i = 0; i < level_; ++i) {
        if (update[i]->next[i] != x) break;
        update[i]->next[i] = x->next[i];
    }
    delete x;
    --size_;

    while (level_ > 1 && head_->next[level_ - 1] == nullptr) {
        --level_;
    }
}

std::optional<std::string> SkipList::Contains(const std::string& searchKey) const {
    Node* x = head_;
    for (int i = level_ - 1; i >= 0; --i) {
        while (x->next[i] && x->next[i]->key < searchKey) {
            x = x->next[i];
        }
    }
    x = x->next[0];
    if (x && x->key == searchKey) {
        return x->key;
    } else {
        return std::nullopt;
    }
}

Node* SkipList::FindGE_(const std::string& target, std::vector<Node*>& update) const {
    Node* x = head_;
    for (int i = level_ -1; i >= 0; --i) {
        while (x->next[i] && x->next[i]->key < target) {
            x = x->next[i];
        }
        update[i] = x;
    }
    return x->next[0];
}

void SkipList::Clear() {
    ClearAll_();
    for (int i = 0; i < max_level_; ++i) {
        head_->next[i] = nullptr;
    }
    size_ = 0;
    level_ = 1;
}

void SkipList::ClearAll_() {
    Node* x = head_->next[0];
    while (x) {
        Node* next = x->next[0];
        delete x;
        x = next;
    }
}

int SkipList::RandomHeight_() const {
    int level = 1;
    while (random_() < prob_ and level < max_level_) {
        level++;
    }
    return level;
}
