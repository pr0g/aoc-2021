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

template<>
struct std::hash<vec2>
{
  std::size_t operator()(const vec2& v2) const
  {
    std::size_t seed = 0;
    hash_combine(seed, v2.x);
    hash_combine(seed, v2.y);
    return seed;
  }
};

int manhattan_distance(vec2 a, vec2 b)
{
  return std::abs(b.x - a.x) + std::abs(b.y - a.y);
}

struct amphipod_t
{
  vec2 pos;
  char type;
  int home;

  [[nodiscard]] bool in_room() const
  {
    return pos.y == 2 || pos.y == 3 || pos.y == 4 || pos.y == 5;
  }
  [[nodiscard]] bool in_home_room() const { return pos.x == home; }
  [[nodiscard]] bool in_top_home_room() const
  {
    return in_home_room() && (pos.y == 2 || pos.y == 3 || pos.y == 4);
  }
  [[nodiscard]] bool in_bottom_home_room() const
  {
    return in_home_room() && pos.y == 5;
  }
  [[nodiscard]] bool in_hallway() const { return pos.y == 1; }
};

bool operator==(const amphipod_t& lhs, const amphipod_t& rhs)
{
  return lhs.pos == rhs.pos && lhs.type == rhs.type && lhs.home == rhs.home;
}

template<>
struct std::hash<amphipod_t>
{
  std::size_t operator()(const amphipod_t& amphipod) const
  {
    std::size_t seed = 0;
    hash_combine(seed, amphipod.pos);
    hash_combine(seed, amphipod.type);
    hash_combine(seed, amphipod.home);
    return seed;
  }
};

template<>
struct std::hash<std::vector<amphipod_t>>
{
  std::size_t operator()(const std::vector<amphipod_t>& amphipods) const
  {
    std::size_t seed = 0;
    for (const auto amphipod : amphipods) {
      std::hash<amphipod_t> hasher;
      seed += hasher(amphipod);
    }
    return seed;
  }
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

  int path_cost = 0;

  [[nodiscard]] bool can_enter(const amphipod_t& amphipod, vec2 room) const
  {
    bool matching = amphipod.home == room.x;
    auto top_it =
      std::find_if(spaces.begin(), spaces.end(), [room](const space_t space) {
        return space.pos == vec2{room.x, 2};
      });
    auto top_mid_it =
      std::find_if(spaces.begin(), spaces.end(), [room](const space_t space) {
        return space.pos == vec2{room.x, 3};
      });
    auto bottom_mid_it =
      std::find_if(spaces.begin(), spaces.end(), [room](const space_t space) {
        return space.pos == vec2{room.x, 4};
      });
    auto bottom_it =
      std::find_if(spaces.begin(), spaces.end(), [room](const space_t space) {
        return space.pos == vec2{room.x, 5};
      });
    return matching
        && ((top_it->occupant == '.' && top_mid_it->occupant == '.'
             && bottom_mid_it->occupant == '.' && bottom_it->occupant == '.'
             && room.y == 5)
            || (top_it->occupant == '.' && top_mid_it->occupant == '.'
             && bottom_mid_it->occupant == '.' && bottom_it->occupant == amphipod.type && room.y == 4)
            || (top_it->occupant == '.' && top_mid_it->occupant == '.'
             && bottom_mid_it->occupant == amphipod.type && bottom_it->occupant == amphipod.type && room.y == 3)
            || (top_it->occupant == '.' && top_mid_it->occupant == amphipod.type
             && bottom_mid_it->occupant == amphipod.type && bottom_it->occupant == amphipod.type && room.y == 2));
  }

  [[nodiscard]] bool amphipod_settled(const amphipod_t& amphipod) const
  {
    if (amphipod.in_top_home_room()) {
      bool all_same = true;
      for (int y = amphipod.pos.y, off = 1; y < 5; ++y, ++off) {
        auto below = amphipod.pos + vec2{0, off};
        auto space = std::find_if(
          spaces.begin(), spaces.end(),
          [below](const space_t space) { return space.pos == below; });
        all_same &= space->occupant == amphipod.type;
      }
      return all_same;
    }

    return amphipod.in_bottom_home_room();
  }

  [[nodiscard]] bool goal_reached() const
  {
    bool all_home = true;
    for (const auto amphipod : amphipods) {
      all_home = amphipod.in_home_room() && all_home;
    }
    return all_home;
  }
};

bool operator==(const situation_t& lhs, const situation_t& rhs)
{
  return lhs.amphipods == rhs.amphipods && lhs.path_cost == rhs.path_cost;
}

