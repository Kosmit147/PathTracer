#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <optional>
#include <stop_token>

#include "tracer/numeric.hpp"
#include "tracer/random.hpp"
#include "tracer/ray.hpp"
#include "tracer/renderer.hpp"

namespace tracer {

class SoftwareRenderer : public Renderer
{
public:
    explicit SoftwareRenderer(const ImageView& image, const Camera& camera = {},
                              const RenderParams& render_params = {});

    auto render(ObjectView world, ProgressCallback progress_callback, std::stop_token stop_token) -> void override;

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

} // namespace tracer
