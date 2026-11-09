#include <iostream>
#include <string>
#include <vector>
#include "GameRow.h"
#include "DataLoader.h"

using namespace std;

int main() {
    vector<string> statFiles = {
        "../data/player_stats.csv",            // 1999-2024
        "../data/stats_player_week_2025.csv",
        "../data/stats_player_week_2026.csv"
    };
    DataLoader loader;
    vector<GameRow> rows = loader.loadPlayerStats(statFiles, "../data/games.csv");

    if (rows.empty()) {
        cerr << "No rows loaded." << endl;
        return 1;
    }
    cout << "Loaded " << rows.size() << " rows" << endl;

    // Test: find Justin Jefferson's games
    string target = GameRow::normalize("justin jefferson");
    vector<GameRow> games;
    for (const GameRow& row : rows) {
        if (GameRow::normalize(row.playerName) == target) games.push_back(row);
    }

    // Print his last 5 games
    cout << "\nJustin Jefferson: " << games.size() << " games found. Last 5:" << endl;
    int first = (int)games.size() - 5;
    if (first < 0) first = 0;
    for (int i = first; i < (int)games.size(); i++) {
        const GameRow& g = games[i];
        cout << g.season << " wk " << g.week << "  "
             << (g.isHome ? "vs " : "@  ") << g.opponent
             << "  rec: " << g.receptions
             << "  rec yds: " << g.recYds << endl;
    }
    return 0;
}