#include "tracer/render.hpp"

#include "tracer/camera.hpp"

namespace tracer {

auto render(const CameraParams& camera_params, const ImageView& image, ObjectView objects,
            ProgressCallback progress_callback) -> void
{
    Camera camera{ camera_params };
    camera.render(image, objects, progress_callback);
}

} // namespace tracer
