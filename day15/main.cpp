#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct offset_t
{
  int r;
  int c;
};

struct path_node_t
{
  offset_t offset;
  offset_t parent;
  int cost;
};

struct node_t
{
  offset_t offset;
  int cost = INT_MAX;
};

void bfs(const std::vector<std::vector<int>>& grid, const offset_t& goal)
{
  auto find_neighbors = [&grid](
                          const offset_t& position,
                          const std::vector<offset_t>& visited,
                          std::vector<node_t>& unvisited) {
    const offset_t offsets[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
    std::vector<offset_t> path;
    for (const auto& offset : offsets) {
      if (
        position.c + offset.c >= 0 && position.c + offset.c < grid.back().size()
        && position.r + offset.r >= 0 && position.r + offset.r < grid.size()) {
        const offset_t next_position = {
          position.r + offset.r, position.c + offset.c};
        if (
          std::find_if(
            visited.begin(), visited.end(),
            [next_position](const auto& visited_node) {
              return next_position.r == visited_node.r
                  && next_position.c == visited_node.c;
            })
          == visited.end()) {
          auto visited_node = std::find_if(
            unvisited.begin(), unvisited.end(),
            [position](const node_t& unvisited_node) {
              return unvisited_node.offset.c == position.c
                  && unvisited_node.offset.r == position.r;
            });
          const int cost =
            visited_node->cost + grid[next_position.r][next_position.c];
          auto unvisited_node = std::find_if(
            unvisited.begin(), unvisited.end(),
            [next_position](const node_t& unvisited_node) {
              return unvisited_node.offset.c == next_position.c
                  && unvisited_node.offset.r == next_position.r;
            });
          if (cost < unvisited_node->cost) {
            unvisited_node->cost = cost;
          }
          path.push_back(next_position);
        }
      }
    }
    return path;
  };

  std::vector<node_t> unvisited;
  for (int row = 0; row < grid.size(); ++row) {
    for (int col = 0; col < grid.back().size(); ++col) {
      unvisited.push_back({offset_t{row, col}});
    }
  }
  unvisited[0].cost = 0;

  std::vector<offset_t> visited;
  std::deque<offset_t> path;
  path.push_back({0, 0});

  while (!path.empty()) {
    auto path_node = path.front();
    if (path_node.r == goal.r && path_node.c == goal.c) {
      break;
    }
    path.pop_front();
    visited.push_back(path_node);
    auto neighbors = find_neighbors(path_node, visited, unvisited);
    for (const auto& next_path_node : neighbors) {
      if (
        std::find_if(
          path.begin(), path.end(),
          [next_path_node](const offset_t& node) {
            return node.c == next_path_node.c && node.r == next_path_node.r;
          })
        == path.end()) {
        path.push_back(next_path_node);
      }
    }
  }

  std::cout
    << "part 1: "
    << unvisited[(grid.size() * (grid.size() - 1)) + (grid.back().size() - 1)]
         .cost - 1
    << '\n';
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> rows;
  for (std::string line; std::getline(reader, line);) {
    rows.push_back(std::move(line));
  }

  std::vector<std::vector<int>> grid;
  for (const auto& row : rows) {
    grid.emplace_back();
    for (const auto c : row) {
      grid.back().push_back(c - '0');
    }
  }

  // check
  for (int row = 0; row < grid.size(); ++row) {
    for (int col = 0; col < grid.back().size(); ++col) {
      std::cout << grid[row][col] << ' ';
    }
    std::cout << '\n';
  }

  const offset_t goal = {int(grid.size() - 1), int(grid.back().size() - 1)};

  bfs(grid, goal);
}
