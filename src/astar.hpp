#pragma once


#include "renderer.hpp"
#include "window.hpp"


class AStar
{
public:
    AStar();

    void run() const;


private:
    Window m_window;
    Renderer m_renderer;


    static void framebufferSizeCallback(GLFWwindow *handle, int width, int height);
    static void windowContentScaleCallback(GLFWwindow *handle, float xscale, float yscale);
    static void windowRefreshCallback(GLFWwindow *handle);


    friend Window;
};