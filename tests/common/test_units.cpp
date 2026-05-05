#include "common/houses.hpp"
#include "common/units.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>

// ─────────────────────────────────────────────
// Fixture
// ─────────────────────────────────────────────
class UnitTest : public ::testing::Test {
  protected:
    static inline House stark{"Stark", makeHand()};
    static inline House lannister{"Lannister", makeHand()};
};

// ─────────────────────────────────────────────
// unitName
// ─────────────────────────────────────────────

TEST_F(UnitTest, FootmanName) {
    Unit u = Footman{&stark};
    EXPECT_EQ(unitName(u), "Fantassin");
}

TEST_F(UnitTest, KnightName) {
    Unit u = Knight{&stark};
    EXPECT_EQ(unitName(u), "Chevalier");
}

// ─────────────────────────────────────────────
// recruitmentCost
// ─────────────────────────────────────────────

TEST_F(UnitTest, FootmanRecruitmentCost) {
    Unit u = Footman{&stark};
    EXPECT_EQ(recruitmentCost(u), 1);
}

TEST_F(UnitTest, KnightRecruitmentCost) {
    Unit u = Knight{&stark};
    EXPECT_EQ(recruitmentCost(u), 2);
}

// ─────────────────────────────────────────────
// combatStrength
// ─────────────────────────────────────────────

TEST_F(UnitTest, FootmanCombatStrength) {
    Unit u = Footman{&stark};
    EXPECT_EQ(combatStrength(u), 1);
}

TEST_F(UnitTest, KnightCombatStrength) {
    Unit u = Knight{&stark};
    EXPECT_EQ(combatStrength(u), 2);
}

TEST_F(UnitTest, RetreatingUnitHasZeroStrength) {
    Unit u = Footman{&stark, true};
    EXPECT_EQ(combatStrength(u), 0);
}

TEST_F(UnitTest, RetreatingKnightHasZeroStrength) {
    Unit u = Knight{&stark, true};
    EXPECT_EQ(combatStrength(u), 0);
}

// ─────────────────────────────────────────────
// unitOwner
// ─────────────────────────────────────────────

TEST_F(UnitTest, FootmanOwner) {
    Unit u = Footman{&stark};
    EXPECT_EQ(&unitOwner(u), &stark);
}

TEST_F(UnitTest, KnightOwner) {
    Unit u = Knight{&lannister};
    EXPECT_EQ(&unitOwner(u), &lannister);
}

// ─────────────────────────────────────────────
// isRetreating
// ─────────────────────────────────────────────

TEST_F(UnitTest, UnitNotRetreatingByDefault) {
    Unit u = Footman{&stark};
    EXPECT_FALSE(isRetreating(u));
}

TEST_F(UnitTest, UnitRetreating) {
    Unit u = Footman{&stark, true};
    EXPECT_TRUE(isRetreating(u));
}

// ─────────────────────────────────────────────
// operator==
// ─────────────────────────────────────────────

TEST_F(UnitTest, SameUnitAreEqual) {
    Unit a = Footman{&stark};
    Unit b = Footman{&stark};
    EXPECT_EQ(a, b);
}

TEST_F(UnitTest, DifferentOwnerAreNotEqual) {
    Unit a = Footman{&stark};
    Unit b = Footman{&lannister};
    EXPECT_NE(a, b);
}

TEST_F(UnitTest, DifferentTypeAreNotEqual) {
    Unit a = Footman{&stark};
    Unit b = Knight{&stark};
    EXPECT_NE(a, b);
}

TEST_F(UnitTest, DifferentRetreatingAreNotEqual) {
    Unit a = Footman{&stark, false};
    Unit b = Footman{&stark, true};
    EXPECT_NE(a, b);
}
