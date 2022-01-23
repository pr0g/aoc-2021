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

as::vec3i rotatate_point(as::vec3i point, int rotation)
{
  auto [x, y, z] = point;
  switch (rotation) {
    case 0:
      return {x, y, z};
    case 1:
      return {x, -z, y};
    case 2:
      return {x, -y, -z};
    case 3:
      return {x, z, -y};
    case 4:
      return {-x, -y, z};
    case 5:
      return {-x, -z, -y};
    case 6:
      return {-x, y, -z};
    case 7:
      return {-x, z, y};
    case 8:
      return {y, x, -z};
    case 9:
      return {y, -x, z};
    case 10:
      return {y, z, x};
    case 11:
      return {y, -z, -x};
    case 12:
      return {-y, x, z};
    case 13:
      return {-y, -x, -z};
    case 14:
      return {-y, -z, x};
    case 15:
      return {-y, z, -x};
    case 16:
      return {z, x, y};
    case 17:
      return {z, -x, -y};
    case 18:
      return {z, -y, x};
    case 19:
      return {z, y, -x};
    case 20:
      return {-z, x, -y};
    case 21:
      return {-z, -x, y};
    case 22:
      return {-z, y, x};
    case 23:
      return {-z, -y, -x};
    default:
      return {-1, -1, -1};
  }
}

bool operator==(const as::vec3i& lhs, const as::vec3i& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct vec3i_hash
{
  std::size_t operator()(const as::vec3i& vec) const
  {
    std::size_t seed = 0;
    hash_combine(seed, vec.x);
    hash_combine(seed, vec.y);
    hash_combine(seed, vec.z);
    return seed;
  }
};

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
  // for (int i = 0; i < scanner_strs.size(); ++i) {
  //   const auto& beacon_strs = scanner_strs[i];
  //   std::cout << "--- scanner " << i << " ---\n";
  //   for (const auto& beacon_str : beacon_strs) {
  //     std::cout << beacon_str << '\n';
  //   }
  // }

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

  // check
  // int scanner_index = 0;
  // for (const auto& scanner : scanners) {
  //   std::cout << "scanner " << scanner_index++ << '\n';
  //   for (const auto& beacon : scanner.beacons) {
  //     std::cout << beacon.x << ',' << beacon.y << ',' << beacon.z << '\n';
  //   }
  //   std::cout << '\n';
  // }

  std::unordered_set<as::vec3i, vec3i_hash> all_beacons;
  all_beacons.insert(scanners[0].beacons.begin(), scanners[0].beacons.end());

  std::vector<scanner_t> found_scanners(1, scanners[0]);
  std::vector<scanner_t> remaining_scanners;
  remaining_scanners.insert(
    remaining_scanners.end(), scanners.begin() + 1, scanners.end());

  std::vector<as::vec3i> scanner_positions(1, as::vec3i::zero());
  while (!remaining_scanners.empty()) {
    for (const auto& found_scanner : found_scanners) {
      for (int remaining_scanner_index = 0;
           remaining_scanner_index < remaining_scanners.size();
           ++remaining_scanner_index) {
        const auto& remaining_scanner =
          remaining_scanners[remaining_scanner_index];
        for (int rot = 0; rot < 24; ++rot) {
          std::unordered_map<as::vec3i, int64_t, vec3i_hash> offsets;
          for (const auto& found_beacon : found_scanner.beacons) {
            for (const auto& remaining_beacon : remaining_scanner.beacons) {
              auto rotated_point = rotatate_point(remaining_beacon, rot);
              auto offset = rotated_point - found_beacon;
              offsets[offset]++;
            }
          }
          for (const auto& offset : offsets) {
            if (offset.second >= 12) {
              scanner_positions.push_back(-offset.first);
              scanner_t next_found_scanner;
              for (const auto& remaining_beacon : remaining_scanner.beacons) {
                const auto rotated_point =
                  rotatate_point(remaining_beacon, rot);
                const auto adjusted_beacon = rotated_point - offset.first;
                next_found_scanner.beacons.push_back(adjusted_beacon);
                all_beacons.insert(adjusted_beacon);
              }
              found_scanners.push_back(next_found_scanner);
              remaining_scanners.erase(
                remaining_scanners.begin() + remaining_scanner_index);
              goto end;
            }
          }
        }
      }
    }
  end:
    int dummy;
  }

  int64_t max_distance = 0;
  for (int lhs = 0; lhs < scanner_positions.size() - 1; ++lhs) {
    for (int rhs = lhs + 1; rhs < scanner_positions.size(); ++rhs) {
      auto vec = scanner_positions[rhs] - scanner_positions[lhs];
      auto dist = std::abs(vec.x) + std::abs(vec.y) + std::abs(vec.z);
      if (dist > max_distance) {
        max_distance = dist;
      }
    }
  }

  std::cout << "part 1: " << all_beacons.size() << '\n';
  std::cout << "part 2: " << max_distance << '\n';
}
