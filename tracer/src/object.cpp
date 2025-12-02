#include "tracer/object.hpp"

#include <glm/exponential.hpp>
#include <glm/geometric.hpp>

#include <optional>

#include "tracer/numeric.hpp"
#include "tracer/ray.hpp"

namespace tracer {

auto Sphere::hit(const Ray& ray, Interval interval) const -> std::optional<Hit>
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

    // a is always positive, which means subtracting discriminant_root will give us the smaller t.
    auto t = (h - discriminant_sqrt) / a;

    if (!interval.contains(t))
    {
        // Try again with bigger t.
        t = (h + discriminant_sqrt) / a;

        if (!interval.contains(t))
            return std::nullopt;
    }

    auto point = ray.at(t);
    auto outward_normal = (point - _center) / _radius; // Normalize by dividing by the radius.
    auto front_face = glm::dot(ray.direction(), outward_normal) < 0.0;

    return Hit{
        .point = point,
        .normal = front_face ? outward_normal : -outward_normal,
        .t = t,
        .front_face = front_face,
    };
}

} // namespace tracer
