#include "ChessBoardImpl.h"
#include "Coordinate.h"
#include "IChessMan.h"
#include "utils.h"


ChessBoardImpl::ChessBoardImpl()
    : IChessBoard()
    , TreadBase("ChessBoardImpl")
    , mBarrierStart(nullptr)
    , mMutex()
    , mWait()
    , mReasonWeakUp(ReasonWeakUp::do_work)
    , mTaskList()
    , mBoard()
{

}

ChessBoardImpl::~ChessBoardImpl()
{
    stop();
}

bool ChessBoardImpl::placeFigure(const chessman::IChessMan &figure,
                                 const Coordinate &to,
                                 std::shared_ptr<board::INotifier> notifier)
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp == ReasonWeakUp::stop) {
        return false;
    }
    mTaskList.emplace_back(Task::Type::place, figure.id(), figure.getCurrentCoordinate(), to, notifier);
    mReasonWeakUp = ReasonWeakUp::do_work;
    mWait.notify_all();
    return true;
}

bool ChessBoardImpl::moveFigure(const chessman::IChessMan &figure,
                                const Coordinate &to,
                                std::shared_ptr<board::INotifier> notifier)
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp == ReasonWeakUp::stop) {
        return false;
    }
    mTaskList.emplace_back(Task::Type::move, figure.id(), figure.getCurrentCoordinate(), to, notifier);
    mReasonWeakUp = ReasonWeakUp::do_work;
    mWait.notify_all();
    return true;
}

bool ChessBoardImpl::removeFigure(const chessman::IChessMan &figure,
                                  std::shared_ptr<board::INotifier> notifier)
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp == ReasonWeakUp::stop) {
        return false;
    }
    mTaskList.emplace_back(Task::Type::remove, figure.id(), figure.getCurrentCoordinate(), Coordinate(), notifier);
    mReasonWeakUp = ReasonWeakUp::do_work;
    mWait.notify_all();
    return true;
}

void ChessBoardImpl::onStart()
{
    TreadBase::onStart();
    if (mBarrierStart)
    {
        pthread_barrier_wait(mBarrierStart);
    }
}

void ChessBoardImpl::loop()
{
    std::unique_lock lock(mMutex);
    while (mReasonWeakUp != ReasonWeakUp::stop)
    {
        while (!mTaskList.empty())
        {
            do_task(lock, mTaskList.front());
            mTaskList.pop_front();
        }
        mReasonWeakUp = ReasonWeakUp::fake;
        mWait.wait(lock, [this]() {
            return mReasonWeakUp != ReasonWeakUp::fake;
        });
    }
}

void ChessBoardImpl::onStop()
{
    std::unique_lock lock(mMutex);

    auto clear = [&lock](auto id, auto notifier) {
        if (notifier)
        {
            lock.unlock();
            notifier->reject(id);
            lock.lock();
        }
    };

    while (!mTaskList.empty())
    {
        auto &w = mTaskList.front();
        clear(w.mId, w.mNotifier.lock());
        mTaskList.pop_front();
    }
    mTaskList.clear();

    for (auto &line: mBoard)
        for (auto &cell: line)
            for (auto &w: cell.second)
                clear(w.first, w.second.lock());

    TreadBase::onStop();
}


uint8_t ChessBoardImpl::sizeBoard() const noexcept
{
    return SIZE_BOARD;
}

void ChessBoardImpl::start(pthread_barrier_t *barrier)
{
    mBarrierStart = barrier;
    TreadBase::start();
}

void ChessBoardImpl::stop()
{
    std::unique_lock lock(mMutex);
    mReasonWeakUp = ReasonWeakUp::stop;
    mWait.notify_all();
}

void ChessBoardImpl::do_task(std::unique_lock<std::mutex> &lock, const ChessBoardImpl::Task &task)
{
    auto notifier = task.mNotifier.lock();
    if (!notifier)
    {
        return;
    }

    switch (task.mTypeTask) {
        case Task::Type::place:
            do_place(lock, task.mId, task.mToCoordinate, notifier);
            break;
        case Task::Type::move:
            do_move(lock, task.mId, task.mFromCoordinate, task.mToCoordinate, notifier);
            break;
        case Task::Type::remove:
            do_remove(lock, task.mId, task.mFromCoordinate, notifier);
            break;
    }
}

void ChessBoardImpl::do_place(std::unique_lock<std::mutex> &lock, std::int32_t id,
                              const Coordinate &to_coordinate, const std::shared_ptr<board::INotifier> &notifier)
{
    auto &to_cell = getCell(to_coordinate);
    if (to_cell.first == sEmptyCell)
    {
        to_cell.first = id;
        lock.unlock();
        notifier->placed(id, to_coordinate);
        lock.lock();
    } else {
        to_cell.second.emplace_back(id, notifier);
        lock.unlock();
        notifier->waitingForCell(id, to_coordinate);
        lock.lock();
    }
}

void ChessBoardImpl::do_move(std::unique_lock<std::mutex> &lock, std::int32_t id,
                             const Coordinate &from_coordinate, const Coordinate &to_coordinate,
                             const std::shared_ptr<board::INotifier> &notifier)
{
    auto &from_cell = getCell(from_coordinate);
    auto &to_cell = getCell(to_coordinate);

    if (from_cell.first != id)
    {
        lock.unlock();
        notifier->reject(id);
        lock.lock();
        return;
    }

    if (to_cell.first == sEmptyCell)
    {
        from_cell.first = sEmptyCell;
        to_cell.first = id;
        lock.unlock();
        notifier->moved(id, to_coordinate);
        lock.lock();
        do_check_waiting(lock, from_coordinate);
    } else {
        to_cell.second.emplace_back(id, notifier);
        lock.unlock();
        notifier->waitingForCell(id, to_coordinate);
        lock.lock();
    }
}

void ChessBoardImpl::do_remove(std::unique_lock<std::mutex> &lock, std::int32_t id,
                               const Coordinate &from_coordinate,
                               const std::shared_ptr<board::INotifier> &notifier)
{
    auto &from_cell = getCell(from_coordinate);
    if (from_cell.first == id)
    {
        from_cell.first = sEmptyCell;
        lock.unlock();
        notifier->removed(id, from_coordinate);
        lock.lock();
        do_check_waiting(lock, from_coordinate);
    } else {
        lock.unlock();
        notifier->reject(id);
        lock.lock();
    }
}

void ChessBoardImpl::do_check_waiting(std::unique_lock<std::mutex> &lock, const Coordinate &current_coordinate)
{
    auto &current_cell = getCell(current_coordinate);
    auto &waiting_list = getCell(current_coordinate).second;

    bool flag = true;
    while (!waiting_list.empty() && flag)
    {
        auto &w = waiting_list.front();
        if (auto notifier = w.second.lock(); notifier)
        {
            current_cell.first = w.first;
            lock.unlock();
            notifier->moved(w.first, current_coordinate);
            lock.lock();
            flag = false;
        }
        waiting_list.pop_front();
    }
}
