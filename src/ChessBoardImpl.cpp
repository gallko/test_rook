#include <iostream>
#include <functional>
#include "ChessBoardImpl.h"
#include "Coordinate.h"
#include "IChessMan.h"
#include "GameRules.h"

using namespace board;

ChessBoardImpl::ChessBoardImpl(std::uint8_t sizeBoard)
    : IChessBoard()
    , TreadBase("ChessBoardImpl")
    , mMutexTasks()
    , mWait()
    , mReasonWeakUp(ReasonWeakUp::do_work)
    , mTaskList()
    , mMutexNotifier()
    , mListNotifiers()
    , mBoard(sizeBoard, Row_t(sizeBoard, {sEmptyCell, {}}))
    , mIds()
{

}

ChessBoardImpl::~ChessBoardImpl()
{
    {
        std::unique_lock lock(mMutexTasks);
        mReasonWeakUp = ReasonWeakUp::exit;
        mWait.notify_all();
    }
    TreadBase::join();
}

/* ************************************************************
 * IMPL IGameElement
 * ************************************************************/
void ChessBoardImpl::startGame()
{
    TreadBase::start();
}

void ChessBoardImpl::stopGame()
{
    std::unique_lock lock(mMutexTasks);
    mReasonWeakUp = ReasonWeakUp::stop;
    mWait.notify_all();
}

/* ************************************************************
 * IMPL board::IChessBoard
 * ************************************************************/
void ChessBoardImpl::addNotifier(std::shared_ptr<board::INotifier> notifier)
{
    std::lock_guard lock(mMutexNotifier);
    mListNotifiers.emplace_back(std::move(notifier));
}

void ChessBoardImpl::removeNotifier(std::shared_ptr<board::INotifier> notifier)
{
    std::lock_guard lock(mMutexNotifier);
    auto it = std::remove(mListNotifiers.begin(), mListNotifiers.end(), notifier);
    mListNotifiers.erase(it, mListNotifiers.end());
}

void ChessBoardImpl::placeFigure(const chessman::IChessMan &figure, const Coordinate &to)
{
    std::lock_guard lock(mMutexTasks);
    mTaskList.emplace_back(Task::Type::place, figure.getID(), Coordinate(), to);
    mReasonWeakUp = ReasonWeakUp::do_work;
    mWait.notify_all();
}

void ChessBoardImpl::moveFigure(const chessman::IChessMan &figure,
                                const Coordinate &to)
{
    auto &from_coordinate = figure.getCurrentCoordinate();
    std::lock_guard lock(mMutexTasks);
    mTaskList.emplace_back(Task::Type::move, figure.getID(), from_coordinate, to);
    mReasonWeakUp = ReasonWeakUp::do_work;
    mWait.notify_all();
}

void ChessBoardImpl::removeFigure(const chessman::IChessMan &figure)
{
    std::lock_guard lock(mMutexTasks);
    mTaskList.emplace_back(Task::Type::remove, figure.getID(), figure.getCurrentCoordinate(), Coordinate());
    mReasonWeakUp = ReasonWeakUp::do_work;
    mWait.notify_all();
}

void ChessBoardImpl::cancelMoveFigure(const chessman::IChessMan &figure, const Coordinate &to)
{
    auto &from_coordinate = figure.getCurrentCoordinate();
    std::lock_guard lock(mMutexTasks);
    mTaskList.emplace_back(Task::Type::cancelMove, figure.getID(), from_coordinate, to);
    mReasonWeakUp = ReasonWeakUp::do_work;
    mWait.notify_all();
}

/* ************************************************************
 * IMPL TreadBase
 * ************************************************************/
void ChessBoardImpl::loop()
{
    std::unique_lock lock(mMutexTasks);
    while (mReasonWeakUp != ReasonWeakUp::stop && mReasonWeakUp != ReasonWeakUp::exit)
    {
        while (!mTaskList.empty())
        {
            auto task = std::move(mTaskList.front());
            mTaskList.pop_front();
            lock.unlock();
            do_task(task);
            lock.lock();
        }
        mReasonWeakUp = ReasonWeakUp::fake;
        mWait.wait(lock, [this]() {
            return mReasonWeakUp != ReasonWeakUp::fake;
        });
    }
}

void ChessBoardImpl::onStop()
{
    auto notify = [this](std::uint32_t id) {
        notifyAll(&board::INotifier::reject, id, board::ReasonReject::boardStopped);
    };
    std::for_each(mIds.begin(), mIds.end(), notify);

    std::unique_lock lock(mMutexTasks);
    mTaskList.clear();
    while (mReasonWeakUp != ReasonWeakUp::exit)
    {
        while (!mTaskList.empty())
        {
            auto &task = mTaskList.front();
            mTaskList.pop_front();
            lock.unlock();
            notify(task.mId);
            lock.lock();
        }
        mReasonWeakUp = ReasonWeakUp::fake;
        mWait.wait(lock, [this]() {
            return mReasonWeakUp != ReasonWeakUp::fake;
        });
    }
    mTaskList.clear();
    TreadBase::onStop();
}

uint8_t ChessBoardImpl::sizeBoard() const noexcept
{
    return GameRules::sizeBoard();
}


/* ************************************************************
 * private
 * ************************************************************/
