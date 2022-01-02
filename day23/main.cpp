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

struct vec2
{
  int x;
  int y;
};

vec2 operator+(const vec2& lhs, const vec2& rhs)
{
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

bool operator==(const vec2& lhs, const vec2& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

int manhattan_distance(vec2 a, vec2 b)
{
  return std::abs((b.x - a.x) + (b.y - a.y));
}

struct amphipod_t
{
  vec2 pos;
  char type;
  int home;

  //  bool at_home() const { return pos.x == home; }
};

struct space_t
{
  vec2 pos;
  char occupant;
};

struct situation_t
{
  std::vector<space_t> spaces;

  std::vector<vec2> hallway;
  std::vector<vec2> rooms;
  std::vector<amphipod_t> amphipods;
};

std::vector<vec2> find_hallway_positions(
  amphipod_t amphipod, const std::vector<space_t>& all_spaces,
  const std::vector<vec2>& hallway)
{
  vec2 offsets[] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0},
                    {1, 0},   {-1, 1}, {0, 1},  {1, 1},  {-2, -2},
                    {-1, -2}, {0, -2}, {1, -2}, {2, -2}, {-2, -1},
                    {2, -1},  {-2, 0}, {2, 0},  {-2, 1}, {2, 1},
                    {-2, 2},  {-1, 2}, {0, 2},  {1, 2},  {2, 2}};

  std::vector<vec2> visited(1, amphipod.pos);
  std::vector<vec2> temp_positions(1, amphipod.pos);
  std::vector<vec2> hallway_positions;

  while (!temp_positions.empty()) {
    auto pos = temp_positions.back();
    temp_positions.pop_back();
    for (const auto& offset : offsets) {
      auto next = pos + offset;
      auto next_it = std::find_if(
        all_spaces.begin(), all_spaces.end(),
        [next](const auto space) { return next == space.pos; });
      if (next_it != all_spaces.end()) {
        if (next_it->occupant == '.') {
          auto exists = std::find(visited.begin(), visited.end(), next);
          if (exists == visited.end()) {
            temp_positions.push_back(next_it->pos);
            visited.push_back(next);
          }
          auto next_it_hall = std::find_if(
            hallway.begin(), hallway.end(),
            [next](const auto hallway_pos) { return next == hallway_pos; });
          if (next_it_hall != hallway.end()) {
            auto exists = std::find(
              hallway_positions.begin(), hallway_positions.end(),
              *next_it_hall);
            if (exists == hallway_positions.end()) {
              hallway_positions.push_back(*next_it_hall);
            }
          }
        }
      }
    }
  }
  return hallway_positions;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> lines;
  for (std::string line; std::getline(reader, line);) {
    lines.push_back(std::move(line));
  }

  for (const std::string& str : lines) {
    std::cout << str << '\n';
  }

  // need a 'state' that's the goal to check against


  situation_t situation;
  for (int row = 0; row < lines.size(); ++row) {
    for (int col = 0; col < lines[row].size(); ++col) {
      const auto cell = lines[row][col];
      if (cell == '.' && col != 3 && col != 5 && col != 7 & col != 9) {
        situation.spaces.push_back(space_t{vec2{col, row}, cell});
        situation.hallway.push_back(vec2{col, row});
      } else if (cell == 'A' || cell == 'B' || cell == 'C' || cell == 'D') {
        situation.spaces.push_back(space_t{vec2{col, row}, cell});
        situation.rooms.push_back(vec2{col, row});
        int home = [cell] {
          if (cell == 'A') {
            return 3;
          } else if (cell == 'B') {
            return 5;
          } else if (cell == 'C') {
            return 7;
          } else {
            return 9;
          }
        }();
        situation.amphipods.push_back(amphipod_t{vec2{col, row}, cell, home});
      }
    }
  }

  for (auto& amphipod : situation.amphipods) {
    if (
      (amphipod.type == 'A' && amphipod.pos == vec2{3, 3})
      || (amphipod.type == 'B' && amphipod.pos == vec2{5, 3})
      || (amphipod.type == 'B' && amphipod.pos == vec2{7, 3})
      || (amphipod.type == 'B' && amphipod.pos == vec2{9, 3})) {
      continue;
    }

    auto hallway_positions =
      find_hallway_positions(amphipod, situation.spaces, situation.hallway);

    for (const auto possible_position : hallway_positions) {
      // calculate cost to next position
      amphipod.pos = possible_position;
      auto prev_position = std::find_if(
        situation.spaces.begin(), situation.spaces.end(),
        [amphipod](const auto& space) { return space.pos == amphipod.pos; });
      if (prev_position != situation.spaces.end()) {
        prev_position->occupant = '.';
      }
      auto next_position = std::find_if(
        situation.spaces.begin(), situation.spaces.end(),
        [possible_position](const auto& space) {
          return space.pos == possible_position;
        });
      if (next_position != situation.spaces.end()) {
        next_position->occupant = amphipod.type;
      }
      break;
    }
  }

  // can only move to hallway once, then back to matching room

  auto t1 = manhattan_distance(situation.hallway[0], situation.rooms[0]);
  auto t2 = manhattan_distance(situation.hallway[3], situation.rooms[6]);

  int i;
  i = 0;

  return 0;
}
