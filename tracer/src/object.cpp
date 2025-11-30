#include "tracer/object.hpp"

#include <glm/exponential.hpp>
#include <glm/geometric.hpp>

#include <optional>

#include "tracer/ray.hpp"

namespace tracer {

auto Sphere::hit(const Ray& ray, double t_min, double t_max) const -> std::optional<Hit>
{
    const auto oc = _center - ray.origin();

    // Solve a quadratic equation.

    auto a = glm::dot(ray.direction(), ray.direction());
    auto h = glm::dot(ray.direction(), oc);
    auto c = glm::dot(oc, oc) - _radius * _radius;
    auto discriminant = h * h - a * c;

    if (discriminant < 0.0)
        return std::nullopt;

    auto discriminant_sqrt = glm::sqrt(discriminant);

    // a is always positive, which means that subtracting discriminant_root will give us the smaller t.
    auto t = (h - discriminant_sqrt) / a;

    // Check if t is outside acceptable range.
    if (t < t_min || t > t_max)
    {
        // Try again with bigger t.
        t = (h + discriminant_sqrt) / a;

        if (t < t_min || t > t_max)
            return std::nullopt;
    }

    auto point = ray.at(t);
    auto normal = (point - _center) / _radius; // Normalize by dividing by the radius.

    return Hit{
        .point = point,
        .normal = normal,
        .t = t,
    };
}

} // namespace tracer
