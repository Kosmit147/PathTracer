#include "tracer/core.hpp"

#include <glm/vec4.hpp>

#include <mdspan>

namespace tracer {

auto render(const std::mdspan<glm::vec4, std::dextents<usize, 2>>& image) -> void
{
    auto height = image.extent(0);
    auto width = image.extent(1);

    for (usize y = 0; y < height; y++)
    {
        for (usize x = 0; x < width; x++)
        {
            auto r = static_cast<float>(x) / static_cast<float>(width - 1);
            auto g = static_cast<float>(y) / static_cast<float>(height - 1);
            auto b = 0.0f;

            image[y, x] = glm::vec4{ r, g, b, 1.0f };
        }
    }
}

} // namespace tracer
