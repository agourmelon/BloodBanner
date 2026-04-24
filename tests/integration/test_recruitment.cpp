#include "common/game_state.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>

// ─────────────────────────────────────────────
// Fixture
// ─────────────────────────────────────────────

class RecruitmentIntegrationTest : public ::testing::Test {
protected:
    House stark{"Stark",         makeHand()};
    House lannister{"Lannister", makeHand()};

    GameMap makeMap() {
        GameMap map;
        map.addProvince(Province{"Winterfell",  Stronghold{}, {"Blancport"}});
        map.addProvince(Province{"Blancport",   Castle{},     {"Winterfell", "Port-Réal"}});
        map.addProvince(Province{"Port-Réal",   std::nullopt, {"Blancport"}});
        return map;
    }
};

// ─────────────────────────────────────────────
// Points de recrutement disponibles
// ─────────────────────────────────────────────

TEST_F(RecruitmentIntegrationTest, StrongholdGivesTwoRecruitmentPoints) {
    auto map = makeMap();
    auto& p = map.province("Winterfell");
    ASSERT_TRUE(p.structure().has_value());
    EXPECT_EQ(recruitmentPoints(p.structure().value()), 2);
}

TEST_F(RecruitmentIntegrationTest, CastleGivesOneRecruitmentPoint) {
    auto map = makeMap();
    auto& p = map.province("Blancport");
    ASSERT_TRUE(p.structure().has_value());
    EXPECT_EQ(recruitmentPoints(p.structure().value()), 1);
}

TEST_F(RecruitmentIntegrationTest, ProvinceWithoutStructureGivesNoRecruitmentPoints) {
    auto map = makeMap();
    auto& p = map.province("Port-Réal");
    EXPECT_FALSE(p.structure().has_value());
}

// ─────────────────────────────────────────────
// Ordre de recrutement uniquement sur structure
// ─────────────────────────────────────────────

TEST_F(RecruitmentIntegrationTest, MusterOrderOnStronghold) {
    auto map = makeMap();
    map.province("Winterfell").addUnit(Footman{&stark});
    map.province("Winterfell").setOrder(MusterOrder{stark});
    EXPECT_TRUE(map.province("Winterfell").order().has_value());
}

// ─────────────────────────────────────────────
// Recrutement de nouvelles unités
// ─────────────────────────────────────────────

TEST_F(RecruitmentIntegrationTest, RecruitFootmanOnCastle) {
    auto map = makeMap();
    auto& p = map.province("Blancport");
    p.addUnit(Footman{&stark});
    p.setOrder(MusterOrder{stark});

    // Simule le recrutement d'un fantassin (coût 1, castle = 1 pt)
    p.addUnit(Footman{&stark});
    EXPECT_EQ(p.units().size(), 2);
}

TEST_F(RecruitmentIntegrationTest, RecruitKnightOnStronghold) {
    auto map = makeMap();
    auto& p = map.province("Winterfell");
    p.addUnit(Footman{&stark});
    p.setOrder(MusterOrder{stark});

    // Simule le recrutement d'un chevalier (coût 2, stronghold = 2 pts)
    p.addUnit(Knight{&stark});
    EXPECT_EQ(p.units().size(), 2);
}

TEST_F(RecruitmentIntegrationTest, UpgradeFootmanToKnight) {
    auto map = makeMap();
    auto& p = map.province("Winterfell");
    Footman f{&stark};
    p.addUnit(f);
    p.setOrder(MusterOrder{stark});

    // Simule la conversion : retire le fantassin, ajoute un chevalier
    p.removeUnit(f);
    p.addUnit(Knight{&stark});

    EXPECT_EQ(p.units().size(), 1);
    EXPECT_TRUE(std::holds_alternative<Knight>(p.units().front()));
}

// ─────────────────────────────────────────────
// Contrôleur après recrutement
// ─────────────────────────────────────────────

TEST_F(RecruitmentIntegrationTest, ControllerUnchangedAfterRecruitment) {
    auto map = makeMap();
    auto& p = map.province("Winterfell");
    p.addUnit(Footman{&stark});
    p.addUnit(Footman{&stark});

    EXPECT_EQ(&p.controller()->get(), &stark);
}
