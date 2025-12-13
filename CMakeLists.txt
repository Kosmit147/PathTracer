cmake_minimum_required(VERSION 4.1)

option(PT_ASSERTS "Enable asserts" FALSE)
option(PT_WARNING_AS_ERROR "Treat warnings as errors" FALSE)
option(PT_OPENGL_DEBUG_CONTEXT "Enable OpenGL debug messages" FALSE)
option(PT_DEBUG_BREAKS "Enable debug breaks" FALSE)

set(PT_COMPILE_FLAGS "" CACHE STRING "Flags to pass to the compiler")

project(
    PathTracer
    VERSION 0.0.1
    LANGUAGES C CXX
)

add_subdirectory(libs/glad SYSTEM)
add_subdirectory(libs/glfw SYSTEM)
add_subdirectory(libs/glm SYSTEM)
add_subdirectory(libs/imgui SYSTEM)
add_subdirectory(libs/spdlog SYSTEM)
add_subdirectory(libs/stb SYSTEM)

add_subdirectory(tracer)

add_subdirectory(presenter)
