#include "common/houses.hpp"
#include <gtest/gtest.h>

// ─────────────────────────────────────────────
// HouseCard
// ─────────────────────────────────────────────

TEST(HouseCardTest, EqualityOperator) {
    HouseCard a{"Eddard Stark", 3, 1, 0};
    HouseCard b{"Eddard Stark", 3, 1, 0};
    HouseCard c{"Robb Stark", 4, 0, 0};

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

// ─────────────────────────────────────────────
// HouseCardHand
// ─────────────────────────────────────────────

class HouseCardHandTest : public ::testing::Test {
  protected:
    HouseCard c1{"Eddard Stark", 3, 1, 0};
    HouseCard c2{"Robb Stark", 4, 0, 0};
    HouseCard c3{"Catelyn Stark", 0, 0, 0};
    HouseCard c4{"Theon Greyjoy", 1, 1, 0};
    HouseCard c5{"Roose Bolton", 2, 0, 0};
    HouseCard c6{"Grande Jon", 2, 0, 1};
    HouseCard c7{"Ser Rodrik", 1, 0, 1};

    HouseCardHand makeHand() {
        return HouseCardHand(std::array<HouseCard, 7>{c1, c2, c3, c4, c5, c6, c7});
    }
};

TEST_F(HouseCardHandTest, InitialState) {
    auto hand = makeHand();
    EXPECT_EQ(hand.available().size(), 7);
    EXPECT_EQ(hand.used().size(), 0);
}

TEST_F(HouseCardHandTest, DiscardMovesCardToUsed) {
    auto hand = makeHand();
    hand.discard(c1);
    EXPECT_EQ(hand.available().size(), 6);
    EXPECT_EQ(hand.used().size(), 1);
    EXPECT_EQ(hand.used().front(), c1);
}

TEST_F(HouseCardHandTest, DiscardUnavailableCardThrows) {
    auto      hand = makeHand();
    HouseCard unknown{"Inconnu", 0, 0, 0};
    EXPECT_THROW(hand.discard(unknown), std::out_of_range);
}

TEST_F(HouseCardHandTest, DiscardLastCardSwapsHands) {
    auto hand = makeHand();

    hand.discard(c1);
    hand.discard(c2);
    hand.discard(c3);
    hand.discard(c4);
    hand.discard(c5);
    hand.discard(c6);
    // c7 est la dernière carte disponible
    hand.discard(c7);

    // toutes les cartes reviennent dans available
    EXPECT_EQ(hand.available().size(), 6);
    EXPECT_EQ(hand.used().size(), 1);
}

TEST_F(HouseCardHandTest, DiscardLastCardIsInUsed) {
    auto hand = makeHand();

    hand.discard(c1);
    hand.discard(c2);
    hand.discard(c3);
    hand.discard(c4);
    hand.discard(c5);
    hand.discard(c6);
    hand.discard(c7);

    // c7 doit se trouver dans available après le swap
    const auto& avail = hand.available();
    EXPECT_EQ(std::ranges::find(avail, c7), avail.end());
    EXPECT_EQ(c7, hand.used().front());
}
