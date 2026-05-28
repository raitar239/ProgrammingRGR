#pragma once
#include "../core/types.h"
#include <vector>
#include <string>

enum class AIAction { CHECK, CALL, RAISE, FOLD };

struct AIDecision {
    AIAction action;
    int raise_total = 0;
};

class AIPlayer {
public:
    static AIDecision decide(
        const Player& bot,
        const std::vector<Card>& community_cards,
        int call_amount,
        int pot,
        int large_blind,
        int stage
    );

private:
    static float hand_strength(
        const std::vector<Card>& hole,
        const std::vector<Card>& community
    );
    static float preflop_strength(const std::vector<Card>& hole);
    static float pot_odds(int call_amount, int pot);
    static float randomf();
};
