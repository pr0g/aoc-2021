#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<int> lanternfish_school;
  for (std::string line; std::getline(reader, line, ',');) {
    lanternfish_school.push_back(std::stoi(line));
  }

  // check
  // for (const auto lanternfish : lanternfish_school) {
  //   std::cout << lanternfish << ", ";
  // }
  // std::cout << '\n';

  const int days = 80;
  for (int day = 0; day < days; ++day) {
    const int lantern_fish_size = lanternfish_school.size();
    for (int fish = 0; fish < lantern_fish_size; ++fish) {
      if (lanternfish_school[fish] == 0) {
        lanternfish_school.push_back(8);
        lanternfish_school[fish] = 7;
      }
      lanternfish_school[fish]--;
    }
  }

  std::cout << "part 1: " << lanternfish_school.size() << '\n';

  return 0;
}
