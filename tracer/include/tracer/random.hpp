#pragma once

#include <glm/vec3.hpp>
#include <pcg_random.hpp>

namespace tracer {

class Random
{
public:
    [[nodiscard]] auto get_float() -> float;
    [[nodiscard]] auto get_float(float min, float max) -> float;

    [[nodiscard]] auto get_double() -> double;
    [[nodiscard]] auto get_double(double min, double max) -> double;

    [[nodiscard]] auto get_dvec3() -> glm::dvec3;
    [[nodiscard]] auto get_dvec3(double min, double max) -> glm::dvec3;
    [[nodiscard]] auto get_unit_dvec3() -> glm::dvec3;

private:
    pcg32_fast _generator{};
};

} // namespace tracer
