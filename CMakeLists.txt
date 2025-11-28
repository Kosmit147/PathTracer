cmake_minimum_required(VERSION 4.1)

project(
    PathTracer
    VERSION 0.0.1
    LANGUAGES C CXX
)

add_subdirectory(libs/glfw)
add_subdirectory(libs/spdlog)

add_subdirectory(tracer)

add_subdirectory(presenter)
