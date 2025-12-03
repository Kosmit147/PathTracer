#include "tracer/random.hpp"

#include <random>

namespace tracer {

namespace {

auto generator = std::mt19937{};
auto float_distribution = std::uniform_real_distribution{ 0.0f, 1.0f };
auto double_distribution = std::uniform_real_distribution{ 0.0, 1.0 };

} // namespace

auto random_float() -> float
{
    return float_distribution(generator);
}

auto random_float(float min, float max) -> float
{
    auto distribution = std::uniform_real_distribution{ min, max };
    return distribution(generator);
}

auto random_double() -> double
{
    return double_distribution(generator);
}

auto random_double(double min, double max) -> double
{
    auto distribution = std::uniform_real_distribution{ min, max };
    return distribution(generator);
}

} // namespace tracer
