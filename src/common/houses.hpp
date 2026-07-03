#pragma once

#include <algorithm>
#include <array>
#include <format>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

struct HouseCard {
    std::string_view name;
    int              combatStrength = 0;
    int              swords         = 0;
    int              towers         = 0;

    bool operator==(const HouseCard&) const = default;
};

class HouseCardHand {
    std::vector<HouseCard> availables_;
    std::vector<HouseCard> used_;

  public:
    HouseCardHand(std::array<HouseCard, 7>&& cards)
        : availables_{std::make_move_iterator(std::begin(cards)),
                      std::make_move_iterator(std::end(cards))},
          used_{} {}

    ~HouseCardHand()                = default;
    HouseCardHand(HouseCardHand&)   = delete;
    void operator=(HouseCardHand&)  = delete;
    HouseCardHand(HouseCardHand&&)  = default;
    void operator=(HouseCardHand&&) = delete;

    const std::vector<HouseCard>& available() const { return availables_; }
    const std::vector<HouseCard>& used() const { return used_; }
    void                          discard(const HouseCard& card) {
        auto it = std::ranges::find(availables_, card);
        if (it == availables_.end())
            throw std::out_of_range(std::format("{} is not found in hand", card.name));
        availables_.erase(it);
        if (availables_.size() > 0) {
            used_.push_back(card);
        } else if (availables_.size() == 0) {
            availables_ = std::move(used_);
            used_.push_back(card);
        } else {
            throw std::logic_error("Hand is empty!!");
        }
    };
};

struct House {
    std::string_view name;
    HouseCardHand    cards;

    House(std::string_view n, HouseCardHand&& c) : name{n}, cards{std::move(c)} {}

    ~House()                = default;
    House(House&)           = delete;
    void operator=(House&)  = delete;
    House(House&&)          = default;
    void operator=(House&&) = delete;

    bool operator==(const House& h) const { return name == h.name; }
};
