#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

int main(int argc, char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> commands;
  for (std::string line; std::getline(reader, line);) {
    commands.push_back(line);
  }

  {
    int depth = 0, horizontal = 0;
    for (const auto& command : commands) {
      std::string direction;
      int distance;
      std::istringstream command_stream(command);
      command_stream >> direction >> distance;
      if (direction == "forward") {
        horizontal += distance;
      } else if (direction == "up") {
        depth -= distance;
      } else if (direction == "down") {
        depth += distance;
      }
    }

    std::cout << "part1: " << depth * horizontal << '\n';
  }

  {
    int aim = 0;
    int depth = 0, horizontal = 0;
    for (const auto& command : commands) {
      std::string direction;
      int distance;
      std::istringstream command_stream(command);
      command_stream >> direction >> distance;
      if (direction == "forward") {
        horizontal += distance;
        depth += aim * distance;
      } else if (direction == "up") {
        aim -= distance;
      } else if (direction == "down") {
        aim += distance;
      }
    }

    std::cout << "part2: " << depth * horizontal << '\n';
  }
}
