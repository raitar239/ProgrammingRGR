#include "utils.h"
#include <iostream>
#include <limits>

void clear_screen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void wait_for_user() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int get_safe_int_input(int min_val, int max_val, const std::string& prompt) {
    int input;
    while (true) {
        std::cout << prompt;
        if (std::cin >> input && input >= min_val && input <= max_val) return input;
        
        std::cout << "Invalid input. Please enter a number between " << min_val << " and " << max_val << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void print_card(const Card& card) {
    std::cout << "[" << RANK_LABELS[card.rank] << SUIT_SYMBOLS[card.suit] << "] ";
}
