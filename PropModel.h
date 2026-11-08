// PropModel.h
// PropHits - Group 37
// Team Name: GatorBoys

// This file will turn a players past games into a hit % and fair american odds.

#ifndef PROPHIT_PROPMODEL_H
#define PROPHIT_PROPMODEL_H

#include "GameRow.h"
#include "DefenseTable.h"
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

// Probability -> American's odds
// p >= 0.5: -100 * p / (1-p)
// p < 0.5: +100 * (1-p) / p
inline double Odds(double p) {
    if (p <= 0.0 || p >= 1.0) return 0.0;
    if (p >= 0.5) return -100.0 * p / (1.0-p);
    return 100 * (1.0 - p) / p;
}

// Standard normal CDF (cumulative distribution function)
inline double normalCdf(double z) {
    return 0.5 * erfc(-z / sqrt(2.0));
}

// Mean and standard deviation of the last N games.
// Returns false if there aren't enough games.
inline bool meanAndSd(const vector<GameRow>& games, StatType stat, int lastN,
                      double& mean, double& sd) {
    if (games.size() < 3) return false;

    int start = max(0, (int)games.size() - lastN);
    int n = (int)games.size() - start;

    double sum = 0.0;
    for (int i = start; i < (int)games.size(); i++) {
        sum += games[i].getStat(stat);
    }
    mean = sum / n;

    double sq = 0.0;
    for (int i = start; i < (int)games.size(); i++) {
        double diff = games[i].getStat(stat) - mean;
        sq += diff * diff;
    }
    sd = sqrt(sq / (n - 1));
    if (sd == 0.0) sd = 1.0;
    return true;
}

inline double baseProb(const vector<GameRow>& games, StatType stat,
                       double line, bool over, int lastN = 17) {
    double mean, sd;
    if (!meanAndSd(games, stat, lastN, mean, sd)) return -1;

    double z = (line - mean) / sd;
    return over ? 1.0 - normalCdf(z) : normalCdf(z);
}

inline double adjustedProb(const vector<GameRow>& games, StatType stat,
                           double line, bool over, double defenseFactor,
                           int lastN = 17) {
    double mean, sd;
    if (!meanAndSd(games, stat, lastN, mean, sd)) return -1;

    mean = mean * defenseFactor;

    double z = (line - mean) / sd;
    return over ? 1.0 - normalCdf(z) : normalCdf(z);
}

#endif //PROPHIT_PROPMODEL_H
