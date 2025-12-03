#include "tracer/render.hpp"

#include "tracer/camera.hpp"

namespace tracer {

auto render(const CameraParams& camera_params, const RenderParams& render_params, const ImageView& image,
            ObjectView objects, ProgressCallback progress_callback) -> void
{
    auto camera = Camera{ camera_params };
    camera.render(image, objects, render_params, progress_callback);
}

} // namespace tracer
