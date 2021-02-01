#pragma once

#include <ostream>

enum class Action
{
    placed,
    deleted,
    moved,
    wait,
    cancel_wait
};

inline void print_help() {
    std::cout << "\"-O\" - placed" << std::endl;
    std::cout << "\"-X\" - deleted" << std::endl;
    std::cout << "\"->\" - moved" << std::endl;
    std::cout << "\"-?\" - wait" << std::endl;
    std::cout << "\"X?\" - cancel wait" << std::endl;
}

inline std::ostream& operator<<(std::ostream& os, const Action& action)
{
    switch (action) {
        case Action::placed:
            os << "-O";
            break;
        case Action::deleted:
            os << "-X";
            break;
        case Action::moved:
            os << "->";
            break;
        case Action::wait:
            os << "-?";
            break;
        case Action::cancel_wait:
            os << "X?";
            break;
    }

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const board::Coordinate& coord)
{
    os <<  static_cast<char>('A' + coord.first) << static_cast<std::uint32_t>(coord.second);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const chessman::IChessMan& chessman)
{
    switch (chessman.getType()) {
        case chessman::ChessmanType::rook:
            os << "rook[" << chessman.getID() << "] " << chessman.getCurrentCoordinate() << "";
            break;
    }
    return os;
}

template <typename... Args>
inline void print_action(Args&&... args)
{
    std::cout << "ID:";
    ((std::cout << " " << std::forward<Args>(args)), ...) << std::endl;
}