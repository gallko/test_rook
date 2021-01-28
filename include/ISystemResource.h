#pragma once

#include <chrono>

namespace resource {

enum class ReasonWakeUp {
    exit, timeOut, externalWakeUp
};

class ISystemResource {
public:
    virtual ~ISystemResource() = default;

    virtual ReasonWakeUp sleep(std::int32_t msec) = 0;
    virtual std::int32_t random(std::int16_t from, std::int16_t to) = 0;
};

}