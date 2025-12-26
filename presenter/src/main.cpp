#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <stb_image_write.h>
#include <tracer/object.hpp>
#include <tracer/renderer.hpp>

#include <array>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include "assert.hpp"
#include "common.hpp"
#include "defer.hpp"
#include "gl.hpp"
#include "log.hpp"
#include "render_worker.hpp"
#include "ui.hpp"

namespace presenter {

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

[[nodiscard]] auto to_rgba8(glm::vec4 color) -> glm::vec<4, u8>
{
    color = glm::clamp(color, glm::vec4{ 0.0f }, glm::vec4{ 0.9999f });
    auto r = static_cast<u8>(color.r * 256.0f);
    auto g = static_cast<u8>(color.g * 256.0f);
    auto b = static_cast<u8>(color.b * 256.0f);
    auto a = static_cast<u8>(color.a * 256.0f);
    return glm::vec<4, u8>{ r, g, b, a };
}

auto write_image(const std::string& path, std::span<const glm::vec4> image, usize image_width, usize image_height)
    -> bool
{
    auto image_rgba8 = std::vector<glm::vec<4, u8>>{};
    image_rgba8.resize(image.size());

    for (usize i = 0; i < image.size(); i++)
        image_rgba8[i] = to_rgba8(image[i]);

    auto stride = image_width * sizeof(glm::vec<4, u8>);
    return stbi_write_png(path.c_str(), static_cast<int>(image_width), static_cast<int>(image_height), 4,
                          image_rgba8.data(), static_cast<int>(stride));
}

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

constexpr u32 window_width = 1920;
constexpr u32 window_height = 1080;

const auto world = std::array<std::shared_ptr<const tracer::Object>, 2>{
    std::make_shared<tracer::Sphere>(glm::dvec3{ 0.0, 0.0, -1.0 }, 0.5),
    std::make_shared<tracer::Sphere>(glm::dvec3{ 0.0, -100.5, -1.0 }, 100.0)
};

// Returns true if a restart of the render job is needed.
[[nodiscard]] auto tracer_ui(const RenderWorker& render_worker, tracer::Camera& camera,
                             tracer::RenderParams& render_params) -> bool
{
    auto restart = false;

    ImGui::Begin("Path Tracer");

    ImGui::ProgressBar(static_cast<float>(render_worker.progress()) / 100.0f);
    auto render_time_ms = render_worker.time_ms();
    auto render_time_s = render_time_ms / 1000.0;
    ImGui::Text("Took %.4fs (%.4fms)", render_time_s, render_time_ms);

    restart |= ImGui::Button("Generate");

    if (ImGui::Button("Save"))
    {
        auto& image = render_worker.image();
        write_image("image.png", image.pixels(), image.width(), image.height());
    }

    ImGui::SeparatorText("Camera");

    restart |= ui::drag("Position", camera.position, 0.1f);
    restart |= ui::drag("Focal Length", camera.focal_length, 0.1f);

    ImGui::SeparatorText("Render Params");

    restart |= ui::input_usize("Samples", render_params.samples);
    restart |= ui::input_usize("Max Depth", render_params.max_depth);

    ImGui::End();

    return restart;
}

auto run() -> int
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

    auto camera = tracer::Camera{};
    auto render_params = tracer::RenderParams{};

    auto render_worker = RenderWorker{ image_width, image_height, world, camera, render_params };

    auto image_vertex_array = gl::VertexArray{};
    auto image_shader = gl::Shader{ vertex_shader_source, fragment_shader_source };
    auto image_texture = gl::Texture{ image_width, image_height };
    image_texture.clear();

    image_vertex_array.bind();
    image_shader.bind();
    image_texture.bind(0);

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto render_status = render_worker.poll_status();

        if (render_status == RenderStatus::InProgress || render_status == RenderStatus::JustCompleted)
            image_texture.upload(render_worker.image().pixels());

        if (tracer_ui(render_worker, camera, render_params))
            render_worker.restart(world, camera, render_params);

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

} // namespace

} // namespace presenter

auto main() -> int
{
    return presenter::run();
}
