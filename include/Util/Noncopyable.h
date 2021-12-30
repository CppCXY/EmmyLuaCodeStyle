#pragma once

class  Noncopyable
{
public:
    Noncopyable() = default;

    virtual ~Noncopyable() = default;

private:
    Noncopyable(const Noncopyable&) = delete;

    Noncopyable& operator=(const Noncopyable&) = delete;
};