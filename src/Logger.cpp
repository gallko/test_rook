#include "ostream"
#include "Logger.h"
#include "IChessMan.h"

inline std::ostream& operator<<(std::ostream& os, const board::Coordinate& coord)
{
    os <<  static_cast<char>('A' + coord.first) << static_cast<std::uint32_t>(coord.second);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const chessman::IChessMan& chessman)
{
    switch (chessman.getType()) {
        case chessman::ChessmanType::rook:
            os << "rook[" << chessman.getID() << "] " << chessman.getCurrentCoordinate();;
            break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Logger::Action& action)
{
    switch (action) {
        case Logger::Action::placed:
            os << "-O";
            break;
        case Logger::Action::moved:
            os << "->";
            break;
        case Logger::Action::cancelMoved:
            os << "X?";
            break;
        case Logger::Action::removed:
            os << "-X";
            break;
        case Logger::Action::waitingForCell:
            os << "-?";
            break;
        case Logger::Action::reject:
            os << "XX";
            break;
    }

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const board::ReasonReject reasonReject)
{
    switch (reasonReject) {
        case board::ReasonReject::empty:
            break;
        case board::ReasonReject::boardStopped:
            os << "boardStopped";
            break;
        case board::ReasonReject::incorrectCoordinate:
            os << "incorrectCoordinate";
            break;
        case board::ReasonReject::idMismatch:
            os << "idMismatch";
            break;
        case board::ReasonReject::incorrectId:
            os << "incorrectId";
            break;
        case board::ReasonReject::duplicateId:
            os << "duplicateId";
            break;
        case board::ReasonReject::waiterNotFound:
            os << "waiterNotFound";
            break;
    }
    return os;
}

template <typename... Args>
inline void print_action(std::ostream &os, Args&&... args)
{
    os << "ID:", ((os << " " << std::forward<Args>(args)), ...) << std::endl;
}

Logger::Logger(std::ostream &os)
    : TreadBase("Logger")
    , mOut(os)
{

}

Logger::~Logger()
{

    if (std::lock_guard lock(mMutex); mReasonWeakUp != ReasonWeakUp::stop)
    {
        mReasonWeakUp = ReasonWeakUp::exit;
        mWait.notify_all();
        TreadBase::join();
    }
}

/* ************************************************************
 * IMPL IGameElement
 * ************************************************************/
void Logger::startGame()
{
    TreadBase::start();
}

void Logger::stopGame()
{
    {
        std::lock_guard lock(mMutex);
        mReasonWeakUp = ReasonWeakUp::stop;
        mWait.notify_all();
    }
    join();
}

/* ************************************************************
 * IMPL TreadBase
 * ************************************************************/
void Logger::onStart()
{
    mOut << "Logger started.\n";
    mOut << "    \"-O\" - placed" << std::endl;
    mOut << "    \"-X\" - deleted" << std::endl;
    mOut << "    \"->\" - moved" << std::endl;
    mOut << "    \"-?\" - wait" << std::endl;
    mOut << "    \"X?\" - cancel wait" << std::endl;
    mOut << "    \"XX\" - reject" << std::endl;
}

void Logger::loop()
{
    std::unique_lock lock(mMutex);
    while (mReasonWeakUp != ReasonWeakUp::stop)
    {
        while (!mQueueLog.empty())
        {
            auto log = std::move(mQueueLog.front());
            mQueueLog.pop();
            lock.unlock();
            print(log);
            lock.lock();
        }
        if (mReasonWeakUp != ReasonWeakUp::stop)
        {
            mReasonWeakUp = ReasonWeakUp::fake;
        }
        mWait.wait(lock, [this]() {
            return mReasonWeakUp != ReasonWeakUp::fake;
        });
    }
}

void Logger::onStop()
{
    if (mReasonWeakUp != ReasonWeakUp::exit) {
        std::unique_lock lock(mMutex);
        while (!mQueueLog.empty())
        {
            print(mQueueLog.front());
            mQueueLog.pop();
        }
    }
    mOut << "Logger stopped.\n";
}

/* ************************************************************
 * IMPL board::INotifier
 * ************************************************************/
void Logger::placed(std::uint32_t id, const board::Coordinate &to) noexcept
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp != ReasonWeakUp::stop)
    {
        mQueueLog.emplace(Action::placed, id, board::Coordinate{}, to, board::ReasonReject::empty);
        mReasonWeakUp = ReasonWeakUp::print_log;
        mWait.notify_all();
    }
}

void Logger::moved(std::uint32_t id, const board::Coordinate &from, const board::Coordinate &to) noexcept
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp != ReasonWeakUp::stop)
    {
        mQueueLog.emplace(Action::moved, id, from, to, board::ReasonReject::empty);
        mReasonWeakUp = ReasonWeakUp::print_log;
        mWait.notify_all();
    }
}

void Logger::cancelMoved(std::uint32_t id, const board::Coordinate &from, const board::Coordinate &to) noexcept
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp != ReasonWeakUp::stop)
    {
        mQueueLog.emplace(Action::cancelMoved, id, from, to, board::ReasonReject::empty);
        mReasonWeakUp = ReasonWeakUp::print_log;
        mWait.notify_all();
    }
}

void Logger::removed(std::uint32_t id, const board::Coordinate &from) noexcept
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp != ReasonWeakUp::stop)
    {
        mQueueLog.emplace(Action::removed, id, from, board::Coordinate{}, board::ReasonReject::empty);
        mReasonWeakUp = ReasonWeakUp::print_log;
        mWait.notify_all();
    }
}

void Logger::waitingForCell(std::uint32_t id, const board::Coordinate &from, const board::Coordinate &to) noexcept
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp != ReasonWeakUp::stop)
    {
        mQueueLog.emplace(Action::waitingForCell, id, from, to, board::ReasonReject::empty);
        mReasonWeakUp = ReasonWeakUp::print_log;
        mWait.notify_all();
    }
}

void Logger::reject(std::uint32_t id, board::ReasonReject reason) noexcept
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp != ReasonWeakUp::stop)
    {
        mQueueLog.emplace(Action::reject, id, board::Coordinate{}, board::Coordinate{}, reason);
        mReasonWeakUp = ReasonWeakUp::print_log;
        mWait.notify_all();
    }
}

/* ************************************************************
 * IMPL private
 * ************************************************************/

void Logger::print(const Logger::LogStruct log)
{
    switch (log.mAction)
    {
        case Logger::Action::placed:
            print_action(mOut, log.mId, log.mAction, log.mToCoordinate);
            break;
        case Logger::Action::moved:
        case Logger::Action::cancelMoved:
        case Logger::Action::waitingForCell:
            if (log.mFromCoordinate != board::invalidCoordinate) {
                print_action(mOut, log.mId, log.mFromCoordinate, log.mAction, log.mToCoordinate);
            } else {
                print_action(mOut, log.mId, log.mAction, log.mToCoordinate);
            }
            break;
        case Logger::Action::removed:
            print_action(mOut, log.mId, log.mAction, log.mFromCoordinate);
            break;
        case Logger::Action::reject:
            print_action(mOut, log.mId, log.mAction, log.mReasonReject);
    }
}
