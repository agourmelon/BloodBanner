#include "server/map_generator.hpp"
#include "common/constants.hpp"
#include "test_utils.hpp"

#include <algorithm>
#include <format>
#include <set>
#include <gtest/gtest.h>
#include <random>

// ─────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────

static std::string strongholdOf(const House& h) {
    const auto it = std::ranges::find_if(HOUSE_STRONGHOLDS,
        [&](const auto& e) { return e.first == h.name; });
    return std::string{it->second};
}

// Construit un vecteur de noms dont les N premiers sont les forteresses des joueurs actifs.
static std::vector<std::string> namesWithStrongholds(
    const std::vector<House*>& players, std::size_t numProvinces
) {
    std::vector<std::string> names;
    for (const auto* h : players)
        names.push_back(strongholdOf(*h));
    int counter = 0;
    while (names.size() < numProvinces)
        names.push_back(std::format("Extra-{:02d}", ++counter));
    return names;
}

// ─────────────────────────────────────────────
// DefaultNamer
// ─────────────────────────────────────────────

TEST(DefaultNamer, Sequential) {
    DefaultNamer namer;
    EXPECT_EQ(namer.next(), "Province-01");
    EXPECT_EQ(namer.next(), "Province-02");
    EXPECT_EQ(namer.next(), "Province-03");
}

TEST(DefaultNamer, UniqueNames) {
    DefaultNamer namer;
    std::vector<std::string> names;
    for (int i = 0; i < 20; ++i)
        names.push_back(namer.next());
    const std::set<std::string> unique(names.begin(), names.end());
    EXPECT_EQ(unique.size(), names.size());
}

// ─────────────────────────────────────────────
// ThematicNamer
// ─────────────────────────────────────────────

TEST(ThematicNamer, UsesGivenNamesInOrder) {
    ThematicNamer namer({"Alpha", "Beta", "Gamma"});
    EXPECT_EQ(namer.next(), "Alpha");
    EXPECT_EQ(namer.next(), "Beta");
    EXPECT_EQ(namer.next(), "Gamma");
}

TEST(ThematicNamer, FallbackStartsAfterListExhausted) {
    ThematicNamer namer({"Alpha"});
    EXPECT_EQ(namer.next(), "Alpha");
    const auto first_fallback = namer.next();
    EXPECT_TRUE(first_fallback.starts_with("Province-"));
}

TEST(ThematicNamer, FallbackContinuesSequentially) {
    ThematicNamer namer({"Alpha"});
    namer.next();
    EXPECT_EQ(namer.next(), "Province-01");
    EXPECT_EQ(namer.next(), "Province-02");
}

// ─────────────────────────────────────────────
// HexGridStrategy
// ─────────────────────────────────────────────

class HexGridStrategyTest : public ::testing::Test {
protected:
    HexGridStrategy strategy;
    std::mt19937    rng{42};
};

TEST_F(HexGridStrategyTest, SingleProvince_NoNeighbors) {
    const auto adj = strategy.buildAdjacencies(1, rng);
    ASSERT_EQ(adj.size(), 1u);
    EXPECT_TRUE(adj[0].empty());
}

TEST_F(HexGridStrategyTest, TwoProvinces_MutuallyAdjacent) {
    // Les 2 premières cellules de hexagonalGrid(1) sont voisines entre elles.
    const auto adj = strategy.buildAdjacencies(2, rng);
    ASSERT_EQ(adj.size(), 2u);
    ASSERT_EQ(adj[0].size(), 1u);
    ASSERT_EQ(adj[1].size(), 1u);
    EXPECT_EQ(adj[0][0], 1u);
    EXPECT_EQ(adj[1][0], 0u);
}

TEST_F(HexGridStrategyTest, SevenProvinces_CenterHasSixNeighbors) {
    // hexagonalGrid(1) : 7 cellules, la cellule centrale est à l'index 3.
    const auto adj = strategy.buildAdjacencies(7, rng);
    ASSERT_EQ(adj.size(), 7u);
    EXPECT_EQ(adj[3].size(), 6u);
}

TEST_F(HexGridStrategyTest, SevenProvinces_PeripheralHasThreeNeighbors) {
    const auto adj = strategy.buildAdjacencies(7, rng);
    for (std::size_t i = 0; i < 7; ++i) {
        if (i == 3) continue; // centre
        EXPECT_EQ(adj[i].size(), 3u) << "Province périphérique " << i << " devrait avoir 3 voisins";
    }
}

