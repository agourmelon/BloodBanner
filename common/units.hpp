#pragma once

#include "houses.hpp"
#include <cstdint>
#include <string_view>
#include <variant>

// ─────────────────────────────────────────────
// Unités
// ─────────────────────────────────────────────

struct Footman {
    static constexpr std::string_view name            = "Fantassin";
    static constexpr int              recruitmentCost = 1;
    static constexpr int              combatStrength  = 1;

    const House& owner;
    bool         retreating = false;
};

struct Knight {
    static constexpr std::string_view name            = "Chevalier";
    static constexpr int              recruitmentCost = 2;
    static constexpr int              combatStrength  = 2;

    const House& owner;
    bool         retreating = false;
};

using Unit = std::variant<Footman, Knight>;

// ─────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────

constexpr std::string_view unitName(const Unit& u) noexcept {
    return std::visit([](const auto& unit) { return unit.name; }, u);
}

constexpr int recruitmentCost(const Unit& u) noexcept {
    return std::visit([](const auto& unit) { return unit.recruitmentCost; }, u);
}

constexpr int combatStrength(const Unit& u) noexcept {
    return std::visit(
        [](const auto& unit) -> int { return unit.retreating ? 0 : unit.combatStrength; }, u);
}

const House& unitOwner(const Unit& u) noexcept {
    return std::visit([](const auto& unit) -> const House& { return unit.owner; }, u);
}

bool isRetreating(const Unit& u) noexcept {
    return std::visit([](const auto& unit) -> bool { return unit.retreating; }, u);
}
