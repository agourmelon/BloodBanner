#pragma once

#include "orders.hpp"
#include "units.hpp"
#include <cassert>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <format>

// ─────────────────────────────────────────────
// Structures
// ─────────────────────────────────────────────

struct Castle {
    static constexpr std::string_view name              = "Château";
    static constexpr int              recruitmentPoints = 1;
};

struct Stronghold {
    static constexpr std::string_view name              = "Forteresse";
    static constexpr int              recruitmentPoints = 2;
};

using Structure = std::variant<Castle, Stronghold>;

constexpr std::string_view structureName(const Structure& s) noexcept {
    return std::visit([](const auto& s) { return s.name; }, s);
}

constexpr int recruitmentPoints(const Structure& s) noexcept {
    return std::visit([](const auto& s) { return s.recruitmentPoints; }, s);
}

// ─────────────────────────────────────────────
// Province
// ─────────────────────────────────────────────

class Province {
    std::string                                        name_;
    std::optional<Structure>                           structure_;
    std::optional<std::reference_wrapper<const House>> controller_;
    std::vector<Unit>                                  units_;
    std::optional<Order>                               order_;
    std::vector<std::string>                           adjacentIds_;

  public:
    Province(std::string name, std::optional<Structure> structure = std::nullopt,
             std::vector<std::string> adjacentIds = {})
        : name_{std::move(name)}, structure_{std::move(structure)},
          adjacentIds_{std::move(adjacentIds)} {}

    // ── Getters ───────────────────────────────

    [[nodiscard]] const std::string&              name() const noexcept { return name_; }
    [[nodiscard]] const std::optional<Structure>& structure() const noexcept { return structure_; }
    [[nodiscard]] const std::vector<Unit>&        units() const noexcept { return units_; }
    [[nodiscard]] const std::optional<Order>&     order() const noexcept { return order_; }
    [[nodiscard]] const std::vector<std::string>& adjacentIds() const noexcept {
        return adjacentIds_;
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const House>> controller() const noexcept {
        return controller_;
    }

    // ── Unités ────────────────────────────────

    void addUnit(Unit u) {
        if (!units_.empty() && (unitOwner(u) != unitOwner(units_.front())))
            throw std::logic_error(
                std::format("Impossible de déplacer unité sur {} : province occupée", name_));
        if (units_.empty())
            controller_ = std::cref(unitOwner(u));
        units_.push_back(std::move(u));
    }

    void removeUnit(const Unit& u) { std::erase(units_, u); }

    // ── Contrôleur ────────────────────────────

    void setController(const House& h) {
        if (!units_.empty())
            throw std::logic_error(
                std::format("Impossible de définir le contrôleur de {} : province occupée", name_));
        controller_ = std::cref(h);
    }

    // ── Ordre ─────────────────────────────────

    void setOrder(Order o) {
        if (units_.empty())
            throw std::logic_error(
                std::format("Impossible de poser un ordre sur {} : province sans unités", name_));
        order_.emplace(std::move(o));
    }

    void clearOrder() { order_.reset(); }
};
