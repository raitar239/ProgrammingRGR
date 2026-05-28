#include <gtest/gtest.h>
#include "../engine/poker_engine.h"

// vector of cards
static Card C(Rank r, Suit s) { return {r, s}; }

// Combinations tests
TEST(HandEval, HighCard) {
    std::vector<Card> hand = {
        C(TWO,   CLUBS),
        C(FIVE,  DIAMONDS),
        C(SEVEN, HEARTS),
        C(NINE,  SPADES),
        C(JACK,  CLUBS)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, HIGH_CARD);
}

TEST(HandEval, OnePair) {
    std::vector<Card> hand = {
        C(ACE,  CLUBS),
        C(ACE,  DIAMONDS),
        C(THREE, HEARTS),
        C(SEVEN, SPADES),
        C(KING,  CLUBS)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, PAIR);
}

TEST(HandEval, TwoPair) {
    std::vector<Card> hand = {
        C(KING, CLUBS),
        C(KING, DIAMONDS),
        C(JACK, HEARTS),
        C(JACK, SPADES),
        C(TWO,  CLUBS)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, TWO_PAIR);
}

TEST(HandEval, ThreeOfAKind) {
    std::vector<Card> hand = {
        C(EIGHT, CLUBS),
        C(EIGHT, DIAMONDS),
        C(EIGHT, HEARTS),
        C(TWO,   SPADES),
        C(FIVE,  CLUBS)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, THREE_OF_A_KIND);
}

TEST(HandEval, Straight) {
    std::vector<Card> hand = {
        C(FIVE,  CLUBS),
        C(SIX,   DIAMONDS),
        C(SEVEN, HEARTS),
        C(EIGHT, SPADES),
        C(NINE,  CLUBS)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, STRAIGHT);
}

TEST(HandEval, WheelStraight) {
    std::vector<Card> hand = {
        C(ACE,  CLUBS),
        C(TWO,  DIAMONDS),
        C(THREE,HEARTS),
        C(FOUR, SPADES),
        C(FIVE, CLUBS)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, STRAIGHT);
}

TEST(HandEval, Flush) {
    std::vector<Card> hand = {
        C(TWO,   HEARTS),
        C(FIVE,  HEARTS),
        C(SEVEN, HEARTS),
        C(JACK,  HEARTS),
        C(ACE,   HEARTS)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, FLUSH);
}

TEST(HandEval, FullHouse) {
    std::vector<Card> hand = {
        C(TEN, CLUBS),
        C(TEN, DIAMONDS),
        C(TEN, HEARTS),
        C(SIX, SPADES),
        C(SIX, CLUBS)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, FULL_HOUSE);
}

TEST(HandEval, FourOfAKind) {
    std::vector<Card> hand = {
        C(QUEEN, CLUBS),
        C(QUEEN, DIAMONDS),
        C(QUEEN, HEARTS),
        C(QUEEN, SPADES),
        C(ACE,   CLUBS)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, FOUR_OF_A_KIND);
}

TEST(HandEval, StraightFlush) {
    std::vector<Card> hand = {
        C(FIVE,  SPADES),
        C(SIX,   SPADES),
        C(SEVEN, SPADES),
        C(EIGHT, SPADES),
        C(NINE,  SPADES)
    };
    auto ev = PokerEngine::evaluate_5_card_hand(hand);
    EXPECT_EQ(ev.hand_rank, STRAIGHT_FLUSH);
}

// Compare evals
TEST(CompareEvals, FlushBeatsStr8) {
    std::vector<Card> flush_hand = { C(TWO, HEARTS), C(FIVE, HEARTS), C(SEVEN, HEARTS), C(JACK, HEARTS), C(ACE, HEARTS) };
    std::vector<Card> str8_hand = { C(FIVE, CLUBS), C(SIX, DIAMONDS), C(SEVEN, HEARTS), C(EIGHT, SPADES), C(NINE, CLUBS) };
    auto flush = PokerEngine::evaluate_5_card_hand(flush_hand);
    auto str8 = PokerEngine::evaluate_5_card_hand(str8_hand);
    EXPECT_TRUE(PokerEngine::compare_evals(flush, str8));
    EXPECT_FALSE(PokerEngine::compare_evals(str8, flush));
}

TEST(CompareEvals, HigherPairWins) {
    std::vector<Card> aces = { C(ACE, CLUBS), C(ACE, DIAMONDS), C(TWO, HEARTS), C(THREE, SPADES), C(FOUR, CLUBS) };
    std::vector<Card> twos = { C(TWO, CLUBS), C(TWO, DIAMONDS), C(ACE, HEARTS), C(THREE, SPADES), C(FOUR, CLUBS) };
    auto pair_aces = PokerEngine::evaluate_5_card_hand(aces);
    auto pair_twos = PokerEngine::evaluate_5_card_hand(twos);
    EXPECT_TRUE(PokerEngine::compare_evals(pair_aces, pair_twos));
}

TEST(CompareEvals, EqualHandsNotGreater) {
    std::vector<Card> h1 = { C(ACE, CLUBS), C(ACE, DIAMONDS), C(KING, HEARTS), C(QUEEN, SPADES), C(JACK, CLUBS) };
    std::vector<Card> h2 = { C(ACE, HEARTS), C(ACE, SPADES), C(KING, CLUBS), C(QUEEN, DIAMONDS), C(JACK, HEARTS) };
    auto e1 = PokerEngine::evaluate_5_card_hand(h1);
    auto e2 = PokerEngine::evaluate_5_card_hand(h2);
    EXPECT_FALSE(PokerEngine::compare_evals(e1, e2));
    EXPECT_FALSE(PokerEngine::compare_evals(e2, e1));
}

// Best of 7 cards
TEST(SevenCard, FindsBestHand) {
    std::vector<Card> hole = { C(ACE, SPADES), C(KING, SPADES) };
    std::vector<Card> community = { C(QUEEN, SPADES), C(JACK, SPADES), C(TEN, SPADES), C(TWO, DIAMONDS), C(THREE, CLUBS)};
    auto ev = PokerEngine::evaluate_7_card_hand(hole, community);
    EXPECT_EQ(ev.hand_rank, STRAIGHT_FLUSH);
}

TEST(SevenCard, PairFromHole) {
    std::vector<Card> hole = { C(ACE, SPADES), C(ACE, DIAMONDS) };
    std::vector<Card> community = { C(TWO, CLUBS), C(THREE, HEARTS), C(FOUR, CLUBS), C(FIVE, DIAMONDS), C(SEVEN, HEARTS) };
    auto ev = PokerEngine::evaluate_7_card_hand(hole, community);
    EXPECT_GE(ev.hand_rank, PAIR);
}
