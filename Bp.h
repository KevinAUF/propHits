//
// Created by gamer on 11/2/2026.
//
#ifndef PROPHITS_BP_H
#define PROPHITS_BP_H

#include <iostream>
#include <string>
#include <vector>
#include "GameRow.h"
#include <cstddef>
#include <utility>

class Bplus {
public:
    using Rows = std::vector<GameRow>;
private:
    static const size_t ORDER = 64;
    static const size_t MAX_KEYS = ORDER - 1;
    struct Node {
        bool leaf;
        std::vector<std::string> keys;

        Node(bool is_leaf) : leaf(is_leaf){}
        virtual ~Node() = default;

    };
    struct InternalNode : Node {
        std::vector<Node*> children;

        InternalNode() : Node(false){}
        ~InternalNode() {for (Node* i : children) delete i;}
    };
    struct LeafNode : Node {
        std::vector<Rows> values;
        LeafNode* next = nullptr;
        LeafNode* prev = nullptr;

        LeafNode() : Node(true){}
    };
    using Path = std::vector<std::pair<InternalNode*, int>>;
    Node* root;
    LeafNode* find_leaf(const std::string& key, Path* path = nullptr);
    void split_leaf(LeafNode* leaf, Path& path);
    void split_internal(InternalNode* node, Path& path);
    void insert_to_parent(Node* left, const std::string& key, Node* right, Path& path);
public:
    Bplus() : root(new LeafNode()){}
    ~Bplus() {delete root;}
    Bplus(const Bplus&) = delete;
    Bplus& operator=(const Bplus&) = delete;
    const Rows* search(const std::string& key);
    std::vector<std::pair<std::string, const Rows*>> range_search(const std::string& low, const std::string& high);
    void insertNode(const std::string& key, const GameRow& row);

};
#endif //PROPHITS_BP_H