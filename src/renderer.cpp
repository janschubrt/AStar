#include "renderer.hpp"

#include "astar.hpp"
#include "global.hpp"
#include "window.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>


void updateViewport(const glm::ivec2 &size)
{
    glViewport(0, 0, size.x, size.y);
}


[[nodiscard]] static bool blockDebugIds(const GLuint id)
{
    constexpr unsigned int GL_SHADER_RECOMPILE_MSG = 131218;

    if (id == GL_SHADER_RECOMPILE_MSG)
    {
        return true;
    }

    return false;
}


static void openGLCallback(
            GLenum source,
            GLenum type,
            GLuint id,
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


static void initOpenGLDebug()
{
    GLint flags;

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


static void initUI(const Window &window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    window.initImGUI();

    const auto glsl_version = "#version 450 core";
    ImGui_ImplOpenGL3_Init(glsl_version);
}


Renderer::Renderer(const Window &window):
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


void Renderer::astar(AStar *astar)
{
    m_astar = astar;
}


bool Renderer::automatic() const
{
    return m_automatic;
}


Buffer *Renderer::buffer() const
{
    return m_buffer.get();
}


void Renderer::processClick(const glm::ivec2 &cursor_position)
{
    if (m_astar == nullptr)
    {
        return;
    }

    const glm::ivec2 tile_position = {cursor_position.y / GLOBAL::TILE_SIZE, cursor_position.x / GLOBAL::TILE_SIZE};

    if (m_click_mode == ClickMode::START && m_window.cursorHeld(GLFW_MOUSE_BUTTON_LEFT))
    {
        m_astar->start(tile_position);
        m_click_mode = ClickMode::DEFAULT;
    }
    else if (m_click_mode == ClickMode::GOAL && m_window.cursorHeld(GLFW_MOUSE_BUTTON_LEFT))
    {
        m_astar->goal(tile_position);
        m_click_mode = ClickMode::DEFAULT;
    }
    else
    {
        if (m_window.cursorHeld(GLFW_MOUSE_BUTTON_LEFT))
        {
            m_astar->addBlocked(tile_position);
        }
        else if (m_window.cursorHeld(GLFW_MOUSE_BUTTON_RIGHT))
        {
            m_astar->removeBlocked(tile_position);
        }
    }
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
    if (m_astar == nullptr)
    {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::Begin("A-Star", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Configure Algorithm");
    ImGui::NewLine();

    ImGui::Text("You can use the left and right\nmouse buttons to add and remove blockades\nand press Enter to run the Algorithim\nin its current mode.\nPress Escape to reset it.");
    ImGui::NewLine();

    ImGui::TextUnformatted("Fill Plane with Noise (%):");
    ImGui::InputInt("## Input", &m_noise_percent);
    if (ImGui::Button("Fill"))
    {
        m_astar->noise(m_noise_percent);
    }

    ImGui::NewLine();
    ImGui::Text("Set Start/Goal on next Click:");
    if (ImGui::Button("Start"))
    {
        m_click_mode = ClickMode::START;
    }
    ImGui::SameLine();
    if (ImGui::Button("Goal"))
    {
        m_click_mode = ClickMode::GOAL;
    }

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::Text("Run Algorithm");
    ImGui::NewLine();

    ImGui::Text("Choose a Mode:");
    ImGui::RadioButton("Manual", &m_automatic, false);
    ImGui::SameLine();
    ImGui::RadioButton("Automatic", &m_automatic, true);

    ImGui::NewLine();
    ImGui::Text("Controls:");
    if (!m_astar->started() && ImGui::Button("Run"))
    {
        m_astar->run();
    }
    else if (m_astar->started() && ImGui::Button("Reset"))
    {
        m_astar->reset();
    }

    if (m_astar->started())
    {
        ImGui::SameLine();
    }

    if (m_automatic == true)
    {
        if (m_astar->running() && m_astar->started() && ImGui::Button("Pause"))
        {
            m_astar->pause();
        }
        else if (!m_astar->running() && m_astar->started() && ImGui::Button("Resume"))
        {
            m_astar->resume();
        }
    }
    else if (m_astar->started())
    {
        if (ImGui::Button("Step"))
        {
            m_astar->step();
        }
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
