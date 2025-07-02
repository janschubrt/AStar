#include "algo.hpp"

#include "buffer.hpp"
#include "window.hpp"

#include <string>


[[nodiscard]] unsigned int heuristic(glm::ivec2 current_position, glm::ivec2 goal_position)
{
    return std::abs(current_position.x - goal_position.x) + std::abs(current_position.y - goal_position.y);
}


[[nodiscard]] unsigned int index(const glm::ivec2 &position)
{
    return position.x + position.y * GLOBALS::GRID_SIZE;
}


bool Tile::operator>(const Tile &other) const
{
    if (this->cost() == other.cost())
    {
        return this->m_cost_h > other.m_cost_h;
    }
    return this->cost() > other.cost();
}


unsigned int Tile::cost() const
{
    return m_cost_g + m_cost_h;
}


Algo::Algo(Window &window, Buffer *buffer, const glm::ivec2 &start, const glm::ivec2 &goal):
    m_start(start),
    m_goal(goal),
    m_buffer(buffer),
    m_window(window)
{
    m_buffer->updateTile(start, TileType::START);
    m_buffer->updateTile(goal, TileType::GOAL);
}


void Algo::addBlocked(const glm::ivec2 &position)
{
    if (m_start_algo || position.x >= GLOBALS::GRID_SIZE || position.y >= GLOBALS::GRID_SIZE)
    {
        return;
    }

    m_blocked_tiles[index(position)] = true;
    m_buffer->updateTile(position, TileType::BLOCKED);
}


void Algo::reset()
{
    m_start_algo = false;

    m_buffer->clear();
    m_buffer->updateTile(m_start, TileType::START);
    m_buffer->updateTile(m_goal, TileType::GOAL);

    m_visited_tiles.reset();
    m_blocked_tiles.reset();
    std::fill(m_came_from.begin(), m_came_from.end(), CAME_FROM_NONE);
    m_cost_g_map.clear();
    m_priority_queue = std::priority_queue<Tile, std::vector<Tile>, std::greater<>>();

    std::string title = "AStar";
    m_window.title(title);
}


void Algo::start()
{
    m_start_algo = true;

    Tile start_tile = {
            m_start,
            0,
            heuristic(m_start, m_goal)
    };

    m_visited_tiles[index(m_start)] = true;
    m_priority_queue.push(start_tile);
    m_came_from[index(m_start)] = CAME_FROM_NONE;
}


void Algo::step()
{
    if (!m_start_algo || m_visited_tiles[index(m_goal)] || m_priority_queue.empty())
    {
        return;
    }

    Tile current = m_priority_queue.top();
    m_priority_queue.pop();

    if (current.m_cost_g > m_cost_g_map[index(current.m_position)])
    {
        return;
    }

    m_visited_tiles[index(current.m_position)] = true;

    if (current.m_position == m_goal)
    {
        createPath();
        return;
    }

    constexpr std::array<glm::ivec2, 4> directions = {
            glm::ivec2(0, 1),
            glm::ivec2(1, 0),
            glm::ivec2(0, -1),
            glm::ivec2(-1, 0)
    };

    for (const glm::ivec2 &offset : directions)
    {
        glm::ivec2 neighbor_position = current.m_position + offset;

        if (neighbor_position.x < 0 || neighbor_position.y < 0 ||
            neighbor_position.x >= GLOBALS::GRID_SIZE || neighbor_position.y >= GLOBALS::GRID_SIZE)
        {
            continue;
        }

        unsigned int neighbour_index = index(neighbor_position);
        if (m_blocked_tiles[neighbour_index])
        {
            continue;
        }

        unsigned int new_g = current.m_cost_g + 1;
        if (!m_cost_g_map.contains(neighbour_index) || new_g < m_cost_g_map[neighbour_index])
        {
            m_cost_g_map[neighbour_index] = new_g;
            m_came_from[neighbour_index] = index(current.m_position);

            Tile neighbour_tile = {
                    neighbor_position,
                    new_g,
                    heuristic(neighbor_position, m_goal)
            };
            m_priority_queue.push(neighbour_tile);

            if (neighbor_position != m_start && neighbor_position != m_goal)
            {
                m_buffer->updateTile(neighbor_position, TileType::VISITED);
            }
        }
    }
}


void Algo::createPath()
{
    int came_from_index = index(m_goal);
    int count = 1;

    while (came_from_index != CAME_FROM_NONE)
    {
        if (came_from_index != static_cast<int>(index(m_start)) && came_from_index != static_cast<int>(index(m_goal)))
        {
            m_buffer->updateTile(came_from_index, TileType::PATH);
        }

        came_from_index = m_came_from[came_from_index];
        count++;
    }

    std::string title = "AStar - Path length" + std::to_string(count);
    m_window.title(title);
}