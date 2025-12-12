#pragma once

#include <stop_token>

#include "tracer/camera.hpp"
#include "tracer/common.hpp"

namespace tracer {

auto render(
    const CameraParams& camera_params, const RenderParams& render_params, const ImageView& image, ObjectView objects,
    ProgressCallback progress_callback = [](i32) {}, std::stop_token stop_token = std::stop_token{}) -> void;

} // namespace tracer