struct situation_hash_t
{
  std::size_t operator()(const situation_t& situation) const
  {
    std::size_t seed = 0;
    hash_combine(seed, situation.path_cost);
    hash_combine(seed, situation.amphipods);
    return seed;
  }
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

bool check_blocked(
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

std::vector<node_t> find_positions(
  amphipod_t amphipod, const std::vector<space_t>& all_spaces,
  const std::vector<vec2>& positions)
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
          blocked_1 |= check_blocked(amphipod.pos, {x, y}, delta, all_spaces);
        }
        for (; x < std::abs(delta.x); ++x) {
          blocked_1 |= check_blocked(amphipod.pos, {x, y}, delta, all_spaces);
        }
      }
      {
        int y = 0;
        int x = 0;
        for (; x < std::abs(delta.x); ++x) {
          blocked_2 |= check_blocked(amphipod.pos, {x, y}, delta, all_spaces);
        }
        for (; y < std::abs(delta.y); ++y) {
          blocked_2 |= check_blocked(amphipod.pos, {x, y}, delta, all_spaces);
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
          auto next_it_pos = std::find_if(
            positions.begin(), positions.end(),
            [next](const auto hallway_pos) { return next == hallway_pos; });
          if (next_it_pos != positions.end()) {
            auto hallway_exists = std::find_if(
              hallway_costs.begin(), hallway_costs.end(),
              [next_it_pos](const auto hallway_cost) {
                return *next_it_pos == hallway_cost.position;
              });
            if (hallway_exists == hallway_costs.end()) {
              hallway_costs.push_back(
                {*next_it_pos,
                 manhattan_distance(amphipod.pos, *next_it_pos) * move_cost});
            }
          }
        }
      }
    }
  }
  return hallway_costs;
}

std::vector<node_t> find_room_positions(
  amphipod_t amphipod, const std::vector<space_t>& all_spaces,
  const std::vector<vec2>& rooms, const situation_t& situation)
{
  auto possible_positions = find_positions(amphipod, all_spaces, rooms);
  possible_positions.erase(
    std::remove_if(
      possible_positions.begin(), possible_positions.end(),
      [amphipod, &situation](const node_t node) {
        return node.position.x != amphipod.home
            || !situation.can_enter(amphipod, node.position);
      }),
    possible_positions.end());
  return possible_positions;
}

std::vector<node_t> find_hallway_positions(
  amphipod_t amphipod, const std::vector<space_t>& all_spaces,
  const std::vector<vec2>& hallway)
{
  return find_positions(amphipod, all_spaces, hallway);
}

void update_situation(
  situation_t& situation, amphipod_t& amphipod, const node_t& node)
{
  auto prev_position = std::find_if(
    situation.spaces.begin(), situation.spaces.end(),
    [amphipod](const auto& space) { return space.pos == amphipod.pos; });
  if (prev_position != situation.spaces.end()) {
    prev_position->occupant = '.';
  }
  amphipod.pos = node.position;
  auto next_position = std::find_if(
    situation.spaces.begin(), situation.spaces.end(),
    [node](const auto& space) { return space.pos == node.position; });
  if (next_position != situation.spaces.end()) {
    next_position->occupant = amphipod.type;
  }
}

int g_best_cost = INT_MAX;

void correct_arrangement(
  situation_t situation,
  std::unordered_map<situation_t, int, situation_hash_t>& seen_situations)
{
  if (situation.path_cost >= g_best_cost) {
    return;
  }

  if (situation.goal_reached()) {
    if (situation.path_cost < g_best_cost) {
      g_best_cost = situation.path_cost;
    }
  }

  for (int amphipod_index = 0; amphipod_index < situation.amphipods.size();
       ++amphipod_index) {
    auto& amphipod = situation.amphipods[amphipod_index];
    std::vector<node_t> moves;
    if (!amphipod.in_hallway() && !situation.amphipod_settled(amphipod)) {
      moves =
        find_hallway_positions(amphipod, situation.spaces, situation.hallway);
    } else if (amphipod.in_hallway()) {
      moves = find_room_positions(
        amphipod, situation.spaces, situation.rooms, situation);
    }
    for (const auto move : moves) {
      auto next_situation = situation;
      next_situation.path_cost += move.cost;
      auto& next_amphipod = next_situation.amphipods[amphipod_index];
      update_situation(next_situation, next_amphipod, move);
      auto f = seen_situations.find(next_situation);
      if (f == seen_situations.end()) {
        correct_arrangement(next_situation, seen_situations);
        seen_situations.insert({next_situation, next_situation.path_cost});
      }
    }
  }
}

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

  situation_t situation;
  for (int row = 0; row < lines.size(); ++row) {
    for (int col = 0; col < lines[row].size(); ++col) {
      const auto cell = lines[row][col];
      if (cell == '.' && col != 3 && col != 5 && col != 7 & col != 9) {
        situation.spaces.push_back(space_t{vec2{col, row}, cell});
        situation.hallway.push_back(vec2{col, row});
      } else if (
        cell == '.' && (col == 3 || col == 5 || col == 7 || col == 9)) {
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

  std::unordered_map<situation_t, int, situation_hash_t> seen_situations;
  correct_arrangement(situation, seen_situations);

  std::cout << "part 2: " << g_best_cost << '\n';

  return 0;
}
