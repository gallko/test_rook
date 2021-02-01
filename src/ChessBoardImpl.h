#pragma once

#include <array>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <future>
#include <set>
#include <vector>
#include <list>

#include "IGameElement.h"
#include "IChessBoard.h"
#include "TreadBase.h"

class IState;

class ChessBoardImpl
        : public board::IChessBoard
        , public IGameElement
        , public TreadBase
{
public:
    explicit ChessBoardImpl(std::uint8_t sizeBoard);
    ~ChessBoardImpl() override;

    void startGame() override;
    void stopGame() override;

    void addNotifier(std::shared_ptr<board::INotifier> notifier) override;
    void removeNotifier(std::shared_ptr<board::INotifier> notifier) override;

    void moveFigure(const chessman::IChessMan &figure, const board::Coordinate &to) override;
    void placeFigure(const chessman::IChessMan &figure, const board::Coordinate &to) override;
    void removeFigure(const chessman::IChessMan &figure) override;
    void cancelMoveFigure(const chessman::IChessMan &figure, const board::Coordinate &to) override;

    uint8_t sizeBoard() const noexcept override;

protected:
    void loop() override;
    void onStop() override;

private:
    using Waiting_t = std::pair<std::uint32_t /* id */, board::Coordinate /* from */>;
    using Cell_t = std::pair<std::uint32_t /* sEmptyCell/id */, std::list<Waiting_t>>;
    using Row_t = std::vector<Cell_t>;
    using Board_t = std::vector<Row_t>;
    struct Task;
    enum class ReasonWeakUp;

    Board_t::value_type::const_reference getCell(const board::Coordinate &coordinate) const;
    Board_t::value_type::reference getCell(const board::Coordinate &coordinate);
    void do_task(const Task &task);
    void do_place(std::uint32_t id, const board::Coordinate &to_coordinate);
    void do_move(std::uint32_t id, const board::Coordinate &from_coordinate, const board::Coordinate &to_coordinate);
    void do_cancel_move(std::uint32_t id, const board::Coordinate &from_coordinate, const board::Coordinate &to_coordinate);
    void do_remove(std::uint32_t id, const board::Coordinate &from_coordinate);
    void do_check_waiting(const board::Coordinate &current_coordinate);

    template<typename Func, typename... Args>
    void notifyAll(Func &&func, Args&&... args) const;

    std::mutex mMutexTasks;
    std::condition_variable mWait;
    ReasonWeakUp mReasonWeakUp;
    std::list<Task> mTaskList;

    mutable std::recursive_mutex mMutexNotifier;
    std::vector<std::shared_ptr<board::INotifier>> mListNotifiers;

    Board_t mBoard;
    std::set<std::uint32_t> mIds;
};

struct ChessBoardImpl::Task {
    enum class Type {
        place, move, cancelMove, remove
    };
    Task(Type type,
         std::uint32_t id,
         board::Coordinate from,
         board::Coordinate to)
         : mId(id)
         , mTypeTask(type)
         , mFromCoordinate(std::move(from))
         , mToCoordinate(std::move(to))
    {

    }
    std::uint32_t mId;
    Type mTypeTask;
    const board::Coordinate mFromCoordinate;
    const board::Coordinate mToCoordinate;
};

enum class ChessBoardImpl::ReasonWeakUp
{ // in order of importance
    exit, stop, do_work, fake,
};

inline ChessBoardImpl::Board_t::value_type::reference ChessBoardImpl::getCell(const board::Coordinate &coordinate)
{
    return mBoard.at(coordinate.first).at(coordinate.second);
}

inline ChessBoardImpl::Board_t::value_type::const_reference ChessBoardImpl::getCell(const board::Coordinate &coordinate) const
{
    return mBoard.at(coordinate.first).at(coordinate.second);
}
