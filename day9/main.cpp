#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <vector>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> rows;
  for (std::string line; std::getline(reader, line);) {
    rows.push_back(line);
  }

  // check
  // for (const auto& row : rows) {
  //   std::cout << row << '\n';
  // }

  std::vector<std::vector<int>> heights;
  for (const auto& row : rows) {
    heights.emplace_back();
    for (const auto height : row) {
      int number = height - '0';
      heights.back().push_back(number);
    }
  }

  // for (const auto& row : heights) {
  //   for (const auto number : row) {
  //     std::cout << number;
  //   }
  //   std::cout << '\n';
  // }

  std::vector<int> lowest_heights;
  for (int row = 0; row < heights.size(); ++row) {
    for (int col = 0; col < heights.back().size(); ++col) {
      const int height = heights[row][col];
      bool smallest = true;
      int top = row - 1;
      int bottom = row + 1;
      int left = col - 1;
      int right = col + 1;
      if (top >= 0) {
        int th = heights[top][col];
        if (th <= height) {
          smallest = false;
        }
      }
      if (bottom < heights.size()) {
        int bh = heights[bottom][col];
        if (bh <= height) {
          smallest = false;
        }
      }
      if (left >= 0) {
        int lh = heights[row][left];
        if (lh <= height) {
          smallest = false;
        }
      }
      if (right < heights.back().size()) {
        int rh = heights[row][right];
        if (rh <= height) {
          smallest = false;
        }
      }
      if (smallest) {
        lowest_heights.push_back(height);
      }
    }
  }

  int sum = 0;
  for (const auto lh : lowest_heights) {
    sum += (lh + 1);
  }

  std::cout << "part 1: " << sum << '\n';

  std::vector<int> basin_sizes;
  std::unordered_map<int, bool> visited_heights;
  struct coord_t
  {
    int row;
    int col;
  };
  std::vector<coord_t> basin;

  const auto index_fn =
    [dimension = heights.back().size()](const int row, const int col) {
      return row * dimension + col;
    };

  for (int row = 0; row < heights.size(); ++row) {
    for (int col = 0; col < heights.back().size(); ++col) {
      const int height = heights[row][col];
      if (
        height == 9
        || visited_heights.find(index_fn(row, col)) != visited_heights.end()) {
        continue;
      }

      basin.push_back({row, col});
      visited_heights[index_fn(row, col)] = true;

      int basin_size = 0;
      while (!basin.empty()) {
        auto coord = basin.back();
        basin.pop_back();
        basin_size++;

        int top = coord.row - 1;
        int bottom = coord.row + 1;
        int left = coord.col - 1;
        int right = coord.col + 1;

        if (top >= 0) {
          int th = heights[top][coord.col];
          if (
            th != 9
            && visited_heights.find(index_fn(top, coord.col))
                 == visited_heights.end()) {
            visited_heights[index_fn(top, coord.col)] = true;
            basin.push_back({top, coord.col});
          }
        }
        if (bottom < heights.size()) {
          int bh = heights[bottom][coord.col];
          if (
            bh != 9
            && visited_heights.find(index_fn(bottom, coord.col))
                 == visited_heights.end()) {
            visited_heights[index_fn(bottom, coord.col)] = true;
            basin.push_back({bottom, coord.col});
          }
        }
        if (left >= 0) {
          int lh = heights[coord.row][left];
          if (
            lh != 9
            && visited_heights.find(index_fn(coord.row, left))
                 == visited_heights.end()) {
            visited_heights[index_fn(coord.row, left)] = true;
            basin.push_back({coord.row, left});
          }
        }
        if (right < heights.back().size()) {
          int rh = heights[coord.row][right];
          if (
            rh != 9
            && visited_heights.find(index_fn(coord.row, right))
                 == visited_heights.end()) {
            visited_heights[index_fn(coord.row, right)] = true;
            basin.push_back({coord.row, right});
          }
        }
      }
      basin_sizes.push_back(basin_size);
    }
  }

  std::sort(basin_sizes.begin(), basin_sizes.end());

  int64_t basin_product = 1;
  for (auto basin_it = basin_sizes.rbegin();
       basin_it < basin_sizes.rbegin() + 3; ++basin_it) {
    basin_product *= *basin_it;
  }

  std::cout << "part 2: " << basin_product << '\n';

  return 0;
}
