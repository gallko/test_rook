#pragma once
#include <cinttypes>
#include <type_traits>
#include <utility>

using Coordinate = std::pair<std::int8_t, std::int8_t>;

template <class N, typename = std::enable_if_t< std::is_integral_v<N> >>
inline constexpr bool operator<=(const Coordinate &coord, N number)
{
    return coord.first <= number && coord.second <= number;
}

template <class N, typename = std::enable_if_t< std::is_integral_v<N> >>
inline constexpr bool operator>=(const Coordinate &coord, N number)
{
    return coord.first >= number && coord.second >= number;
}
