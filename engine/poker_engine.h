#pragma once
#include "../core/types.h"

class PokerEngine {
public:
    static HandEvaluation evaluate_5_card_hand(const std::vector<Card>& hand);
    static HandEvaluation evaluate_7_card_hand(const std::vector<Card>& hole, const std::vector<Card>& community);
    static bool compare_evals(const HandEvaluation& a, const HandEvaluation& b);
};
