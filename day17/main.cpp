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
#include <vector>

struct vec2
{
  int x;
  int y;
};

struct aabb
{
  vec2 min;
  vec2 max;
};

vec2 operator+(vec2 lhs, vec2 rhs)
{
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  // input - target area: x=128..160, y=-142..-88
  const aabb target_area = {{128, -142}, {160, -88}};
  // const aabb target_area = {{20, -10}, {30, -5}};

  const auto contained = [](vec2 point, aabb area) {
    if (
      point.x >= area.min.x && point.x <= area.max.x && point.y >= area.min.y
      && point.y <= area.max.y) {
      return true;
    }
    return false;
  };

  int overall_highest_point = 0;
  for (int yv = 0; yv < 500; yv++) {
    for (int xv = 0; xv < 500; xv++) {
      auto projectile = vec2{0, 0};
      auto velocity = vec2{xv, yv};
      int highest_point = 0;
      for (int step = 0; step < 1500; ++step) {
        highest_point = std::max(highest_point, projectile.y);
        projectile = projectile + velocity;
        if (contained(projectile, target_area)) {
          overall_highest_point =
            std::max(highest_point, overall_highest_point);
        }
        velocity.x = std::max(0, velocity.x - 1);
        velocity.y -= 1;
      }
    }
  }

  std::cout << "part 1: " << overall_highest_point << '\n';
}
