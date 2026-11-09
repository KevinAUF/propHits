//DefenseTable.h
// PropHits - Group 37
// Team Name: GatorBoys

// Hash table that maps a team -> what that team's Defense allows, by position and stat.
// Used by the model to adjust a player's base % for the matchup.

// How it's built: every GameRow is one player's game vs. row.opponent, so we credit that player's stats to the opponent's
// defense

// getDefenseFactor(team, position, stat) = (team allowed per game) / (the leagues average)
// In this program we will use 1.10 -> if the defense allows 10% more than average (good matchup for the player)
// and then 0.90 -> defense allows 10% less than average (this is a bad matchup)



#ifndef PROPHIT_DEFENSETABLE_H
#define PROPHIT_DEFENSETABLE_H

#include "GameRow.h"
#include <list>
#include <set>
#include <string>
#include <vector>
using namespace std;

struct DefenseStats {
    string team;

// allowed[position][stat] is the total that stat is allowed to the position.
    // position index: 0 = QB, 1 = RB, 2 = WR, 3 = TE
    // Stat index: StatType - 1 (0 = Pass_Yds, 1 = Rush_Yds, 2 = Rec_Yds, 3 = Receptions)
    long long allowed[4][4] = {};
    set<int> gamesPlayed; // Distinct games (season*100 + week), so 5 WRs in one game count as actually 1 game.

    double perGame(int pos, int stat) const {
        if (gamesPlayed.empty()) return 0.0;
        return (double)allowed[pos][stat] / gamesPlayed.size();
    }
};

class DefenseTable {
private:
    // 32 teams -> 67 buckets (a prime) keeps chains short, so no rehash is needed.
    static const int CAPACITY = 67;
    vector<list<DefenseStats>> table;
    int season;

    int hash(const string& key) const {
        long long h = 0;
        for (int i = 0; i < (int)key.size(); i++) {
            h = (h * 31 + (unsigned char)key[i]) % CAPACITY;
        }
        return (int)h;
    }
    // Returns -1 if the position isn't one of the positions listed.
    static int positionIndex(const string& pos) {
        if (pos == "QB") return 0;
        if (pos == "RB") return 1;
        if (pos == "WR") return 2;
        if (pos == "TE") return 3;
        return -1;
    }

    DefenseStats* findTeam(const string& team) {
        int index = hash(team);
        for (DefenseStats& d : table[index]) {
            if (d.team == team) return &d;
        }
        return nullptr;
    }

    // Read only version
    const DefenseStats* findTeam(const string& team) const {
        int index = hash(team);
        for (const DefenseStats& d : table[index]) {
            if (d.team == team) return &d;
        }
        return nullptr;
    }

    // This is for the averages of every team's per game number for the position and stat.
    double leagueAverage(int pos, int stat) const {
        double sum = 0.0;
        int teams = 0;
        for (int i = 0; i < CAPACITY; i++) {
            for (const DefenseStats& d : table[i]) {
                sum += d.perGame(pos, stat);
                teams++;
            }
        }
        return  (teams == 0) ? 0.0 : sum / teams;
    }

public:
    // Pick the season you want matchups for.
    // Defenses change every year, so mixing prior seasons is meaningless.

    explicit DefenseTable(int seasonToUse) : table(CAPACITY), season(seasonToUse) {}

    // Credit one player's game to the defense he played against
    void addRow(const GameRow& row) {
        if (row.season != season) return;
        int pos = positionIndex(row.position);
        if (pos == -1 || row.opponent.empty()) return;

        DefenseStats* d = findTeam(row.opponent);
        if (d == nullptr) {
            DefenseStats newTeam;
            newTeam.team = row.opponent;

            table[hash(row.opponent)].push_back(newTeam);
            d = &table[hash(row.opponent)].back();
        }
        for (int s = 0; s < 4; s++) {
            d->allowed[pos][s] += row.getStat((StatType)(s + 1));
        }
        d->gamesPlayed.insert(row.season * 100 + row.week);
    }
    double getDefenseFactor(const string& team, const string& position, StatType stat) const {
        int pos = positionIndex(position);
        if (pos == -1) return 1.0;

        const DefenseStats* d = findTeam(team);
        if (d == nullptr) return 1.0;

        int s = (int)stat - 1;
        double avg = leagueAverage(pos, s);
        if (avg <= 0.0) return 1.0;

        return d->perGame(pos, s) / avg;
    }

// This functions is useful for printing "DAL allows 182 rec yds/game to WRs"
    double allowedPerGame(const string& team, const string& position, StatType stat) const {
        int pos = positionIndex(position);
        const DefenseStats* d = findTeam(team);
        if (pos == -1 || d == nullptr) return 0.0;
        return d->perGame(pos, (int)stat - 1);
    }

    int teamCount() const {
        int count = 0;
        for (int i = 0; i < CAPACITY; i++) count += (int)table[i].size();
        return count;
    }

};

#endif //PROPHIT_DEFENSETABLE_H