void ChessBoardImpl::do_task(const ChessBoardImpl::Task &task)
{
    if (task.mId != sEmptyCell)
    {
        switch (task.mTypeTask) {
            case Task::Type::place:
                do_place(task.mId, task.mToCoordinate);
                break;
            case Task::Type::move:
                do_move(task.mId, task.mFromCoordinate, task.mToCoordinate);
                break;
            case Task::Type::remove:
                do_remove(task.mId, task.mFromCoordinate);
                break;
            case Task::Type::cancelMove:
                do_cancel_move(task.mId, task.mFromCoordinate, task.mToCoordinate);
                break;
        }
    } else {
        notifyAll(&board::INotifier::reject, task.mId, board::ReasonReject::incorrectId);
    }
}

void ChessBoardImpl::do_place(std::uint32_t id, const Coordinate &to_coordinate)
{
    using namespace board;

    if (mIds.find(id) == mIds.end())
    {
        try {
            auto &to_cell = getCell(to_coordinate);
            if (to_cell.first == sEmptyCell) {
                to_cell.first = id;
                mIds.insert(id);
                notifyAll(&INotifier::placed, id, to_coordinate);
            } else {
                to_cell.second.emplace_back(id, invalidCoordinate);
                mIds.insert(id);
                notifyAll(&INotifier::waitingForCell, id, invalidCoordinate, to_coordinate);
            }
        } catch (std::out_of_range &) {
            notifyAll(&INotifier::reject, id, board::ReasonReject::incorrectCoordinate);
        }
    } else {
        notifyAll(&INotifier::reject, id, board::ReasonReject::duplicateId);
    }
}

void ChessBoardImpl::do_move(std::uint32_t id, const Coordinate &from_coordinate, const Coordinate &to_coordinate)
{
    using namespace board;

    try {
        auto &from_cell = getCell(from_coordinate);
        auto &to_cell = getCell(to_coordinate);
        if (from_cell.first == id) {
            if (to_cell.first == sEmptyCell) {
                from_cell.first = sEmptyCell;
                to_cell.first = id;
                notifyAll(&INotifier::moved, id, from_coordinate, to_coordinate);
                do_check_waiting(from_coordinate);
            } else {
                to_cell.second.emplace_back(id, from_coordinate);
                notifyAll(&INotifier::waitingForCell, id, from_coordinate, to_coordinate);
            }
        } else {
            notifyAll(&INotifier::reject, id, board::ReasonReject::idMismatch);
        }
    } catch (std::out_of_range &)  {
        notifyAll(&INotifier::reject, id, board::ReasonReject::incorrectCoordinate);
    }
}

void ChessBoardImpl::do_cancel_move(std::uint32_t id, const Coordinate &from_coordinate, const Coordinate &to_coordinate)
{
    using namespace board;
    try {
        auto &from_cell = getCell(from_coordinate);
        auto &to_cell = getCell(to_coordinate);
        if (from_cell.first == id) {
            auto &wait_list = to_cell.second;
            auto it = std::find_if(wait_list.begin(), wait_list.end(), [&](auto &item) {
                return item.first == id && item.second == from_coordinate;
            });
            if (it != wait_list.end())
            {
                wait_list.erase(it);
                notifyAll(&INotifier::cancelMoved, id, from_coordinate, to_coordinate);
            } else {
                notifyAll(&INotifier::reject, id, board::ReasonReject::waiterNotFound);
            }
        } else {
            notifyAll(&INotifier::reject, id, board::ReasonReject::idMismatch);
        }
    } catch (std::out_of_range &)  {
        notifyAll(&INotifier::reject, id, board::ReasonReject::incorrectCoordinate);
    }
}

void ChessBoardImpl::do_remove(std::uint32_t id, const Coordinate &from_coordinate)
{
    using namespace board;

    try {
        auto &from_cell = getCell(from_coordinate);
        if (from_cell.first == id) {
            from_cell.first = sEmptyCell;
            mIds.erase(id);
            notifyAll(&INotifier::removed, id, from_coordinate);
            do_check_waiting(from_coordinate);
        } else {
            notifyAll(&INotifier::reject, id, board::ReasonReject::idMismatch);
        }
    } catch (std::out_of_range &) {
        notifyAll(&INotifier::reject, id, board::ReasonReject::incorrectCoordinate);
    }
}

void ChessBoardImpl::do_check_waiting(const Coordinate &current_coordinate)
{
    auto &to_cell = getCell(current_coordinate);
    auto &waiting_list = to_cell.second;

    bool flag = true;
    while (!waiting_list.empty() && flag)
    {
        auto &wait_element = waiting_list.front();
        if (mIds.find(wait_element.first) != mIds.end())
        {
            if (wait_element.second != invalidCoordinate)
            {
                flag = false;
                do_move(wait_element.first, wait_element.second, current_coordinate);
            } else {
                to_cell.first = wait_element.first;
                notifyAll(&INotifier::placed, wait_element.first, current_coordinate);
                flag = false;
            }
        }
        waiting_list.pop_front();
    }
}

template<typename Func, typename... Args>
void ChessBoardImpl::notifyAll(Func &&func, Args&&... args) const
{
    std::unique_lock lock(mMutexNotifier);
    for (auto &notifier: mListNotifiers) {
        std::invoke(std::forward<Func>(func), *notifier, std::forward<Args>(args)...);
    }
}
