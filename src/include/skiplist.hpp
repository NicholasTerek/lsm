/*

A skip list is a probabilistic data structure with effective searching 

My Implemendation is based on: 
William Pugh. 1990. Skip lists: a probabilistic alternative to balanced trees. 
Commun. ACM 33, 6 (June 1990), 668–676. https://doi.org/10.1145/78973.78977

Author: Nicholas Terek
*/

#pragma once
#include <string>
#include <vector>
#include <optional>
#include <shared_mutex>

class SkipList {
public:
    SkipList();
    ~SkipList();

    bool isEmpty() const;
    int Size() const;
    void Clear();

    void Insert(const std::string& key, const std::string& value);
    void Erase(const std::string& key);
    std::optional<std::string> Contains(const std::string& key) const;

    // placeholder for later:
    // bool GetResult(const std::string& key);

private:
    struct Node;
    
    int max_level_;
    int level_;
    int size_ ;
    double prob_;
    Node* head_;

    mutable std::shared_mutex mu_;

    Node* FindGE_(const std::string& target, std::vector<Node*>& update) const;
    void ClearAll_();
    int RandomHeight_() const;
};
