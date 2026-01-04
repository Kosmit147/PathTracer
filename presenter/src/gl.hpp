#pragma once

#include <glad/glad.h>
#include <glm/vec4.hpp>

#include <span>
#include <string>

#include "common.hpp"

namespace presenter::gl {

class VertexArray
{
public:
    explicit VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    auto operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&&) = delete;
    auto operator=(VertexArray&&) = delete;

    auto bind() const -> void;

private:
    GLuint _vertex_array_id = GL_NONE;
};

class Shader
{
public:
    explicit Shader(const std::string& vertex_source, const std::string& fragment_source);
    ~Shader();

    Shader(const Shader&) = delete;
    auto operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    auto operator=(Shader&&) = delete;

    auto bind() const -> void;

private:
    GLuint _program_id = GL_NONE;
};

class Texture
{
public:
    explicit Texture(u32 width, u32 height);
    ~Texture();

    Texture(const Texture&) = delete;
    auto operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    auto operator=(Texture&& other) noexcept -> Texture&;

    auto bind(u32 slot) const -> void;
    auto upload(std::span<const glm::vec4> pixels) -> void;
    auto clear() -> void;

    [[nodiscard]] auto width() const -> auto { return _width; }
    [[nodiscard]] auto height() const -> auto { return _height; }

private:
    GLuint _texture_id = GL_NONE;
    u32 _width = 0;
    u32 _height = 0;

private:
    auto destroy() -> void;
};

} // namespace presenter::gl