TEST_F(HexGridStrategyTest, AdjacencyIsSymmetric) {
    const auto adj = strategy.buildAdjacencies(19, rng); // radius 2
    for (std::size_t i = 0; i < adj.size(); ++i) {
        for (const auto j : adj[i]) {
            const auto& adj_j = adj[j];
            EXPECT_NE(std::ranges::find(adj_j, i), adj_j.end())
                << "Province " << i << " voisine de " << j << " mais pas l'inverse";
        }
    }
}

TEST_F(HexGridStrategyTest, NoSelfAdjacency) {
    const auto adj = strategy.buildAdjacencies(19, rng);
    for (std::size_t i = 0; i < adj.size(); ++i) {
        EXPECT_EQ(std::ranges::find(adj[i], i), adj[i].end())
            << "Province " << i << " est adjacente à elle-même";
    }
}

TEST_F(HexGridStrategyTest, CorrectTotalSize) {
    // buildAdjacencies doit retourner exactement numProvinces entrées
    for (const std::size_t n : {1u, 7u, 10u, 19u}) {
        const auto adj = strategy.buildAdjacencies(n, rng);
        EXPECT_EQ(adj.size(), n);
    }
}

// ─────────────────────────────────────────────
// MapGenerator — Intégration
// ─────────────────────────────────────────────

class MapGeneratorTest : public ::testing::Test {
protected:
    House baratheon{"Barathéon", makeHand()};
    House stark    {"Stark",     makeHand()};
    House lannister{"Lannister", makeHand()};
    House greyjoy  {"Greyjoy",   makeHand()};
    House martell  {"Martell",   makeHand()};
    House tyrell   {"Tyrell",    makeHand()};

    HexGridStrategy topology;
    std::mt19937    rng{42};

    GameMap generateFor(std::vector<House*> players) {
        const std::size_t n = MAP_BASE_PROVINCES + MAP_PROVINCES_PER_PLAYER * players.size();
        ThematicNamer namer(namesWithStrongholds(players, n));
        MapGenerator  gen(topology, namer);
        return gen.generate(players, rng);
    }
};

// ── Validation du nombre de joueurs ──────────

TEST_F(MapGeneratorTest, TooFewPlayers_Throws) {
    std::vector<House*> players = {&baratheon};
    const std::size_t n = MAP_BASE_PROVINCES + MAP_PROVINCES_PER_PLAYER;
    ThematicNamer namer(namesWithStrongholds(players, n));
    MapGenerator  gen(topology, namer);
    EXPECT_THROW(gen.generate(players, rng), std::invalid_argument);
}

TEST_F(MapGeneratorTest, TooManyPlayers_Throws) {
    House extra{"Extra", makeHand()};
    std::vector<House*> players = {
        &baratheon, &stark, &lannister, &greyjoy, &martell, &tyrell, &extra
    };
    DefaultNamer namer;
    MapGenerator gen(topology, namer);
    EXPECT_THROW(gen.generate(players, rng), std::invalid_argument);
}

// ── Nombre de provinces ───────────────────────

TEST_F(MapGeneratorTest, TwoPlayers_CorrectProvinceCount) {
    auto map = generateFor({&baratheon, &stark});
    EXPECT_EQ(map.size(), MAP_BASE_PROVINCES + MAP_PROVINCES_PER_PLAYER * 2u);
}

TEST_F(MapGeneratorTest, SixPlayers_CorrectProvinceCount) {
    auto map = generateFor({&baratheon, &stark, &lannister, &greyjoy, &martell, &tyrell});
    EXPECT_EQ(map.size(), MAP_BASE_PROVINCES + MAP_PROVINCES_PER_PLAYER * 6u);
}

// ── Contrôleurs de départ ─────────────────────

TEST_F(MapGeneratorTest, StartingStronghold_HasCorrectController) {
    auto map = generateFor({&baratheon, &stark});

    const auto& accalmie   = map.province("Accalmie");
    const auto& winterfell = map.province("Winterfell");

    ASSERT_TRUE(accalmie.controller().has_value());
    ASSERT_TRUE(winterfell.controller().has_value());
    EXPECT_EQ(&accalmie.controller()->get(),   &baratheon);
    EXPECT_EQ(&winterfell.controller()->get(), &stark);
}

