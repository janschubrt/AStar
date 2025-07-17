#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string_view>


class Shader
{
public:
    Shader(std::string_view vertex_str, std::string_view fragment_str);
    Shader(Shader &) = delete;
    ~Shader();

    void operator=(Shader &) = delete;

    void use() const;

    void mat4(std::string_view name, const glm::mat4 &mat) const;


private:
    GLuint m_id;
};
