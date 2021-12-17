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

struct node_t
{
  offset_t offset;
  int cost = INT_MAX;
};

int bfs(const std::vector<std::vector<int>>& grid, const offset_t& goal)
{
  auto find_neighbors = [&grid](
                          const offset_t& position,
                          const std::vector<node_t>& visited,
                          std::vector<node_t>& unvisited) {
    const offset_t offsets[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
    std::vector<node_t> neighbors;
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
              return next_position.r == visited_node.offset.r
                  && next_position.c == visited_node.offset.c;
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
          neighbors.push_back({next_position, cost});
        }
      }
    }
    return neighbors;
  };

  std::vector<node_t> unvisited;
  for (int row = 0; row < grid.size(); ++row) {
    for (int col = 0; col < grid.back().size(); ++col) {
      unvisited.push_back({offset_t{row, col}});
    }
  }
  unvisited[0].cost = 0;

  std::vector<node_t> visited;
  std::deque<node_t> path;
  path.push_back({{0, 0}, 0});

  while (!path.empty()) {
    auto path_node = path.front();
    visited.push_back(path_node);
    path.pop_front();
    auto neighbors = find_neighbors(path_node.offset, visited, unvisited);
    for (const auto& neighbor : neighbors) {
      if (
        std::find_if(
          path.begin(), path.end(),
          [neighbor](const node_t& node) {
            return node.offset.c == neighbor.offset.c
                && node.offset.r == neighbor.offset.r;
          })
        == path.end()) {
        path.push_back(neighbor);
      }
    }
    std::sort(path.begin(), path.end(), [](const auto& lhs, const auto& rhs) {
      return lhs.cost < rhs.cost;
    });
  }

  auto destination = std::find_if(
    unvisited.begin(), unvisited.end(), [&grid](const node_t& unvisited_node) {
      return unvisited_node.offset.c == grid.back().size() - 1
          && unvisited_node.offset.r == grid.size() - 1;
    });

  return destination->cost;
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
  // for (int row = 0; row < grid.size(); ++row) {
  //   for (int col = 0; col < grid.back().size(); ++col) {
  //     std::cout << grid[row][col] << ' ';
  //   }
  //   std::cout << '\n';
  // }

  std::cout << "part 1: "
            << bfs(grid, {int(grid.size() - 1), int(grid.back().size() - 1)})
            << '\n';

  std::vector<std::vector<int>> big_grid;
  big_grid.resize(grid.size() * 5);
  for (auto& row : big_grid) {
    row.resize(grid.back().size() * 5);
  }

  for (int row = 0; row < grid.size(); ++row) {
    for (int col = 0; col < grid.back().size(); ++col) {
      big_grid[row][col] = grid[row][col];
    }
  }

  for (int outer_c = 1; outer_c < 5; ++outer_c) {
    for (int row = 0; row < grid.size(); ++row) {
      for (int col = 0; col < grid.back().size(); ++col) {
        int r = row;
        int c = grid.back().size() * outer_c + col;
        int rb = row;
        int cb = grid.back().size() * (outer_c - 1) + col;
        int rv = big_grid[rb][cb];
        rv++;
        if (rv > 9) {
          rv = 1;
        }
        big_grid[r][c] = rv;
      }
    }
  }

  for (int col = 0; col < big_grid.size(); ++col) {
    for (int outer_row = 1; outer_row < 5; ++outer_row) {
      for (int row = 0; row < grid.size(); ++row) {
        int r = row + outer_row * grid.size();
        int rb = row + (outer_row - 1) * grid.size();
        int rv = big_grid[rb][col];
        rv++;
        if (rv > 9) {
          rv = 1;
        }
        big_grid[r][col] = rv;
      }
    }
  }

  // for (int row = 0; row < big_grid.size(); ++row) {
  //   for (int col = 0; col < big_grid.back().size(); ++col) {
  //     std::cout << big_grid[row][col];
  //   }
  //   std::cout << '\n';
  // }

  std::cout << "part 2: "
            << bfs(
                 big_grid,
                 {int(big_grid.size() - 1), int(big_grid.back().size() - 1)})
            << "\n";
}
