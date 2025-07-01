#pragma once


#define GLFW_INCLUDE_NONE


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class AStar;


class Window
{
public:
    explicit Window(AStar *astar);
    Window(Window &) = delete;
    ~Window();

    void operator=(Window &) = delete;

    void context() const;
    void initImGUI() const;
    [[nodiscard]] bool running() const;
    [[nodiscard]] glm::vec2 scale() const;
    [[nodiscard]] glm::ivec2 size() const;
    void swap() const;


private:
    GLFWwindow *m_handle;
};