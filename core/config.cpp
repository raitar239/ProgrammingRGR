#include "config.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

Config& Config::instance() {
    static Config inst;
    return inst;
}

bool Config::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_WARNING("Config file not found: " + path + ". Using defaults.");
        return false;
    }

    std::string line;
    int line_num = 0;

    while (std::getline(file, line)) {
        ++line_num;

        auto trim = [](std::string s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                [](unsigned char c){ return !std::isspace(c); }));
            s.erase(std::find_if(s.rbegin(), s.rend(),
                [](unsigned char c){ return !std::isspace(c); }).base(), s.end());
            return s;
        };

        line = trim(line);

        if (line.empty() || line[0] == '#') continue;

        auto eq = line.find('=');
        if (eq == std::string::npos) {
            LOG_WARNING("Config line " + std::to_string(line_num) + " skipped (no '='): " + line);
            continue;
        }

        std::string key   = trim(line.substr(0, eq));
        std::string value = trim(line.substr(eq + 1));

        if (key.empty() || value.empty()) {
            LOG_WARNING("Config line " + std::to_string(line_num) + " skipped (empty key or value): " + line);
            continue;
        }

        try {
            if (key == "STARTING_CHIPS") {
                int v = std::stoi(value);
                if (v < 100 || v > 1000000) throw std::out_of_range("must be 100..1000000");
                starting_chips = v;

            } else if (key == "SMALL_BLIND") {
                int v = std::stoi(value);
                if (v < 1 || v > 10000) throw std::out_of_range("must be 1..10000");
                small_blind = v;

            } else if (key == "LARGE_BLIND") {
                int v = std::stoi(value);
                if (v < 2 || v > 20000) throw std::out_of_range("must be 2..20000");
                large_blind = v;

            } else if (key == "LOG_FILE") {
                log_file = value;

            } else {
                LOG_WARNING("Config: unknown key '" + key + "' on line " + std::to_string(line_num));
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Config line " + std::to_string(line_num) + " invalid value for " + key + ": " + e.what() + ". Using default.");
        }
    }

    if (large_blind <= small_blind) {
        LOG_ERROR("Config: LARGE_BLIND must be > SMALL_BLIND. Resetting to defaults.");
        small_blind = 10;
        large_blind = 20;
    }

    LOG_INFO("Config loaded from " + path +
             " | chips=" + std::to_string(starting_chips) +
             " sb=" + std::to_string(small_blind) +
             " bb=" + std::to_string(large_blind) +
             " log=" + log_file);
    return true;
}
