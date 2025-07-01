#include "astar.hpp"

#include <cassert>


AStar::AStar():
    m_window(this),
    m_renderer(m_window)
{

}


void AStar::run() const
{
    while (m_window.running())
    {
        glfwPollEvents();

        m_renderer.render();
    }
}


void AStar::framebufferSizeCallback([[maybe_unused]] GLFWwindow *handle, int width, int height)
{
    updateViewport({width, height});
}


void AStar::windowContentScaleCallback(GLFWwindow *handle, float xscale, float yscale)
{
    const AStar *astar = static_cast<AStar *>(glfwGetWindowUserPointer(handle));
    assert(astar != nullptr);

    astar->m_renderer.updateWindowScale({xscale, yscale});
}


void AStar::windowRefreshCallback(GLFWwindow *handle)
{
    const AStar *astar = static_cast<AStar *>(glfwGetWindowUserPointer(handle));
    assert(astar != nullptr);

    astar->m_renderer.render();
}


int main()
{
    const AStar astar;
    astar.run();

    return 0;
}