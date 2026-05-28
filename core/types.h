#pragma once
#include <string>
#include <vector>

// ENUMS
enum Suit { CLUBS, DIAMONDS, HEARTS, SPADES };
enum Rank { TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };
enum HandRank { HIGH_CARD, PAIR, TWO_PAIR, THREE_OF_A_KIND, STRAIGHT, FLUSH, FULL_HOUSE, FOUR_OF_A_KIND, STRAIGHT_FLUSH };

// CONSTANTS
inline const std::vector<std::string> SUIT_SYMBOLS    = { "♧", "♢", "♡", "♤" };
inline const std::vector<std::string> RANK_LABELS     = { "", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
inline const std::vector<std::string> HAND_RANK_LABELS = { "High Card", "Pair", "Two Pair", "Three of a Kind", "Straight", "Flush", "Full House", "Four of a Kind", "Straight Flush" };

// STRUCTS
struct Card {
    Rank rank = TWO;
    Suit suit = CLUBS;
    bool operator<(const Card& other) const { return rank < other.rank; }
};

struct HandEvaluation {
    HandRank hand_rank = HIGH_CARD;
    std::vector<int> tie_breakers;
};

struct Player {
    std::string name;
    std::vector<Card> hole_cards;
    int chips = 2000;
    int current_bet = 0;
    bool is_folded = false;
    bool is_all_in = false;
    bool is_ai = false;
};
