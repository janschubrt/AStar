#pragma once


#include "global.hpp"

#include <glm/glm.hpp>

#include <array>
#include <bitset>
#include <queue>
#include <unordered_map>


class Buffer;
class Window;


class Tile
{
public:
    bool operator>(const Tile &other) const;

    [[nodiscard]] unsigned int cost() const;


    glm::ivec2 m_position;
    unsigned int m_cost_g;
    unsigned int m_cost_h;
};


class AStar
{
public:
    AStar(const Window &window, Buffer *buffer, const glm::ivec2 &start, const glm::ivec2 &goal);

    void addBlocked(const glm::ivec2 &position);
    void removeBlocked(const glm::ivec2 &position);

    void start(const glm::ivec2 &start);
    void goal(const glm::ivec2 &goal);

    void noise(int percentage);

    void pause();
    void reset();
    void resume();
    void run();
    [[nodiscard]] bool running() const;
    [[nodiscard]] bool started() const;
    void step();


private:
    glm::ivec2 m_start;
    glm::ivec2 m_goal;

    const int CAME_FROM_NONE = -1;
    bool m_start_algo = false;
    bool m_run_algo = false;

    std::bitset<GLOBAL::GRID_SIZE * GLOBAL::GRID_SIZE> m_visited_tiles;
    std::bitset<GLOBAL::GRID_SIZE * GLOBAL::GRID_SIZE> m_blocked_tiles;
    std::array<int, GLOBAL::GRID_SIZE * GLOBAL::GRID_SIZE> m_came_from;
    std::unordered_map<unsigned int, unsigned int> m_cost_g_map;

    std::priority_queue<Tile, std::vector<Tile>, std::greater<>> m_priority_queue;

    Buffer *m_buffer;
    const Window &m_window;


    void createPath() const;
};
