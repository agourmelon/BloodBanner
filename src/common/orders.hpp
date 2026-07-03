#pragma once

#include "common/helper/technical/overload.hpp"
#include "houses.hpp"
#include <string_view>
#include <variant>

// ─────────────────────────────────────────────
// Ordres
// ─────────────────────────────────────────────

struct MarchOrder {
    const House & owner;
};
struct DefenseOrder {
    const House & owner;
};
struct SupportOrder {
    const House & owner;
};
struct MusterOrder {
    const House & owner;
};
struct RaidOrder {
    const House & owner;
};

using Order = std::variant<MarchOrder, DefenseOrder, SupportOrder, MusterOrder, RaidOrder>;

// ─────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────

constexpr std::string_view orderTypeName(const Order & o) noexcept {
    return std::visit(
        common::helper::technical::overload{
            [](const MarchOrder &) -> std::string_view { return "Marche"; },
            [](const DefenseOrder &) -> std::string_view { return "Défense"; },
            [](const SupportOrder &) -> std::string_view { return "Soutien"; },
            [](const MusterOrder &) -> std::string_view { return "Recrutement"; },
            [](const RaidOrder &) -> std::string_view { return "Raid"; },
        },
        o
    );
}

constexpr bool isRaidable(const Order & o) noexcept {
    return std::holds_alternative<SupportOrder>(o) || std::holds_alternative<MusterOrder>(o);
}

const inline House & orderOwner(const Order & o) noexcept {
    return std::visit([](const auto & order) -> const House & { return order.owner; }, o);
}
