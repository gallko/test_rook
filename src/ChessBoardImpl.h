#pragma once

#include <array>
#include <mutex>

#include <IChessBoard.h>

class ChessBoardImpl : public board::IChessBoard {
public:
    ChessBoardImpl();
    ~ChessBoardImpl() override = default;

    board::ErrorCode moveFigure(const std::shared_ptr<chessman::IChessman> &figure, const Coordinate &to) override;
    board::ErrorCode placeFigure(const std::shared_ptr<chessman::IChessman> &figure, const Coordinate &to) override;
    board::ErrorCode removeFigure(const std::shared_ptr<chessman::IChessman> &figure) override;

    uint8_t sizeBoard() const noexcept override;

private:
    using Board_t = std::array<std::array<board::PlaceType, SIZE_BOARD>, SIZE_BOARD>;

    constexpr Board_t::value_type::const_reference getTypePlace(const Coordinate &coordinate) const;
    Board_t::value_type::reference getTypePlace(const Coordinate &coordinate);

    std::mutex mMutex;
    Board_t mBoard;
};
