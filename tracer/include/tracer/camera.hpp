#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <optional>

#include "tracer/common.hpp"
#include "tracer/numeric.hpp"
#include "tracer/ray.hpp"
#include "tracer/render.hpp"

namespace tracer {

struct CameraParams
{
    glm::dvec3 position{ 0.0 };
    double focal_length = 1.0;
};

struct Viewport
{
    double width = 2.0;
    double height = 2.0;
};

class Camera
{
public:
    explicit Camera(const CameraParams& params = {});

    auto render(const ImageView& image, ObjectView world, ProgressCallback progress_callback) const -> void;

private:
    [[nodiscard]] auto ray(usize x, usize y, usize image_width, usize image_height, Viewport viewport) const -> Ray;
    [[nodiscard]] auto ray_color(const Ray& ray, ObjectView world) const -> glm::vec4;
    [[nodiscard]] auto closest_hit(ObjectView objects, const Ray& ray, Interval interval = Interval::non_negative) const
        -> std::optional<Hit>;
    [[nodiscard]] auto background(const Ray& ray) const -> glm::vec4;

private:
    glm::dvec3 _position{ 0.0 };
    double _focal_length = 1.0;
};

} // namespace tracer
