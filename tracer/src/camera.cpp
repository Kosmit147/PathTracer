#include "tracer/camera.hpp"

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <optional>

#include "tracer/assert.hpp"
#include "tracer/common.hpp"
#include "tracer/numeric.hpp"
#include "tracer/object.hpp"
#include "tracer/random.hpp"
#include "tracer/ray.hpp"

namespace tracer {

Camera::Camera(const CameraParams& params) : _position{ params.position }, _focal_length{ params.focal_length } {}

auto Camera::render(const ImageView& image, ObjectView world, const RenderParams& render_params,
                    ProgressCallback progress_callback) const -> void
{
    const auto image_height = image.extent(0);
    const auto image_width = image.extent(1);

    const auto aspect_ratio = static_cast<double>(image_width) / static_cast<double>(image_height);

    const auto viewport_height = 2.0;
    const auto viewport_width = aspect_ratio * viewport_height;

    const auto viewport = Viewport{
        .width = viewport_width,
        .height = viewport_height,
    };

    i32 progress = -1;

    for (usize y = 0; y < image_height; y++)
    {
        auto new_progress = static_cast<i32>(static_cast<float>(y) / static_cast<float>(image_height) * 100.0f);

        if (new_progress != progress)
        {
            progress = new_progress;
            progress_callback(progress);
        }

        for (usize x = 0; x < image_width; x++)
            image[y, x] = pixel_color(x, y, image_width, image_height, viewport, render_params.samples,
                                      render_params.max_depth, world);
    }

    progress_callback(100);
}

auto Camera::pixel_color(usize x, usize y, usize image_width, usize image_height, Viewport viewport, usize samples,
                         usize max_depth, ObjectView world) const -> glm::vec4
{
    const auto pixel_x_position =
        ((static_cast<double>(x) + 0.5) / static_cast<double>(image_width) - 0.5) * viewport.width;
    const auto pixel_y_position =
        -(((static_cast<double>(y) + 0.5) / static_cast<double>(image_height) - 0.5) * viewport.height);

    TRACER_ASSERT(_focal_length != 0.0);
    const auto pixel_position = glm::dvec3{ pixel_x_position, pixel_y_position, -_focal_length };
    const auto pixel_size = glm::dvec2{ viewport.width / static_cast<double>(image_width),
                                        viewport.height / static_cast<double>(image_height) };

    auto color = glm::vec4{ 0.0f };

    for (usize i = 0; i < samples; i++)
    {
        auto ray = sample_pixel(pixel_position, pixel_size);
        color += ray_color(ray, world, max_depth);
    }

    TRACER_ASSERT(samples != 0);
    color /= static_cast<float>(samples);

    return glm::clamp(color, glm::vec4{ 0.0f }, glm::vec4{ 1.0f });
}

auto Camera::sample_pixel(const glm::dvec3& pixel_position, glm::dvec2 pixel_size) const -> Ray
{
    auto sample = sample_unit_square() * pixel_size;
    auto sample_position = pixel_position + glm::dvec3{ sample.x, sample.y, 0.0 };
    auto ray_direction = glm::normalize(sample_position - _position);

    return Ray{ _position, ray_direction };
}

auto Camera::sample_unit_square() const -> glm::dvec2
{
    return glm::dvec2{ random_double(-0.5, 0.5), random_double(-0.5, 0.5) };
}

auto Camera::ray_color(const Ray& ray, ObjectView world, usize max_depth) const -> glm::vec4
{
    if (max_depth == 0)
        return glm::vec4{ 0.0f };

    if (auto hit = closest_hit(world, ray, Interval{ .min = 0.001, .max = +infinity }))
    {
        static constexpr auto material_color = glm::vec4{ 0.5f, 0.5f, 0.5f, 1.0f };

        auto reflect_direction = random_unit_dvec3_on_hemisphere(hit->normal);
        auto reflected_ray = Ray{
            hit->point,
            reflect_direction,
        };

        return material_color * ray_color(reflected_ray, world, max_depth - 1);
    }

    return ambient(ray);
}

auto Camera::closest_hit(ObjectView objects, const Ray& ray, Interval interval) const -> std::optional<Hit>
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

auto Camera::ambient(const Ray& ray) const -> glm::vec4
{
    static constexpr auto blue = glm::vec3{ 0.5f, 0.7f, 1.0f };
    static constexpr auto white = glm::vec3{ 1.0f };

    auto blend = static_cast<float>(ray.direction().y) / 2.0f + 0.5f;
    auto background = blend * blue + (1.0f - blend) * white;

    return glm::vec4{ background, 1.0f };
}

} // namespace tracer
