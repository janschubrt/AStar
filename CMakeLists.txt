cmake_minimum_required(VERSION 3.20)
project(AStar)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic -Wno-ctad-maybe-unsupported -Wno-microsoft-redeclare-static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors -Wno-cast-function-type-strict -Wno-sign-conversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reserved-macro-identifier -Wno-unused-macros -Wno-global-constructors -Wno-switch-default")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -Werror")


find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)


add_executable(${PROJECT_NAME}
        src/astar.cpp
        src/buffer.cpp
        src/project.cpp
        src/renderer.cpp
        src/shader.cpp
        src/window.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE
        src
)
target_link_libraries(${PROJECT_NAME} PRIVATE
        glad::glad
        glfw
        glm::glm
        imgui::imgui
)