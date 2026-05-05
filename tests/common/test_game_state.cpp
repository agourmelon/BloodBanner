#include "common/game_state.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>

// ─────────────────────────────────────────────
// Fixture
// ─────────────────────────────────────────────
class GameStateTest : public ::testing::Test {
  protected:
    House stark{"Stark", makeHand()};
    House lannister{"Lannister", makeHand()};

    GameMap makeSimpleMap() {
        GameMap map;
        map.addProvince(Province{"Winterfell", std::nullopt, {"Borderive", "Blancport"}});
        map.addProvince(Province{"Borderive", std::nullopt, {"Winterfell", "Blancport"}});
        map.addProvince(Province{"Blancport", Castle{}, {"Winterfell", "Borderive"}});
        return map;
    }
};

// ─────────────────────────────────────────────
// GameMap — ajout de provinces
// ─────────────────────────────────────────────

TEST_F(GameStateTest, AddProvince) {
    GameMap map;
    map.addProvince(Province{"Winterfell"});
    EXPECT_EQ(map.size(), 1);
}

TEST_F(GameStateTest, ProvinceAccessByName) {
    GameMap map;
    map.addProvince(Province{"Winterfell"});
    EXPECT_EQ(map.province("Winterfell").name(), "Winterfell");
}

TEST_F(GameStateTest, UnknownProvinceThrows) {
    GameMap map;
    EXPECT_THROW(auto p{map.province("unknown")}, std::out_of_range);
}

TEST_F(GameStateTest, HasProvince) {
    GameMap map;
    map.addProvince(Province{"Winterfell"});
    EXPECT_TRUE(map.hasProvince("Winterfell"));
    EXPECT_FALSE(map.hasProvince("unknown"));
}

// ─────────────────────────────────────────────
// GameMap — adjacences
// ─────────────────────────────────────────────

TEST_F(GameStateTest, AreAdjacent) {
    auto map = makeSimpleMap();
    EXPECT_TRUE(map.areAdjacent("Winterfell", "Borderive"));
    EXPECT_TRUE(map.areAdjacent("Winterfell", "Blancport"));
}

TEST_F(GameStateTest, AreNotAdjacent) {
    GameMap map;
    map.addProvince(Province{"Winterfell", std::nullopt, {"Borderive"}});
    map.addProvince(Province{"Borderive", std::nullopt, {"Winterfell"}});
    map.addProvince(Province{"Blancport", std::nullopt, {}});
    EXPECT_FALSE(map.areAdjacent("Winterfell", "Blancport"));
}

TEST_F(GameStateTest, Neighbors) {
    auto map       = makeSimpleMap();
    auto neighbors = map.neighbors("Winterfell");
    EXPECT_EQ(neighbors.size(), 2);
}

// ─────────────────────────────────────────────
// GameMap — clearAllOrders
// ─────────────────────────────────────────────

TEST_F(GameStateTest, ClearAllOrders) {
    auto map = makeSimpleMap();
    map.province("Winterfell").addUnit(Footman{&stark});
    map.province("Borderive").addUnit(Footman{&lannister});
    map.province("Winterfell").setOrder(MarchOrder{stark});
    map.province("Borderive").setOrder(DefenseOrder{lannister});

    map.clearAllOrders();

    EXPECT_FALSE(map.province("Winterfell").order().has_value());
    EXPECT_FALSE(map.province("Borderive").order().has_value());
}

// ─────────────────────────────────────────────
// GameState
// ─────────────────────────────────────────────

TEST_F(GameStateTest, InitialTurnNumber) {
    GameState state;
    EXPECT_EQ(state.turnNumber, 0);
}

TEST_F(GameStateTest, AddPlayer) {
    GameState state;
    state.players.push_back(Player{stark});
    EXPECT_EQ(state.players.size(), 1);
    EXPECT_EQ(&state.players.front().house.get(), &stark);
}

TEST_F(GameStateTest, InitialVictoryPoints) {
    GameState state;
    state.players.push_back(Player{stark});
    EXPECT_EQ(state.players.front().victoryPoints, 0);
}
