#pragma once

#include <glm/vec3.hpp>

namespace tracer {

[[nodiscard]] auto random_float() -> float;
[[nodiscard]] auto random_float(float min, float max) -> float;

[[nodiscard]] auto random_double() -> double;
[[nodiscard]] auto random_double(double min, double max) -> double;

[[nodiscard]] auto random_dvec3() -> glm::dvec3;
[[nodiscard]] auto random_dvec3(double min, double max) -> glm::dvec3;
[[nodiscard]] auto random_unit_dvec3() -> glm::dvec3;
[[nodiscard]] auto random_unit_dvec3_on_hemisphere(const glm::dvec3& normal) -> glm::dvec3;

} // namespace tracer
