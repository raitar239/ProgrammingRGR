#include "menu.h"
#include "leaderboard.h"
#include "../core/utils.h"
#include "../core/logger.h"
#include "../engine/texas_holdem.h"
#include <iostream>
#include <algorithm>

void view_rules() {
    clear_screen();
    std::cout << "=================================================================\n";
    std::cout << "                   TEXAS HOLD'EM GAME RULES                      \n";
    std::cout << "=================================================================\n\n";
    std::cout << " 1. Every player receives 2 private hole cards.\n";
    std::cout << " 2. 5 community cards are dealt face-up over 3 distinct rounds:\n";
    std::cout << "    - The Flop (3 cards)\n";
    std::cout << "    - The Turn (1 card)\n";
    std::cout << "    - The River (1 card)\n";
    std::cout << " 3. There are betting windows matching each stage. You can:\n";
    std::cout << "    - Check (Pass turn if stakes match)\n";
    std::cout << "    - Call (Match current active standard bet)\n";
    std::cout << "    - Raise (Increase required entry pool values)\n";
    std::cout << "    - Fold (Drop out, forfeiting committed investments)\n";
    std::cout << " 4. Build the strongest 5-card combination from any mix of your\n";
    std::cout << "    2 private and 5 community cards.\n";
    std::cout << " 5. Highest ranking combo takes home the total round pot.\n\n";
    wait_for_user();
}

void run_main_menu() {
    std::string profile_name = "Player1";

    while (true) {
        clear_screen();
        std::cout << "=======================================\n";
        std::cout << "          TEXAS HOLD'EM POKER        \n";
        std::cout << "=======================================\n";
        std::cout << " 1. Start New Game\n";
        std::cout << " 2. View Rules & Mechanics\n";
        std::cout << " 3. View Global Leaderboard\n";
        std::cout << " 4. Quit Game System\n";
        std::cout << "=======================================\n";

        int choice = get_safe_int_input(1, 4, "Select Option: ");

        if (choice == 1) {
            std::cout << "\nEnter your profile name: ";
            while (true) {
                std::cin >> profile_name;
                if (profile_name.empty()) {
                    std::cout << "Name cannot be empty. Try again: ";
                } else if (profile_name.size() > 20) {
                    std::cout << "Name too long (max 20 chars). Try again: ";
                } else if (!std::all_of(profile_name.begin(), profile_name.end(), ::isalnum)) {
                    std::cout << "Only letters and digits allowed. Try again: ";
                } else {
                    break;
                }
            }
            LOG_INFO("New game started. Player: " + profile_name);

            TexasHoldem session(profile_name);
            while (true) {
                session.play_round();

                if (session.is_user_bankrupt()) {
                    LOG_WARNING("Player " + profile_name + " went bankrupt.");
                    std::cout << "\nGame Over! You have run out of chips.\n";
                    wait_for_user();
                    break;
                }

                clear_screen();
                std::cout << "Current Balance: $" << session.get_user_chips() << "\n";
                std::cout << "1. Deal next hand\n";
                std::cout << "2. Cash out and return to Main Menu\n";
                int next = get_safe_int_input(1, 2, "Action: ");
                if (next == 2) {
                    LOG_INFO("Player " + profile_name + " cashed out with $" + std::to_string(session.get_user_chips()));
                    break;
                }
            }
        } else if (choice == 2) {
            LOG_INFO("Rules viewed.");
            view_rules();
        } else if (choice == 3) {
            LOG_INFO("Leaderboard viewed.");
            display_leaderboard();
        } else {
            std::cout << "\nThank you for playing! Goodbye.\n";
            break;
        }
    }
}
