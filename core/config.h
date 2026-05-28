#pragma once
#include <string>

struct Config {
    int         starting_chips = 2000;
    int         small_blind    = 10;
    int         large_blind    = 20;
    std::string log_file       = "poker.log";

    static Config& instance();

    bool load(const std::string& path);
};
