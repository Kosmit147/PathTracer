#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tracer/core.hpp>

#include <cstdlib>
#include <mdspan>
#include <vector>

#include "assert.hpp"
#include "common.hpp"
#include "defer.hpp"
#include "log.hpp"

namespace {

auto glfw_error_callback(int error_code, const char* description) -> void
{
    PT_ERROR("GLFW Error {}: {}", error_code, description);
}

auto glfw_framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height) -> void
{
    glViewport(0, 0, width, height);
}

auto gl_debug_message_callback([[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type, GLuint id, GLenum severity,
                               [[maybe_unused]] GLsizei length, const GLchar* message,
                               [[maybe_unused]] const void* user_param) -> void
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        PT_INFO("OpenGL Notification {}: {}", id, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        PT_WARN("OpenGL Warning {}: {}", id, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
    case GL_DEBUG_SEVERITY_HIGH:
        PT_ERROR("OpenGL Error {}: {}", id, message);
        break;
    default:
        PT_ASSERT(false);
    }
}

constexpr u32 window_width = 640;
constexpr u32 window_height = 480;

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // TODO: Add a compile option to enable OpenGL debug messages instead of always enabling them.
    glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);

    auto window = glfwCreateWindow(window_width, window_height, "Path Tracer", nullptr, nullptr);

    if (!window)
    {
        PT_CRITICAL("Failed to create a window.");
        return EXIT_FAILURE;
    }

    Defer destroy_window{ [&] { glfwDestroyWindow(window); } };

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        PT_CRITICAL("Failed to load OpenGL functions.");
        return EXIT_FAILURE;
    }

    glViewport(0, 0, window_width, window_height);
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

    // TODO: Add a compile option to enable OpenGL debug messages instead of always enabling them.
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_message_callback, nullptr);

    static constexpr u32 image_width = 256;
    static constexpr u32 image_height = 256;
    static constexpr u32 image_size = image_width * image_height;

    std::vector<tracer::Rgba> image;
    image.reserve(image_size);
    tracer::render(std::mdspan{ std::data(image), image_height, image_width });

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}
