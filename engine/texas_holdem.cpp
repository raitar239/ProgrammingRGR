#include "texas_holdem.h"
#include "poker_engine.h"
#include "ai_player.h"
#include "../core/utils.h"
#include "../core/logger.h"
#include "../core/config.h"
#include "../ui/leaderboard.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <random>

// CONSTRUCTOR
TexasHoldem::TexasHoldem(const std::string& user_name) {
    const Config& cfg = Config::instance();
    SMALL_BLIND = cfg.small_blind;
    LARGE_BLIND = cfg.large_blind;
    int chips   = cfg.starting_chips;


    players.push_back({ user_name,  {}, chips, 0, false, false, false });
    players.push_back({ "Bot1", {}, chips, 0, false, false, true });
    players.push_back({ "Bot2",  {}, chips, 0, false, false, true });
    players.push_back({ "Bot3", {}, chips, 0, false, false, true });
}

// PRIVATE HELPERS
void TexasHoldem::build_deck() {
    deck.clear();
    for (int s = CLUBS; s <= SPADES; ++s)
        for (int r = TWO; r <= ACE; ++r)
            deck.push_back({ static_cast<Rank>(r), static_cast<Suit>(s) });

    auto seed = static_cast<unsigned int>( std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));
}

void TexasHoldem::render_table(bool reveal_all) {
    clear_screen();
    std::cout << "=================================================================\n";
    std::cout << "         TEXAS HOLD'EM POKER  (Pot: $" << pot << ")\n";
    std::cout << "=================================================================\n\n";

    std::cout << "Community Cards: ";
    if (community_cards.empty()) std::cout << "[ No cards on table yet ]";
    for (const auto& card : community_cards) print_card(card);
    std::cout << "\n\n-----------------------------------------------------------------\n";

    for (const auto& p : players) {
        std::cout << (p.is_ai ? "[AI] " : "[YOU] ")
                  << std::left << std::setw(12) << p.name
                  << " Chips: $" << std::left << std::setw(6) << p.chips
                  << " Committed: $" << std::left << std::setw(5) << p.current_bet;

        if (p.chips <= 0 && p.current_bet == 0) {
            std::cout << " [ OUT / BANKRUPT ]";
        } else if (p.is_folded) {
            std::cout << " [ FOLDED ]";
        } else if (p.is_all_in) {
            std::cout << " [ ALL-IN ]";
        } else {
            std::cout << " Cards: ";
            if (p.hole_cards.size() >= 2) {
                if (!p.is_ai || reveal_all) {
                    print_card(p.hole_cards[0]);
                    print_card(p.hole_cards[1]);
                    if (!p.is_ai && community_cards.size() >= 3) {
                        auto ev = PokerEngine::evaluate_7_card_hand(p.hole_cards, community_cards);
                        std::cout << " (" << HAND_RANK_LABELS[ev.hand_rank] << ")";
                    }
                } else {
                    std::cout << "[*] [*]";
                }
            } else {
                std::cout << "[ No Cards ]";
            }
        }
        std::cout << "\n";
    }
    std::cout << "-----------------------------------------------------------------\n\n";
}

