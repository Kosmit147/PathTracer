#include "tracer/render.hpp"

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <array>
#include <mdspan>
#include <memory>
#include <span>

#include "tracer/common.hpp"
#include "tracer/numeric.hpp"
#include "tracer/object.hpp"
#include "tracer/ray.hpp"

namespace tracer {

namespace {

[[nodiscard]] auto closest_hit(std::span<const std::shared_ptr<const Object>> objects, const Ray& ray,
                               Interval interval = Interval::non_negative) -> std::optional<Hit>
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

[[nodiscard]] auto ray_color(const Ray& ray, std::span<const std::shared_ptr<const Object>> world) -> glm::vec4
{
    if (auto hit = closest_hit(world, ray))
    {
        auto normal_color = hit->normal / 2.0 + 0.5;
        return glm::vec4{ normal_color, 1.0f };
    }

    auto blend = static_cast<float>(ray.direction().y) / 2.0f + 0.5f;
    auto blue = glm::vec3{ 0.5f, 0.7f, 1.0f };
    auto white = glm::vec3{ 1.0f };
    auto background = blend * blue + (1 - blend) * white;
    return glm::vec4{ background, 1.0f };
}

} // namespace

auto render(const std::mdspan<glm::vec4, std::dextents<usize, 2>>& image, ProgressCallback progress_callback) -> void
{
    static const auto world =
        std::array<std::shared_ptr<const Object>, 2>{ std::make_shared<Sphere>(glm::dvec3{ 0.0, 0.0, -1.0 }, 0.5),
                                                      std::make_shared<Sphere>(glm::dvec3{ 0.0, -100.5, -1.0 },
                                                                               100.0) };

    const auto height = image.extent(0);
    const auto width = image.extent(1);

    const auto aspect_ratio = static_cast<double>(width) / static_cast<double>(height);
    const auto viewport_height = 2.0;
    const auto viewport_width = aspect_ratio * viewport_height;

    const auto focal_length = 1.0;
    const auto eye = glm::dvec3{ 0.0 };

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
            auto x_position = ((static_cast<double>(x) + 0.5) / static_cast<double>(width) - 0.5) * viewport_width;
            auto y_position = -(((static_cast<double>(y) + 0.5) / static_cast<double>(height) - 0.5) * viewport_height);
            auto pixel_position = glm::dvec3{ x_position, y_position, -focal_length };

            auto ray_direction = glm::normalize(pixel_position - eye);
            auto ray = Ray{ eye, ray_direction };

            image[y, x] = ray_color(ray, world);
        }
    }

    progress_callback(100);
}

} // namespace tracer
