#pragma once


#define GLFW_INCLUDE_NONE


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string_view>


class Project;


class Window
{
public:
    explicit Window(Project *project);
    Window(Window &) = delete;
    ~Window();

    void operator=(Window &) = delete;

    void context() const;
    [[nodiscard]] bool cursorHeld(int button) const;
    [[nodiscard]] glm::ivec2 cursorPosition() const;
    void initImGUI() const;
    [[nodiscard]] bool running() const;
    [[nodiscard]] glm::vec2 scale() const;
    [[nodiscard]] glm::ivec2 size() const;
    void swap() const;
    void title(std::string_view title) const;


private:
    GLFWwindow *m_handle;
};
