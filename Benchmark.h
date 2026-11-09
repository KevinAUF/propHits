//
// Created by gamer on 11/8/2026.
//

#ifndef PROPHITS_BENCHMARK_H
#define PROPHITS_BENCHMARK_H

#include "GameRow.h"
#include "HashTable.h"
#include "Bp.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
inline chrono::high_resolution_clock::time_point startTimer() {
    return chrono::high_resolution_clock::now();
}                                                                   //Timer functions, start timer by saving current time and stop timer returns how much time has passed

inline double stopTimer(chrono::high_resolution_clock::time_point t0) {
    auto t1 = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(t1 - t0).count();
}

inline vector<string> uniqueNames(const vector<GameRow>& rows) { //Builds a list of players to use for tests
    HashTable seen;
    vector<string> names;
    for (const GameRow& row : rows) {
        if (!seen.contains(row.playerName)) {
            seen.insert(row);
            names.push_back(GameRow::normalize(row.playerName));
        }
    }
    return names;
}

inline double timeHashInsert(HashTable& table, const vector<GameRow>& rows) { //time for inserting into hash table
    auto t0 = startTimer();
    for (const GameRow& row : rows) {
        table.insert(row);
    }
    return stopTimer(t0);
}

inline double timeTreeInsert(Bplus& tree, const vector<GameRow>& rows) { //time for inserting into B+ tree
    auto t0 = startTimer();
    for (const GameRow& row : rows) {
        tree.insert(GameRow::normalize(row.playerName), row);
    }
    return stopTimer(t0);
}

inline double timeHashLookup(const HashTable& table, const vector<string>& names, int repeats, long long& found) { //Time for lookup in hash table, keeps track of whether or not lookups are successful
    found = 0;
    auto t0 = startTimer();
    for (int r = 0; r < repeats; r++) {
        for (const string& name : names) {
            if (table.find(name) != nullptr) {
                found++;
            }
        }
    }
    return stopTimer(t0);
}

inline double timeTreeLookup(Bplus& tree, const vector<string>& names, int repeats, long long& found) { //Time for lookup in the B+ tree, keeps track of whether or not lookups are successful
    found = 0;
    auto t0 = startTimer();
    for (int r = 0; r < repeats; r++) {
        for (const string& name : names) {
            if (tree.search(name) != nullptr) {
                found++;
            }
        }
    }
    return stopTimer(t0);
}

inline double timeHashNameRange(const HashTable& table, const vector<string>& names, const string& low, const string& high, int repeats, int& count) { //Time for range_search onm hash table
    auto t0 = startTimer();
    for (int r = 0; r < repeats; r++) {
        count = 0;
        for (const string& name : names) {
            if (name >= low && name <= high && table.contains(name)) {
                count++;
            }
        }
    }
    return stopTimer(t0);
}

inline double timeTreeNameRange(Bplus& tree, const string& low, const string& high, int repeats, int& count) { //Time for range_search on B+ tree
    auto t0 = startTimer();
    for (int r = 0; r < repeats; r++) {
        count = (int)tree.range_search(low, high).size();
    }
    return stopTimer(t0);
}

inline void printRow(const string& test, double hashtime, double treetime) { //Print functions show the output of the tests, showing which data structures were faster and when
    string faster = (hashtime < treetime) ? "Hash" : "B+ tree";
    cout << left << setw(34) << test << setw(14) << hashtime << setw(14) << treetime << faster << "\n";
}

inline void runBenchmarks(const vector<GameRow>& rows) {
    const int LOOKUP_REPEATS = 10;
    const int SEARCH_REPEATS = 100;
    const string RANGE_LOW = "a";
    const string RANGE_HIGH = "c";
    vector<string> names = uniqueNames(rows);
    HashTable hash;
    Bplus tree;
    double hashInsert = timeHashInsert(hash, rows);
    double treeInsert = timeTreeInsert(tree, rows);
    long long hashFound = 0, treeFound = 0;
    double hashLookup = timeHashLookup(hash, names, LOOKUP_REPEATS, hashFound);
    double treeLookup = timeTreeLookup(tree, names, LOOKUP_REPEATS, treeFound);
    int hashRangeCount = 0, treeRangeCount = 0;
    double hashRange = timeHashNameRange(hash, names, RANGE_LOW, RANGE_HIGH, SEARCH_REPEATS, hashRangeCount);
    double treeRange = timeTreeNameRange(tree, RANGE_LOW, RANGE_HIGH, SEARCH_REPEATS, treeRangeCount);
    long long totalLookups = (long long)names.size() * LOOKUP_REPEATS;
    cout << "\n===== PropHits Benchmark =====\n";
    cout << "Rows: " << rows.size() << "   Players: " << names.size() << "\n\n";
    cout << fixed << setprecision(3);
    cout << left << setw(34) << "Test (total ms)" << setw(14) << "Hash table"
         << setw(14) << "B+ tree" << "Faster\n";
    cout << string(70, '-') << "\n";
    printRow("Insert all rows", hashInsert, treeInsert);
    printRow("Lookups (" + to_string(totalLookups) + ")", hashLookup, treeLookup);
    printRow("Names \"" + RANGE_LOW + "\"-\"" + RANGE_HIGH + "\" (x" + to_string(SEARCH_REPEATS) + ")",hashRange, treeRange);
    cout << "\nChecks (hash / tree):\n";
    cout << "  Lookups found:  " << hashFound << " / " << treeFound<< ((hashFound == treeFound && hashFound == totalLookups) ? "   OK" : "   MISMATCH") << "\n";
    cout << "  Range matches:  " << hashRangeCount << " / " << treeRangeCount<< ((hashRangeCount == treeRangeCount) ? "   OK" : "   MISMATCH") << "\n";
    cout << "\nHash table stats:\n";
    cout << "  Buckets:        " << hash.bucketCount() << "\n";
    cout << "  Load factor:    " << hash.loadFactor() << "\n";
    cout << "  Longest chain:  " << hash.longestChain() << "\n";
    cout << "  Memory (MB):    " << hash.memoryUsageBytes() / (1024.0 * 1024.0) << "\n";
}
#endif //PROPHITS_BENCHMARK_H