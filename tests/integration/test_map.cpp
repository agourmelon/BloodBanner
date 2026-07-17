#include "common/game_state.hpp"
#include "common/houses.hpp"
#include "common/orders.hpp"
#include "common/provinces.hpp"
#include "common/units.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>
#include <optional>

// ─────────────────────────────────────────────
// Fixture
// ─────────────────────────────────────────────

class MapIntegrationTest : public ::testing::Test {
protected:

    House stark{"Stark", makeHand()};
    House lannister{"Lannister", makeHand()};
    House baratheon{"Barathéon", makeHand()};

    static GameMap makeMap() {
        GameMap map;
        map.addProvince("Winterfell", Province{Stronghold{}});
        map.addProvince("Blancport", Province{Castle{}});
        map.addProvince("Castral Roc", Province{Castle{}});
        map.addProvince("Port-Réal", Province{Stronghold{}});
        map.addProvince("Vivesaigues", Province{std::nullopt});
        map.linkProvinces("Winterfell", "Castral Roc");
        map.linkProvinces("Winterfell", "Blancport");
        map.linkProvinces("Castral Roc", "Vivesaigues");
        map.linkProvinces("Blancport", "Port-Réal");
        map.linkProvinces("Vivesaigues", "Port-Réal");

        return map;
    }
};

// ─────────────────────────────────────────────
// Topologie
// ─────────────────────────────────────────────

TEST_F(MapIntegrationTest, MapSize) {
    auto map = makeMap();
    EXPECT_EQ(map.size(), 5);
}

TEST_F(MapIntegrationTest, AdjacentProvinces) {
    auto map = makeMap();
    EXPECT_TRUE(map.areProvincesAdjacent("Winterfell", "Blancport"));
    EXPECT_TRUE(map.areProvincesAdjacent("Winterfell", "Castral Roc"));
    EXPECT_FALSE(map.areProvincesAdjacent("Winterfell", "Port-Réal"));
    EXPECT_FALSE(map.areProvincesAdjacent("Winterfell", "Vivesaigues"));
}

TEST_F(MapIntegrationTest, NeighborCount) {
    auto map = makeMap();
    EXPECT_EQ(map.getAdjacentProvinceNames("Winterfell").size(), 2);
    EXPECT_EQ(map.getAdjacentProvinceNames("Blancport").size(), 2);
    EXPECT_EQ(map.getAdjacentProvinceNames("Vivesaigues").size(), 2);
}

// ─────────────────────────────────────────────
// Unités + contrôle
// ─────────────────────────────────────────────

TEST_F(MapIntegrationTest, PlaceUnitsOnMultipleProvinces) {
    GameMap map = makeMap();
    map.getProvince("Winterfell").addUnit(Knight{&stark});
    map.getProvince("Port-Réal").addUnit(Footman{&lannister});

    EXPECT_EQ(map.getProvince("Winterfell").units().size(), 2);
    EXPECT_EQ(map.getProvince("Port-Réal").units().size(), 1);
    EXPECT_TRUE(map.getProvince("Blancport").units().empty());
}

TEST_F(MapIntegrationTest, ControllerFollowsUnits) {
    auto map = makeMap();
    map.getProvince("Winterfell").addUnit(Footman{&stark});
    map.getProvince("Port-Réal").addUnit(Footman{&lannister});

    EXPECT_EQ(&map.getProvince("Winterfell").controller()->get(), &stark);
    EXPECT_EQ(&map.getProvince("Port-Réal").controller()->get(), &lannister);
    EXPECT_FALSE(map.getProvince("Blancport").controller().has_value());
}

// ─────────────────────────────────────────────
// Ordres sur la carte
// ─────────────────────────────────────────────

TEST_F(MapIntegrationTest, PlaceOrdersOnMultipleProvinces) {
    auto map = makeMap();
    map.getProvince("Winterfell").addUnit(Footman{&stark});
    map.getProvince("Port-Réal").addUnit(Footman{&lannister});

    map.getProvince("Winterfell").setOrder(MarchOrder{stark});
    map.getProvince("Port-Réal").setOrder(DefenseOrder{lannister});

    EXPECT_TRUE(map.getProvince("Winterfell").order().has_value());
    EXPECT_TRUE(map.getProvince("Port-Réal").order().has_value());
    EXPECT_FALSE(map.getProvince("Blancport").order().has_value());
}

TEST_F(MapIntegrationTest, ClearAllOrdersAcrossMap) {
    auto map = makeMap();
    map.getProvince("Winterfell").addUnit(Footman{&stark});
    map.getProvince("Port-Réal").addUnit(Footman{&lannister});
    map.getProvince("Castral Roc").addUnit(Knight{&baratheon});

    map.getProvince("Winterfell").setOrder(MarchOrder{stark});
    map.getProvince("Port-Réal").setOrder(DefenseOrder{lannister});
    map.getProvince("Castral Roc").setOrder(RaidOrder{baratheon});

    map.clearAllOrders();

    for (const auto & name : map.getAllProvinceNames())
        EXPECT_FALSE(map.getProvince(name).order().has_value());
}

// ─────────────────────────────────────────────
// Structures
// ─────────────────────────────────────────────

TEST_F(MapIntegrationTest, StructuresOnMap) {
    auto map = makeMap();
    EXPECT_TRUE(map.getProvince("Winterfell").structure().has_value());
    EXPECT_TRUE(map.getProvince("Blancport").structure().has_value());
    EXPECT_FALSE(map.getProvince("Vivesaigues").structure().has_value());
}

TEST_F(MapIntegrationTest, RecruitmentPointsOnMap) {
    auto map = makeMap();
    EXPECT_EQ(recruitmentPoints(map.getProvince("Winterfell").structure().value()), 2);
    EXPECT_EQ(recruitmentPoints(map.getProvince("Blancport").structure().value()), 1);
}
