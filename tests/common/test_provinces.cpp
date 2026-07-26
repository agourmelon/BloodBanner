#include "common/houses.hpp"
#include "common/orders.hpp"
#include "common/provinces.hpp"
#include "common/units.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>

// ─────────────────────────────────────────────
// Fixture
// ─────────────────────────────────────────────

class ProvinceTest : public ::testing::Test {
protected:

    House stark{"Stark", makeHand()};
    House lannister{"Lannister", makeHand()};

    static Province makeEmptyProvince() { return Province{}; }

    static Province makeProvinceWithCastle() { return Province{Castle{}}; }

    static Province makeProvinceWithStronghold() { return Province{Stronghold{}}; }
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

TEST_F(ProvinceTest, SetController) {
    auto p = makeEmptyProvince();
    p.setController(stark);
    ASSERT_TRUE(p.controller().has_value());
    EXPECT_EQ(&p.controller()->get(), &stark);
}

TEST_F(ProvinceTest, ClearController) {
    auto p = makeEmptyProvince();
    p.setController(stark);
    p.clearController();
    EXPECT_FALSE(p.controller().has_value());
}

TEST_F(ProvinceTest, SetControllerOverride) {
    auto p = makeEmptyProvince();
    p.setController(stark);
    p.setController(lannister);
    EXPECT_EQ(&p.controller()->get(), &lannister);
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

TEST_F(ProvinceTest, AddMultipleUnits) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    p.addUnit(Knight{&stark});
    EXPECT_EQ(p.units().size(), 2);
}

TEST_F(ProvinceTest, RemoveUnit) {
    auto    p = makeEmptyProvince();
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

TEST_F(ProvinceTest, SetOrder) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    p.setOrder(MarchOrder{stark});
    EXPECT_TRUE(p.order().has_value());
}

TEST_F(ProvinceTest, SetOrderOnEmptyProvince) {
    auto p = makeEmptyProvince();
    p.setOrder(MarchOrder{stark});
    EXPECT_TRUE(p.order().has_value());
}

TEST_F(ProvinceTest, SetOrderOfDifferentOwner) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    p.setOrder(MarchOrder{lannister});
    EXPECT_TRUE(p.order().has_value());
}

TEST_F(ProvinceTest, ClearOrder) {
    auto p = makeEmptyProvince();
    p.addUnit(Footman{&stark});
    p.setOrder(MarchOrder{stark});
    p.clearOrder();
    EXPECT_FALSE(p.order().has_value());
}
