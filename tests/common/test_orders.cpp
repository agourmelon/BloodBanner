#include "common/orders.hpp"
#include <gtest/gtest.h>

// ─────────────────────────────────────────────
// Fixture
// ─────────────────────────────────────────────

HouseCardHand makeHand() {
    HouseCard c1{"Carte 1",  3, 1, 0};
    HouseCard c2{"Carte 2",    4, 0, 0};
    HouseCard c3{"Carte 3", 0, 0, 0};
    HouseCard c4{"Carte 4", 1, 1, 0};
    HouseCard c5{"Carte 5",  2, 0, 0};
    HouseCard c6{"Carte 6",    2, 0, 1};
    HouseCard c7{"Carte 7",    1, 0, 1};

    return HouseCardHand(std::array<HouseCard, 7>{c1, c2, c3, c4, c5, c6, c7});
}

class OrderTest : public ::testing::Test {
protected:
    static inline House stark{"Stark", makeHand()};
    static inline House lannister{"Lannister", makeHand()};
};

// ─────────────────────────────────────────────
// orderTypeName
// ─────────────────────────────────────────────

TEST_F(OrderTest, MarchOrderName) {
    Order o = MarchOrder{stark};
    EXPECT_EQ(orderTypeName(o), "Marche");
}

TEST_F(OrderTest, DefenseOrderName) {
    Order o = DefenseOrder{stark};
    EXPECT_EQ(orderTypeName(o), "Défense");
}

TEST_F(OrderTest, SupportOrderName) {
    Order o = SupportOrder{stark};
    EXPECT_EQ(orderTypeName(o), "Soutien");
}

TEST_F(OrderTest, MusterOrderName) {
    Order o = MusterOrder{stark};
    EXPECT_EQ(orderTypeName(o), "Recrutement");
}

TEST_F(OrderTest, RaidOrderName) {
    Order o = RaidOrder{stark};
    EXPECT_EQ(orderTypeName(o), "Raid");
}

// ─────────────────────────────────────────────
// isRaidable
// ─────────────────────────────────────────────

TEST_F(OrderTest, SupportIsRaidable) {
    Order o = SupportOrder{stark};
    EXPECT_TRUE(isRaidable(o));
}

TEST_F(OrderTest, MusterIsRaidable) {
    Order o = MusterOrder{stark};
    EXPECT_TRUE(isRaidable(o));
}

TEST_F(OrderTest, MarchIsNotRaidable) {
    Order o = MarchOrder{stark};
    EXPECT_FALSE(isRaidable(o));
}

TEST_F(OrderTest, DefenseIsNotRaidable) {
    Order o = DefenseOrder{stark};
    EXPECT_FALSE(isRaidable(o));
}

TEST_F(OrderTest, RaidIsNotRaidable) {
    Order o = RaidOrder{stark};
    EXPECT_FALSE(isRaidable(o));
}

// ─────────────────────────────────────────────
// orderOwner
// ─────────────────────────────────────────────

TEST_F(OrderTest, MarchOrderOwner) {
    Order o = MarchOrder{stark};
    EXPECT_EQ(&orderOwner(o), &stark);
}

TEST_F(OrderTest, RaidOrderOwner) {
    Order o = RaidOrder{lannister};
    EXPECT_EQ(&orderOwner(o), &lannister);
}
