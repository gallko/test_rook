#pragma once
#include <cinttypes>
#include <type_traits>
#include <utility>

namespace board
{
using Coordinate = std::pair<std::int8_t /* x */, std::int8_t /* y */>;
inline constexpr Coordinate invalidCoordinate{-1, -1};
}

template <class N, typename = std::enable_if_t< std::is_integral_v<N> >>
inline constexpr bool operator<(const board::Coordinate &coord, N number)
{
    return coord.first < number || coord.second < number;
}

template <class N, typename = std::enable_if_t< std::is_integral_v<N> >>
inline constexpr bool operator>(const board::Coordinate &coord, N number)
{
    return coord.first > number || coord.second > number;
}

template <class N, typename = std::enable_if_t< std::is_integral_v<N> >>
inline constexpr bool operator>=(const board::Coordinate &coord, N number)
{
    return coord.first >= number || coord.second >= number;
}

template <class N, typename = std::enable_if_t< std::is_integral_v<N> >>
inline constexpr bool operator<=(const board::Coordinate &coord, N number)
{
    return coord.first <= number || coord.second <= number;
}
