#pragma once

namespace tracer {

[[nodiscard]] auto random_float() -> float;
[[nodiscard]] auto random_float(float min, float max) -> float;

[[nodiscard]] auto random_double() -> double;
[[nodiscard]] auto random_double(double min, double max) -> double;

} // namespace tracer
