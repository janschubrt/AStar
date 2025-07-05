#pragma once


#include "buffer.hpp"

#include <glm/glm.hpp>

#include <memory>


class Algo;
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
    explicit Renderer(Window &window);
    Renderer(const Renderer &) = delete;
    ~Renderer();

    void operator=(Renderer &) = delete;

    void algo(Algo &algo);
    [[nodiscard]] bool automatic() const;
    [[nodiscard]] Buffer *buffer() const;

    void processClick(const glm::ivec2 &cursor_position);
    void updateWindowScale(const glm::ivec2 &size) const;

    void render();


private:
    Window &m_window;
    Algo *m_algo = nullptr;

    std::unique_ptr<Buffer> m_buffer;

    int m_noise_percent = 0;
    ClickMode m_mode = ClickMode::DEFAULT;
    bool m_automatic = true;


    void renderUI();
};