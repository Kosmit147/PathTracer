#include "tracer/random.hpp"

#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <pcg_random.hpp>

#include <random>

namespace tracer {

namespace {

auto generator = pcg32_fast{};

} // namespace

auto random_float() -> float
{
    return random_float(0.0f, 1.0f);
}

auto random_float(float min, float max) -> float
{
    return std::uniform_real_distribution{ min, max }(generator);
}

auto random_double() -> double
{
    return random_double(0.0, 1.0);
}

auto random_double(double min, double max) -> double
{
    return std::uniform_real_distribution{ min, max }(generator);
}

auto random_dvec3() -> glm::dvec3
{
    return glm::dvec3{ random_double(), random_double(), random_double() };
}

auto random_dvec3(double min, double max) -> glm::dvec3
{
    return glm::dvec3{ random_double(min, max), random_double(min, max), random_double(min, max) };
}

auto random_unit_dvec3() -> glm::dvec3
{
    while (true)
    {
        auto vec = random_dvec3(-1.0, 1.0);
        auto length_sq = glm::dot(vec, vec);

        if (length_sq <= 1.0)
        {
            auto length = glm::sqrt(length_sq);

            if (length != 0.0)
                return vec / length;
        }
    }
}

} // namespace tracer
