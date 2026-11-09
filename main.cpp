/*
 main.cpp
 PropHits - Group 37
 Team Name: GatorBoys

 Terminal menu: analyze players
*/

#include <chrono>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "GameRow.h"
#include "DataLoader.h"
#include "HashTable.h"
#include "Bp.h"
#include "DefenseTable.h"
#include "PropModel.h"
#include "Benchmark.h"

using namespace std;

const int LAST_N = 17;   // how many recent games the model uses

string readLine(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}


int readInt(const string& prompt, int low, int high) {
    while (true) {
        string input = readLine(prompt);
        try {
            int value = stoi(input);
            if (value >= low && value <= high) return value;
        } catch (...) {}
        cout << "Please enter a number from " << low << " to " << high << "." << endl;
    }
}

// read positive number
double readPositive(const string& prompt) {
    while (true) {
        string input = readLine(prompt);
        try {
            double value = stod(input);
            if (value > 0) return value;
        } catch (...) {}
        cout << "Please enter a positive number, like 75.5." << endl;
    }
}

//reading Over or Under
bool readOverUnder(const string& prompt) {
    while (true) {
        string input = GameRow::normalize(readLine(prompt));
        if (input == "o" || input == "over") return true;
        if (input == "u" || input == "under") return false;
        cout << "Please enter O or U." << endl;
    }
}

//reads team code
string readOpponent(const string& prompt, const set<string>& teams) {
    while (true) {
        string input = readLine(prompt);
        string team;
        for (char c : input) {
            if (c == ' ') continue;
            if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
            team += c;
        }
        if (team.empty() || teams.count(team)) return team;
        cout << "Unknown team. Use a code like DAL, KC, or SF (or leave blank)." << endl;
    }
}


string statName(StatType stat) {
    if (stat == Pass_Yds) return "pass yds";
    if (stat == Rush_Yds) return "rush yds";
    if (stat == Rec_Yds)  return "rec yds";
    return "receptions";
}


string defenseName(StatType stat) {
    if (stat == Rush_Yds) return "run D";
    return "pass D";
}

int toPercent(double p) {
    return (int)(p * 100 + 0.5);
}


string formatOdds(double odds) {
    if (odds == 0.0) return "N/A";
    int rounded = (int)(odds > 0 ? odds + 0.5 : odds - 0.5);
    if (rounded > 0) return "+" + to_string(rounded);
    return to_string(rounded);
}

//describe
void printWhyLine(const string& label, const string& value) {
    cout << "  " << label;
    for (int i = (int)label.size(); i < 32; i++) cout << ' ';
    cout << value << endl;
}

// count how many games went over
void countHits(const vector<GameRow>& games, StatType stat, double line, bool over,
               int n, int& hits, int& used) {
    int first = (int)games.size() - n;
    if (first < 0) first = 0;
    hits = 0;
    used = 0;
    for (int i = first; i < (int)games.size(); i++) {
        int value = games[i].getStat(stat);
        if ((over && value > line) || (!over && value < line)) hits++;
        used++;
    }
}

