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

template<class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct vec3
{
  int64_t x;
  int64_t y;
  int64_t z;
};

bool operator==(const vec3& lhs, const vec3& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

struct vec3_hash
{
  std::size_t operator()(const vec3& vec) const
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
  std::vector<std::string> reboot_step_strs;
  for (std::string line; std::getline(reader, line);) {
    reboot_step_strs.push_back(std::move(line));
  }

  // check
  // for (const auto& reboot_step : reboot_step_strs) {
  //   std::cout << reboot_step << '\n';
  // }

  struct reboot_step_t
  {
    bool on;
    vec3 min;
    vec3 max;
  };

  std::vector<reboot_step_t> reboot_steps;
  for (const auto& reboot_step_str : reboot_step_strs) {
    reboot_step_t reboot_step;
    auto space = reboot_step_str.find_first_of(' ');
    auto on_str = reboot_step_str.substr(0, space);
    reboot_step.on = on_str == "on";
    std::istringstream range_ss(
      reboot_step_str.substr(space + 1, reboot_step_str.size()));
    int element = 0;
    for (std::string range; std::getline(range_ss, range, ',');) {
      auto equals = range.find_first_of('=');
      auto dots = range.find_first_of("..");
      auto min = range.substr(equals + 1, dots - equals - 1);
      auto max = range.substr(dots + 2, range.size());
      if (element == 0) {
        reboot_step.min.x = std::stoi(min);
        reboot_step.max.x = std::stoi(max);
      } else if (element == 1) {
        reboot_step.min.y = std::stoi(min);
        reboot_step.max.y = std::stoi(max);
      } else {
        reboot_step.min.z = std::stoi(min);
        reboot_step.max.z = std::stoi(max);
      }
      element++;
    }
    reboot_steps.push_back(reboot_step);
  }

  // for (const auto& reboot_step : reboot_steps) {
  //   std::cout << (reboot_step.on ? "on" : "off") << " - min: ("
  //             << reboot_step.min.x << ", " << reboot_step.min.y << ", "
  //             << reboot_step.min.z << "), max: (" << reboot_step.max.x << ",
  //             "
  //             << reboot_step.max.y << ", " << reboot_step.max.z << ")\n";
  // }

  {
    std::unordered_map<vec3, bool, vec3_hash> cubes;
    for (const auto& reboot_step : reboot_steps) {
      if (
        reboot_step.min.x >= -50 && reboot_step.min.x <= 50
        && reboot_step.min.y >= -50 && reboot_step.min.y <= 50
        && reboot_step.min.z >= -50 && reboot_step.min.z <= 50) {
        for (int z = reboot_step.min.z; z <= reboot_step.max.z; ++z) {
          for (int y = reboot_step.min.y; y <= reboot_step.max.y; ++y) {
            for (int x = reboot_step.min.x; x <= reboot_step.max.x; ++x) {
              if (
                x >= -50 && x <= 50 && y >= -50 && y <= 50 && z >= -50
                && z <= 50) {
                cubes[vec3{x, y, z}] = reboot_step.on;
              }
            }
          }
        }
        const auto on_cubes =
          std::count_if(cubes.begin(), cubes.end(), [](const auto cube) {
            return cube.second;
          });

        //        std::cout << on_cubes << '\n';
      }
    }

    const auto on_cubes = std::count_if(
      cubes.begin(), cubes.end(), [](const auto cube) { return cube.second; });

    std::cout << "part 1: " << on_cubes << '\n';
  }

  {
    auto overlaps = [](const reboot_step_t& lhs, const reboot_step_t& rhs) {
      return (lhs.min.x <= rhs.max.x && lhs.max.x >= rhs.min.x)
          && (lhs.min.y <= rhs.max.y && lhs.max.y >= rhs.min.y)
          && (lhs.min.z <= rhs.max.z && lhs.max.z >= rhs.min.z);
    };

    auto create_range =
      [](bool on, const reboot_step_t& lhs, const reboot_step_t& rhs) {
        reboot_step_t next_reboot_step{};
        next_reboot_step.min.x = std::max(lhs.min.x, rhs.min.x);
        next_reboot_step.max.x = std::min(lhs.max.x, rhs.max.x);
        next_reboot_step.min.y = std::max(lhs.min.y, rhs.min.y);
        next_reboot_step.max.y = std::min(lhs.max.y, rhs.max.y);
        next_reboot_step.min.z = std::max(lhs.min.z, rhs.min.z);
        next_reboot_step.max.z = std::min(lhs.max.z, rhs.max.z);
        next_reboot_step.on = on;
        return next_reboot_step;
      };

    std::vector<reboot_step_t> visited;
    for (const auto& reboot_step : reboot_steps) {
      std::vector<reboot_step_t> next;
      for (const auto& step : visited) {
        if (overlaps(reboot_step, step)) {
          next.push_back(create_range(!step.on, reboot_step, step));
        }
      }
      if (reboot_step.on) {
        next.push_back(reboot_step);
      }
      visited.insert(visited.end(), next.begin(), next.end());
    }

    auto volume = [](const reboot_step_t& reboot_step) {
      return std::abs(
        (reboot_step.max.x - reboot_step.min.x + 1)
        * (reboot_step.max.y - reboot_step.min.y + 1)
        * (reboot_step.max.z - reboot_step.min.z + 1));
    };

    uint64_t total = 0;
    for (const auto& visitor : visited) {
      if (visitor.on) {
        total += volume(visitor);
      } else {
        total -= volume(visitor);
      }
    }
    std::cout << "part 2: " << total << '\n';
  }
}
