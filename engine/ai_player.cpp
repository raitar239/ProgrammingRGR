#include "ai_player.h"
#include "poker_engine.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

// UTILS
float AIPlayer::randomf() {
    return static_cast<float>(std::rand()) / RAND_MAX;
}

float AIPlayer::pot_odds(int call_amount, int pot) {
    if (call_amount <= 0) return 1.0f;
    return static_cast<float>(pot) / (pot + call_amount);
}

// PREFLOP
float AIPlayer::preflop_strength(const std::vector<Card>& hole) {
    if (hole.size() < 2) return 0.0f;

    Rank r1 = hole[0].rank > hole[1].rank ? hole[0].rank : hole[1].rank;
    Rank r2 = hole[0].rank < hole[1].rank ? hole[0].rank : hole[1].rank;
    bool suited = (hole[0].suit == hole[1].suit);
    bool paired = (r1 == r2);
    int gap = r1 - r2;

    float score = 0.0f;

    // pair
    if (paired) {
        if (r1 >= JACK)  score = 0.90f;
        else if (r1 >= EIGHT) score = 0.72f;
        else if (r1 >= FIVE)  score = 0.55f;
        else score = 0.40f;
        return score;
    }

    // high card
    if (r1 == ACE) score = 0.55f;
    else if (r1 == KING) score = 0.45f;
    else if (r1 == QUEEN) score = 0.40f;
    else if (r1 == JACK) score = 0.35f;
    else if (r1 == TEN) score = 0.30f;
    else score = 0.20f;

    // second card (+ score)
    if (r2 >= TEN) score += 0.12f;
    else if (r2 >= SEVEN) score += 0.07f;
    else if (r2 >= FOUR) score += 0.03f;

    // suit
    if (suited) score += 0.08f;

    // strit gap
    if (gap == 1) score += 0.06f;
    else if (gap == 2) score += 0.03f;
    else if (gap >= 4) score -= 0.05f;

    return std::min(1.0f, std::max(0.0f, score));
}

// HAND STRENGTH
float AIPlayer::hand_strength(const std::vector<Card>& hole, const std::vector<Card>& community) {
    if (community.empty()) return preflop_strength(hole);
    HandEvaluation ev = PokerEngine::evaluate_7_card_hand(hole, community);

    // base on rang
    float base = 0.0f;
    switch (ev.hand_rank) {
        case HIGH_CARD:       base = 0.10f; break;
        case PAIR:            base = 0.30f; break;
        case TWO_PAIR:        base = 0.55f; break;
        case THREE_OF_A_KIND: base = 0.70f; break;
        case STRAIGHT:        base = 0.80f; break;
        case FLUSH:           base = 0.85f; break;
        case FULL_HOUSE:      base = 0.92f; break;
        case FOUR_OF_A_KIND:  base = 0.97f; break;
        case STRAIGHT_FLUSH:  base = 1.00f; break;
    }

    // high card
    float kicker_bonus = 0.0f;
    if (!ev.tie_breakers.empty()) {
        int top = ev.tie_breakers[0];
        kicker_bonus = (top - 2) / static_cast<float>(ACE - 2) * 0.08f;
    }

    return std::min(1.0f, base + kicker_bonus);
}

// DECISION MAKING
AIDecision AIPlayer::decide(
    const Player& bot,
    const std::vector<Card>& community_cards,
    int call_amount,
    int pot,
    int large_blind,
    int stage
)
{
    std::srand(static_cast<unsigned>(std::time(nullptr) ^ reinterpret_cast<size_t>(&bot)));

    float strength = hand_strength(bot.hole_cards, community_cards);
    float jitter = (randomf() - 0.5f) * 0.10f;
    float eff = std::min(1.0f, std::max(0.0f, strength + jitter));

    // Check
    if (call_amount == 0) {
        // raise with strong
        if (eff >= 0.70f && randomf() < 0.55f) {
            int raise_size = static_cast<int>(pot * (0.5f + randomf() * 0.5f));
            raise_size = std::max(large_blind * 2, raise_size);
            raise_size = std::min(bot.chips, raise_size);
            int total = bot.current_bet + raise_size;
            return { AIAction::RAISE, total };
        }
        // raise with middle (bluff)
        if (eff >= 0.45f && randomf() < 0.20f) {
            int raise_size = large_blind * 2;
            raise_size = std::min(bot.chips, raise_size);
            int total = bot.current_bet + raise_size;
            return { AIAction::RAISE, total };
        }
        return { AIAction::CHECK };
    }

    // Call
    // all-in
    bool calling_all_in = (call_amount >= bot.chips);
    if (calling_all_in) {
        if (eff >= 0.80f) return { AIAction::CALL };
        return { AIAction::FOLD };
    }
    float call_ratio = static_cast<float>(call_amount) / bot.chips;
    if (call_ratio > 0.4f && eff < 0.60f) return { AIAction::FOLD };

    // Pot odds
    float equity_needed = static_cast<float>(call_amount) / (pot + call_amount);
    if (eff < equity_needed - 0.05f) {
        if (randomf() < 0.12f) return { AIAction::CALL };
        return { AIAction::FOLD };
    }

    // raise with strong
    if (eff >= 0.75f && randomf() < 0.60f) {
        int raise_size = static_cast<int>(pot * (0.6f + randomf() * 0.6f));
        raise_size = std::max(call_amount + large_blind, raise_size);
        raise_size = std::min(bot.chips, raise_size);
        int total = bot.current_bet + raise_size;
        return { AIAction::RAISE, total };
    }

    // raise with middle (semi-bluff)
    if (eff >= 0.55f && stage <= 1 && randomf() < 0.25f) {
        int raise_size = call_amount + large_blind * 2;
        raise_size = std::min(bot.chips, raise_size);
        int total = bot.current_bet + raise_size;
        return { AIAction::RAISE, total };
    }

    // bluff raise
    if (eff < 0.35f && stage == 0 && randomf() < 0.08f) {
        int raise_size = call_amount + large_blind * 3;
        raise_size = std::min(bot.chips, raise_size);
        int total = bot.current_bet + raise_size;
        return { AIAction::RAISE, total };
    }

    // call
    return { AIAction::CALL };
}
