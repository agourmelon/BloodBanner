#include "common/game_state.hpp"
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

class GameStateTest : public ::testing::Test {
protected:
    House stark{"Stark", makeHand()};
    House lannister{"Lannister", makeHand()};

    GameMap makeSimpleMap() {
        GameMap map;
        map.addProvince(Province{"Winterfell", std::nullopt, {"Borderive","Blancport"}});
        map.addProvince(Province{"Borderive",  std::nullopt, {"Winterfell","Blancport"}});
        map.addProvince(Province{"Blancport",  Castle{},     {"Winterfell","Borderive"}});
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
    EXPECT_TRUE(map.areAdjacent("Winterfell","Borderive"));
    EXPECT_TRUE(map.areAdjacent("Winterfell","Blancport"));
}

TEST_F(GameStateTest, AreNotAdjacent) {
    GameMap map;
    map.addProvince(Province{"Winterfell", std::nullopt, {"Borderive"}});
    map.addProvince(Province{"Borderive",  std::nullopt, {"Winterfell"}});
    map.addProvince(Province{"Blancport",  std::nullopt, {}});
    EXPECT_FALSE(map.areAdjacent("Winterfell","Blancport"));
}

TEST_F(GameStateTest, Neighbors) {
    auto map = makeSimpleMap();
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
