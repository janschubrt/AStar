#pragma once


#include "shader.hpp"

#include <glm/glm.hpp>

#include <vector>


class Window;


enum class TileType
{
    CLEAR,
    BLOCKED,
    START,
    GOAL,
    VISITED,
    PATH
};


struct SSBData
{
    std::uint32_t position;
    std::uint32_t color;
};


class Buffer
{
public:
    explicit Buffer(const Window &window);
    Buffer(Buffer &) = delete;
    ~Buffer();

    void operator=(Buffer &) = delete;

    void clear();

    void updateScale(const glm::vec2 &scale);
    void updateTile(unsigned int index, TileType type);
    void updateTile(const glm::ivec2 &position, TileType type);

    void update() const;
    void render() const;


private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ssbo;

    Shader m_shader;

    glm::vec2 m_projection_scale;
    glm::ivec2 m_projection_size;

    std::vector<SSBData> m_ssb_data;


    void updateProjection() const;
};
