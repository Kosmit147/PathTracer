#include "tracer/random.hpp"

#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <random>

namespace tracer {

auto Random::get_float() -> float
{
    return get_float(0.0f, 1.0f);
}

auto Random::get_float(float min, float max) -> float
{
    return std::uniform_real_distribution{ min, max }(_generator);
}

auto Random::get_double() -> double
{
    return get_double(0.0, 1.0);
}

auto Random::get_double(double min, double max) -> double
{
    return std::uniform_real_distribution{ min, max }(_generator);
}

auto Random::get_dvec3() -> glm::dvec3
{
    return glm::dvec3{ get_double(), get_double(), get_double() };
}

auto Random::get_dvec3(double min, double max) -> glm::dvec3
{
    return glm::dvec3{ get_double(min, max), get_double(min, max), get_double(min, max) };
}

auto Random::get_unit_dvec3() -> glm::dvec3
{
    while (true)
    {
        auto vec = get_dvec3(-1.0, 1.0);
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
