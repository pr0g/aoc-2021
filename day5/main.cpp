#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

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

  struct coord_t
  {
    int x;
    int y;
  };

  struct line_segment_t
  {
    coord_t begin;
    coord_t end;
  };

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

    std::cout << "begin " << begin_x << ' ' << begin_y << '\n';
    std::cout << "end   " << end_x << ' ' << end_y << '\n';
  }
}
