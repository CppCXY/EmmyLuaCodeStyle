#pragma once

class Nonmoveable
{
public:
    Nonmoveable() = default;

    Nonmoveable(const Nonmoveable&) = default;

    virtual ~Nonmoveable() = default;

    Nonmoveable& operator=(const Nonmoveable&) = default;

private:
    Nonmoveable(Nonmoveable&&) = delete;

    Nonmoveable& operator=(Nonmoveable&&) = delete;
};