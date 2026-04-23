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

class MapIntegrationTest : public ::testing::Test {
protected:
    House stark{"Stark", makeHand()};
    House lannister{"Lannister", makeHand()};
    House baratheon{"Barathéon", makeHand()};

    // Carte à 5 provinces :
    //
    //   Winterfell -- Blancport -- Port-Réal
    //       |                        |
    //   Castral Roc ----------- Vivesaigues
    //
    GameMap makeMap() {
        GameMap map;
        map.addProvince(Province{"Winterfell",  Stronghold{}, {"Blancport",   "Castral Roc"}});
        map.addProvince(Province{"Blancport",   Castle{},     {"Winterfell",  "Port-Réal"}});
        map.addProvince(Province{"Port-Réal",   Stronghold{}, {"Blancport",   "Vivesaigues"}});
        map.addProvince(Province{"Castral Roc", Castle{},     {"Winterfell",  "Vivesaigues"}});
        map.addProvince(Province{"Vivesaigues", std::nullopt, {"Port-Réal",   "Castral Roc"}});
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
    EXPECT_TRUE(map.areAdjacent("Winterfell",  "Blancport"));
    EXPECT_TRUE(map.areAdjacent("Winterfell",  "Castral Roc"));
    EXPECT_FALSE(map.areAdjacent("Winterfell", "Port-Réal"));
    EXPECT_FALSE(map.areAdjacent("Winterfell", "Vivesaigues"));
}

TEST_F(MapIntegrationTest, NeighborCount) {
    auto map = makeMap();
    EXPECT_EQ(map.neighbors("Winterfell").size(),  2);
    EXPECT_EQ(map.neighbors("Blancport").size(),   2);
    EXPECT_EQ(map.neighbors("Vivesaigues").size(), 2);
}

// ─────────────────────────────────────────────
// Unités + contrôle
// ─────────────────────────────────────────────

TEST_F(MapIntegrationTest, PlaceUnitsOnMultipleProvinces) {
    auto map = makeMap();
    map.province("Winterfell").addUnit(Footman{&stark});
    map.province("Winterfell").addUnit(Knight{&stark});
    map.province("Port-Réal").addUnit(Footman{&lannister});

    EXPECT_EQ(map.province("Winterfell").units().size(), 2);
    EXPECT_EQ(map.province("Port-Réal").units().size(),  1);
    EXPECT_TRUE(map.province("Blancport").units().empty());
}

TEST_F(MapIntegrationTest, ControllerFollowsUnits) {
    auto map = makeMap();
    map.province("Winterfell").addUnit(Footman{&stark});
    map.province("Port-Réal").addUnit(Footman{&lannister});

    EXPECT_EQ(&map.province("Winterfell").controller()->get(), &stark);
    EXPECT_EQ(&map.province("Port-Réal").controller()->get(),  &lannister);
    EXPECT_FALSE(map.province("Blancport").controller().has_value());
}

// ─────────────────────────────────────────────
// Ordres sur la carte
// ─────────────────────────────────────────────

TEST_F(MapIntegrationTest, PlaceOrdersOnMultipleProvinces) {
    auto map = makeMap();
    map.province("Winterfell").addUnit(Footman{&stark});
    map.province("Port-Réal").addUnit(Footman{&lannister});

    map.province("Winterfell").setOrder(MarchOrder{stark});
    map.province("Port-Réal").setOrder(DefenseOrder{lannister});

    EXPECT_TRUE(map.province("Winterfell").order().has_value());
    EXPECT_TRUE(map.province("Port-Réal").order().has_value());
    EXPECT_FALSE(map.province("Blancport").order().has_value());
}

TEST_F(MapIntegrationTest, ClearAllOrdersAcrossMap) {
    auto map = makeMap();
    map.province("Winterfell").addUnit(Footman{&stark});
    map.province("Port-Réal").addUnit(Footman{&lannister});
    map.province("Castral Roc").addUnit(Knight{&baratheon});

    map.province("Winterfell").setOrder(MarchOrder{stark});
    map.province("Port-Réal").setOrder(DefenseOrder{lannister});
    map.province("Castral Roc").setOrder(RaidOrder{baratheon});

    map.clearAllOrders();

    for (const auto& name : map.provinceNames())
        EXPECT_FALSE(map.province(name).order().has_value());
}

// ─────────────────────────────────────────────
// Structures
// ─────────────────────────────────────────────

TEST_F(MapIntegrationTest, StructuresOnMap) {
    auto map = makeMap();
    EXPECT_TRUE(map.province("Winterfell").structure().has_value());
    EXPECT_TRUE(map.province("Blancport").structure().has_value());
    EXPECT_FALSE(map.province("Vivesaigues").structure().has_value());
}

TEST_F(MapIntegrationTest, RecruitmentPointsOnMap) {
    auto map = makeMap();
    EXPECT_EQ(recruitmentPoints(map.province("Winterfell").structure().value()), 2); // Stronghold
    EXPECT_EQ(recruitmentPoints(map.province("Blancport").structure().value()),  1); // Castle
}
