#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <tracer/camera.hpp>
#include <tracer/object.hpp>
#include <tracer/render.hpp>

#include <array>
#include <chrono>
#include <cstdlib>
#include <future>
#include <mdspan>
#include <memory>
#include <string>
#include <vector>

#include "assert.hpp"
#include "common.hpp"
#include "defer.hpp"
#include "log.hpp"
#include "timer.hpp"

namespace {

auto glfw_error_callback(int error_code, const char* description) -> void
{
    PRESENTER_ERROR("GLFW Error {}: {}", error_code, description);
}

auto glfw_framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height) -> void
{
    glViewport(0, 0, width, height);
}

#if defined(PT_OPENGL_DEBUG_CONTEXT)

auto gl_debug_message_callback([[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type, GLuint id, GLenum severity,
                               [[maybe_unused]] GLsizei length, const GLchar* message,
                               [[maybe_unused]] const void* user_param) -> void
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        PRESENTER_INFO("OpenGL Notification {}: {}", id, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        PRESENTER_WARN("OpenGL Warning {}: {}", id, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
    case GL_DEBUG_SEVERITY_HIGH:
        PRESENTER_ERROR("OpenGL Error {}: {}", id, message);
        break;
    default:
        PRESENTER_ASSERT(false);
    }
}

#endif

const auto vertex_shader_source = std::string{
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
})"
};

const auto fragment_shader_source = std::string{
    R"(#version 460 core

in vec2 TexCoords;

layout (location = 0) uniform sampler2D image;

out vec4 outColor;

void main()
{
    outColor = texture(image, TexCoords);
})"
};

constexpr u32 image_width = 640;
constexpr u32 image_height = 360;
constexpr u32 image_size = image_width * image_height;

constexpr u32 window_width = 1920;
constexpr u32 window_height = 1080;

const auto world = std::array<std::shared_ptr<const tracer::Object>, 2>{
    std::make_shared<tracer::Sphere>(glm::dvec3{ 0.0, 0.0, -1.0 }, 0.5),
    std::make_shared<tracer::Sphere>(glm::dvec3{ 0.0, -100.5, -1.0 }, 100.0)
};

} // namespace

auto main() -> int
{
    // There's a bug in VS runtime that can cause the application to deadlock when it exits when using asynchronous
    // loggers. Calling spdlog::shutdown() prevents that.
    Defer shutdown_spdlog{ [] { spdlog::shutdown(); } };

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
        PRESENTER_CRITICAL("Failed to initialize GLFW.");
        return EXIT_FAILURE;
    }

    Defer terminate_glfw{ [] { glfwTerminate(); } };

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(PT_OPENGL_DEBUG_CONTEXT)
    glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
#endif

    auto window = glfwCreateWindow(window_width, window_height, "Path Tracer", nullptr, nullptr);

    if (!window)
    {
        PRESENTER_CRITICAL("Failed to create a window.");
        return EXIT_FAILURE;
    }

    Defer destroy_window{ [&] { glfwDestroyWindow(window); } };

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        PRESENTER_CRITICAL("Failed to load OpenGL functions.");
        return EXIT_FAILURE;
    }

#if defined(PT_OPENGL_DEBUG_CONTEXT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_message_callback, nullptr);

        static constexpr auto disabled_messages = std::array<GLuint, 1>{
            131185, // Buffer detailed info from NVIDIA.
        };

        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE,
                              static_cast<GLsizei>(disabled_messages.size()), disabled_messages.data(), GL_FALSE);
    }
#endif

    glViewport(0, 0, window_width, window_height);
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

    PRESENTER_INFO("OpenGL Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    PRESENTER_INFO("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& imgui_io = ImGui::GetIO();
    imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    imgui_io.ConfigDpiScaleFonts = true;     // Automatically overwrite style.FontScaleDpi when monitor DPI changes.
    imgui_io.ConfigDpiScaleViewports = true; // Scale ImGui and platform windows when monitor DPI changes.

    auto monitor_scale = ImGui_ImplGlfw_GetContentScaleForWindow(window);
    auto& imgui_style = ImGui::GetStyle();
    imgui_style.ScaleAllSizes(monitor_scale);
    imgui_style.FontScaleDpi = monitor_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    Defer shut_down_imgui{ [] {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    } };

    auto vertex_source_data = vertex_shader_source.c_str();
    auto fragment_source_data = fragment_shader_source.c_str();

    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source_data, nullptr);
    glCompileShader(vertex_shader);
    GLint vertex_shader_is_compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_is_compiled);
    PRESENTER_ASSERT(vertex_shader_is_compiled == GL_TRUE);
    Defer delete_vertex_shader{ [&] { glDeleteShader(vertex_shader); } };

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source_data, nullptr);
    glCompileShader(fragment_shader);
    GLint fragment_shader_is_compiled;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_is_compiled);
    PRESENTER_ASSERT(fragment_shader_is_compiled == GL_TRUE);
    Defer delete_fragment_shader{ [&] { glDeleteShader(fragment_shader); } };

    auto shader = glCreateProgram();
    glAttachShader(shader, vertex_shader);
    glAttachShader(shader, fragment_shader);
    glLinkProgram(shader);
    GLint shader_is_linked;
    glGetProgramiv(shader, GL_LINK_STATUS, &shader_is_linked);
    PRESENTER_ASSERT(shader_is_linked == GL_TRUE);

    glDetachShader(shader, vertex_shader);
    glDetachShader(shader, fragment_shader);
    Defer delete_shader{ [&] { glDeleteProgram(shader); } };

    glUseProgram(shader);

    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    Defer delete_texture{ [&] { glDeleteTextures(1, &texture); } };

    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureStorage2D(texture, 1, GL_RGBA8, image_width, image_height);
    constexpr static auto black = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    glClearTexImage(texture, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(black));

    GLuint vertex_array;
    glCreateVertexArrays(1, &vertex_array);
    Defer delete_vertex_array{ [&] { glDeleteVertexArrays(1, &vertex_array); } };

    auto image = std::vector<glm::vec4>{};
    image.resize(image_size);
    const auto image_span = std::mdspan{ image.data(), image_height, image_width };

    auto render_result = std::async(std::launch::async, [image_span] {
        auto timer = Timer{};
        timer.start();

        tracer::render(tracer::CameraParams{}, tracer::RenderParams{}, image_span, world, [](i32 progress) {
            if (progress >= 100)
                PRESENTER_INFO("Progress: Done!");
            else
                PRESENTER_INFO("Progress: {}%", progress);
        });

        auto time_ms = timer.elapsed_ms();
        auto time_s = time_ms * 0.001;
        PRESENTER_INFO("Took {:.4f}s ({:.4f}ms).", time_s, time_ms);
    });

    glBindVertexArray(vertex_array);
    glBindTextureUnit(0, texture);

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (render_result.valid())
        {
            using namespace std::chrono_literals;

            if (render_result.wait_for(1ms) == std::future_status::ready)
            {
                render_result.get();
                glTextureSubImage2D(texture, 0, 0, 0, image_width, image_height, GL_RGBA, GL_FLOAT, image.data());
            }
        }

        ImGui::ShowDemoWindow();

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (imgui_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(window);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}
