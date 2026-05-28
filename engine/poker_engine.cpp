#include "poker_engine.h"
#include <algorithm>
#include <map>

HandEvaluation PokerEngine::evaluate_5_card_hand(const std::vector<Card>& hand) {
    std::vector<Card> sorted = hand;
    std::sort(sorted.begin(), sorted.end());

    bool is_flush = true;
    for (int i = 1; i < 5; ++i)
        if (sorted[i].suit != sorted[0].suit) is_flush = false;

    bool is_straight = true;
    for (int i = 1; i < 5; ++i) {
        if (sorted[i].rank != sorted[i - 1].rank + 1) {
            is_straight = false;
            break;
        }
    }
    // Wheel straight: A-2-3-4-5
    if (!is_straight && sorted[4].rank == ACE && sorted[0].rank == TWO &&
        sorted[1].rank == THREE && sorted[2].rank == FOUR && sorted[3].rank == FIVE) {
        is_straight = true;
        return { is_flush ? STRAIGHT_FLUSH : STRAIGHT, {FIVE, FOUR, THREE, TWO, ACE} };
    }

    std::map<Rank, int> counts;
    for (const auto& c : sorted) counts[c.rank]++;

    std::vector<std::pair<int, Rank>> freq;
    for (const auto& p : counts) freq.push_back({ p.second, p.first });
    std::sort(freq.begin(), freq.end(), std::greater<std::pair<int, Rank>>());

    std::vector<int> tie_breakers;
    for (const auto& f : freq) tie_breakers.push_back(f.second);

    if (is_flush && is_straight) return { STRAIGHT_FLUSH, {sorted[4].rank} };
    if (freq[0].first == 4)                                return { FOUR_OF_A_KIND,    tie_breakers };
    if (freq[0].first == 3 && freq[1].first == 2)          return { FULL_HOUSE,        tie_breakers };
    if (is_flush) {
        tie_breakers.clear();
        for (int i = 4; i >= 0; --i) tie_breakers.push_back(sorted[i].rank);
        return { FLUSH, tie_breakers };
    }
    if (is_straight)                                       return { STRAIGHT,          {sorted[4].rank} };
    if (freq[0].first == 3)                                return { THREE_OF_A_KIND,   tie_breakers };
    if (freq[0].first == 2 && freq[1].first == 2)          return { TWO_PAIR,          tie_breakers };
    if (freq[0].first == 2)                                return { PAIR,              tie_breakers };

    tie_breakers.clear();
    for (int i = 4; i >= 0; --i) tie_breakers.push_back(sorted[i].rank);
    return { HIGH_CARD, tie_breakers };
}

HandEvaluation PokerEngine::evaluate_7_card_hand(const std::vector<Card>& hole, const std::vector<Card>& community) {
    std::vector<Card> all_cards = hole;
    all_cards.insert(all_cards.end(), community.begin(), community.end());

    HandEvaluation best_eval = { HIGH_CARD, {0} };
    if (all_cards.size() < 5) return best_eval;

    bool initialized = false;
    std::vector<bool> v(all_cards.size(), false);
    std::fill(v.begin() + static_cast<int>(all_cards.size()) - 5, v.end(), true);

    do {
        std::vector<Card> combo;
        for (size_t i = 0; i < all_cards.size(); ++i)
            if (v[i]) combo.push_back(all_cards[i]);

        if (combo.size() == 5) {
            HandEvaluation current = evaluate_5_card_hand(combo);
            if (!initialized || compare_evals(current, best_eval)) {
                best_eval = current;
                initialized = true;
            }
        }
    } while (std::next_permutation(v.begin(), v.end()));

    return best_eval;
}

bool PokerEngine::compare_evals(const HandEvaluation& a, const HandEvaluation& b) {
    if (a.hand_rank != b.hand_rank) return a.hand_rank > b.hand_rank;
    return a.tie_breakers > b.tie_breakers;
}
