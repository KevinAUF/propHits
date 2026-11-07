/*
 Created by Kevin Astudillo on 11/5/26.

 GameRow.h
 PropHits - Group 37
 Team Name: GatorBoys

 Shared definitions used by every part of the project:
   - StatType: which stat a prop is about
   - GameRow:  one player's stats in one game (one row of player_stats.csv)
   - GameRow::normalize(): cleans up a player name so lookups match no matter how it's typed
*/

#ifndef PROPHITS_GAMEROW_H
#define PROPHITS_GAMEROW_H

#include <string>

// Which stat a prop is about. The numbers match the menu in the wireframe.
enum StatType { Pass_Yds = 1, Rush_Yds = 2, Rec_Yds = 3, Receptions = 4 };

// One player's stats in one game.
struct GameRow {
    std::string playerName;
    std::string position;   // QB, RB, WR, TE
    std::string team;
    std::string opponent;
    int season = 0;
    int week = 0;
    bool isHome = false;    // not in player_stats.csv; filled in by matching games.csv

    int passYds = 0;
    int rushYds = 0;
    int recYds = 0;
    int receptions = 0;

    // Returns the value of the chosen stat for this game.
    int getStat(StatType stat) const {
        if (stat == Pass_Yds) return passYds;
        if (stat == Rush_Yds) return rushYds;
        if (stat == Rec_Yds)  return recYds;
        return receptions;
    }

    //makes the names lowercase and match the table
    static std::string normalize(const std::string& name) {
        int start = 0;
        int end = (int)name.size() - 1;
        while (start <= end && name[start] == ' ') start++;
        while (end >= start && name[end] == ' ') end--;

        std::string result;
        for (int i = start; i <= end; i++) {
            char c = name[i];
            if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
            result += c;
        }
        return result;
    }
};

#endif // PROPHITS_GAMEROW_H