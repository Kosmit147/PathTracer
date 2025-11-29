#pragma once

#include <glm/vec4.hpp>

#include <mdspan>

#include "tracer/common.hpp"

namespace tracer {

using ProgressCallback = void (*)(i32);

auto render(
    const std::mdspan<glm::vec4, std::dextents<usize, 2>>& image, ProgressCallback progress_callback = [](i32) {})
    -> void;

} // namespace tracer
