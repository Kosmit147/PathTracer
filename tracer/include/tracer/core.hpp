#pragma once

#include <mdspan>

#include "common.hpp"

namespace tracer {

// TODO: Use glm for vectors.
struct Rgba
{
    float r, g, b, a;
};

auto render(const std::mdspan<Rgba, std::dextents<usize, 2>>& image) -> void;

} // namespace tracer