//option 1: analyze prop
void analyzeProp(const HashTable& hashTable, Bplus& tree, const DefenseTable& defense,
                 const set<string>& teams) {
    string name = readLine("Player: ");
    string key = GameRow::normalize(name);

    //look player in both data structures
    const int REPEATS = 1000;
    const vector<GameRow>* games = nullptr;
    const vector<GameRow>* treeGames = nullptr;

    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < REPEATS; i++) games = hashTable.find(key);
    auto end = chrono::high_resolution_clock::now();
    double hashMicros = chrono::duration<double, micro>(end - start).count() / REPEATS;

    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < REPEATS; i++) treeGames = tree.search(key);
    end = chrono::high_resolution_clock::now();
    double treeMicros = chrono::duration<double, micro>(end - start).count() / REPEATS;

    // Both structures should agree on decision
    if ((games == nullptr) != (treeGames == nullptr)) {
        cout << "Warning: Hash Table and B+ Tree disagree on \"" << name << "\"." << endl;
    }

    if (games == nullptr) {
        cout << "No games found for \"" << name << "\"." << endl;

        vector<pair<string, const Bplus::Rows*>> matches = tree.range_search(key, key + "~");
        if (!matches.empty()) {
            cout << "Did you mean:" << endl;
            for (int i = 0; i < (int)matches.size() && i < 5; i++) {
                cout << "  " << matches[i].second->back().playerName << endl;
            }
        }
        return;
    }
    if (games->size() < 3) {
        cout << "Not enough games for " << games->back().playerName << " (need at least 3)." << endl;
        return;
    }

    StatType stat = (StatType)readInt("Stat (1 Pass Yds, 2 Rush Yds, 3 Rec Yds, 4 Receptions): ", 1, 4);
    bool over = readOverUnder("Over or Under (O/U): ");
    double line = readPositive("Line: ");
    string opponent = readOpponent("Opponent: ", teams);

    const GameRow& latest = games->back();


    double base = baseProb(*games, stat, line, over, LAST_N);
    double factor = 1.0;
    if (!opponent.empty()) factor = defense.getDefenseFactor(opponent, latest.position, stat);
    double finalProb = adjustedProb(*games, stat, line, over, factor, LAST_N);

    int hits = 0, used = 0;
    countHits(*games, stat, line, over, LAST_N, hits, used);

    //print result
    cout << endl;
    cout << latest.playerName << " " << (over ? "OVER " : "UNDER ") << line
         << " " << statName(stat) << endl;
    cout << toPercent(finalProb) << "% chance  ->  fair odds " << formatOdds(Odds(finalProb)) << endl;
    cout << endl;

    printWhyLine("Base (last " + to_string(used) + " games)", to_string(toPercent(base)) + "%");
    if (!opponent.empty()) {
        int change = toPercent(finalProb) - toPercent(base);
        string sign = (change >= 0) ? "+" : "";
        printWhyLine("vs " + opponent + " " + defenseName(stat), sign + to_string(change) + "%");
    }
    cout << "  " << (over ? "Over" : "Under") << " in " << hits << " of last " << used << " games" << endl;
    cout << endl;
    cout << fixed << setprecision(3);
    cout << "  Lookup time:  Hash Table " << hashMicros << " us  |  B+ Tree " << treeMicros << " us" << endl;
    cout.unsetf(ios::fixed);
    cout << setprecision(6);
}


int main() {
    vector<string> statFiles = {
        "../data/player_stats.csv",            // 1999-2024
        "../data/stats_player_week_2025.csv",
        "../data/stats_player_week_2026.csv"
    };
    DataLoader loader;
    vector<GameRow> rows = loader.loadPlayerStats(statFiles, "../data/games.csv");

    if (rows.empty()) {
        cerr << "No rows loaded. Check the data folder." << endl;
        return 1;
    }

    // Find the newest season
    int latestSeason = 0;
    for (const GameRow& row : rows) {
        if (row.season > latestSeason) latestSeason = row.season;
    }

    // Put every game into both structures and the defense table
    HashTable hashTable;
    Bplus tree;
    DefenseTable defense(latestSeason);
    set<string> teams;
    for (const GameRow& row : rows) {
        string key = GameRow::normalize(row.playerName);
        hashTable.insert(row);
        tree.insert(key, row);
        defense.addRow(row);
        if (row.season == latestSeason) teams.insert(row.team);
    }

    cout << "===== PropHits =====" << endl;
    cout << "Loaded " << rows.size() << " games for " << hashTable.playerCount() << " players" << endl;
    cout << "Matchups use " << latestSeason << " defenses" << endl;

    while (true) {
        cout << endl;
        cout << "1. Analyze a prop" << endl;
        cout << "2. Benchmark Hash Table vs B+ Tree" << endl;
        cout << "3. Quit" << endl;
        int choice = readInt("> ", 1, 3);

        if (choice == 1) {
            analyzeProp(hashTable, tree, defense, teams);
        } else if (choice == 2) {
            runBenchmarks(rows);
            cout << defaultfloat << setprecision(6) << right;   // undo the benchmark's number formatting
        } else {
            break;
        }
    }

    cout << "Goodbye!" << endl;
    return 0;
}