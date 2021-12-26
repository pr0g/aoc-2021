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

#include <as/as-math-ops.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");

  std::string ignore;
  std::getline(reader, ignore);

  std::vector<std::vector<std::string>> scanner_strs;
  scanner_strs.emplace_back();
  for (std::string line; std::getline(reader, line);) {
    if (line.empty()) {
      scanner_strs.emplace_back();
      std::getline(reader, ignore);
      continue;
    }
    scanner_strs.back().push_back(std::move(line));
  }

  // check
  for (int i = 0; i < scanner_strs.size(); ++i) {
    const auto& beacon_strs = scanner_strs[i];
    std::cout << "--- scanner " << i << " ---\n";
    for (const auto& beacon_str : beacon_strs) {
      std::cout << beacon_str << '\n';
    }
  }

  std::cout << "---\n";

  struct scanner_t
  {
    std::vector<as::vec3i> beacons;
  };

  std::vector<scanner_t> scanners;
  for (const auto& scanner : scanner_strs) {
    scanners.emplace_back();
    for (const auto& beacon_str : scanner) {
      as::vec3i vec;
      as::index i = 0;
      std::istringstream reader(beacon_str);
      for (std::string elem; std::getline(reader, elem, ',');) {
        vec[i++] = std::stoi(elem);
      }
      scanners.back().beacons.push_back(vec);
    }
  }

  for (const auto& scanner : scanners) {
    for (const auto& beacon : scanner.beacons) {
      std::cout << beacon.x << ',' << beacon.y << ',' << beacon.z << '\n';
    }
  }
}
