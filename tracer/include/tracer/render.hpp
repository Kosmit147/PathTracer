#pragma once

#include "tracer/camera.hpp"
#include "tracer/common.hpp"

namespace tracer {

auto render(
    const CameraParams& camera_params, const RenderParams& render_params, const ImageView& image, ObjectView objects,
    ProgressCallback progress_callback = [](i32) {}) -> void;

} // namespace tracer
