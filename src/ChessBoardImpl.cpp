#include "ChessBoardImpl.h"
#include "Coordinate.h"
#include "IChessman.h"
#include "utils.h"


ChessBoardImpl::ChessBoardImpl()
    : IChessBoard()
    , mBoard()
{

}

board::ErrorCode ChessBoardImpl::moveFigure(const std::shared_ptr<chessman::IChessman> &figure, const Coordinate &to)
{
    auto error_code = board::ErrorCode::error;
    auto lock = std::unique_lock(mMutex);
    auto &place_to = getTypePlace(to);

    if (place_to == board::PlaceType::empty)
    {
        auto &place_from = getTypePlace(figure->getCurrentCoordinate());
        auto fig_type = figure->type();
        if (place_from == fig_type)
        {
            place_to = utils::to_enum<board::PlaceType>(fig_type);
            place_from = board::PlaceType::empty;
            error_code = board::ErrorCode::success;
        }
    } else {
        error_code = board::ErrorCode::occupied_place;
    }

    return error_code;
}

board::ErrorCode ChessBoardImpl::placeFigure(const std::shared_ptr<chessman::IChessman> &figure, const Coordinate &to)
{
    auto error_code = board::ErrorCode::occupied_place;
    auto lock = std::unique_lock(mMutex);
    auto &place_to = getTypePlace(to);

    if (place_to == board::PlaceType::empty)
    {
        place_to = utils::to_enum<board::PlaceType>(figure->type());
        error_code = board::ErrorCode::success;
    }

    return error_code;
}

board::ErrorCode ChessBoardImpl::removeFigure(const std::shared_ptr<chessman::IChessman> &figure)
{
    auto error_code = board::ErrorCode::success;
    auto lock = std::unique_lock(mMutex);
    auto &place = getTypePlace(figure->getCurrentCoordinate());

    if (place == figure->type())
    {
        place = board::PlaceType::empty;
    } else {
        error_code = board::ErrorCode::error;
    }

    return error_code;
}

inline ChessBoardImpl::Board_t::value_type::reference ChessBoardImpl::getTypePlace(const Coordinate &coordinate)
{
    return mBoard.at(coordinate.mX).at(coordinate.mY);
}

inline constexpr ChessBoardImpl::Board_t::value_type::const_reference ChessBoardImpl::getTypePlace(const Coordinate &coordinate) const
{
    return mBoard.at(coordinate.mX).at(coordinate.mY);
}

uint8_t ChessBoardImpl::sizeBoard() const noexcept {
    return SIZE_BOARD;
}
