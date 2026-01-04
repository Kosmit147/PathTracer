#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <mdspan>
#include <memory>
#include <span>
#include <stop_token>

#include "tracer/assert.hpp"
#include "tracer/common.hpp"
#include "tracer/object.hpp"

namespace tracer {

using ObjectSpan = std::span<const std::shared_ptr<const Object>>;
template<typename PixelType> class ImageView;

class Image
{
public:
    explicit Image() = default;
    explicit Image(usize width, usize height);

    auto resize(usize width, usize height) -> void;

    [[nodiscard]] auto width() const -> auto { return _width; }
    [[nodiscard]] auto height() const -> auto { return _height; }
    [[nodiscard]] auto view() -> ImageView<glm::vec4>;
    [[nodiscard]] auto view() const -> ImageView<const glm::vec4>;
    [[nodiscard]] auto pixels() -> std::span<glm::vec4>;
    [[nodiscard]] auto pixels() const -> std::span<const glm::vec4>;

private:
    usize _width{ 0 };
    usize _height{ 0 };
    std::unique_ptr<glm::vec4[]> _pixels{};
};

template<typename PixelType> class ImageView
{
public:
    explicit ImageView() = default;
    ImageView(Image& image) : ImageView{ image.pixels().data(), image.width(), image.height() } {}
    ImageView(const Image& image) : ImageView{ image.pixels().data(), image.width(), image.height() } {}
    explicit ImageView(PixelType* data, usize width, usize height) : _mdspan{ data, height, width } {}

    [[nodiscard]] auto width() const -> auto { return _mdspan.extent(1); }
    [[nodiscard]] auto height() const -> auto { return _mdspan.extent(0); }

    [[nodiscard]] auto operator[](usize y, usize x) const -> PixelType&
    {
        TRACER_ASSERT(x < width() && y < height());
        return _mdspan[y, x];
    }

private:
    std::mdspan<PixelType, std::dextents<usize, 2>> _mdspan{};
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

    virtual auto render(std::stop_token stop_token = std::stop_token{}, volatile i32* progress = nullptr) -> void = 0;
};

auto render(const ImageView<glm::vec4>& image, ObjectSpan world, const Camera& camera = {},
            const RenderParams& render_params = {}, std::stop_token stop_token = std::stop_token{},
            volatile i32* progress = nullptr) -> void;

} // namespace tracer
