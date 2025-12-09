#include "tracer/random.hpp"

#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <random>

#include "tracer/geometric.hpp"

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

auto random_unit_dvec3_on_hemisphere(const glm::dvec3& normal) -> glm::dvec3
{
    return faceforward(random_unit_dvec3(), normal);
}

} // namespace tracer
