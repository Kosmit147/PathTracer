#include "tracer/core.hpp"

#include <glm/vec4.hpp>

#include <mdspan>

#include "tracer/common.hpp"

namespace tracer {

auto render(const std::mdspan<glm::vec4, std::dextents<usize, 2>>& image, ProgressCallback progress_callback) -> void
{
    auto height = image.extent(0);
    auto width = image.extent(1);
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
            auto r = static_cast<float>(x) / static_cast<float>(width - 1);
            auto g = static_cast<float>(y) / static_cast<float>(height - 1);
            auto b = 0.0f;

            image[y, x] = glm::vec4{ r, g, b, 1.0f };
        }
    }

    progress_callback(100);
}

} // namespace tracer
