#pragma once


#include "globals.hpp"

#include <glm/glm.hpp>

#include <bitset>
#include <queue>
#include <unordered_map>
#include <array>


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


class Algo
{
public:
    Algo(Window &window, Buffer *buffer, const glm::ivec2 &start, const glm::ivec2 &goal);

    void addBlocked(const glm::ivec2 &position);
    void removeBlocked(const glm::ivec2 &position);
    void reset();
    void start();
    void step();


private:
    glm::ivec2 m_start;
    glm::ivec2 m_goal;

    const int CAME_FROM_NONE = -1;

    std::bitset<GLOBALS::GRID_SIZE * GLOBALS::GRID_SIZE> m_visited_tiles;
    std::bitset<GLOBALS::GRID_SIZE * GLOBALS::GRID_SIZE> m_blocked_tiles;
    std::array<int, GLOBALS::GRID_SIZE * GLOBALS::GRID_SIZE> m_came_from;
    std::unordered_map<unsigned int, unsigned int> m_cost_g_map;

    std::priority_queue<Tile, std::vector<Tile>, std::greater<>> m_priority_queue;

    Buffer *m_buffer;
    Window &m_window;

    bool m_start_algo = false;


    void createPath();
};