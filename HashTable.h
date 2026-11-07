// HashTable.h
// PropHits - Group 37
// Team Name: GatorBoys
//
// Overview on why Hash table:
// Hash table that maps a player's name -> all of that player's games.
// looking up a player is one hash + a short walk down one bucket,
// instead of searching through 100K+ rows.


#ifndef PROPHIT_HASHTABLE_H
#define PROPHIT_HASHTABLE_H

#include "GameRow.h"
#include <algorithm>
#include <list>
#include <string>
#include <vector>
using namespace std;

class HashTable {
private:
    struct Node {
        string key;
        vector<GameRow> games;
    };

    vector<std::list<Node>> table;
    int capacity;
    int numPlayers;
    int numRows;
    double maxLoadFactor;

// Polynomial rolling hash: hash = (hash * 31 + c) % capacity
    int hash(const string& key) const {
        long long h = 0;
        for (int i = 0; i < (int)key.size(); i++) {
            h = (h * 31 + (unsigned char)key[i]) % capacity;
        }
        return (int)h;
    }


// Doubles the buckets and puts every player into its new bucket.
    void rehash() {
        vector<std::list<Node>> oldTable = table;
        capacity = capacity * 2;
        table.clear();
        table.resize(capacity);

        for (int i = 0; i < (int)oldTable.size(); i++) {
            for (Node& node : oldTable[i]) {
            // New index with new capacity.
                int index = hash(node.key);
                table[index].push_back(node);
            }
        }
    }

// Sort rows from lowest to highest value of the chosen stat.
    static void sortByStat(std::vector<GameRow>& rows, StatType stat) {
        std::sort(rows.begin(), rows.end(),
                  [stat](const GameRow& a, const GameRow& b) {
                      return a.getStat(stat) < b.getStat(stat);
                  });
    }

public:
    HashTable(int startingCapacity = 1009, double loadFactor = 0.75) {
        capacity = (startingCapacity > 0) ? startingCapacity : 1009;
        maxLoadFactor = loadFactor;
        numPlayers = 0;
        numRows = 0;
        table.resize(capacity);
    }

//If the player is already in the table, add the game to their list. Otherwise make a new node for the player.
    void insert(const GameRow& row) {
        string key = GameRow::normalize(row.playerName);
        int index = hash(key);

        // Walk the chain looking for this player
        for (Node& node : table[index]) {
            if (node.key == key) {
                node.games.push_back(row);
                numRows++;
                return;
            }
        }

        //New player: resize first if we exceed the load factor
        if ((double)(numPlayers + 1) / capacity > maxLoadFactor) {
            rehash();
            index = hash(key);
        }

        Node newNode;
        newNode.key = key;
        newNode.games.push_back(row);
        table[index].push_back(newNode);
        numPlayers++;
        numRows++;
    }

// Return a pointer to the player's games, or nullptr if not found. Also this makes sure we dont keep the
// pointer after another insert.
    const vector<GameRow>* find(const string& playerName) const {
        string key = GameRow::normalize(playerName);
        int index = hash(key);

        for (const Node& node : table[index]) {
            if (node.key == key) return &node.games;
        }
        return nullptr;
    }

    bool contains(const string& playerName) const {
        return find(playerName) != nullptr;
    }

//One player's games where low <= stat <= high, sorted low to high.
    vector<GameRow> rangeQuery(const string& playerName, StatType stat, int low, int high) const {
        vector<GameRow> result;
        const vector<GameRow>* games = find(playerName);
        if (games == nullptr) return result;

        for (const GameRow& game : *games) {
            int value = game.getStat(stat);
            if (value >= low && value <= high) result.push_back(game);
        }
       sortByStat(result, stat);
        return result;
    }

//Same as rangeQuery but this one is for every player (player name left blank).
    vector<GameRow> rangeQueryAll(StatType stat, int low, int high) const {
        vector<GameRow> result;
        for (int i = 0; i < capacity; i++) {
            for (const Node& node : table[i]) {
                for (const GameRow& game : node.games) {
                    int value = game.getStat(stat);
                    if (value >= low && value <= high) result.push_back(game);
                }
            }
        }
        sortByStat(result, stat);
        return result;
    }

// Numbers for the performance comparison
    int playerCount() const { return numPlayers; }
    int rowCount() const { return numRows; }
    int bucketCount() const { return capacity; }
    double loadFactor() const { return (double)numPlayers / capacity; }

// Length of the longest chain
    int longestChain() const {
        int longest = 0;
        for (int i = 0; i < capacity; i++) {
            if ((int)table[i].size() > longest) longest = (int)table[i].size();
        }
        return longest;
    }

// Rough estimate of memory used, in bytes
    long long memoryUsageBytes() const {
        long long bytes = sizeof(*this);
        bytes += (long long)capacity * sizeof(std::list<Node>);
        for (int i = 0; i < capacity; i++) {
            for (const Node& node : table[i]) {
                bytes += sizeof(Node) + 2 * sizeof(void*);   // node + list links
                bytes += node.key.capacity();
                bytes += (long long)node.games.capacity() * sizeof(GameRow);
            }
        }
        return bytes;
    }

    void clear() {
        table.clear();
        table.resize(capacity);
        numPlayers = 0;
        numRows = 0;
    }
};

#endif //PROPHIT_HASHTABLE_H
