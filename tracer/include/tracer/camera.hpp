#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <mdspan>
#include <memory>
#include <optional>
#include <span>
#include <stop_token>

#include "tracer/common.hpp"
#include "tracer/numeric.hpp"
#include "tracer/object.hpp"
#include "tracer/ray.hpp"

namespace tracer {

using ProgressCallback = void (*)(i32);
using ImageView = std::mdspan<glm::vec4, std::dextents<usize, 2>>;
using ObjectView = std::span<const std::shared_ptr<const Object>>;

struct CameraParams
{
    glm::dvec3 position{ 0.0 };
    double focal_length{ 1.0 };
};

struct Viewport
{
    double width{ 2.0 };
    double height{ 2.0 };
};

struct RenderParams
{
    usize samples{ 100 };
    usize max_depth{ 50 };
};

class Camera
{
public:
    explicit Camera(const CameraParams& params = {});

    auto render(
        const ImageView& image, ObjectView world, const RenderParams& render_params,
        ProgressCallback progress_callback = [](i32) {}, std::stop_token stop_token = std::stop_token{}) const -> void;

private:
    [[nodiscard]] auto pixel_color(usize x, usize y, usize image_width, usize image_height, Viewport viewport,
                                   usize samples, usize max_depth, ObjectView world) const -> glm::vec4;
    [[nodiscard]] auto sample_pixel(const glm::dvec3& pixel_position, glm::dvec2 pixel_size) const -> Ray;

    [[nodiscard]] static auto ray_color(const Ray& ray, ObjectView world, usize max_depth) -> glm::vec4;
    [[nodiscard]] static auto closest_hit(ObjectView objects, const Ray& ray,
                                          Interval interval = Interval::non_negative) -> std::optional<Hit>;
    [[nodiscard]] static auto sample_unit_square() -> glm::dvec2;
    [[nodiscard]] static auto ambient(const Ray& ray) -> glm::vec4;

    [[nodiscard]] static auto random_reflection(const glm::dvec3& normal) -> glm::dvec3;
    [[nodiscard]] static auto lambertian_reflection(const glm::dvec3& normal) -> glm::dvec3;

private:
    glm::dvec3 _position{ 0.0 };
    double _focal_length{ 1.0 };
};

} // namespace tracer
