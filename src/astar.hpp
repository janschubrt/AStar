#pragma once


#include "algo.hpp"
#include "renderer.hpp"
#include "window.hpp"


class AStar
{
public:
    AStar();

    void run();


private:
    Window m_window;
    Renderer m_renderer;
    Algo m_algo;

    static void framebufferSizeCallback(GLFWwindow *handle, int width, int height);
    static void keyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods);
    static void windowContentScaleCallback(GLFWwindow *handle, float xscale, float yscale);
    static void windowRefreshCallback(GLFWwindow *handle);


    friend Window;
};