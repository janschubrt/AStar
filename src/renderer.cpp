#include "renderer.hpp"

#include "window.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>

#include <iostream>

#include "algo.hpp"


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


void Renderer::algo(Algo &algo)
{
    m_algo = &algo;
}


bool Renderer::automatic() const
{
    return m_automatic;
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
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("A-Star");

    ImGui::Text("Mode:");
    if (ImGui::Button(m_automatic ? "Switch to: Manual Mode" : "Switch to: Automatic Mode"))
    {
        m_automatic = !m_automatic;
    }

    if (!m_automatic && m_algo != nullptr)
    {
        if (ImGui::Button("Step"))
        {
            m_algo->step();
        }

        ImGui::SameLine();
    }

    if (m_algo != nullptr)
    {
        if (!m_algo->started() && ImGui::Button("Start"))
        {
            m_algo->start();
        }
        else if (m_algo->started() && ImGui::Button("Reset"))
        {
            m_algo->reset();
        }

        if (m_algo->started())
        {
            ImGui::SameLine();
        }

        if (!m_automatic)
        {
            if (m_algo->running() && ImGui::Button("Pause"))
            {
                m_algo->pause();
            }
            else if (!m_algo->running() && m_algo->started() && ImGui::Button("Resume"))
            {
                m_algo->resume();
            }
        }
    }


    ImGui::NewLine();
    ImGui::Text("Fill Plane with Noise");
    ImGui::InputInt("## Input", &m_noise_percent);

    ImGui::SameLine();

    if (ImGui::Button("Go") && m_algo != nullptr)
    {
        m_algo->noise(m_noise_percent);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}