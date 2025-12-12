#include "tracer/render.hpp"

#include <stop_token>
#include <utility>

#include "tracer/camera.hpp"

namespace tracer {

auto render(const CameraParams& camera_params, const RenderParams& render_params, const ImageView& image,
            ObjectView objects, ProgressCallback progress_callback, std::stop_token stop_token) -> void
{
    auto camera = Camera{ camera_params };
    camera.render(image, objects, render_params, progress_callback, std::move(stop_token));
}

} // namespace tracer
