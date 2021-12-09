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
  for (int r = 0; r < heights.size(); ++r) {
    for (int c = 0; c < heights.back().size(); ++c) {
      const int h = heights[r][c];
      bool smallest = true;
      int t = r - 1;
      int b = r + 1;
      int l = c - 1;
      int rr = c + 1;
      if (t >= 0) {
        int th = heights[t][c];
        if (th <= h) {
          smallest = false;
        }
      }
      if (b < heights.size()) {
        int bh = heights[b][c];
        if (bh <= h) {
          smallest = false;
        }
      }
      if (l >= 0) {
        int lh = heights[r][l];
        if (lh <= h) {
          smallest = false;
        }
      }
      if (rr < heights.back().size()) {
        int rh = heights[r][rr];
        if (rh <= h) {
          smallest = false;
        }
      }
      if (smallest) {
        lowest_heights.push_back(h);
      }
    }
  }

  int sum = 0;
  for (const auto lh : lowest_heights) {
    sum += (lh + 1);
  }

  std::cout << "part 1: " << sum << '\n';

  return 0;
}
