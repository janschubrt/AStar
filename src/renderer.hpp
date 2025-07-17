#pragma once


#include "buffer.hpp"

#include <glm/glm.hpp>

#include <memory>


class AStar;
class Window;


void updateViewport(const glm::ivec2 &size);


enum class ClickMode
{
    DEFAULT,
    START,
    GOAL
};


class Renderer
{
public:
    explicit Renderer(const Window &window);
    Renderer(const Renderer &) = delete;
    ~Renderer();

    void operator=(Renderer &) = delete;

    void astar(AStar *astar);
    [[nodiscard]] bool automatic() const;
    [[nodiscard]] Buffer *buffer() const;

    void processClick(const glm::ivec2 &cursor_position);
    void updateWindowScale(const glm::ivec2 &size) const;

    void render();


private:
    const Window &m_window;
    AStar *m_astar = nullptr;

    std::unique_ptr<Buffer> m_buffer;

    ClickMode m_click_mode = ClickMode::DEFAULT;
    int m_noise_percent = 0;
    int m_automatic = 1;        // Muss dank ImGui int sein.


    void renderUI();
};