void TexasHoldem::execute_betting_round(int starting_player_offset) {
    int active_bettors = 0;
    for (const auto& p : players)
        if (!p.is_folded && !p.is_all_in && p.chips > 0) active_bettors++;
    if (active_bettors < 2) return;

    unsigned int current_actor = (dealer_idx + starting_player_offset) % players.size();
    size_t actions_this_round = 0;

    while (actions_this_round < players.size() || !betting_is_equalized()) {
        Player& p = players[current_actor];

        if (!p.is_folded && !p.is_all_in && p.chips > 0) {
            render_table();
            int call_amount = current_highest_bet - p.current_bet;

            if (p.is_ai) {
                AIDecision decision = AIPlayer::decide(
                    p, community_cards, call_amount, pot, LARGE_BLIND, current_stage);

                switch (decision.action) {
                    case AIAction::FOLD:
                        p.is_folded = true;
                        std::cout << p.name << " folds.\n";
                        break;

                    case AIAction::CHECK:
                        std::cout << p.name << " checks.\n";
                        break;

                    case AIAction::CALL: {
                        int actual = std::min(call_amount, p.chips);
                        p.chips       -= actual;
                        p.current_bet += actual;
                        pot           += actual;
                        if (p.chips == 0) p.is_all_in = true;
                        std::cout << p.name << " calls $" << actual << ".\n";
                        break;
                    }

                    case AIAction::RAISE: {
                        int min_total = p.current_bet + call_amount + LARGE_BLIND;
                        int max_total = p.current_bet + p.chips;
                        int target    = std::min(max_total, std::max(min_total, decision.raise_total));
                        int net       = target - p.current_bet;
                        p.chips      -= net;
                        p.current_bet = target;
                        pot          += net;
                        current_highest_bet = std::max(current_highest_bet, target);
                        if (p.chips == 0) p.is_all_in = true;
                        std::cout << p.name << " raises to $" << target << ".\n";
                        break;
                    }
                }

            } else {
                std::cout << ">> YOUR ACTION (Call/Check Amount: $" << call_amount << ")\n";
                std::cout << "1. Check/Call | 2. Raise | 3. Fold\n";
                int choice = get_safe_int_input(1, 3, "Select action: ");

                if (choice == 1) {
                    if (call_amount == 0) {
                        std::cout << "You Checked.\n";
                    } else {
                        int actual_call  = std::min(call_amount, p.chips);
                        p.chips         -= actual_call;
                        p.current_bet   += actual_call;
                        pot             += actual_call;
                        if (p.chips == 0) p.is_all_in = true;
                        std::cout << "You Called $" << actual_call << ".\n";
                    }
                } else if (choice == 2) {
                    int min_raise = call_amount + LARGE_BLIND;
                    if (p.chips <= min_raise) {
                        int actual_bet    = p.chips;
                        p.current_bet    += actual_bet;
                        pot              += actual_bet;
                        current_highest_bet = std::max(current_highest_bet, p.current_bet);
                        p.chips           = 0;
                        p.is_all_in       = true;
                        std::cout << "You raised ALL-IN to $" << p.current_bet << "!\n";
                    } else {
                        int min_total = p.current_bet + min_raise;
                        int max_total = p.current_bet + p.chips;
                        std::cout << "Enter total bet amount (Min: $" << min_total << ", Max: $" << max_total << "): ";
                        int target_total    = get_safe_int_input(min_total, max_total, "");
                        int net_addition    = target_total - p.current_bet;
                        p.chips            -= net_addition;
                        p.current_bet       = target_total;
                        pot                += net_addition;
                        current_highest_bet = target_total;
                        if (p.chips == 0) p.is_all_in = true;
                        std::cout << "You raised total bet to $" << target_total << "!\n";
                    }
                } else {
                    p.is_folded = true;
                    std::cout << "You Folded.\n";
                }
            }
        }
        actions_this_round++;
        current_actor = (current_actor + 1) % players.size();
    }

    for (auto& p : players) p.current_bet = 0;
    current_highest_bet = 0;
}

bool TexasHoldem::betting_is_equalized() {
    int targeted_bet = -1;
    for (const auto& p : players) {
        if (p.is_folded || p.is_all_in || p.chips <= 0) continue;
        if (targeted_bet == -1) targeted_bet = p.current_bet;
        else if (p.current_bet != targeted_bet) return false;
    }
    return true;
}

int TexasHoldem::count_active_players() {
    int count = 0;
    for (const auto& p : players)
        if (!p.is_folded && p.chips >= 0) count++;
    return count;
}

