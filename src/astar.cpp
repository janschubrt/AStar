#include "astar.hpp"

#include "buffer.hpp"
#include "window.hpp"

#include <random>
#include <string>


[[nodiscard]] static unsigned int heuristic(const glm::ivec2 &current_position, const glm::ivec2 &goal_position)
{
    return std::abs(current_position.x - goal_position.x) + std::abs(current_position.y - goal_position.y);
}


[[nodiscard]] static unsigned int index(const glm::ivec2 &position)
{
    return position.x + position.y * GLOBAL::GRID_SIZE;
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


AStar::AStar(const Window &window, Buffer *buffer, const glm::ivec2 &start, const glm::ivec2 &goal):
    m_start(start),
    m_goal(goal),
    m_came_from({CAME_FROM_NONE}),
    m_buffer(buffer),
    m_window(window)
{
    m_buffer->updateTile(start, TileType::START);
    m_buffer->updateTile(goal, TileType::GOAL);
}


void AStar::addBlocked(const glm::ivec2 &position)
{
    if (m_run_algo || position.x >= GLOBAL::GRID_SIZE || position.y >= GLOBAL::GRID_SIZE || position.x < 0 || position.y < 0)
    {
        return;
    }

    if (position != m_start && position != m_goal)
    {
        m_blocked_tiles[index(position)] = true;
        m_buffer->updateTile(position, TileType::BLOCKED);
    }
}


void AStar::removeBlocked(const glm::ivec2 &position)
{
    if (m_run_algo || position.x >= GLOBAL::GRID_SIZE || position.y >= GLOBAL::GRID_SIZE || position.x < 0 || position.y < 0)
    {
        return;
    }

    if (position != m_start && position != m_goal)
    {
        m_blocked_tiles[index(position)] = false;
        m_buffer->updateTile(position, TileType::CLEAR);
    }
}


void AStar::start(const glm::ivec2 &start)
{
    if (m_start_algo)
    {
        return;
    }

    m_buffer->updateTile(m_start, TileType::CLEAR);
    m_start = start;
    m_buffer->updateTile(m_start, TileType::START);
}


void AStar::goal(const glm::ivec2 &goal)
{
    if (m_start_algo)
    {
        return;
    }

    m_buffer->updateTile(m_goal, TileType::CLEAR);
    m_goal = goal;
    m_buffer->updateTile(m_goal, TileType::GOAL);
}


void AStar::noise(int percentage)
{
    reset();

    if (percentage > 100)
    {
        percentage = 100;
    }
    else if (percentage < 0)
    {
        percentage = 0;
    }

    const int blocked_count = GLOBAL::GRID_SIZE * GLOBAL::GRID_SIZE * percentage / 100;

    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution distribution_x(0, GLOBAL::GRID_SIZE - 1);
    std::uniform_int_distribution distribution_y(0, GLOBAL::GRID_SIZE - 1);

    for (int i = 0; i < blocked_count; i++)
    {
        int x = distribution_x(generator);
        int y = distribution_y(generator);

        glm::ivec2 position(x, y);


        if (position != m_start && position != m_goal)
        {
            addBlocked({x, y});
        }
    }
}


void AStar::pause()
{
    m_run_algo = false;

    m_window.title("AStar - Paused");
}


void AStar::reset()
{
    m_start_algo = false;
    m_run_algo = false;

    m_buffer->clear();
    m_buffer->updateTile(m_start, TileType::START);
    m_buffer->updateTile(m_goal, TileType::GOAL);

    m_visited_tiles.reset();
    m_blocked_tiles.reset();
    std::ranges::fill(m_came_from, CAME_FROM_NONE);
    m_cost_g_map.clear();
    m_priority_queue = std::priority_queue<Tile, std::vector<Tile>, std::greater<>>();

    m_window.title("AStar");
}


void AStar::resume()
{
    m_run_algo = true;

    m_window.title("AStar - Searching...");
}


void AStar::run()
{
    m_start_algo = true;
    m_run_algo = true;

    const Tile start_tile = {
        m_start,
        0,
        heuristic(m_start, m_goal)
    };

    m_visited_tiles[index(m_start)] = true;
    m_priority_queue.push(start_tile);
    m_came_from[index(m_start)] = CAME_FROM_NONE;

    m_window.title("AStar - Searching...");
}


bool AStar::running() const
{
    return m_run_algo;
}


bool AStar::started() const
{
    return m_start_algo;
}


void AStar::step()
{
    if (!m_run_algo || m_visited_tiles[index(m_goal)])
    {
        return;
    }
    if (m_priority_queue.empty())
    {
        m_window.title("AStar - No Path");
        return;
    }

    const Tile current = m_priority_queue.top();
    m_priority_queue.pop();

    const unsigned int current_index = index(current.m_position);

    if (current.m_cost_g > m_cost_g_map[current_index])
    {
        return;
    }

    m_visited_tiles[current_index] = true;

    if (current.m_position == m_goal) [[unlikely]]
    {
        createPath();
        return;
    }

    constexpr std::array directions = {
            glm::ivec2(0, 1),
            glm::ivec2(1, 0),
            glm::ivec2(0, -1),
            glm::ivec2(-1, 0)
    };

    for (const glm::ivec2 &offset : directions)
    {
        glm::ivec2 neighbor_position = current.m_position + offset;

        if (neighbor_position.x < 0 || neighbor_position.y < 0 ||
            neighbor_position.x >= GLOBAL::GRID_SIZE || neighbor_position.y >= GLOBAL::GRID_SIZE) [[unlikely]]
        {
            continue;
        }

        unsigned int neighbour_index = index(neighbor_position);
        if (m_blocked_tiles[neighbour_index])
        {
            continue;
        }

        const unsigned int new_g = current.m_cost_g + 1;
        if (!m_cost_g_map.contains(neighbour_index) || new_g < m_cost_g_map[neighbour_index])
        {
            m_cost_g_map[neighbour_index] = new_g;
            m_came_from[neighbour_index] = static_cast<int>(current_index);

            Tile neighbor_tile = {
                    neighbor_position,
                    new_g,
                    heuristic(neighbor_position, m_goal)
            };
            m_priority_queue.push(neighbor_tile);

            if (neighbor_position != m_start && neighbor_position != m_goal)
            {
                m_buffer->updateTile(neighbor_position, TileType::VISITED);
            }
        }
    }
}


void AStar::createPath() const
{
    int came_from_index = static_cast<int>(index(m_goal));
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

    m_window.title("AStar - Path length " + std::to_string(count));
}
