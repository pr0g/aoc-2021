#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> lines;
  for (std::string line; std::getline(reader, line);) {
    lines.push_back(std::move(line));
  }

  // for (const std::string& str : lines) {
  //   std::cout << str << '\n';
  // }

  // std::cout << "---\n";

  std::vector<std::vector<char>> sea_cucumbers;
  for (const std::string& line : lines) {
    sea_cucumbers.emplace_back();
    for (const auto c : line) {
      sea_cucumbers.back().push_back(c);
    }
  }

  // for (int row = 0; row < sea_cucumbers.size(); ++row) {
  //   for (int col = 0; col < sea_cucumbers.back().size(); ++col) {
  //     std::cout << sea_cucumbers[row][col];
  //   }
  //   std::cout << '\n';
  // }

  // std::cout << "---\n";

  int count = 0;
  bool moved;
  do {
    moved = false;
    auto sea_cucumbers_before = sea_cucumbers;

    for (int row = 0; row < sea_cucumbers.size(); ++row) {
      for (int col = 0; col < sea_cucumbers.back().size(); ++col) {
        if (sea_cucumbers_before[row][col] == '>') {
          if (
            sea_cucumbers_before[row][(col + 1) % sea_cucumbers_before.back().size()]
            == '.') {
            moved = true;
            sea_cucumbers[row][col] = '.';
            sea_cucumbers[row][(col + 1) % sea_cucumbers.back().size()] = '>';
          }
        }
      }
    }

    sea_cucumbers_before = sea_cucumbers;

    for (int row = 0; row < sea_cucumbers.size(); ++row) {
      for (int col = 0; col < sea_cucumbers.back().size(); ++col) {
        if (sea_cucumbers_before[row][col] == 'v') {
          if (sea_cucumbers_before[(row + 1) % sea_cucumbers_before.size()][col] == '.') {
            moved = true;
            sea_cucumbers[row][col] = '.';
            sea_cucumbers[(row + 1) % sea_cucumbers.size()][col] = 'v';
          }
        }
      }
    }
    count++;

    // for (int row = 0; row < sea_cucumbers.size(); ++row) {
    //   for (int col = 0; col < sea_cucumbers.back().size(); ++col) {
    //     std::cout << sea_cucumbers[row][col];
    //   }
    //   std::cout << '\n';
    // }

    // std::cout << "---\n";

  } while (moved);

  std::cout << "part 1: " << count << '\n';

  return 0;
}
