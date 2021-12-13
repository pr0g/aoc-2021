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

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> coordinates;
  for (std::string line; std::getline(reader, line);) {
    if (line.empty()) {
      break;
    }
    coordinates.push_back(std::move(line));
  }

  // check
  // for (const auto& coord : coordinates) {
  //   std::cout << coord << "\n";
  // }

  std::vector<std::string> folds;
  for (std::string line; std::getline(reader, line);) {
    folds.push_back(std::move(line));
  }

  struct fold_t
  {
    int fold_position;
    std::string axis;
  };

  std::vector<fold_t> folds_to_perform;
  for (const auto& fold : folds) {
    // std::cout << fold << "\n";
    auto split = fold.find('=');
    auto fold_position = std::stoi(fold.substr(split + 1, fold.size()));
    auto axis = fold.substr(split - 1, 1);
    folds_to_perform.push_back({fold_position, axis});
  }

  struct vec2_t
  {
    int x;
    int y;
  };

  std::vector<vec2_t> coords;
  for (const auto& coord : coordinates) {
    std::istringstream coord_ss(coord);
    int i = 0;
    int temp[2];
    for (std::string line; std::getline(coord_ss, line, ',');) {
      temp[i++] = std::stoi(line);
    }
    coords.push_back({temp[0], temp[1]});
  }

  // check
  // for (const auto& coord : coords) {
  //   std::cout << coord.x << '-' << coord.y << '\n';
  // }

  auto max_x = std::max_element(
    coords.begin(), coords.end(),
    [](const auto lhs, const auto rhs) { return lhs.x < rhs.x; });
  auto max_y = std::max_element(
    coords.begin(), coords.end(),
    [](const auto lhs, const auto rhs) { return lhs.y < rhs.y; });

  std::vector<std::vector<bool>> grid;
  grid.resize(max_y->y + 1);
  for (auto& row : grid) {
    row.resize(max_x->x + 1);
  }

  for (const auto coord : coords) {
    grid[coord.y][coord.x] = true;
  }

  // for (size_t row = 0; row < grid.size(); ++row) {
  //   for (size_t col = 0; col < grid.back().size(); ++col) {
  //     std::cout << (grid[row][col] ? '#' : '.');
  //   }
  //   std::cout << '\n';
  // }

  int part1 = -1;
  for (const auto& fold : folds_to_perform) {
    auto fold_position = fold.fold_position;
    if (fold.axis == "y") {
      for (size_t row = fold_position + 1; row < grid.size(); ++row) {
        for (size_t col = 0; col < grid.back().size(); ++col) {
          if (grid[row][col]) {
            grid[fold_position - (row - fold_position)][col] = true;
          }
        }
      }
      grid.resize(fold_position);
    }
    if (fold.axis == "x") {
      for (size_t row = 0; row < grid.size(); ++row) {
        for (size_t col = fold_position + 1; col < grid.back().size(); ++col) {
          if (grid[row][col]) {
            grid[row][fold_position - (col - fold_position)] = true;
          }
        }
      }
      for (auto& row : grid) {
        row.resize(fold_position);
      }
    }
    if (part1 == -1) {
      auto count = 0;
      for (size_t row = 0; row < grid.size(); ++row) {
        for (size_t col = 0; col < grid.back().size(); ++col) {
          count += grid[row][col] ? 1 : 0;
        }
      }
      part1 = count;
    }
  }

  std::cout << "part 1: " << part1 << '\n';
  std::cout << "part 2: " <<  '\n';

  for (size_t row = 0; row < grid.size(); ++row) {
    for (size_t col = 0; col < grid.back().size(); ++col) {
      std::cout << (grid[row][col] ? '#' : '.');
    }
    std::cout << '\n';
  }
}
