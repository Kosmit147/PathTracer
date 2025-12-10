#include "gl.hpp"

#include <glad/glad.h>

#include <array>
#include <string>

#include "assert.hpp"
#include "defer.hpp"

namespace presenter::gl {

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
    glDeleteProgram(_program_id);
}

auto Shader::bind() const -> void
{
    glUseProgram(_program_id);
}

} // namespace presenter::gl
