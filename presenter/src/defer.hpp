#pragma once

#include <concepts>
#include <functional>
#include <utility>

template<std::regular_invocable F> class Defer
{
public:
    Defer(F&& f) : _f{ std::forward<decltype(f)>(f) } {}

    ~Defer()
    {
        if (!_dismissed)
            std::invoke(_f);
    }

    Defer(const Defer&) = delete;
    auto operator=(const Defer&) = delete;
    Defer(Defer&&) = delete;
    auto operator=(Defer&&) = delete;

    auto dismiss() -> void { _dismissed = true; }

private:
    const F _f;
    bool _dismissed{ false };
};
