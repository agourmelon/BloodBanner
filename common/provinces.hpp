#pragma once

#include "orders.hpp"
#include "units.hpp"
#include <cassert>
#include <format>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

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
    std::string                                        id_;
    std::string                                        name_;
    std::optional<Structure>                           structure_;
    std::optional<std::reference_wrapper<const House>> controller_;
    std::vector<Unit>                                  units_;
    std::optional<Order>                               order_;
    std::vector<std::string>                           adjacentIds_;

  public:
    Province(std::string id, std::string name, std::optional<Structure> structure = std::nullopt,
             std::vector<std::string> adjacentIds = {})
        : id_{std::move(id)}, name_{std::move(name)}, structure_{std::move(structure)},
          adjacentIds_{std::move(adjacentIds)} {}

    // ── Getters ───────────────────────────────

    [[nodiscard]] const std::string&              id() const noexcept { return id_; }
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
        assert(units_.empty() || unitOwner(u) == unitOwner(units_.front()));
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
        order_ = std::move(o);
    }

    void clearOrder() { order_.reset(); }
};
