#include "shader.hpp"

#include <iostream>
#include <string>


static void shrinkLog(std::string &log)
{
    const std::size_t position = log.find_first_of('\0');
    if (position != std::string::npos)
    {
        log.resize(position);
    }
}


static void checkShaderError(const GLuint element, const bool is_shader)
{
    constexpr int LOG_SIZE = 1024;

    GLint result;
    std::string log(LOG_SIZE, '\0');

    if (is_shader)
    {
        glGetShaderiv(element, GL_COMPILE_STATUS, &result);
        if (!result)
        {
            glGetShaderInfoLog(element, LOG_SIZE, nullptr, &log.front());
            shrinkLog(log);
            std::cerr << "OpenGL Shader Compilation Error:\n" << log;
        }
    }
    else
    {
        glGetProgramiv(element, GL_LINK_STATUS, &result);
        if (!result)
        {
            glGetProgramInfoLog(element, LOG_SIZE, nullptr, &log.front());
            shrinkLog(log);
            std::cerr << "OpenGL Program Link Error:\n" << log;
        }
    }
}


Shader::Shader(const std::string_view vertex_str, const std::string_view fragment_str)
{
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *vertex_code = vertex_str.data();

    glShaderSource(vertex_shader, 1, &vertex_code, nullptr);
    glCompileShader(vertex_shader);
    checkShaderError(vertex_shader, true);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar *fragment_code = fragment_str.data();

    glShaderSource(fragment_shader, 1, &fragment_code, nullptr);
    glCompileShader(fragment_shader);
    checkShaderError(fragment_shader, true);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertex_shader);
    glAttachShader(m_id, fragment_shader);
    glLinkProgram(m_id);
    checkShaderError(m_id, false);

    glDetachShader(m_id, vertex_shader);
    glDeleteShader(vertex_shader);
    glDetachShader(m_id, fragment_shader);
    glDeleteShader(fragment_shader);
}


Shader::~Shader()
{
    glDeleteProgram(m_id);
}


void Shader::use() const
{
    glUseProgram(m_id);
}


void Shader::mat4(const std::string_view name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.data()), 1, GL_FALSE, &mat[0][0]);
}
