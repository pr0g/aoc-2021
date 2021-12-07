#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
#include <unordered_map>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<int> positions;
  for (std::string line; std::getline(reader, line, ',');) {
    positions.push_back(std::stoi(line));
  }

  // check
  // for (const auto position : positions) {
  //   std::cout << position << ", ";
  // }
  // std::cout << '\n';

  {
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
  }

  {
    // distance, cost
    std::unordered_map<int, int> dist_cost_lookup;
    auto max = std::max_element(positions.begin(), positions.end());
    int min_cost = INT_MAX;
    for (int i = 0; i < *max; ++i) {
      int cost = 0;
      for (int p = 0; p < positions.size(); ++p) {
        if (i == positions[p]) {
          continue;
        }
        int diff = std::abs(i - positions[p]);
         auto cost_it = dist_cost_lookup.find(diff);
         if (cost_it == dist_cost_lookup.end()) {
          int sum = 0;
          int counter = 1;
          for (int c = 0; c < diff; ++c) {
            sum += counter;
            counter += 1;
          }
           dist_cost_lookup[diff] = sum;
          cost += sum;
         } else {
           cost += cost_it->second;
         }
      }
      if (cost <= min_cost) {
        min_cost = cost;
      }
    }

    std::cout << "part 2: " << min_cost << '\n';
  }

  return 0;
}
