#include "tracer/renderer.hpp"

#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <optional>
#include <stop_token>
#include <utility>

#include "tracer/assert.hpp"
#include "tracer/common.hpp"
#include "tracer/geometric.hpp"
#include "tracer/numeric.hpp"
#include "tracer/object.hpp"
#include "tracer/random.hpp"
#include "tracer/ray.hpp"

namespace tracer {

Renderer::Renderer(const ImageView& image, const Camera& camera, const RenderParams& render_params)
    : _image{ image }, _camera{ camera }, _render_params{ render_params }
{
    const auto aspect_ratio = static_cast<double>(_image.width()) / static_cast<double>(_image.height());

    const auto viewport_height = 2.0;
    const auto viewport_width = aspect_ratio * viewport_height;

    _viewport = Viewport{
        .width = viewport_width,
        .height = viewport_height,
    };
}

auto Renderer::render(ObjectView world, ProgressCallback progress_callback, std::stop_token stop_token) const -> void
{
    i32 progress = -1;

    for (usize y = 0; y < _image.height(); y++)
    {
        auto new_progress = static_cast<i32>(static_cast<float>(y) / static_cast<float>(_image.height()) * 100.0f);

        if (new_progress != progress)
        {
            progress = new_progress;
            progress_callback(progress);
        }

        for (usize x = 0; x < _image.width(); x++)
            _image[y, x] = pixel_color(x, y, world);

        if (stop_token.stop_requested())
            return;
    }

    progress_callback(100);
}

auto Renderer::pixel_color(usize x, usize y, ObjectView world) const -> glm::vec4
{
    const auto pixel_x_position =
        ((static_cast<double>(x) + 0.5) / static_cast<double>(_image.width()) - 0.5) * _viewport.width;
    const auto pixel_y_position =
        -(((static_cast<double>(y) + 0.5) / static_cast<double>(_image.height()) - 0.5) * _viewport.height);

    TRACER_ASSERT(_camera.focal_length != 0.0);
    const auto pixel_position = glm::dvec3{ pixel_x_position, pixel_y_position, -_camera.focal_length };
    const auto pixel_size = glm::dvec2{ _viewport.width / static_cast<double>(_image.width()),
                                        _viewport.height / static_cast<double>(_image.height()) };

    auto color = glm::vec4{ 0.0f };

    for (usize i = 0; i < _render_params.samples; i++)
    {
        auto ray = sample_pixel(pixel_position, pixel_size);
        color += ray_color(ray, world, _render_params.max_depth);
    }

    TRACER_ASSERT(_render_params.samples != 0);
    color /= static_cast<float>(_render_params.samples);
    color = gamma_correction(color);
    color = glm::clamp(color, glm::vec4{ 0.0f }, glm::vec4{ 1.0f });

    return color;
}

auto Renderer::sample_pixel(const glm::dvec3& pixel_position, glm::dvec2 pixel_size) const -> Ray
{
    auto sample = sample_unit_square() * pixel_size;
    auto sample_position = pixel_position + glm::dvec3{ sample.x, sample.y, 0.0 };
    auto ray_direction = glm::normalize(sample_position - _camera.position);

    return Ray{ _camera.position, ray_direction };
}

auto Renderer::ray_color(const Ray& ray, ObjectView world, usize max_depth) -> glm::vec4
{
    if (max_depth == 0)
        return glm::vec4{ 0.0f };

    if (auto hit = closest_hit(world, ray, Interval{ .min = 0.001, .max = +infinity }))
    {
        static constexpr auto material_color = glm::vec4{ 0.5f, 0.5f, 0.5f, 1.0f };

        auto reflect_direction = lambertian_reflection(hit->normal);
        auto reflected_ray = Ray{
            hit->point,
            reflect_direction,
        };

        return material_color * ray_color(reflected_ray, world, max_depth - 1);
    }

    return ambient(ray);
}

auto Renderer::closest_hit(ObjectView objects, const Ray& ray, Interval interval) -> std::optional<Hit>
{
    auto closest = std::optional<Hit>{};

    for (auto& object : objects)
    {
        if (auto hit = object->hit(ray, interval))
        {
            closest = hit;
            interval.max = closest->t;
        }
    }

    return closest;
}

auto Renderer::sample_unit_square() -> glm::dvec2
{
    return glm::dvec2{ random_double(-0.5, 0.5), random_double(-0.5, 0.5) };
}

auto Renderer::ambient(const Ray& ray) -> glm::vec4
{
    static constexpr auto blue = glm::vec3{ 0.5f, 0.7f, 1.0f };
    static constexpr auto white = glm::vec3{ 1.0f };

    auto blend = static_cast<float>(ray.direction().y) / 2.0f + 0.5f;
    auto background = blend * blue + (1.0f - blend) * white;

    return glm::vec4{ background, 1.0f };
}

auto Renderer::random_reflection(const glm::dvec3& normal) -> glm::dvec3
{
    return faceforward(random_unit_dvec3(), normal);
}

auto Renderer::lambertian_reflection(const glm::dvec3& normal) -> glm::dvec3
{
    // Pick a random point on a unit sphere tangent to the intersection point.
    return glm::normalize(normal + random_unit_dvec3());
}

auto Renderer::gamma_correction(glm::vec4 linear_space_color) -> glm::vec4
{
    // We're using a gamma value of 2.0, therefore the inverse is just the square root.
    auto rgb = glm::vec3{ linear_space_color };
    // Clamp negative rgb values.
    rgb = glm::clamp(rgb, glm::vec3{ 0.0f }, glm::vec3{ 1.0f });
    auto corrected_rgb = glm::sqrt(rgb);
    return glm::vec4{ corrected_rgb, linear_space_color.a };
}

auto render(const ImageView& image, ObjectView world, const Camera& camera, const RenderParams& render_params,
            ProgressCallback progress_callback, std::stop_token stop_token) -> void
{
    auto renderer = Renderer{ image, camera, render_params };
    renderer.render(world, progress_callback, std::move(stop_token));
}

} // namespace tracer
