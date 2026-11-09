/*
DataLoader.h
 PropHits - Group 37
 Team Name: GatorBoys
*/

#ifndef PROPHITS_DATALOADER_H
#define PROPHITS_DATALOADER_H

#include <string>
#include <unordered_map>
#include <vector>
#include "GameRow.h"

class DataLoader {
public:
    // Loads all stats into file
    std::vector<GameRow> loadPlayerStats(const std::vector<std::string>& statsPaths, const std::string& gamesPath = "");

private:
    void loadOneStatsFile(const std::string& statsPath, const std::unordered_map<std::string, bool>& homeLookup,std::vector<GameRow>& rows);
    std::vector<std::string> splitCSVLine(const std::string& line);
    int findColumn(const std::vector<std::string>& header, const std::string& name);
    int toInt(const std::string& field);
    std::string currentTeamCode(const std::string& team);
};

#endif // PROPHITS_DATALOADER_H