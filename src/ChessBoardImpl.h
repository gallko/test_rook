#pragma once

#include <array>
#include <mutex>
#include <list>
#include <condition_variable>
#include <utility>
#include <future>

#include "IChessBoard.h"
#include "TreadBase.h"

class IState;

class ChessBoardImpl : public board::IChessBoard, public TreadBase, std::enable_shared_from_this<ChessBoardImpl> {
public:
    ChessBoardImpl();
    ~ChessBoardImpl() override;

    bool moveFigure(const chessman::IChessMan &figure, const Coordinate &to,
                    std::shared_ptr<board::INotifier> notifier) override;
    bool placeFigure(const chessman::IChessMan &figure, const Coordinate &to,
                     std::shared_ptr<board::INotifier> notifier) override;
    bool removeFigure(const chessman::IChessMan &figure, std::shared_ptr<board::INotifier> notifier) override;

    uint8_t sizeBoard() const noexcept override;

    void start(pthread_barrier_t *barrier);
    void stop();

protected:
    void onStart() override;
    void loop() override;
    void onStop() override;

private:
    static constexpr std::int32_t sEmptyCell = 0 ;
    using Waiting_t = std::pair<std::int32_t, std::weak_ptr<board::INotifier>>;
    using Cell_t = std::pair<std::int32_t, std::list<Waiting_t>>;
    using Board_t = std::array<std::array<Cell_t, SIZE_BOARD>, SIZE_BOARD>;
    struct Task;
    enum class ReasonWeakUp;

    Board_t::value_type::const_reference getCell(const Coordinate &coordinate) const;
    Board_t::value_type::reference getCell(const Coordinate &coordinate);
    void do_task(std::unique_lock<std::mutex> &lock, const Task &task); // call protected by mMutex
    void do_place(std::unique_lock<std::mutex> &lock, std::int32_t id,
                  const Coordinate &to_coordinate, const std::shared_ptr<board::INotifier> &notifier); // call protected by mMutex

    void do_move(std::unique_lock<std::mutex> &lock, std::int32_t id,
                 const Coordinate &from_coordinate, const Coordinate &to_coordinate,
                 const std::shared_ptr<board::INotifier> &notifier); // call protected by mMutex

    void do_remove(std::unique_lock<std::mutex> &lock, std::int32_t id,
                   const Coordinate &from_coordinate,
                   const std::shared_ptr<board::INotifier> &notifier); // call protected by mMutex
    void do_check_waiting(std::unique_lock<std::mutex> &lock, const Coordinate &current_coordinate); // call protected by mMutex

    pthread_barrier_t *mBarrierStart;
    std::mutex mMutex;
    std::condition_variable mWait;
    ReasonWeakUp mReasonWeakUp;
    std::list<Task> mTaskList;

    Board_t mBoard;
};

struct ChessBoardImpl::Task {
    enum class Type {
        place, move, remove
    };
    Task(Type type,
         std::int32_t id,
         const Coordinate &from,
         const Coordinate &to,
         const std::shared_ptr<board::INotifier> &notifier)
         : mId(id)
         , mTypeTask(type)
         , mFromCoordinate(from)
         , mToCoordinate(to)
         , mNotifier(notifier)
    {

    }
    std::int32_t mId;
    Type mTypeTask;
    const Coordinate mFromCoordinate;
    const Coordinate mToCoordinate;
    std::weak_ptr<board::INotifier> mNotifier;
};

enum class ChessBoardImpl::ReasonWeakUp
{ // in order of importance
    stop, do_work, fake
};

inline ChessBoardImpl::Board_t::value_type::reference ChessBoardImpl::getCell(const Coordinate &coordinate)
{
    return mBoard.at(coordinate.first).at(coordinate.second);
}

inline ChessBoardImpl::Board_t::value_type::const_reference ChessBoardImpl::getCell(const Coordinate &coordinate) const
{
    return mBoard.at(coordinate.first).at(coordinate.second);
}
