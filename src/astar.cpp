#include "astar.hpp"

#include <cassert>


AStar::AStar():
    m_window(this),
    m_renderer(m_window),
    m_algo(m_window, m_renderer.buffer(), {20, 20}, {80, 80})
{
    m_renderer.algo(m_algo);
}


void AStar::run()
{
    while (m_window.running())
    {
        glfwPollEvents();

        m_renderer.processClick(m_window.cursorPosition());

        if (m_renderer.automatic())
        {
            m_algo.step();
        }

        m_renderer.render();
    }
}


void AStar::framebufferSizeCallback([[maybe_unused]] GLFWwindow *handle, int width, int height)
{
    updateViewport({width, height});
}


void AStar::keyCallback(GLFWwindow *handle, const int key, [[maybe_unused]] int scancode, const int action, [[maybe_unused]] int mods)
{
    AStar *astar = static_cast<AStar *>(glfwGetWindowUserPointer(handle));
    assert(astar != nullptr);

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ENTER)
        {
            astar->m_algo.run();
        }
        else if (key == GLFW_KEY_ESCAPE)
        {
            astar->m_algo.reset();
        }
    }
}


void AStar::windowContentScaleCallback(GLFWwindow *handle, float xscale, float yscale)
{
    const AStar *astar = static_cast<AStar *>(glfwGetWindowUserPointer(handle));
    assert(astar != nullptr);

    astar->m_renderer.updateWindowScale({xscale, yscale});
}


void AStar::windowRefreshCallback(GLFWwindow *handle)
{
    AStar *astar = static_cast<AStar *>(glfwGetWindowUserPointer(handle));
    assert(astar != nullptr);

    astar->m_renderer.render();
}


int main()
{
    AStar astar;
    astar.run();

    return 0;
}