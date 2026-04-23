#pragma once

#include "houses.hpp"
#include <string_view>
#include <variant>

// ─────────────────────────────────────────────
// Ordres
// ─────────────────────────────────────────────

struct MarchOrder {
    const House& owner;
};
struct DefenseOrder {
    const House& owner;
};
struct SupportOrder {
    const House& owner;
};
struct MusterOrder {
    const House& owner;
};
struct RaidOrder {
    const House& owner;
};

using Order = std::variant<MarchOrder, DefenseOrder, SupportOrder, MusterOrder, RaidOrder>;

// ─────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────

constexpr std::string_view orderTypeName(const Order& o) noexcept {
    return std::visit(
        [](const auto& order) -> std::string_view {
            using T = std::decay_t<decltype(order)>;
            if constexpr (std::is_same_v<T, MarchOrder>)
                return "Marche";
            else if constexpr (std::is_same_v<T, DefenseOrder>)
                return "Défense";
            else if constexpr (std::is_same_v<T, SupportOrder>)
                return "Soutien";
            else if constexpr (std::is_same_v<T, MusterOrder>)
                return "Recrutement";
            else if constexpr (std::is_same_v<T, RaidOrder>)
                return "Raid";
        },
        o);
}

constexpr bool isRaidable(const Order& o) noexcept {
    return std::holds_alternative<SupportOrder>(o) || std::holds_alternative<MusterOrder>(o);
}

inline const House& orderOwner(const Order& o) noexcept {
    return std::visit([](const auto& order) -> const House& { return order.owner; }, o);
}
