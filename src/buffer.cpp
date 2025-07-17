#include "buffer.hpp"

#include "global.hpp"
#include "window.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <string>


namespace SHADER
{
    constexpr std::uint32_t COLOR_CLEAR     = 0xffffffff;
    constexpr std::uint32_t COLOR_BLOCKED   = 0x00000000;
    constexpr std::uint32_t COLOR_START     = 0xff00ff00;
    constexpr std::uint32_t COLOR_GOAL      = 0xff0000ff;
    constexpr std::uint32_t COLOR_VISITED   = 0xffff0000;
    constexpr std::uint32_t COLOR_PATH      = 0xff00a5ff;


    const std::string BUFFER_VERTEX = R"glsl(
#version 450 core


struct SSBData
{
    uint position;
    uint color;
};


layout (location = 0) in vec2 vbo_position;
layout (std430, binding = 0) readonly buffer ssbo
{
    SSBData ssbo_data[];
};


uniform mat4 projection;


flat out uint v_color;
flat out vec2 v_local;


void main()
{
    SSBData data = ssbo_data[gl_InstanceID];

    vec2 extracted_position;
    extracted_position.x = bitfieldExtract(data.position, 16, 16);
    extracted_position.y = bitfieldExtract(data.position,  0, 16);

    v_local = vbo_position * vec2(10, 10);
    vec2 scaled_position = v_local + extracted_position;

    gl_Position = projection * vec4(scaled_position, 0.0, 1.0);
    v_color = data.color;
}
)glsl";


    const std::string BUFFER_FRAGMENT = R"glsl(
#version 450 core


out vec4 frag_color;


flat in uint v_color;
     in vec2 v_local;


void main()
{
    bool is_border =
        v_local.x <  1.0 ||
        v_local.x >= 9.0 ||
        v_local.y <  1.0 ||
        v_local.y >= 9.0;

    if (is_border) {
        frag_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        frag_color = unpackUnorm4x8(v_color);
    }
}
)glsl";


}


static std::uint32_t packPosition(const glm::ivec2 &position)
{
    return (static_cast<uint32_t>(position.x) & 0xFFFFu) << 16 |
        (static_cast<uint32_t>(position.y) & 0xFFFFu);
}


static std::uint32_t colorFromType(const TileType type)
{
    std::uint32_t color = 0xffffffff;

    switch (type)
    {
        case TileType::CLEAR:
            color = SHADER::COLOR_CLEAR;
            break;

        case TileType::BLOCKED:
            color = SHADER::COLOR_BLOCKED;
            break;

        case TileType::START:
            color = SHADER::COLOR_START;
            break;

        case TileType::GOAL:
            color = SHADER::COLOR_GOAL;
            break;

        case TileType::VISITED:
            color = SHADER::COLOR_VISITED;
            break;

        case TileType::PATH:
            color = SHADER::COLOR_PATH;
            break;
    }

    return color;
}


Buffer::Buffer(const Window &window):
    m_shader(SHADER::BUFFER_VERTEX, SHADER::BUFFER_FRAGMENT),
    m_projection_scale(window.scale()),
    m_projection_size(window.size())
{
    updateProjection();
    m_ssb_data.reserve(GLOBAL::GRID_SIZE * GLOBAL::GRID_SIZE);

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_vbo);
    glCreateBuffers(1, &m_ssbo);

    constexpr std::array vertices = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glNamedBufferData(m_vbo, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, 2 * sizeof(float));
    glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_vao, 0, 0);
    glEnableVertexArrayAttrib(m_vao, 0);

    for (int i = 0; i < GLOBAL::GRID_SIZE; i++)
    {
        for (int j = 0; j < GLOBAL::GRID_SIZE; j++)
        {
            SSBData data = {};
            data.position = packPosition({i * 10, j * 10});
            data.color = SHADER::COLOR_CLEAR;

            m_ssb_data.push_back(data);
        }
    }

    glNamedBufferData(m_ssbo, static_cast<GLsizeiptr>(m_ssb_data.size() * sizeof(SSBData)), m_ssb_data.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);
}


Buffer::~Buffer()
{
    glDeleteBuffers(1, &m_ssbo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}


void Buffer::clear()
{
    for (unsigned int i = 0; i < m_ssb_data.size(); i++)
    {
        updateTile(i, TileType::CLEAR);
    }
}


void Buffer::updateScale(const glm::vec2 &scale)
{
    m_projection_scale = scale;
    updateProjection();
}


void Buffer::updateTile(const unsigned int index, const TileType type)
{
    if (index > m_ssb_data.size())
    {
        return;
    }

    m_ssb_data[index].color = colorFromType(type);
}


void Buffer::updateTile(const glm::ivec2 &position, const TileType type)
{
    if (position.x >= GLOBAL::GRID_SIZE || position.y >= GLOBAL::GRID_SIZE || position.x < 0 || position.y < 0)
    {
        return;
    }

    const int index = position.x + position.y * GLOBAL::GRID_SIZE;
    m_ssb_data[index].color = colorFromType(type);
}


void Buffer::update() const
{
    glNamedBufferSubData(m_ssbo, 0, static_cast<GLsizeiptr>(m_ssb_data.size() * sizeof(SSBData)), m_ssb_data.data());
}


void Buffer::render() const
{
    m_shader.use();
    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, static_cast<int>(m_ssb_data.size()));
}


void Buffer::updateProjection() const
{
    const glm::vec2 scaled_size = glm::vec2(m_projection_size) * m_projection_scale;
    const glm::mat4 projection = glm::ortho(0.0f, scaled_size.x, scaled_size.y, 0.0f, -1.0f, 1.0f);

    m_shader.use();
    m_shader.mat4("projection", projection);
}
