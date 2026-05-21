#pragma once
namespace common::helper::technical {
    template <typename... Ts>
    struct overload : Ts... {
        using Ts::operator()...;
    };
    template <class... Ts>
    overload(Ts...) -> overload<Ts...>;

} // namespace common::helper::technical
