cmake_minimum_required(VERSION 4.1)

option(PT_ASSERTS "Enable asserts" FALSE)
option(PT_WARNING_AS_ERROR "Treat warnings as errors" FALSE)

set(PT_COMPILE_FLAGS "" CACHE STRING "Flags to pass to the compiler")

project(
    PathTracer
    VERSION 0.0.1
    LANGUAGES C CXX
)

add_subdirectory(libs/glad)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)
add_subdirectory(libs/spdlog)

add_subdirectory(tracer)

add_subdirectory(presenter)
