#pragma once
#include <string>
#include <vector>
#include "../core/types.h"

class TexasHoldem {
public:
    explicit TexasHoldem(const std::string& user_name);

    void play_round();
    bool is_user_bankrupt() const;
    int  get_user_chips()   const;

private:
    std::vector<Player> players;
    std::vector<Card>   deck;
    std::vector<Card>   community_cards;

    int          pot                 = 0;
    int          current_highest_bet = 0;
    int          current_stage       = 0;
    unsigned int dealer_idx          = 0;

    int SMALL_BLIND;
    int LARGE_BLIND;

    void build_deck();
    void render_table(bool reveal_all = false);
    void execute_betting_round(int starting_player_offset);
    bool betting_is_equalized();
    int  count_active_players();
};