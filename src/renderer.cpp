#include "renderer.hpp"

#include "window.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>

#include <iostream>


constexpr unsigned int GL_SHADER_RECOMPILE_MSG   = 131218;


void updateViewport(const glm::ivec2 &size)
{
    glViewport(0, 0, size.x, size.y);
}


[[nodiscard]] bool blockDebugIds(const unsigned int id)
{
    if (id == GL_SHADER_RECOMPILE_MSG)
    {
        return true;
    }

    return false;
}


void openGLCallback(
            GLenum source,
            GLenum type,
            [[maybe_unused]] GLuint id,
            GLenum severity,
            [[maybe_unused]] GLsizei length,
            const char *message,
            [[maybe_unused]] const void *user)
{
    if (blockDebugIds(id))
    {
        return;
    }

    std::string source_str;
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        source_str = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source_str = "WINDOW_SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_str = "SHADER_COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source_str = "THIRD_PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        source_str = "APPLICATION";
        break;

    default:
        source_str = "OTHER";
        break;
    }

    std::string type_str;
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        type_str = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        type_str = "DEPRECATED_BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        type_str = "UNDEFINED_BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        type_str = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        type_str = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_MARKER:
        type_str = "MARKER";
        break;

    case GL_DEBUG_TYPE_PUSH_GROUP:
        type_str = "PUSH_GROUP";
        break;

    case GL_DEBUG_TYPE_POP_GROUP:
        type_str = "POP_GROUP";
        break;

    default:
        type_str = "OTHER";
        break;
    }

    std::string severity_str;
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW:
        severity_str = "LOW";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
    	severity_str = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_HIGH:
        severity_str = "HIGH";
        break;

    default:
        severity_str = "UNKNOWN";
        break;
    }

    std::cerr <<
        "OpenGL Debug Callback send a Message" <<
        "\nSeverity: " << severity_str <<
        "\nSource: " << source_str <<
        "\nType: " << type_str <<
        "\nMessage:\n" << message << "\n";
}


void initOpenGLDebug()
{
    int flags;

    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT))
    {
        std::cerr << "Could not initialize OpenGL Debug Output\n";
        return;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(openGLCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
}


void initUI(const Window &window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    window.initImGUI();

    const char *glsl_version = "#version 450 core";
    ImGui_ImplOpenGL3_Init(glsl_version);
}


Renderer::Renderer(Window &window):
    m_window(window)
{
    m_window.context();
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "GLAD was unable to load the OpenGL functions\n";
        return;
    }

    if (!GLAD_GL_VERSION_4_5)
    {
        std::cerr << "OpenGL 4.5 is not supported\n";
        return;
    }

    glfwSwapInterval(1);

    glEnable(GL_MULTISAMPLE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    initOpenGLDebug();
    initUI(m_window);

    m_buffer = std::make_unique<Buffer>(m_window);
}


Renderer::~Renderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


Buffer *Renderer::buffer() const
{
    return m_buffer.get();
}


void Renderer::updateWindowScale(const glm::ivec2 &size) const
{
    m_buffer->updateScale(size);
}


void Renderer::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    m_buffer->update();
    m_buffer->render();

    renderUI();

    m_window.swap();
}


void Renderer::renderUI()
{
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("A-Star");

    if (ImGui::Button(m_automatic ? "Switch to: Manual Mode" : "Switch to: Automatic Mode"))
    {
        m_automatic = !m_automatic;
    }

    if (ImGui::Button("<"))
    {

    }

    ImGui::SameLine();

    if (ImGui::Button(">"))
    {

    }

    ImGui::Text("Fill Plane with noise");
    ImGui::InputInt("## Input", &m_noise_percent);

    ImGui::SameLine();

    if (ImGui::Button("Noise!"))
    {

    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}