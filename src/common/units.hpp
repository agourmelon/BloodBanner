#pragma once

#include "houses.hpp"
#include <string_view>
#include <variant>

// ─────────────────────────────────────────────
// Unités
// ─────────────────────────────────────────────
struct UnitBase {
    const House* owner;
    bool         retreating = false;

    bool operator==(const UnitBase& o) const noexcept {
        return owner == o.owner && retreating == o.retreating;
    }
};

struct Footman : UnitBase {
    static constexpr std::string_view name            = "Fantassin";
    static constexpr int              recruitmentCost = 1;
    static constexpr int              combatStrength  = 1;

    bool operator==(const Footman& o) const = default;
};

struct Knight : UnitBase {
    static constexpr std::string_view name            = "Chevalier";
    static constexpr int              recruitmentCost = 2;
    static constexpr int              combatStrength  = 2;

    bool operator==(const Knight& o) const = default;
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

inline const House& unitOwner(const Unit& u) noexcept {
    return *std::visit([](const auto& unit) -> const House* { return unit.owner; }, u);
}

inline bool isRetreating(const Unit& u) noexcept {
    return std::visit([](const auto& unit) -> bool { return unit.retreating; }, u);
}
