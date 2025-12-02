#pragma once

#include <limits>

namespace tracer {

inline constexpr auto infinity = std::numeric_limits<double>::infinity();

struct Interval
{
    // Default interval is empty.
    double min = +infinity;
    double max = -infinity;

    [[nodiscard]] constexpr auto length() const -> auto { return max - min; }
    [[nodiscard]] constexpr auto contains(double x) const -> bool { return x >= min && x <= max; }
    [[nodiscard]] constexpr auto surrounds(double x) const -> bool { return x > min && x < max; }

    static const Interval empty;
    static const Interval non_negative;
    static const Interval universe;
};

inline constexpr Interval Interval::empty{ .min = +infinity, .max = -infinity };
inline constexpr Interval Interval::non_negative{ .min = 0.0, .max = +infinity };
inline constexpr Interval Interval::universe{ .min = -infinity, .max = +infinity };

} // namespace tracer
