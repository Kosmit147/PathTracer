#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <tracer/core.hpp>

#include <cstdlib>
#include <mdspan>
#include <string>
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

const std::string vertex_shader_source =
    R"(#version 460 core

out vec2 TexCoords;

void main()
{
    vec2 position;
    vec2 tex_coords;

    switch (gl_VertexID)
    {
    case 0:
        position = vec2(-1.0, 1.0);
        tex_coords = vec2(0.0, 0.0);
        break;
    case 1:
        position = vec2(-1.0, -1.0);
        tex_coords = vec2(0.0, 1.0);
        break;
    case 2:
        position = vec2(1.0, 1.0);
        tex_coords = vec2(1.0, 0.0);
        break;
    case 3:
        position = vec2(1.0, -1.0);
        tex_coords = vec2(1.0, 1.0);
        break;
    }

    gl_Position = vec4(position, 0.0, 1.0);
    TexCoords = tex_coords;
})";

const std::string fragment_shader_source =
    R"(#version 460 core

in vec2 TexCoords;

layout (location = 0) uniform sampler2D image;

out vec4 outColor;

void main()
{
    outColor = texture(image, TexCoords);
})";

constexpr u32 image_width = 256;
constexpr u32 image_height = 256;
constexpr u32 image_size = image_width * image_height;

constexpr u32 window_width = 1024;
constexpr u32 window_height = 1024;

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

    // TODO: Add a compile option to enable OpenGL debug messages instead of always enabling them.
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_message_callback, nullptr);

    glViewport(0, 0, window_width, window_height);
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

    PT_INFO("OpenGL Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    PT_INFO("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    auto vertex_source_data = vertex_shader_source.c_str();
    auto fragment_source_data = fragment_shader_source.c_str();

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source_data, nullptr);
    glCompileShader(vertex_shader);
    GLint vertex_shader_is_compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_is_compiled);
    PT_ASSERT(vertex_shader_is_compiled == GL_TRUE);
    Defer delete_vertex_shader{ [&] { glDeleteShader(vertex_shader); } };

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source_data, nullptr);
    glCompileShader(fragment_shader);
    GLint fragment_shader_is_compiled;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_is_compiled);
    PT_ASSERT(fragment_shader_is_compiled == GL_TRUE);
    Defer delete_fragment_shader{ [&] { glDeleteShader(fragment_shader); } };

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vertex_shader);
    glAttachShader(shader, fragment_shader);
    glLinkProgram(shader);
    GLint shader_is_linked;
    glGetProgramiv(shader, GL_LINK_STATUS, &shader_is_linked);
    PT_ASSERT(shader_is_linked == GL_TRUE);

    glDetachShader(shader, vertex_shader);
    glDetachShader(shader, fragment_shader);
    Defer delete_shader{ [&] { glDeleteProgram(shader); } };

    glUseProgram(shader);

    std::vector<glm::vec4> image;
    image.reserve(image_size);
    tracer::render(std::mdspan{ std::data(image), image_height, image_width }, [](i32 progress) {
        if (progress >= 100)
            PT_INFO("Progress: Done");
        else
            PT_INFO("Progress: {}%", progress);
    });

    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    Defer delete_texture{ [&] { glDeleteTextures(1, &texture); } };

    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(texture, 1, GL_RGBA8, image_width, image_height);
    glTextureSubImage2D(texture, 0, 0, 0, image_width, image_height, GL_RGBA, GL_FLOAT, image.data());

    GLuint vertex_array;
    glCreateVertexArrays(1, &vertex_array);
    Defer delete_vertex_array{ [&] { glDeleteVertexArrays(1, &vertex_array); } };

    glBindVertexArray(vertex_array);
    glBindTextureUnit(0, texture);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}
