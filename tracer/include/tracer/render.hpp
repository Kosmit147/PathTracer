#pragma once

#include <glm/vec4.hpp>

#include <mdspan>
#include <memory>
#include <span>

#include "tracer/common.hpp"
#include "tracer/object.hpp"

namespace tracer {

struct CameraParams;

using ProgressCallback = void (*)(i32);
using ImageView = std::mdspan<glm::vec4, std::dextents<usize, 2>>;
using ObjectView = std::span<const std::shared_ptr<const Object>>;

auto render(
    const CameraParams& camera_params, const ImageView& image, ObjectView objects,
    ProgressCallback progress_callback = [](i32) {}) -> void;

} // namespace tracer
