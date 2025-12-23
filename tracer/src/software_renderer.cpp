#include "tracer/software_renderer.hpp"

#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <optional>
#include <stop_token>

#include "tracer/assert.hpp"
#include "tracer/common.hpp"
#include "tracer/geometric.hpp"
#include "tracer/numeric.hpp"
#include "tracer/object.hpp"
#include "tracer/random.hpp"
#include "tracer/ray.hpp"

namespace tracer {

SoftwareRenderer::SoftwareRenderer(const ImageView<glm::vec4>& image, const Camera& camera,
                                   const RenderParams& render_params)
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

auto SoftwareRenderer::render(ObjectView world, std::stop_token stop_token, volatile i32* progress) -> void
{
    if (progress)
        *progress = 0;

    for (usize y = 0; y < _image.height(); y++)
    {
        if (progress)
            *progress = static_cast<i32>(static_cast<float>(y) / static_cast<float>(_image.height()) * 100.0f);

        for (usize x = 0; x < _image.width(); x++)
            _image[y, x] = glm::vec4{ pixel_color(x, y, world), 1.0f };

        if (stop_token.stop_requested())
            return;
    }

    if (progress)
        *progress = 100;
}

auto SoftwareRenderer::pixel_color(usize x, usize y, ObjectView world) -> glm::vec3
{
    const auto pixel_x_position =
        ((static_cast<double>(x) + 0.5) / static_cast<double>(_image.width()) - 0.5) * _viewport.width;
    const auto pixel_y_position =
        -(((static_cast<double>(y) + 0.5) / static_cast<double>(_image.height()) - 0.5) * _viewport.height);

    TRACER_ASSERT(_camera.focal_length != 0.0);
    const auto pixel_position = glm::dvec3{ pixel_x_position, pixel_y_position, -_camera.focal_length };
    const auto pixel_size = glm::dvec2{ _viewport.width / static_cast<double>(_image.width()),
                                        _viewport.height / static_cast<double>(_image.height()) };

    auto color = glm::vec3{ 0.0f };

    for (usize i = 0; i < _render_params.samples; i++)
    {
        auto ray = sample_pixel(pixel_position, pixel_size);
        color += ray_color(ray, world, _render_params.max_depth);
    }

    TRACER_ASSERT(_render_params.samples != 0);
    color /= static_cast<float>(_render_params.samples);
    color = gamma_correction(color);
    color = glm::clamp(color, glm::vec3{ 0.0f }, glm::vec3{ 1.0f });

    return color;
}

auto SoftwareRenderer::sample_pixel(const glm::dvec3& pixel_position, glm::dvec2 pixel_size) -> Ray
{
    auto sample = sample_unit_square() * pixel_size;
    auto sample_position = pixel_position + glm::dvec3{ sample.x, sample.y, 0.0 };
    auto ray_direction = glm::normalize(sample_position - _camera.position);

    return Ray{ _camera.position, ray_direction };
}

auto SoftwareRenderer::ray_color(const Ray& ray, ObjectView world, usize max_depth) -> glm::vec3
{
    if (max_depth == 0)
        return glm::vec3{ 0.0f };

    if (auto hit = closest_hit(world, ray, Interval{ .min = 0.001, .max = +infinity }))
    {
        static constexpr auto material_color = glm::vec3{ 0.5f };

        auto reflect_direction = lambertian_reflection(hit->normal);
        auto reflected_ray = Ray{
            hit->point,
            reflect_direction,
        };

        return material_color * ray_color(reflected_ray, world, max_depth - 1);
    }

    return ambient(ray);
}

auto SoftwareRenderer::closest_hit(ObjectView objects, const Ray& ray, Interval interval) -> std::optional<Hit>
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

auto SoftwareRenderer::ambient(const Ray& ray) -> glm::vec3
{
    static constexpr auto blue = glm::vec3{ 0.5f, 0.7f, 1.0f };
    static constexpr auto white = glm::vec3{ 1.0f };

    auto blend = static_cast<float>(ray.direction().y) / 2.0f + 0.5f;
    auto color = blend * blue + (1.0f - blend) * white;

    return color;
}

auto SoftwareRenderer::random_reflection(const glm::dvec3& normal) -> glm::dvec3
{
    return faceforward(_random.get_unit_dvec3(), normal);
}

auto SoftwareRenderer::lambertian_reflection(const glm::dvec3& normal) -> glm::dvec3
{
    // Pick a random point on a unit sphere tangent to the intersection point.
    return glm::normalize(normal + _random.get_unit_dvec3());
}

auto SoftwareRenderer::sample_unit_square() -> glm::dvec2
{
    return glm::dvec2{ _random.get_double(-0.5, 0.5), _random.get_double(-0.5, 0.5) };
}

auto SoftwareRenderer::gamma_correction(glm::vec3 linear_space_color) -> glm::vec3
{
    // We're using a gamma value of 2.0, therefore the inverse is just the square root.
    linear_space_color = glm::clamp(linear_space_color, glm::vec3{ 0.0f }, glm::vec3{ 1.0f });
    return glm::sqrt(linear_space_color);
}

} // namespace tracer
