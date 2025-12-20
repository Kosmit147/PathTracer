#include "tracer/renderer.hpp"

#include "tracer/software_renderer.hpp"

namespace tracer {

auto render(const ImageView& image, ObjectView world, const Camera& camera, const RenderParams& render_params,
            ProgressCallback progress_callback, std::stop_token stop_token) -> void
{
    auto renderer = SoftwareRenderer{ image, camera, render_params };
    renderer.render(world, progress_callback, std::move(stop_token));
}

} // namespace tracer
