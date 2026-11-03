//
// Created by gamer on 11/2/2026.
//
#ifndef PROPHITS_BP_H
#define PROPHITS_BP_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Bplus {
    struct Node {
        bool leaf;
        vector<string> keys;

        Node(bool is_leaf) : leaf(is_leaf){}
        virtual ~Node() = default;

    };
    struct InternalNode : Node {
        vector<Node*> children;

        InternalNode() : Node(false){}
        ~InternalNode() {for (Node* i : children) delete i;}
    };
    struct LeafNode : Node {
        vector<float> values;
        LeafNode* next = nullptr;
        LeafNode* prev = nullptr;

        LeafNode() : Node(true){}
    };
    Node* root;
    LeafNode* find_leaf(const string& key, vector<InternalNode*>* path);
    int child_index(InternalNode* parent, Node* child);
    void split_leaf(LeafNode* leaf, vector<InternalNode*>& path);
    void split_internal(InternalNode* node, vector<InternalNode*>& path);
    void insert_to_parent(Node* left, const string& key, Node* right, vector<InternalNode*>& path);
public:
    Bplus() : root(new LeafNode()){}
    ~Bplus() {delete root;}
    bool search(const string& key, float& out);
    vector<pair<string, float>> range_search(const string& low, const string& high);
    void insertNode(const string& key, float value);
    bool remove(const string& key);

};
#endif //PROPHITS_BP_H