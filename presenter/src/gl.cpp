#include "gl.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>

#include <array>
#include <span>
#include <string>
#include <utility>

#include "assert.hpp"
#include "common.hpp"
#include "defer.hpp"

namespace presenter::gl {

VertexArray::VertexArray()
{
    glCreateVertexArrays(1, &_vertex_array_id);
}

VertexArray::~VertexArray()
{
    destroy();
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : _vertex_array_id{ std::exchange(other._vertex_array_id, GL_NONE) }
{}

auto VertexArray::operator=(VertexArray&& other) noexcept -> VertexArray&
{
    destroy();

    _vertex_array_id = std::exchange(other._vertex_array_id, GL_NONE);

    return *this;
}

auto VertexArray::bind() const -> void
{
    glBindVertexArray(_vertex_array_id);
}

auto VertexArray::destroy() -> void
{
    glDeleteVertexArrays(1, &_vertex_array_id);
}

Shader::Shader(const std::string& vertex_source, const std::string& fragment_source)
{
    auto vertex_shader_sources = std::array<const GLchar*, 1>{ vertex_source.c_str() };
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_sources.data(), nullptr);
    glCompileShader(vertex_shader);
    GLint vertex_shader_is_compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_is_compiled);
    PRESENTER_ASSERT(vertex_shader_is_compiled == GL_TRUE);
    Defer delete_vertex_shader{ [&] { glDeleteShader(vertex_shader); } };

    auto fragment_shader_sources = std::array<const GLchar*, 1>{ fragment_source.c_str() };
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_sources.data(), nullptr);
    glCompileShader(fragment_shader);
    GLint fragment_shader_is_compiled;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_is_compiled);
    PRESENTER_ASSERT(fragment_shader_is_compiled == GL_TRUE);
    Defer delete_fragment_shader{ [&] { glDeleteShader(fragment_shader); } };

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    Defer detach_vertex_shader{ [&] { glDetachShader(program, vertex_shader); } };
    glAttachShader(program, fragment_shader);
    Defer detach_fragment_shader{ [&] { glDetachShader(program, fragment_shader); } };
    glLinkProgram(program);
    GLint program_is_linked;
    glGetProgramiv(program, GL_LINK_STATUS, &program_is_linked);
    PRESENTER_ASSERT(program_is_linked == GL_TRUE);

    _program_id = program;
}

Shader::~Shader()
{
    destroy();
}

Shader::Shader(Shader&& other) noexcept : _program_id{ std::exchange(other._program_id, GL_NONE) } {}

auto Shader::operator=(Shader&& other) noexcept -> Shader&
{
    destroy();

    _program_id = std::exchange(other._program_id, GL_NONE);

    return *this;
}

auto Shader::bind() const -> void
{
    glUseProgram(_program_id);
}

auto Shader::destroy() -> void
{
    glDeleteProgram(_program_id);
}

Texture::Texture(u32 width, u32 height)
{
    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);

    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureStorage2D(texture, 1, GL_RGBA8, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

    _texture_id = texture;
    _width = width;
    _height = height;
}

Texture::~Texture()
{
    destroy();
}

Texture::Texture(Texture&& other) noexcept
    : _texture_id{ std::exchange(other._texture_id, GL_NONE) }, _width{ std::exchange(other._width, 0) },
      _height{ std::exchange(other._height, 0) }
{}

auto Texture::operator=(Texture&& other) noexcept -> Texture&
{
    destroy();

    _texture_id = std::exchange(other._texture_id, GL_NONE);
    _width = std::exchange(other._width, 0);
    _height = std::exchange(other._height, 0);

    return *this;
}

auto Texture::bind(u32 slot) const -> void
{
    glBindTextureUnit(slot, _texture_id);
}

auto Texture::upload(std::span<const glm::vec4> pixels) -> void
{
    PRESENTER_ASSERT(pixels.size() == static_cast<usize>(_width) * static_cast<usize>(_height));
    PRESENTER_ASSERT(pixels.size_bytes()
                     == static_cast<usize>(_width) * static_cast<usize>(_height) * sizeof(GLfloat) * 4);

    glTextureSubImage2D(_texture_id, 0, 0, 0, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height), GL_RGBA,
                        GL_FLOAT, pixels.data());
}

auto Texture::clear() -> void
{
    constexpr static auto black = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    glClearTexImage(_texture_id, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(black));
}

auto Texture::destroy() -> void
{
    glDeleteTextures(1, &_texture_id);
}

} // namespace presenter::gl
