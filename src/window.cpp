#include "window.hpp"

#include "project.hpp"

#include <imgui_impl_glfw.h>

#include <cassert>
#include <iostream>


static void errorCallback([[maybe_unused]] int error_code, const char *description)
{
    std::cerr << "GLFW threw an Error: " << description << "\n";
}


Window::Window(Project *project)
{
    assert(project != nullptr);
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
    {
        std::cerr << "GLFW cloud not be initialized\n";
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    m_handle = glfwCreateWindow(1000, 1000, "AStar", nullptr, nullptr);
    if (!m_handle)
    {
        std::cerr << "GLFW could not create the application window\n";
        return;
    }

    glfwSetWindowUserPointer(m_handle, project);
    glfwSetFramebufferSizeCallback(m_handle, Project::framebufferSizeCallback);
    glfwSetKeyCallback(m_handle, Project::keyCallback);
    glfwSetWindowContentScaleCallback(m_handle, Project::windowContentScaleCallback);
    glfwSetWindowRefreshCallback(m_handle, Project::windowRefreshCallback);
}


Window::~Window()
{
    glfwTerminate();
}


void Window::context() const
{
    glfwMakeContextCurrent(m_handle);
}


bool Window::cursorHeld(const int button) const
{
    return glfwGetMouseButton(m_handle, button) == GLFW_PRESS;
}


glm::ivec2 Window::cursorPosition() const
{
    glm::dvec2 d_position;
    glfwGetCursorPos(m_handle, &d_position.x, &d_position.y);

    const glm::ivec2 i_position = d_position;
    return i_position;
}


void Window::initImGUI() const
{
    ImGui_ImplGlfw_InitForOpenGL(m_handle, true);
}


bool Window::running() const
{
    return !glfwWindowShouldClose(m_handle);
}


glm::vec2 Window::scale() const
{
    glm::vec2 scale;
    glfwGetWindowContentScale(m_handle, &scale.x, &scale.y);

    return scale;
}



glm::ivec2 Window::size() const
{
    glm::ivec2 size;
    glfwGetWindowSize(m_handle, &size.x, &size.y);

    return size;
}


void Window::swap() const
{
    glfwSwapBuffers(m_handle);
}


void Window::title(const std::string_view title) const
{
    glfwSetWindowTitle(m_handle, title.data());
}
