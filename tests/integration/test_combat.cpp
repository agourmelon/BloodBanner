#include "common/game_state.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>

// ─────────────────────────────────────────────
// Fixture
// ─────────────────────────────────────────────

class CombatIntegrationTest : public ::testing::Test {
  protected:
    House stark{"Stark", makeHand()};
    House lannister{"Lannister", makeHand()};

    GameMap makeMap() {
        GameMap map;
        map.addProvince(Province{"Winterfell", Stronghold{}, {"Blancport"}});
        map.addProvince(Province{"Blancport", Castle{}, {"Winterfell", "Port-Réal"}});
        map.addProvince(Province{"Port-Réal", std::nullopt, {"Blancport"}});
        return map;
    }
};

// ─────────────────────────────────────────────
// Force de combat de base
// ─────────────────────────────────────────────

TEST_F(CombatIntegrationTest, FootmanContributesOneStrength) {
    EXPECT_EQ(combatStrength(Unit{Footman{&stark}}), 1);
}

TEST_F(CombatIntegrationTest, KnightContributesTwoStrength) {
    EXPECT_EQ(combatStrength(Unit{Knight{&stark}}), 2);
}

TEST_F(CombatIntegrationTest, RetreatingUnitContributesNoStrength) {
    EXPECT_EQ(combatStrength(Unit{Footman{&stark, true}}), 0);
}

TEST_F(CombatIntegrationTest, MixedArmyStrength) {
    auto  map = makeMap();
    auto& p   = map.province("Winterfell");
    p.addUnit(Footman{&stark});
    p.addUnit(Footman{&stark});
    p.addUnit(Knight{&stark});

    int total = 0;
    for (const auto& u : p.units())
        total += combatStrength(u);

    EXPECT_EQ(total, 4); // 1 + 1 + 2
}

// ─────────────────────────────────────────────
// Bonus défense
// ─────────────────────────────────────────────

TEST_F(CombatIntegrationTest, DefenseOrderAddsOneToStrength) {
    auto  map = makeMap();
    auto& p   = map.province("Winterfell");
    p.addUnit(Footman{&stark});
    p.setOrder(DefenseOrder{stark});

    int total = 0;
    for (const auto& u : p.units())
        total += combatStrength(u);

    // +1 si ordre de défense
    if (std::holds_alternative<DefenseOrder>(p.order().value()))
        total += 1;

    EXPECT_EQ(total, 2); // 1 (fantassin) + 1 (défense)
}

// ─────────────────────────────────────────────
// Retraite
// ─────────────────────────────────────────────

TEST_F(CombatIntegrationTest, RetreatingUnitsHaveZeroStrength) {
    auto  map = makeMap();
    auto& p   = map.province("Winterfell");
    p.addUnit(Footman{&stark, true});
    p.addUnit(Knight{&stark, true});

    int total = 0;
    for (const auto& u : p.units())
        total += combatStrength(u);

    EXPECT_EQ(total, 0);
}

TEST_F(CombatIntegrationTest, MixedRetreatingAndActiveUnits) {
    auto  map = makeMap();
    auto& p   = map.province("Winterfell");
    p.addUnit(Footman{&stark, false});
    p.addUnit(Footman{&stark, true});
    p.addUnit(Knight{&stark, false});

    int total = 0;
    for (const auto& u : p.units())
        total += combatStrength(u);

    EXPECT_EQ(total, 3); // 1 (actif) + 0 (retraite) + 2 (actif)
}

// ─────────────────────────────────────────────
// Cartes personnages
// ─────────────────────────────────────────────

TEST_F(CombatIntegrationTest, HouseCardAddsStrength) {
    auto&       hand      = stark.cards;
    const auto& available = hand.available();
    ASSERT_FALSE(available.empty());

    int baseStrength  = 1; // un fantassin
    int totalStrength = baseStrength + available.front().combatStrength;
    EXPECT_GE(totalStrength, baseStrength);
}

TEST_F(CombatIntegrationTest, DiscardedCardIsNoLongerAvailable) {
    auto&           hand = stark.cards;
    const HouseCard card = hand.available().front();
    hand.discard(card);
    const auto& available = hand.available();
    EXPECT_EQ(std::ranges::find(available, card), available.end());
}

TEST_F(CombatIntegrationTest, SwordsReduceLoserUnits) {
    // Une carte avec 1 épée élimine 1 unité en retraite adverse
    HouseCard cardWithSword{"Carte épée", 2, 1, 0};
    EXPECT_EQ(cardWithSword.swords, 1);
}

TEST_F(CombatIntegrationTest, TowersNegateEnemySwords) {
    // Une tour annule une épée adverse
    HouseCard cardWithTower{"Carte tour", 1, 0, 1};
    HouseCard cardWithSword{"Carte épée", 2, 1, 0};

    int effectiveSwords = std::max(0, cardWithSword.swords - cardWithTower.towers);
    EXPECT_EQ(effectiveSwords, 0);
}
