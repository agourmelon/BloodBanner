#include "common/provinces.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

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

class ProvinceTest : public ::testing::Test {
protected:
    House stark{"Stark", makeHand()};
    House lannister{"Lannister", makeHand()};

    Province makeEmptyProvince() {
        return Province{"p01", "Winterfell"};
    }

    Province makeProvinceWithCastle() {
        return Province{"p02", "Port-Réal", Castle{}};
    }

    Province makeProvinceWithStronghold() {
        return Province{"p03", "Castral Roc", Stronghold{}};
    }
};

// ─────────────────────────────────────────────
// Structure
// ─────────────────────────────────────────────

TEST_F(ProvinceTest, NoStructureByDefault) {
    auto p = makeEmptyProvince();
    EXPECT_FALSE(p.structure().has_value());
}

TEST_F(ProvinceTest, CastleStructure) {
    auto p = makeProvinceWithCastle();
    ASSERT_TRUE(p.structure().has_value());
    EXPECT_EQ(recruitmentPoints(p.structure().value()), 1);
}

TEST_F(ProvinceTest, StrongholdStructure) {
    auto p = makeProvinceWithStronghold();
    ASSERT_TRUE(p.structure().has_value());
    EXPECT_EQ(recruitmentPoints(p.structure().value()), 2);
}

// ─────────────────────────────────────────────
// Contrôleur
// ─────────────────────────────────────────────

TEST_F(ProvinceTest, NoControllerByDefault) {
    auto p = makeEmptyProvince();
    EXPECT_FALSE(p.controller().has_value());
}

TEST_F(ProvinceTest, SetControllerOnEmptyProvince) {
    auto p = makeEmptyProvince();
    p.setController(stark);
    ASSERT_TRUE(p.controller().has_value());
    EXPECT_EQ(&p.controller()->get(), &stark);
}

TEST_F(ProvinceTest, SetControllerOnOccupiedProvinceThrows) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    EXPECT_THROW(p.setController(lannister), std::logic_error);
}

// ─────────────────────────────────────────────
// Unités
// ─────────────────────────────────────────────

TEST_F(ProvinceTest, NoUnitsByDefault) {
    auto p = makeEmptyProvince();
    EXPECT_TRUE(p.units().empty());
}

TEST_F(ProvinceTest, AddUnitSetsController) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    ASSERT_TRUE(p.controller().has_value());
    EXPECT_EQ(&p.controller()->get(), &stark);
}

TEST_F(ProvinceTest, AddMultipleUnitsOfSameOwner) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    p.addUnit(Knight{&stark});
    EXPECT_EQ(p.units().size(), 2);
}

TEST_F(ProvinceTest, AddUnitOfDifferentOwnerAsserts) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    EXPECT_THROW(p.addUnit(Footman{&lannister}), std::logic_error);
}

TEST_F(ProvinceTest, RemoveUnit) {
    auto p = makeEmptyProvince();
    Footman f{&stark};
    p.addUnit(f);
    p.removeUnit(f);
    EXPECT_TRUE(p.units().empty());
}

// ─────────────────────────────────────────────
// Ordres
// ─────────────────────────────────────────────

TEST_F(ProvinceTest, NoOrderByDefault) {
    auto p = makeEmptyProvince();
    EXPECT_FALSE(p.order().has_value());
}

TEST_F(ProvinceTest, SetOrderOnOccupiedProvince) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    p.setOrder(MarchOrder{stark});
    EXPECT_TRUE(p.order().has_value());
}

TEST_F(ProvinceTest, SetOrderOnEmptyProvinceThrows) {
    auto p = makeEmptyProvince();
    EXPECT_THROW(p.setOrder(MarchOrder{stark}), std::logic_error);
}

TEST_F(ProvinceTest, ClearOrder) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    p.setOrder(MarchOrder{stark});
    p.clearOrder();
    EXPECT_FALSE(p.order().has_value());
}
