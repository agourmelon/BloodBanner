#pragma once

#include "houses.hpp"
#include "provinces.hpp"
#include <cstddef>
#include <functional>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// ─────────────────────────────────────────────
// Joueur
// ─────────────────────────────────────────────

struct Player {
    std::reference_wrapper<const House> house;
    int                                 victoryPoints = 0;
};

// ─────────────────────────────────────────────
// Carte de jeu
// ─────────────────────────────────────────────

class GameMap {
    std::unordered_map<std::string_view, Province>                             provinces_;
    std::unordered_map<std::string_view, std::unordered_set<std::string_view>> borders_;

public:

    void addProvince(std::string_view name, Province && p) {
        if (!provinces_.try_emplace(name, std::move(p)).second) {
            throw std::invalid_argument("A province with the same name already exists");
        };
        borders_.emplace(name, std::unordered_set<std::string_view>());
    }

    void linkProvinces(std::string_view name1, std::string_view name2) {
        borders_.at(name1).emplace(name2);
        borders_.at(name2).emplace(name1);
    }

    [[nodiscard]] Province & getProvince(std::string_view name) { return provinces_.at(name); }

    [[nodiscard]] bool hasProvince(std::string_view name) const {
        return provinces_.contains(name);
    }

    [[nodiscard]] std::vector<std::string_view> getAllProvinceNames() const noexcept {
        return provinces_ | std::views::keys | std::ranges::to<std::vector>();
    }

    [[nodiscard]] std::size_t size() const noexcept { return provinces_.size(); }

    // ── Helpers ───────────────────────────────

    [[nodiscard]] std::unordered_set<std::string_view> getAdjacentProvinceNames(
        std::string_view name
    ) const {
        return borders_.at(name);
    }

    [[nodiscard]] bool areProvincesAdjacent(std::string_view name1, std::string_view name2) const {
        return borders_.at(name1).contains(name2);
    }

    void clearAllOrders() {
        for (auto & [id, p] : provinces_)
            p.clearOrder();
    }
};

// ─────────────────────────────────────────────
// État du jeu
// ─────────────────────────────────────────────

struct GameState {
    GameMap             map;
    std::vector<Player> players;
    int                 turnNumber = 0;
};
