#include <iostream>
#include <fstream>
#include <string>
#include "GameRow.h"

int main() {
    std::ifstream file("../data/player_stats.csv");

    if (!file.is_open()) {
        std::cerr << "Could not open ../data/player_stats.csv" << std::endl;
        return 1;
    }

    std::string line;

    std::getline(file, line);
    std::cout << line << std::endl;

    int count = 0;
    while (std::getline(file, line)) {
        count++;
    }

    std::cout << "Rows: " << count << std::endl;
    return 0;
}