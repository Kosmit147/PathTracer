#include <GLFW/glfw3.h>
#include <tracer/core.hpp>

#include <cstdlib>

#include "log.hpp"

namespace {

auto glfw_error_callback(int error_code, const char* description) -> void
{
    PT_ERROR("GLFW Error {}: {}", error_code, description);
}

} // namespace

auto main() -> int
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
        PT_CRITICAL("Failed to initialize GLFW.");
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Path Tracer", nullptr, nullptr);

    if (!window)
    {
        PT_CRITICAL("Failed to create a window.");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
