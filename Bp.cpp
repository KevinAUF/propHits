//
// Created by gamer on 11/5/2026.
//
#include "Bp.h"
#include <algorithm>
#include <iterator>
using namespace std;

Bplus::LeafNode* Bplus::find_leaf(const string& key, Path* path) {
    Node* node = root;
    while (!node->leaf) {
        auto* in = static_cast<InternalNode*>(node);
        size_t i = upper_bound(in->keys.begin(), in->keys.end(), key) - in->keys.begin();
        if (path) {
            path->push_back({in ,(int)i});
        }
        node = in->children[i];
    }
    return static_cast<LeafNode*>(node);
}

void Bplus::insert_to_parent(Node* left, const string& key, Node* right, Path& path) {
    if (path.empty()) {
        auto* new_root = new InternalNode();
        new_root->keys.push_back(key);
        new_root->children = {left, right};
        root = new_root;
        return;
    }
    InternalNode* parent = path.back().first;
    size_t i = path.back().second;
    path.pop_back();
    parent->keys.insert(parent->keys.begin() + i, key);
    parent->children.insert(parent->children.begin() + i + 1, right);
    if (parent->keys.size() > MAX_KEYS) {
        split_internal(parent, path);
    }
}

void Bplus::split_leaf(LeafNode* leaf, Path& path) {
    auto* right = new LeafNode();
    std::size_t mid = leaf->keys.size() / 2;

    right->keys.assign(std::make_move_iterator(leaf->keys.begin() + mid),
                       std::make_move_iterator(leaf->keys.end()));
    right->values.assign(std::make_move_iterator(leaf->values.begin() + mid),
                         std::make_move_iterator(leaf->values.end()));
    leaf->keys.erase(leaf->keys.begin() + mid, leaf->keys.end());
    leaf->values.erase(leaf->values.begin() + mid, leaf->values.end());

    // splice right into the leaf linked list
    right->next = leaf->next;
    right->prev = leaf;
    if (leaf->next) {
        leaf->next->prev = right;
    }
    leaf->next = right;

    insert_to_parent(leaf, right->keys.front(), right, path);
}

void Bplus::split_internal(InternalNode* node, Path& path) {
    auto* right = new InternalNode();
    size_t mid = node->keys.size() / 2;
    string up = node->keys[mid];
    right->keys.assign(make_move_iterator(node->keys.begin() + mid + 1), make_move_iterator(node->keys.end()));
    right->children.assign(node->children.begin() + mid + 1, node->children.end());
    node->keys.erase(node->keys.begin() + mid, node->keys.end());
    node->children.erase(node->children.begin() + mid + 1, node->children.end());
    insert_to_parent(node, up, right, path);
}
const Bplus::Rows* Bplus::search(const string& key) {
    LeafNode* leaf = find_leaf(key);
    auto it = lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    if (it == leaf->keys.end() || *it != key) {
        return nullptr;
    }
    return &leaf->values[it - leaf->keys.begin()];
}

void Bplus::insertNode(const string& key, const GameRow& row) {
    Path path;
    LeafNode* leaf = find_leaf(key, &path);
    size_t i = lower_bound(leaf->keys.begin(), leaf->keys.end(), key) - leaf->keys.begin();
    if ( i < leaf->keys.size() && leaf->keys[i] == key) {
        leaf->values[i].push_back(row);
        return;
    }
    leaf->keys.insert(leaf->keys.begin() + i, key);
    leaf->values.insert(leaf->values.begin() + i, Rows{row});
    if (leaf->keys.size() > MAX_KEYS) {
        split_leaf(leaf, path);
    }
}

vector<pair<string, const Bplus::Rows*>> Bplus::range_search(const string& low, const string& high) {
    vector<pair<string, const Rows*>> out;
    LeafNode* leaf = find_leaf(low);
    size_t i = lower_bound(leaf->keys.begin(), leaf->keys.end(), low) - leaf->keys.begin();
    while (leaf != nullptr) {
        for (size_t j = i; j < leaf->keys.size(); j++) {
            if (leaf->keys[j] > high) {
                return out;
            }
            out.push_back({leaf->keys[j], &leaf->values[j]});
        }
        leaf = leaf->next;
        i = 0;
    }
    return out;
}