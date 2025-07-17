#include "project.hpp"

#include <cassert>


Project::Project():
    m_window(this),
    m_renderer(m_window),
    m_astar(m_window, m_renderer.buffer(), {80, 20}, {20, 80})
{
    m_renderer.astar(&m_astar);
}


void Project::run()
{
    while (m_window.running())
    {
        glfwPollEvents();

        m_renderer.processClick(m_window.cursorPosition());

        if (m_renderer.automatic())
        {
            m_astar.step();
        }

        m_renderer.render();
    }
}


void Project::framebufferSizeCallback([[maybe_unused]] GLFWwindow *handle, int width, int height)
{
    updateViewport({width, height});
}


void Project::keyCallback(GLFWwindow *handle, const int key, [[maybe_unused]] int scancode, const int action, [[maybe_unused]] int mods)
{
    auto *project = static_cast<Project *>(glfwGetWindowUserPointer(handle));
    assert(project != nullptr);

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ENTER)
        {
            project->m_astar.run();
        }
        else if (key == GLFW_KEY_ESCAPE)
        {
            project->m_astar.reset();
        }
    }
}


void Project::windowContentScaleCallback(GLFWwindow *handle, float xscale, float yscale)
{
    const Project *project = static_cast<Project *>(glfwGetWindowUserPointer(handle));
    assert(project != nullptr);

    project->m_renderer.updateWindowScale({xscale, yscale});
}


void Project::windowRefreshCallback(GLFWwindow *handle)
{
    auto *project = static_cast<Project *>(glfwGetWindowUserPointer(handle));
    assert(project != nullptr);

    project->m_renderer.render();
}


int main()
{
    Project project;
    project.run();

    return 0;
}