// ── Unités de départ sur la forteresse ───────

TEST_F(MapGeneratorTest, StartingStronghold_HasCorrectUnitComposition) {
    auto map = generateFor({&baratheon, &stark});
    const auto& units = map.province("Accalmie").units();

    int footmen = 0, knights = 0;
    for (const auto& u : units) {
        std::visit([&](const auto& unit) {
            if constexpr (std::is_same_v<std::decay_t<decltype(unit)>, Footman>) ++footmen;
            else if constexpr (std::is_same_v<std::decay_t<decltype(unit)>, Knight>) ++knights;
        }, u);
    }

    EXPECT_EQ(footmen, STARTING_FOOTMEN_ON_STRONGHOLD);
    EXPECT_EQ(knights, STARTING_KNIGHTS_ON_STRONGHOLD);
}

TEST_F(MapGeneratorTest, StartingStronghold_UnitsOwnedByCorrectHouse) {
    auto map = generateFor({&baratheon, &stark});
    for (const auto& u : map.province("Accalmie").units()) {
        EXPECT_EQ(&unitOwner(u), &baratheon);
    }
}

// ── Non-adjacence des forteresses ─────────────

TEST_F(MapGeneratorTest, StartingStrongholds_AreNotAdjacent) {
    auto map = generateFor({&baratheon, &stark, &lannister});
    EXPECT_FALSE(map.areAdjacent("Accalmie",   "Winterfell"));
    EXPECT_FALSE(map.areAdjacent("Accalmie",   "Castral Roc"));
    EXPECT_FALSE(map.areAdjacent("Winterfell", "Castral Roc"));
}

// ── Propriétés topologiques ───────────────────

TEST_F(MapGeneratorTest, AllAdjacenciesAreSymmetric) {
    auto map = generateFor({&baratheon, &stark});
    for (const auto& name : map.provinceNames()) {
        for (const auto* neighbor : map.neighbors(name)) {
            EXPECT_TRUE(map.areAdjacent(neighbor->name(), name))
                << neighbor->name() << " → " << name << " n'est pas symétrique";
        }
    }
}

TEST_F(MapGeneratorTest, AllProvincesHaveAtLeastOneNeighbor) {
    auto map = generateFor({&baratheon, &stark});
    for (const auto& name : map.provinceNames()) {
        EXPECT_FALSE(map.neighbors(name).empty())
            << "Province " << name << " n'a aucun voisin";
    }
}

// ── Structures ───────────────────────────────

TEST_F(MapGeneratorTest, StructureCount_MatchesRatio) {
    auto map = generateFor({&baratheon, &stark});
    const std::size_t total = map.size();

    std::size_t withStructure = 0;
    for (const auto& name : map.provinceNames()) {
        if (map.province(name).structure().has_value()) ++withStructure;
    }

    const auto expected =
        static_cast<std::size_t>(static_cast<float>(total) * MAP_STRUCTURE_RATIO);
    EXPECT_EQ(withStructure, expected);
}

// ── Déterminisme ─────────────────────────────

TEST_F(MapGeneratorTest, SameSeed_ProducesSameMap) {
    const std::vector<House*> players1 = {&baratheon, &stark};
    const std::size_t n = MAP_BASE_PROVINCES + MAP_PROVINCES_PER_PLAYER * 2;

    std::mt19937 rng1{99};
    ThematicNamer namer1(namesWithStrongholds(players1, n));
    auto map1 = MapGenerator{topology, namer1}.generate(players1, rng1);

    House bar2{"Barathéon", makeHand()};
    House sta2{"Stark",     makeHand()};
    std::vector<House*> players2 = {&bar2, &sta2};

    std::mt19937 rng2{99};
    ThematicNamer namer2(namesWithStrongholds(players2, n));
    auto map2 = MapGenerator{topology, namer2}.generate(players2, rng2);

    EXPECT_EQ(map1.size(), map2.size());
    EXPECT_EQ(map1.provinceNames(), map2.provinceNames());
    for (const auto& name : map1.provinceNames())
        EXPECT_EQ(map1.neighbors(name).size(), map2.neighbors(name).size());
}
