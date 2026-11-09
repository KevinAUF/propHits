/*
 DataLoader.cpp
 PropHits - Group 37
 Team Name: GatorBoys
*/

#include "DataLoader.h"

#include <fstream>
#include <iostream>

using namespace std;

vector<GameRow> DataLoader::loadPlayerStats(const vector<string>& statsPaths, const string& gamesPath) {
    vector<GameRow> rows;

    //build table to see if game is at home
    unordered_map<string, bool> homeLookup;
    if (!gamesPath.empty()) {
        ifstream gamesFile(gamesPath);
        if (!gamesFile.is_open()) {
            cerr << "Warning: could not open " << gamesPath << endl;
        } else {
            string line;
            getline(gamesFile, line);
            vector<string> header = splitCSVLine(line);
            int seasonCol   = findColumn(header, "season");
            int weekCol     = findColumn(header, "week");
            int homeCol     = findColumn(header, "home_team");
            int awayCol     = findColumn(header, "away_team");
            int locationCol = findColumn(header, "location");

            while (getline(gamesFile, line)) {
                vector<string> f = splitCSVLine(line);
                if ((int)f.size() < (int)header.size()) continue;

                string prefix = f[seasonCol] + "_" + f[weekCol] + "_";
                bool neutral = (f[locationCol] == "Neutral");   // playing at a neutral location
                homeLookup[prefix + currentTeamCode(f[homeCol])] = !neutral;
                homeLookup[prefix + currentTeamCode(f[awayCol])] = false;
            }
        }
    }

    // Loading stat file into one
    for (const string& path : statsPaths) {
        loadOneStatsFile(path, homeLookup, rows);
    }

    return rows;
}

void DataLoader::loadOneStatsFile(const string& statsPath, const unordered_map<string, bool>& homeLookup, vector<GameRow>& rows) {
    ifstream file(statsPath);
    if (!file.is_open()) {
        cerr << "Error: could not open " << statsPath << endl;
        return;
    }

    string line;
    getline(file, line);
    vector<string> header = splitCSVLine(line);

    // Find each column by name
    int nameCol    = findColumn(header, "player_display_name");
    int posCol     = findColumn(header, "position");
    int teamCol    = findColumn(header, "recent_team");      // 1999-2024 file
    if (teamCol < 0) teamCol = findColumn(header, "team");   // 2025+ files
    int oppCol     = findColumn(header, "opponent_team");
    int seasonCol  = findColumn(header, "season");
    int weekCol    = findColumn(header, "week");
    int passYdsCol = findColumn(header, "passing_yards");
    int rushYdsCol = findColumn(header, "rushing_yards");
    int recYdsCol  = findColumn(header, "receiving_yards");
    int recCol     = findColumn(header, "receptions");

    if (nameCol < 0 || posCol < 0 || teamCol < 0 || oppCol < 0 || seasonCol < 0 || weekCol < 0 || passYdsCol < 0 || rushYdsCol < 0 || recYdsCol < 0 || recCol < 0) {
        cerr << "Error: " << statsPath << " is missing a column" << endl;
        return;
    }

    while (getline(file, line)) {
        vector<string> f = splitCSVLine(line);
        if ((int)f.size() < (int)header.size()) continue;

        // Only keep the position that we are making props for
        string pos = f[posCol];
        if (pos != "QB" && pos != "RB" && pos != "WR" && pos != "TE") continue;
        GameRow row;
        row.playerName = f[nameCol];
        row.position   = pos;
        row.team       = f[teamCol];
        row.opponent   = f[oppCol];
        row.season     = toInt(f[seasonCol]);
        row.week       = toInt(f[weekCol]);
        row.passYds    = toInt(f[passYdsCol]);
        row.rushYds    = toInt(f[rushYdsCol]);
        row.recYds     = toInt(f[recYdsCol]);
        row.receptions = toInt(f[recCol]);
        auto it = homeLookup.find(f[seasonCol] + "_" + f[weekCol] + "_" + row.team);
        if (it != homeLookup.end()) row.isHome = it->second;
        rows.push_back(row);
    }
}

vector<string> DataLoader::splitCSVLine(const string& line) {
    vector<string> fields;
    string current;
    bool inQuotes = false;
    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(current);
            current.clear();
        } else if (c != '\r') {
            current += c;
        }
    }
    fields.push_back(current);
    return fields;
}
int DataLoader::findColumn(const vector<string>& header, const string& name) {
    for (int i = 0; i < (int)header.size(); i++) {
        if (header[i] == name) return i;
    }
    return -1;
}

int DataLoader::toInt(const string& field) {
    if (field.empty() || field == "NA") return 0;
    return stoi(field);
}
//change to current team name
string DataLoader::currentTeamCode(const string& team) {
    if (team == "STL") return "LA";
    if (team == "OAK") return "LV";
    if (team == "SD")  return "LAC";
    return team;
}