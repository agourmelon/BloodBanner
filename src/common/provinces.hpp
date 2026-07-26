#pragma once

#include "common/houses.hpp"
#include "orders.hpp"
#include "units.hpp"
#include <functional>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>

// ─────────────────────────────────────────────
// Structures
// ─────────────────────────────────────────────

struct Castle {
    constexpr static std::string_view name              = "Château";
    constexpr static int              recruitmentPoints = 1;
};

struct Stronghold {
    constexpr static std::string_view name              = "Forteresse";
    constexpr static int              recruitmentPoints = 2;
};

using Structure = std::variant<Castle, Stronghold>;

constexpr std::string_view structureName(const Structure & s) noexcept {
    return std::visit([](const auto & s) { return s.name; }, s);
}

constexpr int recruitmentPoints(const Structure & s) noexcept {
    return std::visit([](const auto & s) { return s.recruitmentPoints; }, s);
}

// ─────────────────────────────────────────────
// Province
// ─────────────────────────────────────────────

class Province {
    std::optional<Structure>                           structure_;
    std::optional<std::reference_wrapper<const House>> controller_;
    std::vector<Unit>                                  units_;
    std::optional<Order>                               order_;

public:

    explicit Province(std::optional<Structure> structure = std::nullopt)
        : structure_{structure} {}
    // ── Getters ───────────────────────────────

    [[nodiscard]] const std::optional<Structure> & structure() const noexcept { return structure_; }
    [[nodiscard]] const std::vector<Unit> &        units() const noexcept { return units_; }
    [[nodiscard]] const std::optional<Order> &     order() const noexcept { return order_; }
    [[nodiscard]] std::optional<std::reference_wrapper<const House>> controller() const noexcept {
        return controller_;
    }

    // ── Unités ────────────────────────────────

    void addUnit(Unit u) {
        if (units_.empty())
            controller_ = std::cref(unitOwner(u));
        units_.push_back(u);
    }

    void removeUnit(const Unit & u) { std::erase(units_, u); }

    // ── Contrôleur ────────────────────────────

    void setController(const House & h) { controller_ = std::cref(h); }

    void clearController() { controller_.reset(); }

    // ── Ordre ─────────────────────────────────

    void setOrder(Order o) { order_.emplace(o); }
    void clearOrder() { order_.reset(); }
};
