#pragma once

#include <cassert>

constexpr inline bool operator==(const board::PlaceType &typePlace, const chessman::ChessmanType &chessmanType) noexcept
{
    switch (typePlace)
    {
        case board::PlaceType::empty:
            return false;
        case board::PlaceType::occupied_by_rook:
            return chessmanType == chessman::ChessmanType::rook;
#ifdef TEST_BUILD
        case board::PlaceType::test:
            return chessmanType == chessman::ChessmanType::test;
#endif
        default:
            assert(!"Incorrect value \"typePlace\"");
    }
}

constexpr inline bool operator!=(const board::PlaceType &typePlace, const chessman::ChessmanType &chessmanType) noexcept
{
    return !(typePlace == chessmanType);
}

constexpr inline bool operator==(const chessman::ChessmanType &chessmanType, const board::PlaceType &typePlace) noexcept
{
    return typePlace == chessmanType;
}

constexpr inline bool operator!=(const chessman::ChessmanType &chessmanType, const board::PlaceType &typePlace) noexcept
{
    return typePlace != chessmanType;
}

namespace utils {

template<typename To, typename From>
constexpr inline To to_enum(const From &type) noexcept;

template<>
constexpr inline board::PlaceType to_enum<board::PlaceType, chessman::ChessmanType>(const chessman::ChessmanType &type) noexcept {
    board::PlaceType place{};
    switch (type) {
        case chessman::ChessmanType::rook:
            place = board::PlaceType::occupied_by_rook;
            break;
#ifdef TEST_BUILD
        case chessman::ChessmanType::test:
            place = board::PlaceType::test;
            break;
#endif
        default:
            assert(!"Incorrect value \"typePlace\"");
    }
    return place;
}

}