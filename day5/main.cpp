#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

template<typename T>
int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}

template<class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct coord_t
{
  int x;
  int y;
};

bool operator==(const coord_t& lhs, const coord_t& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

struct line_segment_t
{
  coord_t begin;
  coord_t end;
};

struct coord_hash_t
{
  std::size_t operator()(const coord_t& coord) const
  {
    std::size_t seed = 0;
    hash_combine(seed, coord.x);
    hash_combine(seed, coord.y);
    return seed;
  }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> line_segment_strs;
  for (std::string line; std::getline(reader, line);) {
    line_segment_strs.push_back(line);
  }

  // check
  // for (const auto& line_segment_str : line_segment_strs) {
  //   std::cout << line_segment_str << '\n';
  // }

  std::vector<line_segment_t> line_segments;

  std::string ignore;
  for (const auto& line_segment_str : line_segment_strs) {
    int begin_x, begin_y, end_x, end_y;

    auto sep1 = line_segment_str.find_first_of(' ');
    auto begin_str = line_segment_str.substr(0, sep1);
    auto sep2 = line_segment_str.find_last_of(' ');
    auto end_str = line_segment_str.substr(sep2, line_segment_str.size());

    {
      std::istringstream iss(begin_str);
      std::string elem;
      std::getline(iss, elem, ',');
      begin_x = std::stoi(elem);
      std::getline(iss, elem, ' ');
      begin_y = std::stoi(elem);
    }

    {
      std::istringstream iss(end_str);
      std::string elem;
      std::getline(iss, elem, ',');
      end_x = std::stoi(elem);
      std::getline(iss, elem, ' ');
      end_y = std::stoi(elem);
    }

    coord_t begin_coord{begin_x, begin_y};
    coord_t end_coord{end_x, end_y};

    line_segment_t line_segment{begin_coord, end_coord};

    line_segments.push_back(line_segment);

    // check
    // std::cout << "begin " << begin_x << ' ' << begin_y << '\n';
    // std::cout << "end   " << end_x << ' ' << end_y << '\n';
  }

  {
    std::unordered_map<coord_t, int, coord_hash_t> visited_coords;
    for (const auto& line_segment : line_segments) {
      if (line_segment.begin.x == line_segment.end.x) {
        const auto delta = line_segment.end.y - line_segment.begin.y;
        for (int i = 0; i <= std::abs(delta); ++i) {
          visited_coords[coord_t{
            line_segment.begin.x, line_segment.begin.y + (i * sgn(delta))}]++;
        }
      }

      if (line_segment.begin.y == line_segment.end.y) {
        const auto delta = line_segment.end.x - line_segment.begin.x;
        for (int i = 0; i <= std::abs(delta); ++i) {
          visited_coords[coord_t{
            line_segment.begin.x + (i * sgn(delta)), line_segment.begin.y}]++;
        }
      }
    }

    const auto overlap_count = std::count_if(
      visited_coords.begin(), visited_coords.end(),
      [](const auto& visited_coord) { return visited_coord.second > 1; });

    std::cout << "part 1: " << overlap_count << '\n';
  }

  {
    std::unordered_map<coord_t, int, coord_hash_t> visited_coords;
    for (const auto& line_segment : line_segments) {
        const auto delta_x = line_segment.end.x - line_segment.begin.x;
        const auto delta_y = line_segment.end.y - line_segment.begin.y;
        for (int i = 0; i <= std::max(std::abs(delta_y), std::abs(delta_x)); ++i) {
          visited_coords[coord_t{
            line_segment.begin.x + (i * sgn(delta_x)),
            line_segment.begin.y + (i * sgn(delta_y))}]++;
        }
    }

    const auto overlap_count = std::count_if(
      visited_coords.begin(), visited_coords.end(),
      [](const auto& visited_coord) { return visited_coord.second > 1; });

    std::cout << "part 2: " << overlap_count << '\n';
  }
}
