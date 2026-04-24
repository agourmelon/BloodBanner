#pragma once

#include "houses.hpp"
#include "provinces.hpp"
#include <string>
#include <unordered_map>
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
    std::unordered_map<std::string, Province> provinces_;
    std::vector<std::string>                  orderedIds_;

  public:
    void addProvince(Province p) {
        const std::string id = p.name();
        orderedIds_.push_back(id);
        provinces_.emplace(id, std::move(p));
    }

    [[nodiscard]] Province& province(const std::string& id) {
        auto it = provinces_.find(id);
        if (it == provinces_.end())
            throw std::out_of_range(std::format("Province inconnue : {}", id));
        return it->second;
    }

    [[nodiscard]] const Province& province(const std::string& id) const {
        auto it = provinces_.find(id);
        if (it == provinces_.end())
            throw std::out_of_range(std::format("Province inconnue : {}", id));
        return it->second;
    }

    [[nodiscard]] bool hasProvince(const std::string& id) const { return provinces_.contains(id); }

    [[nodiscard]] const std::vector<std::string>& provinceNames() const noexcept {
        return orderedIds_;
    }

    [[nodiscard]] std::size_t size() const noexcept { return provinces_.size(); }

    // ── Helpers ───────────────────────────────

    [[nodiscard]] std::vector<const Province*> neighbors(const std::string& id) const {
        std::vector<const Province*> result;
        for (const auto& adjId : province(id).adjacentNames())
            result.push_back(&province(adjId));
        return result;
    }

    [[nodiscard]] bool areAdjacent(const std::string& a, const std::string& b) const {
        const auto& adj = province(a).adjacentNames();
        return std::ranges::find(adj, b) != adj.end();
    }

    void clearAllOrders() {
        for (auto& [id, p] : provinces_)
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
