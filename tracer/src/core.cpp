#include "tracer/core.hpp"

#include <mdspan>

namespace tracer {

auto render(const std::mdspan<Rgba, std::dextents<usize, 2>>& image) -> void
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

            image[y, x] = Rgba{ .r = r, .g = g, .b = b, .a = 1.0f };
        }
    }
}

} // namespace tracer
