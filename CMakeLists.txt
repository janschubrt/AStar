cmake_minimum_required(VERSION 3.20)
project(AStar)


set(CMAKE_CXX_STANDARD 20)


if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
    set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()


find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)


add_executable(${PROJECT_NAME}
        src/algo.cpp
        src/astar.cpp
        src/buffer.cpp
        src/renderer.cpp
        src/shader.cpp
        src/window.cpp
        src/algo.cpp
        src/algo.hpp
        src/globals.hpp
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