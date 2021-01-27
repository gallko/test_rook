#pragma once

class RemoveCopyMove {
public:
    RemoveCopyMove() = default;
    virtual ~RemoveCopyMove() = default;

    RemoveCopyMove(const RemoveCopyMove &) = delete;
    RemoveCopyMove(RemoveCopyMove &&) = delete;

    RemoveCopyMove &operator=(const RemoveCopyMove &) = delete;
    RemoveCopyMove &operator=(RemoveCopyMove &&) = delete;
};
