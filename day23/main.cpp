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

vec2 operator-(const vec2& lhs, const vec2& rhs)
{
  return {lhs.x - rhs.x, lhs.y - rhs.y};
}

bool operator==(const vec2& lhs, const vec2& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

int manhattan_distance(vec2 a, vec2 b)
{
  return std::abs(b.x - a.x) + std::abs(b.y - a.y);
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

struct node_t
{
  vec2 position;
  int cost;
};

int amphipod_cost(char amphipod)
{
  switch (amphipod) {
    case 'A':
      return 1;
    case 'B':
      return 10;
    case 'C':
      return 100;
    case 'D':
      return 1000;
    default:
      std::cout << "error\n";
      return 0;
  }
}

template<typename T>
int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}

bool check(
  vec2 amphipod, vec2 delta, vec2 offset,
  const std::vector<space_t>& all_spaces)
{
  int xp = amphipod.x + delta.x * sgn(offset.x);
  int yp = amphipod.y + delta.y * sgn(offset.y);
  if (vec2{xp, yp} == amphipod) {
    return false;
  }
  auto f = std::find_if(
    all_spaces.begin(), all_spaces.end(), [xp, yp](const auto space) {
      return vec2{xp, yp} == space.pos;
    });
  if (
    f == all_spaces.end()
    || (f != all_spaces.end() && (f->occupant == 'A' || f->occupant == 'B' || f->occupant == 'C' || f->occupant == 'D'))) {
    return true;
  }
  return false;
}

std::vector<node_t> find_hallway_positions(
  amphipod_t amphipod, const std::vector<space_t>& all_spaces,
  const std::vector<vec2>& hallway)
{
  vec2 offsets[] = {{-1, -1}, {0, -1},  {1, -1},  {-1, 0},  {1, 0},  {-1, 1},
                    {0, 1},   {1, 1},   {-2, -2}, {-1, -2}, {0, -2}, {1, -2},
                    {2, -2},  {-2, -1}, {2, -1},  {-2, 0},  {2, 0},  {-2, 1},
                    {2, 1},   {-2, 2},  {-1, 2},  {0, 2},   {1, 2},  {2, 2}};

  std::vector<vec2> visited(1, amphipod.pos);
  std::vector<vec2> temp_positions(1, amphipod.pos);
  std::vector<node_t> hallway_costs;

  int move_cost = amphipod_cost(amphipod.type);

  while (!temp_positions.empty()) {
    auto pos = temp_positions.back();
    temp_positions.pop_back();
    for (const auto& offset : offsets) {
      auto next = pos + offset;
      auto delta = next - amphipod.pos;
      bool blocked_1 = false;
      bool blocked_2 = false;
      {
        int y = 0;
        int x = 0;
        for (; y < std::abs(delta.y); ++y) {
          blocked_1 |= check(amphipod.pos, {x, y}, delta, all_spaces);
        }
        for (; x < std::abs(delta.x); ++x) {
          blocked_1 |= check(amphipod.pos, {x, y}, delta, all_spaces);
        }
      }
      {
        int y = 0;
        int x = 0;
        for (; x < std::abs(delta.x); ++x) {
          blocked_2 |= check(amphipod.pos, {x, y}, delta, all_spaces);
        }
        for (; y < std::abs(delta.y); ++y) {
          blocked_2 |= check(amphipod.pos, {x, y}, delta, all_spaces);
        }
      }
      if (blocked_1 && blocked_2) {
        continue;
      }
      auto next_it = std::find_if(
        all_spaces.begin(), all_spaces.end(),
        [next](const auto space) { return next == space.pos; });
      if (next_it != all_spaces.end()) {
        if (next_it->occupant == '.') {
          auto visited_exists = std::find(visited.begin(), visited.end(), next);
          if (visited_exists == visited.end()) {
            temp_positions.push_back(next_it->pos);
            visited.push_back(next);
          }
          auto next_it_hall = std::find_if(
            hallway.begin(), hallway.end(),
            [next](const auto hallway_pos) { return next == hallway_pos; });
          if (next_it_hall != hallway.end()) {
            auto hallway_exists = std::find_if(
              hallway_costs.begin(), hallway_costs.end(),
              [next_it_hall](const auto hallway_cost) {
                return *next_it_hall == hallway_cost.position;
              });
            if (hallway_exists == hallway_costs.end()) {
              hallway_costs.push_back(
                {*next_it_hall,
                 manhattan_distance(amphipod.pos, *next_it_hall) * move_cost});
            }
          }
        }
      }
    }
  }
  return hallway_costs;
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
      } else if (cell == '.' && (col == 3 || col == 5 || col == 7 || col == 9)) {
        situation.spaces.push_back(space_t{vec2{col, row}, cell});
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

    auto hallway_positions_test = find_hallway_positions(
      situation.amphipods[4], situation.spaces, situation.hallway);

    for (const auto possible_position : hallway_positions) {
      // calculate cost to next position
      auto prev_position = std::find_if(
        situation.spaces.begin(), situation.spaces.end(),
        [amphipod](const auto& space) { return space.pos == amphipod.pos; });
      if (prev_position != situation.spaces.end()) {
        prev_position->occupant = '.';
      }
      amphipod.pos = possible_position.position;
      auto next_position = std::find_if(
        situation.spaces.begin(), situation.spaces.end(),
        [possible_position](const auto& space) {
          return space.pos == possible_position.position;
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
