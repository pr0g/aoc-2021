#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<int> positions;
  for (std::string line; std::getline(reader, line, ',');) {
    positions.push_back(std::stoi(line));
  }

  // check
  for (const auto position : positions) {
    std::cout << position << ", ";
  }
  std::cout << '\n';

  int min_dist = INT_MAX;
  for (int i = 0; i < positions.size(); ++i) {
    int dist = 0;
    for (int p = 0; p < positions.size(); ++p) {
      if (i == p) {
        continue;
      }
      int diff = std::abs(positions[i] - positions[p]);
      dist += diff;
    }
    if (dist <= min_dist) {
      min_dist = dist;
    }
  }

  std::cout << "part 1: " << min_dist << '\n';

  return 0;
}
