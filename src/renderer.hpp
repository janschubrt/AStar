#pragma once


#include "buffer.hpp"

#include <glm/glm.hpp>

#include <memory>


class Window;


void updateViewport(const glm::ivec2 &size);


class Renderer
{
public:
    explicit Renderer(Window &window);
    Renderer(const Renderer &) = delete;
    ~Renderer();

    void operator=(Renderer &) = delete;

    [[nodiscard]] Buffer *buffer() const;
    void updateWindowScale(const glm::ivec2 &size) const;

    void render();


private:
    Window &m_window;

    std::unique_ptr<Buffer> m_buffer;

    int m_noise_percent = 0;
    bool m_automatic = true;


    void renderUI();
};