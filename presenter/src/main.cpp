#include <GLFW/glfw3.h>

#include <cstdlib>

#include "defer.hpp"
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

    Defer terminate_glfw{ [] { glfwTerminate(); } };

    GLFWwindow* window = glfwCreateWindow(640, 480, "Path Tracer", nullptr, nullptr);

    if (!window)
    {
        PT_CRITICAL("Failed to create a window.");
        return EXIT_FAILURE;
    }

    Defer destroy_window{ [&] { glfwDestroyWindow(window); } };

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}
