#pragma once


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

    void vec2(std::string_view name, const glm::vec2 &value) const;
    void vec2(std::string_view name, float x, float y) const;
    void vec3(std::string_view name, const glm::vec3 &value) const;
    void vec3(std::string_view name, float x, float y, float z) const;
    void vec4(std::string_view name, const glm::vec4 &value) const;
    void vec4(std::string_view name, float x, float y, float z, float w) const;
    void mat2(std::string_view name, const glm::mat2 &mat) const;
    void mat3(std::string_view name, const glm::mat3 &mat) const;
    void mat4(std::string_view name, const glm::mat4 &mat) const;


private:
    unsigned int m_id;
};