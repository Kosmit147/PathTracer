#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <mdspan>
#include <memory>
#include <span>
#include <stop_token>

#include "tracer/common.hpp"
#include "tracer/object.hpp"

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
    virtual ~Renderer() = default;

    virtual auto render(
        ObjectView world, ProgressCallback progress_callback = [](i32) {},
        std::stop_token stop_token = std::stop_token{}) -> void = 0;
};

auto render(
    const ImageView& image, ObjectView world, const Camera& camera = {}, const RenderParams& render_params = {},
    ProgressCallback progress_callback = [](i32) {}, std::stop_token stop_token = std::stop_token{}) -> void;

} // namespace tracer
