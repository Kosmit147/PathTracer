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
#include "tracer/random.hpp"
#include "tracer/ray.hpp"

namespace tracer {

using ProgressCallback = void (*)(i32);
using ObjectView = std::span<const std::shared_ptr<const Object>>;

struct ImageView : std::mdspan<glm::vec4, std::dextents<usize, 2>>
{
    using std::mdspan<glm::vec4, std::dextents<usize, 2>>::mdspan;

    [[nodiscard]] auto width() const -> auto { return extent(1); }
    [[nodiscard]] auto height() const -> auto { return extent(0); }
};

struct Camera
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

class Renderer
{
public:
    explicit Renderer(const ImageView& image, const Camera& camera = {}, const RenderParams& render_params = {});

    auto render(
        ObjectView world, ProgressCallback progress_callback = [](i32) {},
        std::stop_token stop_token = std::stop_token{}) -> void;

private:
    [[nodiscard]] auto pixel_color(usize x, usize y, ObjectView world) -> glm::vec3;
    [[nodiscard]] auto sample_pixel(const glm::dvec3& pixel_position, glm::dvec2 pixel_size) -> Ray;

    [[nodiscard]] auto ray_color(const Ray& ray, ObjectView world, usize max_depth) -> glm::vec3;
    [[nodiscard]] static auto closest_hit(ObjectView objects, const Ray& ray,
                                          Interval interval = Interval::non_negative) -> std::optional<Hit>;
    [[nodiscard]] static auto ambient(const Ray& ray) -> glm::vec3;

    [[nodiscard]] auto random_reflection(const glm::dvec3& normal) -> glm::dvec3;
    [[nodiscard]] auto lambertian_reflection(const glm::dvec3& normal) -> glm::dvec3;

    [[nodiscard]] auto sample_unit_square() -> glm::dvec2;

    [[nodiscard]] static auto gamma_correction(glm::vec3 linear_space_color) -> glm::vec3;

private:
    ImageView _image{};
    Camera _camera{};
    RenderParams _render_params{};
    Viewport _viewport{};
    Random _random{};
};

auto render(
    const ImageView& image, ObjectView world, const Camera& camera = {}, const RenderParams& render_params = {},
    ProgressCallback progress_callback = [](i32) {}, std::stop_token stop_token = std::stop_token{}) -> void;

} // namespace tracer