// PLAY ROUND
void TexasHoldem::play_round() {
    build_deck();
    community_cards.clear();
    pot = 0;

    int players_with_money = 0;
    for (auto& p : players) {
        p.hole_cards.clear();
        if (p.chips > 0) {
            p.is_folded  = false;
            p.is_all_in  = false;
            players_with_money++;
        } else {
            p.is_folded = true;
        }
        p.current_bet = 0;
    }

    if (players_with_money < 2) {
        LOG_ERROR("Not enough players with chips to start a round.");
        std::cout << "Not enough players with chips remaining.\n";
        return;
    }

    LOG_INFO("--- Round started. Players in: " + std::to_string(players_with_money) + " ---");

    // Blinds
    unsigned int sb_player = (dealer_idx + 1) % players.size();
    while (players[sb_player].is_folded)
        sb_player = (sb_player + 1) % players.size();

    unsigned int lb_player = (sb_player + 1) % players.size();
    while (players[lb_player].is_folded)
        lb_player = (lb_player + 1) % players.size();

    int sb_paid = std::min(SMALL_BLIND, players[sb_player].chips);
    players[sb_player].chips      -= sb_paid;
    players[sb_player].current_bet = sb_paid;
    pot += sb_paid;
    if (players[sb_player].chips == 0) players[sb_player].is_all_in = true;

    int lb_paid = std::min(LARGE_BLIND, players[lb_player].chips);
    players[lb_player].chips      -= lb_paid;
    players[lb_player].current_bet = lb_paid;
    pot += lb_paid;
    current_highest_bet = lb_paid;
    if (players[lb_player].chips == 0) players[lb_player].is_all_in = true;

    // Deal hole cards
    for (int i = 0; i < 2; ++i)
        for (auto& p : players)
            if (!p.is_folded && !deck.empty()) {
                p.hole_cards.push_back(deck.back());
                deck.pop_back();
            }

    // Preflop
    current_stage = 0;
    execute_betting_round((lb_player + 1) % players.size());

    // Flop
    if (count_active_players() > 1 && !deck.empty()) {
        deck.pop_back(); // burn
        for (int i = 0; i < 3; ++i)
            if (!deck.empty()) { community_cards.push_back(deck.back()); deck.pop_back(); }
        current_stage = 1;
        execute_betting_round(1);
    }

    // Turn
    if (count_active_players() > 1 && !deck.empty()) {
        deck.pop_back();
        community_cards.push_back(deck.back()); deck.pop_back();
        current_stage = 2;
        execute_betting_round(1);
    }

    // River
    if (count_active_players() > 1 && !deck.empty()) {
        deck.pop_back();
        community_cards.push_back(deck.back()); deck.pop_back();
        current_stage = 3;
        execute_betting_round(1);
    }

    // Showdown
    render_table(true);
    std::cout << "=================== SHOWDOWN ===================\n";

    int final_actives = 0;
    for (const auto& p : players) if (!p.is_folded) final_actives++;

    if (final_actives == 1) {
        for (auto& p : players) {
            if (!p.is_folded) {
                std::cout << p.name << " wins the pot of $" << pot << " (all others folded).\n";
                LOG_INFO(p.name + " wins $" + std::to_string(pot) + " (all folded).");                
                p.chips += pot;
                break;
            }
        }
    } else if (final_actives > 1) {
        std::vector<std::pair<unsigned int, HandEvaluation>> rankings;
        for (size_t i = 0; i < players.size(); ++i) {
            if (!players[i].is_folded && players[i].hole_cards.size() >= 2) {
                HandEvaluation ev = PokerEngine::evaluate_7_card_hand( players[i].hole_cards, community_cards);
                rankings.push_back({ static_cast<unsigned int>(i), ev });
                std::cout << players[i].name << " exhibits: " << HAND_RANK_LABELS[ev.hand_rank] << "\n";
                LOG_INFO(players[i].name + " shows: " + HAND_RANK_LABELS[ev.hand_rank]);
            }
        }

        if (!rankings.empty()) {
            std::sort(rankings.begin(), rankings.end(),
                [](const auto& a, const auto& b) {
                    return PokerEngine::compare_evals(a.second, b.second);
                });

            std::vector<unsigned int> winners = { rankings[0].first };
            for (size_t i = 1; i < rankings.size(); ++i) {
                if (!PokerEngine::compare_evals(rankings[0].second, rankings[i].second) &&
                    !PokerEngine::compare_evals(rankings[i].second, rankings[0].second)) {
                    winners.push_back(rankings[i].first);
                } else break;
            }

            int split_pot = pot / static_cast<int>(winners.size());
            for (auto idx : winners) {
                std::cout << players[idx].name << " wins with " << HAND_RANK_LABELS[rankings[0].second.hand_rank] << " ($" << split_pot << ")\n";
                LOG_INFO(players[idx].name + " wins $" + std::to_string(split_pot) + " with " + HAND_RANK_LABELS[rankings[0].second.hand_rank]);
                players[idx].chips += split_pot;
            }
        }
    }

    save_score(players[0].name, players[0].chips);
    dealer_idx = (dealer_idx + 1) % players.size();
    wait_for_user();
}

// ACCESSORS
bool TexasHoldem::is_user_bankrupt() const { return players[0].chips <= 0; }
int  TexasHoldem::get_user_chips() const { return players[0].chips; }
