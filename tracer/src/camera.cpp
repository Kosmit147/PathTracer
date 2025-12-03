#include "tracer/camera.hpp"

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <optional>

#include "tracer/common.hpp"
#include "tracer/numeric.hpp"
#include "tracer/object.hpp"
#include "tracer/ray.hpp"

namespace tracer {

Camera::Camera(const CameraParams& params) : _position{ params.position }, _focal_length{ params.focal_length } {}

auto Camera::render(const ImageView& image, ObjectView world, ProgressCallback progress_callback) const -> void
{
    const auto height = image.extent(0);
    const auto width = image.extent(1);

    auto aspect_ratio = static_cast<double>(width) / static_cast<double>(height);

    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;

    auto viewport = Viewport{
        .width = viewport_width,
        .height = viewport_height,
    };

    i32 progress = -1;

    for (usize y = 0; y < height; y++)
    {
        auto new_progress = static_cast<i32>(static_cast<float>(y) / static_cast<float>(height) * 100.0f);

        if (new_progress != progress)
        {
            progress = new_progress;
            progress_callback(progress);
        }

        for (usize x = 0; x < width; x++)
        {
            auto r = ray(x, y, width, height, viewport);
            auto color = ray_color(r, world);
            image[y, x] = glm::clamp(color, glm::vec4{ 0.0f }, glm::vec4{ 1.0f });
        }
    }

    progress_callback(100);
}

auto Camera::ray(usize x, usize y, usize image_width, usize image_height, Viewport viewport) const -> Ray
{
    auto x_position = ((static_cast<double>(x) + 0.5) / static_cast<double>(image_width) - 0.5) * viewport.width;
    auto y_position = -(((static_cast<double>(y) + 0.5) / static_cast<double>(image_height) - 0.5) * viewport.height);
    auto pixel_position = glm::dvec3{ x_position, y_position, -_focal_length };
    auto ray_direction = glm::normalize(pixel_position - _position);

    return Ray{ _position, ray_direction };
}

auto Camera::ray_color(const Ray& ray, ObjectView world) const -> glm::vec4
{
    if (auto hit = closest_hit(world, ray))
    {
        auto normal_color = hit->normal / 2.0 + 0.5;
        return glm::vec4{ normal_color, 1.0f };
    }

    return background(ray);
}

auto Camera::closest_hit(ObjectView objects, const Ray& ray, Interval interval) const -> std::optional<Hit>
{
    auto closest = std::optional<Hit>{ std::nullopt };

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

auto Camera::background(const Ray& ray) const -> glm::vec4
{
    static constexpr auto blue = glm::vec3{ 0.5f, 0.7f, 1.0f };
    static constexpr auto white = glm::vec3{ 1.0f };

    auto blend = static_cast<float>(ray.direction().y) / 2.0f + 0.5f;
    auto background = blend * blue + (1.0f - blend) * white;

    return glm::vec4{ background, 1.0f };
}

} // namespace tracer
