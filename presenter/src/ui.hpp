#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <imgui.h>

#include <type_traits>

#include "common.hpp"

namespace presenter::ui {

inline auto drag(const char* label, float& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f,
                 const char* format = "%.3f", ImGuiSliderFlags flags = 0) -> bool
{
    return ImGui::DragScalar(label, ImGuiDataType_Float, &v, v_speed, &v_min, &v_max, format, flags);
}

inline auto drag(const char* label, double& v, float v_speed = 1.0f, double v_min = 0.0, double v_max = 0.0,
                 const char* format = "%.3f", ImGuiSliderFlags flags = 0) -> bool
{
    return ImGui::DragScalar(label, ImGuiDataType_Double, &v, v_speed, &v_min, &v_max, format, flags);
}

template<usize Count>
auto drag(const char* label, float v[Count], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f,
          const char* format = "%.3f", ImGuiSliderFlags flags = 0) -> bool
{
    return ImGui::DragScalarN(label, ImGuiDataType_Float, v, Count, v_speed, &v_min, &v_max, format, flags);
}

template<usize Count>
auto drag(const char* label, double v[Count], float v_speed = 1.0f, double v_min = 0.0, double v_max = 0.0,
          const char* format = "%.3f", ImGuiSliderFlags flags = 0) -> bool
{
    return ImGui::DragScalarN(label, ImGuiDataType_Double, v, Count, v_speed, &v_min, &v_max, format, flags);
}

template<int Count>
auto drag(const char* label, glm::vec<Count, float>& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f,
          const char* format = "%.3f", ImGuiSliderFlags flags = 0) -> bool
{
    return ImGui::DragScalarN(label, ImGuiDataType_Float, glm::value_ptr(v), Count, v_speed, &v_min, &v_max, format,
                              flags);
}

template<int Count>
auto drag(const char* label, glm::vec<Count, double>& v, float v_speed = 1.0f, double v_min = 0.0, double v_max = 0.0,
          const char* format = "%.3f", ImGuiSliderFlags flags = 0) -> bool
{
    return ImGui::DragScalarN(label, ImGuiDataType_Double, glm::value_ptr(v), Count, v_speed, &v_min, &v_max, format,
                              flags);
}

inline auto input_usize(const char* label, usize& v, usize step = 1, usize step_fast = 100,
                        ImGuiInputTextFlags flags = 0) -> bool
{
    static_assert(sizeof(usize) == 8);
    static_assert(std::is_same_v<usize, std::size_t>);
    return ImGui::InputScalar(label, ImGuiDataType_U64, &v, step > 0 ? &step : nullptr,
                              step_fast > 0 ? &step_fast : nullptr, "%zu", flags);
}

inline auto input_u32(const char* label, u32& v, u32 step = 1, u32 step_fast = 100, ImGuiInputTextFlags flags = 0)
    -> bool
{
    static_assert(std::is_same_v<u32, unsigned int>);
    return ImGui::InputScalar(label, ImGuiDataType_U32, &v, step > 0 ? &step : nullptr,
                              step_fast > 0 ? &step_fast : nullptr, "%u", flags);
}

} // namespace presenter::ui
