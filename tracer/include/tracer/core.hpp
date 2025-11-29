#pragma once

#include <glm/vec4.hpp>

#include <mdspan>

#include "common.hpp"

namespace tracer {

auto render(const std::mdspan<glm::vec4, std::dextents<usize, 2>>& image) -> void;

} // namespace tracer
