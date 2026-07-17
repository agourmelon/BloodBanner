#include "common/game_state.hpp"
#include "common/houses.hpp"
#include "common/orders.hpp"
#include "common/provinces.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>
#include <optional>
#include <stdexcept>

// ─────────────────────────────────────────────
// Fixture
// ─────────────────────────────────────────────
class GameStateTest : public ::testing::Test {
protected:

    House stark{"Stark", makeHand()};
    House lannister{"Lannister", makeHand()};

    static GameMap makeSimpleMap() {
        GameMap map;
        map.addProvince("Winterfell", Province{std::nullopt});
        map.addProvince("Borderive", Province{std::nullopt});
        map.addProvince("Blancport", Province{Castle{}});

        map.linkProvinces("Winterfell", "Borderive");
        map.linkProvinces("Winterfell", "Blancport");
        return map;
    }
};

// ─────────────────────────────────────────────
// GameMap — ajout de provinces
// ─────────────────────────────────────────────

TEST_F(GameStateTest, AddProvince) {
    GameMap map;
    map.addProvince("Winterfell", Province{});
    EXPECT_EQ(map.size(), 1);
}

TEST_F(GameStateTest, ProvinceAccessByName) {
    GameMap map = makeSimpleMap();
    EXPECT_EQ(typeid(map.getProvince("Blancport").structure()), typeid(Castle));
    EXPECT_EQ(map.getProvince("Winterfell").structure(), std::nullopt);
    EXPECT_EQ(map.getProvince("Borderive").structure(), std::nullopt);
}

TEST_F(GameStateTest, UnknownProvinceThrows) {
    GameMap map;
    EXPECT_THROW(auto p{map.getProvince("unknown")}, std::out_of_range);
}

TEST_F(GameStateTest, HasProvince) {
    GameMap map;
    map.addProvince("Winterfell", Province{});
    EXPECT_TRUE(map.hasProvince("Winterfell"));
    EXPECT_FALSE(map.hasProvince("unknown"));
}

// ─────────────────────────────────────────────
// GameMap — adjacences
// ─────────────────────────────────────────────

TEST_F(GameStateTest, AreAdjacent) {
    auto map = makeSimpleMap();
    EXPECT_TRUE(map.areProvincesAdjacent("Winterfell", "Borderive"));
    EXPECT_TRUE(map.areProvincesAdjacent("Winterfell", "Blancport"));
}

TEST_F(GameStateTest, AreNotAdjacent) {
    auto map = makeSimpleMap();
    EXPECT_FALSE(map.areProvincesAdjacent("Borderive", "Blancport"));
}

TEST_F(GameStateTest, AdjacentProvinceNames) {
    auto map       = makeSimpleMap();
    auto neighbors = map.getAdjacentProvinceNames("Winterfell");
    EXPECT_EQ(neighbors.size(), 2);
}

// ─────────────────────────────────────────────
// GameMap — clearAllOrders
// ─────────────────────────────────────────────

TEST_F(GameStateTest, ClearAllOrders) {
    auto map = makeSimpleMap();
    map.getProvince("Winterfell").setOrder(MarchOrder{stark});
    map.getProvince("Borderive").setOrder(DefenseOrder{lannister});

    map.clearAllOrders();

    EXPECT_FALSE(map.getProvince("Winterfell").order().has_value());
    EXPECT_FALSE(map.getProvince("Borderive").order().has_value());
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
    state.players.push_back(Player{.house = stark});
    EXPECT_EQ(state.players.size(), 1);
    EXPECT_EQ(&state.players.front().house.get(), &stark);
}

TEST_F(GameStateTest, InitialVictoryPoints) {
    GameState state;
    state.players.push_back(Player{.house = stark});
    EXPECT_EQ(state.players.front().victoryPoints, 0);
}
