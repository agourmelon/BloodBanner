#pragma once

#include "common/helper/technical/overload.hpp"
#include "houses.hpp"
#include <string_view>
#include <variant>

// ─────────────────────────────────────────────
// Ordres
// ─────────────────────────────────────────────

struct MarchOrder {
    House const & owner;
};
struct DefenseOrder {
    House const & owner;
};
struct SupportOrder {
    House const & owner;
};
struct MusterOrder {
    House const & owner;
};
struct RaidOrder {
    House const & owner;
};

using Order = std::variant<MarchOrder, DefenseOrder, SupportOrder, MusterOrder, RaidOrder>;

// ─────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────

constexpr std::string_view orderTypeName(Order const & o) noexcept {
    return std::visit(
        common::helper::technical::overload{
            [](MarchOrder const &) -> std::string_view { return "Marche"; },
            [](DefenseOrder const &) -> std::string_view { return "Défense"; },
            [](SupportOrder const &) -> std::string_view { return "Soutien"; },
            [](MusterOrder const &) -> std::string_view { return "Recrutement"; },
            [](RaidOrder const &) -> std::string_view { return "Raid"; },
        },
        o
    );
}

constexpr bool isRaidable(Order const & o) noexcept {
    return std::holds_alternative<SupportOrder>(o) || std::holds_alternative<MusterOrder>(o);
}

inline House const & orderOwner(Order const & o) noexcept {
    return std::visit([](auto const & order) -> House const & { return order.owner; }, o);
}
