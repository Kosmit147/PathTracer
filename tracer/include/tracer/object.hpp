#pragma once

#include <glm/vec3.hpp>

#include <optional>

#include "tracer/ray.hpp"

namespace tracer {

class Object
{
public:
    virtual ~Object() = default;

    [[nodiscard]] virtual auto hit(const Ray& ray, double t_min, double t_max) const -> std::optional<Hit> = 0;
};

class Sphere : public Object
{
public:
    explicit constexpr Sphere(const glm::dvec3& center, double radius) : _center{ center }, _radius{ radius } {}

    ~Sphere() override = default;

    [[nodiscard]] auto hit(const Ray& ray, double t_min, double t_max) const -> std::optional<Hit> override;

    [[nodiscard]] auto center() const -> auto { return _center; }
    [[nodiscard]] auto radius() const -> auto { return _radius; }

private:
    glm::dvec3 _center{ 0.0 };
    double _radius = 0.0;
};

} // namespace tracer
