#pragma once

#include <glad/glad.h>

#include <string>

namespace presenter::gl {

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

} // namespace presenter::gl
