#pragma once

#include <glm/vec3.hpp>

namespace tracer {

class Ray
{
public:
    explicit Ray(const glm::dvec3& origin, const glm::dvec3& direction) : _origin{ origin }, _direction{ direction } {}

    [[nodiscard]] auto origin() const -> auto { return _origin; }
    [[nodiscard]] auto direction() const -> auto { return _direction; }

    [[nodiscard]] auto at(double t) const -> auto { return _origin + t * _direction; }

private:
    glm::dvec3 _origin = glm::dvec3{ 0.0 };
    glm::dvec3 _direction = glm::dvec3{ 0.0 };
};

} // namespace tracer
