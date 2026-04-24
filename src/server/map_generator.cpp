#include "map_generator.hpp"
#include "common/constants.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>
#include <numeric>
#include <stdexcept>

// ─────────────────────────────────────────────
// Détails d'implémentation
// ─────────────────────────────────────────────

namespace {

struct CubeCoord {
    int q, r, s;
    bool operator==(const CubeCoord&) const = default;
};

inline constexpr std::array<CubeCoord, 6> CUBE_DIRECTIONS = {{
    { 1, -1,  0}, { 1,  0, -1}, { 0,  1, -1},
    {-1,  1,  0}, {-1,  0,  1}, { 0, -1,  1},
}};

std::vector<CubeCoord> hexagonalGrid(int radius) {
    std::vector<CubeCoord> coords;
    for (int q = -radius; q <= radius; ++q) {
        const int r1 = std::max(-radius, -q - radius);
        const int r2 = std::min( radius, -q + radius);
        for (int r = r1; r <= r2; ++r)
            coords.push_back({q, r, -q - r});
    }
    return coords;
}

int radiusForProvinces(std::size_t numProvinces) {
    int radius = 0;
    while (static_cast<std::size_t>(3*radius*radius + 3*radius + 1) < numProvinces)
        ++radius;
    return radius;
}

std::string strongholdOf(const House& house) {
    const auto it = std::ranges::find_if(HOUSE_STRONGHOLDS,
        [&](const auto& entry) { return entry.first == house.name; }
    );
    if (it == HOUSE_STRONGHOLDS.end())
        throw std::runtime_error(
            std::format("Forteresse introuvable pour la maison {}", house.name)
        );
    return std::string{it->second};
}

} // namespace

// ─────────────────────────────────────────────
// HexGridStrategy
// ─────────────────────────────────────────────

std::vector<std::vector<std::size_t>> HexGridStrategy::buildAdjacencies(
    std::size_t   numProvinces,
    std::mt19937& rng
) const {
    std::ignore = rng;

    const int radius  = radiusForProvinces(numProvinces);
    const auto coords = hexagonalGrid(radius);

    auto indexOf = [&](const CubeCoord& c) -> std::optional<std::size_t> {
        for (std::size_t i = 0; i < std::min(coords.size(), numProvinces); ++i)
            if (coords[i] == c) return i;
        return std::nullopt;
    };

    std::vector<std::vector<std::size_t>> adjacencies(numProvinces);

    for (std::size_t i = 0; i < numProvinces; ++i) {
        for (const auto& dir : CUBE_DIRECTIONS) {
            const CubeCoord neighbor{
                coords[i].q + dir.q,
                coords[i].r + dir.r,
                coords[i].s + dir.s
            };
            if (const auto ni = indexOf(neighbor))
                adjacencies[i].push_back(*ni);
        }
    }

    return adjacencies;
}

// ─────────────────────────────────────────────
// DefaultNamer
// ─────────────────────────────────────────────

std::string DefaultNamer::next() {
    return std::format("Province-{:02d}", ++counter_);
}

// ─────────────────────────────────────────────
// ThematicNamer
// ─────────────────────────────────────────────

ThematicNamer::ThematicNamer(std::vector<std::string> names)
    : names_{std::move(names)}
{}

std::string ThematicNamer::next() {
    if (counter_ < static_cast<size_t>(names_.size()))
        return names_[counter_++];
    return fallback_.next();
}

// ─────────────────────────────────────────────
// MapGenerator
// ─────────────────────────────────────────────

MapGenerator::MapGenerator(const IMapTopologyStrategy& topology, IProvinceNamer& namer)
    : topology_{topology}
    , namer_   {namer}
{}

GameMap MapGenerator::generate(
    const std::vector<House*>& activePlayers,
    std::mt19937&              rng
) const {
    if (activePlayers.size() < static_cast<std::size_t>(MIN_PLAYERS) ||
        activePlayers.size() > static_cast<std::size_t>(MAX_PLAYERS))
        throw std::invalid_argument(
            std::format("Nombre de joueurs invalide : {}", activePlayers.size())
        );

    const std::size_t numProvinces =
        MAP_BASE_PROVINCES + MAP_PROVINCES_PER_PLAYER * activePlayers.size();

    // 1. Construit les provinces avec noms et structures
    auto provinces = buildProvinces(numProvinces, rng);

    // 2. Calcule les adjacences via la stratégie choisie
    const auto adjacencies = topology_.buildAdjacencies(numProvinces, rng);

    // 3. Assigne les adjacences aux provinces
    for (std::size_t i = 0; i < numProvinces; ++i)
        for (const auto ni : adjacencies[i])
            provinces[i].addAdjacentName(provinces[ni].name());

    // 4. Construit la GameMap
    GameMap map;
    for (auto& p : provinces)
        map.addProvince(std::move(p));

    // 5. Place les forteresses de départ
    placeStartingStrongholds(map, activePlayers);

    // 6. Place les unités de départ
    placeStartingUnits(map, activePlayers, rng);

    return map;
}

std::vector<Province> MapGenerator::buildProvinces(
    std::size_t   numProvinces,
    std::mt19937& rng
) const {
    std::vector<Province> provinces;
    provinces.reserve(numProvinces);

    const auto numStructures =
        static_cast<std::size_t>(static_cast<float>(numProvinces) * MAP_STRUCTURE_RATIO);

    std::vector<std::size_t> indices(numProvinces);
    std::iota(indices.begin(), indices.end(), 0);
    std::ranges::shuffle(indices, rng);
    indices.resize(numStructures);

    const auto numCastles =
        static_cast<std::size_t>(static_cast<float>(numStructures) * MAP_CASTLE_RATIO);

    for (std::size_t i = 0; i < numProvinces; ++i) {
        std::optional<Structure> structure = std::nullopt;

        const auto it = std::ranges::find(indices, i);
        if (it != indices.end()) {
            const auto structIdx =
                static_cast<std::size_t>(std::distance(indices.begin(), it));
            structure = (structIdx < numCastles)
                ? Structure{Castle{}}
                : Structure{Stronghold{}};
        }

        provinces.emplace_back(namer_.next(), std::move(structure));
    }

    return provinces;
}

void MapGenerator::placeStartingStrongholds(
    GameMap&                   map,
    const std::vector<House*>& activePlayers
) const {
    for (const auto& house : activePlayers) {
        const auto name = strongholdOf(*house);

        if (!map.hasProvince(name))
            throw std::runtime_error(
                std::format("Province introuvable : {}", name)
            );

        map.province(name).setController(*house);
    }
}

void MapGenerator::placeStartingUnits(
    GameMap&                   map,
    const std::vector<House*>& activePlayers,
    std::mt19937&              rng
) const {
    for (const auto& house : activePlayers) {
        const auto strongholdName = strongholdOf(*house);
        auto& stronghold          = map.province(strongholdName);

        // Unités sur la forteresse de départ
        for (int f = 0; f < STARTING_FOOTMEN_ON_STRONGHOLD; ++f)
            stronghold.addUnit(Footman{house});
        for (int k = 0; k < STARTING_KNIGHTS_ON_STRONGHOLD; ++k)
            stronghold.addUnit(Knight{house});

        // Unités sur les provinces adjacentes
        auto neighbors = map.neighbors(strongholdName);
        std::ranges::shuffle(neighbors, rng);

        const std::size_t adjCount = std::min(
            static_cast<std::size_t>(STARTING_ADJACENT_PROVINCES),
            neighbors.size()
        );

        for (std::size_t j = 0; j < adjCount; ++j)
            map.province(neighbors[j]->name()).addUnit(Footman{house});
    }
}
