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

class Bplus { //B+ tree that stores players along with all the games they've played
public:
    using Rows = std::vector<GameRow>; //Game history for each player, each element of the vector is a game
private:
    static const size_t ORDER = 64;
    static const size_t MAX_KEYS = ORDER - 1; //Sets the order and max amount of keys for each node
    struct Node { //Parent or base struct, holds shared attributed of child structs
        bool leaf; //leaf node or internal node
        std::vector<std::string> keys;

        Node(bool is_leaf) : leaf(is_leaf){}
        virtual ~Node() = default;

    };
    struct InternalNode : Node { //sub struct that holds keys for the internal nodes
        std::vector<Node*> children;

        InternalNode() : Node(false){}
        ~InternalNode() {for (Node* i : children) delete i;}
    };
    struct LeafNode : Node { //sub struct for the leaves that will hold the data and also link the leaves into a list
        std::vector<Rows> values;
        LeafNode* next = nullptr;
        LeafNode* prev = nullptr;

        LeafNode() : Node(true){}
    };
    using Path = std::vector<std::pair<InternalNode*, int>>; //shortens syntax to make passing into functions easier
    Node* root;
    LeafNode* find_leaf(const std::string& key, Path* path = nullptr); //private functions for helping the main functions
    void split_leaf(LeafNode* leaf, Path& path);
    void split_internal(InternalNode* node, Path& path);
    void insert_to_parent(Node* left, const std::string& key, Node* right, Path& path);
public:
    Bplus() : root(new LeafNode()){} //default constructor, handles tree deletion and standard B+ tree functions
    ~Bplus() {delete root;}
    Bplus(const Bplus&) = delete;
    Bplus& operator=(const Bplus&) = delete;
    const Rows* search(const std::string& key);
    std::vector<std::pair<std::string, const Rows*>> range_search(const std::string& low, const std::string& high);
    void insert(const std::string& key, const GameRow& row);

};
#endif //PROPHITS_BP_H