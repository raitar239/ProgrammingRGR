#include "leaderboard.h"
#include "../core/utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>

void save_score(const std::string& player_name, int chips) {
    std::map<std::string, int> leaderboard;

    std::ifstream infile("leaderboard.txt");
    if (infile.is_open()) {
        std::string name;
        int score;
        while (infile >> name >> score)
            leaderboard[name] = std::max(leaderboard[name], score);
        infile.close();
    }

    leaderboard[player_name] = std::max(leaderboard[player_name], chips);

    std::ofstream outfile("leaderboard.txt");
    if (outfile.is_open())
        for (const auto& pair : leaderboard)
            outfile << pair.first << " " << pair.second << "\n";
}

void display_leaderboard() {
    clear_screen();
    std::cout << "=======================================\n";
    std::cout << "             LEADERBOARD               \n";
    std::cout << "=======================================\n";

    std::ifstream infile("leaderboard.txt");
    std::vector<std::pair<std::string, int>> scores;

    if (infile.is_open()) {
        std::string name;
        int score;
        while (infile >> name >> score)
            scores.push_back({ name, score });
        infile.close();
    }

    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    if (scores.empty()) {
        std::cout << "No records found yet. Be the first to win!\n";
    } else {
        std::cout << std::left << std::setw(20) << "Player Name" << "Max Chips\n";
        std::cout << "---------------------------------------\n";
        for (size_t i = 0; i < std::min(scores.size(), size_t(10)); ++i)
            std::cout << std::left << std::setw(20) << scores[i].first << scores[i].second << "\n";
    }

    wait_for_user();
}
