#pragma once

#include "common/houses.hpp"

inline HouseCardHand makeHand() {
    HouseCard c1{"Carte 1", 3, 1, 0};
    HouseCard c2{"Carte 2", 4, 0, 0};
    HouseCard c3{"Carte 3", 0, 0, 0};
    HouseCard c4{"Carte 4", 1, 1, 0};
    HouseCard c5{"Carte 5", 2, 0, 0};
    HouseCard c6{"Carte 6", 2, 0, 1};
    HouseCard c7{"Carte 7", 1, 0, 1};
    return HouseCardHand(std::array<HouseCard, 7>{c1, c2, c3, c4, c5, c6, c7});
}
