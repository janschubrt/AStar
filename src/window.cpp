#include "window.hpp"

#include "astar.hpp"

#include "imgui_impl_glfw.h"

#include <cassert>
#include <iostream>


constexpr int WINDOW_HEIGHT = 1000;
constexpr int WINDOW_WIDTH = 1000;


void errorCallback([[maybe_unused]] int error_code, const char *description)
{
    std::cerr << "GLFW threw an Error: " << description << "\n";
}


Window::Window(AStar *astar)
{
    assert(astar != nullptr);
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
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_handle = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "AStar", nullptr, nullptr);
    if (!m_handle)
    {
        std::cerr << "GLFW could create the application window\n";
        return;
    }

    glfwSetWindowUserPointer(m_handle, astar);
    glfwSetFramebufferSizeCallback(m_handle, AStar::framebufferSizeCallback);
    glfwSetWindowContentScaleCallback(m_handle, AStar::windowContentScaleCallback);
    glfwSetWindowRefreshCallback(m_handle, AStar::windowRefreshCallback);
}


Window::~Window()
{
    glfwTerminate();
}


void Window::context() const
{
    glfwMakeContextCurrent(m_handle);
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