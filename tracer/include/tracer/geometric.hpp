#pragma once

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <concepts>

namespace tracer {

// The incident vector is assumed to be the same as the vector to orient.
template<glm::length_t L, std::floating_point T>
[[nodiscard]] constexpr auto faceforward(const glm::vec<L, T>& v, const glm::vec<L, T>& normal) -> decltype(auto)
{
    return glm::faceforward(v, v, normal);
}

template<glm::length_t L, std::floating_point T>
[[nodiscard]] constexpr auto faceforward(const glm::vec<L, T>& v, const glm::vec<L, T>& incident,
                                         const glm::vec<L, T>& normal) -> decltype(auto)
{
    return glm::faceforward(v, incident, normal);
}

} // namespace tracer
