#pragma once

#include <array>
#include <format>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

struct House {
    std::string_view name;
    HouseCardHand    cards;

    ~House()                = default;
    House(House&)           = delete;
    void operator=(House&)  = delete;
    House(House&&)          = delete;
    void operator=(House&&) = delete;
};

struct HouseCard {
    std::string_view name;
    const int        combatStrength = 0;
    const int        swords         = 0;
    const int        towers         = 0;

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
    HouseCardHand(HouseCardHand&&)  = delete;
    void operator=(HouseCardHand&&) = delete;

    const std::vector<HouseCard>& available() const { return availables_; }
    const std::vector<HouseCard>& used() const { return used_; }
    void                          discard(const HouseCard& card) {
        auto it = std::ranges::find(availables_, card);
        if (it == availables_.end())
            throw std::out_of_range(std::format("{} is not found in hand", card.name));
        if (availables_.size() > 1) {
            used_.push_back(card);
            availables_.erase(it);
        } else if (availables_.size() == 1) {
            std::swap(availables_, used_);
        } else {
            throw std::logic_error("Hand is empty!!");
        }
    };
};
