#pragma once


#include "astar.hpp"
#include "renderer.hpp"
#include "window.hpp"


class Project
{
public:
    Project();

    void run();


private:
    Window m_window;
    Renderer m_renderer;
    AStar m_astar;

    static void framebufferSizeCallback(GLFWwindow *handle, int width, int height);
    static void keyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods);
    static void windowContentScaleCallback(GLFWwindow *handle, float xscale, float yscale);
    static void windowRefreshCallback(GLFWwindow *handle);


    friend Window;
};
