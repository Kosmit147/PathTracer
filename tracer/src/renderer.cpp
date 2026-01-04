#include "tracer/renderer.hpp"

#include <glm/vec4.hpp>

#include <memory>
#include <span>
#include <utility>

#include "tracer/common.hpp"
#include "tracer/software_renderer.hpp"

namespace tracer {

Image::Image(usize width, usize height)
{
    resize(width, height);
}

auto Image::resize(usize width, usize height) -> void
{
    if (width == _width && height == _height)
        return;

    _width = width;
    _height = height;
    _pixels = std::make_unique<glm::vec4[]>(_width * _height);
}

auto Image::view() -> ImageView<glm::vec4>
{
    return ImageView{ _pixels.get(), width(), height() };
}

auto Image::view() const -> ImageView<const glm::vec4>
{
    return ImageView{ static_cast<const glm::vec4*>(_pixels.get()), width(), height() };
}

auto Image::pixels() -> std::span<glm::vec4>
{
    return std::span{ _pixels.get(), _width * _height };
}

auto Image::pixels() const -> std::span<const glm::vec4>
{
    return std::span{ _pixels.get(), _width * _height };
}

auto render(const ImageView<glm::vec4>& image, ObjectSpan world, const Camera& camera,
            const RenderParams& render_params, std::stop_token stop_token, volatile i32* progress) -> void
{
    SoftwareRenderer{ image, world, camera, render_params }.render(std::move(stop_token), progress);
}

} // namespace tracer
