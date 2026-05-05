#pragma once

#include "common/game_state.hpp"
#include <cstddef>
#include <random>
#include <vector>

// ─────────────────────────────────────────────
// Interface de topologie (plugin)
// ─────────────────────────────────────────────

struct IMapTopologyStrategy {
    // Retourne une liste d'adjacences pour N provinces
    // adjacencies[i] = liste des indices des provinces voisines de i
    virtual std::vector<std::vector<size_t>> buildAdjacencies(
        size_t          numProvinces,
        std::mt19937& rng
    ) const = 0;

    virtual ~IMapTopologyStrategy() = default;
};

// ─────────────────────────────────────────────
// Implémentation grille hexagonale
// ─────────────────────────────────────────────

struct HexGridStrategy : IMapTopologyStrategy {
    std::vector<std::vector<size_t>> buildAdjacencies(
        size_t          numProvinces,
        std::mt19937& rng
    ) const override;
};

// ─────────────────────────────────────────────
// Interface de nommage (plugin)
// ─────────────────────────────────────────────

struct IProvinceNamer {
    // Retourne le prochain nom disponible
    virtual std::string next() = 0;
    virtual ~IProvinceNamer() = default;
};

// Noms génériques : "Province-01", "Province-02", ...
struct DefaultNamer : IProvinceNamer {
    std::string next() override;
private:
    size_t counter_ = 0;
};

// Noms thématiques avec fallback générique si la liste est épuisée
struct ThematicNamer : IProvinceNamer {
    explicit ThematicNamer(std::vector<std::string> names);
    std::string next() override;
private:
    std::vector<std::string> names_;
    size_t                   counter_ = 0;
    DefaultNamer             fallback_;
};

// ─────────────────────────────────────────────
// Générateur de carte
// ─────────────────────────────────────────────

class MapGenerator {
    const IMapTopologyStrategy& topology_;
    IProvinceNamer&             namer_;

public:
    MapGenerator(const IMapTopologyStrategy& topology, IProvinceNamer& namer);

    // Génère une carte proportionnée au nombre de joueurs,
    // place les forteresses de départ et les unités initiales.
    GameMap generate(
        const std::vector<House*>& activePlayers,
        std::mt19937&              rng
    ) const;

private:
    // Génère les provinces avec leurs noms et structures
    std::vector<Province> buildProvinces(size_t numProvinces, std::mt19937& rng) const;

    // Place les forteresses de départ pour chaque maison
    void placeStartingStrongholds(
        GameMap&                   map,
        const std::vector<House*>& activePlayers
    ) const;

    // Place les unités de départ sur les forteresses et provinces adjacentes
    void placeStartingUnits(
        GameMap&                   map,
        const std::vector<House*>& activePlayers,
        std::mt19937&              rng
    ) const;

    // Assigne les structures (châteaux/forteresses) aux provinces
    void assignStructures(
        std::vector<Province>& provinces,
        std::mt19937&          rng
    ) const;
};
